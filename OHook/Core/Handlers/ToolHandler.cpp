#include "ToolHandler.h"
#include "Core/DetourManager.h"
#include "Misc/Utils.h"

#include "SDK/AssetRegistry_parameters.hpp"
#include "SDK/Palia_parameters.hpp"

void ToolHandler::SetupDetours(AValeriaCharacter* VC, AValeriaPlayerController* VPC) {
	DetourManager::AddInstance(VC);
	DetourManager::AddFunctionListener("Function Palia.ValeriaCharacter.HandleEquipmentChanged", nullptr, &Func_PE_EquipmentChanged);
	DetourManager::AddInstance(VPC);
	DetourManager::AddFunctionListener("Function Engine.PlayerController.ClientTravelInternal", &Func_PE_ClientTravelInternal, nullptr);
}

void ToolHandler::Func_PE_EquipmentChanged(const UObject* Context, UFunction* Function, void* Parms) {
    auto Params = static_cast<Params::ValeriaCharacter_HandleEquipmentChanged*>(Parms);
    if (Params->Param_EquipmentComponent->GetOwner() != GetValeriaCharacter()) // All players trigger this event
        return;
    EquippedToolBagSlot = Params->Item.ItemMetaData.InventoryLocation;
    auto [Tool, Tier] = ParseItem(Params->Param_EquipmentComponent->GetValeriaItemFromEquipped());
    EquippedTool = Tool;
    EquippedToolTier = Tier;
}

void ToolHandler::Func_PE_ClientTravelInternal(const UObject* Context, UFunction* Function, void* Parms) {
    EquippedToolBagSlot = FBagSlotLocation{-1,-1};
    EquippedTool = ETools::None;
    EquippedToolTier = EToolTier::None;
}

std::pair<ETools, EToolTier> ToolHandler::ParseItem(FValeriaItem Item) {
    auto Tool = ETools::None;
	auto Tier = EToolTier::None;
    if (UValeriaBlueprintFunctionLibrary::ItemIsEmpty(Item))
        return {Tool,Tier};

	auto EquippedName = Item.ItemType->Name.ToString();

    if (EquippedName.find("Tool_Axe_") != std::string::npos) {
        Tool = ETools::Axe;
    }
    else if (EquippedName.find("Tool_InsectBallLauncher_") != std::string::npos) {
        Tool = ETools::Belt;
    }
    else if (EquippedName.find("Tool_Bow_") != std::string::npos) {
        Tool = ETools::Bow;
    }
    else if (EquippedName.find("Tool_Rod_") != std::string::npos) {
        Tool = ETools::FishingRod;
    }
    else if (EquippedName.find("Tool_Hoe_") != std::string::npos) {
        Tool = ETools::Hoe;
    }
    else if (EquippedName.find("Tool_Pick") != std::string::npos) {
        Tool = ETools::Pick;
    }
    else if (EquippedName.find("Tool_WateringCan_") != std::string::npos) {
        Tool = ETools::WateringCan;
    }

    if (EquippedName.find("_Exquisite") != std::string::npos) {
        Tier = EToolTier::Exquisite;
    } 
    else if (EquippedName.find("_Fine") != std::string::npos) {
        Tier = EToolTier::Fine;
    }
    else if (EquippedName.find("_Simple") != std::string::npos) {
        Tier = EToolTier::Simple;
    }
    else if (EquippedName.find("_Standard") != std::string::npos) {
        Tier = EToolTier::Standard;
    }

	return { Tool, Tier };
}

void ToolHandler::SetEquippedTool(ETools Tool) {
    if (ToolActionMap.empty()) PopulateToolActionMap();
    if (auto FoundEntry = ToolActionMap.find(Tool); FoundEntry != ToolActionMap.end()) {
        auto PC = GetPlayerController();
        if (PC && PC->IsA(ABP_ValeriaPlayerController_C::StaticClass())) {
            auto VPC = static_cast<ABP_ValeriaPlayerController_C*>(PC);
            VPC->EquipToolFromInputAction(FoundEntry->second);
        }
    }
}

void ToolHandler::PopulateToolActionMap() {
    auto ClassPath = UKismetSystemLibrary::GetClassTopLevelAssetPath(UInputAction::StaticClass());
    auto AssetRegistry = UAssetRegistryHelpers::GetAssetRegistry().GetObjectRef();
    if (AssetRegistry) {
        UFunction* GetAssetsByClass = UObject::FindObject<UFunction>("Function AssetRegistry.AssetRegistry.GetAssetsByClass");
        if (GetAssetsByClass) {
            Params::AssetRegistry_GetAssetsByClass Parms{};
            Parms.ClassPathName = ClassPath;
            Parms.bSearchSubClasses = false;
            AssetRegistry->ProcessEvent(GetAssetsByClass, &Parms);
            if (Parms.ReturnValue) {
                for (auto& AssetData : Parms.OutAssetData) {
                    auto PackageName = AssetData.PackageName.ToString();
                    if (PackageName.find("_ToolEquip_") == std::string::npos) continue;
                    auto Tool = ETools::None;
                    if (PackageName.find("_Axe") != std::string::npos) {
                        Tool = ETools::Axe;
                    }
                    else if (PackageName.find("_BombBag") != std::string::npos) {
                        Tool = ETools::Belt;
                    }
                    else if (PackageName.find("_Bow") != std::string::npos) {
                        Tool = ETools::Bow;
                    }
                    else if (PackageName.find("_FishingRod") != std::string::npos) {
                        Tool = ETools::FishingRod;
                    }
                    else if (PackageName.find("_Hoe") != std::string::npos) {
                        Tool = ETools::Hoe;
                    }
                    else if (PackageName.find("_Pickaxe") != std::string::npos) {
                        Tool = ETools::Pick;
                    }
                    else if (PackageName.find("_WateringCan") != std::string::npos) {
                        Tool = ETools::WateringCan;
                    }
                    if (Tool != ETools::None && UAssetRegistryHelpers::IsValid(AssetData)) {
                        auto Asset = UAssetRegistryHelpers::GetAsset(AssetData);
                        if (Asset) ToolActionMap.insert({ Tool, static_cast<UInputAction*>(Asset) });
                    }
                }
            }
        }
    }
}