// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AiCharacter.h"
#include "EnemyAiCharacter.generated.h"


UCLASS()
class LIGHTSPARK_API AEnemyAiCharacter : public AAiCharacter
{
	GENERATED_BODY()
	
public:
	AEnemyAiCharacter();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	virtual void EvaluateLightInteraction(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;
};
