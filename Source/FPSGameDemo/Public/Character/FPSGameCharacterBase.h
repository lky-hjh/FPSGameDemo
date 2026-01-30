// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "FPSGameCharacterBase.generated.h"
class AWeapon;

UCLASS()
class FPSGAMEDEMO_API AFPSGameCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSGameCharacterBase();

	// Set attribute synchronization
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Input Actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* FireAction;
	
	// Movement properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed = 900.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float JumpZVelocity = 420.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float AirControl = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float CrouchSpeed = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MaxStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StaminaDrainRate = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StaminaRecoveryRate = 15.0f;

	// Replicated properties
	UPROPERTY(ReplicatedUsing = OnRep_IsSprinting, BlueprintReadOnly, Category = "Movement")
	bool bIsSprinting = false;

	UPROPERTY(ReplicatedUsing = OnRep_IsCrouching, BlueprintReadOnly, Category = "Movement")
	bool bIsCrouching = false;

	UPROPERTY(ReplicatedUsing = OnRep_Stamina, BlueprintReadOnly, Category = "Movement")
	float Stamina = 100.0f;

	// Input handling functions
	void MoveInput(const FInputActionValue& Value);
	void LookInput(const FInputActionValue& Value);
	void JumpInput(const FInputActionValue& Value);
	void SprintInput(const FInputActionValue& Value);
	void CrouchInput(const FInputActionValue& Value);
	void FireInput(const FInputActionValue& Value);

	// Movement functions
	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual void DoMove(float Right, float Forward);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual void DoLook(float Yaw, float Pitch);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual void DoJump();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual void DoStopJumping();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual void DoStartSprint();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual void DoStopSprint();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual void DoStartCrouch();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual void DoStopCrouch();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual void ToggleCrouch();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual bool CanSprint() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual void UpdateStamina(float DeltaTime);

	// Replication callbacks
	UFUNCTION()
	void OnRep_IsSprinting();

	UFUNCTION()
	void OnRep_IsCrouching();

	UFUNCTION()
	void OnRep_Stamina();

	// Health system (from previous implementation)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_CurrentHealth, Category = "Stats")
	float CurrentHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth = 100.0f;

	UFUNCTION()
	void OnRep_CurrentHealth();

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void SetHealth(float NewHealth);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	float GetHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintCallable, Category = "Stats")
	float GetHealthPercent() const { return MaxHealth > 0 ? CurrentHealth / MaxHealth : 0.0f; }

	// Server RPC functions
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDoStartSprint();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDoStopSprint();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDoStartCrouch();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDoStopCrouch();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetHealth(float NewHealth);
	
	// Weapon system
	// TSubclassOf restricts the selection to only subclasses of AWeapon.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<AWeapon> DefaultWeaponClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Compoents")
	AWeapon* CurrentWeapon;

	// Name of the hand attachment point
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	FName WeaponSocketName;

	// 
	void SpawnDefaultWeapon();

	virtual float TakeDamage(
		float DamageAmount, 
		struct FDamageEvent const& DamageEvent, 
		class AController* EventInstigator, 
		AActor* DamageCauser
	) override;
};
