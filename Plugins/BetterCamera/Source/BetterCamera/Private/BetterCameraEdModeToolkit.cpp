// Copyright Epic Games, Inc. All Rights Reserved.

#include "BetterCameraEdModeToolkit.h"
#include "BetterCameraEdMode.h"
#include "Engine/Selection.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "EditorModeManager.h"
#include "Builders/CubeBuilder.h"

#define LOCTEXT_NAMESPACE "FBetterCameraEdModeToolkit"

FBetterCameraEdModeToolkit::FBetterCameraEdModeToolkit()
{
}

void FBetterCameraEdModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost)
{
	struct Locals
	{
		static bool IsWidgetEnabled()
		{
			return GEditor->GetSelectedActors()->Num() != 0;
		}

		static FReply OnButtonClick(FVector InOffset)
		{
			USelection* SelectedActors = GEditor->GetSelectedActors();

			// Let editor know that we're about to do something that we want to undo/redo
			GEditor->BeginTransaction(LOCTEXT("MoveActorsTransactionName", "MoveActors"));

			// For each selected actor
			for (FSelectionIterator Iter(*SelectedActors); Iter; ++Iter) {
                if (ABrush* brush = Cast<ABrush>(*Iter)) {
                    brush->Modify();
                    brush->TeleportTo(brush->GetActorLocation() + FVector(0, 0, 1), FRotator(0, 0, 0));
                    GEditor->RebuildAlteredBSP();

//                    if (UCubeBuilder* cubeBuilder = Cast<UCubeBuilder>(brush->BrushBuilder)) {
//                        UWorld* world = brush->GetWorld();
//                        if (world) {
//                            cubeBuilder->X = 1000;
//                            cubeBuilder->Y = 1000;
//                            cubeBuilder->Z = 1000;
//
////                            brush->Modify(true);
////                            brush->bInManipulation = true;
//                            cubeBuilder->Build(world, brush);
//                            //brush->Brush->BuildBound();
//                            //if (!brush->bSuppressBSPRegeneration && IsStaticBrush() && GUndo)
//                            GEditor->RebuildAlteredBSP();
//                            //GEditor->Exec(world, TEXT("BRUSH MOVETO X=0 Y=0 Z=0"));
//                        }
//                    }

//                    cubeBuilder->Build(World, this);
//
//                    GEditor->Exec(World, TEXT("BRUSH ADDED"));
//                    GEditorModeTools().MapChangeNotify();
                    //brush->TeleportTo(brush->GetActorLocation() + InOffset, FRotator(0, 0, 0));
                }

//				if (AActor* LevelActor = Cast<AActor>(*Iter))
//				{
//					// Register actor in opened transaction (undo/redo)
//					LevelActor->Modify();
//					// Move actor to given location
//					LevelActor->TeleportTo(LevelActor->GetActorLocation() + InOffset, FRotator(0, 0, 0));
//				}
			}

			// We're done moving actors so close transaction
			GEditor->EndTransaction();

			return FReply::Handled();
		}

		static TSharedRef<SWidget> MakeButton(FText InLabel, const FVector InOffset)
		{
			return SNew(SButton)
				.Text(InLabel)
				.OnClicked_Static(&Locals::OnButtonClick, InOffset);
		}
	};

	const float Factor = 256.0f;

	SAssignNew(ToolkitWidget, SBorder)
		.HAlign(HAlign_Center)
		.Padding(25)
		.IsEnabled_Static(&Locals::IsWidgetEnabled)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Center)
			.Padding(50)
			[
				SNew(STextBlock)
				.AutoWrapText(true)
				.Text(LOCTEXT("HelperLabel", "Select some actors and move them around using buttons below"))
			]
			+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					Locals::MakeButton(LOCTEXT("UpButtonLabel", "Up"), FVector(0, 0, Factor))
				]
			+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						Locals::MakeButton(LOCTEXT("LeftButtonLabel", "Left"), FVector(0, -Factor, 0))
					]
					+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							Locals::MakeButton(LOCTEXT("RightButtonLabel", "Right"), FVector(0, Factor, 0))
						]
				]
			+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					Locals::MakeButton(LOCTEXT("DownButtonLabel", "Down1"), FVector(0, 0, -Factor))
				]

		];
		
	FModeToolkit::Init(InitToolkitHost);
}

FName FBetterCameraEdModeToolkit::GetToolkitFName() const
{
	return FName("BetterCameraEdMode");
}

FText FBetterCameraEdModeToolkit::GetBaseToolkitName() const
{
	return NSLOCTEXT("BetterCameraEdModeToolkit", "DisplayName", "BetterCameraEdMode Tool");
}

class FEdMode* FBetterCameraEdModeToolkit::GetEditorMode() const
{
	return GLevelEditorModeTools().GetActiveMode(FBetterCameraEdMode::EM_BetterCameraEdModeId);
}

#undef LOCTEXT_NAMESPACE
