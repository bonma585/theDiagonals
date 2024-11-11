#include "MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

UMyGameInstance::UMyGameInstance()
{
}

void UMyGameInstance::Init()
{
    Super::Init();

    if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
    {
        SessionInterface = Subsystem->GetSessionInterface();
        if (SessionInterface.IsValid())
        {
            SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMyGameInstance::OnCreateSessionComplete);
            SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMyGameInstance::OnFindSessionsComplete);
            SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMyGameInstance::OnJoinSessionComplete);
        }
    }
}

void UMyGameInstance::OnCreateSessionComplete(FName SessionName, bool Succeeded)
{
    if (Succeeded)
    {
        UWorld* World = GetWorld();
        FString SessionId = SessionInterface->GetNamedSession(SESSION_NAME)->GetSessionIdStr();
        UE_LOG(LogTemp, Warning, TEXT("Session ID: %s"), *SessionId);
        if (!ensure(World != nullptr)) { return; };
        World->ServerTravel("/Game/TopDown/Maps/TopDownMap?listen", true, TRAVEL_Absolute);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to create session: %s"), *SessionName.ToString());
    }
}

void UMyGameInstance::OnFindSessionsComplete(bool Succeeded)
{
    if (Succeeded && SessionSearch.IsValid())
    {
        TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;

        if (SearchResults.Num() > 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("Session found. Attempting to join..."));
            SessionInterface->JoinSession(0, SESSION_NAME, SearchResults[0]);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("No sessions found to join."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to find sessions."));
    }
}

void UMyGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    if (APlayerController* PController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
    {
        FString JoinAddress = "";
        SessionInterface->GetResolvedConnectString(SessionName, JoinAddress);

        if (!JoinAddress.IsEmpty())
        {
            UE_LOG(LogTemp, Warning, TEXT("Joining session at address: %s"), *JoinAddress);
            PController->ClientTravel(JoinAddress, ETravelType::TRAVEL_Absolute);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to get join address."));
        }
    }
}

void UMyGameInstance::CreateServer()
{
    if (SessionInterface.IsValid())
    {
        FName SubsystemName = IOnlineSubsystem::Get()->GetSubsystemName();

        // Check if a session already exists, and destroy it if necessary
        FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
        if (ExistingSession != nullptr)
        {
            UE_LOG(LogTemp, Warning, TEXT("Destroying existing session..."));
            SessionInterface->DestroySession(SESSION_NAME);
        }

        UE_LOG(LogTemp, Warning, TEXT("Creating server session..."));

        // Set up session settings
        FOnlineSessionSettings SessionSettings;
        SessionSettings.bIsLANMatch = (SubsystemName == "NULL");
        SessionSettings.bShouldAdvertise = true;
        SessionSettings.NumPublicConnections = 5;

        if (SubsystemName == "Steam")
        {
            SessionSettings.bUsesPresence = true;
            SessionSettings.bUseLobbiesIfAvailable = true;
        }

        // Attempt to create the session
        bool bSessionCreated = SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
        if (bSessionCreated)
        {
            UE_LOG(LogTemp, Warning, TEXT("Session creation initiated."));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to initiate session creation."));
        }
    }
}

void UMyGameInstance::JoinServer()
{
    if (SessionInterface.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("Joining server..."));

        if (!SessionSearch.IsValid())
        {
            UE_LOG(LogTemp, Warning, TEXT("No session search in progress."));
            return;
        }

        if (SessionSearch->SearchResults.Num() > 0)
        {
            SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[0]);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("No session found to join."));
        }
    }
}

void UMyGameInstance::SearchForSessions()
{
    if (SessionInterface.IsValid())
    {
        SessionSearch = MakeShared<FOnlineSessionSearch>();

        SessionSearch->MaxSearchResults = 10;  // Limit search to 10 results
        SessionSearch->QuerySettings.Set(FName(TEXT("PRESENCESEARCH")), true, EOnlineComparisonOp::Equals);

        UE_LOG(LogTemp, Warning, TEXT("Searching for sessions..."));
        SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
    }
}
