// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShooterUECharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Movement/StaminaComponent.h"
#include "ShooterUE.h"

AShooterUECharacter::AShooterUECharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	// Create the first person mesh that will be viewed only by this character's owner
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	// Create the Camera Component	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	// configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;

	Stamina = CreateDefaultSubobject<UStaminaComponent>(TEXT("Stamina"));
}

void AShooterUECharacter::BeginPlay()
{
	Super::BeginPlay();

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->GetNavAgentPropertiesRef().bCanCrouch = true;
	Movement->SetCrouchedHalfHeight(60.0f);

	FirstPersonMeshBaseLocation = FirstPersonMesh->GetRelativeLocation();
}

void AShooterUECharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!IsPlayerControlled())
	{
		return;
	}

	if (IsJogging() && GetVelocity().SizeSquared2D() > FMath::Square(BaseWalkSpeed * 1.1f))
	{
		Stamina->Drain(JogStaminaDrainRate, DeltaSeconds);
	}

	UpdateMovementSpeed();

	const float TargetLean = IsJogging() ? 0.0f : LeanInputValue;
	CurrentLean = FMath::FInterpTo(CurrentLean, TargetLean, DeltaSeconds, LeanInterpSpeed);
	CurrentCrouchBlend = FMath::FInterpTo(CurrentCrouchBlend, bIsCrouched ? 1.0f : 0.0f, DeltaSeconds, CrouchInterpSpeed);

	const FVector WorldOffset = GetActorRightVector() * (CurrentLean * LeanCameraOffset) - FVector(0.0f, 0.0f, CurrentCrouchBlend * CrouchCameraDrop);
	const FVector LocalOffset = GetMesh()->GetComponentTransform().InverseTransformVector(WorldOffset);
	FirstPersonMesh->SetRelativeLocation(FirstPersonMeshBaseLocation + LocalOffset);

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		FRotator ControlRotation = PC->GetControlRotation();
		ControlRotation.Roll = CurrentLean * LeanRollAngle;
		PC->SetControlRotation(ControlRotation);
	}
}

bool AShooterUECharacter::IsJogging() const
{
	return bWantsToJog && !bIsCrouched && !Stamina->IsExhausted() && GetCharacterMovement()->IsMovingOnGround();
}

bool AShooterUECharacter::IsJogAllowed() const
{
	return !Stamina->IsExhausted();
}

void AShooterUECharacter::UpdateMovementSpeed()
{
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->MaxWalkSpeed = IsJogging() ? JogSpeed : BaseWalkSpeed;
	Movement->MaxWalkSpeedCrouched = CrouchSpeed;
	Movement->MaxAcceleration = 1024.0f;
	Movement->BrakingDecelerationWalking = 1500.0f;
	Movement->AirControl = 0.1f;
	Movement->JumpZVelocity = 380.0f;
}

void AShooterUECharacter::InitHardcoreInput(UEnhancedInputComponent* EnhancedInputComponent)
{
	JogAction = NewObject<UInputAction>(this, TEXT("IA_Jog_Runtime"));
	JogAction->ValueType = EInputActionValueType::Boolean;

	CrouchAction = NewObject<UInputAction>(this, TEXT("IA_Crouch_Runtime"));
	CrouchAction->ValueType = EInputActionValueType::Boolean;

	LeanAction = NewObject<UInputAction>(this, TEXT("IA_Lean_Runtime"));
	LeanAction->ValueType = EInputActionValueType::Axis1D;

	HardcoreInputContext = NewObject<UInputMappingContext>(this, TEXT("IMC_Hardcore_Runtime"));
	HardcoreInputContext->MapKey(JogAction, EKeys::LeftShift);
	HardcoreInputContext->MapKey(CrouchAction, EKeys::LeftControl);
	HardcoreInputContext->MapKey(CrouchAction, EKeys::C);
	HardcoreInputContext->MapKey(LeanAction, EKeys::E);
	FEnhancedActionKeyMapping& LeanLeftMapping = HardcoreInputContext->MapKey(LeanAction, EKeys::Q);
	LeanLeftMapping.Modifiers.Add(NewObject<UInputModifierNegate>(this));

	EnhancedInputComponent->BindAction(JogAction, ETriggerEvent::Started, this, &AShooterUECharacter::JogInputStarted);
	EnhancedInputComponent->BindAction(JogAction, ETriggerEvent::Completed, this, &AShooterUECharacter::JogInputCompleted);
	EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AShooterUECharacter::CrouchInput);
	EnhancedInputComponent->BindAction(LeanAction, ETriggerEvent::Triggered, this, &AShooterUECharacter::LeanInput);
	EnhancedInputComponent->BindAction(LeanAction, ETriggerEvent::Completed, this, &AShooterUECharacter::LeanInput);

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
			{
				Subsystem->AddMappingContext(HardcoreInputContext, 1);
			}
		}
	}
}

void AShooterUECharacter::JogInputStarted()
{
	bWantsToJog = true;
}

void AShooterUECharacter::JogInputCompleted()
{
	bWantsToJog = false;
}

void AShooterUECharacter::CrouchInput()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void AShooterUECharacter::LeanInput(const FInputActionValue& Value)
{
	LeanInputValue = FMath::Clamp(Value.Get<float>(), -1.0f, 1.0f);
}

void AShooterUECharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AShooterUECharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AShooterUECharacter::DoJumpEnd);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AShooterUECharacter::MoveInput);

		// Looking/Aiming
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AShooterUECharacter::LookInput);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AShooterUECharacter::LookInput);

		InitHardcoreInput(EnhancedInputComponent);
	}
	else
	{
		UE_LOG(LogShooterUE, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void AShooterUECharacter::MoveInput(const FInputActionValue& Value)
{
	// get the Vector2D move axis
	FVector2D MovementVector = Value.Get<FVector2D>();

	// pass the axis values to the move input
	DoMove(MovementVector.X, MovementVector.Y);

}

void AShooterUECharacter::LookInput(const FInputActionValue& Value)
{
	// get the Vector2D look axis
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// pass the axis values to the aim input
	DoAim(LookAxisVector.X, LookAxisVector.Y);

}

void AShooterUECharacter::DoAim(float Yaw, float Pitch)
{
	if (GetController())
	{
		// pass the rotation inputs
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AShooterUECharacter::DoMove(float Right, float Forward)
{
	if (GetController())
	{
		// pass the move inputs
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

void AShooterUECharacter::DoJumpStart()
{
	if (bIsCrouched)
	{
		UnCrouch();
		return;
	}

	if (!Stamina->TryConsume(JumpStaminaCost))
	{
		return;
	}

	Jump();
}

void AShooterUECharacter::DoJumpEnd()
{
	// pass StopJumping to the character
	StopJumping();
}
