// Fill out your copyright notice in the Description page of Project Settings.

#include "Lightspark.h"
#include "AI_Controller.h"
#include "Lightspark/LightsparkCharacter/AI/EnemyAiCharacter.h"
#include "Lightspark/LightsparkCharacter/PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Lightspark/LightsparkCharacter/AI/EnemyWayPoint.h"
#include "Navigation/CrowdFollowingComponent.h"

AAI_Controller::AAI_Controller()
{
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));

	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));

	CFollowing = CreateDefaultSubobject<UCrowdFollowingComponent>(TEXT("CFollowing"));
}

void AAI_Controller::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);
	AEnemyAiCharacter* Char = Cast<AEnemyAiCharacter>(InPawn);

	if (Char && Char->Behavior)
	{
		BlackboardComp->InitializeBlackboard(*Char->Behavior->BlackboardAsset);

		LocationToGoKey = "LocationToGo";
		EnemyKey = "Enemy";

		BehaviorComp->StartTree(*Char->Behavior);
	}
}

void AAI_Controller::SetEnemy(APawn * InPawn)
{
	AEnemyAiCharacter* Char = Cast<AEnemyAiCharacter>(InPawn);

	BlackboardComp->SetValueAsObject(EnemyKey, InPawn);
	
}

