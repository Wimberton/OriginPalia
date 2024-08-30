#include "Detours/Main/StoreDetours.h"
#include "Core/Configuration.h"
#include "Core/DetourManager.h"
#include "Core/Handlers/WidgetHandler.h"
#include "Misc/Utils.h"

void StoreDetours::SetupDetours(AValeriaCharacter* VC, AValeriaPlayerController* VPC) {
    DetourManager::AddInstance(VC);
	DetourManager::AddFunctionListener("Function Engine.Actor.ReceiveTick", &Tick, nullptr);
    DetourManager::AddFunctionListener("Function Palia.ValeriaCharacter.OnInventoryChanged",nullptr, &OnInventoryChanged);
    DetourManager::AddInstance(VC->StoreComponent);
	DetourManager::AddFunctionListener("Function Palia.VillagerStoreComponent.RpcServer_BuyItem", &RpcServer_BuyItem, nullptr);
    DetourManager::AddFunctionListener("Function Palia.VillagerStoreComponent.RpcClient_SellItemsReady", nullptr, &RpcClient_SellItemsReady);
}

void StoreDetours::Tick(const UObject* Context, UFunction* Function, void* Parms) {
    if (bShouldOpenConfigStore) {
        bShouldOpenConfigStore = false;
        Func_OpenConfigStore();
    }
    else if (bShouldManualSell) {
        bShouldManualSell = false;
        Func_SellItem();
    }
}

void StoreDetours::RpcServer_BuyItem(const UObject* Context, UFunction* Function, void* Parms) {
    if (Configuration::bEnableBuyMultiplier) {
        // Check if CTRL key is held down using VK_CONTROL and if the feature is enabled
        if (Configuration::bEnableCtrlClickBuy && (GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
            static_cast<Params::VillagerStoreComponent_RpcServer_BuyItem*>(Parms)->NumToBuy = Configuration::buyMultiplierValue;
        }
        // If CTRL key is not required, just apply the multiplier directly
        else if (!Configuration::bEnableCtrlClickBuy) {
            static_cast<Params::VillagerStoreComponent_RpcServer_BuyItem*>(Parms)->NumToBuy = Configuration::buyMultiplierValue;
        }
    }
}

void StoreDetours::RpcClient_SellItemsReady(const UObject* Context, UFunction* Function, void* Parms) {
    bStoreLoading = false;
    if (bManualSellPending) {
        bManualSellPending = false;
        Func_SellItem();
    }
}

void StoreDetours::Func_OpenGuildStore(const AValeriaCharacter* ValeriaCharacter, int storeId) {
    if (!IsActorValid(ValeriaCharacter))
        return;

    if (WidgetHandler::GameplayUIManager == nullptr) {
        return;
    }

    auto StoreComponent = ValeriaCharacter->StoreComponent;
    if (!StoreComponent || !StoreComponent->IsValidLowLevel())
        return;

    StoreComponent->Client_SetVillagerStore(storeId);
    StoreComponent->Client_OpenStore();
    WidgetHandler::GameplayUIManager->Implementation_OpenVillagerStore(storeId);
}

void StoreDetours::Func_OpenConfigStore() {
    auto ValeriaCharacter = GetValeriaCharacter();

    if (WidgetHandler::GameplayUIManager == nullptr) {
        return;
    }

    auto StoreComponent = ValeriaCharacter->StoreComponent;
    if (!StoreComponent || !StoreComponent->IsValidLowLevel())
        return;

    auto StoreConfig = static_cast<UVillagerStoreConfig*>(LoadAssetPtrFromPath(SelectedStoreConfig));
    if (!StoreConfig) return;

    StoreComponent->Client_SetStoreByConfig(StoreConfig);
    StoreComponent->Client_OpenStore();
    WidgetHandler::GameplayUIManager->Implementation_OpenStore(StoreConfig);
}

void StoreDetours::Func_SellItem() {
    if (ManualSellLocation.BagIndex == -1 || ManualSellLocation.SlotIndex == -1) return;

    auto ValeriaCharacter = GetValeriaCharacter();

    auto StoreComponent = ValeriaCharacter->StoreComponent;
    if (!StoreComponent || !StoreComponent->IsValidLowLevel())
        return;

    if (StoreComponent->StoreCanBuyItem(ManualSellLocation)) {
        ValeriaCharacter->StoreComponent->Client_SellItem_Custom(ManualSellLocation, ManualSellQty);
        ManualSellLocation = FBagSlotLocation{ -1,-1 };
    }
    else {
        bManualSellPending = true;
        Func_SetToGeneralStore();
    }
}

void StoreDetours::Func_SetToGeneralStore() {
    auto ValeriaCharacter = GetValeriaCharacter();

    if (WidgetHandler::GameplayUIManager == nullptr) {
        return;
    }

    auto StoreComponent = ValeriaCharacter->StoreComponent;
    if (!StoreComponent || !StoreComponent->IsValidLowLevel())
        return;

    auto StoreConfig = static_cast<UVillagerStoreConfig*>(LoadAssetPtrFromPath("/Game/Configs/RegisterStores/DA_Store_General.DA_Store_General"));
    if (!StoreConfig) return;

    bStoreLoading = true;
    StoreComponent->Client_SetStoreByConfig(StoreConfig);
    StoreComponent->Client_OpenStore();
}

void StoreDetours::OnInventoryChanged(const UObject* Context, UFunction* Function, void* Parms) {
    if (bStoreLoading) return;

    const auto ValeriaController = GetValeriaController();
    if (!ValeriaController)
        return;

    const auto ValeriaCharacter = GetValeriaCharacter();
    if (!ValeriaCharacter)
        return;

    UVillagerStoreComponent* StoreComponent = ValeriaCharacter->StoreComponent;
    if(!IsComponentValid(StoreComponent))
        return;
    
    const UInventoryComponent* InventoryComponent = ValeriaCharacter->GetInventory();
    if(!IsComponentValid(InventoryComponent))
        return;

    // Sell / Discard / Storage
    for (int BagIndex = 0; BagIndex < InventoryComponent->Bags.Num(); BagIndex++) {
        FInventoryBag Bag = InventoryComponent->Bags[BagIndex];
        // check if starter bag or expansion bags
        if (Bag.InventoryBagConfigId != 1 && Bag.InventoryBagConfigId != 4) continue;
        for (int SlotIndex = 0; SlotIndex < Bag.Slots.Num(); SlotIndex++) {
            FBagSlotLocation Slot{ BagIndex, SlotIndex };
            FValeriaItem Item = InventoryComponent->GetItemAt(Slot);
            if (UValeriaBlueprintFunctionLibrary::ItemIsEmpty(Item)) continue;
            bool shouldSell = false;

            if (Configuration::bFishingSell && Item.ItemType->Category == EItemCategory::Fish) {
                shouldSell = Configuration::bFishingSellRarity[(int)Item.ItemType->Rarity];
                if (Item.MetaData.QualityStars > 0 && !Configuration::bFishingSellSQ) {
                    shouldSell = false;
                };  
            }
            else if (Configuration::bBugSell && Item.ItemType->Category == EItemCategory::Bug) {
                shouldSell = Configuration::bBugSellRarity[(int)Item.ItemType->Rarity];
                if (Item.MetaData.QualityStars > 0 && !Configuration::bBugSellSQ) {
                    shouldSell = false;
                };
            }
            else if (Configuration::bHuntingSell && UBlueprintGameplayTagLibrary::HasTag(Item.ItemType->Tags.GameplayTags, FGameplayTag{ UKismetStringLibrary::Conv_StringToName(L"Item.Hunting") }, true)) {
                shouldSell = Configuration::bHuntingSellRarity[(int)Item.ItemType->Rarity];
                if (Item.MetaData.QualityStars > 0 && !Configuration::bHuntingSellSQ) {
                    shouldSell = false;
                };
            }
            else if (Configuration::bFlowerSell && UBlueprintGameplayTagLibrary::HasTag(Item.ItemType->Tags.GameplayTags, FGameplayTag{ UKismetStringLibrary::Conv_StringToName(L"Item.Decor.Flowers") }, true)) {
                shouldSell = true;
            }
            else if (Configuration::bForageableSell && UBlueprintGameplayTagLibrary::HasTag(Item.ItemType->Tags.GameplayTags, FGameplayTag{ UKismetStringLibrary::Conv_StringToName(L"Item.Forageable") }, false)) {
                shouldSell = Configuration::bForageableSellRarity[(int)Item.ItemType->Rarity];
                if (Item.MetaData.QualityStars > 0 && !Configuration::bForageableSellSQ) {
                    shouldSell = false;
                };
            }
            else if (Configuration::bTreeSell && UBlueprintGameplayTagLibrary::HasTag(Item.ItemType->Tags.GameplayTags, FGameplayTag{ UKismetStringLibrary::Conv_StringToName(L"Item.Wood") }, false)) {
                switch (Item.ItemType->PersistId) {
                case 6277:
                    // sapwood
                    shouldSell = Configuration::bTreeSellRarity[1];
                    break;
                case 7305:
                    // heartwood
                    shouldSell = Configuration::bTreeSellRarity[2];
                    break;
                case 19774:
                    // flow
                    shouldSell = Configuration::bTreeSellRarity[3];
                    break;
                }
            }
            else if (Configuration::bTreeSell && Item.ItemType->PersistId == 28731) {
                // plant fiber
                shouldSell = Configuration::bTreeSellRarity[0];
            }
            else if (Configuration::bFishingDiscard && Item.ItemType->Category == EItemCategory::Junk) {
                // Don't ever discard more than the amount of the stack
                ValeriaController->DiscardItem(Slot, Item.Amount);
            }
            else if (Item.ItemType->PersistId == 2810) { // Waterlogged Chest
                if (!Configuration::bFishingOpenStoreWaterlogged) {
                    // Don't ever discard more than the amount of the stack
                    ValeriaController->DiscardItem(Slot, Item.Amount);
                }
                else {
                    ValeriaController->ConsumeItem(Slot);
                }
            }
            else if (Configuration::bFishingOpenStoreWaterlogged && Item.ItemType->Name.ToString().find("DA_ItemType_Decor_Makeshift_") != std::string::npos) {
                ValeriaController->MoveItemSlotToStorage(Slot, 1, EStoragePoolType::Primary);
            }
            if (shouldSell) {
                if (!StoreComponent->StoreCanBuyItem(Slot)) {
                    Func_SetToGeneralStore();
                }
                else {
                    StoreComponent->Client_SellItem(Slot);
                }
            }
        }
    }
};