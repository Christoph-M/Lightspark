// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "BTWaypointSelection.generated.h"

/**
 * 
 */
UCLASS()
class LIGHTSPARK_API UBTWaypointSelection : public UBTTaskNode
{
	GENERATED_BODY()
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Go to last WP and back", meta = (AllowPrivateAccess = true))
	bool FlipFlop;

private:

	int32 Index = -1;
	bool forward = true;
};
