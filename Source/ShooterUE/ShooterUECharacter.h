// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "ShooterUECharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class UEnhancedInputComponent;
class UStaminaComponent;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A basic first person character
 */
UCLASS(abstract)
class AShooterUECharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: first person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UStaminaComponent* Stamina;

	UPROPERTY(EditAnywhere, Category="Hardcore|Movement", meta=(ClampMin=0, Units="cm/s"))
	float BaseWalkSpeed = 200.0f;

	UPROPERTY(EditAnywhere, Category="Hardcore|Movement", meta=(ClampMin=0, Units="cm/s"))
	float JogSpeed = 420.0f;

	UPROPERTY(EditAnywhere, Category="Hardcore|Movement", meta=(ClampMin=0, Units="cm/s"))
	float CrouchSpeed = 130.0f;

	UPROPERTY(EditAnywhere, Category="Hardcore|Movement", meta=(ClampMin=0))
	float JogStaminaDrainRate = 10.0f;

	UPROPERTY(EditAnywhere, Category="Hardcore|Movement", meta=(ClampMin=0))
	float JumpStaminaCost = 15.0f;

	UPROPERTY(EditAnywhere, Category="Hardcore|Lean", meta=(ClampMin=0, ClampMax=45, Units="Degrees"))
	float LeanRollAngle = 12.0f;

	UPROPERTY(EditAnywhere, Category="Hardcore|Lean", meta=(ClampMin=0, Units="cm"))
	float LeanCameraOffset = 35.0f;

	UPROPERTY(EditAnywhere, Category="Hardcore|Lean", meta=(ClampMin=0.1))
	float LeanInterpSpeed = 8.0f;

	UPROPERTY(EditAnywhere, Category="Hardcore|Crouch", meta=(ClampMin=0, Units="cm"))
	float CrouchCameraDrop = 50.0f;

	UPROPERTY(EditAnywhere, Category="Hardcore|Crouch", meta=(ClampMin=0.1))
	float CrouchInterpSpeed = 10.0f;

	UPROPERTY(Transient)
	UInputMappingContext* HardcoreInputContext;

	UPROPERTY(Transient)
	UInputAction* JogAction;

	UPROPERTY(Transient)
	UInputAction* CrouchAction;

	UPROPERTY(Transient)
	UInputAction* LeanAction;

	bool bWantsToJog = false;

	float LeanInputValue = 0.0f;

	float CurrentLean = 0.0f;

	float CurrentCrouchBlend = 0.0f;

	FVector FirstPersonMeshBaseLocation = FVector::ZeroVector;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* MouseLookAction;
	
public:
	AShooterUECharacter();

	virtual void Tick(float DeltaSeconds) override;

protected:

	virtual void BeginPlay() override;

	void InitHardcoreInput(UEnhancedInputComponent* EnhancedInputComponent);

	void JogInputStarted();

	void JogInputCompleted();

	void CrouchInput();

	void LeanInput(const FInputActionValue& Value);

	bool IsJogAllowed() const;

	void UpdateMovementSpeed();

	/** Called from Input Actions for movement input */
	void MoveInput(const FInputActionValue& Value);

	/** Called from Input Actions for looking input */
	void LookInput(const FInputActionValue& Value);

	/** Handles aim inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoAim(float Yaw, float Pitch);

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles jump start inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump end inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

protected:

	/** Set up input action bindings */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	

public:

	/** Returns the first person mesh **/
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	/** Returns first person camera component **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	/** Returns the stamina component **/
	UStaminaComponent* GetStamina() const { return Stamina; }

	UFUNCTION(BlueprintPure, Category="Hardcore")
	bool IsJogging() const;
};

