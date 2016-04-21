// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "LightInteractable.generated.h"

UENUM(BlueprintType)
enum class EInteractionState {
	Lit,
	Unlit,
	Destroyed,
	Unknown
};

UCLASS()
class LIGHTSPARK_API ALightInteractable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALightInteractable();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;


	FORCEINLINE class UStaticMeshComponent* GetMesh() const { return InteractableMesh; }


	UFUNCTION(BlueprintPure, Category = "LightInteractable")
	EInteractionState GetCurrentState() { return CurrentState; }
	void SetCurrentState(EInteractionState state) { CurrentState = state; }

protected:
	EInteractionState CurrentState;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LightInteractable", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* InteractableMesh;
};
