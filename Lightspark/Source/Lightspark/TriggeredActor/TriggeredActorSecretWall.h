// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TriggeredActor/TriggeredActor.h"
#include "TriggeredActorSecretWall.generated.h"


UCLASS()
class LIGHTSPARK_API ATriggeredActorSecretWall : public ATriggeredActor
{
	GENERATED_BODY()
	
	
public:
	ATriggeredActorSecretWall();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	FORCEINLINE class UBoxComponent* GetBox() { return Box; }

private:
	UFUNCTION()
	void DestroyWall(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LightInteractable", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* Box;
	
};
