#include "BlackMarketDetours.h"
#include "Misc/SDKExt.h"
#include "Misc/Utils.h"
#include "Core/Configuration.h"
#include "Core/DetourManager.h"

using namespace SDK;

void BlackMarketDetours::SetupDetours(AValeriaCharacter* VC, AValeriaPlayerController* VPC) {
    DetourManager::AddFunctionDetour("Function Palia.VAL_PrizeWheelComponent.OnClientInteract", nullptr, &Func_OnClientInteract);
    DetourManager::AddFunctionDetour("Function Palia.VAL_PrizeWheelComponent.RpcNetMulticast_SpinTheWheel", &Func_SpinTheWheel, nullptr);
    DetourManager::AddFunctionDetour("Function Palia.VAL_PrizeWheelComponent.IsInteractable", nullptr, &Func_IsInteractable);

    // 9591 Get SoftPtr for Coin
    PrizeWheelCoin = UVAL_ItemTypeDefinitionAsset::GetAssetByPersistId_Cast(9591);
    WheelComponentUsed = nullptr;
    bCoinsStored = false;
    TimeSpun = -1;
}

void BlackMarketDetours::Func_OnClientInteract(void* Context, void* TheStack, void* Result) {
    // store which of the two wheels we're interacting with
    WheelComponentUsed = static_cast<UVAL_PrizeWheelComponent*>(Context);
    TimeSpun = WheelComponentUsed->GetOwner()->GetGameTimeSinceCreation();
    //std::cout << "\nInteracted with wheel at " <<  TimeSpun << std::endl;
}

void BlackMarketDetours::Func_SpinTheWheel(void* Context, void* TheStack, void* Result) {
    // check what position we got, store coins if not the one we want
    if (static_cast<UVAL_PrizeWheelComponent*>(Context) == WheelComponentUsed) {
        //std::cout << "\nOur Wheel Rolled\n" << std::endl;
        auto Stack = static_cast<SDKExt::FFrame*>(TheStack);
        auto Params = reinterpret_cast<Params::VAL_PrizeWheelComponent_RpcNetMulticast_SpinTheWheel*>(Stack->Locals);
        auto slot = Params->Slot;
        if (Configuration::bEnablePrizeWheel && !Configuration::PrizeWheelSlots[slot]) {
            if (auto ValeriaCharacter = GetValeriaCharacter(); ValeriaCharacter) {
                if (auto InventoryComponent = ValeriaCharacter->GetInventory(); InventoryComponent) {
                    for (int trycount = 0; trycount < 5; trycount++) {
                        CoinLocation = InventoryComponent->FindItemByType(PrizeWheelCoin, false);
                        if (CoinLocation.BagIndex == -1 || CoinLocation.SlotIndex == -1)
                            break; // no coins found
                        
                        if (auto ValeriaController = GetValeriaController(); ValeriaController) {
                            if (Configuration::bPrizeWheelUseLocked) {
                                ValeriaController->MoveItemSlotToStorage(CoinLocation, 100, EStoragePoolType::Lockbox);
                            } else {
                                ValeriaController->MoveItemSlotToStorage(CoinLocation, 100, EStoragePoolType::Primary);
                            }
                            bCoinsStored = true;
                        }
                    }
                }
            }
        }
    }
}

void BlackMarketDetours::Func_IsInteractable(void* Context, void* TheStack, void* Result) {
    if (auto thisWheelComponent = static_cast<UVAL_PrizeWheelComponent*>(Context); thisWheelComponent == WheelComponentUsed) {
        if (!thisWheelComponent->bIsSpinning) { // wheel not spinning
            if (auto ResultBool = *static_cast<bool*>(Result); ResultBool) { // can interact with wheel
                if (auto timeNow = thisWheelComponent->GetOwner()->GetGameTimeSinceCreation(); TimeSpun > 0 && (timeNow - TimeSpun) > 5) {
                    //std::cout << "\nCoins being returned at " << timeNow << std::endl;
                    WheelComponentUsed = nullptr;
                    TimeSpun = -1;
                    if (bCoinsStored) {
                        if (auto ValeriaCharacter = GetValeriaCharacter(); ValeriaCharacter) {
                            if (auto StorageComponent = ValeriaCharacter->GetStorage(); StorageComponent) {
                                EStoragePoolType Pool = EStoragePoolType::Primary;
                                if (Configuration::bPrizeWheelUseLocked) Pool = EStoragePoolType::Lockbox;
                                auto Item = StorageComponent->GetAllItemsOfType(PrizeWheelCoin, Pool);
                                if (Item.Num() > 0) {
                                    bCoinsStored = false;
                                    if (auto ValeriaController = ValeriaCharacter->GetValeriaPlayerController(); ValeriaController) {
                                        ValeriaController->MoveStorageToItemSlot(Item[0], CoinLocation, Pool);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}