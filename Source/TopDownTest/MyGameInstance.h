#pragma once

#include "CoreMinimal.h"

#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

/**
 * GameInstance class for handling multiplayer sessions (host/join/find).
 */
UCLASS()
class TOPDOWNTEST_API UMyGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UMyGameInstance();

protected:
    
    TSharedPtr<FOnlineSessionSearch> SessionSearch;
    
    IOnlineSessionPtr SessionInterface;

    FName SESSION_NAME = FName("My Game");

    FName testKey = FName("ServerKey");
    FName testValue = FName("ServerName");

    bool bWantsToCreateSessionAfterDestroy = false;

    
    void Init() override;

    UFUNCTION(BlueprintCallable)
    void CreateServer();
    UFUNCTION(BlueprintCallable)
    void JoinServer();

    //Delegates
    void OnCreateSessionComplete(FName SessionName, bool wasSuccessful);
    void OnFindSessionComplete(bool Succeeded);
    void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
    void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);



};
