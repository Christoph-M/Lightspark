// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LightsparkCharacter/AI/AiCharacter.h"
#include "FriendlyAiCharacter.generated.h"


UENUM(BlueprintType)
enum class ESprintEmpowerment {
	FasterSprint,
	SprintReducedCost,
	Dash,
	Thrust,
	None
};

UENUM(BlueprintType)
enum class EJumpEmpowerment {
	DoubleJump,
	HigherJump,
	JumpReducedCost,
	Glide,
	None
};

UCLASS()
class LIGHTSPARK_API AFriendlyAiCharacter : public AAiCharacter
{
	GENERATED_BODY()

	
public:
	AFriendlyAiCharacter();

	ESprintEmpowerment GetSprintEmpowerment() { return SprintEmpowerment; }
	EJumpEmpowerment GetJumpEmpowerment() { return JumpEmpowerment; }

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	void Merge();
	void Merge_Implementation();
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ESprintEmpowerment> SprintEmpowerment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<EJumpEmpowerment> JumpEmpowerment;
};
