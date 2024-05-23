#include "PaliaOverlay.h"
#include <SDK/Palia_parameters.hpp>
#include <imgui.h>

#include<iostream>
#include <windows.h>

#include <algorithm>
#include <detours.h>
#include <RendererDetector.h>
#include <BaseHook.h>
#include <format>
#include <unordered_set>
#include <chrono>
#include <vector>
#include <unordered_map>
#include <string>
#include <cmath>
#include "ImGuiExt.h"
#include "SDKExt.h"
#include "Utils.h"
#include <HookManager.h>
#include <imgui_internal.h>
#include <tchar.h>

using namespace VectorMath;
using namespace SDK;

std::map<int, std::string> PaliaOverlay::CreatureQualityNames = {
	{0, "Unknown"},
	{1, "T1"},
	{2, "T2"},
	{3, "T3"},
	{4, "Chase"}
};
std::map<int, std::string> PaliaOverlay::BugQualityNames = {
	{0, "Unknown"},
	{1, "Common"},
	{2, "Uncommon"},
	{3, "Rare"},
	{4, "Rare2"},
	{5, "Epic"}
};
std::map<int, std::string> PaliaOverlay::GatherableSizeNames = {
	{0, "Unknown"},
	{1, "Sm"},
	{2, "Md"},
	{3, "Lg"},
	{4, "Bush"}
};

template<typename SearchType>
SearchType GetFlagSingle(std::string Text, std::map<SearchType, std::vector<std::string>>& map) {
	SearchType T = (SearchType)0;
	for (auto& Entry : map)
	{
		bool bFound = false;

		for (auto& Str : Entry.second) {
			if (Text.find(Str) != std::string::npos) {
				T = Entry.first;
				bFound = true;
				break;
			}
		}

		if (bFound)
			break;
	}
	return T;
}

template<typename SearchType>
SearchType GetFlagSingleEnd(std::string Text, std::map<SearchType, std::vector<std::string>>& map) {
	SearchType T = (SearchType)0;
	for (auto& Entry : map)
	{
		bool bFound = false;

		for (auto& Str : Entry.second) {
			if (Text.ends_with(Str)) {
				T = Entry.first;
				bFound = true;
				break;
			}
		}

		if (bFound)
			break;
	}
	return T;
}

template<typename SearchType>
SearchType GetFlagMulti(std::string Text, std::map<SearchType, std::vector<std::string>>& map) {
	SearchType T = (SearchType)0;
	for (auto& Entry : map)
	{
		for (auto& Str : Entry.second) {
			if (Text.find(Str) != std::string::npos) {
				T |= Entry.first;
			}
		}
	}
	return T;
}

template <size_t size_x>
bool AnyTrue(bool(&arr)[size_x]) {
	for (int x = 0; x < size_x; x++) {
		if (arr[x]) return true;
	}
	return false;
}

template <size_t size_x, size_t size_y>
bool AnyTrue2D(bool(&arr)[size_x][size_y]) {
	for (int x = 0; x < size_x; x++) {
		for (int y = 0; y < size_y; y++) {
			if (arr[x][y]) return true;
		}
	}
	return false;
}

template <size_t size_x, size_t size_y, size_t size_z>
bool AnyTrue3D(bool(&arr)[size_x][size_y][size_z]) {
	for (int x = 0; x < size_x; x++) {
		for (int y = 0; y < size_y; y++) {
			for (int z = 0; z < size_z; z++) {
				if (arr[x][y][z]) return true;
			}
		}
	}
	return false;
}

void PaliaOverlay::SetupColors() {
	// Forageable colors
	for (int pos : ForageableCommon) {
		ForageableColors[pos] = IM_COL32(0xCD, 0xCD, 0xCD, 0xFF); // Light Gray
	}
	for (int pos : ForageableUncommon) {
		ForageableColors[pos] = IM_COL32(0x32, 0xCD, 0x32, 0xFF); // Lime Green
	}
	for (int pos : ForageableRare) {
		ForageableColors[pos] = IM_COL32(0x1E, 0x90, 0xFF, 0xFF); // Dodger Blue
	}
	for (int pos : ForageableEpic) {
		ForageableColors[pos] = IM_COL32(0xFF, 0xD7, 0x00, 0xFF); // Gold
	}

	// Animal colors
	for (int i = 0; i < (int)ECreatureKind::MAX; i++) {
		AnimalColors[i][(int)ECreatureQuality::Tier1] = IM_COL32(0xCD, 0xCD, 0xCD, 0xFF); // Light Gray
		AnimalColors[i][(int)ECreatureQuality::Tier2] = IM_COL32(0x32, 0xCD, 0x32, 0xFF); // Lime Green
		AnimalColors[i][(int)ECreatureQuality::Tier3] = IM_COL32(0x1E, 0x90, 0xFF, 0xFF); // Dodger Blue
		AnimalColors[i][(int)ECreatureQuality::Chase] = IM_COL32(0xFF, 0xD7, 0x00, 0xFF); // Gold
	}

	// Bug colors
	for (int i = 0; i < (int)EBugKind::MAX; i++) {
		BugColors[i][(int)EBugQuality::Common] = IM_COL32(0xCD, 0xCD, 0xCD, 0xFF); // Light Gray
		BugColors[i][(int)EBugQuality::Uncommon] = IM_COL32(0x32, 0xCD, 0x32, 0xFF); // Lime Green
		BugColors[i][(int)EBugQuality::Rare] = IM_COL32(0x1E, 0x90, 0xFF, 0xFF); // Dodger Blue
		BugColors[i][(int)EBugQuality::Rare2] = IM_COL32(0x00, 0xBF, 0xFF, 0xFF); // Deep Sky Blue
		BugColors[i][(int)EBugQuality::Epic] = IM_COL32(0xFF, 0xD7, 0x00, 0xFF); // Gold
	}

	// Player & Entities colors
	SingleColors[(int)EOneOffs::Player] = IM_COL32(0xFF, 0x63, 0x47, 0xFF); // Tomato Red
	SingleColors[(int)EOneOffs::NPC] = IM_COL32(0xDE, 0xB8, 0x87, 0xFF); // Burly Wood
	SingleColors[(int)EOneOffs::Loot] = IM_COL32(0xEE, 0x82, 0xEE, 0xFF); // Violet
	SingleColors[(int)EOneOffs::Quest] = IM_COL32(0xFF, 0xA5, 0x00, 0xFF); // Orange
	SingleColors[(int)EOneOffs::RummagePiles] = IM_COL32(0xFF, 0x45, 0x00, 0xFF); // Orange Red
	SingleColors[(int)EOneOffs::Others] = IM_COL32(0xFF, 0xFF, 0xFF, 0xFF); // White

	// Define a different color for Stables
	SingleColors[(int)EOneOffs::Stables] = IM_COL32(0x8B, 0x45, 0x13, 0xFF); // Saddle Brown
};

std::vector<std::string> debugger;

#define STATIC_CLASS(CName)						\
{												\
	static class UClass* Clss = nullptr;		\
	if (!Clss || !Clss->IsValidLowLevel())		\
		Clss = UObject::FindClassFast(CName);	\
	SearchClass = Clss;							\
}
#define STATIC_CLASS_MULT(CName)				\
{												\
	static class UClass* Clss = nullptr;		\
	if (!Clss || !Clss->IsValidLowLevel())		\
		Clss = UObject::FindClassFast(CName);	\
	SearchClasses.push_back(Clss);				\
}

static void(__fastcall* OriginalProcEvent)(const UObject*, class UFunction*, void*);
static void* HookedClient = nullptr;
static void* vmt = nullptr;
static std::unordered_set<std::string> invocations;
static UFont* Roboto = nullptr;
bool SortByName(const FEntry& a, const FEntry& b) {
	return a.DisplayName < b.DisplayName;
}
bool IsGameWindowActive() {
	HWND foregroundWindow = GetForegroundWindow();
	// You may need to adjust the class name or window title to match your game
	TCHAR windowClassName[256];
	GetClassName(foregroundWindow, windowClassName, sizeof(windowClassName) / sizeof(TCHAR));
	return _tcscmp(windowClassName, TEXT("UnrealWindow")) == 0;
}

// Housing Placement Modification Logic
void SetCanPlaceHereTrue() {

	auto World = GetWorld();
	if (!World) return;

	auto GameInstance = World->OwningGameInstance;
	if (!GameInstance) return;

	if (GameInstance->LocalPlayers.Num() == 0) return;

	ULocalPlayer* LocalPlayer = GameInstance->LocalPlayers[0];
	if (!LocalPlayer) return;

	APlayerController* PlayerController = LocalPlayer->PlayerController;
	if (!PlayerController) return;

	AValeriaCharacter* ValeriaCharacter = static_cast<AValeriaPlayerController*>(PlayerController)->GetValeriaCharacter();
	if (!ValeriaCharacter) return;

	UPlacementComponent* PlacementComponent = ValeriaCharacter->GetPlacement();
	if (!PlacementComponent) return;

	// Set CanPlaceHere to true
	PlacementComponent->CanPlaceHere = true;
}

// Logic for Updating InteliFOV information
void UpdateInteliAim(float FOVRadius, UWorld* World, AValeriaPlayerController* ValeriaController, APawn* PlayerPawn, AValeriaCharacter* Character) {
	auto Overlay = dynamic_cast<PaliaOverlay*>(OverlayBase::Instance);
	
	FVector PawnLocation = PlayerPawn->K2_GetActorLocation();
	FRotator PawnRotation = PlayerPawn->GetControlRotation();
	FVector ForwardVector = UKismetMathLibrary::GetForwardVector(PawnRotation);
	double BestScore = FLT_MAX;  // Using a scoring system based on various factors such as distance, area fov, prediction

	// FVector BestTargetLocation;
	// FRotator BestTargetRotation;
	//
	// FVector AdjustedTargetLocation;
	// FVector DirectionToBestActor;
	// FRotator NewRotation;

	for (auto& [Actor, WorldPosition, DisplayName, ActorType, Type, Quality, Variant, shouldAdd] : Overlay->CachedActors) {
		if (!Actor) continue;
		if (!Actor->IsValidLowLevel() || Actor->IsDefaultObject()) continue;

		bool bShouldConsider = false;

		switch (ActorType) {
		case EType::Animal:
			bShouldConsider = Overlay->Animals[Type][Variant]; // Toggle for different types of animals
			break;
		case EType::Ore:
			bShouldConsider = Overlay->Ores[Type][Variant]; // Toggle for different types of ores
			break;
		case EType::Bug:
			bShouldConsider = Overlay->Bugs[Type][Variant][Quality]; // Toggle for different types of bugs
			break;
		case EType::Forage:
			bShouldConsider = Overlay->Forageables[Type][Quality]; // Toggle for forageable items
			break;
		case EType::Players:
			bShouldConsider = Overlay->Singles[static_cast<int>(EOneOffs::Player)]; // Toggle for player visibility
			break;
		case EType::NPCs:
			bShouldConsider = Overlay->Singles[static_cast<int>(EOneOffs::NPC)]; // Toggle for NPCs
			break;
		case EType::Quest:
			bShouldConsider = Overlay->Singles[static_cast<int>(EOneOffs::Quest)]; // Toggle for quest items
			break;
		case EType::Loot:
			bShouldConsider = Overlay->Singles[static_cast<int>(EOneOffs::Loot)]; // Toggle for loot
			break;
		case EType::RummagePiles:
			bShouldConsider = Overlay->Singles[static_cast<int>(EOneOffs::RummagePiles)]; // Toggle for RummagePiles
			break;
		case EType::Stables:
			bShouldConsider = Overlay->Singles[static_cast<int>(EOneOffs::Stables)]; // Toggle for Stables
			break;
		case EType::Tree:
			bShouldConsider = Overlay->Trees[Type][Variant]; // Toggle for trees
			break;
		case EType::Fish:
			bShouldConsider = Overlay->Fish[Type]; // Toggle for fish types
			break;
		default:
			break;
		}

		if (!bShouldConsider) continue;

		FVector ActorLocation = Actor->K2_GetActorLocation();
		FVector DirectionToActor = (ActorLocation - PawnLocation).GetNormalized();
		FVector TargetVelocity = Actor->GetVelocity();
		FVector RelativeVelocity = TargetVelocity - PlayerPawn->GetVelocity();
		FVector RelativeDirection = RelativeVelocity.GetNormalized();
			
		double Distance = PawnLocation.GetDistanceToInMeters(ActorLocation);
		double Angle = CustomMath::RadiansToDegrees(acosf(ForwardVector.Dot(DirectionToActor)));
		double TimeToTarget = Distance / 343.0;
		FVector PredictedLocation = ActorLocation + TargetVelocity * TimeToTarget; // Predictive aiming

		if (ActorLocation.X == 0 && ActorLocation.Y == 0 && ActorLocation.Z == 0) continue;
		// double Distance = sqrt(pow(PawnLocation.X - ActorLocation.X, 2) + pow(PawnLocation.Y - ActorLocation.Y, 2) + pow(PawnLocation.Z - ActorLocation.Z, 2)) * 0.01;

		if (Distance < 2.0) continue;
		if (Overlay->bEnableESPCulling && Distance > Overlay->CullDistance) continue;

		// Weighting factors for different factors
		double AngleWeight = 0.5, DistanceWeight = 0.3, MovementWeight = 0.0;
			
		// Adjust weighting factors based on EType
		switch (ActorType) {
		case EType::Animal:
			AngleWeight = 0.10;
			DistanceWeight = 0.0;
			MovementWeight = 0.0;
			break;
		case EType::Ore:
			AngleWeight = 0.10;
			DistanceWeight = 0.0;
			MovementWeight = 0.0;
			break;
		case EType::Bug:
			AngleWeight = 0.10;
			DistanceWeight = 0.0;
			MovementWeight = 0.0;
			break;
		default:
			AngleWeight = 0.10;
			DistanceWeight = 0.0;
			MovementWeight = 0.0;
			break;
		}
			
		// Calculate score based on weighted sum of factors
		// double Score = (AngleWeight * Angle) + (DistanceWeight * Distance / 100.0) + (MovementWeight * RelativeDirection.Magnitude());
		double Score = (AngleWeight * Angle) + (DistanceWeight * Distance) + (MovementWeight * RelativeDirection.Magnitude());

		if (Angle <= FOVRadius / 2.0 && Score < Overlay->SelectionThreshold) {
			if (Score < BestScore) {
				BestScore = Score;
				FRotator CurrentRotation = ValeriaController->GetControlRotation();
				FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(PawnLocation, ActorLocation);
				FRotator NewRotation = CustomMath::RInterpTo(CurrentRotation, TargetRotation, UGameplayStatics::GetWorldDeltaSeconds(World), Overlay->SmoothingFactor);
				Overlay->BestTargetActor = Actor;
				Overlay->BestTargetActorType = ActorType;
				Overlay->BestTargetLocation = ActorLocation;
				Overlay->BestTargetRotation = UKismetMathLibrary::FindLookAtRotation(PawnLocation, ActorLocation);
			}
		}
	}

	if (Overlay->bTeleportToTargeted) {
		auto now = std::chrono::steady_clock::now();
		if (IsKeyHeld(VK_XBUTTON2) && BestScore != FLT_MAX) {
			if (std::chrono::duration_cast<std::chrono::seconds>(now - Overlay->LastTeleportToTargetTime).count() >= 2) {
				bool shouldTeleport = true;

				// Avoid teleporting to players if configured to
				if (Overlay->bAvoidTeleportingToPlayers && Overlay->BestTargetActorType == EType::Players) {
					shouldTeleport = false;
				}

				if (shouldTeleport) {
					// Teleportation logic
					FVector TargetLocation = Overlay->BestTargetLocation;
					FVector ActorLocation = PlayerPawn->K2_GetActorLocation();

					FVector Direction = (TargetLocation - ActorLocation).GetNormalized(); // UKismetMathLibrary, calculate cross product
					FVector UpVector = FVector(0, 0, 1); // Z-axis up vector
					//FVector SideOffset = UKismetMathLibrary::Cross_VectorVector(Direction, UpVector).GetNormalized() * 100.0f;

					TargetLocation.Z += 150.0f; // Raise by 150 units in the Z direction
					//FVector NewLocation = TargetLocation + SideOffset;

					FHitResult HitResult;
					PlayerPawn->K2_SetActorLocation(TargetLocation, false, &HitResult, true);
					Overlay->LastTeleportToTargetTime = now;
				}
			}
		}
	}

	// Don't aimbot while the overlay is showing
	if (Overlay->bEnableAimbot && !Overlay->ShowOverlay()) {
		if (IsKeyHeld(VK_LBUTTON) && BestScore != FLT_MAX) {
			// Only aimbot when a bow is equipped
			bool bowEquipped = Character->GetEquippedItem().ItemType->Name.ToString().find("Tool_Bow_") != std::string::npos;
			if (bowEquipped) {
				bool IsAnimal = false;
				for (FEntry& Entry : Overlay->CachedActors) {
					if (Entry.shouldAdd && Entry.ActorType == EType::Animal && Entry.Actor && Entry.Actor->IsValidLowLevel() && !Entry.Actor->IsDefaultObject()) {
						FVector ActorLocation = Entry.Actor->K2_GetActorLocation();
						if (ActorLocation == Overlay->BestTargetLocation) {
							IsAnimal = true;
							break;
						}
					}
				}
				// Adjust the aim rotation only if the selected best target is an animal
				if (IsAnimal) {
					// Apply offset to pitch and yaw directly
					float DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(World);
					FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(PawnLocation, Overlay->BestTargetLocation);
					TargetRotation.Pitch += Overlay->AimOffset.X;
					TargetRotation.Yaw += Overlay->AimOffset.Y;

					// Smooth rotation adjustment
					float InvertedSmoothing = 100.0f / Overlay->SmoothingFactor;
					FRotator NewRotation = CustomMath::RInterpTo(PawnRotation, TargetRotation, UGameplayStatics::GetTimeSeconds(World), Overlay->SmoothingFactor);
					// FRotator NewRotation = CustomMath::RInterpTo(PawnRotation, TargetRotation, DeltaTime, InvertedSmoothing);
					ValeriaController->SetControlRotation(NewRotation);
				}
			}
		}
	}
}

// Creation of the InteliFOV Circle
void DrawCircle(UCanvas* Canvas, FVector2D Center, float Radius, int32 NumSegments, FLinearColor Color, float Thickness = 1.0f) {
	// Calculate screen center more accurately
	FVector2D ScreenCenter(Canvas->ClipX / 2, Canvas->ClipY / 2);

	float Increment = 360.0f / NumSegments;
	FVector2D LastPos = FVector2D(ScreenCenter.X + Radius, ScreenCenter.Y);

	for (int i = 1; i <= NumSegments; i++) {
		float Rad = CustomMath::DegreesToRadians(Increment * i);
		FVector2D NewPos = FVector2D(ScreenCenter.X + Radius * cos(Rad), ScreenCenter.Y + Radius * sin(Rad));
		Canvas->K2_DrawLine(LastPos, NewPos, Thickness, Color);
		LastPos = NewPos;
	}
}

// Function to clear cache based on the game state
void ClearActorCache(PaliaOverlay* Overlay, const UWorld* World) {
	// Clear cache on level change
	if (Overlay->CurrentLevel != World->PersistentLevel) {
		Overlay->CachedActors.clear();
		Overlay->CurrentLevel = World->PersistentLevel;
		Overlay->CurrentMap = UGameplayStatics::GetCurrentLevelName(World, false).ToString();
	}
}

// Function to manage cache outside of general functions
void ManageActorCache(PaliaOverlay* Overlay, const UWorld* World) {
	if (const double WorldTime = UGameplayStatics::GetTimeSeconds(World); abs(WorldTime - Overlay->LastCachedTime) > 0.1) {
		Overlay->LastCachedTime = WorldTime;
		Overlay->ProcessActors(Overlay->ActorStep);

		Overlay->ActorStep++;
		if (Overlay->ActorStep >= static_cast<int>(EType::MAX)) {
			Overlay->ActorStep = 0;
		}
	}
}

// Function to convert std::vector<FEntry> to TArray<FEntry>
TArray<FEntry> ConvertToTArray(const std::vector<FEntry>& VectorEntries) {
	TArray<FEntry> TArrayEntries;
	for (const auto& Entry : VectorEntries) {
		TArrayEntries.Add(Entry);
	}
	return TArrayEntries;
}


// HACK LOGICS //
void Func_DoRemoveGates(const PaliaOverlay* Overlay, UWorld* World) {
	if (!Overlay->bRemoveGates) return;
	
	std::vector<UClass*> SearchClasses;

	STATIC_CLASS_MULT("BP_Stables_FrontGate_01_C")
	STATIC_CLASS_MULT("BP_Stables_FrontGate_02_C")

	for (const std::vector<AActor*> Actors = FindAllActorsOfTypes(World, SearchClasses); AActor* Actor : Actors) {
		FVector GatePurgatory = {0.0, 0.0, -2000.0};
		FHitResult HitResult;
		Actor->K2_SetActorLocation(GatePurgatory, false, &HitResult, true);
	}
}

void Func_DoInteliAim(const PaliaOverlay* Overlay, UWorld* World, AValeriaPlayerController* const ValeriaController, APawn* PlayerPawn, AValeriaCharacter* Character) {
	if (!Overlay->bEnableAimbot && !Overlay->bDrawFOVCircle) return;

	UpdateInteliAim(Overlay->FOVRadius, World, ValeriaController, PlayerPawn, Character);
}

void Func_DoPersistentMovement(const PaliaOverlay* Overlay, const AValeriaCharacter* Character) {
	if (!Overlay->bEnablePersistantMovement) return;
	
	UValeriaCharacterMoveComponent* MovementComponent = Character->GetValeriaCharacterMovementComponent();
	if (!MovementComponent) return;

	if (MovementComponent->IsValidLowLevel() && !MovementComponent->IsDefaultObject()) {
		MovementComponent->MaxWalkSpeed = Overlay->CustomWalkSpeed;
		MovementComponent->SprintSpeedMultiplier = Overlay->CustomSprintSpeedMultiplier;
		MovementComponent->ClimbingSpeed = Overlay->CustomClimbingSpeed;
		MovementComponent->GlidingMaxSpeed = Overlay->CustomGlidingSpeed;
		MovementComponent->GlidingFallSpeed = Overlay->CustomGlidingFallSpeed;
		MovementComponent->JumpZVelocity = Overlay->CustomJumpVelocity;
		MovementComponent->MaxStepHeight = Overlay->CustomMaxStepHeight;
	}
}

void Func_DoESP(PaliaOverlay* Overlay, const AHUD* HUD, AValeriaPlayerController* const ValeriaController, APawn* PlayerPawn, AValeriaCharacter* Character) {
	if (Overlay->bEnableESP) {
		FVector PawnLocation = PlayerPawn->K2_GetActorLocation();

		// Draw ESP Names Entities
		for (auto& [Actor, WorldPosition, DisplayName, ActorType, Type, Quality, Variant, shouldAdd] : Overlay->CachedActors) {
			FVector ActorPosition = WorldPosition;
			if (ActorType == EType::Animal || ActorType == EType::Bug || ActorType == EType::Players || ActorType == EType::Loot) {
				if (!Actor) continue;
				if (!Actor->IsValidLowLevel() || Actor->IsDefaultObject()) continue;
				ActorPosition = Actor->K2_GetActorLocation();
			}

			// Shouldn't this be above the ActorPosition.Z change?? (Changed for now)
			// HACK: Skip actors that return [0,0,0] due to the hack I had to add to K2_GetActorLocation
			if (ActorPosition.X == 0 && ActorPosition.Y == 0 && ActorPosition.Z == 0) continue;
			
			// Adjust Z coordinate for head-level display
			float HeightAdjustment = 100.0f; // Adjust this value based on typical actor height
			ActorPosition.Z += HeightAdjustment;

			double Distance = sqrt(pow(PawnLocation.X - ActorPosition.X, 2) + pow(PawnLocation.Y - ActorPosition.Y, 2) + pow(PawnLocation.Z - ActorPosition.Z, 2)) * 0.01;

			if (Distance < 2.0) continue;
			if (Overlay->bEnableESPCulling && Distance > Overlay->CullDistance) continue;

			// Teleport loot to player mod
			if (Overlay->bEnableLootbagTeleportation && ActorType == EType::Loot) {
				FHitResult HitResult;
				Actor->K2_SetActorLocation(PawnLocation, true, &HitResult, true);
			}

			FVector2D ScreenLocation;
			if (ValeriaController->ProjectWorldLocationToScreen(ActorPosition, &ScreenLocation, true)) {

				ImU32 Color = IM_COL32(0xFF, 0xFF, 0xFF, 0xFF);
				bool bShouldDraw = false;

				switch (ActorType) {
				case EType::Forage:
					if (Overlay->Forageables[Type][Quality]) {
						bShouldDraw = true;
						Color = Overlay->ForageableColors[Type];
					}
					break;
				case EType::Ore:
					if (Overlay->Ores[Type][Variant]) {
						bShouldDraw = true;
						Color = Overlay->OreColors[Type];
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
					}
					break;
				case EType::Bug:
					if (Overlay->Bugs[Type][Variant][Quality]) {
						bShouldDraw = true;
						Color = Overlay->BugColors[Type][Variant];
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
						if (auto Pile = static_cast<ATimedLootPile*>(Actor)) {
							if (Pile->CanGather(Character) && Pile->bActivated) {
								bShouldDraw = true;
								Color = Overlay->SingleColors[static_cast<int>(EOneOffs::RummagePiles)];
							}
							else if (Overlay->bVisualizeDefault) {
								bShouldDraw = true;
								if (Pile->bActivated) {
									Color = IM_COL32(0xFF, 0xFF, 0xFF, 0xFF);
								}
								else {
									Color = IM_COL32(0xFF, 0x00, 0x00, 0xFF);
								}
							}
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
				default:
					break;
				}

				if (Overlay->bVisualizeDefault && Type == 0) bShouldDraw = true;
				if (!bShouldDraw) continue;
				if (!Roboto) {
					Roboto = reinterpret_cast<UFont*>(UObject::FindObject("Font Roboto.Roboto", EClassCastFlags::None));
				}
				if (!Roboto) continue;

				// Construct text string
				std::string qualityName = (Quality > 0) ? PaliaOverlay::GetQualityName(Quality, ActorType) : "";

				// Prepare text with optional parts depending on the index values
				std::string text = DisplayName;
				if (!qualityName.empty()) {
					text += " [" + qualityName + "]";
				}
				text += std::format(" [{:.2f}m]", Distance);
				std::wstring wideText(text.begin(), text.end());

				double BaseScale = 1.0; // Default scale at a reference distance
				double ReferenceDistance = 100.0; // Distance at which no scaling is applied
				double ScalingFactor = 0.005; // Determines how much the scale changes with distance

				double DistanceScale;
				DistanceScale = BaseScale - ScalingFactor * (Distance - ReferenceDistance);
				DistanceScale = CustomMath::Clamp(DistanceScale, 0.5, BaseScale); // Clamp the scale to a reasonable range

				const FVector2D TextScale = {DistanceScale, DistanceScale};
				ImColor IMC(Color);
				FLinearColor TextColor = { IMC.Value.x, IMC.Value.y, IMC.Value.z, IMC.Value.w };

				// Setup shadow properties
				ImColor IMCS(Color);
				FLinearColor ShadowColor = { IMCS.Value.x, IMCS.Value.y, IMCS.Value.z, IMCS.Value.w };

				// Calculate positions
				FVector2D TextPosition = ScreenLocation;
				FVector2D ShadowPosition = {TextPosition.X + 1.0, TextPosition.Y + 1.0};

				// Draw shadow text
				HUD->Canvas->K2_DrawText(Roboto, FString(wideText.data()), ShadowPosition, TextScale, TextColor, 0, { 0,0,0,1 }, FVector2D(1.0f, 1.0f), true, true, true, { 0,0,0,1 });

				// Draw main text
				HUD->Canvas->K2_DrawText(Roboto, FString(wideText.data()), TextPosition, TextScale, ShadowColor, 0, { 0,0,0,1 }, FVector2D(1.0f, 1.0f), true, true, true, { 0,0,0,1 });
			}
		}

		// Logic for FOV and Targeting Drawing
		if (Overlay->bDrawFOVCircle) {
			FVector2D PlayerScreenPosition;
			FVector2D TargetScreenPosition;

			if (ValeriaController->ProjectWorldLocationToScreen(PawnLocation, &PlayerScreenPosition, true)) {
				// Calculate the center of the FOV circle based on the player's screen position
				FVector2D FOVCenter = {HUD->Canvas->ClipX * 0.5f, HUD->Canvas->ClipY * 0.5f};
				DrawCircle(HUD->Canvas, FOVCenter, Overlay->FOVRadius, 1200, FLinearColor(0.485, 0.485, 0.485, 0.485), 1.0f);

				if (!Overlay->BestTargetLocation.IsZero()) {

					if (ValeriaController->ProjectWorldLocationToScreen(Overlay->BestTargetLocation, &TargetScreenPosition, true)) {
						if (CustomMath::DistanceBetweenPoints(TargetScreenPosition, FOVCenter) <= Overlay->FOVRadius) {
							HUD->Canvas->K2_DrawLine(FOVCenter, TargetScreenPosition, 0.5f, FLinearColor(0.485, 0.485, 0.485, 0.485));
						}
					}
				}
			}
		}
	} else {
		Overlay->CachedActors.clear();
	}
}

void Func_DoAutoFishing(const PaliaOverlay* Overlay, AValeriaCharacter* Character) {
	if (!Overlay->bEnableAutoFishing) return;
	
	if (Character && Character->GetEquippedItem().ItemType->IsFishingRod()) {
		if (!Overlay->bRequireClickFishing || IsKeyHeld(VK_LBUTTON)) {
			Character->ToolPrimaryActionPressed();
			Character->ToolPrimaryActionReleased();
		}
	}
}

void Func_DoInstantFishing(const PaliaOverlay* Overlay, AValeriaPlayerController* const ValeriaController, const AValeriaCharacter* Character) {
	if (!Overlay->bEnableInstantFishing) return;
	
	UFishingComponent* FishingComponent = Character->GetFishing();
	if (!FishingComponent) return;

	const auto FishingState = static_cast<EFishingState_NEW>(FishingComponent->GetFishingState());
	if (FishingState == EFishingState_NEW::Bite) {
		FFishingEndContext Context;
		Context.Result = EFishingMiniGameResult::Success;
		Context.Perfect = Overlay->bPerfectCatch;
		Context.DurabilityReduction = 0;
		Context.SourceWaterBody = nullptr;
		Context.bUsedMultiplayerHelp = false;
		Context.StartRodHealth = Overlay->StartRodHealth;
		Context.EndRodHealth = Overlay->EndRodHealth;
		Context.StartFishHealth = Overlay->StartFishHealth;
		Context.EndFishHealth = Overlay->EndFishHealth;

		FishingComponent->RpcServer_EndFishing(Context);
		FishingComponent->SetFishingState(EFishingState_OLD::None);

		if (Overlay->bDoInstantSellFish) {
			UVillagerStoreComponent* StoreComponent = Character->StoreComponent;
			UInventoryComponent* InventoryComponent = Character->GetInventory();

			if (StoreComponent && InventoryComponent) {
				for (int BagIndex = 0; BagIndex < InventoryComponent->Bags.Num(); BagIndex++) {
					for (int SlotIndex = 0; SlotIndex < 8; SlotIndex++) {
						FBagSlotLocation Slot{ BagIndex, SlotIndex };
						FValeriaItem Item = InventoryComponent->GetItemAt(Slot);
						if (Item.ItemType->Category == EItemCategory::Fish || Item.ItemType->Category == EItemCategory::Junk) {
							StoreComponent->RpcServer_SellItem(Slot, 10);
						}
					}
				}
			}
		}
		
		if (Overlay->bDestroyCustomizationFishing) {
			ValeriaController->DiscardItem(FBagSlotLocation{ .BagIndex = 0, .SlotIndex = 0 }, 1);
		}
	}

	//else if (FishingState == EFishingState_NEW::None || FishingState == EFishingState_NEW::EFishingState_MAX) 
}

void Func_DoNoClip(PaliaOverlay* Overlay, AValeriaPlayerController* const ValeriaController, AValeriaCharacter* Character) {
	if (Overlay->bEnableNoclip != Overlay->bPreviousNoclipState) {

		UValeriaCharacterMoveComponent* MovementComponent = Character->GetValeriaCharacterMovementComponent();
		if (!MovementComponent) return;

		if (Overlay->bEnableNoclip) {
			MovementComponent->SetMovementMode(EMovementMode::MOVE_Flying, 5);
			Character->CapsuleComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		} else {
			MovementComponent->SetMovementMode(EMovementMode::MOVE_Walking, 1);
			Character->CapsuleComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
			Character->CapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
		}
	}

	Overlay->bPreviousNoclipState = Overlay->bEnableNoclip;

	// Logic for Noclip Camera
	if (Overlay->bEnableNoclip) {
		if (!IsGameWindowActive()) return;
		
		UValeriaCharacterMoveComponent* MovementComponent = Character->GetValeriaCharacterMovementComponent();
		if (!MovementComponent) return;

		// Calculate forward and right vectors based on the camera's yaw
		const FRotator& CameraRot = ValeriaController->PlayerCameraManager->GetCameraRotation();

		//float YawInRadians = CustomMath::DegreesToRadians(CameraRot.Yaw);

		FVector CameraForward = UKismetMathLibrary::GetForwardVector(CameraRot);
		FVector CameraRight = UKismetMathLibrary::GetRightVector(CameraRot);
		constexpr FVector CameraUp = {0.f, 0.f, 1.f};

		CameraForward.Normalize();
		CameraRight.Normalize();

		FVector MovementDirection= {0.f, 0.f, 0.f};
		const float FlySpeed = 800.0f;

		if (IsKeyHeld('W')) {
			MovementDirection += CameraForward * FlySpeed;
		}
		if (IsKeyHeld('S')) {
			MovementDirection -= CameraForward * FlySpeed;
		}
		if (IsKeyHeld('D')) {
			MovementDirection += CameraRight * FlySpeed;
		}
		if (IsKeyHeld('A')) {
			MovementDirection -= CameraRight * FlySpeed;
		}
		if (IsKeyHeld(VK_SPACE)) {
			MovementDirection += CameraUp * FlySpeed;
		}
		if (IsKeyHeld(VK_CONTROL)) {
			MovementDirection -= CameraUp * FlySpeed;
		}

		// Normalize the total movement direction
		MovementDirection.Normalize();
		MovementDirection *= FlySpeed;

		// Time delta
		const float DeltaTime = 1.0f / 60.0f;  // Assuming 60 FPS

		const FVector MovementDelta = MovementDirection * DeltaTime;

		// Update character position
		FHitResult HitResult;
		Character->K2_SetActorLocation(Character->K2_GetActorLocation() + MovementDelta, false, &HitResult, false);
	}
}

void DrawHUD(const AHUD* HUD, UWorld* World, AValeriaPlayerController* const ValeriaController, APawn* PlayerPawn, AValeriaCharacter* Character) {
	const auto Overlay = dynamic_cast<PaliaOverlay*>(OverlayBase::Instance);
	
	// Remove Gates Logic
	Func_DoRemoveGates(Overlay, World);

	// Manage Cache Logic
	ManageActorCache(Overlay, World);
	ClearActorCache(Overlay, World);

	// Persistent Movement Logic
	Func_DoPersistentMovement(Overlay, Character);

	// Logic for ESP Drawing & FOV Circle/Line
	Func_DoESP(Overlay, HUD, ValeriaController, PlayerPawn, Character);

	// Logic For InteliTargeting Updates (FOV)
	Func_DoInteliAim(Overlay, World, ValeriaController, PlayerPawn, Character);

	// Auto Fishing Logic
	Func_DoAutoFishing(Overlay, Character);

	// Logic for Fishing-Related Actions
	Func_DoInstantFishing(Overlay, ValeriaController, Character);

	// Logic for Noclip
	Func_DoNoClip(Overlay, ValeriaController, Character);
	
	// Housing Place Anywhere Logic
	if (Overlay->bPlaceAnywhere) {
		SetCanPlaceHereTrue();
	}
}

void PaliaOverlay::ProcessActors(int step) {
	CachedActors.erase(
		std::remove_if(
			CachedActors.begin(), CachedActors.end(),
			[step](FEntry Entry) { return (int)Entry.ActorType == step; }),
		CachedActors.end()
	);

	auto World = GetWorld();

	auto ActorType = static_cast<EType>(step);
	std::vector<AActor*> Actors;
	UClass* SearchClass = nullptr;
	std::vector<UClass*> SearchClasses;

	switch (ActorType) {
	case EType::Tree: if (AnyTrue2D(Trees)) { STATIC_CLASS("BP_ValeriaGatherableLoot_Lumber_C") }
		break;
	case EType::Ore: if (AnyTrue2D(Ores)) { STATIC_CLASS("BP_ValeriaGatherableLoot_Mining_Base_C") }
		break;
	case EType::Bug: if (AnyTrue3D(Bugs)) { STATIC_CLASS("BP_ValeriaBugCatchingCreature_C") }
		break;
	case EType::Animal: if (AnyTrue2D(Animals)) { STATIC_CLASS("BP_ValeriaHuntingCreature_C") }
		break;
	case EType::Forage: if (AnyTrue2D(Forageables)) { STATIC_CLASS("BP_Valeria_Gatherable_Placed_C") }
		break;
	case EType::Loot:
		if (Singles[static_cast<int>(EOneOffs::Loot)] || bEnableLootbagTeleportation) {
			STATIC_CLASS("BP_Loot_C")
		}
		break;
	case EType::Players:
		if (Singles[static_cast<int>(EOneOffs::Player)]) {
			SearchClass = AValeriaCharacter::StaticClass();
		}
		break;
	case EType::NPCs:
		if (Singles[static_cast<int>(EOneOffs::NPC)]) {
			SearchClass = AValeriaVillagerCharacter::StaticClass();
		}
		break;
	case EType::Quest:
		if (Singles[static_cast<int>(EOneOffs::Quest)]) {
			STATIC_CLASS_MULT("BP_SimpleInspect_Base_C")
			STATIC_CLASS_MULT("BP_QuestInspect_Base_C")
			STATIC_CLASS_MULT("BP_QuestItem_BASE_C")
		}
		break;
	case EType::RummagePiles:
		if (Singles[static_cast<int>(EOneOffs::RummagePiles)]) {
			STATIC_CLASS_MULT("BP_BeachPile_C")
			STATIC_CLASS_MULT("BP_ChapaaPile_C")
		}
		break;
	case EType::Stables:
		if (Singles[static_cast<int>(EOneOffs::Stables)]) {
			STATIC_CLASS_MULT("BP_Stables_Sign_C")
		}
		break;
	case EType::Fish:
		if (AnyTrue(Fish)) {
			STATIC_CLASS_MULT("BP_WaterPlane_Fishing_Base_SQ_C")
			STATIC_CLASS_MULT("BP_Minigame_Fish_C")
		}
		break;
	default:
		break;
	}

	if (SearchClass) {
		if (ActorType == EType::RummagePiles || ActorType == EType::Stables) {
			Actors = FindAllActorsOfType(World, SearchClass);
		}
		else { Actors = FindActorsOfType(World, SearchClass); }
	}

	if (!SearchClasses.empty()) {
		if (ActorType == EType::RummagePiles || ActorType == EType::Stables) {
			Actors = FindAllActorsOfTypes(World, SearchClasses);
		}
		else { Actors = FindActorsOfTypes(World, SearchClasses); }
	}

	for (AActor* Actor : Actors) {
		if (!Actor || !Actor->IsValidLowLevel() || Actor->IsDefaultObject())
			continue;

		FVector ActorPosition = Actor->K2_GetActorLocation();
		if (ActorPosition.X == 0 && ActorPosition.Y == 0 && ActorPosition.Z == 0)
			continue;

		auto ClassName = Actor->Class->GetName();

		int Type = 0;
		int Quality = 0;
		int Variant = 0;

		bool shouldAdd = false;

		switch (ActorType)
		{
		case EType::Tree:
		{
			auto Tree = ETreeType::Unknown;
			Tree = GetFlagSingle(ClassName, TREE_TYPE_MAPPINGS);
			if (Tree != ETreeType::Unknown) {
				auto Size = EGatherableSize::Unknown;
				Size = GetFlagSingle(ClassName, GATHERABLE_SIZE_MAPPINGS);
				if (Size != EGatherableSize::Unknown) {
					shouldAdd = true;
					Type = static_cast<int>(Tree);
					Variant = static_cast<int>(Size);
				}
			}
		}
		break;
		case EType::Ore:
		{
			auto Ore = EOreType::Unknown;
			Ore = GetFlagSingle(ClassName, MINING_TYPE_MAPPINGS);
			if (Ore != EOreType::Unknown) {
				auto Size = EGatherableSize::Unknown;
				Size = GetFlagSingle(ClassName, GATHERABLE_SIZE_MAPPINGS);
				if (Ore == EOreType::Clay) Size = EGatherableSize::Large;
				if (Size != EGatherableSize::Unknown) {
					shouldAdd = true;
					Type = static_cast<int>(Ore);
					Variant = static_cast<int>(Size);
				}
			}
		}
		break;
		case EType::Bug:
		{
			auto Bug = EBugKind::Unknown;
			Bug = GetFlagSingle(ClassName, CREATURE_BUGKIND_MAPPINGS);
			if (Bug != EBugKind::Unknown) {
				auto BVar = EBugQuality::Unknown;
				BVar = GetFlagSingleEnd(ClassName, CREATURE_BUGQUALITY_MAPPINGS);
				if (BVar != EBugQuality::Unknown) {
					shouldAdd = true;
					Type = static_cast<int>(Bug);
					Variant = static_cast<int>(BVar);
					if (ClassName.ends_with("+_C")) {
						Quality = 1;
					}
				}
			}
		}
		break;
		case EType::Animal:
		{
			auto CK = ECreatureKind::Unknown;
			CK = GetFlagSingle(ClassName, CREATURE_KIND_MAPPINGS);
			if (CK != ECreatureKind::Unknown) {
				auto CQ = ECreatureQuality::Unknown;
				CQ = GetFlagSingleEnd(ClassName, CREATURE_KINDQUALITY_MAPPINGS);
				if (CQ != ECreatureQuality::Unknown) {
					shouldAdd = true;
					Type = static_cast<int>(CK);
					Variant = static_cast<int>(CQ);
				}
			}
		}
		break;
		case EType::Forage:
		{
			if (!Actor->bActorEnableCollision) continue;
			auto Forage = EForageableType::Unknown;
			Forage = GetFlagSingle(ClassName, FORAGEABLE_TYPE_MAPPINGS);
			if (Forage != EForageableType::Unknown) {
				shouldAdd = true;
				Type = static_cast<int>(Forage);
				if (ClassName.ends_with("+_C")) {
					Quality = 1;
				}
			}
		}
		break;
		case EType::Loot:
			shouldAdd = true;
			Type = 1; // doesn't matter, but isn't "unknown"
			break;
		case EType::Players:
		{
			shouldAdd = true;
			Type = 1; // doesn't matter, but isn't "unknown"
			auto VActor = static_cast<AValeriaCharacter*>(Actor);
			ClassName = VActor->CharacterName.ToString();
		}
		break;
		case EType::NPCs:
			shouldAdd = true;
			Type = 1; // doesn't matter, but isn't "unknown"
			break;
		case EType::Quest:
			if (!Actor->bActorEnableCollision) continue;
			shouldAdd = true;
			Type = 1;
			break;
		case EType::RummagePiles:
			shouldAdd = true;
			Type = 1;
			break;
		case EType::Stables:
			shouldAdd = true;
			Type = 1;
			break;
		case EType::Fish:
		{
			auto Fish = EFishType::Unknown;
			Fish = GetFlagSingle(ClassName, FISH_TYPE_MAPPINGS);
			if (Fish != EFishType::Unknown) {
				shouldAdd = true;
				Type = static_cast<int>(Fish);
			}
		}
		break;
		default:
			break;
		};

		if (!shouldAdd && !bVisualizeDefault) continue;
		std::string Name = CLASS_NAME_ALIAS.contains(ClassName) ? CLASS_NAME_ALIAS[ClassName] : ClassName;
		CachedActors.push_back({Actor, ActorPosition, Name, ActorType, Type, Quality, Variant, shouldAdd});
	}
}

static void ProcessEventDetour(const UObject* Class, class UFunction* Function, void* Params) {
	auto Overlay = dynamic_cast<PaliaOverlay*>(OverlayBase::Instance);
	auto fn = Function->GetFullName();
	invocations.insert(fn);

	// Gotta put this in it's own function, and only update it when needed (like in an ingame function e.g: ReceiveDrawHUD)
	const auto World = GetWorld();
	if (!World) return;

	const auto GameplayStatics = static_cast<UGameplayStatics*>(UGameplayStatics::StaticClass()->DefaultObject);
	if (!GameplayStatics) return;
	
	const auto GameInstance = World->OwningGameInstance;
	if (!GameInstance || GameInstance->LocalPlayers.Num() == 0) return;

	APlayerController* PlayerController = GameInstance->LocalPlayers[0]->PlayerController;
	if (!PlayerController) return;
	
	const auto ValeriaController = static_cast<AValeriaPlayerController*>(PlayerController);
	if (!ValeriaController) return;

	// Prevents crashing, can't get GetValeriaCharacter if you can't get K2_GetPawn?
	APawn* ValeriaPawn = ValeriaController->K2_GetPawn();
	if (!ValeriaPawn) return;
	
	AValeriaCharacter* ValeriaCharacter = ValeriaController->GetValeriaCharacter();
	if (!ValeriaCharacter) return;
	
	// Custom Tick-Safe Method for calling functions only allows within player ticks
	if (fn == "Function Engine.Actor.ReceiveTick") {
		// Custom tick
	}
	else if (fn == "Function Engine.HUD.ReceiveDrawHUD") {
		DrawHUD(reinterpret_cast<const AHUD*>(Class), World, ValeriaController, ValeriaPawn, ValeriaCharacter);
	}

	// Capture Current Fishing Location by Event
	if (fn == "Function Palia.FishingComponent.RpcServer_SelectLoot") {
		auto SelectLoot = static_cast<Params::FishingComponent_RpcServer_SelectLoot*>(Params);
		if (Overlay->bCaptureFishingSpot) {
			memcpy(&Overlay->sOverrideFishingSpot, &SelectLoot->RPCLootParams.WaterType_Deprecated, sizeof(FName));
			Overlay->bCaptureFishingSpot = false;
		}
		if (Overlay->bOverrideFishingSpot) {
			memcpy(&SelectLoot->RPCLootParams.WaterType_Deprecated, &Overlay->sOverrideFishingSpot, sizeof(FName));
		}
	}

	// Silent Aim Projectile Logic
	if (fn == "Function Palia.ProjectileFiringComponent.RpcServer_FireProjectile") {
		auto FireProjectile = static_cast<Params::ProjectileFiringComponent_RpcServer_FireProjectile*>(Params);
		
		auto Component = static_cast<UProjectileFiringComponent*>(const_cast<UObject*>(Class));
		if (!Component) return;
		
		if (Overlay->bEnableSilentAimbot && Overlay->BestTargetActor) {
			FVector TargetLocation = Overlay->BestTargetActor->K2_GetActorLocation();
			//FRotator TargetRotation = FireProjectile->SpawnRotation;
			//FTransform SpawnTransform = PC->K2_GetPawn()->GetTransform();
			//TArray<AActor*> AOEHitActors;
			
			FVector HitLocation = TargetLocation;

			// Find the projectile after it's fired and directly set its hit status
			for (auto& [ProjectileId, Pad_22C8, ProjectileActor, HasHit, Pad_22C9] : Component->FiredProjectiles) {
				if (ProjectileId == FireProjectile->ProjectileId) {
					FVector ProjectileLocation = ProjectileActor->K2_GetActorLocation();
					FVector FiringTargetLocation = Overlay->BestTargetActor->K2_GetActorLocation();

					// Calculate a point slightly in front of the target location
					FVector DirectionToTarget = (FiringTargetLocation - ProjectileLocation).GetNormalized();
					float DistanceBeforeTarget = 500.0f;  // Adjust this distance as needed
					FVector NewProjectileLocation = FiringTargetLocation - (DirectionToTarget * DistanceBeforeTarget);

					HasHit = true;
					FHitResult HitResult;
					ProjectileActor->K2_SetActorLocation(NewProjectileLocation, false, &HitResult, false);
					HitResult.Location = {NewProjectileLocation};
					
					//HitResult.Location = FVector_NetQuantize(NewProjectileLocation);
					//OriginalProcEvent(Class, Function, Params);
					
					Component->RpcServer_NotifyProjectileHit(FireProjectile->ProjectileId, Overlay->BestTargetActor, HitLocation);
				}
			}
			//PC->RpcServer_PerformClientAuthoritativeHit(FireProjectile->ProjectileId, Overlay->BestTargetActor, AOEHitActors);
		}
	}

	// Movement Velocity Logic
	if (fn == "Function Palia.ValeriaClientPriMovementComponent.RpcServer_SendMovement") {
		auto MovementParams = static_cast<SDK::Params::ValeriaClientPriMovementComponent_RpcServer_SendMovement*>(Params);
		
		UValeriaCharacterMoveComponent* MovementComponent = ValeriaCharacter->GetValeriaCharacterMovementComponent();
		if (!MovementComponent) return;

		FValeriaClientToServerMoveInfo BypassServerMove;
		MovementParams->MoveInfo.TargetVelocity = FVector(0, 0, 0);
	}

	//GetVFunction<void(*)(const UObject*, class UFunction*, void*)>(this, Offsets::ProcessEventIdx)(this, Function, Parms);
	OriginalProcEvent(Class, Function, Params);
}

void PaliaOverlay::DrawHUD()
{
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable keyboard navigation once at initialization if possible.

	ImGui::SetNextWindowBgAlpha(0.35f);
	ImGuiStyle& style = ImGui::GetStyle();
	float prevWindowRounding = style.WindowRounding;
	style.WindowRounding = 5.0f; // Temporary change of style.

	// Calculate watermark text only once, not in the drawing loop.
	std::string watermarkText = "OriginPalia Menu By Wimberton & The UnknownCheats Community";
	if (CurrentLevel && (CurrentMap == "MAP_PreGame" || CurrentMap == "Unknown")) {
		watermarkText = "Waiting for in-game login or loading...";
	}

	ImGui::SetNextWindowPos(ImVec2((io.DisplaySize.x - ImGui::CalcTextSize(watermarkText.c_str()).x) * 0.5f, 10.0f));
	ImGui::Begin("Watermark", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);
	ImGui::Text("%s", watermarkText.c_str());
	ImGui::End();

	style.WindowRounding = prevWindowRounding; // Restore style after the temporary change.

	auto World = GetWorld();
	if (!World) return; // Check for valid World pointer.

	auto GameInstance = World->OwningGameInstance;
	if (!GameInstance) return; // Check for valid GameInstance pointer.

	if (GameInstance->LocalPlayers.Num() == 0) return; // Check if there are any local players.

	ULocalPlayer* LocalPlayer = GameInstance->LocalPlayers[0];
	if (!LocalPlayer) return; // Ensure the local player is valid.

	APlayerController* PlayerController = LocalPlayer->PlayerController;
	if (!PlayerController) return; // Ensure there's a valid player controller.

	// HOOKING FISHINGCOMPONENT
	if (PlayerController->Pawn) {
		AValeriaCharacter* ValeriaCharacter = (static_cast<AValeriaPlayerController*>(PlayerController))->GetValeriaCharacter();
		if (ValeriaCharacter) {
			UFishingComponent* FishingComponent = ValeriaCharacter->GetFishing();
			if (FishingComponent) {
				void* Instance = FishingComponent;
				const void** Vtable = *reinterpret_cast<const void***>(const_cast<void*>(Instance));
				DWORD OldProtection;
				VirtualProtect(Vtable, sizeof(DWORD) * 1024, PAGE_EXECUTE_READWRITE, &OldProtection);
				int32 Idx = Offsets::ProcessEventIdx;
				OriginalProcEvent = reinterpret_cast<void(*)(const UObject*, class UFunction*, void*)>(uintptr_t(GetModuleHandle(0)) + Offsets::ProcessEvent);
				const void* NewProcEvt = ProcessEventDetour;
				Vtable[Idx] = NewProcEvt;
				HookedClient = FishingComponent;
				VirtualProtect(Vtable, sizeof(DWORD) * 1024, OldProtection, &OldProtection);
			}
		}
	}

	// HOOKING FIRINGCOMPONENT
	if (PlayerController->Pawn) {
		AValeriaCharacter* ValeriaCharacter = (static_cast<AValeriaPlayerController*>(PlayerController))->GetValeriaCharacter();
		if (ValeriaCharacter) {
			if (bEnableSilentAimbot) {
				UProjectileFiringComponent* FiringComponent = ValeriaCharacter->GetFiringComponent();
				if (FiringComponent) {
					void* Instance = FiringComponent;
					const void** Vtable = *reinterpret_cast<const void***>(const_cast<void*>(Instance));
					DWORD OldProtection;
					VirtualProtect(Vtable, sizeof(DWORD) * 1024, PAGE_EXECUTE_READWRITE, &OldProtection);
					int32 Idx = Offsets::ProcessEventIdx;
					OriginalProcEvent = reinterpret_cast<void(*)(const UObject*, class UFunction*, void*)>(uintptr_t(GetModuleHandle(0)) + Offsets::ProcessEvent);
					const void* NewProcEvt = ProcessEventDetour;
					Vtable[Idx] = NewProcEvt;
					HookedClient = FiringComponent;
					VirtualProtect(Vtable, sizeof(DWORD) * 1024, OldProtection, &OldProtection);
				}
			}
		}
	}

	// HOOKING MOVEMENTCOMPONENT
	if (PlayerController->Pawn) {
		AValeriaCharacter* ValeriaCharacter = (static_cast<AValeriaPlayerController*>(PlayerController))->GetValeriaCharacter();
		if (ValeriaCharacter) {
			UValeriaCharacterMoveComponent* MovementComponent = ValeriaCharacter->GetValeriaCharacterMovementComponent();
			if (MovementComponent) {
				void* Instance = MovementComponent;
				const void** Vtable = *reinterpret_cast<const void***>(const_cast<void*>(Instance));
				DWORD OldProtection;
				VirtualProtect(Vtable, sizeof(DWORD) * 1024, PAGE_EXECUTE_READWRITE, &OldProtection);
				int32 Idx = Offsets::ProcessEventIdx;
				OriginalProcEvent = reinterpret_cast<void(*)(const UObject*, class UFunction*, void*)>(uintptr_t(GetModuleHandle(0)) + Offsets::ProcessEvent);
				const void* NewProcEvt = ProcessEventDetour;
				Vtable[Idx] = NewProcEvt;
				HookedClient = MovementComponent;
				VirtualProtect(Vtable, sizeof(DWORD) * 1024, OldProtection, &OldProtection);
			}
		}
	}

	// HOOKING PLACEMENTCOMPONENT
	if (PlayerController->Pawn) {
		AValeriaCharacter* ValeriaCharacter = (static_cast<AValeriaPlayerController*>(PlayerController))->GetValeriaCharacter();
		if (ValeriaCharacter) {
			UPlacementComponent* PlacementComponent = ValeriaCharacter->GetPlacement();
			if (PlacementComponent) {
				void* Instance = PlacementComponent;
				const void** Vtable = *reinterpret_cast<const void***>(const_cast<void*>(Instance));
				DWORD OldProtection;
				VirtualProtect(Vtable, sizeof(DWORD) * 1024, PAGE_EXECUTE_READWRITE, &OldProtection);
				int32 Idx = Offsets::ProcessEventIdx;
				OriginalProcEvent = reinterpret_cast<void(*)(const UObject*, class UFunction*, void*)>(uintptr_t(GetModuleHandle(0)) + Offsets::ProcessEvent);
				const void* NewProcEvt = ProcessEventDetour;
				Vtable[Idx] = NewProcEvt;
				HookedClient = PlacementComponent;
				VirtualProtect(Vtable, sizeof(DWORD) * 1024, OldProtection, &OldProtection);
			}
		}
	}

	// HOOKING PROCESSEVENT IN AHUD
	if (HookedClient != PlayerController->MyHUD && PlayerController->MyHUD != nullptr) {
		void* Instance = PlayerController->MyHUD;
		const void** Vtable = *reinterpret_cast<const void***>(const_cast<void*>(Instance));
		vmt = Vtable;
		DWORD OldProtection;
		VirtualProtect(Vtable, sizeof(DWORD) * 1024, PAGE_EXECUTE_READWRITE, &OldProtection);
		int32 Idx = Offsets::ProcessEventIdx;
		// TODO: Use original content of VTable instead of pointer to UObject::ProcessEvent as the ProcessEvent is overriden by AActor::ProcessEvent and executes some delegates - I have yet to see any negative impact
		// So this works for now and doesn't cause infinite loop (which is caused by me doing something stoopid)
		OriginalProcEvent = reinterpret_cast<void(*)(const UObject*, class UFunction*, void*)>(uintptr_t(GetModuleHandle(0)) + Offsets::ProcessEvent);
		const void* NewProcEvt = ProcessEventDetour;
		Vtable[Idx] = NewProcEvt;
		HookedClient = PlayerController->MyHUD;
		VirtualProtect(Vtable, sizeof(DWORD) * 1024, OldProtection, &OldProtection);
	}
}

void PaliaOverlay::DrawOverlay()
{
	bool show = true;
	ImGuiIO& io = ImGui::GetIO();
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse;

	// Calculate the center position for the window
	ImVec2 center_pos = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
	ImVec2 window_size = ImVec2(1450, 950); // Set the initial window size
	ImVec2 window_pos = ImVec2(center_pos.x - window_size.x * 0.5f, center_pos.y - window_size.y * 0.5f);

	// Set the initial window position to the center of the screen
	ImGui::SetNextWindowPos(window_pos, ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(window_size, ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowBgAlpha(0.98f);

	std::string WindowTitle = std::string("OriginPalia Menu - V1.7.4.1 (Game Version 0.179.1)");

	if (ImGui::Begin(WindowTitle.data(), &show, window_flags))
	{
		static int OpenTab = 0;

		// Draw tabs
		if (ImGui::BeginTabBar("OverlayTabs"))
		{
			if (ImGui::BeginTabItem("ESP & Visuals"))
			{
				OpenTab = 0;
				ImGui::EndTabItem();
			}


			if (ImGui::BeginTabItem("Aimbots & Fun"))
			{
				OpenTab = 1;
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Movement & Teleport"))
			{
				OpenTab = 2;
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Selling & Items"))
			{
				OpenTab = 3;
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Skills & Tools"))
			{
				OpenTab = 4;
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Housing & Decorating"))
			{
				OpenTab = 5;
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}

		// ==================================== 0 Visuals & ESPs TAB
		if (OpenTab == 0) {
			ImGui::Columns(3, nullptr, false);

			// Base ESP controls
			if (ImGui::CollapsingHeader("Visual Settings - General", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Checkbox("Enable ESP", &bEnableESP);

				ImGui::Checkbox("Limit Distance", &bEnableESPCulling);
				if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip("Limit the maximum distance the ESP will render. Turn this down to a low value if you're having performance problems.");
				ImGui::InputInt("Distance", &CullDistance);

				ImGui::Checkbox("Enable InteliAim Circle", &bDrawFOVCircle);
				if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip("Enable the smart FOV targeting system. Teleport to actors, enable aimbots, and more.");
				if (bDrawFOVCircle) {
					ImGui::SliderFloat("InteliAim Radius", &FOVRadius, 10.0f, 600.0f, "%1.0f");
				}
			}

			ImGui::NextColumn();

			if (ImGui::CollapsingHeader("Animals")) {
                ImGui::BeginTable("Animals", 3);
                {
                    ImGui::TableSetupColumn("Name");
                    ImGui::TableSetupColumn("Show", ImGuiTableColumnFlags_WidthFixed, 40);
                    ImGui::TableSetupColumn("Color", ImGuiTableColumnFlags_WidthFixed, 40);
                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::TableNextColumn();
                    ImGui::Text("Show");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Sernuk")) {
                        Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier1)] =
                            !Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(
                                ECreatureQuality::Tier1)];
                    }
                    ImGui::TableNextColumn();
                    ImGui::Checkbox(
                        "##Sernuk",
                        &Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier1)]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker(
                        "##Sernuk",
                        &AnimalColors[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(
                            ECreatureQuality::Tier1)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Elder Sernuk")) {
                        Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier2)] =
                            !Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(
                                ECreatureQuality::Tier2)];
                    }
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##ElderSernuk",
                                    &Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(
                                        ECreatureQuality::Tier2)]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##ElderSernuk",
                                       &AnimalColors[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(
                                           ECreatureQuality::Tier2)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Proudhorn Sernuk")) {
                        Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier3)] =
                            !Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(
                                ECreatureQuality::Tier3)];
                    }
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##ProudhornSernuk",
                                    &Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(
                                        ECreatureQuality::Tier3)]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##ProudhornSernuk",
                                       &AnimalColors[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(
                                           ECreatureQuality::Tier3)]);
                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::TableNextColumn();
                    ImGui::Text("Show");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Chapaa")) {
                        Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier1)] =
                            !Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(
                                ECreatureQuality::Tier1)];
                    }
                    ImGui::TableNextColumn();
                    ImGui::Checkbox(
                        "##Chapaa",
                        &Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier1)]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker(
                        "##Chapaa",
                        &AnimalColors[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(
                            ECreatureQuality::Tier1)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Striped Chapaa")) {
                        Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier2)] =
                            !Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(
                                ECreatureQuality::Tier2)];
                    }
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##StripedChapaa",
                                    &Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(
                                        ECreatureQuality::Tier2)]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##StripedChapaa",
                                       &AnimalColors[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(
                                           ECreatureQuality::Tier2)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Azure Chapaa")) {
                        Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier3)] =
                            !Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(
                                ECreatureQuality::Tier3)];
                    }
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##AzureChapaa",
                                    &Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(
                                        ECreatureQuality::Tier3)]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##AzureChapaa",
                                       &AnimalColors[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(
                                           ECreatureQuality::Tier3)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Minigame Chapaa")) {
                        Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Chase)] =
                            !Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(
                                ECreatureQuality::Chase)];
                    }
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##MinigameChapaa",
                                    &Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(
                                        ECreatureQuality::Chase)]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##MinigameChapaa",
                                       &AnimalColors[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(
                                           ECreatureQuality::Chase)]);
                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::TableNextColumn();
                    ImGui::Text("Show");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Muujin")) {
                        Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier1)]
                            =
                            !Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(
                                ECreatureQuality::Tier1)];
                    }
                    ImGui::TableNextColumn();
                    ImGui::Checkbox(
                        "##Muujin",
                        &Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(
                            ECreatureQuality::Tier1)]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker(
                        "##Muujin",
                        &AnimalColors[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(
                            ECreatureQuality::Tier1)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Banded Muujin")) {
                        Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier2)]
                            =
                            !Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(
                                ECreatureQuality::Tier2)];
                    }
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##BandedMuujin",
                                    &Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(
                                        ECreatureQuality::Tier2)]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##BandedMuujin",
                                       &AnimalColors[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(
                                           ECreatureQuality::Tier2)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Bluebristle Muujin")) {
                        Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier3)]
                            =
                            !Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(
                                ECreatureQuality::Tier3)];
                    }
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##BluebristleMuujin",
                                    &Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(
                                        ECreatureQuality::Tier3)]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##BluebristleMuujin",
                                       &AnimalColors[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(
                                           ECreatureQuality::Tier3)]);
                }
                ImGui::EndTable();
            }
            if (ImGui::CollapsingHeader("Ores")) {
                ImGui::BeginTable("Ores", 5);
                {
                    ImGui::TableSetupColumn("Name");
                    ImGui::TableSetupColumn("Sm", ImGuiTableColumnFlags_WidthFixed, 40);
                    ImGui::TableSetupColumn("Med", ImGuiTableColumnFlags_WidthFixed, 40);
                    ImGui::TableSetupColumn("Lg", ImGuiTableColumnFlags_WidthFixed, 40);
                    ImGui::TableSetupColumn("Color", ImGuiTableColumnFlags_WidthFixed, 40);
                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::TableNextColumn();
                    ImGui::Text("Sm");
                    ImGui::TableNextColumn();
                    ImGui::Text("Med");
                    ImGui::TableNextColumn();
                    ImGui::Text("Lg");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Clay")) {
                        Ores[static_cast<int>(EOreType::Clay)][static_cast<int>(EGatherableSize::Large)] =
                            !Ores[static_cast<int>(EOreType::Clay)][static_cast<int>(EGatherableSize::Large)];
                    }
                    ImGui::TableNextColumn();
                    ImGui::TableNextColumn();
                    ImGui::TableNextColumn();
                    ImGui::Checkbox(
                        "##ClayLg", &Ores[static_cast<int>(EOreType::Clay)][static_cast<int>(EGatherableSize::Large)]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Clay", &OreColors[static_cast<int>(EOreType::Clay)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Stone")) {
                        Ores[static_cast<int>(EOreType::Stone)][static_cast<int>(EGatherableSize::Large)] =
                            Ores[static_cast<int>(EOreType::Stone)][static_cast<int>(EGatherableSize::Medium)] =
                            Ores[static_cast<int>(EOreType::Stone)][static_cast<int>(EGatherableSize::Small)] =
                            !Ores[static_cast<int>(EOreType::Stone)][static_cast<int>(EGatherableSize::Small)];
                    }
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##StoneSm",
                                    &Ores[static_cast<int>(EOreType::Stone)][static_cast<int>(EGatherableSize::Small)]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##StoneMed",
                                    &Ores[static_cast<int>(EOreType::Stone)][static_cast<int>(
                                        EGatherableSize::Medium)]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##StoneLg",
                                    &Ores[static_cast<int>(EOreType::Stone)][static_cast<int>(EGatherableSize::Large)]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Stone", &OreColors[static_cast<int>(EOreType::Stone)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Copper")) {
                        Ores[static_cast<int>(EOreType::Copper)][static_cast<int>(EGatherableSize::Large)] =
                            Ores[static_cast<int>(EOreType::Copper)][static_cast<int>(EGatherableSize::Medium)] =
                            Ores[static_cast<int>(EOreType::Copper)][static_cast<int>(EGatherableSize::Small)] =
                            !Ores[static_cast<int>(EOreType::Copper)][static_cast<int>(EGatherableSize::Small)];
                    }
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##CopperSm",
                                    &Ores[static_cast<int>(EOreType::Copper)][static_cast<int>(
                                        EGatherableSize::Small)]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##CopperMed",
                                    &Ores[static_cast<int>(EOreType::Copper)][static_cast<int>(
                                        EGatherableSize::Medium)]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##CopperLg",
                                    &Ores[static_cast<int>(EOreType::Copper)][static_cast<int>(
                                        EGatherableSize::Large)]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Copper", &OreColors[static_cast<int>(EOreType::Copper)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Iron")) {
                        Ores[static_cast<int>(EOreType::Iron)][static_cast<int>(EGatherableSize::Large)] =
                            Ores[static_cast<int>(EOreType::Iron)][static_cast<int>(EGatherableSize::Medium)] =
                            Ores[static_cast<int>(EOreType::Iron)][static_cast<int>(EGatherableSize::Small)] =
                            !Ores[static_cast<int>(EOreType::Iron)][static_cast<int>(EGatherableSize::Small)];
                    }
                    ImGui::TableNextColumn();
                    ImGui::Checkbox(
                        "##IronSm", &Ores[static_cast<int>(EOreType::Iron)][static_cast<int>(EGatherableSize::Small)]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##IronMed",
                                    &Ores[static_cast<int>(EOreType::Iron)][static_cast<int>(EGatherableSize::Medium)]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox(
                        "##IronLg", &Ores[static_cast<int>(EOreType::Iron)][static_cast<int>(EGatherableSize::Large)]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Iron", &OreColors[static_cast<int>(EOreType::Iron)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Palium")) {
                        Ores[static_cast<int>(EOreType::Palium)][static_cast<int>(EGatherableSize::Large)] =
                            Ores[static_cast<int>(EOreType::Palium)][static_cast<int>(EGatherableSize::Medium)] =
                            Ores[static_cast<int>(EOreType::Palium)][static_cast<int>(EGatherableSize::Small)] =
                            !Ores[static_cast<int>(EOreType::Palium)][static_cast<int>(EGatherableSize::Small)];
                    }
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##PaliumSm",
                                    &Ores[static_cast<int>(EOreType::Palium)][static_cast<int>(
                                        EGatherableSize::Small)]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##PaliumMed",
                                    &Ores[static_cast<int>(EOreType::Palium)][static_cast<int>(
                                        EGatherableSize::Medium)]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##PaliumLg",
                                    &Ores[static_cast<int>(EOreType::Palium)][static_cast<int>(
                                        EGatherableSize::Large)]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Palium", &OreColors[static_cast<int>(EOreType::Palium)]);
                }
                ImGui::EndTable();
            }
            if (ImGui::CollapsingHeader("Forageables")) {
                ImGui::Text("Enable all:");
                ImGui::SameLine();

                if (ImGui::Button("Common##Forage")) {
                    for (int pos : ForageableCommon) {
                        Forageables[pos][1] = Forageables[pos][0] = !Forageables[pos][0];
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Uncommon##Forage")) {
                    for (int pos : ForageableUncommon) {
                        Forageables[pos][1] = Forageables[pos][0] = !Forageables[pos][0];
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Rare##Forage")) {
                    for (int pos : ForageableRare) { Forageables[pos][1] = Forageables[pos][0] = !Forageables[pos][0]; }
                }
                ImGui::SameLine();
                if (ImGui::Button("Epic##Forage")) {
                    for (int pos : ForageableEpic) { Forageables[pos][1] = Forageables[pos][0] = !Forageables[pos][0]; }
                }
                ImGui::SameLine();
                if (ImGui::Button("Star##Forage")) {
                    for (auto& Forageable : Forageables) {
	                    Forageable[1] = !Forageable[1];
                    }
                }
                ImGui::BeginTable("Forageables", 4);
                {
                    ImGui::TableSetupColumn("Name");
                    ImGui::TableSetupColumn("Normal", ImGuiTableColumnFlags_WidthFixed, 40);
                    ImGui::TableSetupColumn("Star", ImGuiTableColumnFlags_WidthFixed, 40);
                    ImGui::TableSetupColumn("Color", ImGuiTableColumnFlags_WidthFixed, 40);
                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::Text("Beach");
                    ImGui::TableNextColumn();
                    ImGui::Text("Normal");
                    ImGui::TableNextColumn();
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Coral");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##Coral", &Forageables[static_cast<int>(EForageableType::Coral)][0]);
                    ImGui::TableNextColumn();
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Coral", &ForageableColors[static_cast<int>(EForageableType::Coral)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Oyster");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##Oyster", &Forageables[static_cast<int>(EForageableType::Oyster)][0]);
                    ImGui::TableNextColumn();
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Oyster", &ForageableColors[static_cast<int>(EForageableType::Oyster)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Shell");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##Shell", &Forageables[static_cast<int>(EForageableType::Shell)][0]);
                    ImGui::TableNextColumn();
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Shell", &ForageableColors[static_cast<int>(EForageableType::Shell)]);
                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::Text("Flower");
                    ImGui::TableNextColumn();
                    ImGui::Text("Normal");
                    ImGui::TableNextColumn();
                    ImGui::Text("Star");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Briar Daisy");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##PoisonFlower", &Forageables[static_cast<int>(EForageableType::PoisonFlower)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##PoisonFlowerP",
                                    &Forageables[static_cast<int>(EForageableType::PoisonFlower)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##PoisonFlower",
                                       &ForageableColors[static_cast<int>(EForageableType::PoisonFlower)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Crystal Lake Lotus");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##WaterFlower", &Forageables[static_cast<int>(EForageableType::WaterFlower)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##WaterFlowerP", &Forageables[static_cast<int>(EForageableType::WaterFlower)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##WaterFlower",
                                       &ForageableColors[static_cast<int>(EForageableType::WaterFlower)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Heartdrop Lily");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##Heartdrop", &Forageables[static_cast<int>(EForageableType::Heartdrop)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##HeartdropP", &Forageables[static_cast<int>(EForageableType::Heartdrop)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Heartdrop", &ForageableColors[static_cast<int>(EForageableType::Heartdrop)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Sundrop Lily");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##Sundrop", &Forageables[static_cast<int>(EForageableType::Sundrop)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##SundropP", &Forageables[static_cast<int>(EForageableType::Sundrop)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Sundrop", &ForageableColors[static_cast<int>(EForageableType::Sundrop)]);
                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::Text("Moss");
                    ImGui::TableNextColumn();
                    ImGui::Text("Normal");
                    ImGui::TableNextColumn();
                    ImGui::Text("Star");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Dragon's Beard Peat");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##DragonsBeard", &Forageables[static_cast<int>(EForageableType::DragonsBeard)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##DragonsBeardP",
                                    &Forageables[static_cast<int>(EForageableType::DragonsBeard)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##DragonsBeard",
                                       &ForageableColors[static_cast<int>(EForageableType::DragonsBeard)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Emerald Carpet Moss");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##EmeraldCarpet",
                                    &Forageables[static_cast<int>(EForageableType::EmeraldCarpet)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##EmeraldCarpetP",
                                    &Forageables[static_cast<int>(EForageableType::EmeraldCarpet)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##EmeraldCarpet",
                                       &ForageableColors[static_cast<int>(EForageableType::EmeraldCarpet)]);
                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::Text("Mushroom");
                    ImGui::TableNextColumn();
                    ImGui::Text("Normal");
                    ImGui::TableNextColumn();
                    ImGui::Text("Star");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Brightshroom");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##MushroomBlue", &Forageables[static_cast<int>(EForageableType::MushroomBlue)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##MushroomBlueP",
                                    &Forageables[static_cast<int>(EForageableType::MushroomBlue)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##MushroomBlue",
                                       &ForageableColors[static_cast<int>(EForageableType::MushroomBlue)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Mountain Morel");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##MushroomRed", &Forageables[static_cast<int>(EForageableType::MushroomRed)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##MushroomRedP", &Forageables[static_cast<int>(EForageableType::MushroomRed)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##MushroomRed",
                                       &ForageableColors[static_cast<int>(EForageableType::MushroomRed)]);
                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::Text("Spice");
                    ImGui::TableNextColumn();
                    ImGui::Text("Normal");
                    ImGui::TableNextColumn();
                    ImGui::Text("Star");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Dari Cloves");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##DariCloves", &Forageables[static_cast<int>(EForageableType::DariCloves)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##DariClovesP", &Forageables[static_cast<int>(EForageableType::DariCloves)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##DariCloves",
                                       &ForageableColors[static_cast<int>(EForageableType::DariCloves)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Heat Root");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##HeatRoot", &Forageables[static_cast<int>(EForageableType::HeatRoot)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##HeatRootP", &Forageables[static_cast<int>(EForageableType::HeatRoot)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##HeatRoot", &ForageableColors[static_cast<int>(EForageableType::HeatRoot)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Spice Sprouts");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##SpicedSprouts",
                                    &Forageables[static_cast<int>(EForageableType::SpicedSprouts)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##SpicedSproutsP",
                                    &Forageables[static_cast<int>(EForageableType::SpicedSprouts)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##SpicedSprouts",
                                       &ForageableColors[static_cast<int>(EForageableType::SpicedSprouts)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Sweet Leaf");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##SweetLeaves", &Forageables[static_cast<int>(EForageableType::SweetLeaves)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##SweetLeavesP", &Forageables[static_cast<int>(EForageableType::SweetLeaves)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##SweetLeaves",
                                       &ForageableColors[static_cast<int>(EForageableType::SweetLeaves)]);
                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::Text("Vegetable");
                    ImGui::TableNextColumn();
                    ImGui::Text("Normal");
                    ImGui::TableNextColumn();
                    ImGui::Text("Star");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Wild Garlic");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##Garlic", &Forageables[static_cast<int>(EForageableType::Garlic)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##GarlicP", &Forageables[static_cast<int>(EForageableType::Garlic)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Garlic", &ForageableColors[static_cast<int>(EForageableType::Garlic)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Wild Ginger");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##Ginger", &Forageables[static_cast<int>(EForageableType::Ginger)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##GingerP", &Forageables[static_cast<int>(EForageableType::Ginger)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Ginger", &ForageableColors[static_cast<int>(EForageableType::Ginger)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Wild Green Onion");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##GreenOnion", &Forageables[static_cast<int>(EForageableType::GreenOnion)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##GreenOnionP", &Forageables[static_cast<int>(EForageableType::GreenOnion)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##GreenOnion",
                                       &ForageableColors[static_cast<int>(EForageableType::GreenOnion)]);
                }
                ImGui::EndTable();
            }

			ImGui::NextColumn();

			if (ImGui::CollapsingHeader("Bugs")) {
                ImGui::Text("Enable all:");
                ImGui::SameLine();

                if (ImGui::Button("Common##Bugs")) {
                    for (auto& Bug : Bugs) {
	                    Bug[static_cast<int>(EBugQuality::Common)][1] = Bug[static_cast<int>(
                            EBugQuality::Common)][0] = !Bug[static_cast<int>(EBugQuality::Common)][0];
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Uncommon##Bugs")) {
                    for (auto& Bug : Bugs) {
	                    Bug[static_cast<int>(EBugQuality::Uncommon)][1] = Bug[static_cast<int>(
                            EBugQuality::Uncommon)][0] = !Bug[static_cast<int>(EBugQuality::Uncommon)][0];
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Rare##Bugs")) {
                    for (auto& Bug : Bugs) {
	                    Bug[static_cast<int>(EBugQuality::Rare)][1] = Bug[static_cast<int>(EBugQuality::Rare)][
                            0] = !Bug[static_cast<int>(EBugQuality::Rare)][0];
	                    Bug[static_cast<int>(EBugQuality::Rare2)][1] = Bug[static_cast<int>(EBugQuality::Rare2)]
                            [0] = !Bug[static_cast<int>(EBugQuality::Rare2)][0];
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Epic##Bugs")) {
                    for (auto& Bug : Bugs) {
	                    Bug[static_cast<int>(EBugQuality::Epic)][1] = Bug[static_cast<int>(EBugQuality::Epic)][
                            0] = !Bug[static_cast<int>(EBugQuality::Epic)][0];
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Star##Bugs")) {
                    for (auto& Bug : Bugs) {
                        for (auto& j : Bug) { j[1] = !j[1]; }
                    }
                }
                ImGui::BeginTable("Bugs", 4);
                {
                    ImGui::TableSetupColumn("Name");
                    ImGui::TableSetupColumn("Normal", ImGuiTableColumnFlags_WidthFixed, 40);
                    ImGui::TableSetupColumn("Star", ImGuiTableColumnFlags_WidthFixed, 40);
                    ImGui::TableSetupColumn("Color", ImGuiTableColumnFlags_WidthFixed, 40);
                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::Text("Bee");
                    ImGui::TableNextColumn();
                    ImGui::Text("Normal");
                    ImGui::TableNextColumn();
                    ImGui::Text("Star");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Bahari Bee");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox(
                        "##BeeU", &Bugs[static_cast<int>(EBugKind::Bee)][static_cast<int>(EBugQuality::Uncommon)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox(
                        "##BeeUP", &Bugs[static_cast<int>(EBugKind::Bee)][static_cast<int>(EBugQuality::Uncommon)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker(
                        "##BeeU", &BugColors[static_cast<int>(EBugKind::Bee)][static_cast<int>(EBugQuality::Uncommon)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Golden Glory Bee");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox(
                        "##BeeR", &Bugs[static_cast<int>(EBugKind::Bee)][static_cast<int>(EBugQuality::Rare)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox(
                        "##BeeRP", &Bugs[static_cast<int>(EBugKind::Bee)][static_cast<int>(EBugQuality::Rare)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker(
                        "##Bee", &BugColors[static_cast<int>(EBugKind::Bee)][static_cast<int>(EBugQuality::Rare)]);
                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::Text("Beetle");
                    ImGui::TableNextColumn();
                    ImGui::Text("Normal");
                    ImGui::TableNextColumn();
                    ImGui::Text("Star");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Spotted Stink Bug");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##BeetleC",
                                    &Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Common)][
                                        0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##BeetleCP",
                                    &Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Common)][
                                        1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##BeetleC",
                                       &BugColors[static_cast<int>(EBugKind::Beetle)][static_cast<int>(
                                           EBugQuality::Common)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Proudhorned Stag Beetle");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##BeetleU",
                                    &Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Uncommon)][
                                        0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##BeetleUP",
                                    &Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Uncommon)][
                                        1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##BeetleU",
                                       &BugColors[static_cast<int>(EBugKind::Beetle)][static_cast<int>(
                                           EBugQuality::Uncommon)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Raspberry Beetle");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##BeetleR",
                                    &Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Rare)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##BeetleRP",
                                    &Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Rare)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##BeetleR",
                                       &BugColors[static_cast<int>(EBugKind::Beetle)][static_cast<int>(
                                           EBugQuality::Rare)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Ancient Amber Beetle");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##BeetleE",
                                    &Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Epic)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##BeetleEP",
                                    &Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Epic)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##BeetleE",
                                       &BugColors[static_cast<int>(EBugKind::Beetle)][static_cast<int>(
                                           EBugQuality::Epic)]);
                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::Text("Butterfly");
                    ImGui::TableNextColumn();
                    ImGui::Text("Normal");
                    ImGui::TableNextColumn();
                    ImGui::Text("Star");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Common Blue Butterfly");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##ButterflyC",
                                    &Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Common)][
                                        0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##ButterflyCP",
                                    &Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Common)][
                                        1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##ButterflyC",
                                       &BugColors[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(
                                           EBugQuality::Common)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Duskwing Butterfly");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##ButterflyU",
                                    &Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(
                                        EBugQuality::Uncommon)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##ButterflyUP",
                                    &Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(
                                        EBugQuality::Uncommon)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##ButterflyU",
                                       &BugColors[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(
                                           EBugQuality::Uncommon)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Brighteye Butterfly");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##ButterflyR",
                                    &Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Rare)][
                                        0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##ButterflyRP",
                                    &Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Rare)][
                                        1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##ButterflyR",
                                       &BugColors[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(
                                           EBugQuality::Rare)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Rainbow-Tipped Butterfly");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##ButterflyE",
                                    &Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Epic)][
                                        0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##ButterflyEP",
                                    &Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Epic)][
                                        1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##ButterflyE",
                                       &BugColors[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(
                                           EBugQuality::Epic)]);
                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::Text("Cicada");
                    ImGui::TableNextColumn();
                    ImGui::Text("Normal");
                    ImGui::TableNextColumn();
                    ImGui::Text("Star");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Common Bark Cicada");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##CicadaC",
                                    &Bugs[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Common)][
                                        0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##CicadaCP",
                                    &Bugs[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Common)][
                                        1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##CicadaC",
                                       &BugColors[static_cast<int>(EBugKind::Cicada)][static_cast<int>(
                                           EBugQuality::Common)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Cerulean Cicada");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##CicadaU",
                                    &Bugs[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Uncommon)][
                                        0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##CicadaUP",
                                    &Bugs[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Uncommon)][
                                        1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##CicadaU",
                                       &BugColors[static_cast<int>(EBugKind::Cicada)][static_cast<int>(
                                           EBugQuality::Uncommon)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Spitfire Cicada");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##CicadaR",
                                    &Bugs[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Rare)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##CicadaRP",
                                    &Bugs[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Rare)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##CicadaR",
                                       &BugColors[static_cast<int>(EBugKind::Cicada)][static_cast<int>(
                                           EBugQuality::Rare)]);
                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::Text("Crab");
                    ImGui::TableNextColumn();
                    ImGui::Text("Normal");
                    ImGui::TableNextColumn();
                    ImGui::Text("Star");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Bahari Crab");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox(
                        "##CrabC", &Bugs[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Common)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox(
                        "##CrabCP", &Bugs[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Common)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker(
                        "##CrabC", &BugColors[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Common)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Spineshell Crab");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox(
                        "##CrabU", &Bugs[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Uncommon)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox(
                        "##CrabUP",
                        &Bugs[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Uncommon)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker(
                        "##CrabU",
                        &BugColors[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Uncommon)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Vampire Crab");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox(
                        "##CrabR", &Bugs[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Rare)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox(
                        "##CrabRP", &Bugs[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Rare)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker(
                        "##CrabR", &BugColors[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Rare)]);
                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::Text("Cricket");
                    ImGui::TableNextColumn();
                    ImGui::Text("Normal");
                    ImGui::TableNextColumn();
                    ImGui::Text("Star");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Common Field Cricket");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##CricketC",
                                    &Bugs[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Common)][
                                        0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##CricketCP",
                                    &Bugs[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Common)][
                                        1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##CricketC",
                                       &BugColors[static_cast<int>(EBugKind::Cricket)][static_cast<int>(
                                           EBugQuality::Common)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Garden Leafhopper");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##CricketU",
                                    &Bugs[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Uncommon)][
                                        0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##CricketUP",
                                    &Bugs[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Uncommon)][
                                        1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##CricketU",
                                       &BugColors[static_cast<int>(EBugKind::Cricket)][static_cast<int>(
                                           EBugQuality::Uncommon)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Azure Stonehopper");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##CricketR",
                                    &Bugs[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Rare)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##CricketRP",
                                    &Bugs[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Rare)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##CricketR",
                                       &BugColors[static_cast<int>(EBugKind::Cricket)][static_cast<int>(
                                           EBugQuality::Rare)]);
                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::Text("Dragonfly");
                    ImGui::TableNextColumn();
                    ImGui::Text("Normal");
                    ImGui::TableNextColumn();
                    ImGui::Text("Star");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Brushtail Dragonfly");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##DragonflyC",
                                    &Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Common)][
                                        0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##DragonflyCP",
                                    &Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Common)][
                                        1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##DragonflyC",
                                       &BugColors[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(
                                           EBugQuality::Common)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Inky Dragonfly");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##DragonflyU",
                                    &Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(
                                        EBugQuality::Uncommon)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##DragonflyUP",
                                    &Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(
                                        EBugQuality::Uncommon)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##DragonflyU",
                                       &BugColors[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(
                                           EBugQuality::Uncommon)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Firebreathing Dragonfly");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##DragonflyR",
                                    &Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Rare)][
                                        0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##DragonflyRP",
                                    &Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Rare)][
                                        1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##DragonflyR",
                                       &BugColors[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(
                                           EBugQuality::Rare)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Jewelwing Dragonfly");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##DragonflyE",
                                    &Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Epic)][
                                        0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##DragonflyEP",
                                    &Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Epic)][
                                        1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##DragonflyE",
                                       &BugColors[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(
                                           EBugQuality::Epic)]);

                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::Text("Glowbug");
                    ImGui::TableNextColumn();
                    ImGui::Text("Normal");
                    ImGui::TableNextColumn();
                    ImGui::Text("Star");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Paper Lantern Bug");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##GlowbugC",
                                    &Bugs[static_cast<int>(EBugKind::Glowbug)][static_cast<int>(EBugQuality::Common)][
                                        0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##GlowbugCP",
                                    &Bugs[static_cast<int>(EBugKind::Glowbug)][static_cast<int>(EBugQuality::Common)][
                                        1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##GlowbugC",
                                       &BugColors[static_cast<int>(EBugKind::Glowbug)][static_cast<int>(
                                           EBugQuality::Common)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Bahari Glowbug");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##GlowbugU",
                                    &Bugs[static_cast<int>(EBugKind::Glowbug)][static_cast<int>(EBugQuality::Uncommon)][
                                        0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##GlowbugUP",
                                    &Bugs[static_cast<int>(EBugKind::Glowbug)][static_cast<int>(EBugQuality::Uncommon)][
                                        1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##GlowbugU",
                                       &BugColors[static_cast<int>(EBugKind::Glowbug)][static_cast<int>(
                                           EBugQuality::Uncommon)]);

                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::Text("Ladybug");
                    ImGui::TableNextColumn();
                    ImGui::Text("Normal");
                    ImGui::TableNextColumn();
                    ImGui::Text("Star");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Garden Ladybug");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##LadybugC",
                                    &Bugs[static_cast<int>(EBugKind::Ladybug)][static_cast<int>(EBugQuality::Common)][
                                        0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##LadybugCP",
                                    &Bugs[static_cast<int>(EBugKind::Ladybug)][static_cast<int>(EBugQuality::Common)][
                                        1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##LadybugC",
                                       &BugColors[static_cast<int>(EBugKind::Ladybug)][static_cast<int>(
                                           EBugQuality::Common)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Princess Ladybug");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##LadybugU",
                                    &Bugs[static_cast<int>(EBugKind::Ladybug)][static_cast<int>(EBugQuality::Uncommon)][
                                        0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##LadybugUP",
                                    &Bugs[static_cast<int>(EBugKind::Ladybug)][static_cast<int>(EBugQuality::Uncommon)][
                                        1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##LadybugU",
                                       &BugColors[static_cast<int>(EBugKind::Ladybug)][static_cast<int>(
                                           EBugQuality::Uncommon)]);

                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::Text("Mantis");
                    ImGui::TableNextColumn();
                    ImGui::Text("Normal");
                    ImGui::TableNextColumn();
                    ImGui::Text("Star");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Garden Mantis");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##MantisU",
                                    &Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Uncommon)][
                                        0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##MantisUP",
                                    &Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Uncommon)][
                                        1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##MantisU",
                                       &BugColors[static_cast<int>(EBugKind::Mantis)][static_cast<int>(
                                           EBugQuality::Uncommon)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Spotted Mantis");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##MantisR",
                                    &Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Rare)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##MantisRP",
                                    &Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Rare)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##MantisR",
                                       &BugColors[static_cast<int>(EBugKind::Mantis)][static_cast<int>(
                                           EBugQuality::Rare)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Leafstalker Mantis");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##MantisR2",
                                    &Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Rare2)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##MantisR2P",
                                    &Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Rare2)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##MantisR2",
                                       &BugColors[static_cast<int>(EBugKind::Mantis)][static_cast<int>(
                                           EBugQuality::Rare2)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Fairy Mantis");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##MantisE",
                                    &Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Epic)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##MantisEP",
                                    &Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Epic)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##MantisE",
                                       &BugColors[static_cast<int>(EBugKind::Mantis)][static_cast<int>(
                                           EBugQuality::Epic)]);

                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::Text("Moth");
                    ImGui::TableNextColumn();
                    ImGui::Text("Normal");
                    ImGui::TableNextColumn();
                    ImGui::Text("Star");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Kilima Night Moth");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox(
                        "##MothC", &Bugs[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Common)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox(
                        "##MothCP", &Bugs[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Common)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker(
                        "##MothC", &BugColors[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Common)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Lunar Fairy Moth");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox(
                        "##MothU", &Bugs[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Uncommon)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox(
                        "##MothUP",
                        &Bugs[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Uncommon)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker(
                        "##MothU",
                        &BugColors[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Uncommon)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Gossamer Veil Moth");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox(
                        "##MothR", &Bugs[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Rare)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox(
                        "##MothRP", &Bugs[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Rare)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker(
                        "##MothR", &BugColors[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Rare)]);

                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::Text("Pede");
                    ImGui::TableNextColumn();
                    ImGui::Text("Normal");
                    ImGui::TableNextColumn();
                    ImGui::Text("Star");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Garden Millipede");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox(
                        "##PedeU", &Bugs[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Uncommon)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox(
                        "##PedeUP",
                        &Bugs[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Uncommon)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker(
                        "##PedeU",
                        &BugColors[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Uncommon)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Hairy Millipede");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox(
                        "##PedeR", &Bugs[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Rare)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox(
                        "##PedeRP", &Bugs[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Rare)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker(
                        "##PedeR", &BugColors[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Rare)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Scintillating Centipede");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox(
                        "##PedeR2", &Bugs[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Rare2)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##PedeR2P",
                                    &Bugs[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Rare2)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker(
                        "##PedeR2", &BugColors[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Rare2)]);
                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::Text("Snail");
                    ImGui::TableNextColumn();
                    ImGui::Text("Normal");
                    ImGui::TableNextColumn();
                    ImGui::Text("Star");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Garden Snail");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox(
                        "##SnailU",
                        &Bugs[static_cast<int>(EBugKind::Snail)][static_cast<int>(EBugQuality::Uncommon)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##SnailUP",
                                    &Bugs[static_cast<int>(EBugKind::Snail)][static_cast<int>(EBugQuality::Uncommon)][
                                        1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker(
                        "##SnailU",
                        &BugColors[static_cast<int>(EBugKind::Snail)][static_cast<int>(EBugQuality::Uncommon)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Stripeshell Snail");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox(
                        "##SnailR", &Bugs[static_cast<int>(EBugKind::Snail)][static_cast<int>(EBugQuality::Rare)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##SnailRP",
                                    &Bugs[static_cast<int>(EBugKind::Snail)][static_cast<int>(EBugQuality::Rare)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker(
                        "##SnailR", &BugColors[static_cast<int>(EBugKind::Snail)][static_cast<int>(EBugQuality::Rare)]);
                }
                ImGui::EndTable();
            }
            if (ImGui::CollapsingHeader("Trees")) {
                ImGui::BeginTable("Trees", 5);
                {
                    ImGui::TableSetupColumn("Name");
                    ImGui::TableSetupColumn("Sm", ImGuiTableColumnFlags_WidthFixed, 40);
                    ImGui::TableSetupColumn("Med", ImGuiTableColumnFlags_WidthFixed, 40);
                    ImGui::TableSetupColumn("Lg", ImGuiTableColumnFlags_WidthFixed, 40);
                    ImGui::TableSetupColumn("Color", ImGuiTableColumnFlags_WidthFixed, 40);
                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::TableNextColumn();
                    ImGui::Text("Sm");
                    ImGui::TableNextColumn();
                    ImGui::Text("Med");
                    ImGui::TableNextColumn();
                    ImGui::Text("Lg");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Bush")) {
                        Trees[static_cast<int>(ETreeType::Bush)][static_cast<int>(EGatherableSize::Bush)] =
                            !Trees[static_cast<int>(ETreeType::Bush)][static_cast<int>(EGatherableSize::Bush)];
                    }
                    ImGui::TableNextColumn();
                    ImGui::Checkbox(
                        "##BushSm", &Trees[static_cast<int>(ETreeType::Bush)][static_cast<int>(EGatherableSize::Bush)]);
                    ImGui::TableNextColumn();
                    ImGui::TableNextColumn();
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Bush", &TreeColors[static_cast<int>(ETreeType::Bush)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Sapwood")) {
                        Trees[static_cast<int>(ETreeType::Sapwood)][static_cast<int>(EGatherableSize::Large)] =
                            Trees[static_cast<int>(ETreeType::Sapwood)][static_cast<int>(EGatherableSize::Medium)] =
                            Trees[static_cast<int>(ETreeType::Sapwood)][static_cast<int>(EGatherableSize::Small)] =
                            !Trees[static_cast<int>(ETreeType::Sapwood)][static_cast<int>(EGatherableSize::Small)];
                    }
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##SapwoodSm",
                                    &Trees[static_cast<int>(ETreeType::Sapwood)][static_cast<int>(
                                        EGatherableSize::Small)]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##SapwoodMed",
                                    &Trees[static_cast<int>(ETreeType::Sapwood)][static_cast<int>(
                                        EGatherableSize::Medium)]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##SapwoodLg",
                                    &Trees[static_cast<int>(ETreeType::Sapwood)][static_cast<int>(
                                        EGatherableSize::Large)]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Sapwood", &TreeColors[static_cast<int>(ETreeType::Sapwood)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Heartwood")) {
                        Trees[static_cast<int>(ETreeType::Heartwood)][static_cast<int>(EGatherableSize::Large)] =
                            Trees[static_cast<int>(ETreeType::Heartwood)][static_cast<int>(EGatherableSize::Medium)] =
                            Trees[static_cast<int>(ETreeType::Heartwood)][static_cast<int>(EGatherableSize::Small)] =
                            !Trees[static_cast<int>(ETreeType::Heartwood)][static_cast<int>(EGatherableSize::Small)];
                    }
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##HeartwoodSm",
                                    &Trees[static_cast<int>(ETreeType::Heartwood)][static_cast<int>(
                                        EGatherableSize::Small)]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##HeartwoodMed",
                                    &Trees[static_cast<int>(ETreeType::Heartwood)][static_cast<int>(
                                        EGatherableSize::Medium)]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##HeartwoodLg",
                                    &Trees[static_cast<int>(ETreeType::Heartwood)][static_cast<int>(
                                        EGatherableSize::Large)]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Heartwood", &TreeColors[static_cast<int>(ETreeType::Heartwood)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Flow-Infused")) {
                        Trees[static_cast<int>(ETreeType::Flow)][static_cast<int>(EGatherableSize::Large)] =
                            Trees[static_cast<int>(ETreeType::Flow)][static_cast<int>(EGatherableSize::Medium)] =
                            Trees[static_cast<int>(ETreeType::Flow)][static_cast<int>(EGatherableSize::Small)] =
                            !Trees[static_cast<int>(ETreeType::Flow)][static_cast<int>(EGatherableSize::Small)];
                    }
                    ImGui::TableNextColumn();
                    ImGui::Checkbox(
                        "##FlowSm",
                        &Trees[static_cast<int>(ETreeType::Flow)][static_cast<int>(EGatherableSize::Small)]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##FlowMed",
                                    &Trees[static_cast<int>(ETreeType::Flow)][static_cast<int>(
                                        EGatherableSize::Medium)]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox(
                        "##FlowLg",
                        &Trees[static_cast<int>(ETreeType::Flow)][static_cast<int>(EGatherableSize::Large)]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Flow", &TreeColors[static_cast<int>(ETreeType::Flow)]);
                }
                ImGui::EndTable();
            }
			if (ImGui::CollapsingHeader("Player & Entities"))
			{
				ImGui::BeginTable("Odds", 3);
				{
					ImGui::TableSetupColumn("Name");
					ImGui::TableSetupColumn("Show", ImGuiTableColumnFlags_WidthFixed, 40);
					ImGui::TableSetupColumn("Color", ImGuiTableColumnFlags_WidthFixed, 40);
					ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
					ImGui::TableNextColumn();
					ImGui::TableNextColumn();
					ImGui::Text("Show");
					ImGui::TableNextColumn();
					ImGui::Text("Color");
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					if (ImGui::Button("Players")) {
						Singles[static_cast<int>(EOneOffs::Player)] =
							!Singles[static_cast<int>(EOneOffs::Player)];
					}
					ImGui::TableNextColumn();
					ImGui::Checkbox("##Players", &Singles[static_cast<int>(EOneOffs::Player)]);
					ImGui::TableNextColumn();
					ImGui::ColorPicker("##Players", &SingleColors[static_cast<int>(EOneOffs::Player)]);
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					if (ImGui::Button("NPCs")) {
						Singles[static_cast<int>(EOneOffs::NPC)] =
							!Singles[static_cast<int>(EOneOffs::NPC)];
					}
					ImGui::TableNextColumn();
					ImGui::Checkbox("##NPC", &Singles[static_cast<int>(EOneOffs::NPC)]);
					ImGui::TableNextColumn();
					ImGui::ColorPicker("##NPC", &SingleColors[static_cast<int>(EOneOffs::NPC)]);
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					if (ImGui::Button("Fish")) {
						Fish[static_cast<int>(EFishType::Hook)] =
							!Fish[static_cast<int>(EFishType::Hook)];
					}
					ImGui::TableNextColumn();
					ImGui::Checkbox("##Fish", &Fish[static_cast<int>(EFishType::Hook)]);
					ImGui::TableNextColumn();
					ImGui::ColorPicker("##Fish", &FishColors[static_cast<int>(EFishType::Hook)]);
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					if (ImGui::Button("Fish Pools")) {
						Fish[static_cast<int>(EFishType::Node)] =
							!Fish[static_cast<int>(EFishType::Node)];
					}
					ImGui::TableNextColumn();
					ImGui::Checkbox("##Pools", &Fish[static_cast<int>(EFishType::Node)]);
					ImGui::TableNextColumn();
					ImGui::ColorPicker("##Pools", &FishColors[static_cast<int>(EFishType::Node)]);
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					if (ImGui::Button("Loot")) {
						Singles[(int)EOneOffs::Loot] =
							!Singles[(int)EOneOffs::Loot];
					}
					ImGui::TableNextColumn();
					ImGui::Checkbox("##Loot", &Singles[static_cast<int>(EOneOffs::Loot)]);
					ImGui::TableNextColumn();
					ImGui::ColorPicker("##Loot", &SingleColors[static_cast<int>(EOneOffs::Loot)]);
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					if (ImGui::Button("Quests")) {
						Singles[static_cast<int>(EOneOffs::Quest)] =
							!Singles[static_cast<int>(EOneOffs::Quest)];
					}
					ImGui::TableNextColumn();
					ImGui::Checkbox("##Quest", &Singles[static_cast<int>(EOneOffs::Quest)]);
					ImGui::TableNextColumn();
					ImGui::ColorPicker("##Quest", &SingleColors[static_cast<int>(EOneOffs::Quest)]);
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					if (ImGui::Button("Rummage Piles")) {
						Singles[static_cast<int>(EOneOffs::RummagePiles)] =
							!Singles[static_cast<int>(EOneOffs::RummagePiles)];
					}
					ImGui::TableNextColumn();
					ImGui::Checkbox("##RummagePiles", &Singles[static_cast<int>(EOneOffs::RummagePiles)]);
					ImGui::TableNextColumn();
					ImGui::ColorPicker("##RummagePiles", &SingleColors[static_cast<int>(EOneOffs::RummagePiles)]);
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					if (ImGui::Button("Stables")) {
						Singles[static_cast<int>(EOneOffs::Stables)] =
							!Singles[static_cast<int>(EOneOffs::Stables)];
					}
					ImGui::TableNextColumn();
					ImGui::Checkbox("##Stables", &Singles[static_cast<int>(EOneOffs::Stables)]);
					ImGui::TableNextColumn();
					ImGui::ColorPicker("##Stables", &SingleColors[static_cast<int>(EOneOffs::Stables)]);
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					if (ImGui::Button("Others")) {
						bVisualizeDefault = !bVisualizeDefault;
					}
					if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip("Shows other gatherables or creatures that were not successfully categorized.");
					ImGui::TableNextColumn();
					ImGui::Checkbox("##Others", &bVisualizeDefault);
					ImGui::TableNextColumn();
					ImGui::ColorPicker("##Others", &SingleColors[static_cast<int>(EOneOffs::Others)]);
				}
				ImGui::EndTable();
			}
		}
		// ==================================== 1 Aimbots & Fun TAB
		else if (OpenTab == 1) {
			ImGui::Columns(2, nullptr, false);

			// InteliTarget Controls
			if (ImGui::CollapsingHeader("InteliTarget Settings", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Checkbox("Enable Silent Aimbot", &bEnableSilentAimbot);
				ImGui::Checkbox("Enable Legacy Aimbot", &bEnableAimbot);

				if (bEnableAimbot) {
					ImGui::Text("Aim Smoothing:");
					ImGui::SliderFloat("Smoothing Factor", &SmoothingFactor, 1.0f, 100.0f, "%1.0f");
					ImGui::Text("Aim Offset Adjustment (Drag Point):");
					const auto canvas_size = ImVec2(200, 200); // Canvas size
					static auto cursor_pos = ImVec2(0, 0); // Start at the center (0, 0 relative to center)
					constexpr float scaling_factor = 0.5f; // Reduced scaling factor for finer control

					ImU32 gridColor = IM_COL32(50, 45, 139, 255); // Grid lines color
					ImU32 gridBackgroundColor = IM_COL32(26, 28, 33, 255); // Background color
					ImU32 cursorColor = IM_COL32(69, 39, 160, 255); // Cursor color

					if (ImGui::BeginChild("GridArea", ImVec2(200, 200), false, ImGuiWindowFlags_NoScrollbar)) {
						ImDrawList* draw_list = ImGui::GetWindowDrawList();
						ImVec2 canvas_p0 = ImGui::GetCursorScreenPos(); // Top-left corner of the canvas
						auto grid_center = ImVec2(canvas_p0.x + canvas_size.x * 0.5f, canvas_p0.y + canvas_size.y * 0.5f);

						draw_list->AddRectFilled(canvas_p0, ImVec2(canvas_p0.x + canvas_size.x, canvas_p0.y + canvas_size.y), gridBackgroundColor);
						draw_list->AddLine(ImVec2(grid_center.x, canvas_p0.y), ImVec2(grid_center.x, canvas_p0.y + canvas_size.y), gridColor);
						draw_list->AddLine(ImVec2(canvas_p0.x, grid_center.y), ImVec2(canvas_p0.x + canvas_size.x, grid_center.y), gridColor);

						ImGui::SetCursorScreenPos(ImVec2(grid_center.x + cursor_pos.x - 5, grid_center.y + cursor_pos.y - 5));
						ImGui::InvisibleButton("cursor", ImVec2(10, 10));
						if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0)) {
							cursor_pos.x = ImClamp(cursor_pos.x - ImGui::GetIO().MouseDelta.x * scaling_factor, -canvas_size.x * 0.5f, canvas_size.x * 0.5f);
							cursor_pos.y = ImClamp(cursor_pos.y - ImGui::GetIO().MouseDelta.y * scaling_factor, -canvas_size.y * 0.5f, canvas_size.y * 0.5f);
						}

						draw_list->AddCircleFilled(ImVec2(grid_center.x + cursor_pos.x, grid_center.y + cursor_pos.y), 5, cursorColor, 12);

						// Sliders for fine-tuned control
						ImGui::SetCursorPosY(canvas_p0.y + canvas_size.y + 5);
						ImGui::SliderFloat2("Horizontal & Vertical", reinterpret_cast<float*>(&cursor_pos), -canvas_size.x * 0.5f, canvas_size.x * 0.5f, "H: %.1f, V: %.1f");
					}
					ImGui::EndChild();

					// Convert cursor_pos to AimOffset affecting Pitch and Yaw
					AimOffset = {cursor_pos.x * scaling_factor, cursor_pos.y * scaling_factor, 0.0f};
					ImGui::Text("Current Offset: Pitch: %.2f, Yaw: %.2f", AimOffset.X, AimOffset.Y);
				}
				ImGui::Checkbox("Teleport to Targeted", &bTeleportToTargeted);
				if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip("Teleport to the targeted entity by using your top side mouse button.");

				ImGui::Checkbox("Avoid Teleporting To Targeted Players", &bAvoidTeleportingToPlayers);
			}

			ImGui::NextColumn();

			if (ImGui::CollapsingHeader("Fun Mods - Entities", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Checkbox("Teleport Dropped Loot to Player", &bEnableLootbagTeleportation);
				if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip("Automatically teleport dropped loot to your current location.");

				ImGui::Checkbox("Target Animals to Orbit", &bAddAnimalToOrbit);
				if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip("Teleport the targeted animals into orbit around you!");
			}
		}
		// ==================================== 2 Movement & Teleport TAB
		else if (OpenTab == 2) {
			auto World = GetWorld();
			if (World) {
				auto GameInstance = World->OwningGameInstance;
				if (GameInstance && GameInstance->LocalPlayers.Num() > 0) {
					ULocalPlayer* LocalPlayer = GameInstance->LocalPlayers[0];
					if (LocalPlayer) {
						APlayerController* PlayerController = LocalPlayer->PlayerController;
						AValeriaPlayerController* ValeriaPlayerController = static_cast<AValeriaPlayerController*>(PlayerController);
						if (PlayerController && PlayerController->Pawn) {
							AValeriaCharacter* ValeriaCharacter = static_cast<AValeriaPlayerController*>(PlayerController)->GetValeriaCharacter();
							if (ValeriaCharacter) {
								AValeriaPlayerController* ValeriaPlayerController = static_cast<AValeriaPlayerController*>(PlayerController);
								UValeriaCharacterMoveComponent* MovementComponent = ValeriaCharacter->GetValeriaCharacterMovementComponent();
								UGameplayStatics* GameplayStatics = static_cast<UGameplayStatics*>(UGameplayStatics::StaticClass()->DefaultObject);
								// if (!MovementComponent) return;

								APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;

								FVector MyLocation = ValeriaCharacter->K2_GetActorLocation();
								FRotator MyRotation = ValeriaCharacter->K2_GetActorRotation();
								FVector CameraLocation = PlayerController->PlayerCameraManager->GetCameraLocation();
								FRotator CameraRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
								static FVector TeleportLocation;
								static FRotator TeleportRotate;

								const double d20 = 20., d5 = 5., d1 = 1., dhalf = 0.5;
								const float f1000 = 1000.0f, f20 = 20.f, f5 = 5.f, f1 = 1.f, fhalf = 0.5;

								// Setting the columns layout
								ImGui::Columns(2, nullptr, false);

								// Movement settings column
								if (ImGui::CollapsingHeader("Movement Settings - General", ImGuiTreeNodeFlags_DefaultOpen)) {
									ImGui::Text("Character: %s - Map: %s", ValeriaCharacter->CharacterName.ToString().c_str(), CurrentMap.c_str());

									ImGui::Spacing();
									static const char* movementModes[] = { "Walking", "Flying", "Fly No Collision" }; // Dropdown menu options

									ImGui::Checkbox("Enable Noclip", &bEnableNoclip);

									// Create a combo box for selecting the movement mode
									ImGui::Text("Movement Mode");
									ImGui::SetNextItemWidth(200.0f); // Adjust the width as needed
									if (ImGui::BeginCombo("##MovementMode", movementModes[currentMovementModeIndex])) {
										for (int n = 0; n < IM_ARRAYSIZE(movementModes); n++) {
											const bool isSelected = (currentMovementModeIndex == n);
											if (ImGui::Selectable(movementModes[n], isSelected)) {
												currentMovementModeIndex = n;
											}
											// Set the initial focus when opening the combo
											if (isSelected) {
												ImGui::SetItemDefaultFocus();
											}
										}
										ImGui::EndCombo();
									}
									ImGui::SameLine();
									// Button to apply the selected movement mode
									if (ImGui::Button("Set")) {
										switch (currentMovementModeIndex) {
										case 0: // Walking
											MovementComponent->SetMovementMode(EMovementMode::MOVE_Walking, 1);
											ValeriaCharacter->CapsuleComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
											break;
										case 1: // Swimming
											MovementComponent->SetMovementMode(EMovementMode::MOVE_Flying, 4);
											ValeriaCharacter->CapsuleComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
											break;
										case 2: // Noclip
											MovementComponent->SetMovementMode(EMovementMode::MOVE_Flying, 5);
											ValeriaCharacter->CapsuleComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
											ValeriaCharacter->CapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Ignore);
											ValeriaCharacter->CapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
											ValeriaCharacter->CapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
											ValeriaCharacter->CapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);
											ValeriaCharacter->CapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Destructible, ECollisionResponse::ECR_Ignore);
											break;
										}
									}

									const float InputWidth = 80.0f;

									// Global Game Speed with slider
									ImGui::Text("Global Game Speed: ");
									if (ImGui::InputScalar("##GlobalGameSpeed", ImGuiDataType_Float, &CustomGameSpeed, &f1, &f1000, "%.2f", ImGuiInputTextFlags_None)) {
										static_cast<UGameplayStatics*>(UGameplayStatics::StaticClass()->DefaultObject)->SetGlobalTimeDilation(World, CustomGameSpeed);
									}
									ImGui::SameLine();
									if (ImGui::Button("R##GlobalGameSpeed")) {
										CustomGameSpeed = GameSpeed;
										static_cast<UGameplayStatics*>(UGameplayStatics::StaticClass()->DefaultObject)->SetGlobalTimeDilation(World, GameSpeed);
									}

									// Walk Speed
									ImGui::Text("Walk Speed: ");
									if (ImGui::InputScalar("##WalkSpeed", ImGuiDataType_Float, &CustomWalkSpeed, &f5)) {
										MovementComponent->MaxWalkSpeed = CustomWalkSpeed;
									}
									ImGui::SameLine();
									if (ImGui::Button("R##WalkSpeed")) {
										CustomWalkSpeed = WalkSpeed;
										MovementComponent->MaxWalkSpeed = WalkSpeed;
									}

									// Sprint Speed
									ImGui::Text("Sprint Speed: ");
									if (ImGui::InputScalar("##SprintSpeedMultiplier", ImGuiDataType_Float, &CustomSprintSpeedMultiplier, &f5)) {
										MovementComponent->SprintSpeedMultiplier = CustomSprintSpeedMultiplier;
									}
									
									ImGui::SameLine();
									if (ImGui::Button("R##SprintSpeedMultiplier")) {
										CustomSprintSpeedMultiplier = SprintSpeedMultiplier;
										MovementComponent->SprintSpeedMultiplier = SprintSpeedMultiplier;
									}

									// Climbing Speed
									ImGui::Text("Climbing Speed: ");
									if (ImGui::InputScalar("##ClimbingSpeed", ImGuiDataType_Float, &CustomClimbingSpeed, &f5)) {
										MovementComponent->ClimbingSpeed = CustomClimbingSpeed;
									}
									
									ImGui::SameLine();
									if (ImGui::Button("R##ClimbingSpeed")) {
										CustomClimbingSpeed = ClimbingSpeed;
										MovementComponent->ClimbingSpeed = ClimbingSpeed;
									}

									// Gliding Speed
									ImGui::Text("Gliding Speed: ");
									if (ImGui::InputScalar("##GlidingSpeed", ImGuiDataType_Float, &CustomGlidingSpeed, &f5)) {
										MovementComponent->GlidingMaxSpeed = CustomGlidingSpeed;
									}
									
									ImGui::SameLine();
									if (ImGui::Button("R##GlidingSpeed")) {
										CustomGlidingSpeed = GlidingSpeed;
										MovementComponent->GlidingMaxSpeed = GlidingSpeed;
									}

									// Gliding Fall Speed
									ImGui::Text("Gliding Fall Speed: ");
									if (ImGui::InputScalar("##GlidingFallSpeed", ImGuiDataType_Float, &CustomGlidingFallSpeed, &f5)) {
										MovementComponent->GlidingFallSpeed = CustomGlidingFallSpeed;
									}
									
									ImGui::SameLine();
									if (ImGui::Button("R##GlidingFallSpeed")) {
										CustomGlidingFallSpeed = GlidingFallSpeed;
										MovementComponent->GlidingFallSpeed = GlidingFallSpeed;
									}

									// Jump Velocity
									ImGui::Text("Jump Velocity: ");
									if (ImGui::InputScalar("##JumpVelocity", ImGuiDataType_Float, &CustomJumpVelocity, &f5)) {
										MovementComponent->JumpZVelocity = CustomJumpVelocity;
									}
									
									ImGui::SameLine();
									if (ImGui::Button("R##JumpVelocity")) {
										CustomJumpVelocity = JumpVelocity;
										MovementComponent->JumpZVelocity = JumpVelocity;
									}

									// Step Height
									ImGui::Text("Step Height: ");
									if (ImGui::InputScalar("##MaxStepHeight", ImGuiDataType_Float, &CustomMaxStepHeight, &f5)) {
										MovementComponent->MaxStepHeight = CustomMaxStepHeight;
									}
									
									ImGui::SameLine();
									if (ImGui::Button("R##MaxStepHeight")) {
										CustomMaxStepHeight = MaxStepHeight;
										MovementComponent->MaxStepHeight = MaxStepHeight;
									}
								}

								ImGui::NextColumn();

								// Locations and exploits column
								if (ImGui::CollapsingHeader("Locations & Coordinates", ImGuiTreeNodeFlags_DefaultOpen)) {
									ImGui::Text("Teleport List");
									ImGui::Text("Double-click a location listing to teleport");
									ImGui::ListBoxHeader("##TeleportList", ImVec2(-1, 150));
									for (FLocation& Entry : TeleportLocations) {
										if (CurrentMap == Entry.MapName || Entry.MapName == "UserDefined") {
											if (ImGui::Selectable(Entry.Name.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
												if (ImGui::IsMouseDoubleClicked(0)) {
													if (Entry.Type == ELocation::Global_Home) {
														ValeriaCharacter->GetTeleportComponent()->RpcServerTeleport_Home();
													}
													else {
														//ValeriaCharacter->K2_TeleportTo(Entry.Location, Entry.Rotate);
														FHitResult HitResult;
														ValeriaCharacter->K2_SetActorLocation(Entry.Location, false, &HitResult, true);

														PlayerController->ClientForceGarbageCollection();
														PlayerController->ClientFlushLevelStreaming();
													}
												}
											}
										}
									}
									ImGui::ListBoxFooter();

									// Buttons for coordinate actions
									if (ImGui::Button("Add New Location")) {
										ImGui::OpenPopup("Add New Location");
									}

									ImGui::Text("Current Coords: %.3f, %.3f, %.3f, %.3f", ValeriaCharacter->K2_GetActorLocation().X, ValeriaCharacter->K2_GetActorLocation().Y, ValeriaCharacter->K2_GetActorLocation().Z, ValeriaCharacter->K2_GetActorRotation().Yaw);
									ImGui::Spacing();

									// Set the width for the labels and inputs
									const float labelWidth = 50.0f;
									const float inputWidth = 200.0f;

									// X Coordinate
									ImGui::AlignTextToFramePadding();
									ImGui::Text("X: ");
									ImGui::SameLine(labelWidth);
									ImGui::SetNextItemWidth(inputWidth);
									ImGui::InputScalar("##TeleportLocationX", ImGuiDataType_Double, &TeleportLocation.X, &d5);

									// Y Coordinate
									ImGui::AlignTextToFramePadding();
									ImGui::Text("Y: ");
									ImGui::SameLine(labelWidth);
									ImGui::SetNextItemWidth(inputWidth);
									ImGui::InputScalar("##TeleportLocationY", ImGuiDataType_Double, &TeleportLocation.Y, &d5);

									// Z Coordinate
									ImGui::AlignTextToFramePadding();
									ImGui::Text("Z: ");
									ImGui::SameLine(labelWidth);
									ImGui::SetNextItemWidth(inputWidth);
									ImGui::InputScalar("##TeleportLocationZ", ImGuiDataType_Double, &TeleportLocation.Z, &d5);

									// Yaw
									ImGui::AlignTextToFramePadding();
									ImGui::Text("YAW: ");
									ImGui::SameLine(labelWidth);
									ImGui::SetNextItemWidth(inputWidth);
									ImGui::InputScalar("##TeleportRotateYaw", ImGuiDataType_Double, &TeleportRotate.Yaw, &d1);

									ImGui::Spacing();
									
									if (ImGui::Button("Get Current Coordinates")) {
										TeleportLocation = ValeriaCharacter->K2_GetActorLocation();
										TeleportRotate = ValeriaCharacter->K2_GetActorRotation();
									}
									ImGui::SameLine();
									if (ImGui::Button("Teleport To Coordinates")) {
										//ValeriaCharacter->K2_TeleportTo(TeleportLocation, TeleportRotate);
										FHitResult HitResult;
										ValeriaCharacter->K2_SetActorLocation(TeleportLocation, false, &HitResult, true);

										PlayerController->ClientForceGarbageCollection();
										PlayerController->ClientFlushLevelStreaming();
									}
								}

								if (ImGui::CollapsingHeader("Gatherable Items Options")) {

									ImGui::Text("Pickable List. Double-click a pickable to teleport to it.");
									ImGui::Text("Populates from enabled Forageable ESP options.");

									// Automatically sort by name before showing the list
									std::sort(CachedActors.begin(), CachedActors.end(), [](const FEntry& a, const FEntry& b) { return a.DisplayName < b.DisplayName; });

									if (ImGui::ListBoxHeader("##PickableTeleportList", ImVec2(-1, 150))) {
										for (FEntry& Entry : CachedActors) {
											if (Entry.shouldAdd && (Entry.ActorType == EType::Forage || Entry.ActorType == EType::Loot)) {

												// Enabled ESP options only
												if (Entry.ActorType == EType::Forage && !Forageables[Entry.Type][Entry.Quality]) continue;

												if (Entry.Actor && Entry.Actor->IsValidLowLevel() && !Entry.Actor->IsDefaultObject()) {
													FVector PickableLocation = Entry.Actor->K2_GetActorLocation();
													FRotator PickableRotation = Entry.Actor->K2_GetActorRotation();

													if (ImGui::Selectable(Entry.DisplayName.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
														if (ImGui::IsMouseDoubleClicked(0)) {
															PickableLocation.Z += 150; // Raise by 150 units in the Z direction

															FHitResult PickableHitResult;
															ValeriaCharacter->K2_SetActorLocation(PickableLocation, false, &PickableHitResult, true);
														}
													}

												}
											}
										}
										ImGui::ListBoxFooter();

										// future : interact with/without being within interactable location?
										//			ValeriaCharacter->PrimaryInteractPressed();
										//			ValeriaCharacter->PrimaryInteractReleased();
									}
								}

								// Begin List adding Popup
								if (ImGui::BeginPopupModal("Add New Location", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
									static int selectedWorld = 0; // 0 for Kilima, 1 for Bahari
									static char locationName[128] = "";

									// World selection dropdown
									ImGui::Combo("World", &selectedWorld, "Kilima\0Bahari\0");
									ImGui::InputText("Location Name", locationName, IM_ARRAYSIZE(locationName));

									// Button to submit the new location
									if (ImGui::Button("Add to List")) {
										FVector newLocation = ValeriaCharacter->K2_GetActorLocation();
										FRotator newRotation = ValeriaCharacter->K2_GetActorRotation();
										std::string mapRoot = selectedWorld == 0 ? "Village_Root" : "AZ1_01_Root";
										std::string mapName = selectedWorld == 0 ? "Kilima" : "Bahari";
										std::string locationNameStr(locationName);

										TeleportLocations.push_back({ mapRoot, ELocation::UserDefined, mapName + " - " + locationNameStr + " [USER]", newLocation, newRotation});
										ImGui::CloseCurrentPopup();
									}
									ImGui::EndPopup();
								}
							}
						}
					}
				}
			}
		}
		// ==================================== 3 Selling & Items TAB
		else if (OpenTab == 3) {
			auto World = GetWorld();
			if (World) {
				auto GameInstance = World->OwningGameInstance;
				if (GameInstance && GameInstance->LocalPlayers.Num() > 0) {
					ULocalPlayer* LocalPlayer = GameInstance->LocalPlayers[0];
					if (LocalPlayer) {
						APlayerController* PlayerController = LocalPlayer->PlayerController;
						AValeriaPlayerController* ValeriaPlayerController = static_cast<AValeriaPlayerController*>(PlayerController);
						if (PlayerController && PlayerController->Pawn) {
							AValeriaCharacter* ValeriaCharacter = static_cast<AValeriaPlayerController*>(PlayerController)->GetValeriaCharacter();
							if (ValeriaCharacter) {
								const double d20 = 20., d5 = 5., d1 = 1., dhalf = 0.5, dzero = 0.;
								const float f20 = 20.f, f5 = 5.f, f1 = 1.f, fhalf = 0.5, fzero = 0.0;

								ImGui::Columns(2, nullptr, false);

								if (ImGui::CollapsingHeader("Selling Settings - Bag 1", ImGuiTreeNodeFlags_DefaultOpen))
								{
									ImGui::Text("Quickly Sell Items - Bag 1");
									ImGui::Spacing();
									ImGui::Text("Select the bag, slot, and quantity to sell.");
									ImGui::Spacing();
									static int selectedSlot = 0;
									static int selectedQuantity = 1;
									static const char* quantities[] = { "1", "10", "50", "999", "Custom" };
									static char customQuantity[64] = "100";

									// Slot selection dropdown
									if (ImGui::BeginCombo("Slot", std::to_string(selectedSlot).c_str())) {
										for (int i = 0; i < 8; i++) {
											const bool isSelected = (selectedSlot == i);
											if (ImGui::Selectable(std::to_string(i + 1).c_str(), isSelected)) {
												selectedSlot = i;
											}
											if (isSelected) {
												ImGui::SetItemDefaultFocus();
											}
										}
										ImGui::EndCombo();
									}

									// Quantity selection dropdown
									if (ImGui::BeginCombo("Quantity", quantities[selectedQuantity])) {
										for (int i = 0; i < IM_ARRAYSIZE(quantities); i++) {
											const bool isSelected = (selectedQuantity == i);
											if (ImGui::Selectable(quantities[i], isSelected)) {
												selectedQuantity = i;
											}
											if (isSelected) {
												ImGui::SetItemDefaultFocus();
											}
										}
										ImGui::EndCombo();
									}

									if (selectedQuantity == 4) {
										ImGui::InputText("##CustomQuantity", customQuantity, IM_ARRAYSIZE(customQuantity));
									}

									if (ImGui::Button("Sell Items")) {
										FBagSlotLocation bag = {};
										bag.BagIndex = 0;
										bag.SlotIndex = selectedSlot;

										int quantityToSell = selectedQuantity < 4 ? atoi(quantities[selectedQuantity]) : atoi(customQuantity);
										ValeriaCharacter->StoreComponent->RpcServer_SellItem(bag, quantityToSell);
									}
									if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip("Visit a storefront first, then the sell button will function.");
								}

								ImGui::NextColumn();

								if (ImGui::CollapsingHeader("Player Features", ImGuiTreeNodeFlags_DefaultOpen))
								{
									if (ImGui::Button("Toggle Challenge Easy Mode")) {
										ValeriaCharacter->RpcServer_ToggleDevChallengeEasyMode();
										bEasyModeActive = !bEasyModeActive;
									}
									if (bEasyModeActive) {
										ImGui::Text("CHALLENGE EASY MODE ON");
									}
									else {
										ImGui::Text("CHALLENGE EASY MODE OFF");
									}
								}

								if (ImGui::CollapsingHeader("Selling Hotkeys - Quickselling"))
								{
									ImGui::Text("Quicksell All - Bag 1 Slots");
									ImGui::Text("Visit a storefront then use the hotkeys to sell your inventory quickly");
									ImGui::Checkbox("Enable Quicksell Hotkeys", &bEnableQuicksellHotkeys);
									ImGui::Spacing();
									ImGui::Text("NUM1 - NUM8 | Sell All Items, Slots 1 through 8");

									const int numpadKeys[] = { VK_NUMPAD1, VK_NUMPAD2, VK_NUMPAD3, VK_NUMPAD4, VK_NUMPAD5, VK_NUMPAD6, VK_NUMPAD7, VK_NUMPAD8 };
									if (bEnableQuicksellHotkeys) {
										for (int i = 0; i < 8; ++i) {
											if (IsKeyHeld(numpadKeys[i])) {
												FBagSlotLocation quicksellBag = {};
												quicksellBag.BagIndex = 0;
												quicksellBag.SlotIndex = i + 1;

												ValeriaCharacter->StoreComponent->RpcServer_SellItem(quicksellBag, 5);
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
		// ==================================== 4 Skills & Tools TAB
		else if (OpenTab == 4) {
			auto World = GetWorld();
			if (World) {
				auto GameInstance = World->OwningGameInstance;
				if (GameInstance && GameInstance->LocalPlayers.Num() > 0) {
					ULocalPlayer* LocalPlayer = GameInstance->LocalPlayers[0];
					if (LocalPlayer) {
						APlayerController* PlayerController = LocalPlayer->PlayerController;
						AValeriaPlayerController* ValeriaPlayerController = static_cast<AValeriaPlayerController*>(PlayerController);
						if (PlayerController && PlayerController->Pawn) {
							AValeriaCharacter* ValeriaCharacter = static_cast<AValeriaPlayerController*>(PlayerController)->GetValeriaCharacter();
							if (ValeriaCharacter) {
								//UVAL_CharacterCustomizationComponent* CharacterCustomization = ValeriaCharacter->GetCharacterCustomizationComponent();
								UGardenPlantingComponent* GardenComponent = ValeriaCharacter->GetGardenPlanting();
								UFishingComponent* FishingComponent = ValeriaCharacter->GetFishing();

								FValeriaItem Equipped = ValeriaCharacter->GetEquippedItem();
								ETools EquippedTool = ETools::None;
								std::string EquippedName = Equipped.ItemType->Name.ToString();

								if (EquippedName.find("Tool_Axe_") != std::string::npos) {
									EquippedTool = ETools::Axe;
								}
								else if (EquippedName.find("Tool_InsectBallLauncher_") != std::string::npos) {
									EquippedTool = ETools::Belt;
								}
								else if (EquippedName.find("Tool_Bow_") != std::string::npos) {
									EquippedTool = ETools::Bow;
								}
								else if (EquippedName.find("Tool_Rod_") != std::string::npos) {
									EquippedTool = ETools::FishingRod;
								}
								else if (EquippedName.find("Tool_Hoe_") != std::string::npos) {
									EquippedTool = ETools::Hoe;
								}
								else if (EquippedName.find("Tool_Pick") != std::string::npos) {
									EquippedTool = ETools::Pick;
								}
								else if (EquippedName.find("Tool_WateringCan_") != std::string::npos) {
									EquippedTool = ETools::WateringCan;
								}

								ImGui::Columns(2, nullptr, false);

								if (ImGui::CollapsingHeader("Skill Settings - General", ImGuiTreeNodeFlags_DefaultOpen))
								{
									if (ValeriaCharacter) {
										ImGui::Text("Equipped Tool : %s", STools[(int)EquippedTool]);
									}
									else { ImGui::Text("No equipment available for viewing");}

									//ImGui::Checkbox("Unlock All Outfits", &bTempUnlockAllEntitlements);
									//if (bTempUnlockAllEntitlements) {
									//	CharacterCustomization->bTemporarilyAllowAllEntitlements = true;
									//}
									//else {
									//	CharacterCustomization->bTemporarilyAllowAllEntitlements = false;
									//}
								}

								ImGui::NextColumn();

								if (ImGui::CollapsingHeader("Fishing Settings - General", ImGuiTreeNodeFlags_DefaultOpen))
								{
									if (FishingComponent) {
										ImGui::Checkbox("Enable Instant Fishing", &bEnableInstantFishing);
										if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip("Automatically catch fish when your bobber hits the water.");

										if (EquippedTool == ETools::FishingRod) {
											ImGui::Checkbox("Auto Fishing", &bEnableAutoFishing);
											if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip("Automatically casts the fishing rod.");

											if (bEnableAutoFishing) {
												ImGui::Checkbox("Require Holding Left-Click To Auto Fish", &bRequireClickFishing);
												if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip("Require left-click to automatically recast your fishing rod.");
											}
										}
										else {
											ImGui::Spacing();
											ImGui::Text("Equip your fishing rod to see more auto-fishing options");
											ImGui::Spacing();

											bEnableAutoFishing = false;
											bRequireClickFishing = true;
										}

										ImGui::Checkbox("Always Perfect Catch", &bPerfectCatch);
										if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip("Choose whether to catch all fish perfectly or not.");

										ImGui::Checkbox("Instant Sell Fish (All Slots)", &bDoInstantSellFish);
										if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip("Visit a storefront first, then enable this fishing feature.");

										ImGui::Checkbox("Discard Other Unsellables (Slot 1)", &bDestroyCustomizationFishing);
										if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip("Discard all unsellable items such as Waterlogged chests when fishing to save inventory space.");

										ImGui::Spacing();
										ImGui::Text("Custom Fishing Catch Parameters:");
										ImGui::SliderFloat("Start Rod Health", &StartRodHealth, 0.0f, 100.0f, "%.1f");
										ImGui::SliderFloat("End Rod Health", &EndRodHealth, 0.0f, 100.0f, "%.1f");
										ImGui::SliderFloat("Start Fish Health", &StartFishHealth, 0.0f, 100.0f, "%.1f");
										ImGui::SliderFloat("End Fish Health", &EndFishHealth, 0.0f, 100.0f, "%.1f");
										ImGui::Checkbox("Capture fishing spot", &bCaptureFishingSpot);
										ImGui::Checkbox("Override fishing spot", &bOverrideFishingSpot);
										ImGui::SameLine();
										ImGui::Text("%s", sOverrideFishingSpot.ToString().c_str());
									}
								}
							}
						}
					}
				}
			}
		}
		// ==================================== 5 Housing & Decorating TAB
		else if (OpenTab == 5) {
			auto World = GetWorld();
			if (World) {
				auto GameInstance = World->OwningGameInstance;
				if (GameInstance && GameInstance->LocalPlayers.Num() > 0) {
					ULocalPlayer* LocalPlayer = GameInstance->LocalPlayers[0];
					if (LocalPlayer) {
						APlayerController* PlayerController = LocalPlayer->PlayerController;
						AValeriaPlayerController* ValeriaPlayerController = static_cast<AValeriaPlayerController*>(PlayerController);
						if (PlayerController && PlayerController->Pawn) {
							AValeriaCharacter* ValeriaCharacter = static_cast<AValeriaPlayerController*>(PlayerController)->GetValeriaCharacter();
							if (ValeriaCharacter) {
								UPlacementComponent* PlacementComponent = ValeriaCharacter->GetPlacement();

								ImGui::Columns(1, nullptr, false);
								if (ImGui::CollapsingHeader("Housing Settings - General", ImGuiTreeNodeFlags_DefaultOpen)) {
									if (PlacementComponent) {
										ImGui::Checkbox("Place Items Anywhere", &bPlaceAnywhere);
									}
									else {
										ImGui::Text("No Placement Component available.");
									}
								}
							}
						}
					}
				}
			}
		}
	}
	ImGui::End();

	if (!show)
		ShowOverlay(false);
}