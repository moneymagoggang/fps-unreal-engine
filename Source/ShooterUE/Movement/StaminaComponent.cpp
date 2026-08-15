#include "StaminaComponent.h"

UStaminaComponent::UStaminaComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UStaminaComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentStamina = MaxStamina;
}

void UStaminaComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentStamina < MaxStamina && GetWorld()->GetTimeSeconds() - LastDrainTime >= RegenDelay)
	{
		SetStamina(CurrentStamina + RegenRate * DeltaTime);
	}
}

bool UStaminaComponent::TryConsume(float Amount)
{
	if (bExhausted || CurrentStamina < Amount)
	{
		return false;
	}

	LastDrainTime = GetWorld()->GetTimeSeconds();
	SetStamina(CurrentStamina - Amount);
	return true;
}

void UStaminaComponent::Drain(float Rate, float DeltaTime)
{
	LastDrainTime = GetWorld()->GetTimeSeconds();
	SetStamina(CurrentStamina - Rate * DeltaTime);
}

void UStaminaComponent::SetStamina(float NewValue)
{
	const float OldValue = CurrentStamina;
	CurrentStamina = FMath::Clamp(NewValue, 0.0f, MaxStamina);

	if (CurrentStamina != OldValue)
	{
		OnStaminaChanged.Broadcast(GetStaminaPercent());
	}

	if (!bExhausted && CurrentStamina <= 0.0f)
	{
		bExhausted = true;
		OnExhaustionChanged.Broadcast(true);
	}
	else if (bExhausted && CurrentStamina >= ExhaustionRecoveryThreshold)
	{
		bExhausted = false;
		OnExhaustionChanged.Broadcast(false);
	}
}
