#pragma once
#include <string>
#include <sstream>
#include <fstream>
#include <functional>
#include <cmath>
#include <vector>
#include <map>
#include <Windows.h>
#include <ShlObj.h>
#include <tchar.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define IsKeyHeld(key) (GetAsyncKeyState(key) & 0x8000)
#define StrPrinter ::_StrPrinter()

class _StrPrinter : public std::string {
public:
    _StrPrinter() {}

    template <typename T>
    _StrPrinter& operator<<(T&& data) {
        _stream << std::forward<T>(data);
        this->std::string::operator=(_stream.str());
        return *this;
    }

    std::string operator<<(std::ostream& (*f)(std::ostream&)) const { return *this; }

private:
    std::stringstream _stream;
};

#define WStrPrinter ::_WStrPrinter()

class _WStrPrinter : public std::wstring {
public:
    _WStrPrinter() {}

    template <typename T>
    _WStrPrinter& operator<<(T&& data) {
        _stream << std::forward<T>(data);
        this->std::wstring::operator=(_stream.str());
        return *this;
    }

    std::wstring operator<<(std::ostream& (*f)(std::ostream&)) const { return static_cast<std::wstring>(*this); }

private:
    std::wstringstream _stream;
};

struct vec3 {
    float x, y, z;

    explicit vec3(const float x = 0.0f, const float y = 0.0f, const float z = 0.0f) : x(x), y(y), z(z) {}

    FVector ToFVector() const { return FVector(x, y, z); }

    // Vector addition
    vec3 operator+(const vec3& other) const { return vec3(x + other.x, y + other.y, z + other.z); }

    // Vector subtraction
    vec3 operator-(const vec3& other) const { return vec3(x - other.x, y - other.y, z - other.z); }

    // Scalar multiplication
    vec3 operator*(const float scalar) const { return vec3(x * scalar, y * scalar, z * scalar); }

    // Dot product
    float Dot(const vec3& other) const { return x * other.x + y * other.y + z * other.z; }

    // Cross product
    vec3 Cross(const vec3& other) const {
        return vec3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    // Size of the vector
    float Size() const { return std::sqrt(x * x + y * y + z * z); }

    // Distance between two vectors
    float Distance(const vec3& other) const { return (*this - other).Size(); }

    // Convert to string (for debugging)
    std::string ToString() const {
        std::stringstream ss;
        ss << "vec3(" << x << ", " << y << ", " << z << ")";
        return ss.str();
    }
};

inline FString CharToWide(const char* NarrowString) {
    wchar_t WideString[1024];
    MultiByteToWideChar(CP_ACP, 0, NarrowString, -1, WideString, 1024);
    return FString(WideString);
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

template <typename SearchType>
SearchType GetFlagMulti(std::string Text, std::map<SearchType, std::vector<std::string>>& map) {
    SearchType T = static_cast<SearchType>(0);
    for (auto& Entry : map) {
        for (auto& Str : Entry.second) { if (Text.find(Str) != std::string::npos) { T |= Entry.first; } }
    }
    return T;
}

template <size_t size_x>
bool AnyTrue(bool (&arr)[size_x]) {
    for (int x = 0; x < size_x; x++) {
        if (arr[x])
            return true;
    }
    return false;
}

template <size_t size_x, size_t size_y>
bool AnyTrue2D(bool (&arr)[size_x][size_y]) {
    for (int x = 0; x < size_x; x++) {
        for (int y = 0; y < size_y; y++) {
            if (arr[x][y])
                return true;
        }
    }
    return false;
}

template <size_t size_x, size_t size_y, size_t size_z>
bool AnyTrue3D(bool (&arr)[size_x][size_y][size_z]) {
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

// Function to convert std::vector<FEntry> to TArray<FEntry>
inline TArray<FEntry> ConvertToTArray(const std::vector<FEntry>& VectorEntries) {
    TArray<FEntry> TArrayEntries;
    for (const auto& Entry : VectorEntries) { TArrayEntries.Add(Entry); }
    return TArrayEntries;
}

inline bool SortByName(const FEntry& a, const FEntry& b) {
    return a.DisplayName < b.DisplayName;
}

inline bool IsGameWindowActive() {
    const HWND foregroundWindow = GetForegroundWindow();
    TCHAR windowClassName[256];
    GetClassName(foregroundWindow, windowClassName, sizeof(windowClassName) / sizeof(TCHAR));
    return _tcscmp(windowClassName, TEXT("UnrealWindow")) == 0;
}

#define STATIC_CLASS(CName)                   \
{                                             \
    static class UClass* Clss = nullptr;      \
    if (!Clss || !Clss->IsValidLowLevel())    \
        Clss = UObject::FindClassFast(CName); \
    SearchClass = Clss;                       \
}

#define STATIC_CLASS_MULT(CName)              \
{                                             \
    static class UClass* Clss = nullptr;      \
    if (!Clss || !Clss->IsValidLowLevel())    \
        Clss = UObject::FindClassFast(CName); \
    SearchClasses.push_back(Clss);            \
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

        return FVector(CP * CY, CP * SY, SP);
    }

    static FVector GetVectorRight(const FRotator& Rotation) {
        const float YawRadians = Rotation.Yaw * M_PI / 180.0f;

        const float CY = std::cos(YawRadians - M_PI / 2); // Subtract 90 degrees to get the right vector
        const float SY = std::sin(YawRadians - M_PI / 2);

        return FVector(CY, SY, 0.0f); // Right vector is on the horizontal plane, so Z component is 0
    }
}

namespace CustomMath {

    constexpr float PI = 3.14159265358979323846f;

    template <typename T>
    inline T Clamp(const T& value, const T& min, const T& max) {
        return value < min ? min : (value > max ? max : value);
    }

    template <typename T>
    inline T Abs(const T& value) { return value < 0 ? -value : value; }

    inline float DegreesToRadians(float degrees) { return degrees * (PI / 180.0f); }

    // Custom square root function
    inline float Sqrt(float value) { return std::sqrt(value); }

    // Custom square function
    template <typename T>
    inline T Square(const T& value) { return value * value; }

    // Custom arccosine function
    inline float Acos(float value) { return std::acos(Clamp(value, -1.0f, 1.0f)); }

    float DistanceBetweenPoints(const FVector2D& Point1, const FVector2D& Point2) {
        return sqrt(pow(Point2.X - Point1.X, 2) + pow(Point2.Y - Point1.Y, 2));
    }

    // Custom radians to degrees function
    inline float RadiansToDegrees(float radians) { return radians * (180.0f / PI); }

    inline double Fmod(double Value, double Mod) { return std::fmod(Value, Mod); }

    FRotator RInterpTo(const FRotator& Current, const FRotator& Target, double DeltaTime, float InterpSpeed) {
        // If no interpolation speed, just return the target
        if (InterpSpeed <= 0.0f) { return Target; }

        // Calculate the difference in each component
        double DeltaPitch = Target.Pitch - Current.Pitch;
        double DeltaYaw = Target.Yaw - Current.Yaw;
        double DeltaRoll = Target.Roll - Current.Roll;

        // Wrap angles to ensure shortest path is taken
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
