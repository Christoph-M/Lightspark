// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TriggeredActor/TriggeredActor.h"
#include "TriggeredActorSegmentDoor.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLightUpSegment, int32, segment);

UCLASS()
class LIGHTSPARK_API ATriggeredActorSegmentDoor : public ATriggeredActor
{
	GENERATED_BODY()


public:
	ATriggeredActorSegmentDoor();

	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void MyBeginPlay() override;

	virtual void Trigger_Implementation(class AActor* OtherActor) override;

	void OpenDoor();

	UFUNCTION(BlueprintNativeEvent)
	void ActivateDoor();
	void ActivateDoor_Implementation();

	UFUNCTION()
	bool IsDoorOpen() { return doorOpen; }

	UPROPERTY(BlueprintAssignable)
	FLightUpSegment OnDoorOpened;
	
public:
	/**
	* Segment (int)
	* The level segment this door belongs to.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Segment", meta = (AllowPrivateAccess = "true"))
	int segment;

private:
	/**
	* Save Spots (TArray<AEnvLightInteractableSaveSpot*>)
	* The save spots that need to be activated for this door to open.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TriggeredActors", meta = (AllowPrivateAccess = "true"))
	TArray<class AEnvLightInteractableSaveSpot*>  SaveSpots;

	/**
	* Plates Lit Up (TArray<bool>)
	* Which plates should be lit up.
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TriggeredActors", meta = (AllowPrivateAccess = "true"))
	TArray<bool> PlatesLitUp;

	/**
	* Door Open (bool)
	* Wether the door is open or not.
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TriggeredActors", meta = (AllowPrivateAccess = "true"))
	bool doorOpen;

private:
	bool doorCanBeOpened;
};
