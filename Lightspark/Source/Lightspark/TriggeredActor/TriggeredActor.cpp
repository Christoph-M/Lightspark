// Fill out your copyright notice in the Description page of Project Settings.

#include "Lightspark.h"
#include "TriggeredActor.h"
#include "LightsparkGameMode.h"
#include "IndexList.h"


// Sets default values
ATriggeredActor::ATriggeredActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
}

// Called when the game starts or when spawned
void ATriggeredActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATriggeredActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ATriggeredActor::Trigger(class AActor* otherActor) {
	UE_LOG(LogClass, Log, TEXT("Triggering actor: %s"), *otherActor->GetName());
}

void ATriggeredActor::SetID() {
	UIndexList* IndexListInstance = ALightsparkGameMode::LoadIndexList();

	if (IndexListInstance) {
		for (FIndexListData Entry : IndexListInstance->TriggeredActorIndexList) {
			if (this->GetActorLocation() == Entry.ActorLocation) {
				id = Entry.id;
			}
		}
	} else {
		UE_LOG(LogClass, Error, TEXT("Index List was not found!"));
	}

	UE_LOG(LogClass, Log, TEXT("TriggeredActor ID: %d"), id);
}