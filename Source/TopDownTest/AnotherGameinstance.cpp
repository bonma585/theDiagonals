// Fill out your copyright notice in the Description page of Project Settings.


#include "AnotherGameinstance.h"
#include "Kismet/GameplayStatics.h"
#include "TopDownTestPlayerController.h"
#include "Online/OnlineSessionNames.h"

UAnotherGameinstance::UAnotherGameinstance()
{
}

void UAnotherGameinstance::Init()
{
    Super::Init();

    if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get()) {
        SessionInterface = Subsystem->GetSessionInterface();
        if (SessionInterface.IsValid()) {
            SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UAnotherGameinstance::OnCreateSessionComplete);
            SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UAnotherGameinstance::OnFindSessionComplete);
            SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UAnotherGameinstance::OnJoinSessionComplete);
            SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UAnotherGameinstance::OnDestroySessionComplete);

        }
    }
}

void UAnotherGameinstance::CreateServer()
{
    UE_LOG(LogTemp, Warning, TEXT("Create Server"));

    /*bool bExists = SessionInterface->GetNamedSession(SESSION_NAME) != nullptr;

    if (bExists) {
        UE_LOG(LogTemp, Warning, TEXT("A Session already exists"));
        SessionInterface->DestroySession(SESSION_NAME);
        return;
    }
    */
    FName SubsystemName = IOnlineSubsystem::Get()->GetSubsystemName();
    FOnlineSessionSettings SessionSettings;

    // Ensure LAN and Steam settings match MyGameInstance exactly
    if (SubsystemName == "NULL") {
        SessionSettings.bAllowJoinInProgress = true;
        SessionSettings.bIsLANMatch = true;
        SessionSettings.bShouldAdvertise = true;
        SessionSettings.NumPublicConnections = 5;
        SessionSettings.bAllowJoinInProgress = true;
    }
    else if (SubsystemName == "Steam") {
        SessionSettings.bIsLANMatch = false;
        SessionSettings.bShouldAdvertise = true;
        SessionSettings.bUsesPresence = true;
        SessionSettings.bUseLobbiesIfAvailable = true;
        SessionSettings.NumPublicConnections = 5;
        SessionSettings.bAllowJoinInProgress = true;
    }

    UE_LOG(LogTemp, Warning, TEXT("Logging Session Settings:"));
    for (const auto& Setting : SessionSettings.Settings)
    {
        FString Key = Setting.Key.ToString();
        FString Value;
        Setting.Value.Data.GetValue(Value);
        UE_LOG(LogTemp, Warning, TEXT("%s: %s"), *Key, *Value);
    }

    // Create the session
    SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
}

void UAnotherGameinstance::JoinServer()
{
    UE_LOG(LogTemp, Warning, TEXT("Join Server"));

    // Make sure the session search logic is identical to UMyGameInstance
    SessionSearch = MakeShareable(new FOnlineSessionSearch());
    SessionSearch->bIsLanQuery = (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL");
    SessionSearch->MaxSearchResults = 10000;

    // Use the same query settings as MyGameInstance
    SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

    // Initiate session search
    SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}


void UAnotherGameinstance::OnCreateSessionComplete(FName SessionName, bool bSucceeded)
{
    if (bSucceeded) {
        FString SessionId = SessionInterface->GetNamedSession(SESSION_NAME)->GetSessionIdStr();
        UE_LOG(LogTemp, Warning, TEXT("Session ID: %s"), *SessionId);

        // Ensure ServerTravel is called correctly for both cases
        GetWorld()->ServerTravel("/Game/TopDown/Maps/TopDownMap?listen");
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("Failed to create session"));
    }
}

void UAnotherGameinstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    if (APlayerController* PController = UGameplayStatics::GetPlayerController(GetWorld(), 0)) {
        FString JoinAddress = "";
        SessionInterface->GetResolvedConnectString(SessionName, JoinAddress);

        if (JoinAddress != "") {
            PController->ClientTravel(JoinAddress, ETravelType::TRAVEL_Absolute);
        }
    }
}

void UAnotherGameinstance::OnFindSessionComplete(bool bSucceeded)
{
    if (!bSucceeded) {
        return;
    }

    TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;

    if (SearchResults.Num()) {
        UE_LOG(LogTemp, Warning, TEXT("LISTING SESSIONS"));
        for (const FOnlineSessionSearchResult& SearchResult : SearchResults) {
            UE_LOG(LogTemp, Warning, TEXT("Owning User Name: %s"), *FString(SearchResult.Session.OwningUserName));
        }

        // Join the first found session
        SessionInterface->JoinSession(0, SESSION_NAME, SearchResults[0]);
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("No sessions found"));
    }
}


void UAnotherGameinstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
    if (bWasSuccessful) {
        // After destroying the session, create a new one
        CreateServer();
    }
}

