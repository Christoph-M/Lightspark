// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "LightInteractable.generated.h"

UENUM(BlueprintType)
enum class EInteractionState {
	Default,
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
	FORCEINLINE class UPointLightComponent* GetLight() const { return PointLight; }


	UFUNCTION(BlueprintPure, Category = "LightInteractable")
	EInteractionState GetCurrentState() { return CurrentState; }
	void ChangeState(EInteractionState newState);


	UFUNCTION(BlueprintNativeEvent)
	void StateChangeDefault();
	virtual void StateChangeDefault_Implementation();

	UFUNCTION(BlueprintNativeEvent)
	void StateChangeLit();
	virtual void StateChangeLit_Implementation();

	UFUNCTION(BlueprintNativeEvent)
	void StateChangeUnlit();
	virtual void StateChangeUnlit_Implementation();

	UFUNCTION(BlueprintNativeEvent)
	void StateChangeDestroyed();
	virtual void StateChangeDestroyed_Implementation();

	UFUNCTION(BlueprintNativeEvent)
	void StateChangeUnknown();
	virtual void StateChangeUnknown_Implementation();

protected:
	EInteractionState CurrentState;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LightInteractable", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* InteractableMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LightInteractable", meta = (AllowPrivateAccess = "true"))
	class UPointLightComponent* PointLight;
};
