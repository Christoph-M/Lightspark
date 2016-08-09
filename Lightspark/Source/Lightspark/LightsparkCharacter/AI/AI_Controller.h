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

	UPROPERTY(VisibleAnywhere)
		class UCrowdFollowingComponent* CFollowing;

	virtual void Possess(class APawn * InPawn);

	void SetEnemy(class APawn* InPawn);


	FORCEINLINE UBlackboardComponent* GetBlackboardComp() const { return BlackboardComp; }
	
	UFUNCTION(BlueprintCallable, Category = "Waypoints")
	FORCEINLINE TArray<AActor*> GetEnemyWaypoints() { return EnemyWaypoints; }

	UFUNCTION(BlueprintCallable, Category = "Waypoints")
		void SetEnemyWaypoints(TArray<AActor*> WP) { EnemyWaypoints = WP; }

	
protected:

	FName EnemyKey;
	FName LocationToGoKey;

	TArray<AActor*> EnemyWaypoints;
		
};
