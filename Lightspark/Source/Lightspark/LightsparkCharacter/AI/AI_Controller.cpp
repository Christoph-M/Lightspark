// Fill out your copyright notice in the Description page of Project Settings.

#include "Lightspark.h"
#include "AI_Controller.h"
#include "Lightspark/LightsparkCharacter/AI/EnemyAiCharacter.h"
#include "Lightspark/LightsparkCharacter/PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"

AAI_Controller::AAI_Controller()
{
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));

	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
}

void AAI_Controller::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);
	AEnemyAiCharacter* Char = Cast<AEnemyAiCharacter>(InPawn);

	if (Char && Char->Behavior)
	{
		BlackboardComp->InitializeBlackboard(*Char->Behavior->BlackboardAsset);

		EnemyKeyID = BlackboardComp->GetKeyID("Enemy");
		EnemyLocationID = BlackboardComp->GetKeyID("Destination");
		StartLocation = Char->GetActorLocation();

		BehaviorComp->StartTree(*Char->Behavior);
	}
}

void AAI_Controller::SetEnemy(APawn * InPawn)
{
	AEnemyAiCharacter* Char = Cast<AEnemyAiCharacter>(InPawn);

	if (Char->IsEnemyInSight())
	{
		BlackboardComp->SetValueAsObject(BlackboardComp->GetKeyName(EnemyKeyID), InPawn);
		BlackboardComp->SetValueAsVector(BlackboardComp->GetKeyName(EnemyLocationID), InPawn->GetActorLocation());
	}
	else
	{
		BlackboardComp->SetValueAsObject(BlackboardComp->GetKeyName(EnemyKeyID), NULL);
		BlackboardComp->SetValueAsVector(BlackboardComp->GetKeyName(EnemyLocationID), StartLocation);
	}
}

