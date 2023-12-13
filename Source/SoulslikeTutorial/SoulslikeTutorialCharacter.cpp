// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoulslikeTutorialCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "SoulAttributes.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

//DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ASoulslikeTutorialCharacter

ASoulslikeTutorialCharacter::ASoulslikeTutorialCharacter()
{
	// Set size for collision capsule
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
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComp");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<USoulAttributes>("Attributes");

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	RollTag = FGameplayTag::RequestGameplayTag(FName("Player.Action.Roll"));
	IsRollingTag = FGameplayTag::RequestGameplayTag(FName("Character.isRolling"));
	BlockTag = FGameplayTag::RequestGameplayTag(FName("Player.Action.Block"));
	IsBlockingTag = FGameplayTag::RequestGameplayTag(FName("Character.isBlocking"));
	JumpTag = FGameplayTag::RequestGameplayTag(FName("Player.Action.Jump"));
	IsAirborneTag = FGameplayTag::RequestGameplayTag(FName("Player.Action.isAirborne"));
	IsGroundedTag = FGameplayTag::RequestGameplayTag(FName("Player.Action.isGrounded"));
	AttackTag = FGameplayTag::RequestGameplayTag(FName("Character.Attack"));
}

UAbilitySystemComponent* ASoulslikeTutorialCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ASoulslikeTutorialCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (AbilitySystemComponent)
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	
	InitializeAttributes();
	GiveDefaultAbilities();
}

void ASoulslikeTutorialCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	if (AbilitySystemComponent)
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

	InitializeAttributes();
}

void ASoulslikeTutorialCharacter::InitializeAttributes()
{
	if (AbilitySystemComponent && DefaultAttributeEffect)
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributeEffect, 1, EffectContext);
		if (NewHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*NewHandle.Data.Get());
		}
	}
}

void ASoulslikeTutorialCharacter::GiveDefaultAbilities()
{
	if(HasAuthority() && AbilitySystemComponent)
		for(TSubclassOf<UGameplayAbility>& StartupAbility : DefaultAbilities)
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility.GetDefaultObject(), 1, 0));
			
}


void ASoulslikeTutorialCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}



//////////////////////////////////////////////////////////////////////////
// Input

void ASoulslikeTutorialCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ASoulslikeTutorialCharacter::MyJump);
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ASoulslikeTutorialCharacter::MyStopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASoulslikeTutorialCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASoulslikeTutorialCharacter::Look);

		// Rolling
		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Triggered, this, &ASoulslikeTutorialCharacter::Roll);

		// Attacking
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ASoulslikeTutorialCharacter::Attack);

		// Blocking
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Triggered, this, &ASoulslikeTutorialCharacter::Block);
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Ongoing, this, &ASoulslikeTutorialCharacter::Block);
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Completed, this, &ASoulslikeTutorialCharacter::StopBlocking);




	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ASoulslikeTutorialCharacter::Move(const FInputActionValue& Value)
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

void ASoulslikeTutorialCharacter::Look(const FInputActionValue& Value)
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

void ASoulslikeTutorialCharacter::MyJump(const FInputActionValue& Value)
{	
	if (!AbilitySystemComponent->HasMatchingGameplayTag(IsRollingTag))
	{
		AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(JumpTag));
		AbilitySystemComponent->RemoveLooseGameplayTag(IsGroundedTag);
		AbilitySystemComponent->RemoveLooseGameplayTag(IsBlockingTag);
		ACharacter::Jump();
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Log, TEXT("Player is rolling and can't jump"));
	}
	
}

void ASoulslikeTutorialCharacter::MyStopJumping(const FInputActionValue& Value)
{
	if (HasAuthority() && !AbilitySystemComponent->HasMatchingGameplayTag(IsRollingTag))
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(IsAirborneTag);
		AbilitySystemComponent->AddLooseGameplayTag(IsGroundedTag);
	}
	
	ACharacter::StopJumping();
}

void ASoulslikeTutorialCharacter::Roll(const FInputActionValue& Value)
{
	// if player is grounded, print message

if (GetCharacterMovement()->IsMovingOnGround())
	{
		UE_LOG(LogTemplateCharacter, Log, TEXT("Player is rolling"));
		if (HasAuthority())
		{
			AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(RollTag));
			AbilitySystemComponent->RemoveLooseGameplayTag(IsBlockingTag);
		}
			
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Log, TEXT("Player is not grounded"));
	}
}

void ASoulslikeTutorialCharacter::Attack(const FInputActionValue& Value)
{
	// if player is grounded, print message

	if (GetCharacterMovement()->IsMovingOnGround())
	{
		UE_LOG(LogTemplateCharacter, Log, TEXT("Player is attacking"));
		if (HasAuthority())
		{
			AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(AttackTag));
			AbilitySystemComponent->RemoveLooseGameplayTag(IsBlockingTag);
		}
			
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Log, TEXT("Player is not grounded"));
	}
}

void ASoulslikeTutorialCharacter::Block(const FInputActionValue& Value)
{
	// if player is grounded, print message

	if (GetCharacterMovement()->IsMovingOnGround())
	{
		UE_LOG(LogTemplateCharacter, Log, TEXT("Player is blocking"));
		if (HasAuthority() && !AbilitySystemComponent->HasMatchingGameplayTag(AttackTag))
		{
			AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(BlockTag));
		}
			
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Log, TEXT("Player is not grounded"));
	}
}

void ASoulslikeTutorialCharacter::StopBlocking(const FInputActionValue& Value)
{
	if (HasAuthority())
	{
		//AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(IsBlockingTag));
		UE_LOG(LogTemplateCharacter, Log, TEXT("STOP BLOCKING"));
		AbilitySystemComponent->RemoveLooseGameplayTag(IsBlockingTag);
	}
}




