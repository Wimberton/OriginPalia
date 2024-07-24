#include "HUDDetours.h"
#include "Configuration.h"
#include "PaliaOverlay.h"
#include "ActorHandler.h"
#include "Misc/SDKExt.h"
#include "Misc/Utils.h"

#include <algorithm>
#include "functional"
#include <format>

#include <SDK/Palia_parameters.hpp>

#include "AimDetours.h"

using namespace SDK;

void HUDDetours::Func_DoESP(const AHUD* HUD) {
    if (!Configuration::bEnableESP || !Configuration::bESPIcons) {
        return;
    }

    const auto ValeriaCharacter = GetValeriaCharacter();
    if (!ValeriaCharacter)
        return;
    
    FVector PawnLocation = ValeriaCharacter->K2_GetActorLocation();

    // Calculate distance - actor
    auto CachedActors = ActorHandler::GetActors();
    for (auto& Actor : CachedActors) {
        if (!Actor.Actor || !IsActorValid(Actor.Actor))
            continue;

        // only update location on movable pawns, otherwise use cached
        if (Actor.ActorType == EType::Animal ||
            Actor.ActorType == EType::Players ||
            Actor.ActorType == EType::Bug ||
            Actor.ActorType == EType::NPCs ||
            Actor.WorldPosition.IsZero()) 
        {
            Actor.WorldPosition = Actor.Actor->K2_GetActorLocation();
            if (Actor.WorldPosition.IsZero()) continue;
        }
        Actor.Distance = PawnLocation.GetDistanceToInMeters(Actor.WorldPosition);
    }

    FVector2D CanvasSize = {static_cast<float>(HUD->Canvas->SizeX), static_cast<float>(HUD->Canvas->SizeY)};

    // Draw ESP Names Entities
    for (auto& [Actor, WorldPosition, DisplayName, ActorType, Type, Quality, Variant, Distance, Texture] : CachedActors) {
        if (WorldPosition.IsZero() || !Actor || !IsActorValid(Actor)) {
            continue;
        }
        
        if (ActorType == EType::Players) {
            if (Actor == ValeriaCharacter)
                continue;
        }
        
        const auto PlayerController = GetPlayerController();
        if (!PlayerController)
            break;
        
        FVector ActorLocation = WorldPosition;
        
        // Adjust Z coordinate for head-level display
        float HeightAdjustment = 100.0f; // Adjust this value based on typical actor height
        ActorLocation.Z += HeightAdjustment;

        if (Configuration::bEnableESPCulling && Distance > Configuration::CullDistance)
            continue;

        ImU32 Color = IM_COL32(0xFF, 0xFF, 0xFF, 0xFF);
        bool bShouldDraw = false;

        bool bShouldDrawStar = false;
        bool bIconShouldUseColor = false;
        bool bOverlayText = false;
        std::string OverlayText;

        switch (ActorType) {
        case EType::Forage:
            if (Forageables[Type][Quality]) {
                if (!Actor->bActorEnableCollision) {
                    continue;
                }
                bShouldDraw = true;
                if (auto it = Configuration::ForageableColors.find(static_cast<EForageableType>(Type)); it != Configuration::ForageableColors.end())
                    Color = it->second;
                if (Quality == 1) {
                    bShouldDrawStar = true;
                }
            }
            break;
        case EType::Ore:
            if (Ores[Type][Variant]) {
                auto Ore = static_cast<ABP_ValeriaGatherableLoot_C*>(Actor);
                if (Ore && IsActorValid(Ore)) {
                    if (Ore->IAmAlive) {
                        bShouldDraw = true;
                        if (auto it = Configuration::OreColors.find(static_cast<EOreType>(Type)); it != Configuration::OreColors.end())
                            Color = it->second;
                        bOverlayText = true;
                        OverlayText = GatherableSizeNames[Variant];
                    }
                }
            }
            break;
        case EType::Players:
            if (Singles[static_cast<int>(EOneOffs::Player)]) {
				if (DisplayName.empty()) continue;
                bShouldDraw = true;
                if (auto it = Configuration::SingleColors.find(EOneOffs::Player); it != Configuration::SingleColors.end())
                    Color = it->second;
            }
            break;
        case EType::Animal:
            if (Animals[Type][Variant]) {
                bShouldDraw = true;
                if (auto it = Configuration::AnimalColors.find(FCreatureType(static_cast<ECreatureKind>(Type), static_cast<ECreatureQuality>(Variant))); it != Configuration::AnimalColors.end())
                    Color = it->second;
            }
            break;
        case EType::Tree:
            if (Trees[Type][Variant]) {
                auto Tree = static_cast<ABP_ValeriaGatherableLoot_C*>(Actor);
                if (Tree && IsActorValid(Tree)) {
                    if (Tree->IAmAlive) {
                        bShouldDraw = true;
                        if (auto it = Configuration::TreeColors.find(static_cast<ETreeType>(Type)); it != Configuration::TreeColors.end())
                            Color = it->second;
                        bOverlayText = true;
                        OverlayText = GatherableSizeNames[Variant];
                    }
                }
            }
            break;
        case EType::Bug:
            if (Bugs[Type][Variant][Quality]) {
                bShouldDraw = true;
                if (auto it = Configuration::BugColors.find(FBugType(static_cast<EBugKind>(Type), static_cast<EBugQuality>(Variant))); it != Configuration::BugColors.end())
                    Color = it->second;
                if (Quality == 1) {
                    bShouldDrawStar = true;
                }
            }
            break;
        case EType::NPCs:
            if (Singles[static_cast<int>(EOneOffs::NPC)]) {
                bShouldDraw = true;
                if (auto it = Configuration::SingleColors.find(EOneOffs::NPC); it != Configuration::SingleColors.end())
                    Color = it->second;
            }
            break;
        case EType::Loot:
            if (Singles[static_cast<int>(EOneOffs::Loot)]) {
                bShouldDraw = true;
                if (auto it = Configuration::SingleColors.find(EOneOffs::Loot); it != Configuration::SingleColors.end())
                    Color = it->second;
                bIconShouldUseColor = true;
            }
            break;
        case EType::Quest:
            if (Singles[static_cast<int>(EOneOffs::Quest)]) {
                if (Distance > 250) continue; // quests don't always update collision until within this range
                if (!Actor->bActorEnableCollision) {
                    continue;
                }
                bShouldDraw = true;
                if (auto it = Configuration::SingleColors.find(EOneOffs::Quest); it != Configuration::SingleColors.end())
                    Color = it->second;
            }
            break;
        case EType::RummagePiles:
            if (Singles[static_cast<int>(EOneOffs::RummagePiles)]) {
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
                    if (auto it = Configuration::SingleColors.find(EOneOffs::RummagePiles); it != Configuration::SingleColors.end())
                        Color = it->second;
                }
            }
            break;
        case EType::Stables:
            if (Singles[static_cast<int>(EOneOffs::Stables)]) {
                bShouldDraw = true;
                if (auto it = Configuration::SingleColors.find(EOneOffs::Stables); it != Configuration::SingleColors.end())
                    Color = it->second;
            }
            break;
        case EType::Fish:
            if (Fish[Type]) {
                bShouldDraw = true;
                if (auto it = Configuration::FishColors.find(static_cast<EFishType>(Type)); it != Configuration::FishColors.end())
                    Color = it->second;
            }
            break;
        case EType::Treasure:
            if (Singles[static_cast<int>(EOneOffs::Treasure)]) {
                if (Distance > 150) continue; // Chests don't seem to properly setup collision until in update range
                if (!Actor->bActorEnableCollision) {
                    continue;
                }
                
                auto Treasure = static_cast<AWorldPersistGatherable*>(Actor);
                if (!Treasure || !IsActorValid(Treasure)) break;
                
                if (Configuration::bEnableOthers) {
                    bShouldDraw = true;
                    if (auto it = Configuration::SingleColors.find(EOneOffs::Treasure); it != Configuration::SingleColors.end())
                        Color = it->second;
                } else if (Treasure->CanGather(ValeriaCharacter)) {
                    bShouldDraw = true;
                    if (auto it = Configuration::SingleColors.find(EOneOffs::Treasure); it != Configuration::SingleColors.end())
                        Color = it->second;
                }
            }
            break;
        case EType::TimedDrop:
            if (Singles[static_cast<int>(EOneOffs::TimedDrop)]) {
                bShouldDraw = true;
                if (auto it = Configuration::SingleColors.find(EOneOffs::TimedDrop); it != Configuration::SingleColors.end())
                    Color = it->second;
                if (Variant > 1) {
                    bOverlayText = true;
                    OverlayText = std::format("{}", Variant);
                }
            }
            break;
        default:
            break;
        }

        if (Configuration::bEnableOthers && Type == 0)
            bShouldDraw = true;

        if (!bShouldDraw)
            continue;

        FVector2D ScreenLocation;
        if (PlayerController->ProjectWorldLocationToScreen(ActorLocation, &ScreenLocation, true)) {
            if (ScreenLocation.X < 0 || ScreenLocation.Y < 0 || ScreenLocation.X > CanvasSize.X || ScreenLocation.Y > CanvasSize.Y) continue;
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
            std::wstring text;
            if (!qualityName.empty()) {
                text += L"" + qualityName;
            }
            text += std::format(L" [{:.0f}m]", Distance);

            if (Configuration::bESPIconDistance && ActorType != EType::Players) {
                IconText.push_back(std::format(L"{:.0f}m", Distance));
            }

            // ESP TEXT INFO
            double BaseScale = 1.0; // Default scale at a reference distance
            double ReferenceDistance = 100.0; // Distance at which no scaling is applied
            double ScalingFactor = 0; // Determines how much the scale changes with distance
            double DistanceScale;
            DistanceScale = BaseScale - ScalingFactor * (Distance - ReferenceDistance);
            DistanceScale = CustomMath::Clamp(DistanceScale, 0.5, BaseScale); // Clamp the scale to a reasonable range

            const FVector2D TextScale = { DistanceScale /** Configuration::ESPTextScale*/, DistanceScale /** Configuration::ESPTextScale*/ }; // ESPTextScale Disabled.

            FVector2D textSize = HUD->Canvas->K2_TextSize(Roboto, FString(text.data()), TextScale);
            textSize.X = textSize.X - 36.0f;

            if (ScreenLocation.X - textSize.X < 0.0f)
                ScreenLocation.X = textSize.X;
            else if (ScreenLocation.X + textSize.X > CanvasSize.X)
                ScreenLocation.X = CanvasSize.X - textSize.X;

            if (ScreenLocation.Y - textSize.Y < 0.0f)
                ScreenLocation.Y = textSize.Y;
            else if (ScreenLocation.Y + textSize.Y > CanvasSize.Y)
                ScreenLocation.Y = CanvasSize.Y - textSize.Y;

            ImColor IMC(Color);
            FLinearColor TextColor = { IMC.Value.x, IMC.Value.y, IMC.Value.z, IMC.Value.w };
            
            // Setup shadow properties
            ImColor IMCS(Color);
            FLinearColor ShadowColor = { IMCS.Value.x, IMCS.Value.y, IMCS.Value.z, IMCS.Value.w };
            
            if (!Texture.empty()) {
                UTexture2D* ActorTexture = nullptr;
                ActorTexture = ActorHandler::TextureLookup(Texture);
                if (!ActorTexture || !ActorTexture->IsValidLowLevel()) continue;
                float ShadowXY = 36;
                float IconXY = 32;
                FVector2D TextureSize = { static_cast<float>(ActorTexture->Blueprint_GetSizeX()), static_cast<float>(ActorTexture->Blueprint_GetSizeY()) };

                float ShadowScale = static_cast<float>(max(TextureSize.X, TextureSize.Y)) / ShadowXY;
                FVector2D ShadowSize = { TextureSize.X / ShadowScale,TextureSize.Y / ShadowScale };
                FVector2D ShadowLocation = { ScreenLocation.X - ShadowSize.X / 2, ScreenLocation.Y - ShadowSize.Y / 2 };

                if (ActorType == EType::Players)
                    ShadowLocation.Y -= 10.0f;

                // shadow texture
                HUD->Canvas->K2_DrawTexture(ActorTexture, ShadowLocation, ShadowSize, { 0,0 }, { 1, 1 }, { 1,1,1,1 }, EBlendMode::BLEND_AlphaHoldout, 0.0F, { 0.5,0.5 });
                // normal texture
                float IconScale = static_cast<float>(max(TextureSize.X, TextureSize.Y)) / IconXY;

                FVector2D IconSize = { TextureSize.X / IconScale,TextureSize.Y / IconScale };
                FVector2D IconLocation = { ScreenLocation.X - IconSize.X / 2, ScreenLocation.Y - IconSize.Y / 2 };

                // offset icon for players
                if (ActorType == EType::Players)
                    IconLocation.Y -= 10.0f;

                FLinearColor IconColor = { 1,1,1,1 };
                if (bIconShouldUseColor) {
                    IconColor = TextColor;
                }
                HUD->Canvas->K2_DrawTexture(ActorTexture, IconLocation, IconSize, { 0,0 }, { 1, 1 }, IconColor, EBlendMode::BLEND_Translucent, 0.0F, { 0.5,0.5 });
                if (bShouldDrawStar) {
                    if (auto StarTexture = static_cast<UTexture2D*>(LoadAssetPtrFromPath("/Game/UI/Assets_Shared/Icons/Icon_Special_01.Icon_Special_01")); StarTexture != nullptr) {
                        FVector2D StarSize = { 16,16 };
                        FVector2D StarLocation = { ScreenLocation.X - IconSize.X / 2 - StarSize.X / 2, ScreenLocation.Y + IconSize.Y / 2 - StarSize.Y / 2 };
                        HUD->Canvas->K2_DrawTexture(StarTexture, StarLocation, StarSize, { 0,0 }, { 1, 1 }, { 1,0.9f,0,1 }, EBlendMode::BLEND_Translucent, 0.0F, { 0.5,0.5 });
                    }
                }
                if (bOverlayText && !OverlayText.empty()) {
                    std::wstring OverlayWText(OverlayText.begin(), OverlayText.end());
                    FVector2D OverlayPosition = ScreenLocation; //{ ScreenLocation.X - IconSize.X / 2, ScreenLocation.Y + IconSize.Y / 2 };
                    HUD->Canvas->K2_DrawText(Roboto, FString(OverlayWText.data()), OverlayPosition, { 0.825,0.825 }, TextColor, 0, { 1,1,1,1 }, { 1.0f, 1.0f }, true, true, true, { 0, 0, 0, 1 });
                }
                FVector2D Offset = { 0.0, IconSize.Y / 2 };
                if (!IconText.empty()) {
                    for (auto& line : IconText) {
                        auto DisplayString = FString(line.data());
                        auto textSize = HUD->Canvas->K2_TextSize(Roboto, DisplayString, TextScale);
                        HUD->Canvas->K2_DrawText(Roboto, DisplayString, { ScreenLocation.X, ScreenLocation.Y + Offset.Y }, TextScale, TextColor, 0, { 0,0,0,1 }, { 1,1 }, true, false, true, { 0,0,0,1 });
                        Offset.Y += textSize.Y;
                    }
                }
                if (!IconAboveText.empty()) {
                    auto AboveString = FString(IconAboveText.data());
                    auto textSize = HUD->Canvas->K2_TextSize(Roboto, AboveString, TextScale);
                    HUD->Canvas->K2_DrawText(Roboto, AboveString, { ScreenLocation.X, ScreenLocation.Y - IconSize.Y / 2 - textSize.Y }, TextScale, TextColor, 0, { 0,0,0,1 }, { 1,1 }, true, false, true, { 0,0,0,1 });
                }
            }
        }
    }
}