// Copyright Epic Games, Inc. All Rights Reserved.

#include "BetterCameraEdMode.h"
#include "BetterCameraEdModeToolkit.h"
#include "Toolkits/ToolkitManager.h"
#include "Engine/Selection.h"
#include "EditorModeManager.h"
#include "Builders/CubeBuilder.h"

#define LOCTEXT_NAMESPACE "FBetterCameraEdMode"

const FEditorModeID FBetterCameraEdMode::EM_BetterCameraEdModeId = TEXT("EM_BetterCameraEdMode");

FBetterCameraEdMode::FBetterCameraEdMode()
{

}

FBetterCameraEdMode::~FBetterCameraEdMode()
{

}

void FBetterCameraEdMode::Enter()
{
	FEdMode::Enter();

	if (!Toolkit.IsValid() && UsesToolkits())
	{
		Toolkit = MakeShareable(new FBetterCameraEdModeToolkit);
		Toolkit->Init(Owner->GetToolkitHost());
	}
}

void FBetterCameraEdMode::Exit()
{
	if (Toolkit.IsValid())
	{
		FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
		Toolkit.Reset();
	}

	// Call base Exit method to ensure proper cleanup
	FEdMode::Exit();
}

void FBetterCameraEdMode::Tick(FEditorViewportClient* ViewportClient, float DeltaTime) {
    FEdMode::Tick(ViewportClient, DeltaTime);

//    UE_LOG(LogCore, Display, TEXT("Camera Location: %s "), *ViewportClient->GetViewLocation().ToString());
//    UE_LOG(LogOutputDevice, Display, TEXT("Camera Location: %s "), *ViewportClient->GetViewLocation().ToString());
//
//    UE_LOG(LogCore, All, TEXT("Camera Location: %s "), *ViewportClient->GetViewLocation().ToString());
//    UE_LOG(LogOutputDevice, All, TEXT("Camera Location: %s "), *ViewportClient->GetViewLocation().ToString());
//
//    const FVector location = ViewportClient->GetViewLocation();
//    const FRotator rotation = ViewportClient->GetViewRotation();
}

bool FBetterCameraEdMode::MouseEnter(FEditorViewportClient* ViewportClient, FViewport* Viewport, int32 x, int32 y) {
    UE_LOG(LogCore, Display, TEXT("Mouse Enter: %d, %d"), x, y);

    return FEdMode::MouseEnter(ViewportClient, Viewport, x, y);
}

bool FBetterCameraEdMode::MouseLeave(FEditorViewportClient* ViewportClient, FViewport* Viewport) {
    UE_LOG(LogCore, Display, TEXT("Mouse Leave"));

    return FEdMode::MouseLeave(ViewportClient, Viewport);
}

bool FBetterCameraEdMode::MouseMove(FEditorViewportClient* ViewportClient, FViewport* Viewport, int32 x, int32 y) {
//    UE_LOG(LogCore, Display, TEXT("Mouse Move: %d, %d"), x, y);

    return FEdMode::MouseMove(ViewportClient, Viewport, x, y);
}

bool FBetterCameraEdMode::CapturedMouseMove(FEditorViewportClient* InViewportClient, FViewport* InViewport, int32 InMouseX, int32 InMouseY) {
    UE_LOG(LogCore, Display, TEXT("Captured Mouse Move: %d, %d"), InMouseX, InMouseY);

    return FEdMode::CapturedMouseMove(InViewportClient, InViewport, InMouseX, InMouseY);
}

/*
 * Input Button/Axis Events & Mouse Capture
 *
 *   event sequence for left mouse down/capture/up is:
 *     - InputKey( EKeys::LeftMouseButton, IE_PRESSED )
 *     - StartTracking()
 *     - CapturedMouseMove() and InputAxis() (repeated for each mouse-move)
 *     - Inputkey( EKeys::LeftMouseButton, IE_RELEASED )
 *     - EndTracking()
 *
 *   for doubleclick, we get one of the above sequence, then a second
 *   where instead of the first IE_PRESSED we get a IE_DoubleClick
 *
 *   for mouse wheel, we get following sequence
 *     - InputKey ( EKeys::MouseScrollUp / Down, IE_PRESSED )
 *     - InputAxis() for wheel move
 *     - InputKey ( EKeys::MouseScrollUp / Down, IE_RELEASED )
 *   it appears that there will only ever be *one* InputAxis() between the pressed/released sequenece
 *
 *   Note that this wheel event sequence can happen *during* a
 *   middle-mouse tracking sequence on a wheel-button (nice!)
 */
bool FBetterCameraEdMode::InputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event) {
    UE_LOG(LogCore, Display, TEXT("Input Key: %s, %d"), *Key.ToString(), (int32)Event);

    if (Key == EKeys::MouseScrollUp) {
    }

    if (Key == EKeys::LeftShift) {
        shiftPressed = (Event == IE_Pressed || Event == IE_Released);
    }

    if (Key == EKeys::LeftControl) {
        controlPressed = (Event == IE_Pressed || Event == IE_Repeat);
    }

    bool isArrow = (Key == EKeys::Up) || (Key == EKeys::Down) || (Key == EKeys::Left) || (Key == EKeys::Right);

    if (shiftPressed && controlPressed && isArrow) {
        if (Event == IE_Pressed || Event == IE_Repeat) {
            if (Key == EKeys::Up) {
                size += 1;
            }

            if (Key == EKeys::Down) {
                size -= 1;
            }
        }

        if (Event == IE_Released) {
            USelection* SelectedActors = GEditor->GetSelectedActors();

            // Let editor know that we're about to do something that we want to undo/redo
            GEditor->BeginTransaction(LOCTEXT("ResizeActorsTransactionName", "ResizeActors"));

            // For each selected actor
            for (FSelectionIterator Iter(*SelectedActors); Iter; ++Iter) {
                if (ABrush* brush = Cast<ABrush>(*Iter)) {
                    if (UCubeBuilder* cubeBuilder = Cast<UCubeBuilder>(brush->BrushBuilder)) {
                        if (UWorld* world = brush->GetWorld()) {
                            bool gridEnabled = GetDefault<ULevelEditorViewportSettings>()->GridEnabled;

                            GEditor->Exec(world, TEXT("MODE GRID=0"));

                            brush->Modify();

                            cubeBuilder->Y += size;
                            cubeBuilder->Build(world, brush);

                            FVector oldLoc = brush->GetActorLocation();
                            FVector newLoc = oldLoc + FVector(0, size * 0.5f, 0);

//                            UE_LOG(LogCore, Display, TEXT("Input Key: %s, %s"), *oldLoc.ToString(), *newLoc.ToString());

                            brush->TeleportTo(newLoc, FRotator(0, 0, 0));

                            GEditor->RebuildAlteredBSP();

                            if (gridEnabled) {
                                GEditor->Exec(world, TEXT("MODE GRID=1"));
                            }
                        }
                    }
                }
            }

            GEditor->EndTransaction();

            size = 0;
        }

        return true;
    }

    if (shiftPressed && !controlPressed && isArrow) {
        if (Event == IE_Pressed || Event == IE_Repeat) {
            if (Key == EKeys::Up) {
                offset += FVector(0, 0, 1);
            }

            if (Key == EKeys::Down) {
                offset -= FVector(0, 0, 1);
            }
        }

        if (Event == IE_Released && offset != FVector(0, 0, 0)) {
            USelection* SelectedActors = GEditor->GetSelectedActors();

            // Let editor know that we're about to do something that we want to undo/redo
            GEditor->BeginTransaction(LOCTEXT("MoveActorsTransactionName", "MoveActors"));

            // For each selected actor
            for (FSelectionIterator Iter(*SelectedActors); Iter; ++Iter) {
                if (ABrush* brush = Cast<ABrush>(*Iter)) {
                    brush->Modify();
                    brush->TeleportTo(brush->GetActorLocation() + offset, FRotator(0, 0, 0));
                    GEditor->RebuildAlteredBSP();
                }
            }

            GEditor->EndTransaction();

            offset = FVector(0, 0, 0);
        }

        return true;
    }

    return FEdMode::InputKey(ViewportClient, Viewport, Key, Event);
}

bool FBetterCameraEdMode::InputAxis(FEditorViewportClient* InViewportClient, FViewport* Viewport, int32 ControllerId, FKey Key, float Delta, float DeltaTime) {
    //UE_LOG(LogCore, Display, TEXT("Input Axis: %d, %s, %f, %f"), ControllerId, *Key.ToString(), Delta, DeltaTime);

    if (Key == EKeys::MouseWheelAxis) {
        UE_LOG(LogCore, Display, TEXT("Input Axis: %d, %s, %f, %f"), ControllerId, *Key.ToString(), Delta, DeltaTime);
    }

    return FEdMode::InputAxis(InViewportClient, Viewport, ControllerId, Key, Delta, DeltaTime);
}

bool FBetterCameraEdMode::InputDelta(FEditorViewportClient* InViewportClient, FViewport* InViewport, FVector& InDrag, FRotator& InRot, FVector& InScale) {
    UE_LOG(LogCore, Display, TEXT("Input Delta: %s, %s, %s"), *InDrag.ToString(), *InRot.ToString(), *InScale.ToString());

    return FEdMode::InputDelta(InViewportClient, InViewport, InDrag, InRot, InScale);
}


bool FBetterCameraEdMode::UsesToolkits() const
{
	return true;
}

#undef LOCTEXT_NAMESPACE
