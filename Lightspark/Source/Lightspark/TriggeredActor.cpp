// Fill out your copyright notice in the Description page of Project Settings.

#include "Lightspark.h"
#include "TriggeredActor.h"


// Sets default values
ATriggeredActor::ATriggeredActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InteractionMesh"));
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