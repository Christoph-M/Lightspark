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
	
protected:
	UFUNCTION()
	void DoorOpened(int32 segmentt);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Waypoints, meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> cEnemyWaypoints;

	UFUNCTION()
	void PlayerSneakToggle(bool isSneaking);

	UFUNCTION(BlueprintImplementableEvent, Category = "Disabled")
	void Disabled();

private:
	UFUNCTION()
	virtual void EvaluateLightInteraction(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;

	UFUNCTION()
	void CheckPlayer(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

private:

	UFUNCTION()
		void InAttRad(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
	UFUNCTION()
		void InSensRad(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnSeePawn(APawn* OtherCharacter);

private:
	class APlayerCharacter* PlayerCharacter;

	void NotifyOtherEnemies(APawn* Pawn);

	bool EnemyInSight;
};
