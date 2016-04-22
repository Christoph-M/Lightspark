// Fill out your copyright notice in the Description page of Project Settings.

#include "Lightspark.h"
#include "EnvLightInteractablePlant.h"


AEnvLightInteractablePlant::AEnvLightInteractablePlant() {

}

void AEnvLightInteractablePlant::BeginPlay()
{
	Super::BeginPlay();

	GetMesh()->OnComponentEndOverlap.AddDynamic(this, &AEnvLightInteractablePlant::ResetState);
}

void AEnvLightInteractablePlant::ResetState(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	ChangeState(EInteractionState::Default);
}