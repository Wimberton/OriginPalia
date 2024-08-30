#pragma once
#include <unordered_map>
#include <functional>
#include <unordered_set>
#include <string>
#include <set> 
#include <SDK.hpp>

class HotkeysManager {
public:
    static void SetupDetours(SDK::AValeriaCharacter* VC, SDK::AValeriaPlayerController* VPC);

    static void RegisterHotkey(const std::string& actionName, int key, const std::function<void()>& callback);
    static void CheckHotkeys(const SDK::UObject* Context, SDK::UFunction* Function, void* Parms);
    static void SetHotkey(const std::string& actionName, int key);
    static int GetHotkey(const std::string& actionName);
    static const std::unordered_map<std::string, std::pair<int, std::function<void()>>>& GetHotkeys();
    static const std::set<int>& GetDisallowedKeys();

    static void InitHotkeys();

private:
    static std::unordered_map<std::string, std::pair<int, std::function<void()>>> hotkeys;
    static std::unordered_set<int> pressedKeys;
    static std::set<int> disallowedKeys;
    static bool hotkeySelectorOpen;
};