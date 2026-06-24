/*****************************************************************************
* Project: OutOfBounce
* File : GameLoadingSubsystem.h
* Author : Julian Serve
*
* CopyRight (c) 2026 Core Memory Entertainment. All Rights Reserved.
*
* Description:
* This subsystem manages the loading and transition between game levels, 
* providing a seamless experience for players. It handles the display of loading screens,
* asynchronous asset loading, and ensures that transitions between levels are smooth and free of hitches.
* The subsystem also broadcasts events when loading starts and finishes, 
* allowing other parts of the game to respond accordingly (e.g., updating UI, pausing gameplay, etc.).
* By centralizing the loading logic in a subsystem,
* we can easily manage level transitions and provide a consistent experience across the game.
* 
******************************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/StreamableManager.h"
#include "GameLoadingSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLoadingStateChanged, FString, FromLevel, FString, ToLevel);

UCLASS()
class SHADOWSTEP_API UGameLoadingSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
protected:
	/// <summary>
	/// Initializes the subsystem, binding to map load events and caching settings from 'GameLoadingSettings'.
	/// </summary>
	/// <param name="Collection"></param>
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/// <summary>
	/// Deinitialize be called when the subsystem is being deinitialized (game instance shutdown), it clears references to widgets and resets handles to prevent dangling references and potential crashes.
	/// </summary>
	virtual void Deinitialize() override;

public:
	/// <summary>
	/// Transitions to a new level, optionally showing a loading screen and enforcing a minimum loading time. It handles the entire loading process, from starting the async load to broadcasting events when loading starts and finishes.
	/// </summary>
	/// <param name="a_levelToLoad"></param>
	/// <param name="a_useLoadingScreen"></param>
	/// <param name="a_minLoadingTime"></param>
	void TransitionToLevel(TSoftObjectPtr<UWorld> a_levelToLoad, bool a_useLoadingScreen = true, float a_minLoadingTime = 2.0f);

private:
	/// <summary>
	/// Sets up the movie player to display a loading screen widget while the new level is loading.
	/// It creates the widget, prevents it from being garbage collected, converts it to a slate widget for thread safety,
	/// and adds it to the viewport with a high Z-order to ensure it's visible on top of other content.
	/// </summary>
	void SetupMoviePlayer();

	/// <summary>
	/// Loads the target level asset asynchronously, while the transition map is active. It also starts a timer for the minimum loading time, and both the asset load and timer must complete before finishing the loading process and opening the new level.
	/// </summary>
	void StartAsyncLoad();

	/// <summary>
	/// On Asset loaded, we set the flag and check if the minimum loading time has already elapsed. If both conditions are met, we proceed to finish loading and transition to the new level. This ensures that the loading screen is displayed for at least the minimum time, even if the asset loads quickly, providing a smoother user experience.
	/// </summary>
	void OnAssetLoaded();

	/// <summary>
	/// On Min Time Expired, we set the flag and check if the map asset has already been loaded. If both conditions are met, we proceed to finish loading and transition to the new level. This ensures that the loading screen is displayed for at least the minimum time, even if the asset loads quickly, providing a smoother user experience.
	/// </summary>
	void OnMinTimeExpired();

	/// <summary>
	/// Checks if both the map asset is loaded and the minimum loading time has elapsed. If both conditions are met, it proceeds to finish loading and transition to the new level. This ensures that the loading screen is displayed for at least the minimum time, even if the asset loads quickly, providing a smoother user experience.
	/// </summary>
	void CheckCompletion();

	/// <summary>
	/// When both the map asset is loaded and the minimum loading time has elapsed, this function is called to finalize the loading process. It binds a lambda to the 'PostLoadMapWithWorld' delegate to detect when the new level is fully loaded and ready to play. Once the new level is loaded, it broadcasts the 'OnLoadingFinished' event, cleans up the loading screen widget, and resets the loading state to allow for future transitions. This ensures a smooth transition between levels while providing feedback to the player through events and a loading screen if enabled.
	/// </summary>
	void FinishLoading();

	/// <summary>
	/// On Finished Loading Transition Map, we check if the loaded map is the transition map (void), if so, we can be sure that all old actors are destroyed and we can safely start loading the new level's data into RAM without risking hitching due to GC or streaming while the player is still in the old level. This also ensures that the loading screen remains visible during the entire loading process, providing a smoother user experience.
	/// </summary>
	/// <param name="a_loadedWorld"></param>
	void OnFinishedLoadingTransitionMap(UWorld* a_loadedWorld);

public:
	// Events
	UPROPERTY(BlueprintAssignable, Category = "Own | Loading")
	FOnLoadingStateChanged OnLoadingStarted;

	UPROPERTY(BlueprintAssignable, Category = "Own | Loading")
	FOnLoadingStateChanged OnLoadingFinished;

private:
	bool m_isInitialized{};

	UPROPERTY()
	TSubclassOf<UUserWidget> m_loadingScreenWidgetClass{ nullptr };

	UPROPERTY()
	TObjectPtr<UUserWidget> m_activeLoadingWidget{ nullptr };

	UPROPERTY()
	TSoftObjectPtr<UWorld> m_pendingLevel{ nullptr };

	TSharedPtr<SWidget> m_cachedSlateWidget{ nullptr };

	FString m_currentLevelName{};
	FString m_targetLevelName{};
	bool m_shouldUseLoadingScreen{};
	float m_minLoadingTime{};
	bool m_isMapAssetLoaded{};
	bool m_isMinTimeElapsed{};
	bool m_isLoadingInProgress{};

	// Async handles
	FStreamableManager m_streamableManager{};
	TSharedPtr<FStreamableHandle> m_currentLoadHandle{ nullptr };
	FTimerHandle m_minLoadingTimerHandle{};
	FDelegateHandle m_postLoadHandle{};

	FString m_transitionMapName{};
};
