// Fill out your copyright notice in the Description page of Project Settings.

#include "Lightspark.h"
#include "FriendlyAiCharacter.h"


AFriendlyAiCharacter::AFriendlyAiCharacter() {
	SprintEmpowerment = ESprintEmpowerment::None;
	JumpEmpowerment = EJumpEmpowerment::None;
}

void AFriendlyAiCharacter::Merge_Implementation() {
	this->Destroy();
}