// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Weapon.h"

// Sets default values
AWeapon::AWeapon()
{
 	// don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("ROOT"));
	SetRootComponent(Root);

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(Root);

	// Weapon don't need collision, just need tracing
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}





