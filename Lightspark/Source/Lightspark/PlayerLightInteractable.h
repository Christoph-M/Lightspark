// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LightInteractable.h"
#include "PlayerLightInteractable.generated.h"


UENUM(BlueprintType)
enum class EInteractionType {
	SpendConsume,
	SpendOnly,
	ConsumeOnly,
	NoInteraction,
	Unknown
};

UCLASS()
class LIGHTSPARK_API APlayerLightInteractable : public ALightInteractable
{
	GENERATED_BODY()
	
	
public:
	APlayerLightInteractable();

	
	EInteractionType GetInteractionType() { return InteractionType; }

protected:
	virtual void ActivateTriggerActor() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<EInteractionType> InteractionType;
};
