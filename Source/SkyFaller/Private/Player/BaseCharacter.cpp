// Sky Faller. All rights reserved.

#include "Player/BaseCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/BGCCharacterMovementComponent.h"
#include "Components/BGCHealthComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SFWeaponComponent.h"

ABaseCharacter::ABaseCharacter(const FObjectInitializer& ObjInit) : Super(
	ObjInit.SetDefaultSubobjectClass<UBGCCharacterMovementComponent>(CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComponent->SetupAttachment(GetMesh());
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);

	HealthComponent = CreateDefaultSubobject<UBGCHealthComponent>("HealthComponent");
	WeaponComponent = CreateDefaultSubobject<USFWeaponComponent>("WeaponComponent");
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HealthComponent) HealthComponent->OnDeath.AddDynamic(this, &ABaseCharacter::OnDeath);
	else UE_LOG(LogTemp, Warning, TEXT("Health not valid"));
}

float ABaseCharacter::GetMovementDirection() const
{
	const FVector Velocity = GetVelocity();
	if (Velocity.IsNearlyZero()) return 0.0f;

	const FVector ActorForward = GetActorForwardVector();

	const FVector VelocityNormal = Velocity.GetSafeNormal();
	const float AngleBetween = FMath::Acos(FVector::DotProduct(ActorForward, VelocityNormal));
	const FVector CrossProduct = FVector::CrossProduct(ActorForward, VelocityNormal);
	const float Degrees = FMath::RadiansToDegrees(AngleBetween);

	return CrossProduct.IsNearlyZero() ? Degrees : Degrees * FMath::Sign(CrossProduct.Z);
}

void ABaseCharacter::OnDeath()
{
	if (const auto MovementComp = GetCharacterMovement()) MovementComp->DisableMovement();

	SetLifeSpan(LifeSpanOnDeath);

	if (Controller) Controller->ChangeState(NAME_Spectating);

	if (const auto CapsuleComp = GetCapsuleComponent()) CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	if (const auto MeshComp = GetMesh())
	{
		MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		MeshComp->SetSimulatePhysics(true);
	}
}
