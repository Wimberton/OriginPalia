#include "VillagerDetours.h"
#include "Core/Configuration.h"
#include "Core/DetourManager.h"
#include "Misc/SDKExt.h"
#include "Misc/Utils.h"

void VillagerDetours::SetupDetours(AValeriaCharacter* VC, AValeriaPlayerController* VPC) {
    //DetourManager::AddInstance(VC);
    //DetourManager::AddFunctionListener("Function Engine.Actor.ReceiveTick", &Tick, nullptr);
    DetourManager::AddFunctionDetour("Function Palia.GiftPreferenceTrackingComponent.IsAwareOfGiftingPreference",nullptr,&Func_IsAwareOfGiftingPreference);
}

void VillagerDetours::Func_IsAwareOfGiftingPreference(void* Context, void* TheStack, void* Result) {
	if (Configuration::bEnableShowWeeklyWants) {
		*static_cast<bool*>(Result) = true;
	}
}

// NOT FINISHED
void VillagerDetours::GiftVillagers() {
	LastReport.clear();
	
	auto World = GetWorld();

	auto GameStateBase = UGameplayStatics::GetGameState(World);
	if (!GameStateBase->IsA(AValeriaGameState::StaticClass())) return;

	auto ValeriaGameState = static_cast<AValeriaGameState*>(GameStateBase);
	if (!ValeriaGameState) return;

	auto ValeriaCharacter = GetValeriaCharacter();
	if (!ValeriaCharacter) return;

	auto ValeriaController = ValeriaCharacter->GetValeriaPlayerController();
	if (!ValeriaController) return;

	auto GiftHistory = ValeriaCharacter->GetVillagerGiftHistory();
	if (!GiftHistory) return;

	auto DialogueManager = ValeriaCharacter->GetDialogueManager();
	if (!DialogueManager) return;

	auto DynamicGiftPref = ValeriaGameState->GetDynamicGiftPrefDataComponent();
	if (!DynamicGiftPref) return;

	InGiftLoop = true;

	for (auto Entry : ActorHandler::GetActorsOfType(EType::NPCs)) {
		auto Villager = static_cast<AValeriaVillagerCharacter*>(Entry.Actor);
		if (!IsActorValid(Villager)) continue;
		auto VillagerConfigID = Villager->GetVillagerConfigId();
		auto VillagerCoreID = UValeriaBlueprintFunctionLibrary::GetVillagerCoreIdByVillagerId(World, VillagerConfigID);

		std::string VillagerName(Entry.DisplayName.begin(), Entry.DisplayName.end());
		std::cout << "Villager: " << VillagerName << "\n";
		if (auto GiftReceiver = Villager->GetVillagerGiftReceiver(); GiftReceiver && GiftReceiver->IsVillagerInGiftableSpace()) {
			if (GiftReceiver->CanGiftVillager(ValeriaCharacter)) {
				std::cout << "Can Gift\n";
				for (auto ID : DynamicGiftPref->GetPreferenceIdsForVillager(VillagerCoreID)) {
					std::cout << "Gift PersistID: " << ID << "\n";
					auto GiftPreference = UValeriaBlueprintFunctionLibrary::GetVillagerGiftPreferenceConfigById(World, ID);
					auto ItemPersistID = GiftPreference.ItemType->PersistId;
					std::cout << "Item PersistID: " << ItemPersistID << "\n";
					auto SoftItem = UVAL_ItemTypeDefinitionAsset::GetAssetByPersistId_Cast(ItemPersistID);
					if (GiftHistory->AlreadyGiftedItemThisReset(VillagerCoreID, SoftItem, ValeriaCharacter)) {
						std::cout << "Already Received Gift\n";
						continue;
					}
					auto BagLocation = ValeriaCharacter->GetInventory()->FindItemByType(SoftItem, false);
					if (BagLocation.BagIndex == -1 || BagLocation.SlotIndex == -1) {
						std::cout << "Item Not In Inventory\n";
						continue;
					}
					GetValeriaController()->RpcServer_ForceEnterDialogue(Villager->GetDialogue());
					DialogueManager->RpcServer_GiveGiftTo(GiftReceiver, BagLocation);
					WaitingForDialogue = true;
					return;
				}
				LastReport.push_back(VillagerName + " - Can Gift");
				continue;
			}
			else {
				LastReport.push_back(VillagerName + " - Gifted");
				continue;
			}
		}
	}
	InGiftLoop = false;
}

void VillagerDetours::Tick(const UObject* Context, UFunction* Function, void* Parms) {
	if (InGiftLoop) {
		if (WaitingForDialogue) {
			auto ValeriaCharacter = GetValeriaCharacter();
			if (!ValeriaCharacter) return;

			auto DialogueManager = ValeriaCharacter->GetDialogueManager();
			if (!DialogueManager) return;

			if (DialogueManager->IsDialogueTextFullyDisplayed()) {
				DialogueManager->RpcServer_RequestExitDialogue();
				WaitingForClose = true;
			}
		}
		if (WaitingForClose) {
			auto ValeriaCharacter = GetValeriaCharacter();
			if (!ValeriaCharacter) return;

			auto CharacterStateMachine = ValeriaCharacter->GetCSM();
			if (!CharacterStateMachine) return;

			if (CharacterStateMachine->GetCurrentStateType() != ECharacterState::Dialogue) {
				WaitingForClose = false;
				GiftVillagers();
			}
		}
	}
}

