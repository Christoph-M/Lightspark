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
	UFUNCTION()
	virtual void MyBeginPlay();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;


	FORCEINLINE class UStaticMeshComponent* GetMesh() const { return InteractableMesh; }
	FORCEINLINE class UPointLightComponent* GetLight() const { return PointLight; }


	uint32 GetID() { return id; }
	virtual void SetID();


	class ATriggeredActor* GetTriggeredActor() const { return TriggeredActor; }
	class ALightsparkCharacter* GetTriggeredCharacter() const { return TriggeredCharacter; }

	EInteractionState GetTriggerActorOnStateChange() { return TriggerActorOnStateChange; }
	EInteractionState GetTriggerCharacterOnStateChange() { return TriggerCharacterOnStateChange; }

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
	virtual void ActivateTriggerActor();
	virtual void ActivateTriggerCharacter();

	UFUNCTION()
	void LightUp(int32 segment);

public:
	/**
	* Segment (int)
	* The level segment this light interactable belongs to.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Segment", meta = (AllowPrivateAccess = "true"))
	int segment;

protected:
	uint32 id;

	EInteractionState CurrentState;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LightInteractable", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* InteractableMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LightInteractable", meta = (AllowPrivateAccess = "true"))
	class UPointLightComponent* PointLight;

	class ATriggeredActorSegmentDoor* SegmentDoor;

	/**
	* Triggered Actor (class ATriggeredActor*)
	* Actor to be triggered by state change.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TriggeredActors", meta = (AllowPrivateAccess = "true"))
	class ATriggeredActor* TriggeredActor;

	/**
	* Triggered Character (class ALightsparkCharacter*)
	* If an enemy character should be lured, set the respective character here.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TriggeredActors", meta = (AllowPrivateAccess = "true"))
	class ALightsparkCharacter* TriggeredCharacter;

	/**
	* Trigger Actor On State Change (TEnumAsByte<EInteractionState>)
	* On which state change the Triggered Actor should be triggered. Leave Unknown if nothing should be triggered.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TriggeredActors", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<EInteractionState> TriggerActorOnStateChange;

	/**
	* Trigger Character On State Change (TEnumAsByte<EInteractionState>)
	* On which state change the Triggered Character should be triggered. Leave Unknown if nothing should be triggered.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TriggeredActors", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<EInteractionState> TriggerCharacterOnStateChange;
};
