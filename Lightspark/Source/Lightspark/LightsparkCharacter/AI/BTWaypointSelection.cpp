// Fill out your copyright notice in the Description page of Project Settings.

#include "Lightspark.h"
#include "BTWaypointSelection.h"
#include "Lightspark/LightsparkCharacter/AI/EnemyWayPoint.h"
#include "Lightspark/LightsparkCharacter/AI/AI_Controller.h"
#include "Lightspark/LightsparkCharacter/AI/EnemyAiCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTWaypointSelection::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	AAI_Controller* AICon = Cast<AAI_Controller>(OwnerComp.GetAIOwner());

	if (AICon)
	{
		UBlackboardComponent* BlackboardComp = AICon->GetBlackboardComp();
		AEnemyWayPoint* CurrentPoint = Cast<AEnemyWayPoint>(BlackboardComp->GetValueAsObject("LocationToGo"));

		TArray<AActor*> AvailableTargetPoints = AICon->GetEnemyWaypoints();

		AEnemyWayPoint* NextWayPoint = nullptr;

		if (AvailableTargetPoints.Num() != 0) 
		{
			if (AvailableTargetPoints.Num() == 1)
			{
				BlackboardComp->SetValueAsObject("LocationToGo", AvailableTargetPoints[0]);
			}
			else 
			{
				do
				{
					if (!FlipFlop)
					{
						if (Index < AvailableTargetPoints.Num() - 1)
							Index++;
						else
							Index = 0;

						NextWayPoint = Cast<AEnemyWayPoint>(AvailableTargetPoints[Index]);
					}
					else
					{
						if (forward) {
							if (Index < AvailableTargetPoints.Num() - 1) {
								Index++;
							}
							else {
								forward = false;
							}
						}
						else {
							if (Index > 0) {
								Index--;
							}
							else
							{
								forward = true;
							}
						}

						NextWayPoint = Cast<AEnemyWayPoint>(AvailableTargetPoints[Index]);
					}

				} while (CurrentPoint == NextWayPoint);

				BlackboardComp->SetValueAsObject("LocationToGo", NextWayPoint);
			}
		}

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}


