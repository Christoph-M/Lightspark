// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LightInteractable/LightInteractable.h"
#include "EnvLightInteractable.generated.h"


UCLASS()
class LIGHTSPARK_API AEnvLightInteractable : public ALightInteractable
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LightInteractable", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* InteractionSphere;

public:
	AEnvLightInteractable();


	FORCEINLINE USphereComponent* const GetSphere() { return InteractionSphere; }

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;


	virtual void CheckForCharacters();

protected:
	UFUNCTION()
	void UpdateState(class AActor * OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	bool resetState;
};
