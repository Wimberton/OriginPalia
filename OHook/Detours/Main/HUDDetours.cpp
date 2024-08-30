#include "Detours/Main/HUDDetours.h"
#include "Core/Configuration.h"
#include "Core/DetourManager.h"
#include "Core/Handlers/ActorHandler.h"
#include "Core/Handlers/WidgetHandler.h"
#include "Overlay/PaliaOverlay.h"
#include "Misc/SDKExt.h"
#include "Misc/Utils.h"

#include "functional"
#include <format>

#include <SDK/Palia_parameters.hpp>

using namespace SDK;

void HUDDetours::SetupDetours(AValeriaCharacter* VC, AValeriaPlayerController* VPC) {
    DetourManager::AddInstance(VPC->MyHUD);
    DetourManager::AddFunctionListener("Function Engine.HUD.ReceiveDrawHUD", &ReceiveDrawHUD, nullptr);
}

void HUDDetours::ReceiveDrawHUD(const UObject* Context, UFunction* Function, void* Parms) {
    auto HUD = static_cast<const AHUD*>(Context);
    if (!Configuration::bEnableESP || !Configuration::bEnableESPIcons) {
        return;
    }

    if (WidgetHandler::IsWidgetOpen()) return;

    const auto ValeriaCharacter = GetValeriaCharacter();
    if (!ValeriaCharacter)
        return;
    
    FVector2D CanvasSize = {static_cast<float>(HUD->Canvas->SizeX), static_cast<float>(HUD->Canvas->SizeY)};

    // Draw ESP Names Entities
    for (auto& [Actor, WorldPosition, DisplayName, ActorType, Type, Quality, Variant, Distance, Texture, Despawn] : ActorHandler::GetActors()) {
        if (WorldPosition.IsZero() || !IsActorValid(Actor)) {
            continue;
        }

        // skip hidden actors from other sub-plots
        if ((ActorHandler::CurrentMap & EValeriaLevel::Home) && !Actor->bActorEnableCollision) continue;
        
        if (ActorType == EType::Players) {
            if (Actor == ValeriaCharacter)
                continue;
        }
        
        const auto PlayerController = GetPlayerController();
        if (!PlayerController)
            break;
        
        FVector ActorLocation = WorldPosition;
        
        // Adjust Z coordinate for head-level display
        ActorLocation.Z += 100.0; // Adjust this value based on typical actor height

        if (Distance > Configuration::CullDistance)
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
                        // if (ActorHandler::IsActorA(Ore, L"/Game/Gameplay/Skills/Mining/BP_ValeriaGatherableLoot_Mining_MultiHarvest.BP_ValeriaGatherableLoot_Mining_MultiHarvest")) {
                        //     auto OreMulti = static_cast<ABP_ValeriaGatherableLoot_Mining_MultiHarvest_C*>(Ore);
                        //     if (OreMulti->DespawnTimestamp > 0  && OreMulti->ParticipationTracking) { // if it's not despawning we don't need to check if we've broken it
                        //         if (UBlueprintGameplayTagLibrary::IsGameplayTagValid(OreMulti->PreviousCycleParticipationTag)) {
                        //             bShouldDraw = !OreMulti->ParticipationTracking->IsParticipantForType(ValeriaCharacter, OreMulti->PreviousCycleParticipationTag);
                        //         }
                        //     }
                        // }
                        if (auto it = Configuration::OreColors.find(static_cast<EOreType>(Type)); it != Configuration::OreColors.end())
                            Color = it->second;
                        if (Type != static_cast<uint8_t>(EOreType::Clay)) {
                            bOverlayText = true;
                            OverlayText = GatherableSizeNames[Variant];
                        }
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
                        if (Type != static_cast<uint8_t>(ETreeType::Bush)) {
                            bOverlayText = true;
                            OverlayText = GatherableSizeNames[Variant];
                        }
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
                
                if (Singles[static_cast<int>(EOneOffs::Others)]) {
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
            if (Singles[static_cast<int>(EOneOffs::FishHook)]) {
                bShouldDraw = true;
                if (auto it = Configuration::SingleColors.find(EOneOffs::FishHook); it != Configuration::SingleColors.end())
                    Color = it->second;
                bIconShouldUseColor = true;
            }
            break;
        case EType::Pool:
            if (Singles[static_cast<int>(EOneOffs::FishPool)]) {
                bShouldDraw = true;
                if (auto it = Configuration::SingleColors.find(EOneOffs::FishPool); it != Configuration::SingleColors.end())
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
                
                if (Singles[static_cast<int>(EOneOffs::Others)]) {
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
        case EType::Relic:
            if (Singles[static_cast<int>(EOneOffs::Relic)]) {
                bShouldDraw = true;
                if (auto it = Configuration::SingleColors.find(EOneOffs::Relic); it != Configuration::SingleColors.end())
                    Color = it->second;
            }
            break;
        default:
            break;
        }

        if (Singles[static_cast<int>(EOneOffs::Others)] && Type == 0)
            bShouldDraw = true;

        if (!bShouldDraw)
            continue;

        FVector2D ScreenLocation;
        if (PlayerController->ProjectWorldLocationToScreen(ActorLocation, &ScreenLocation, true)) {
            if (ScreenLocation.X < 0.0 || ScreenLocation.Y < 0.0 || ScreenLocation.X > CanvasSize.X || ScreenLocation.Y > CanvasSize.Y) continue;
            if (!Roboto) {
                Roboto = reinterpret_cast<UFont*>(UObject::FindObject("Font Roboto.Roboto", EClassCastFlags::None));
                if (!Roboto)
                    continue;
            }

            // Construct text string
            std::vector<std::wstring> IconText{};
            std::wstring IconAboveText;

            if ((ActorType == EType::RummagePiles && Variant > 0) || ActorType == EType::Fish || ActorType == EType::Pool) {
                IconText.push_back(DisplayName);
            }

            if (Configuration::bEnableESPDistance) {
                if (ActorType == EType::Players) {
                    IconAboveText = std::format(L"{} [{:.0f}m]", DisplayName, Distance);;
                }
                else {
                    IconText.push_back(std::format(L"{:.0f}m", Distance));
                }
            }
            else if (ActorType == EType::Players) {
                IconAboveText = DisplayName;
            }
            if (Configuration::bEnableESPDespawnTimer && Despawn > 0) {
                IconText.push_back(std::format(L"{:.0f}s", Despawn));
            }

            if (ScreenLocation.X - 32.0 < 0.0)
                ScreenLocation.X = 32.0;
            else if (ScreenLocation.X + 32.0 > CanvasSize.X)
                ScreenLocation.X = CanvasSize.X - 32.0;

            if (ScreenLocation.Y - 32.0 < 0.0)
                ScreenLocation.Y = 32.0;
            else if (ScreenLocation.Y + 32.0 > CanvasSize.Y)
                ScreenLocation.Y = CanvasSize.Y - 32.0;

            // ESP TEXT INFO
            double BaseScale = 1.0; // Default scale at a reference distance
            double ReferenceDistance = 100.0; // Distance at which no scaling is applied
            double ScalingFactor = 0.0; // Determines how much the scale changes with distance
            double DistanceScale;
            DistanceScale = BaseScale - ScalingFactor * (Distance - ReferenceDistance);
            DistanceScale = CustomMath::Clamp(DistanceScale, 0.5, BaseScale); // Clamp the scale to a reasonable range

            const FVector2D TextScale = { DistanceScale * (double)Configuration::ESPTextScale, DistanceScale * (double)Configuration::ESPTextScale };

            ImColor IMC(Color);
            FLinearColor TextColor = { IMC.Value.x, IMC.Value.y, IMC.Value.z, IMC.Value.w };
            
            // Setup shadow properties
            //ImColor IMCS(Color);
            //FLinearColor ShadowColor = { IMCS.Value.x, IMCS.Value.y, IMCS.Value.z, IMCS.Value.w };
            
            if (!Texture.empty()) {
                UTexture2D* ActorTexture = nullptr;
                ActorTexture = ActorHandler::TextureLookup(Texture);
                if (!ActorTexture || !ActorTexture->IsValidLowLevel()) continue;
                double ShadowXY = 36.0;
                double IconXY = 32.0;
                FVector2D TextureSize = { static_cast<double>(ActorTexture->Blueprint_GetSizeX()), static_cast<double>(ActorTexture->Blueprint_GetSizeY()) };

                double ShadowScale = max(TextureSize.X, TextureSize.Y) / ShadowXY / (double)Configuration::ESPTextScale;
                FVector2D ShadowSize = { TextureSize.X / ShadowScale, TextureSize.Y / ShadowScale };
                FVector2D ShadowLocation = { ScreenLocation.X - ShadowSize.X / 2.0, ScreenLocation.Y - ShadowSize.Y / 2.0 };

                // shadow texture
                HUD->Canvas->K2_DrawTexture(ActorTexture, ShadowLocation, ShadowSize, { 0.0,0.0 }, { 1.0, 1.0 }, { 1.0f,1.0f,1.0f,1.0f }, EBlendMode::BLEND_AlphaHoldout, 0.0f, { 0.5,0.5 });
                // normal texture
                double IconScale = max(TextureSize.X, TextureSize.Y) / IconXY / (double)Configuration::ESPTextScale;

                FVector2D IconSize = { TextureSize.X / IconScale, TextureSize.Y / IconScale};
                FVector2D IconLocation = { ScreenLocation.X - IconSize.X / 2.0, ScreenLocation.Y - IconSize.Y / 2.0 };

                FLinearColor IconColor = { 1.0f,1.0f,1.0f,1.0f };
                if (bIconShouldUseColor) {
                    IconColor = TextColor;
                }
                HUD->Canvas->K2_DrawTexture(ActorTexture, IconLocation, IconSize, { 0.0,0.0 }, { 1.0, 1.0 }, IconColor, EBlendMode::BLEND_Translucent, 0.0f, { 0.5,0.5 });
                if (bShouldDrawStar) {
                    if (auto StarTexture = static_cast<UTexture2D*>(LoadAssetPtrFromPath("/Game/UI/Assets_Shared/Icons/Icon_Special_01.Icon_Special_01")); StarTexture != nullptr) {
                        FVector2D StarSize = { 16.0,16.0 };
                        FVector2D StarLocation = { ScreenLocation.X - IconSize.X / 2.0 - StarSize.X / 2.0, ScreenLocation.Y + IconSize.Y / 2.0 - StarSize.Y / 2.0 };
                        HUD->Canvas->K2_DrawTexture(StarTexture, StarLocation, StarSize, { 0.0,0.0 }, { 1.0, 1.0 }, { 1.0f,0.9f,0.0f,1.0f }, EBlendMode::BLEND_Translucent, 0.0f, { 0.5,0.5 });
                    }
                }
                if (bOverlayText && !OverlayText.empty()) {
                    std::wstring OverlayWText(OverlayText.begin(), OverlayText.end());
                    FVector2D OverlayPosition = ScreenLocation; //{ ScreenLocation.X - IconSize.X / 2, ScreenLocation.Y + IconSize.Y / 2 };
                    HUD->Canvas->K2_DrawText(Roboto, FString(OverlayWText.data()), OverlayPosition, { 0.825 * (double)Configuration::ESPTextScale, 0.825 * (double)Configuration::ESPTextScale }, TextColor, 0.0f, { 1.0f,1.0f,1.0f,1.0f }, { 1.0, 1.0 }, true, true, true, { 0.0f, 0.0f, 0.0f, 1.0f });
                }
                FVector2D Offset = { 0.0, IconSize.Y / 2.0 };
                if (!IconText.empty()) {
                    for (auto line : IconText) {
                        auto DisplayString = FString(line.data());
                        auto textSize = HUD->Canvas->K2_TextSize(Roboto, DisplayString, TextScale);
                        HUD->Canvas->K2_DrawText(Roboto, DisplayString, { ScreenLocation.X, ScreenLocation.Y + Offset.Y }, TextScale, TextColor, 0.0f, { 0.0f,0.0f,0.0f,1.0f }, { 1.0,1.0 }, true, false, true, { 0.0f,0.0f,0.0f,1.0f });
                        Offset.Y += textSize.Y;
                    }
                }
                if (!IconAboveText.empty()) {
                    auto AboveString = FString(IconAboveText.data());
                    auto textSize = HUD->Canvas->K2_TextSize(Roboto, AboveString, TextScale);
                    HUD->Canvas->K2_DrawText(Roboto, AboveString, { ScreenLocation.X, ScreenLocation.Y - IconSize.Y / 2.0 - textSize.Y }, TextScale, TextColor, 0.0f, { 0.0f,0.0f,0.0f,1.0f }, { 1.0,1.0 }, true, false, true, { 0.0f,0.0f,0.0f,1.0f });
                }
            }
        }
    }
}