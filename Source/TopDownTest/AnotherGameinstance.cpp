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
            // Bind delegates to session events
            SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UAnotherGameinstance::OnCreateSessionComplete);
            SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UAnotherGameinstance::OnFindSessionComplete);
            SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UAnotherGameinstance::OnJoinSessionComplete);
            SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UAnotherGameinstance::OnDestroySessionComplete);
        }
        else {
            UE_LOG(LogTemp, Error, TEXT("Failed to initialize SessionInterface"));
        }
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("OnlineSubsystem is null"));
    }
}

void UAnotherGameinstance::CreateServer()
{
    UE_LOG(LogTemp, Warning, TEXT("Create Server"));

    bool bExists = SessionInterface->GetNamedSession(SESSION_NAME) != nullptr;

    if (bExists) {
        UE_LOG(LogTemp, Warning, TEXT("A Session already exists"));
        SessionInterface->DestroySession(SESSION_NAME);
        return;
    }
    FName SubsystemName = IOnlineSubsystem::Get()->GetSubsystemName();
    FOnlineSessionSettings SessionSettings;


    if (SubsystemName == "NULL") {
        SessionSettings.bAllowJoinInProgress = true;
        SessionSettings.bIsLANMatch = true;
        SessionSettings.bShouldAdvertise = true;
        SessionSettings.NumPublicConnections = 5;
    }
    else if (SubsystemName == "Steam") {
        SessionSettings.bIsLANMatch = false;
        SessionSettings.bShouldAdvertise = true;
        SessionSettings.bUsesPresence = true;
        SessionSettings.bUseLobbiesIfAvailable = true;
        SessionSettings.NumPublicConnections = 5;
    }


    SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
}

void UAnotherGameinstance::JoinServer()
{
    UE_LOG(LogTemp, Warning, TEXT("Join Server"));
    SessionSearch = MakeShareable(new FOnlineSessionSearch());
    SessionSearch->bIsLanQuery = (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL");
    SessionSearch->MaxSearchResults = 10000;
    SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
    SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UAnotherGameinstance::OnCreateSessionComplete(FName SessionName, bool bSucceeded)
{
    if (bSucceeded) {

        FString SessionId = SessionInterface->GetNamedSession(SESSION_NAME)->GetSessionIdStr();

        UE_LOG(LogTemp, Warning, TEXT("Session ID: %s"), *SessionId);
        GetWorld()->ServerTravel("/Game/TopDown/Maps/TopDownMap?listen");
        UE_LOG(LogTemp, Error, TEXT("Succeeded to travel"));
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("Failed to create session"));
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

void UAnotherGameinstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
    if (bWasSuccessful) {
        CreateServer();
    }
}