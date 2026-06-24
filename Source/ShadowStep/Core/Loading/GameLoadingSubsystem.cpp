//  CopyRight (c) 2026 Core Memory Entertainment. All Rights Reserved.

#include "GameLoadingSubsystem.h"
#include "GameLoadingSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"


void UGameLoadingSubsystem::Initialize(FSubsystemCollectionBase& Collection) {
	Super::Initialize(Collection);

	// Bind to the load delegate (to detect when a map finishes loading)
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UGameLoadingSubsystem::OnFinishedLoadingTransitionMap);

	// Get loading settings
	const UGameLoadingSettings* loadingSettings = GetDefault<UGameLoadingSettings>();

	if (!loadingSettings) {
		UE_LOGFMT(LogTemp,Warning,"'GameLoadingSubsystem' failed to access the 'GameLoadingSettings'!"); return;
	}

	// Cache values
	if (!loadingSettings->LoadingScreenWidgetClass) {
		UE_LOGFMT(LogTemp, Warning,"'GameLoadingSettings' has no assigned 'LoadingScreenWidgetClass'"); return;
	}

	m_loadingScreenWidgetClass = loadingSettings->LoadingScreenWidgetClass;

	if (loadingSettings->TransitionMap.IsNull()) {
		UE_LOGFMT(LogTemp, Warning,"'GameLoadingSettings' has no assigned 'TransitionMap'"); return;
	}

	m_transitionMapName = loadingSettings->TransitionMap.GetAssetName();

	// Finished intialization
	m_isInitialized = true;
}


void UGameLoadingSubsystem::Deinitialize() {

	// Clear references
	if (m_activeLoadingWidget) {
		if (m_activeLoadingWidget->IsRooted()) {
			m_activeLoadingWidget->RemoveFromRoot();
		}

		m_activeLoadingWidget = nullptr;
	}

	if (m_cachedSlateWidget.IsValid())
		m_cachedSlateWidget.Reset();

	Super::Deinitialize();
}


void UGameLoadingSubsystem::TransitionToLevel(TSoftObjectPtr<UWorld> a_levelToLoad, bool a_useLoadingScreen /*= true*/, float a_minLoadingTime /*= 2.0f*/) {
	if (!m_isInitialized) {
		UE_LOGFMT(LogTemp, Warning, "'GameLoadingSubsystem' isn't initialized!"); return;
	}

	if (m_isLoadingInProgress) {
		UE_LOGFMT(LogTemp, Warning, "'GameLoadingSubsystem' is already loading a level!"); return;
	}

	if (a_levelToLoad.IsNull()) {
		UE_LOGFMT(LogTemp, Warning, "The desired level to be loaded by 'GameLoadingSubsystem' is invalid!"); return;
	}

	// Lock system
	m_isLoadingInProgress = true;

	// Store values
	m_pendingLevel = a_levelToLoad;
	m_shouldUseLoadingScreen = a_useLoadingScreen;
	m_minLoadingTime = a_minLoadingTime;

	// Get clean names for debug/UI
	m_currentLevelName = GetWorld()->GetMapName();
	m_currentLevelName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
	m_targetLevelName = m_pendingLevel.GetAssetName();

	// Reset flags
	m_isMapAssetLoaded = false;
	m_isMinTimeElapsed = false;

	// Broadcast start
	OnLoadingStarted.Broadcast(m_currentLevelName, m_targetLevelName);

	if (m_shouldUseLoadingScreen) {
		// Setup threaded UI
		SetupMoviePlayer();

		// Load transition map (void) -> destroys old world, gamemode, actors, etc.
		UGameplayStatics::OpenLevel(this, FName(*m_transitionMapName));
	}
	else {
		// Just load data -> player keeps playing as long as its loading
		StartAsyncLoad();
	}
}


void UGameLoadingSubsystem::SetupMoviePlayer() {
	if (!m_loadingScreenWidgetClass) {
		UE_LOGFMT(LogTemp, Warning, "'GameLoadingSubsystem' has no assigned loading screen widget class!"); return;
	}

	// Create widget
	m_activeLoadingWidget = CreateWidget<UUserWidget>(GetGameInstance(), m_loadingScreenWidgetClass);

	// Prevent widget from dying (being GCed) when leaving current level (the level it was created in)
	m_activeLoadingWidget->AddToRoot();

	// Convert to slate (thread safe snapshot) -> takes visual structure of widget and hands it to movie player
	m_cachedSlateWidget = m_activeLoadingWidget->TakeWidget();

	// Create the widget for the loading screen
	if (m_cachedSlateWidget.IsValid() && GetGameInstance()->GetGameViewportClient())
		GetGameInstance()->GetGameViewportClient()->AddViewportWidgetContent(m_cachedSlateWidget.ToSharedRef(), 1000);
}


void UGameLoadingSubsystem::StartAsyncLoad() {
	// Minimum loading time
	if (m_minLoadingTime > 0.0f) {
		GetWorld()->GetTimerManager().SetTimer(m_minLoadingTimerHandle, this, &UGameLoadingSubsystem::OnMinTimeExpired, m_minLoadingTime, false);
	}
	else {
		m_isMinTimeElapsed = true;
	}

	// Start async load (pre-load into RAM)
	FSoftObjectPath levelPath = m_pendingLevel.ToSoftObjectPath();

	m_currentLoadHandle = m_streamableManager.RequestAsyncLoad(levelPath, FStreamableDelegate::CreateUObject(this, &UGameLoadingSubsystem::OnAssetLoaded));
}


void UGameLoadingSubsystem::OnAssetLoaded() {
	m_isMapAssetLoaded = true;
	CheckCompletion();
}


void UGameLoadingSubsystem::OnMinTimeExpired() {
	m_isMinTimeElapsed = true;
	CheckCompletion();
}


void UGameLoadingSubsystem::CheckCompletion() {
	// Make sure the map asset is loaded and the minimum time passed
	if (m_isMapAssetLoaded && m_isMinTimeElapsed) {
		FinishLoading();
	}
}

void UGameLoadingSubsystem::FinishLoading() {
	// Reset handle if exists (shouldn't be necessary (since lambda cleans this in the end), but safe is safe)
	if (m_postLoadHandle.IsValid()) {
		FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(m_postLoadHandle);
	}

	// Now check when the new level is actually ready to play ('PostLoadMapWithWorld' is called after 'InitializeActorsForPlay', which calls 'RegisterComponent' and 'BeginPlay')
	m_postLoadHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddWeakLambda(this, [this](UWorld* world) { // Use a weak lambda, since unreal doesn't gc doesn't -> would crash if global event is called and this is dead (unlikely)

		FString loadedMapName = world->GetName();
		loadedMapName.RemoveFromStart(world->StreamingLevelsPrefix);

		// Make sure this is our target level (not some other intermediate level, if one exists in the future)
		if (loadedMapName == m_pendingLevel.GetAssetName()) {
			// Broadcast finished
			OnLoadingFinished.Broadcast(m_currentLevelName, m_targetLevelName);

			// Unbind lambda (so we don't keep  multiple levels bound to it that are not used anymore)
			FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(m_postLoadHandle);
			m_postLoadHandle.Reset();

			if (world) { // Wait for 1 frame, so renderer catches up
				world->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]() {

					if (m_cachedSlateWidget.IsValid()) {
						if (GetGameInstance()->GetGameViewportClient()) {
							GetGameInstance()->GetGameViewportClient()->RemoveViewportWidgetContent(m_cachedSlateWidget.ToSharedRef());
						}
						m_cachedSlateWidget.Reset();
					}

					// Remove from root again
					if (m_activeLoadingWidget) {
						m_activeLoadingWidget->RemoveFromRoot();
						m_activeLoadingWidget = nullptr;
					}

					m_isLoadingInProgress = false;
					}));
			}
		}
	});

	// Map data is now in ram -> 'OpenLevel' will now be way faster
	// NOTE: 'BeginPlay' will run and freeze main thread -> keep loading screen active
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, m_pendingLevel);
}


void UGameLoadingSubsystem::OnFinishedLoadingTransitionMap(UWorld* a_loadedWorld) {
	if (m_shouldUseLoadingScreen && a_loadedWorld->GetName() == m_transitionMapName) {
		// Entered void -> old actors are gone, but their memory might still be "pending kill" -> force GC to clear ram
		GEngine->ForceGarbageCollection(true);

		StartAsyncLoad(); // Now start loading
	}
}
