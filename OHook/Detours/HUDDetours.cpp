#include "HUDDetours.h"
#include "Configuration.h"
#include "PaliaOverlay.h"
#include "SDKExt.h"
#include "Utils.h"

#include <algorithm>
#include "functional"
#include <format>

#include <SDK/Palia_parameters.hpp>

using namespace SDK;

void DrawCircle(UCanvas* Canvas, const float Radius, const int32 NumSegments, const FLinearColor Color, const float Thickness = 1.0f) {
    // Calculate screen center more accurately
    const FVector2D ScreenCenter = { static_cast<double>(Canvas->ClipX) / 2.0, static_cast<double>(Canvas->ClipY) / 2.0 };

    const double Increment = 360.0 / static_cast<double>(NumSegments);
    FVector2D LastPos = { ScreenCenter.X + Radius, ScreenCenter.Y };

    for (int i = 1; i <= NumSegments; i++) {
        const float Rad = CustomMath::DegreesToRadians(static_cast<float>(Increment * i));
        FVector2D NewPos = { ScreenCenter.X + Radius * cos(Rad), ScreenCenter.Y + Radius * sin(Rad) };
        Canvas->K2_DrawLine(LastPos, NewPos, Thickness, Color);
        LastPos = NewPos;
    }
}

void HUDDetours::Func_DoFOVCircle(const PaliaOverlay* Overlay, const AHUD* HUD) {
    if (!Configuration::bDrawFOVCircle)
        return;

    const auto PlayerController = GetPlayerController();
    if (!PlayerController)
        return;

    FVector PawnLocation = PlayerController->K2_GetPawn()->K2_GetActorLocation();
    FVector2D PlayerScreenPosition;
    FVector2D TargetScreenPosition;

    if (PlayerController->ProjectWorldLocationToScreen(PawnLocation, &PlayerScreenPosition, true)) {
        // Calculate the center of the FOV circle based on the player's screen position
        FVector2D FOVCenter = { HUD->Canvas->ClipX * 0.5f, HUD->Canvas->ClipY * 0.5f };
        DrawCircle(HUD->Canvas, Configuration::FOVRadius, 1200, { 0.485f, 0.485f, 0.485f, 0.485f }, 1.0f);

        if (Overlay->BestTargetLocation.IsZero())
            return;
        if (!PlayerController->ProjectWorldLocationToScreen(Overlay->BestTargetLocation, &TargetScreenPosition, true))
            return;
        if (!(CustomMath::DistanceBetweenPoints(TargetScreenPosition, FOVCenter) <= Configuration::FOVRadius))
            return;

        HUD->Canvas->K2_DrawLine(FOVCenter, TargetScreenPosition, 0.5f, { 0.485f, 0.485f, 0.485f, 0.485f });
    }
}

void HUDDetours::Func_DoESP(PaliaOverlay* Overlay, const AHUD* HUD) {
    if (!Configuration::bEnableESP) {
        Overlay->CachedActors.clear();
        return;
    }

    // Manage Cache Logic
    DetourManager::ClearActorCache(Overlay);
    DetourManager::ManageActorCache(Overlay);

    const auto PlayerController = GetPlayerController();
    if (!PlayerController)
        return;

    const auto ValeriaCharacter = GetValeriaCharacter();
    if (!ValeriaCharacter)
        return;
    
    FVector PawnLocation = PlayerController->K2_GetPawn()->K2_GetActorLocation();

    // Calculate distance - actor
    for (auto& Actor : Overlay->CachedActors) {
        if (!Actor.Actor || !IsActorValid(Actor.Actor))
            continue;

        FVector ActorLocation = Actor.WorldPosition.IsZero() ? Actor.Actor->K2_GetActorLocation() : Actor.WorldPosition;
        Actor.Distance = PawnLocation.GetDistanceTo(ActorLocation);
    }

    // Sort actors based on distance - prioritizing BestTargetActor
    std::ranges::sort(Overlay->CachedActors, [&](const auto& a, const auto& b) {
        if (a.Actor == Overlay->BestTargetActor)
            return false;
        if (b.Actor == Overlay->BestTargetActor)
            return true;

        return a.Distance > b.Distance;
    });
    
    // Draw ESP Names Entities
    for (auto& [Actor, WorldPosition, DisplayName, ActorType, Type, Quality, Variant, Distance, Texture] : Overlay->CachedActors) {
        if (!Actor || !IsActorValid(Actor) || WorldPosition.IsZero())
            continue;

        if (ActorType == EType::Players) {
            if (Actor == ValeriaCharacter)
                continue;
        }

        FVector ActorLocation = WorldPosition;
        if (ActorLocation = Actor->K2_GetActorLocation(); ActorLocation.IsZero())
            continue;

        // Adjust Z coordinate for head-level display
        float HeightAdjustment = 100.0f; // Adjust this value based on typical actor height
        ActorLocation.Z += HeightAdjustment;

        Distance = PawnLocation.GetDistanceToInMeters(ActorLocation);

        if (Configuration::bEnableESPCulling && Distance > Configuration::CullDistance)
            continue;

        FVector2D ScreenLocation;
        if (PlayerController->ProjectWorldLocationToScreen(ActorLocation, &ScreenLocation, true)) {
            ImU32 Color = IM_COL32(0xFF, 0xFF, 0xFF, 0xFF);
            bool bShouldDraw = false;

            bool bShouldDrawStar = false;
            bool bIconShouldUseColor = false;
            bool bOverlayText = false;
            std::string OverlayText = "";

            switch (ActorType) {
            case EType::Forage:
                if (Overlay->Forageables[Type][Quality]) {
                    bShouldDraw = true;
                    Color = Overlay->ForageableColors[Type];
                    if (Quality == 1) {
                        bShouldDrawStar = true;
                    }
                }
                break;
            case EType::Ore:
                if (Overlay->Ores[Type][Variant]) {
                    auto Ore = static_cast<ABP_ValeriaGatherableLoot_C*>(Actor);
                    if (Ore && IsActorValid(Ore)) {
                        if (Ore->IAmAlive) {
                            bShouldDraw = true;
                            Color = Overlay->OreColors[Type];
                            bOverlayText = true;
                            OverlayText = GatherableSizeNames[Variant];
                        }
                    }
                }
                break;
            case EType::Players:
                if (Overlay->Singles[static_cast<int>(EOneOffs::Player)]) {
                    bShouldDraw = true;
                    Color = Overlay->SingleColors[static_cast<int>(EOneOffs::Player)];
                }
                break;
            case EType::Animal:
                if (Overlay->Animals[Type][Variant]) {
                    bShouldDraw = true;
                    Color = Overlay->AnimalColors[Type][Variant];
                }
                break;
            case EType::Tree:
                if (Overlay->Trees[Type][Variant]) {
                    bShouldDraw = true;
                    Color = Overlay->TreeColors[Type];
                    bOverlayText = true;
                    OverlayText = GatherableSizeNames[Variant];
                }
                break;
            case EType::Bug:
                if (Overlay->Bugs[Type][Variant][Quality]) {
                    bShouldDraw = true;
                    Color = Overlay->BugColors[Type][Variant];
                    if (Quality == 1) {
                        bShouldDrawStar = true;
                    }
                }
                break;
            case EType::NPCs:
                if (Overlay->Singles[static_cast<int>(EOneOffs::NPC)]) {
                    bShouldDraw = true;
                    Color = Overlay->SingleColors[static_cast<int>(EOneOffs::NPC)];
                }
                break;
            case EType::Loot:
                if (Overlay->Singles[static_cast<int>(EOneOffs::Loot)]) {
                    bShouldDraw = true;
                    Color = Overlay->SingleColors[static_cast<int>(EOneOffs::Loot)];
                    bIconShouldUseColor = true;
                }
                break;
            case EType::Quest:
                if (Overlay->Singles[static_cast<int>(EOneOffs::Quest)]) {
                    bShouldDraw = true;
                    Color = Overlay->SingleColors[static_cast<int>(EOneOffs::Quest)];
                }
                break;
            case EType::RummagePiles:
                if (Overlay->Singles[static_cast<int>(EOneOffs::RummagePiles)]) {
                    auto Pile = static_cast<ATimedLootPile*>(Actor);
                    if (!Pile || !IsActorValid(Pile)) {
                        bShouldDraw = false;
                        break;
                    }

                    if (Configuration::bEnableOthers) {
                        bShouldDraw = true;
                        Color = Pile->bActivated ? IM_COL32(0xFF, 0xFF, 0xFF, 0xFF) : IM_COL32(0xFF, 0x00, 0x00, 0xFF);
                    }
                    else if (Pile->CanGather(ValeriaCharacter) && Pile->bActivated) {
                        bShouldDraw = true;
                        Color = Overlay->SingleColors[static_cast<int>(EOneOffs::RummagePiles)];
                    }
                }
                break;
            case EType::Stables:
                if (Overlay->Singles[static_cast<int>(EOneOffs::Stables)]) {
                    bShouldDraw = true;
                    Color = Overlay->SingleColors[static_cast<int>(EOneOffs::Stables)];
                }
                break;
            case EType::Fish:
                if (Overlay->Fish[Type]) {
                    bShouldDraw = true;
                    Color = Overlay->FishColors[Type];
                }
                break;
            case EType::Treasure:
                if (Overlay->Singles[static_cast<int>(EOneOffs::Treasure)]) {
                    auto Treasure = static_cast<AWorldGatherableBase*>(Actor);
                    if (!Treasure || !IsActorValid(Treasure)) {
                        break;
                    }
                    if (Treasure->CanGather(ValeriaCharacter)) {
                        bShouldDraw = true;
                        Color = Overlay->SingleColors[static_cast<int>(EOneOffs::Treasure)];
                    }
                }
            default:
                break;
            }

            if (Configuration::bEnableOthers && Type == 0)
                bShouldDraw = true;

            if (!bShouldDraw)
                continue;

            if (!Roboto) {
                Roboto = reinterpret_cast<UFont*>(UObject::FindObject("Font Roboto.Roboto", EClassCastFlags::None));

                if (!Roboto)
                    continue;
            }

            // Construct text string
            std::wstring qualityName = (Quality > 0) ? PaliaOverlay::GetQualityName(Quality, ActorType) : L"";

            std::vector<std::wstring> IconText{};
            std::wstring IconAboveText;
            // Prepare text with optional parts depending on the index values
            std::wstring text = DisplayName;
            if (!qualityName.empty()) {
                text += L" " + qualityName;
            }
            text += std::format(L" [{:.0f}m]", Distance);
            
            /*if (Configuration::bESPIconDistance) {
                if (ActorType == EType::Players) {
                    IconAboveText = text;
                }
                else {
                    IconText.push_back(std::format(L"{:.0f}m", Distance));
                }                
            } else if (ActorType == EType::Players) {
                IconAboveText = DisplayName;
            }*/

            if (Configuration::bEnableDespawnTimer) {
                double seconds = 0;

                if (IsActorValid(Actor)) {
                    if (ActorType == EType::Ore) {
                        auto GatherableLoot = static_cast<ABP_ValeriaGatherableLoot_Mining_MultiHarvest_C*>(Actor);
                        if (GatherableLoot && IsActorValid(GatherableLoot)) {
                            if (GatherableLoot->GatherableLoot && GatherableLoot->GatherableLoot->IsValidLowLevel()) {
                                GatherableLoot->GetSecondsUntilDespawn(&seconds);
                            }
                        }
                    }
                    else if (ActorType == EType::Forage) {
                        auto ForageableLoot = static_cast<ABP_ValeriaGatherable_C*>(Actor);
                        if (ForageableLoot && IsActorValid(ForageableLoot)) {
                            if (ForageableLoot->Gatherable && ForageableLoot->Gatherable->IsValidLowLevel()) {
                                seconds = ForageableLoot->Gatherable->GetSecondsUntilDespawn();
                            }
                        }
                    }
                }

                if (seconds > 0) {
                    text += std::format(L" {:.0f}s", seconds);
                    IconText.push_back(std::format(L"{:.0f}s", seconds));
                }
            }

            // ESP TEXT INFO
            double BaseScale = 1.0; // Default scale at a reference distance
            double ReferenceDistance = 100.0; // Distance at which no scaling is applied
            double ScalingFactor = 0; // Determines how much the scale changes with distance
            double DistanceScale;
            DistanceScale = BaseScale - ScalingFactor * (Distance - ReferenceDistance);
            DistanceScale = CustomMath::Clamp(DistanceScale, 0.5, BaseScale); // Clamp the scale to a reasonable range

            const FVector2D TextScale = { DistanceScale * Configuration::ESPTextScale, DistanceScale * Configuration::ESPTextScale };
            ImColor IMC(Color);
            FLinearColor TextColor = { IMC.Value.x, IMC.Value.y, IMC.Value.z, IMC.Value.w };

            // Setup shadow properties
            ImColor IMCS(Color);
            FLinearColor ShadowColor = { IMCS.Value.x, IMCS.Value.y, IMCS.Value.z, IMCS.Value.w };

            // Calculate positions
            FVector2D TextPosition = ScreenLocation;
            FVector2D ShadowPosition = { TextPosition.X + 1.0, TextPosition.Y + 1.0 };

            /*UTexture2D* ActorTexture = nullptr;
            if (!Texture.empty()) {
                ActorTexture = static_cast<UTexture2D*>(LoadAssetPtrFromPath(Texture));
            }
            if (ActorTexture && Configuration::bESPIcons) {
                float ShadowXY = 36;
                float IconXY = 32;
                FVector2D TextureSize = { ActorTexture->Blueprint_GetSizeX() , ActorTexture->Blueprint_GetSizeY() };
                float ShadowScale = max(TextureSize.X, TextureSize.Y) / ShadowXY;
                FVector2D ShadowSize = { TextureSize.X/ShadowScale,TextureSize.Y / ShadowScale };
                FVector2D ShadowLocation = { ScreenLocation.X - ShadowSize.X / 2, ScreenLocation.Y - ShadowSize.Y / 2 };
                // shadow texture
                HUD->Canvas->K2_DrawTexture(ActorTexture, ShadowLocation, ShadowSize, { 0,0 }, { 1, 1 }, { 1,1,1,1 }, EBlendMode::BLEND_AlphaHoldout, 0.0F, { 0.5,0.5 });
                // normal texture
                float IconScale = max(TextureSize.X, TextureSize.Y) / IconXY;
                FVector2D IconSize = { TextureSize.X / IconScale,TextureSize.Y / IconScale };
                FVector2D IconLocation = { ScreenLocation.X - IconSize.X / 2, ScreenLocation.Y - IconSize.Y / 2 };
                FLinearColor IconColor = { 1,1,1,1 };
                if (bIconShouldUseColor) {
                    IconColor = TextColor;
                }
                HUD->Canvas->K2_DrawTexture(ActorTexture, IconLocation, IconSize, { 0,0 }, { 1, 1 }, IconColor, EBlendMode::BLEND_Translucent, 0.0F, { 0.5,0.5 });
                if (bShouldDrawStar) {
                    if (auto StarTexture = static_cast<UTexture2D*>(LoadAssetPtrFromPath("/Game/UI/Assets_Shared/Icons/Icon_Special_01.Icon_Special_01")); StarTexture != nullptr) {
                        FVector2D StarSize = { 16,16 };
                        FVector2D StarLocation = { ScreenLocation.X - IconSize.X / 2 - StarSize.X / 2, ScreenLocation.Y + IconSize.Y / 2 - StarSize.Y / 2 };
                        HUD->Canvas->K2_DrawTexture(StarTexture, StarLocation, StarSize, { 0,0 }, { 1, 1 }, { 1,0.9,0,1 }, EBlendMode::BLEND_Translucent, 0.0F, { 0.5,0.5 });
                    }
                }
                if (bOverlayText && !OverlayText.empty()) {
                    std::wstring OverlayWText(OverlayText.begin(), OverlayText.end());
                    FVector2D OverlayPosition = ScreenLocation; //{ ScreenLocation.X - IconSize.X / 2, ScreenLocation.Y + IconSize.Y / 2 };
                    HUD->Canvas->K2_DrawText(Roboto, FString(OverlayWText.data()), OverlayPosition, { 0.825,0.825 }, TextColor, 0, {1,1,1,1}, {1.0f, 1.0f}, true, true, true, {0, 0, 0, 1});
                }
                FVector2D Offset = { 0.0, IconSize.Y / 2 };
                if (IconText.size() > 0) {
                    for (int i = 0; i < IconText.size(); i++) {
                        auto line = IconText[i];
                        FString DisplayString = FString(line.data());
                        auto textSize = HUD->Canvas->K2_TextSize(Roboto, DisplayString, TextScale);
                        HUD->Canvas->K2_DrawText(Roboto, DisplayString, { ScreenLocation.X, ScreenLocation.Y + Offset.Y }, TextScale, TextColor, 0, { 0,0,0,1 }, { 1,1 }, true, false, true, { 0,0,0,1 });
                        Offset.Y += textSize.Y;
                    }
                }
                if (!IconAboveText.empty()) {
                    FString AboveString = FString(IconAboveText.data());
                    auto textSize = HUD->Canvas->K2_TextSize(Roboto, AboveString, TextScale);
                    HUD->Canvas->K2_DrawText(Roboto, AboveString, { ScreenLocation.X, ScreenLocation.Y - IconSize.Y / 2 - textSize.Y}, TextScale, TextColor, 0, { 0,0,0,1 }, { 1,1 }, true, false, true, { 0,0,0,1 });
                }
            }*/
            /*else {
                // Draw shadow, and main text
                HUD->Canvas->K2_DrawText(Roboto, FString(text.data()), ShadowPosition, TextScale, TextColor, 0, { 0, 0, 0, 1 }, { 1.0f, 1.0f }, true, true, true, { 0, 0, 0, 1 }); //shadow
                HUD->Canvas->K2_DrawText(Roboto, FString(text.data()), TextPosition, TextScale, ShadowColor, 0, { 0, 0, 0, 1 }, { 1.0f, 1.0f }, true, true, true, { 0, 0, 0, 1 }); //main
            }*/

            HUD->Canvas->K2_DrawText(Roboto, FString(text.data()), ShadowPosition, TextScale, TextColor, 0, { 0, 0, 0, 1 }, { 1.0f, 1.0f }, true, true, true, { 0, 0, 0, 1 }); //shadow
            HUD->Canvas->K2_DrawText(Roboto, FString(text.data()), TextPosition, TextScale, ShadowColor, 0, { 0, 0, 0, 1 }, { 1.0f, 1.0f }, true, true, true, { 0, 0, 0, 1 }); //main
        }
    }
}