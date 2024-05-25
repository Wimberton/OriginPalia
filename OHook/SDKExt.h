#pragma once

#include <SDK.hpp>
#include <vector>

namespace SDK
{
	UEngine* GetEngine() {
		static UEngine* GEngine = nullptr;

		if (!GEngine)
		{
			for (int i = 0; i < UObject::GObjects->Num(); i++)
			{
				UObject* Obj = UObject::GObjects->GetByIndex(i);

				if (!Obj)
					continue;

				if (Obj->IsA(UEngine::StaticClass()) && !Obj->IsDefaultObject()) 
				{
					GEngine = static_cast<UEngine*>(Obj);
					break;
				}
			}
		}

		return GEngine;
	}

	template<typename UEType = UObject>
	std::vector<UEType*> FindObjectsOfType()
	{
		std::vector<UEType*> Objects;
		UClass* Class = UEType::StaticClass();
		for (int i = 0; i < UObject::GObjects->Num(); ++i)
		{
			UObject* Obj = UObject::GObjects->GetByIndex(i);

			if (!Obj || !Obj->IsValidLowLevel()) // TODO: Figure out why sometimes we get 0xffffffffffffffff
				continue;

			if (Obj->IsA(Class) && !Obj->IsDefaultObject())
				Objects.push_back(static_cast<UEType*>(Obj));
		}

		return Objects;
	}

	template<typename UEType = AActor>
	std::vector<UEType*> FindActorsOfType(UWorld* WorldContext)
	{
		std::vector<UEType*> Actors;

		if (!WorldContext || !WorldContext->IsValidLowLevel()) return Actors;

		UClass* Class = UEType::StaticClass();
		TArray<AActor*> WorldActors = WorldContext->PersistentLevel->Actors;
		for (int i = 0; i < WorldActors.Num(); ++i)
		{
			AActor* Actor = WorldActors[i];

			if (!Actor || !Actor->IsValidLowLevel()) // TODO: Figure out why sometimes we get 0xffffffffffffffff
				continue;

			if (Actor->IsA(Class) && !Actor->IsDefaultObject())
				Actors.push_back(static_cast<UEType*>(Actor));
		}

		return Actors;
	}

	std::vector<AActor*> FindActorsOfType(UWorld* WorldContext, UClass* Class)
	{
		std::vector<AActor*> Actors;

		if (!WorldContext || !WorldContext->IsValidLowLevel()) return Actors;

		TArray<AActor*> WorldActors = WorldContext->PersistentLevel->Actors;
		for (int i = 0; i < WorldActors.Num(); ++i)
		{
			AActor* Actor = WorldActors[i];

			if (!Actor || !Actor->IsValidLowLevel()) // TODO: Figure out why sometimes we get 0xffffffffffffffff
				continue;

			if (Actor->IsA(Class) && !Actor->IsDefaultObject())
				Actors.push_back(Actor);
		}

		return Actors;
	}

	std::vector<AActor*> FindActorsOfTypes(UWorld* WorldContext, std::vector<UClass*> Classes)
	{
		std::vector<AActor*> Actors;

		if (!WorldContext || !WorldContext->IsValidLowLevel()) return Actors;

		TArray<AActor*> WorldActors = WorldContext->PersistentLevel->Actors;
		for (int i = 0; i < WorldActors.Num(); ++i)
		{
			AActor* Actor = WorldActors[i];

			if (!Actor || !Actor->IsValidLowLevel()) // TODO: Figure out why sometimes we get 0xffffffffffffffff
				continue;

			for (UClass* Class : Classes) {
				if (Actor->IsA(Class) && !Actor->IsDefaultObject())
					Actors.push_back(Actor);
			}
		}

		return Actors;
	}
	
	template<typename UEType = AActor>
	std::vector<UEType*> FindAllActorsOfType(UWorld* WorldContext)
	{
		std::vector<UEType*> Actors;

		if (!WorldContext || !WorldContext->IsValidLowLevel()) return Actors;

		UClass* Class = UEType::StaticClass();
		TArray<AActor*> WorldActors;
		static_cast<UGameplayStatics*>(UGameplayStatics::StaticClass()->DefaultObject)->GetAllActorsOfClass(WorldContext, Class, &WorldActors);
		for (int i = 0; i < WorldActors.Num(); ++i)
		{
			AActor* Actor = WorldActors[i];

			if (!Actor || !Actor->IsValidLowLevel()) // TODO: Figure out why sometimes we get 0xffffffffffffffff
				continue;

			if (Actor->IsA(Class) && !Actor->IsDefaultObject())
				Actors.push_back(static_cast<UEType*>(Actor));
		}

		return Actors;
	}

	std::vector<AActor*> FindAllActorsOfType(UWorld* WorldContext, UClass* Class)
	{
		std::vector<AActor*> Actors;

		if (!WorldContext || !WorldContext->IsValidLowLevel()) return Actors;

		TArray<AActor*> WorldActors;
		static_cast<UGameplayStatics*>(UGameplayStatics::StaticClass()->DefaultObject)->GetAllActorsOfClass(WorldContext, Class, &WorldActors);
		for (int i = 0; i < WorldActors.Num(); ++i)
		{
			AActor* Actor = WorldActors[i];

			if (!Actor || !Actor->IsValidLowLevel()) // TODO: Figure out why sometimes we get 0xffffffffffffffff
				continue;

			if (Actor->IsA(Class) && !Actor->IsDefaultObject())
				Actors.push_back(Actor);
		}

		return Actors;
	}

	std::vector<AActor*> FindAllActorsOfTypes(UWorld* WorldContext, std::vector<UClass*> Classes)
	{
		std::vector<AActor*> Actors;

		if (!WorldContext || !WorldContext->IsValidLowLevel()) return Actors;

		TArray<AActor*> WorldActors;
		for (UClass* Class : Classes) {
			static_cast<UGameplayStatics*>(UGameplayStatics::StaticClass()->DefaultObject)->GetAllActorsOfClass(WorldContext, Class, &WorldActors);
			for (int i = 0; i < WorldActors.Num(); ++i)
			{
				AActor* Actor = WorldActors[i];

				if (!Actor || !Actor->IsValidLowLevel()) // TODO: Figure out why sometimes we get 0xffffffffffffffff
					continue;

				if (Actor->IsA(Class) && !Actor->IsDefaultObject())
					Actors.push_back(Actor);
			}
		}

		return Actors;
	}

	UWorld* GetWorld() {
		if (UEngine* Engine = GetEngine()) {
			if (!Engine->GameViewport)
				return nullptr;
			return Engine->GameViewport->World;
		}
		return nullptr;
	}
}
namespace SDKExt
{
	// Define TWeakObjectPtr in the SDK namespace
	template<typename UEType>
	class TWeakObjectPtr : public SDK::FWeakObjectPtr
	{
	public:
		TWeakObjectPtr() : FWeakObjectPtr() {}
		TWeakObjectPtr(UEType* Object) { *this = Object; }
		TWeakObjectPtr(const TWeakObjectPtr& Other) : FWeakObjectPtr(Other) {}

		template<typename OtherType>
		TWeakObjectPtr(const TWeakObjectPtr<OtherType>& Other) {
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

		UEType* Get() const { return static_cast<UEType*>(FWeakObjectPtr::Get()); }
		UEType* operator->() const { return Get(); }
		bool IsValid() const { return FWeakObjectPtr::IsValid(); }
	};
}