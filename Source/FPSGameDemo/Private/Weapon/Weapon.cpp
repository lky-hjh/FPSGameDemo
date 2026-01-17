// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Weapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"

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

void AWeapon::Fire()
{
	// 1. 获取拥有者 (角色) 和 它的控制器
	AActor* MyOwner = GetOwner();
	if (!MyOwner) return;

	AController* MyController = MyOwner->GetInstigatorController();
	if (!MyController) return;

	// 2. 计算射线的起点和终点 (这是 FPS 的核心数学)
	FVector EyeLocation;
	FRotator EyeRotation;
    
	// 获取玩家视角的眼睛位置和朝向 (而不是枪口，防止指哪打不哪)
	MyController->GetPlayerViewPoint(EyeLocation, EyeRotation);

	FVector TraceStart = EyeLocation;
	FVector TraceEnd = TraceStart + (EyeRotation.Vector() * MaxRange);

	// 3. 配置碰撞查询参数
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);    // 忽略枪自己
	QueryParams.AddIgnoredActor(MyOwner); // 忽略拿着枪的人 (别打到自己的脚)

	// 4. 发射射线 !
	FHitResult HitResult;
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		ECC_Visibility, // 碰撞通道，通常用 Visibility 或 Camera
		QueryParams
	);

	// 5. 处理命中
	if (bHit)
	{
		AActor* HitActor = HitResult.GetActor();
        
		// [Debug] 画一条红线，持续 2 秒，方便我们看打到了哪里
		DrawDebugLine(GetWorld(), TraceStart, HitResult.Location, FColor::Red, false, 2.0f, 0, 1.0f);
        
		if (HitActor)
		{
			// 1. 获取造成伤害的“凶手” (Instigator)
			// Instigator 通常指“开枪的人的控制器”，DamageCauser 指“这把枪”
			AController* MyOwnerController = MyOwner->GetInstigatorController();

			// 2. 调用 UE 静态库造成伤害
			UGameplayStatics::ApplyDamage(
				HitActor,                   // 受害者
				Damage,                     // 伤害数值
				MyOwnerController,          // 谁打的 (EventInstigator)
				this,                       // 用什么打的 (DamageCauser - 这里指枪)
				UDamageType::StaticClass()  // 伤害类型 (比如你可以传 UFireDamageType::StaticClass())
			);
		}
	}
	else
	{
		// 没打中，画一条通长的线
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Green, false, 2.0f, 0, 1.0f);
	}
}




