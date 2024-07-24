#pragma once
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include <vector>
#include <map>
#include <Windows.h>
#include <ShlObj.h>
#include <tchar.h>

#include "SDKExt.h"
#include <SDK/AssetRegistry_parameters.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define IsKeyHeld(key) (GetAsyncKeyState(key) & 0x8000)

inline bool IsKeyUp(const int key) {
    static bool wasKeyDown = false;
    bool isKeyDown = GetAsyncKeyState(key) & 0x8000;

    if (wasKeyDown && !isKeyDown) {
        // Key was released
        wasKeyDown = false; // Update the state
        return true;
    }

    // Update the state for the next call
    wasKeyDown = isKeyDown;
    return false;
}

inline bool IsGameWindowActive() {
    const HWND foregroundWindow = GetForegroundWindow();
    TCHAR windowClassName[256];
    GetClassName(foregroundWindow, windowClassName, sizeof(windowClassName) / sizeof(TCHAR));
    return _tcscmp(windowClassName, TEXT("UnrealWindow")) == 0;
}

inline FString CharToWide(const char* NarrowString) {
    wchar_t WideString[1024];
    MultiByteToWideChar(CP_ACP, 0, NarrowString, -1, WideString, 1024);
    return {WideString};
}

template <typename SearchType>
SearchType GetFlagSingle(std::string Text, std::map<SearchType, std::vector<std::string>>& map) {
    SearchType T = static_cast<SearchType>(0);
    for (auto& Entry : map) {
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

template <typename SearchType>
SearchType GetFlagSingleEnd(std::string Text, std::map<SearchType, std::vector<std::string>>& map) {
    SearchType T = static_cast<SearchType>(0);
    for (auto& Entry : map) {
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

template <size_t size_x>
bool AnyTrue(bool(&arr)[size_x]) {
    for (int x = 0; x < size_x; x++) {
        if (arr[x])
            return true;
    }
    return false;
}

template <size_t size_x, size_t size_y>
bool AnyTrue2D(bool(&arr)[size_x][size_y]) {
    for (int x = 0; x < size_x; x++) {
        for (int y = 0; y < size_y; y++) {
            if (arr[x][y])
                return true;
        }
    }
    return false;
}

template <size_t size_x, size_t size_y, size_t size_z>
bool AnyTrue3D(bool(&arr)[size_x][size_y][size_z]) {
    for (int x = 0; x < size_x; x++) {
        for (int y = 0; y < size_y; y++) {
            for (int z = 0; z < size_z; z++) {
                if (arr[x][y][z])
                    return true;
            }
        }
    }
    return false;
}

// Vector math utilities
namespace VectorMath {
    static FVector GetVectorForward(const FRotator& Rotation) {
        const float YawRadians = Rotation.Yaw * M_PI / 180.0f;
        const float PitchRadians = Rotation.Pitch * M_PI / 180.0f;

        const float CP = std::cos(PitchRadians);
        const float SP = std::sin(PitchRadians);
        const float CY = std::cos(YawRadians);
        const float SY = std::sin(YawRadians);

        return {CP * CY, CP * SY, SP};
    }

    static FVector GetVectorRight(const FRotator& Rotation) {
        const float YawRadians = Rotation.Yaw * M_PI / 180.0f;

        const float CY = std::cos(YawRadians - M_PI / 2); // Subtract 90 degrees to get the right vector
        const float SY = std::sin(YawRadians - M_PI / 2);

        return {CY, SY, 0.0f}; // Right vector is on the horizontal plane, so Z component is 0
    }
}

namespace CustomMath {
    constexpr float PI = 3.14159265358979323846f;

    template <typename T>
    T Clamp(const T& value, const T& min, const T& max) {
        return value < min ? min : (value > max ? max : value);
    }

    template <typename T>
    T Abs(const T& value) {
        return value < 0 ? -value : value;
    }

    inline float DegreesToRadians(const float degrees) {
        return degrees * (PI / 180.0f);
    }

    // Custom square root function
    inline float Sqrt(const float value) {
        return std::sqrt(value);
    }

    // Custom square function
    template <typename T>
    T Square(const T& value) {
        return value * value;
    }

    // Custom arccosine function
    inline float Acos(const float value) {
        return std::acos(Clamp(value, -1.0f, 1.0f));
    }

    inline float DistanceBetweenPoints(const FVector2D& Point1, const FVector2D& Point2) {
        return sqrt(pow(Point2.X - Point1.X, 2) + pow(Point2.Y - Point1.Y, 2));
    }

    // Custom radians to degrees function
    inline float RadiansToDegrees(const float radians) {
        return radians * (180.0f / PI);
    }

    inline double Fmod(const double Value, const double Mod) {
        return std::fmod(Value, Mod);
    }

    inline FRotator RInterpTo(const FRotator& Current, const FRotator& Target, double DeltaTime, float InterpSpeed) {
        // If no interpolation speed, just return the target
        if (InterpSpeed <= 0.0f) {
            return Target;
        }

        // Calculate the difference in each component
        double DeltaPitch = Target.Pitch - Current.Pitch;
        double DeltaYaw = Target.Yaw - Current.Yaw;
        double DeltaRoll = Target.Roll - Current.Roll;

        // Wrap angles to ensure the shortest path is taken
        DeltaPitch = Fmod(DeltaPitch + 180.0, 360.0) - 180.0;
        DeltaYaw = Fmod(DeltaYaw + 180.0, 360.0) - 180.0;
        DeltaRoll = Fmod(DeltaRoll + 180.0, 360.0) - 180.0;

        // Calculate the step for each component based on the interpolation speed and delta time
        double PitchStep = DeltaPitch * Clamp(InterpSpeed * DeltaTime, 0.0, 1.0);
        double YawStep = DeltaYaw * Clamp(InterpSpeed * DeltaTime, 0.0, 1.0);
        double RollStep = DeltaRoll * Clamp(InterpSpeed * DeltaTime, 0.0, 1.0);

        // Generate the new interpolated rotation
        FRotator InterpolatedRotation;
        InterpolatedRotation.Pitch = Current.Pitch + PitchStep;
        InterpolatedRotation.Yaw = Current.Yaw + YawStep;
        InterpolatedRotation.Roll = Current.Roll + RollStep;

        return InterpolatedRotation;
    }
}

//NEW, FAST SEARCHCONTAINER
#define STATIC_CLASS(CName) { SearchClasses.push_back(UObject::FindClassFast(CName)); } \

// Actor Validator
inline bool IsActorValid(const AActor* Actor) {
    if (!Actor || !Actor->IsValidLowLevel() || !Actor->RootComponent->IsValidLowLevel() || Actor->IsActorBeingDestroyed()) {
        return false;
    }
    return true;
}

inline APlayerController* CachedPlayerController = nullptr;
inline AValeriaPlayerController* CachedValeriaController = nullptr;
inline AValeriaCharacter* CachedValeriaCharacter = nullptr;

// Player Controller
inline APlayerController* GetPlayerController() {
    if (IsActorValid(CachedPlayerController)) {
        return CachedPlayerController;
    }
    
    const UWorld* World = GetWorld();
    if (!World) {
        return nullptr;
    }

    UGameInstance* GameInstance = World->OwningGameInstance;
    if (!GameInstance || GameInstance->LocalPlayers.Num() == 0) {
        return nullptr;
    }

    const ULocalPlayer* LocalPlayer = GameInstance->LocalPlayers[0];
    if (!LocalPlayer || !LocalPlayer->PlayerController || !LocalPlayer->PlayerController->Pawn) {
        return nullptr;
    }

    CachedPlayerController = LocalPlayer->PlayerController;
    return LocalPlayer->PlayerController;
}

// Valeria Controller
inline AValeriaPlayerController* GetValeriaController() {
    if (IsActorValid(CachedValeriaController)) {
        return CachedValeriaController;
    }

    APlayerController* PlayerController = GetPlayerController();
    if (!PlayerController) {
        return nullptr;
    }

    CachedValeriaController = static_cast<AValeriaPlayerController*>(PlayerController);
    return IsActorValid(CachedValeriaController) ? CachedValeriaController : nullptr;
}

// Valeria Character
inline AValeriaCharacter* GetValeriaCharacter() {
    if (IsActorValid(CachedValeriaCharacter)) {
        return CachedValeriaCharacter;
    }
    
    AValeriaPlayerController* ValeriaController = GetValeriaController();
    if (!ValeriaController) {
        return nullptr;
    }

    CachedValeriaCharacter = ValeriaController->GetValeriaCharacter();
    return IsActorValid(CachedValeriaCharacter) ? CachedValeriaCharacter : nullptr;
}

enum class ETeleportDirection {
    None,
    Forward,
    Right,
    Back,
    Left,
    Upwards,
    Downwards
};

// Load Asset PTR
inline static UFunction* GetAssetByObjectPath;
inline UObject* LoadAssetPtrFromPath(std::string Path) {
    std::wstring widePath(Path.begin(), Path.end());
    auto FS_Path = FString(widePath.data());
    auto SoftObjPath = UKismetSystemLibrary::MakeSoftObjectPath(FS_Path);
    // Try AssetRegistry first
    auto AssetRegistry = UAssetRegistryHelpers::GetAssetRegistry().GetObjectRef();
    if (AssetRegistry) {
        if (!GetAssetByObjectPath) {
            GetAssetByObjectPath = UObject::FindObject<UFunction>("Function AssetRegistry.AssetRegistry.K2_GetAssetByObjectPath");
        }
        if (GetAssetByObjectPath) {
            Params::AssetRegistry_K2_GetAssetByObjectPath Parms{};
            Parms.ObjectPath = std::move(SoftObjPath);
            Parms.bIncludeOnlyOnDiskAssets = false;
            Parms.bSkipARFilteredAssets = false;
            AssetRegistry->ProcessEvent(GetAssetByObjectPath, &Parms);
            auto AssetData = Parms.ReturnValue;
            if (UAssetRegistryHelpers::IsValid(AssetData)) {
                auto Asset = UAssetRegistryHelpers::GetAsset(AssetData);
                if (Asset) return Asset;
            }
        }
    }
    // Fall back to kismet
    auto SoftObjRef = UKismetSystemLibrary::Conv_SoftObjPathToSoftObjRef(SoftObjPath);
    UObject* Asset = UKismetSystemLibrary::LoadAsset_Blocking(SoftObjRef);
    return Asset;
}

// CanGather
inline bool IsFree(const UGatherableComponent* comp) {
    auto isFree = [](EVitalType type) {
        return type != EVitalType::Coins &&
            type != EVitalType::CommunityPoints &&
            type != EVitalType::Health &&
            type != EVitalType::MedalFishing &&
            type != EVitalType::PremiumCurrency;
        };
    return isFree(comp->VitalRequired) && isFree(comp->Vital2Required);
}

// Is Facing Utility
inline bool IsFacingTarget(const FVector& SourceLocation, const FVector& TargetLocation, const FVector& ForwardVector, float AngleThreshold) {
    FVector DirectionToTarget = (TargetLocation - SourceLocation).GetNormalized();
    float CosOfAngle = ForwardVector.GetNormalized().Dot(DirectionToTarget);
    float Angle = UKismetMathLibrary::Acos(CosOfAngle);
    float AngleDegrees = UKismetMathLibrary::RadiansToDegrees(Angle);

    return AngleDegrees <= AngleThreshold;
}

inline bool IsValidPtr(void* p)
{
    return (p >= (void*)0x10000) && (p < (void*)0x000F000000000000) && p != nullptr;
}

// void TeleportDetours::Func_DoTeleportLoot(PaliaOverlay* Overlay) {
//     if (!Configuration::bEnableLootbagTeleportation)
//         return;
//
//     auto* ValeriaCharacter = GetValeriaCharacter();
//     if (!ValeriaCharacter)
//         return;
//
//     FVector PawnLocation = ValeriaCharacter->K2_GetActorLocation();
//
//     for (auto& Entry : ActorHandler::GetActors()) {
//         if (Entry.ActorType == EType::Loot && Entry.Actor) {
//             FHitResult HitResult;
//             Entry.Actor->K2_SetActorLocation(PawnLocation, true, &HitResult, true);
//         }
//     }
// }

//
// Unused stuff, left it down here for easier retrieval if ever needed.
//

// #define StrPrinter ::_StrPrinter()
//
// class _StrPrinter : public std::string {
// public:
//     _StrPrinter() = default;
//
//     template <typename T>
//     _StrPrinter& operator<<(T&& data) {
//         _stream << std::forward<T>(data);
//         this->std::string::operator=(_stream.str());
//         return *this;
//     }
//
//     std::string operator<<(std::ostream& (*f)(std::ostream&)) const { return static_cast<std::string>(*this); }
//
// private:
//     std::stringstream _stream;
// };
//
// #define WStrPrinter ::_WStrPrinter()
//
// class _WStrPrinter : public std::wstring {
// public:
//     _WStrPrinter() = default;
//
//     template <typename T>
//     _WStrPrinter& operator<<(T&& data) {
//         _stream << std::forward<T>(data);
//         this->std::wstring::operator=(_stream.str());
//         return *this;
//     }
//
//     std::wstring operator<<(std::ostream& (*f)(std::ostream&)) const { return static_cast<std::wstring>(*this); }
//
// private:
//     std::wstringstream _stream;
// };

// struct vec3 {
//     float x, y, z;
//
//     explicit vec3(const float x = 0.0f, const float y = 0.0f, const float z = 0.0f) : x(x), y(y), z(z) {}
//
//     FVector ToFVector() const { return FVector(x, y, z); }
//
//     // Vector addition
//     vec3 operator+(const vec3& other) const { return vec3(x + other.x, y + other.y, z + other.z); }
//
//     // Vector subtraction
//     vec3 operator-(const vec3& other) const { return vec3(x - other.x, y - other.y, z - other.z); }
//
//     // Scalar multiplication
//     vec3 operator*(const float scalar) const { return vec3(x * scalar, y * scalar, z * scalar); }
//
//     // Dot product
//     float Dot(const vec3& other) const { return x * other.x + y * other.y + z * other.z; }
//
//     // Cross product
//     vec3 Cross(const vec3& other) const {
//         return vec3(
//             y * other.z - z * other.y,
//             z * other.x - x * other.z,
//             x * other.y - y * other.x
//         );
//     }
//
//     // Size of the vector
//     float Size() const { return std::sqrt(x * x + y * y + z * z); }
//
//     // Distance between two vectors
//     float Distance(const vec3& other) const { return (*this - other).Size(); }
//
//     // Convert to string (for debugging)
//     std::string ToString() const {
//         std::stringstream ss;
//         ss << "vec3(" << x << ", " << y << ", " << z << ")";
//         return ss.str();
//     }
// };

// template <typename SearchType>
// SearchType GetFlagMulti(std::string Text, std::map<SearchType, std::vector<std::string>>& map) {
//     SearchType T = static_cast<SearchType>(0);
//     for (auto& Entry : map) {
//         for (auto& Str : Entry.second) {
//             if (Text.find(Str) != std::string::npos) {
//                 T |= Entry.first;
//             }
//         }
//     }
//     return T;
// }

// Function to convert std::vector<FEntry> to TArray<FEntry>
// inline TArray<FEntry> ConvertToTArray(const std::vector<FEntry>& VectorEntries) {
//     TArray<FEntry> TArrayEntries;
//     for (const auto& Entry : VectorEntries) { TArrayEntries.Add(Entry); }
//     return TArrayEntries;
// }

// inline bool SortByName(const FEntry& a, const FEntry& b) {
//     return a.DisplayName < b.DisplayName;
// }

// union FunctionPointerUnion {
//     const void* ProcessEventPointer;
//     void (*ProcessEventFunction)(const UObject*, UFunction*, void*);
// };


// OLD, ACCURATE SEARCHCONTAINER (GATES REAPPEAR, SPECIFIC ESP NOT DETECTED IN CURRENT WORLD SOMETIMES)
/*#define STATIC_CLASS(CName, SearchContainer)                          \
{                                                                     \
    static class UClass* Clss = nullptr;                              \
    if (!Clss || !Clss->IsValidLowLevel() || Clss->IsDefaultObject()) \
        Clss = UObject::FindClassFast(CName);                         \
    SearchContainer.push_back(Clss);                                  \
}*/

