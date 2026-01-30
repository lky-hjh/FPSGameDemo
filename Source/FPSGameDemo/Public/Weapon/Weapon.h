// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Character/FPSGameCharacterBase.h"
#include "Weapon.generated.h"

UCLASS()
class FPSGAMEDEMO_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

	// fire
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void Fire();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Compoents")
	class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Compoents")
	class USceneComponent* Root;

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void OnFireEffects(const FVector& HitLocation, bool bHitEnemy);

	
public:	
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }

	// fire range
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,  Category = "Stats")
	float MaxRange = 5000.f;

	// fire damage
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,  Category = "Stats")
	float Damage = 1.f;
};
