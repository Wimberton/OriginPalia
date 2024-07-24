#pragma once

#include <SDK.hpp>
#include <vector>

namespace SDK {
    inline UEngine* GetEngine() {
        static UEngine* GEngine = nullptr;

        if (!GEngine) {
            for (int i = 0; i < UObject::GObjects->Num(); i++) {
                UObject* Obj = UObject::GObjects->GetByIndex(i);

                if (!Obj)
                    continue;

                if (Obj->IsA(UEngine::StaticClass()) && !Obj->IsDefaultObject()) {
                    GEngine = static_cast<UEngine*>(Obj);
                    break;
                }
            }
        }

        return GEngine;
    }

    template <typename UEType = UObject>
    std::vector<UEType*> FindObjectsOfType() {
        std::vector<UEType*> Objects;
        UClass* Class = UEType::StaticClass();
        if (Class && Class->IsValidLowLevel()) { // Accessing NumElements on nullptr crashes
            for (int i = 0; i < UObject::GObjects->Num(); ++i) {
                UObject* Obj = UObject::GObjects->GetByIndex(i);

                if (!Obj || !Obj->IsValidLowLevel() || Obj->IsDefaultObject()) // TODO: Figure out why sometimes we get 0xffffffffffffffff // TODO: Are we done with the first note? (Lets look for future crashes)
                    continue;

                if (Obj->IsA(Class))
                    Objects.push_back(static_cast<UEType*>(Obj));
            }
        }
        
        return Objects;
    }

    template <typename UEType = AActor>
    std::vector<UEType*> FindActorsOfType(UWorld* WorldContext) {
        std::vector<UEType*> Actors;

        if (!WorldContext || !WorldContext->IsValidLowLevel() || !WorldContext->PersistentLevel || !WorldContext->PersistentLevel->IsValidLowLevel())
            return Actors;

        UClass* Class = UEType::StaticClass();
        TArray<AActor*> WorldActors = WorldContext->PersistentLevel->Actors;

        if (WorldActors) {
            for (int i = 0; i < WorldActors.Num(); ++i) {
                AActor* Actor = WorldActors[i];

                if (!Actor || !Actor->IsValidLowLevel()) // TODO: Figure out why sometimes we get 0xffffffffffffffff // TODO: Are we done with the first note? (Lets look for future crashes)
                    continue;

                if (Actor->IsA(Class))
                    Actors.push_back(static_cast<UEType*>(Actor));
            }
        }
        
        return Actors;
    }

    inline std::vector<AActor*> FindActorsOfType(const UWorld* WorldContext, UClass* Class) {
        std::vector<AActor*> Actors;

        if (!WorldContext || !WorldContext->IsValidLowLevel() || !WorldContext->PersistentLevel || !WorldContext->PersistentLevel->IsValidLowLevel())
            return Actors;

        TArray<AActor*> WorldActors = WorldContext->PersistentLevel->Actors;
        if (WorldActors) {
            for (int i = 0; i < WorldActors.Num(); ++i) {
                AActor* Actor = WorldActors[i];

                if (!Actor || !Actor->IsValidLowLevel()) // TODO: Figure out why sometimes we get 0xffffffffffffffff // TODO: Are we done with the first note? (Lets look for future crashes)
                    continue;

                if (Actor->IsA(Class))
                    Actors.push_back(Actor);
            }
        }
        
        return Actors;
    }

    inline std::vector<AActor*> FindActorsOfTypes(const UWorld* WorldContext, const std::vector<UClass*>& Classes) {
        std::vector<AActor*> Actors;

        if (!WorldContext || !WorldContext->IsValidLowLevel() || !WorldContext->PersistentLevel || !WorldContext->PersistentLevel->IsValidLowLevel())
            return Actors;

        TArray<AActor*> WorldActors = WorldContext->PersistentLevel->Actors;

        if (WorldActors) {
            for (int i = 0; i < WorldActors.Num(); ++i) {
                AActor* Actor = WorldActors[i];

                if (!Actor || !Actor->IsValidLowLevel()) // TODO: Figure out why sometimes we get 0xffffffffffffffff // TODO: Are we done with the first note? (Lets look for future crashes)
                    continue;

                for (UClass* Class : Classes) {
                    if (Actor->IsA(Class))
                        Actors.push_back(Actor);
                }
            }
        }
        
        return Actors;
    }

    template <typename UEType = AActor>
    std::vector<UEType*> FindAllActorsOfType(UWorld* WorldContext) {
        std::vector<UEType*> Actors;

        if (!WorldContext || !WorldContext->IsValidLowLevel() || !WorldContext->PersistentLevel || !WorldContext->PersistentLevel->IsValidLowLevel())
            return Actors;

        UClass* Class = UEType::StaticClass();
        TArray<AActor*> WorldActors;
        static_cast<UGameplayStatics*>(UGameplayStatics::StaticClass()->DefaultObject)->GetAllActorsOfClass(WorldContext, Class, &WorldActors);

        if(WorldActors) {
            for (int i = 0; i < WorldActors.Num(); ++i) {
                AActor* Actor = WorldActors[i];

                if (!Actor || !Actor->IsValidLowLevel()) // TODO: Figure out why sometimes we get 0xffffffffffffffff // TODO: Are we done with the first note? (Lets look for future crashes)
                    continue;

                if (Actor->IsA(Class))
                    Actors.push_back(static_cast<UEType*>(Actor));
            }
        }
        
        return Actors;
    }

    inline std::vector<AActor*> FindAllActorsOfType(const UWorld* WorldContext, UClass* Class) {
        std::vector<AActor*> Actors;

        if (!WorldContext || !WorldContext->IsValidLowLevel() || !WorldContext->PersistentLevel || !WorldContext->PersistentLevel->IsValidLowLevel())
            return Actors;

        TArray<AActor*> WorldActors;
        static_cast<UGameplayStatics*>(UGameplayStatics::StaticClass()->DefaultObject)->GetAllActorsOfClass(WorldContext, Class, &WorldActors);
        if(WorldActors) {
            for (int i = 0; i < WorldActors.Num(); ++i) {
                AActor* Actor = WorldActors[i];

                if (!Actor || !Actor->IsValidLowLevel()) // TODO: Figure out why sometimes we get 0xffffffffffffffff // TODO: Are we done with the first note? (Lets look for future crashes)
                    continue;

                if (Actor->IsA(Class))
                    Actors.push_back(Actor);
            }
        }
        
        return Actors;
    }

    inline std::vector<AActor*> FindAllActorsOfTypes(const UWorld* WorldContext, const std::vector<UClass*>& Classes) {
        std::vector<AActor*> Actors;

        if (!WorldContext || !WorldContext->IsValidLowLevel() || !WorldContext->PersistentLevel || !WorldContext->PersistentLevel->IsValidLowLevel())
            return Actors;

        TArray<AActor*> WorldActors;
        for (UClass* Class : Classes) {
            static_cast<UGameplayStatics*>(UGameplayStatics::StaticClass()->DefaultObject)->GetAllActorsOfClass(WorldContext, Class, &WorldActors);
            if(WorldActors) {
                for (int i = 0; i < WorldActors.Num(); ++i) {
                    AActor* Actor = WorldActors[i];

                    if (!Actor || !Actor->IsValidLowLevel()) // TODO: Figure out why sometimes we get 0xffffffffffffffff // TODO: Are we done with the first note? (Lets look for future crashes)
                        continue;

                    if (Actor->IsA(Class))
                        Actors.push_back(Actor);
                }
            }
        }

        return Actors;
    }

    inline UWorld* GetWorld() {
        if (const UEngine* Engine = GetEngine()) {
            if (!Engine->GameViewport)
                return nullptr;
            return Engine->GameViewport->World;
        }
        return nullptr;
    }

    
}

namespace SDKExt {
    // Define TWeakObjectPtr in the SDK namespace
    template <typename UEType>
    class TWeakObjectPtr : public FWeakObjectPtr {
    public:
        TWeakObjectPtr() : FWeakObjectPtr() {
        }

        explicit TWeakObjectPtr(UEType* Object) {
            *this = Object;
        }

        TWeakObjectPtr(const TWeakObjectPtr& Other) : FWeakObjectPtr(Other) {
        }

        template <typename OtherType>
        explicit TWeakObjectPtr(const TWeakObjectPtr<OtherType>& Other) {
            *this = Other.Get();
        }

        TWeakObjectPtr& operator=(UEType* Object) {
            FWeakObjectPtr::operator=(Object);
            return *this;
        }

        TWeakObjectPtr& operator=(const TWeakObjectPtr& Other) {
            FWeakObjectPtr::operator=(Other);
            return *this;
        }

        UEType* Get() const {
            return static_cast<UEType*>(FWeakObjectPtr::Get());
        }

        UEType* operator->() const {
            return Get();
        }

        bool IsValid() const {
            return FWeakObjectPtr::IsValid();
        }
    };

    // Define FFrame (TheStack)
    using FlowStackType = uint8_t[0x30];
    struct alignas(0x8) FOutParmRec {
        FProperty* Property;
        uint8* PropAddr;
        FOutParmRec* NextOutParm;
    };
    class alignas(0x8) FFrame {
    public:
        uint8_t Pad_FOutputDevice[0x10];
        UFunction* Node;
        UObject* Object;
        uint8_t* Code;
        uint8_t* Locals;

        UProperty* MostRecentProperty;
        uint8_t* MostRecentPropertyAddress;
        uint8* MostRecentPropertyContainer;
        FlowStackType FlowStack;
        FFrame* PreviousFrame;
        FOutParmRec* OutParms;
        UField* PropertyChainForCompiledIn;
        UFunction* CurrentNativeFunction;
        bool bArrayContextFailed;
    };

    // I am a madlad
    class FUObjectCreateListener
    {
    public:
        virtual ~FUObjectCreateListener() = default;
        virtual void NotifyUObjectCreated(const class UObject* Object, int32 Index) = 0;
        virtual void OnUObjectArrayShutdown() = 0;
    };

    class FUObjectDeleteListener
    {
    public:
        virtual ~FUObjectDeleteListener() = default;
        virtual void NotifyUObjectDeleted(const class UObject* Object, int32 Index) = 0;
        virtual void OnUObjectArrayShutdown() = 0;
    };

    class UObjectArray
    {
    public:
        static TArray<FUObjectCreateListener*>* GetCreateArray() {
            return reinterpret_cast<TArray<FUObjectCreateListener*>*>(InSDKUtils::GetImageBase() + Offsets::GObjects + 0x58);
        };
        static TArray<FUObjectDeleteListener*>* GetDeleteArray() {
            return reinterpret_cast<TArray<FUObjectDeleteListener*>*>(InSDKUtils::GetImageBase() + Offsets::GObjects + 0x68);
        };
        static void AddUObjectCreateListener(FUObjectCreateListener* Listener) {
            auto arr = GetCreateArray();
            if (arr->Max() == 0) {
                TArray<FUObjectCreateListener*> newArr(arr->Max() + 2);
                newArr.Add(Listener);
                *arr = newArr;
            }
            else {
                arr->Add(Listener);
            }
        };
        static void RemoveUObjectCreateListener(FUObjectCreateListener* Listener) {
            auto arr = GetCreateArray();
            for (int i = 0; i < arr->Num(); i++) {
                if ((*arr)[i] == Listener) {
                    arr->Remove(i);
                    break;
                }
            }
        };
        static void AddUObjectDeleteListener(FUObjectDeleteListener* Listener) {
            auto arr = GetDeleteArray();
            if (arr->Max() == 0) {
                TArray<FUObjectDeleteListener*> newArr(arr->Max() + 2);
                newArr.Add(Listener);
                *arr = newArr;
            }
            else {
                arr->Add(Listener);
            }
        };
        static void RemoveUObjectDeleteListener(FUObjectDeleteListener* Listener) {
            auto arr = GetDeleteArray();
            for (int i = 0; i < arr->Num(); i++) {
                if ((*arr)[i] == Listener) {
                    arr->Remove(i);
                    break;
                }
            }
        };
    };
}
