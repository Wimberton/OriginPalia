#include <Windows.h>

struct cfg_ESP_t
{
	// ESP Booleans
	bool bEnableESP = true;
	bool bEnableFOVCircle = true;
	bool bVisualizeDefault = false;
	bool bEnableESPCulling = true;
	
	// ESP Numericals
	int CullDistance = 220;
	int ESPFovValue = 90;

	__forceinline cfg_ESP_t* operator->() { return this; }
};

struct cfg_MovementTeleport_t
{
	// Movement Booleans
	bool bEnableGameSpeed = false;
	bool bEnableNoclip = false;
	bool bPreviousNoclipState = false;
	bool bEnableESPCulling = true;

	// Movement Numericals
	int currentMovementModeIndex = 0;
	int globalGameSpeedIndex = 0;
	int walkSpeedIndex = 0;
	int climbingSpeedIndex = 0;
	int glidingSpeedIndex = 0;
	int glidingFallSpeedIndex = 0;
	int jumpVelocityIndex = 0;
	int gravityScaleIndex = 0;

	float GlobalGameSpeed = 1.0f;
	float NoClipFlySpeed = 600.0f;

	__forceinline cfg_MovementTeleport_t* operator->() { return this; }
};

struct cfg_FishingGardening_t
{
	// Fishing Booleans
	bool bEnableInstantFishing = false;
	bool bPerfectCatch = true;
	bool bDoInstantSellFish = false;
	bool bAutoFishing = false;

	// Fishing Numericals
	float StartRodHealth = 100.0f;
	float EndRodHealth = 100.0f;
	float StartFishHealth = 100.0f;
	float EndFishHealth = 0.0f;

	// Fishing Captures
	bool bCaptureFishingSpot = false;
	bool bOverrideFishingSpot = false;

	__forceinline cfg_FishingGardening_t* operator->() { return this; }
};

struct cfg_ItemSkills_t
{
	// Fishing Booleans
	bool bEnablePremiumUnlocks = false;
	bool bEnableLootbagTeleportation = false;
	bool bEnableMagicArrow = false;
	bool bModifyArrowGravity = false;
	bool bEnableAutoGathering = false;
	bool bEnablePlantAllSeeds = false;

	float MagicArrowFOV = 30.0;
	float ArrowSpeed = 2000.0f;
	float ArrowGravityScale = 0.0f;

	__forceinline cfg_ItemSkills_t* operator->() { return this; }
};

struct cfg_Menu_t
{
	bool bShowMenu = true;
	USHORT ToggleKey = VK_INSERT;

	__forceinline cfg_Menu_t* operator->() { return this; }
};

struct cfg_t
{
	cfg_ESP_t esp;
	cfg_MovementTeleport_t movementteleport;
	cfg_FishingGardening_t fishinggardening;
	cfg_ItemSkills_t itemskills;
	cfg_Menu_t paliamenu;

	__forceinline cfg_t* operator->() { return this; }
};
inline cfg_t cfg;