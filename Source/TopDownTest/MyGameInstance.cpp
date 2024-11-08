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
        FString SessionId = SessionInterface->GetNamedSession(SESSION_NAME)->GetSessionIdStr();

        UE_LOG(LogTemp, Warning, TEXT("Session ID: %s"), *SessionId);
        GetWorld()->ServerTravel("/Game/TopDown/Maps/TopDownMap?listen");
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to create session: %s"), *SessionName.ToString());
    }
}

/*
void UMyGameInstance::OnFindSessionsComplete(bool Succeeded)
{
    if (Succeeded && SessionSearch.IsValid())
    {
        TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;

        if (SearchResults.Num() > 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("Session found. Attempting to join..."));
            SessionInterface->JoinSession(0, FName("Diagonal Session"), SearchResults[0]);
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
*/

void UMyGameInstance::OnFindSessionComplete(bool bSucceeded)
{

    if (!bSucceeded) {
        return;
    }

    TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;


    if (SearchResults.Num()) {
        UE_LOG(LogTemp, Warning, TEXT("LISTING SESSIONS"));
        UE_LOG(LogTemp, Warning, TEXT("-----------"));


        for (FOnlineSessionSearchResult i : SearchResults) {
            UE_LOG(LogTemp, Warning, TEXT("Owning User Name: %s"), *FString(i.Session.OwningUserName));
        }

        SessionInterface->JoinSession(0, SESSION_NAME, SearchResults[0]);

    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("No sessions found"));
    }
}

/*
void UMyGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    if (Result == EOnJoinSessionCompleteResult::Success)
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
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to join session: %s"), *FString::Printf(TEXT("%d"), (int32)Result));
    }
}
*/

void UMyGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    if (APlayerController* PController = UGameplayStatics::GetPlayerController(GetWorld(), 0)) {
        FString JoinAddress = "";
        SessionInterface->GetResolvedConnectString(SessionName, JoinAddress);

        if (JoinAddress != "") {
            PController->ClientTravel(JoinAddress, ETravelType::TRAVEL_Absolute);
        }
    }
}


void UMyGameInstance::CreateServer()
{
    if (SessionInterface.IsValid())
    {
        FName SubsystemName = IOnlineSubsystem::Get()->GetSubsystemName();

        // Check if a session already exists, and destroy it if necessary
        FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(FName("Diagonal Session"));
        if (ExistingSession != nullptr)
        {
            UE_LOG(LogTemp, Warning, TEXT("Destroying existing session..."));
            SessionInterface->DestroySession(FName("Diagonal Session"));
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

        // Define the session name
        const FName SESSION_NAME = FName("Diagonal Session");

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
        UE_LOG(LogTemp, Warning, TEXT("Initiating session search..."));

        // Create the session search object
        SessionSearch = MakeShareable(new FOnlineSessionSearch());

        // Set LAN query based on the subsystem name
        FName SubsystemName = IOnlineSubsystem::Get()->GetSubsystemName();
        SessionSearch->bIsLanQuery = (SubsystemName == "NULL");

        SessionSearch->MaxSearchResults = 10000;

        // Use SEARCH_PRESENCE to look for sessions
        SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

        // Start searching for sessions
        SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
    }
}
