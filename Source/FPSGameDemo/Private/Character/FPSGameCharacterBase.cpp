// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/FPSGameCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/Weapon.h"

// Sets default values
AFPSGameCharacterBase::AFPSGameCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Configure character movement
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->JumpZVelocity = JumpZVelocity;
	GetCharacterMovement()->AirControl = AirControl;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Configure capsule component
	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	WeaponSocketName = FName("WeaponSocket");

}

void AFPSGameCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSGameCharacterBase, CurrentHealth);
	DOREPLIFETIME(AFPSGameCharacterBase, bIsSprinting);
	DOREPLIFETIME(AFPSGameCharacterBase, bIsCrouching);
	DOREPLIFETIME(AFPSGameCharacterBase, Stamina);
}

// Called when the game starts or when spawned
void AFPSGameCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
	// Initialize health
	CurrentHealth = MaxHealth;

	// Spawn weapon
	SpawnDefaultWeapon();
}

// Called every frame
void AFPSGameCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update stamina system
	UpdateStamina(DeltaTime);
}

// Called to bind functionality to input
void AFPSGameCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AFPSGameCharacterBase::JumpInput);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AFPSGameCharacterBase::JumpInput);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFPSGameCharacterBase::MoveInput);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFPSGameCharacterBase::LookInput);

		// Sprinting
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AFPSGameCharacterBase::SprintInput);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AFPSGameCharacterBase::SprintInput);

		// Crouching
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AFPSGameCharacterBase::CrouchInput);

		// Fire
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AFPSGameCharacterBase::FireInput);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AFPSGameCharacterBase::MoveInput(const FInputActionValue& Value)
{
	// Get the Vector2D move axis
	FVector2D MovementVector = Value.Get<FVector2D>();

	// Pass the axis values to the move input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AFPSGameCharacterBase::LookInput(const FInputActionValue& Value)
{
	// Get the Vector2D look axis
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// Pass the axis values to the look input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AFPSGameCharacterBase::JumpInput(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
	{
		DoJump();
	}
	else
	{
		DoStopJumping();
	}
}

void AFPSGameCharacterBase::SprintInput(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
	{
		DoStartSprint();
	}
	else
	{
		DoStopSprint();
	}
}

void AFPSGameCharacterBase::CrouchInput(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
	{
		ToggleCrouch();
	}
}

void AFPSGameCharacterBase::FireInput(const FInputActionValue& Value)
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Fire();
	}
}

void AFPSGameCharacterBase::DoMove(float Right, float Forward)
{
	if (GetController())
	{
		// Pass the move inputs
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

void AFPSGameCharacterBase::DoLook(float Yaw, float Pitch)
{
	if (GetController())
	{
		// Pass the rotation inputs
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AFPSGameCharacterBase::DoJump()
{
	// Pass Jump to the character
	Jump();
}

void AFPSGameCharacterBase::DoStopJumping()
{
	// Pass StopJumping to the character
	StopJumping();
}

void AFPSGameCharacterBase::DoStartSprint()
{
	// Check if we can sprint
	if (!CanSprint())
	{
		return;
	}

	if (HasAuthority())
	{
		bIsSprinting = true;
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
	else
	{
		ServerDoStartSprint();
	}
}

void AFPSGameCharacterBase::DoStopSprint()
{
	if (HasAuthority())
	{
		bIsSprinting = false;
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
	else
	{
		ServerDoStopSprint();
	}
}

void AFPSGameCharacterBase::DoStartCrouch()
{
	if (HasAuthority())
	{
		bIsCrouching = true;
		GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
		Crouch();
	}
	else
	{
		ServerDoStartCrouch();
	}
}

void AFPSGameCharacterBase::DoStopCrouch()
{
	if (HasAuthority())
	{
		bIsCrouching = false;
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		UnCrouch();
	}
	else
	{
		ServerDoStopCrouch();
	}
}

void AFPSGameCharacterBase::ToggleCrouch()
{
	if (bIsCrouching)
	{
		DoStopCrouch();
	}
	else
	{
		DoStartCrouch();
	}
}

bool AFPSGameCharacterBase::CanSprint() const
{
	// Can sprint if we have enough stamina and are not crouching
	return Stamina > 0.0f && !bIsCrouching;
}

void AFPSGameCharacterBase::UpdateStamina(float DeltaTime)
{
	if (HasAuthority())
	{
		if (bIsSprinting && GetVelocity().Size() > 0.0f)
		{
			// Drain stamina while sprinting
			Stamina = FMath::Max(0.0f, Stamina - StaminaDrainRate * DeltaTime);
			
			// Stop sprinting if out of stamina
			if (Stamina <= 0.0f)
			{
				DoStopSprint();
			}
		}
		else
		{
			// Recover stamina when not sprinting
			Stamina = FMath::Min(MaxStamina, Stamina + StaminaRecoveryRate * DeltaTime);
		}
	}
}

void AFPSGameCharacterBase::OnRep_IsSprinting()
{
	// Update movement speed when sprinting state is replicated
	GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? SprintSpeed : WalkSpeed;
}

void AFPSGameCharacterBase::OnRep_IsCrouching()
{
	// Update movement speed when crouching state is replicated
	GetCharacterMovement()->MaxWalkSpeed = bIsCrouching ? CrouchSpeed : WalkSpeed;
	
	// Update crouch state
	if (bIsCrouching)
	{
		Crouch();
	}
	else
	{
		UnCrouch();
	}
}

void AFPSGameCharacterBase::OnRep_Stamina()
{
	// Handle stamina replication
	// Can add visual/audio effects here
}

void AFPSGameCharacterBase::OnRep_CurrentHealth()
{
	// Handle health replication
	// Can add visual/audio effects here
}

void AFPSGameCharacterBase::SetHealth(float NewHealth)
{
	if (HasAuthority())
	{
		CurrentHealth = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
	}
	else
	{
		ServerSetHealth(NewHealth);
	}
}

// Server RPC functions
void AFPSGameCharacterBase::ServerDoStartSprint_Implementation()
{
	DoStartSprint();
}

bool AFPSGameCharacterBase::ServerDoStartSprint_Validate()
{
	return true;
}

void AFPSGameCharacterBase::ServerDoStopSprint_Implementation()
{
	DoStopSprint();
}

bool AFPSGameCharacterBase::ServerDoStopSprint_Validate()
{
	return true;
}

void AFPSGameCharacterBase::ServerSetHealth_Implementation(float NewHealth)
{
	SetHealth(NewHealth);
}

bool AFPSGameCharacterBase::ServerSetHealth_Validate(float NewHealth)
{
	return true;
}

void AFPSGameCharacterBase::ServerDoStartCrouch_Implementation()
{
	DoStartCrouch();
}

bool AFPSGameCharacterBase::ServerDoStartCrouch_Validate()
{
	return true;
}

void AFPSGameCharacterBase::ServerDoStopCrouch_Implementation()
{
	DoStopCrouch();
}

bool AFPSGameCharacterBase::ServerDoStopCrouch_Validate()
{
	return true;
}

void AFPSGameCharacterBase::SpawnDefaultWeapon()
{
	if (GetWorld() && DefaultWeaponClass)
	{
		// 1. 设置生成参数
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;      // 设定所有者（对网络同步很重要）
		SpawnParams.Instigator = this; // 设定施加者（造成伤害时算谁的头）

		// 2. 生成 Actor (位置先不重要，马上会 Attach)
		CurrentWeapon = GetWorld()->SpawnActor<AWeapon>(
			DefaultWeaponClass, 
			GetTransform(), 
			SpawnParams
		);

		// 3. 绑定到手上
		if (CurrentWeapon && GetMesh())
		{
			// SnapToTargetNotIncludingScale: 位置和旋转对齐插槽，但缩放保持武器自己的
			FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
            
			CurrentWeapon->AttachToComponent(
				GetMesh(), 
				AttachmentRules, 
				WeaponSocketName
			);
		}
	}
}

float AFPSGameCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// 1. 必须先调用父类的实现 (它会处理一些底层逻辑)
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// 2. 如果已经死了，或者伤害为0，直接返回
	if (CurrentHealth <= 0.0f || ActualDamage <= 0.0f)
	{
		return 0.0f;
	}

	// 3. 执行扣血逻辑
	// FMath::Clamp 防止血量变成负数
	CurrentHealth = FMath::Clamp(CurrentHealth - ActualDamage, 0.0f, MaxHealth);

	UE_LOG(LogTemp, Warning, TEXT("%s took %f damage from %s. Health: %f"), 
		*GetName(), 
		ActualDamage, 
		*GetNameSafe(DamageCauser), // 打印出是哪把枪打的
		CurrentHealth
	);

	// 4. 死亡判定
	if (CurrentHealth <= 0.0f)
	{
		// Die(); // 这里调用你的死亡函数
		UE_LOG(LogTemp, Error, TEXT("Character Died!"));
	}

	// 5. 返回实际造成的伤害值
	return ActualDamage;
}