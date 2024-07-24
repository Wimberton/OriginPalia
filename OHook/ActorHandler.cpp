#include "ActorHandler.h"
#include "Misc/Utils.h"
#include "Configuration.h"
#include "Misc/SDKExt.h"
#include <format>

using namespace SDK;

const void* vtable_hook(const void** vtable, const int index, const void* hook) {
	DWORD old_protect;
	VirtualProtect(&vtable[index], sizeof(void*), PAGE_READWRITE, &old_protect);
	const auto* orig = vtable[index];
	vtable[index] = hook;
	VirtualProtect(&vtable[index], sizeof(void*), old_protect, &old_protect);
	return orig;
}

#define AH_BEGINPLAY_HOOK(ActorClass)																			\
if (!Orig_BeginPlay_##ActorClass) {																				\
	auto Actor = ActorClass::GetDefaultObj();																	\
	if (Actor) {																								\
		const void** Vtable = *static_cast<const void***>(static_cast<void*>(Actor));							\
		Orig_BeginPlay_##ActorClass = (ActorBeginPlay)vtable_hook(Vtable, 0x72, &Detour_BeginPlay_##ActorClass);\
	}																											\
}

struct FActorCreateListener : public SDKExt::FUObjectCreateListener
{
	static FActorCreateListener ActorCreateListener;

	void NotifyUObjectCreated(const UObject* object, int32 index) override
	{

		if (object->IsA(EClassCastFlags::Class)) {
			ActorHandler::AddClass(std::bit_cast<UClass*>(object));
		}
		if (object->IsA(EClassCastFlags::Actor) && !object->IsDefaultObject()) {
			ActorHandler::ProcessActor(std::bit_cast<AActor*>(object));
		}
	}

	void OnUObjectArrayShutdown() override
	{
		SDKExt::UObjectArray::RemoveUObjectCreateListener(&ActorCreateListener);
	}
};
FActorCreateListener FActorCreateListener::ActorCreateListener{};

struct FActorDeleteListener : public SDKExt::FUObjectDeleteListener
{
	static FActorDeleteListener ActorDeleteListener;

	void NotifyUObjectDeleted(const UObject* object, int32 index) override
	{
		if (object->IsA(EClassCastFlags::Class)) {
			ActorHandler::RemoveClass(std::bit_cast<UClass*>(object));
		}
		if (object->IsA(EClassCastFlags::Actor)) {
			ActorHandler::RemoveActor(std::bit_cast<AActor*>(object));
		}
	}

	void OnUObjectArrayShutdown() override
	{
		SDKExt::UObjectArray::RemoveUObjectDeleteListener(&ActorDeleteListener);
	}
};
FActorDeleteListener FActorDeleteListener::ActorDeleteListener{};

void ActorHandler::SetupListeners() {
	SDKExt::UObjectArray::AddUObjectCreateListener(&FActorCreateListener::ActorCreateListener);
	SDKExt::UObjectArray::AddUObjectDeleteListener(&FActorDeleteListener::ActorDeleteListener);
	bListenersAdded = true;
}

#undef AH_BEGINPLAY_HOOK

void ActorHandler::ClientTravelInternal() {
	bIsTraveling = true;
}

void ActorHandler::LevelChange(ULevel* level) {
	LastUpdate = 0; // GetTimeSeconds is level based
	LastDormantUpdate = 0;
	bIsTraveling = false;
}

/*
	AActor
		AGatherableActor
			ABP_ValeriaGatherableLoot_C
				ABP_ValeriaGatherableLoot_Lumber_C
				ABP_ValeriaGatherableLoot_Mining_Base_C
		APawn
			AValeriaMoveablePawn
				ACreatureCharacter
					ABugCatchingCreatureCharacter
						ABP_ValeriaBugCatchingCreature_C
					ABP_ValeriaHuntingCreature_C
				AValeriaCharacter
					ABP_ValeriaCharacter_C
				AValeriaPetCharacter
					ABP_Pet_Base_C
				AValeriaVillagerCharacter
					ABP_ValeriaVillagerCharacter_C
		AWorldGatherableBase
			ATimedLootPile
				ABP_TimedLootPileBase_C
			AWorldPersistGatherable
				ABP_WorldPersistGatherable_Base_C
					ABP_WorldPersistGatherable_Treasure_Base_C

		ABP_ConfigurableBook_C
		ABP_Loot_C
		ABP_QuestInspect_Base_C
		ABP_SimpleInspect_Base_C
		ABP_Stables_Sign_C
		ABP_ValeriaGatherable_C
			ABP_Facility_Well_C
			ABP_QuestItem_BASE_C
			ABP_Valeria_Gatherable_Placed_C
*/

#define CHECK_ACTOR_TYPE(className, type)				\
if (IsValidPtr(Actor) && IsActorA(Actor, className)) {	\
	ActorType = type;									\
}

void ActorHandler::ProcessActor(AActor* Actor) {
	// first pass highest superclass possible to clear unwanted actors fast
	auto ActorType = EType::Unknown;

	// Tree, Ore
	if (Actor->IsA(AGatherableActor::StaticClass())) {
		// Tree
		CHECK_ACTOR_TYPE(L"/Game/Gameplay/Skills/Lumberjacking/Felling/BP_ValeriaGatherableLoot_Lumber.BP_ValeriaGatherableLoot_Lumber", EType::Tree)
			// Ore
	else CHECK_ACTOR_TYPE(L"/Game/Gameplay/Skills/Mining/BP_ValeriaGatherableLoot_Mining_Base.BP_ValeriaGatherableLoot_Mining_Base", EType::Ore)
	}
	// Bugs, Animals, Players, NPCs
	else if (Actor->IsA(AValeriaMoveablePawn::StaticClass())) {
		// NPC
		if (Actor->IsA(AValeriaVillagerCharacter::StaticClass())) {
			ActorType = EType::NPCs;
		}
		// Player
		else if (Actor->IsA(AValeriaCharacter::StaticClass())) {
			ActorType = EType::Players;
		}
		// Bug
		else if (Actor->IsA(ABugCatchingCreatureCharacter::StaticClass())) {
			ActorType = EType::Bug;
		}
		// Animal
		else CHECK_ACTOR_TYPE(L"/Game/Gameplay/Skills/Hunting/BP_ValeriaHuntingCreature.BP_ValeriaHuntingCreature", EType::Animal)
	}
	// Rummage, Treaure
	else if (Actor->IsA(AWorldGatherableBase::StaticClass())) {
		// Rummage
		if (Actor->IsA(ATimedLootPile::StaticClass())) {
			ActorType = EType::RummagePiles;
		}
		// Treasure
		else CHECK_ACTOR_TYPE(L"/Game/Core/TreasureTypes/BP_WorldPersistGatherable_Treasure_Base.BP_WorldPersistGatherable_Treasure_Base", EType::Treasure)
	}
	else if (Actor->IsA(AVAL_TeleporterBase::StaticClass())) {
		Teleporters.push_back(Actor);
		return;
	}
	else if (IsBlueprintClass(Actor->Class)) {
		ProcessBPActor(Actor);
		return;
	}
	if (ActorType == EType::Unknown) return;
	FinishProcessingActor(Actor, ActorType);
}

void ActorHandler::ProcessBPActor(AActor* Actor) {
	auto ActorType = EType::Unknown;
	for (auto& BPClass : BPActors) {
		if (IsValidPtr(Actor) && IsActorA(Actor, BPClass.first)) {
			ActorType = BPClass.second;
			break;
		}
	}
	if (ActorType == EType::Unknown) return;
	FinishProcessingActor(Actor, ActorType);
}

bool ActorHandler::IsBlueprintClass(UClass* Class) {
	auto classFlags = *std::bit_cast<EClassFlags*>(std::bit_cast<char*>(Class) + 0xD4);
	return classFlags & EClassFlags::CompiledFromBlueprint;
}

#undef CHECK_ACTOR_TYPE

void ActorHandler::FinishProcessingActor(AActor* Actor, EType ActorType) {
	auto ClassName = Actor->Class->GetName();

	uint8_t Type = 0;
	uint8_t Quality = 0;
	uint8_t Variant = 0;
	std::string Texture;
	std::wstring Name;

	bool shouldAdd = false;

	switch (ActorType) {
	case EType::Tree: {
		if (auto Tree = GetFlagSingle(ClassName, TREE_TYPE_MAPPINGS); Tree != ETreeType::Unknown) {
			if (auto Size = GetFlagSingle(ClassName, GATHERABLE_SIZE_MAPPINGS); Size != EGatherableSize::Unknown) {
				shouldAdd = true;
				Type = static_cast<uint8_t>(Tree);
				Variant = static_cast<uint8_t>(Size);
				Texture = TreeTextures[Type];
			}
		}
		break;
	}
	case EType::Ore: {
		if (auto OreType = GetFlagSingle(ClassName, MINING_TYPE_MAPPINGS); OreType != EOreType::Unknown) {
			auto Size = GetFlagSingle(ClassName, GATHERABLE_SIZE_MAPPINGS);
			if (OreType == EOreType::Clay)
				Size = EGatherableSize::Large;
			if (Size != EGatherableSize::Unknown) {
				shouldAdd = true;
				Type = static_cast<uint8_t>(OreType);
				Variant = static_cast<uint8_t>(Size);
				Texture = OreTextures[Type];
			}
		}
		break;
	}
	case EType::Bug: {
		if (auto BugType = GetFlagSingle(ClassName, CREATURE_BUGKIND_MAPPINGS); BugType != EBugKind::Unknown) {
			if (auto BVar = GetFlagSingleEnd(ClassName, CREATURE_BUGQUALITY_MAPPINGS); BVar != EBugQuality::Unknown) {
				shouldAdd = true;
				Type = static_cast<uint8_t>(BugType);
				Variant = static_cast<uint8_t>(BVar);
				if (ClassName.ends_with("+_C")) {
					Quality = 1;
				}
				Texture = BugTextures[Type][Variant];
			}
		}
		break;
	}
	case EType::Animal: {
		if (auto CKType = GetFlagSingle(ClassName, CREATURE_KIND_MAPPINGS); CKType != ECreatureKind::Unknown) {
			if (auto CQType = GetFlagSingleEnd(ClassName, CREATURE_KINDQUALITY_MAPPINGS); CQType != ECreatureQuality::Unknown) {
				shouldAdd = true;
				Type = static_cast<uint8_t>(CKType);
				Variant = static_cast<uint8_t>(CQType);
				Texture = AnimalTextures[Type][Variant];
			}
		}
		break;
	}
	case EType::Forage: {
		if (auto ForageType = GetFlagSingle(ClassName, FORAGEABLE_TYPE_MAPPINGS); ForageType != EForageableType::Unknown) {
			shouldAdd = true;
			Type = static_cast<uint8_t>(ForageType);
			if (ClassName.ends_with("+_C")) {
				Quality = 1;
			}
			Texture = ForageableTextures[Type];
		}
		break;
	}
	case EType::Loot: {
		shouldAdd = true;
		Type = 1; // doesn't matter, but isn't "unknown"
		Texture = "/Game/UI/Assets_Shared/Icons/WT_Icon_Inventory_Bag.WT_Icon_Inventory_Bag";
		break;
	}
	case EType::Players: {
		shouldAdd = true;
		Type = 1; // doesn't matter, but isn't "unknown"
		Name = static_cast<AValeriaCharacter*>(Actor)->CharacterName.ToWString();
		Texture = "/Game/UI/Clock/Icon_DayClockArrow_01.Icon_DayClockArrow_01";
		break;
	}
	case EType::NPCs: {
		shouldAdd = true;
		Type = 1; // doesn't matter, but isn't "unknown"
		Texture = VillagerTextures[ClassName];
		break;
	}
	case EType::Quest: {
		shouldAdd = true;
		Type = 1;
		Texture = "/Game/UI/Assets_Shared/Icons/Icon_Compass_Quest_01.Icon_Compass_Quest_01";
		break;
	}
	case EType::RummagePiles: {
		shouldAdd = true;
		Type = 1;
		Texture = "/Game/UI/Icons/Icon_Deco_Chapaa_Nest.Icon_Deco_Chapaa_Nest";
		break;
	}
	case EType::Stables: {
		shouldAdd = true;
		Type = 1;
		Texture = "/Game/UI/Assets_Shared/Icons/Icon_Compass_Stable_01.Icon_Compass_Stable_01";
		break;
	}
	case EType::Fish: {
		if (auto FishType = GetFlagSingle(ClassName, FISH_TYPE_MAPPINGS); FishType != EFishType::Unknown) {
			shouldAdd = true;
			Type = static_cast<uint8_t>(FishType);
		}
		break;
	}
	case EType::Treasure: {
		shouldAdd = true;
		Type = 1;
		Texture = TreasureTextures[ClassName];
		break;
	}
	case EType::TimedDrop: {
		shouldAdd = true;
		Type = 1;
		break;
	}
	case EType::Gates: {
		if (ClassName.find("FrontGate") != std::string::npos) {
			shouldAdd = true;
			Type = 1;
		}
		break;
	}
	default:
		break;
	}

	if (!shouldAdd && !Configuration::bEnableOthers)
		return;

	const FVector ActorPosition = Actor->K2_GetActorLocation();
	double Distance = 0;
	if (APlayerController* PlayerController = GetPlayerController(); PlayerController) {
		Distance = PlayerController->K2_GetPawn()->K2_GetActorLocation().GetDistanceTo(ActorPosition);
	}
	if (Name.empty()) {
		std::string tempName = CLASS_NAME_ALIAS.contains(ClassName) ? CLASS_NAME_ALIAS[ClassName] : ClassName;
		std::wstring tempWName(tempName.begin(), tempName.end());
		Name = tempWName;
	}
	ActorCacheMutex.lock();
	ActorCache.push_back({ Actor, ActorPosition, Name, ActorType, Type, Quality, Variant, Distance, Texture });
	ActorCacheMutex.unlock();
}

UClass* ActorHandler::ClassLookup(std::wstring clazz) {
	auto SpaceLocation = clazz.find_last_of(L"/");
	auto NameWithoutPath = std::wstring{ clazz.begin() + SpaceLocation + 1, clazz.end() } + L"_C";
	if (auto foundEntry = ClassPtrMap.find(NameWithoutPath); foundEntry == ClassPtrMap.end()) {
		if (NameWithoutPath == L"BP_Stables_Sign.BP_Stables_Sign_C") {
			// created before we hook, needs a lookup
			auto stable = UObject::FindClassFast("BP_Stables_Sign_C");
			if (stable) {
				AddClass(stable);
				return stable;
			}
		}
		return nullptr;
	}
	else {
		return foundEntry->second;
	}
}

UTexture2D* ActorHandler::TextureLookup(const std::string& texture) {
	/*
	if (auto foundEntry = TexturePtrMap.find(texture); foundEntry == TexturePtrMap.end()) {
		auto TexturePtr = static_cast<UTexture2D*>(LoadAssetPtrFromPath(texture));
		TexturePtrMap.emplace(texture, TexturePtr);
		return TexturePtr;
	}
	else {
		auto TexturePtr = foundEntry->second;
		if (!TexturePtr || !TexturePtr->IsValidLowLevel()) {
			auto TexturePtr = static_cast<UTexture2D*>(LoadAssetPtrFromPath(texture));
			TexturePtrMap.emplace(texture, TexturePtr);
			return TexturePtr;
		}
		if (TexturePtr->Flags & EObjectFlags::BeginDestroyed) {
			auto TexturePtr = static_cast<UTexture2D*>(LoadAssetPtrFromPath(texture));
			TexturePtrMap.emplace(texture, TexturePtr);
			return TexturePtr;
		}
		return TexturePtr;
	}*/
	return static_cast<UTexture2D*>(LoadAssetPtrFromPath(texture));
}

void ActorHandler::Tick() {
	auto timeNow = UGameplayStatics::GetTimeSeconds(GetWorld());
	if (timeNow - LastUpdate > 0.5) {
		LastUpdate = timeNow;
		UpdateActors(false);
	}
	if (timeNow - LastDormantUpdate > 15) {
		LastDormantUpdate = timeNow;
		UpdateActors(true);
	}
}

void ActorHandler::UpdateActors(bool DormantUpdate) {
	for (auto& entry : ActorCache) {
		if (DormantUpdate) {
			entry.Actor->FlushNetDormancy();
		}
		if (entry.WorldPosition.IsZero()) {
			entry.WorldPosition = entry.Actor->K2_GetActorLocation();
		}
		switch (entry.ActorType) {
		case EType::Gates:
			entry.Actor->K2_DestroyActor();
			break;
		case EType::TimedDrop:
			if (entry.Variant == 0) {
				auto TimedDropComponent = *std::bit_cast<UTimedDropComponent**>(std::bit_cast<char*>(entry.Actor) + 0x2C8); // watch dumps to make sure this offset doesn't change
				auto& Reward = TimedDropComponent->RolledRewards[0]; // should only be one reward
				auto& Item = Reward.Items[0]; // should only be one item

				if (auto ItemInfo = TimedDropLoot.find(Item.ItemPersistId); ItemInfo != TimedDropLoot.end()) {
					auto& found = ItemInfo->second;
					entry.DisplayName = std::format(L"{}x {}", Item.Amount, found.first);
					entry.TexturePath = found.second;
				}
				else {
					auto ItemDefPtr = static_cast<TSoftObjectPtr<UObject>>(UVAL_ItemTypeDefinitionAsset::GetAssetByPersistId_Cast(Item.ItemPersistId));
					auto ItemDef = static_cast<UVAL_ItemTypeDefinitionAsset*>(UKismetSystemLibrary::LoadAsset_Blocking(ItemDefPtr));
					entry.DisplayName = std::format(L"{}x {}", Item.Amount, ItemDef->DisplayName.GetStringRef().ToWString());
				}
				entry.Variant = Item.Amount;
			}
			break;
		case EType::Players:
			if (entry.DisplayName == L"BP_ValeriaCharacter_C") {
				entry.DisplayName = static_cast<AValeriaCharacter*>(entry.Actor)->CharacterName.ToWString();
			}
			break;
		default:
			break;
		}
	}
}

std::vector<FEntry> ActorHandler::GetActors() {
	if (bIsTraveling) return {};
	ActorCacheMutex.lock();
	auto newActorCache = ActorCache;
	ActorCacheMutex.unlock();
	return newActorCache;
}

std::vector<FEntry> ActorHandler::GetActorsOfType(EType type) {
	if (bIsTraveling) return {};
	std::vector<FEntry> newActorCache{};
	ActorCacheMutex.lock();
	for (auto Entry : ActorCache) {
		if (type & Entry.ActorType) {
			newActorCache.push_back(Entry);
		}
	}
	ActorCacheMutex.unlock();
	return newActorCache;
}

void ActorHandler::RemoveActor(AActor* Actor) {
	ActorCacheMutex.lock();
	std::erase_if(ActorCache, [Actor](const FEntry& Entry) {
		return Entry.Actor == Actor;
		});
	std::erase_if(Teleporters, [Actor](const AActor* Stored) {
		return Actor == Stored;
		});
	ActorCacheMutex.unlock();
}

void ActorHandler::AddClass(UClass* Class) {
	auto FullName = Class->GetFullName();
	auto SpaceLocation = FullName.find(" ");
	auto FullNameWithoutType = std::wstring{ FullName.begin() + SpaceLocation + 1, FullName.end() };
	ClassPtrMap.emplace(FullNameWithoutType, Class);
}

void ActorHandler::RemoveClass(UClass* Class) {
	std::erase_if(ClassPtrMap, [Class](const auto& Item) {
		auto const& [stringClass, classPtr] = Item;
		return classPtr == Class;
		});
}

bool ActorHandler::IsActorA(AActor* Actor, std::wstring Class) {
	UStruct* CurrentStruct = Actor->Class;
	do {
		UStruct* Struct = ClassLookup(Class);
		if (Struct && CurrentStruct == Struct)
		{
			return true;
		}
		CurrentStruct = CurrentStruct->Super;
	} while (CurrentStruct && CurrentStruct != AActor::StaticClass());
	return false;
}

std::unordered_map<std::string, AActor*> ActorHandler::GetTeleporters()
{
	std::unordered_map<std::string, AActor*>TeleMap = {};
	for (auto Actor : Teleporters) {
		auto Teleport = static_cast<AVAL_TeleporterBase*>(Actor);
		if (auto Origin = Teleport->TeleportOrigin; Origin) {
			if (auto Asset = Origin->TargetTravelAsset; Asset) {
				auto TeleName = Asset->GetName();
				if (auto ReadableName = TeleporterMap.find(TeleName); ReadableName != TeleporterMap.end()) {
					TeleName = ReadableName->second;
				}
				if (TeleName == "DO NOT USE") continue;
				TeleMap.emplace(TeleName, Actor);
			}
		}
	}
	return TeleMap;
}

