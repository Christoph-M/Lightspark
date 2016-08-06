// Fill out your copyright notice in the Description page of Project Settings.

#include "Lightspark.h"
#include "TriggeredActor.h"
#include "LightsparkGameMode.h"
#include "IndexList.h"
#include "LightsparkSaveGame.h"


// Sets default values
ATriggeredActor::ATriggeredActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	hasBeenTriggered = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
}

// Called when the game starts or when spawned
void ATriggeredActor::BeginPlay()
{
	Super::BeginPlay();
	
	this->SetID();

	/*ALightsparkGameMode* GameModeInstance = Cast<ALightsparkGameMode>(GetWorld()->GetAuthGameMode());

	if (GameModeInstance && !GameModeInstance->OnGameModeBeginPlay.IsAlreadyBound(this, &ATriggeredActor::MyBeginPlay)) {
		GameModeInstance->OnGameModeBeginPlay.AddDynamic(this, &ATriggeredActor::MyBeginPlay);
	}*/
}

void ATriggeredActor::MyBeginPlay() {
	UE_LOG(LogClass, Warning, TEXT("%s"), *this->GetName());
}

void ATriggeredActor::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	ALightsparkGameMode* GameModeInstance = Cast<ALightsparkGameMode>(GetWorld()->GetAuthGameMode());

	if (GameModeInstance && !GameModeInstance->OnGameModeBeginPlay.IsAlreadyBound(this, &ATriggeredActor::MyBeginPlay)) {
		GameModeInstance->OnGameModeBeginPlay.RemoveDynamic(this, &ATriggeredActor::MyBeginPlay);
	}

	Super::EndPlay(EndPlayReason);
}

// Called every frame
void ATriggeredActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ATriggeredActor::Trigger_Implementation(AActor* OtherActor) {
	hasBeenTriggered = true;
	UE_LOG(LogClass, Log, TEXT("Triggering actor: %s"), *OtherActor->GetName());
}

void ATriggeredActor::SetID() {
	ALightsparkGameMode* GameMode = Cast<ALightsparkGameMode>(GetWorld()->GetAuthGameMode());
	UIndexList* IndexListInstance = GameMode->LoadIndexList();

	for (FIndexListData Entry : IndexListInstance->TriggeredActorIndexList) {
		if (this->GetActorLocation() == Entry.ActorLocation) {
			id = Entry.id;
		}
	}

	UE_LOG(LogClass, Log, TEXT("TriggeredActor ID: %d"), id);
}