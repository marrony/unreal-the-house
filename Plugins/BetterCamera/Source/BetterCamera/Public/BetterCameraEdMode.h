// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EdMode.h"

class FBetterCameraEdMode : public FEdMode
{
public:
	const static FEditorModeID EM_BetterCameraEdModeId;

    float size;
    FVector offset;
    bool shiftPressed;
    bool controlPressed;
public:
	FBetterCameraEdMode();
	virtual ~FBetterCameraEdMode();

	// FEdMode interface
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Tick(FEditorViewportClient* ViewportClient, float DeltaTime) override;

    virtual bool MouseEnter(FEditorViewportClient* ViewportClient, FViewport* Viewport, int32 x, int32 y) override;
    virtual bool MouseLeave(FEditorViewportClient* ViewportClient, FViewport* Viewport) override;
    virtual bool MouseMove(FEditorViewportClient* ViewportClient, FViewport* Viewport, int32 x, int32 y) override;
    virtual bool CapturedMouseMove(FEditorViewportClient* InViewportClient, FViewport* InViewport, int32 InMouseX, int32 InMouseY) override;

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

    // This is not just called for keyboard keys, it is also called for mouse down/up events!
    // Eg for left-press we get Key = EKeys::LeftMouseButton and Event = IE_Pressed
    // Return value indicates "handled'. If we return true for mouse press events then
    // the StartTracking/CapturedMouseMove/EndTracking sequence is *not* called (but we
    // also don't get MouseMove(), so the mouse-movement events appear to be lost?)
    virtual bool InputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event) override;

    // Called for 1D Axis movements - EKeys::MouseX, EKeys::MouseY, and EKeys::MouseWheelAxis
    // Called even if we return true from InputKey which otherwise blocks tracking.
    // Return value indicates "handled" but has no effect on CapturedMouseMove events (ie we still get them)
    virtual bool InputAxis(FEditorViewportClient* InViewportClient, FViewport* Viewport, int32 ControllerId, FKey Key, float Delta, float DeltaTime) override;
    virtual bool InputDelta(FEditorViewportClient* InViewportClient, FViewport* InViewport, FVector& InDrag, FRotator& InRot, FVector& InScale) override;

	//virtual void Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI) override;
	//virtual void ActorSelectionChangeNotify() override;
	bool UsesToolkits() const override;
	// End of FEdMode interface
};
