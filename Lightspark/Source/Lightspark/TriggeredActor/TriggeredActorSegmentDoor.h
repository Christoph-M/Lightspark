// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TriggeredActor/TriggeredActor.h"
#include "TriggeredActorSegmentDoor.generated.h"


UCLASS()
class LIGHTSPARK_API ATriggeredActorSegmentDoor : public ATriggeredActor
{
	GENERATED_BODY()
	
	
public:
	ATriggeredActorSegmentDoor();

	virtual void Trigger_Implementation(class AActor* OtherActor) override;

	UFUNCTION(BlueprintNativeEvent)
	void OpenDoor();
	void OpenDoor_Implementation();
	
private:
	/**
	* Save Spots (TArray<AEnvLightInteractableSaveSpot*>)
	* The save spots that need to be activated for this door to open.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TriggeredActors", meta = (AllowPrivateAccess = "true"))
	TArray<class AEnvLightInteractableSaveSpot*>  SaveSpots;
};
