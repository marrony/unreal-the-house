// Copyright Epic Games, Inc. All Rights Reserved.

#include "BetterCamera.h"
#include "BetterCameraEdMode.h"

#define LOCTEXT_NAMESPACE "FBetterCameraModule"

void FBetterCameraModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FEditorModeRegistry::Get().RegisterMode<FBetterCameraEdMode>(FBetterCameraEdMode::EM_BetterCameraEdModeId, LOCTEXT("BetterCameraEdModeName", "BetterCameraEdMode"), FSlateIcon(), true);
}

void FBetterCameraModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FEditorModeRegistry::Get().UnregisterMode(FBetterCameraEdMode::EM_BetterCameraEdModeId);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBetterCameraModule, BetterCamera)