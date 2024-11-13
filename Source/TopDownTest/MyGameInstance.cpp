#include "MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "TopDownTestPlayerController.h"
#include "Online/OnlineSessionNames.h"

UMyGameInstance::UMyGameInstance()
{
    bWantsToCreateSessionAfterDestroy = false;
}

void UMyGameInstance::Init()
{
    Super::Init();

    if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get()) {
        SessionInterface = Subsystem->GetSessionInterface();
        if (SessionInterface.IsValid()) {
            // Bind delegates to session events
            SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMyGameInstance::OnCreateSessionComplete);
            SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMyGameInstance::OnFindSessionComplete);
            SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMyGameInstance::OnJoinSessionComplete);
            SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UMyGameInstance::OnDestroySessionComplete);
        }
        else {
            UE_LOG(LogTemp, Error, TEXT("Failed to initialize SessionInterface"));
        }
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("OnlineSubsystem is null"));
    }
}

void UMyGameInstance::CreateServer()
{
    UE_LOG(LogTemp, Warning, TEXT("Create Server"));

    // Check if a session already exists and destroy it if necessary
    if (SessionInterface->GetNamedSession(SESSION_NAME)) {
        bWantsToCreateSessionAfterDestroy = true;
        SessionInterface->DestroySession(SESSION_NAME);
        return;
    }

    if (!SessionInterface.IsValid()) {
        UE_LOG(LogTemp, Error, TEXT("SessionInterface is not valid!"));
        return;
    }

    bool bExists = SessionInterface->GetNamedSession(SESSION_NAME) != nullptr;

    if (bExists) {
        UE_LOG(LogTemp, Warning, TEXT("A Session already exists"));
        SessionInterface->DestroySession(SESSION_NAME);
        return;
    }

    // Create session settings
    FName SubsystemName = IOnlineSubsystem::Get()->GetSubsystemName();
    FOnlineSessionSettings SessionSettings;
    //SessionSettings.BuildUniqueId = 12345;

    if (SubsystemName == "NULL") { // LAN Session
        SessionSettings.bAllowJoinInProgress = true;
        SessionSettings.bIsLANMatch = true;
        SessionSettings.bShouldAdvertise = true;
        SessionSettings.NumPublicConnections = 5;
    }
    else if (SubsystemName == "Steam") { // Steam Session
        SessionSettings.bIsLANMatch = false;
        SessionSettings.bShouldAdvertise = true;
        SessionSettings.bUsesPresence = true;
        SessionSettings.bUseLobbiesIfAvailable = true;
        SessionSettings.NumPublicConnections = 5;
        SessionSettings.bAllowJoinInProgress = true;
    }

    // Create the session
    SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
}

void UMyGameInstance::JoinServer()
{
    UE_LOG(LogTemp, Warning, TEXT("Join Server"));

    // Setup search parameters
    SessionSearch = MakeShareable(new FOnlineSessionSearch());
    SessionSearch->bIsLanQuery = (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL"); // LAN search if "NULL"
    SessionSearch->MaxSearchResults = 1000;
    SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals); // Ensure presence is enabled for search

    // Start session search
    SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
    UE_LOG(LogTemp, Warning, TEXT("Attempting to find sessions");
}

void UMyGameInstance::OnCreateSessionComplete(FName SessionName, bool bSucceeded)
{
    if (bSucceeded) {
        FString SessionId = SessionInterface->GetNamedSession(SESSION_NAME)->GetSessionIdStr();
        UE_LOG(LogTemp, Warning, TEXT("Session created successfully. Session ID: %s"), *SessionId);

        // Attempt to travel to the map after creating the session
        GetWorld()->ServerTravel("/Game/TopDown/Maps/TopDownMap?listen");  
        UE_LOG(LogTemp, Error, TEXT("Succeeded to travel"));
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("Failed to create session"));
    }
}

void UMyGameInstance::OnFindSessionComplete(bool bSucceeded)
{
    if (!bSucceeded) {
        UE_LOG(LogTemp, Warning, TEXT("Session search failed"));
        return;
    }

    TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;
    UE_LOG(LogTemp, Error, TEXT("TArray Populated"));
    if (SearchResults.Num()) {
        UE_LOG(LogTemp, Warning, TEXT("LISTING SESSIONS"));
        UE_LOG(LogTemp, Warning, TEXT("-----------"));

        // Display search results
        for (const FOnlineSessionSearchResult& Result : SearchResults) {
            UE_LOG(LogTemp, Warning, TEXT("Owning User Name: %s"), *FString(Result.Session.OwningUserName));
        }

        // Automatically join the first available session
        SessionInterface->JoinSession(0, SESSION_NAME, SearchResults[0]);
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("No sessions found"));
    }
}

void UMyGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    if (APlayerController* PController = UGameplayStatics::GetPlayerController(GetWorld(), 0)) {
        FString JoinAddress = "";
        SessionInterface->GetResolvedConnectString(SessionName, JoinAddress);

        if (JoinAddress != "") {
            UE_LOG(LogTemp, Warning, TEXT("Joining session at: %s"), *JoinAddress);
            PController->ClientTravel(JoinAddress, ETravelType::TRAVEL_Absolute);
        }
        else {
            UE_LOG(LogTemp, Error, TEXT("Failed to get join address for session"));
        }
    }
}

void UMyGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
    if (bWasSuccessful && bWantsToCreateSessionAfterDestroy) {
        bWantsToCreateSessionAfterDestroy = false;
        CreateServer(); // Recreate session after destroy
    }
}
