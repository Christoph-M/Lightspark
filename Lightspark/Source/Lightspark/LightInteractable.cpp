// Fill out your copyright notice in the Description page of Project Settings.

#include "Lightspark.h"
#include "LightInteractable.h"


// Sets default values
ALightInteractable::ALightInteractable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	InteractableMesh = CreateAbstractDefaultSubobject<UStaticMeshComponent>(TEXT("InteractionMesh"));
	RootComponent = InteractableMesh;
}

// Called when the game starts or when spawned
void ALightInteractable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALightInteractable::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

