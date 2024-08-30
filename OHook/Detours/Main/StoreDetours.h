#pragma once

#include <SDK.hpp>
#include <map>

using namespace SDK;

class StoreDetours final {
public:
    static void SetupDetours(AValeriaCharacter* VC, AValeriaPlayerController* VPC);

    static void Tick(const UObject* Context, UFunction* Function, void* Parms);
    static void OnInventoryChanged(const UObject* Context, UFunction* Function, void* Parms);
    static void RpcServer_BuyItem(const UObject* Context, UFunction* Function, void* Parms);
    static void RpcClient_SellItemsReady(const UObject* Context, UFunction* Function, void* Parms);

    static void Func_OpenGuildStore(const AValeriaCharacter* ValeriaCharacter, int storeId);
    static void Func_OpenConfigStore();
    static void Func_SellItem();

private:
    static void Func_SetToGeneralStore();

public:
    static bool bShouldOpenConfigStore;
    static bool bShouldManualSell;

    static bool bStoreLoading;
    static bool bManualSellPending;

    static std::string SelectedStoreConfig;
    static FBagSlotLocation ManualSellLocation;
    static int ManualSellQty;

    static inline std::map<std::string, std::string> ConfigStores = {
        {"/Game/Configs/RegisterStores/DA_Store_BlackMarket.DA_Store_BlackMarket", "Black Market"},
        {"/Game/Configs/RegisterStores/DA_Store_BlackMarket_Bar.DA_Store_BlackMarket_Bar", "Black Market Bar"},
        {"/Game/Configs/RegisterStores/DA_Store_Blacksmith.DA_Store_Blacksmith", "Blacksmith"},
        {"/Game/Configs/RegisterStores/DA_Store_Housing.DA_Store_Housing", "City Hall"},
        {"/Game/Configs/RegisterStores/DA_Store_Crops.DA_Store_Crops", "Farm"},
        {"/Game/Configs/RegisterStores/DA_Store_Furniture.DA_Store_Furniture", "Furniture"},
        {"/Game/Configs/RegisterStores/DA_Store_General.DA_Store_General", "General Store"},
        {"/Game/Configs/RegisterStores/DA_Store_HousingTours.DA_Store_HousingTours", "Home Tours Store"},
        {"/Game/Configs/RegisterStores/DA_Store_Inn.DA_Store_Inn", "Inn"},
        {"/Game/Configs/RegisterStores/DA_Store_Library.DA_Store_Library", "Library"},
        {"/Game/Configs/RegisterStores/DA_Store_Halloween.DA_Store_Halloween", "Spooky Moon Store"},
        {"/Game/Configs/RegisterStores/DA_Store_Winter.DA_Store_Winter", "Winterlights Store"},
        {"/Game/Configs/RegisterStores/DA_Store_MajiMarket_Fireworks.DA_Store_MajiMarket_Fireworks", "Maji Market Fireworks"},
        {"/Game/Configs/RegisterStores/DA_Store_MajiMarket_ExperimentalFood.DA_Store_MajiMarket_ExperimentalFood", "Maji Market Reth"},
        {"/Game/Configs/RegisterStores/DA_Store_MajiMarket_ChapaaBooth.DA_Store_MajiMarket_ChapaaBooth", "Maji Market Chapaa Booth"},
        {"/Game/Configs/RegisterStores/DA_Store_LNY_ChapaaBooth.DA_Store_LNY_ChapaaBooth", "LNY - Chapaa Booth"},
        {"/Game/Configs/RegisterStores/DA_Store_LNY_ChapaaChase.DA_Store_LNY_ChapaaChase", "LNY - Chapaa Chase"},
        {"/Game/Configs/RegisterStores/DA_Store_LNY_Delaila.DA_Store_LNY_Delaila", "LNY - Delaila"},
        {"/Game/Configs/RegisterStores/DA_Store_LNY_ExperimentalFood.DA_Store_LNY_ExperimentalFood", "LNY - Reth"},
        {"/Game/Configs/RegisterStores/DA_Store_LNY_Fireworks.DA_Store_LNY_Fireworks", "LNY - Fireworks"},
        {"/Game/Configs/RegisterStores/DA_Store_LNY_ZekiSnacks.DA_Store_LNY_ZekiSnacks", "LNY - Zeki"},
    };
    static inline std::map<int, std::string> GuildStores = {
        {2,"Fishing Guild Store"},
        {3,"Bug Catching Guild Store"},
        {6,"Foraging Guild Store"},
        {7,"Hunting Guild Store"},
        {10,"Cooking Guild Store"},
        {12,"Furniture Guild Store"},
        {8806,"Mining Guild Store"},
        {19928,"Gardening Guild Store"},
    };
};

inline bool StoreDetours::bShouldOpenConfigStore = false;
inline bool StoreDetours::bShouldManualSell = false;

inline bool StoreDetours::bStoreLoading = false;
inline bool StoreDetours::bManualSellPending = false;

inline std::string StoreDetours::SelectedStoreConfig = "/Game/Configs/RegisterStores/DA_Store_BlackMarket.DA_Store_BlackMarket";
inline FBagSlotLocation StoreDetours::ManualSellLocation = FBagSlotLocation{ -1,-1 };
inline int StoreDetours::ManualSellQty = 0;