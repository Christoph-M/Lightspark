// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LightsparkCharacter/LightsparkCharacter.h"
#include "AiCharacter.generated.h"

UCLASS()
class LIGHTSPARK_API AAiCharacter : public ALightsparkCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAiCharacter();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	uint32 GetID() { return id; }

	bool IsEnabled() { return isEnabled; }

	void Disable();

protected:
	FORCEINLINE class ATriggeredActorSegmentDoor* GetSegmentDoor() const { return SegmentDoor; }

public:
	/**
	* Segment (int)
	* The level segment this NPC belongs to.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Segment", meta = (AllowPrivateAccess = "true"))
	int segment;

protected:
	uint32 id;

	class ATriggeredActorSegmentDoor* SegmentDoor;

	bool isEnabled;
};
