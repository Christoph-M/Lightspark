// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "TriggeredActor.generated.h"

UCLASS()
class LIGHTSPARK_API ATriggeredActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATriggeredActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void MyBeginPlay();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;


	FORCEINLINE class UStaticMeshComponent* GetMesh() const { return Mesh; }


	uint32 GetID() { return id; }
	void SetID();


	UFUNCTION(BlueprintCallable, Category = "Trigger")
	bool HasBeenTriggered() { return hasBeenTriggered; }

	UFUNCTION(BlueprintNativeEvent)
	void Trigger(class AActor* OtherActor);
	virtual void Trigger_Implementation(class AActor* OtherActor);

protected:
	bool hasBeenTriggered;
	
private:
	uint32 id;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LightInteractable", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Mesh;
};
