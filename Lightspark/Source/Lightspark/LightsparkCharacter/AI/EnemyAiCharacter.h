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
	virtual void PostInitializeComponents() override;

	void SetID();

	UPROPERTY(EditAnywhere, Category = Behavior)
		class UBehaviorTree* Behavior;

	UPROPERTY(EditAnywhere, Category = Sensing)
		class UPawnSensingComponent* PawnSensor;

	UPROPERTY(EditAnywhere, Category = Sensing)
		class USphereComponent* SensingRadius;

	UPROPERTY(EditAnywhere, Category = Sensing)
		class USphereComponent* AttentionRadius;

	//bool IsEnemyInSight() { return EnemyInSight; };

protected:
	UFUNCTION()
	void DoorOpened(int32 segmentt);

	UFUNCTION()
	void PlayerSneakToggle(bool isSneaking);

private:
	UFUNCTION()
	virtual void EvaluateLightInteraction(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;

	UFUNCTION()
	void CheckPlayer(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

private:
	class APlayerCharacter* PlayerCharacter;

	UFUNCTION()
		void InAttRad(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
	UFUNCTION()
		void InSensRad(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnSeePawn(APawn* OtherCharacter);
};
