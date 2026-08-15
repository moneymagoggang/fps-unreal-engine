#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StaminaComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStaminaChangedDelegate, float, Percent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FExhaustionChangedDelegate, bool, bExhausted);

UCLASS(ClassGroup=(Movement), meta=(BlueprintSpawnableComponent))
class SHOOTERUE_API UStaminaComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UStaminaComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Stamina", meta=(ClampMin=1))
	float MaxStamina = 100.0f;

	UPROPERTY(EditAnywhere, Category="Stamina", meta=(ClampMin=0))
	float RegenRate = 8.0f;

	UPROPERTY(EditAnywhere, Category="Stamina", meta=(ClampMin=0, Units="s"))
	float RegenDelay = 1.2f;

	UPROPERTY(EditAnywhere, Category="Stamina", meta=(ClampMin=0))
	float ExhaustionRecoveryThreshold = 25.0f;

	UPROPERTY(BlueprintAssignable, Category="Stamina")
	FStaminaChangedDelegate OnStaminaChanged;

	UPROPERTY(BlueprintAssignable, Category="Stamina")
	FExhaustionChangedDelegate OnExhaustionChanged;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool TryConsume(float Amount);

	void Drain(float Rate, float DeltaTime);

	UFUNCTION(BlueprintPure, Category="Stamina")
	float GetStamina() const { return CurrentStamina; }

	UFUNCTION(BlueprintPure, Category="Stamina")
	float GetStaminaPercent() const { return MaxStamina > 0.0f ? CurrentStamina / MaxStamina : 0.0f; }

	UFUNCTION(BlueprintPure, Category="Stamina")
	bool IsExhausted() const { return bExhausted; }

protected:

	virtual void BeginPlay() override;

	void SetStamina(float NewValue);

	float CurrentStamina = 0.0f;

	float LastDrainTime = -1000.0f;

	bool bExhausted = false;
};
