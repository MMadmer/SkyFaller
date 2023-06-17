// Sky Faller. All rights reserved.


#include "Player/BaseCharacter.h"
#include "Player/Weapon/SFBaseWeapon.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SFHealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SFWeaponComponent.h"
#include "Components/SFProgressComponent.h"
#include "..\..\Public\Player\BaseCharacter.h"

DEFINE_LOG_CATEGORY_STATIC(LogBaseCharacter, All, All)

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);

	HealthComponent = CreateDefaultSubobject<USFHealthComponent>("HealthComponent");

	WeaponComponent = CreateDefaultSubobject<USFWeaponComponent>("WeaponComponent");

	ProgressComponent = CreateDefaultSubobject<USFProgressComponent>("ProgressComponent");
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	check(HealthComponent);

	HealthComponent->OnDeath.AddUObject(this, &ABaseCharacter::OnDeath);
}

void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Moving
	PlayerInputComponent->BindAxis("MoveForward", this, &ABaseCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABaseCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &ABaseCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookAround", this, &ABaseCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ABaseCharacter::Jump);

	// Shooting
	PlayerInputComponent->BindAction("Fire", IE_Pressed, WeaponComponent, &USFWeaponComponent::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, WeaponComponent, &USFWeaponComponent::StopFire);
}

bool ABaseCharacter::IsMoving() const
{
	return !GetVelocity().IsZero() && !GetCharacterMovement()->IsFalling();
}

float ABaseCharacter::GetMovementDirection() const
{
	const auto VelocityNormal = GetVelocity().GetSafeNormal();
	const auto AngleBetween = FMath::Acos(FVector::DotProduct(GetActorForwardVector(), VelocityNormal));
	const auto CrossProduct = FVector::CrossProduct(GetActorForwardVector(), VelocityNormal);
	const auto Degrees = FMath::RadiansToDegrees(AngleBetween);

	return CrossProduct.IsZero() ? Degrees : Degrees * FMath::Sign(CrossProduct.Z);
}

void ABaseCharacter::OnDeath()
{
	// UE_LOG(LogBaseCharacter, Display, TEXT("Player %s is dead"), *GetName());

	GetCharacterMovement()->DisableMovement();

	SetLifeSpan(LifeSpanOnDeath);

	if (Controller)
	{
		Controller->ChangeState(NAME_Spectating);
	}
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetSimulatePhysics(true);
}

void ABaseCharacter::MoveForward(float Amount)
{
	AddMovementInput(GetActorForwardVector(), Amount);
}

void ABaseCharacter::MoveRight(float Amount)
{
	AddMovementInput(GetActorRightVector(), Amount);
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If under kill height
	if (GetActorTransform().GetLocation().Z <= KillHeight && !HealthComponent->IsDead())
	{
		TakeDamage(HealthComponent->MaxHealth, FDamageEvent(), GetController(), this);
	}
}
