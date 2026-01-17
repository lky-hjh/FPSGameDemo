// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class FPSGAMEDEMO_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

	// fire
	virtual void Fire();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Compoents")
	class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Compoents")
	class USceneComponent* Root;
	
public:	
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }

	// fire range
	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float MaxRange = 5000.f;

	// fire damage
	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float Damage = 20.f;
};
