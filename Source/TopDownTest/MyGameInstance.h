#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

/**
 *
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

    virtual void Init() override;

    // Function declarations
    virtual void OnCreateSessionComplete(FName SessionName, bool Succeeded);
    virtual void OnFindSessionsComplete(bool Succeeded);
    virtual void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

    UFUNCTION(BlueprintCallable)
    void CreateServer();

    UFUNCTION(BlueprintCallable)
    void JoinServer();
};
