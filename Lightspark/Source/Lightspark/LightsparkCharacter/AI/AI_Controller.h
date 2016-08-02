// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "AI_Controller.generated.h"

/**
 * 
 */
UCLASS()
class LIGHTSPARK_API AAI_Controller : public AAIController
{
	GENERATED_BODY()
	
public:

	AAI_Controller();

	UPROPERTY(transient)
		class UBlackboardComponent* BlackboardComp;

	UPROPERTY(transient)
		class UBehaviorTreeComponent* BehaviorComp;

	virtual void Possess(class APawn * InPawn);

	void SetEnemy(class APawn* InPawn);

	//UFUNCTION(BlueprintCallable, Category = Behavior)
	//	void SearchForEnemy();

	/*UFUNCTION(BlueprintCallable, Category = Sensing)
		bool GetEnemyInSight() { return EnemyInSight; };

	void SetEnemyInSight(bool b) { EnemyInSight = b; };*/

protected:

	uint8 EnemyKeyID;
	uint8 EnemyLocationID;

	/*UPROPERTY(BlueprintReadOnly, Category = Sensing)
		bool EnemyInSight;*/

private:

	FVector StartLocation;
	
	
};
