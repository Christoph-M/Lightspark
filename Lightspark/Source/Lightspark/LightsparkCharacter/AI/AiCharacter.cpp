// Fill out your copyright notice in the Description page of Project Settings.

#include "Lightspark.h"
#include "AiCharacter.h"


// Sets default values
AAiCharacter::AAiCharacter()
{
	GetCapsuleComponent()->ComponentTags.Add(TEXT("NPC"));

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	isEnabled = true;
}

// Called when the game starts or when spawned
void AAiCharacter::BeginPlay()
{
	Super::BeginPlay();

	
}

// Called every frame
void AAiCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Called to bind functionality to input
void AAiCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

void AAiCharacter::Disable() {
	this->SetActorHiddenInGame(true);
	this->SetActorTickEnabled(false);
	this->SetActorEnableCollision(false);
	isEnabled = false;
}