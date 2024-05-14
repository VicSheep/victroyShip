// Fill out your copyright notice in the Description page of Project Settings.

//game
#include "OJS/FarmLifePlayableCharacter.h"

//engine
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter1);

// Sets default values
AFarmLifePlayableCharacter::AFarmLifePlayableCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	InteractionCheckFrequency = 0.1;
	InteractionCheckDistance = 225.0f;

	BaseEyeHeight = 74.0f;
}

// Called to bind functionality to input
void AFarmLifePlayableCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {


		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFarmLifePlayableCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFarmLifePlayableCharacter::Look);

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AFarmLifePlayableCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AFarmLifePlayableCharacter::StopJumping);

		// Jumping
		EnhancedInputComponent->BindAction(IntractAction, ETriggerEvent::Started, this, &AFarmLifePlayableCharacter::BeginInteract);
		EnhancedInputComponent->BindAction(IntractAction, ETriggerEvent::Completed, this, &AFarmLifePlayableCharacter::EndInteract);
		
	}
	else
	{
		UE_LOG(LogTemplateCharacter1, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

// Called when the game starts or when spawned
void AFarmLifePlayableCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFarmLifePlayableCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime) > InteractionCheckFrequency)
	{
		PerformInteractionCheck();
	}
}

void AFarmLifePlayableCharacter::PerformInteractionCheck()
{
	InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();

	FVector TraceStart{GetPawnViewLocation()};
	FVector TraceEnd{TraceStart + (GetViewRotation().Vector() * InteractionCheckDistance)};

	float LookDirection = FVector::DotProduct(GetActorForwardVector(), GetViewRotation().Vector());

	if(LookDirection > 0)
	{
	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Green, false, 1.0f, 0,2.0f);
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		FHitResult TraceHit;

		if(GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
		{
			if(TraceHit.GetActor()->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
			{
				//const float Distance = (TraceStart - TraceHit.ImpactPoint).Size();
				//if(TraceHit.GetActor() != InteractionData.CurrentInteractable && Distance <= InteractionCheckDistance)
				if(TraceHit.GetActor() != InteractionData.CurrentInteractable)
				{
					FoundInteractable(TraceHit.GetActor());
					return;
				}
				if(TraceHit.GetActor()==InteractionData.CurrentInteractable)
				{
					return;
				}
			}
		}
	}

	NoInteractableFound();
	
}

void AFarmLifePlayableCharacter::FoundInteractable(AActor* NewInteractable)
{
	if(IsInteracting())
	{
		EndInteract();
	}

	if(InteractionData.CurrentInteractable)
	{
		TargetInteractable = InteractionData.CurrentInteractable;
		TargetInteractable->EndFocus();
	}

	InteractionData.CurrentInteractable = NewInteractable;
	TargetInteractable = NewInteractable;

	TargetInteractable->BeginFocus();
}

void AFarmLifePlayableCharacter::NoInteractableFound()
{
	if(IsInteracting())
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);
	}
	if(InteractionData.CurrentInteractable)
	{
		if(IsValid(TargetInteractable.GetObject()))
		{
			TargetInteractable->EndFocus();
		}

		//hide interaction widget on the HUD

		InteractionData.CurrentInteractable = nullptr;
		TargetInteractable = nullptr;
	}
}

void AFarmLifePlayableCharacter::BeginInteract()
{
	//verify nothing has changed with the interactable state since beginning interaction
	PerformInteractionCheck();

	if(InteractionData.CurrentInteractable)
	{
		if(IsValid(TargetInteractable.GetObject()))
		{
			TargetInteractable->BeginInteract();

			if(FMath::IsNearlyZero(TargetInteractable->InteractableData.InteractionDuration, 0.1f))
			{
				Interact();
			}else
			{
				GetWorldTimerManager().SetTimer(TimerHandle_Interaction,
					this,
					&AFarmLifePlayableCharacter::Interact,
					TargetInteractable->InteractableData.InteractionDuration,
					false);
			}
		}
	}
}

void AFarmLifePlayableCharacter::EndInteract()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);

	if(IsValid(TargetInteractable.GetObject()))
	{
		TargetInteractable->EndInteract();
	}
}

void AFarmLifePlayableCharacter::Interact()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);

	if(IsValid(TargetInteractable.GetObject()))
	{
		TargetInteractable->Interact(this);
	}
}


void AFarmLifePlayableCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AFarmLifePlayableCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}