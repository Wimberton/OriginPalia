#pragma once

#include <SDK.hpp>
#include <SDK/Palia_parameters.hpp>

class PaliaOverlay;

using namespace SDK;

enum class EViewModel {
	Unknown,
	PreGame,
	Main,
	CharacterCustomization,
	PremiumStore,
	Subgame,
	Inventory,
	Housing,
	PaliaPass,
};

enum class EWidgetType {
	Unknown,
	PMenu,
	RepairStation,
	CollectionsScreen,
	ReportPlayer,
};

class WidgetHandler final {
public:
	WidgetHandler() = default;

	static void SetupDetours(AValeriaCharacter* VC, AValeriaPlayerController* VPC);

	static void Tick(const UObject* Context, UFunction* Function, void* Parms);
	static void ReceivePlay(const UObject* Context, UFunction* Function, void* Parms);
	static void ReceiveDrawHUD(const UObject* Context, UFunction* Function, void* Parms);

	static void FindWidget(UClass* DesiredWidgetClass, UUserWidget* *ReturnedWidget);

	static void Func_ActivateModalWidget(void*, void*, void*);
	static void Func_RemoveModalWidget(void*, void*, void*);
	static bool IsWidgetOpen();
	static bool IsMapOpen();

	static UWBP_GameplayUIManager_C* GameplayUIManager;
	static EViewModel CurrentViewModel;
	static US6UI_StateUserWidget* CurrentStateWidget;
    
	static UWidget* ActiveWidget;
	static EWidgetType ActiveWidgetType;

	static std::pair<bool,FVector2D> GetMapPoint(const FVector&);
	
private:

	static void ResetActiveWidget();
	static EWidgetType ParseWidget(std::string);

	static UClass* PMenuClass;
	static UWBP_PMenu_CM_C* PMenu;
	static UFunction* PMenu_GetOrCreateContentForTab;

	static UClass* WorldMapClass;
	static UWBP_WorldMap_CM_C* WorldMap;
	static UFunction* WorldMap_WorldPositionToUiMapPosition;
	static UWBP_WorldMap_CM_C* GetMapWidget();

	static void SetupPoints();
	static FVector2D MapUpperLeft;
	static FVector2D MapLowerRight;
	static FVector2D MapCenter;

	static FName ViewModelContext;

	static void ParseStoreContent(UWBP_PremiumStoreScreen_CM_C*);
};

inline UWBP_GameplayUIManager_C* WidgetHandler::GameplayUIManager = nullptr;
inline UWidget* WidgetHandler::ActiveWidget = nullptr;
inline EWidgetType WidgetHandler::ActiveWidgetType = EWidgetType::Unknown;

inline UClass* WidgetHandler::PMenuClass = nullptr;
inline UWBP_PMenu_CM_C* WidgetHandler::PMenu = nullptr;
inline UFunction* WidgetHandler::PMenu_GetOrCreateContentForTab = nullptr;

inline UClass* WidgetHandler::WorldMapClass = nullptr;
inline UWBP_WorldMap_CM_C* WidgetHandler::WorldMap = nullptr;
inline UFunction* WidgetHandler::WorldMap_WorldPositionToUiMapPosition = nullptr;

inline FName WidgetHandler::ViewModelContext = FName{ 0,0 };
inline EViewModel WidgetHandler::CurrentViewModel = EViewModel::Unknown;
inline US6UI_StateUserWidget* WidgetHandler::CurrentStateWidget = nullptr;

inline FVector2D WidgetHandler::MapUpperLeft = { 0,0 };
inline FVector2D WidgetHandler::MapLowerRight = { 0,0 };
inline FVector2D WidgetHandler::MapCenter = { 0,0 };

inline std::unordered_map<std::string, EViewModel>EViewModelMap = {
	{"PreGameHUD",EViewModel::PreGame},
	{"MainHUD",EViewModel::Main},
	{"CharacterCustomizationHUD",EViewModel::CharacterCustomization},
	{"PremiumStoreHUD",EViewModel::PremiumStore},
	{"Subgame",EViewModel::Subgame},
	{"InventoryHUD",EViewModel::Inventory},
	{"HousingHUD",EViewModel::Housing},
	{"PaliaPass",EViewModel::PaliaPass},
};