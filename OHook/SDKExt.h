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
}
