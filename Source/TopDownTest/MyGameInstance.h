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
    // Session Interface and Settings
    TSharedPtr<FOnlineSessionSearch> SessionSearch;
    IOnlineSessionPtr SessionInterface;

    // Function declarations
    virtual void Init() override;

    // Callback for session creation
    virtual void OnCreateSessionComplete(FName SessionName, bool Succeeded);

    // Callback for session finding completion
    virtual void OnFindSessionsComplete(bool Succeeded);

    // Callback for session join completion
    virtual void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

    // Host a server
    UFUNCTION(BlueprintCallable)
    void CreateServer();

    // Join an existing server
    UFUNCTION(BlueprintCallable)
    void JoinServer();

    // Search for available sessions
    UFUNCTION(BlueprintCallable)
    void SearchForSessions();

private:
    const FName SESSION_NAME = FName("Diagonal Session");

    FOnlineSessionSettings SessionSettings;
};
