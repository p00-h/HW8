#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Character.h"
#include "SpartaCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;

UCLASS()
class SP_API ASpartaCharacter : public ACharacter
{
	GENERATED_BODY()

	float NormalSpeed;
	float SprintSpeed;
	float SprintSpeedMultiplier;

public:
	ASpartaCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

public:
	UFUNCTION(BlueprintPure, Category="Health")
	int32 GetHealth() const;
	UFUNCTION(BlueprintCallable, Category="Health")
	void AddHealth(float Amount);
	UFUNCTION(BlueprintCallable, Category="Health")
	float GetMaxHealth() const;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* OverheadWidget;

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void Move(const FInputActionValue& value);
	UFUNCTION()
	void StartJump(const FInputActionValue& value);
	UFUNCTION()
	void StopJump(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
	UFUNCTION()
	void StartSprint(const FInputActionValue& value);
	UFUNCTION()
	void StopSprint(const FInputActionValue& value);


protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Health")
	float MaxHealth;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Health")
	float Health;

	UFUNCTION(BlueprintCallable, Category="Health")
	virtual void OnDeath();

	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser
	) override;

	void UpdateOverheadHP();

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
