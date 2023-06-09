// Sky Faller. All rights reserved.


#include "MainGameModeBase.h"
#include "Player/BaseCharacter.h"
#include "Player/BasePlayerController.h"
#include "UI/SFGameHUD.h"

AMainGameModeBase::AMainGameModeBase()
{
	DefaultPawnClass = ABaseCharacter::StaticClass();
	PlayerControllerClass = ABasePlayerController::StaticClass();
	HUDClass = ASFGameHUD::StaticClass();
}