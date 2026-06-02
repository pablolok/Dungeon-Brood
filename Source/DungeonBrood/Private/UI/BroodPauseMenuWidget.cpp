#include "UI/BroodPauseMenuWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/BroodPlayerController.h"

void UBroodPauseMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	BuildMenu();
}

void UBroodPauseMenuWidget::BuildMenu()
{
	if (!WidgetTree || bMenuBuilt)
	{
		return;
	}
	bMenuBuilt = true;

	UCanvasPanel* RootPanel = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("PauseRoot"));
	WidgetTree->RootWidget = RootPanel;

	UBorder* ScreenShade = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("ScreenShade"));
	ScreenShade->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.72f));
	UCanvasPanelSlot* ScreenShadeSlot = RootPanel->AddChildToCanvas(ScreenShade);
	ScreenShadeSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
	ScreenShadeSlot->SetOffsets(FMargin(0.0f));

	UBorder* MenuPanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("MenuPanel"));
	MenuPanel->SetBrushColor(FLinearColor(0.035f, 0.055f, 0.045f, 0.98f));
	MenuPanel->SetPadding(FMargin(32.0f, 26.0f));
	UCanvasPanelSlot* MenuPanelSlot = RootPanel->AddChildToCanvas(MenuPanel);
	MenuPanelSlot->SetAnchors(FAnchors(0.5f, 0.5f));
	MenuPanelSlot->SetAlignment(FVector2D(0.5f, 0.5f));
	MenuPanelSlot->SetAutoSize(true);

	UVerticalBox* MenuBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("PauseMenuBox"));
	MenuPanel->SetContent(MenuBox);

	UTextBlock* Title = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("PauseTitle"));
	Title->SetText(FText::FromString(TEXT("DUNGEON BROOD\nPAUSED")));
	Title->SetColorAndOpacity(FSlateColor(FLinearColor(0.2f, 1.0f, 0.35f)));
	Title->SetJustification(ETextJustify::Center);

	UVerticalBoxSlot* TitleSlot = MenuBox->AddChildToVerticalBox(Title);
	TitleSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 18.0f));

	UButton* ResumeButton = AddMenuButton(MenuBox, TEXT("Resume"));
	UButton* RestartButton = AddMenuButton(MenuBox, TEXT("Restart Run"));
	UButton* ExitButton = AddMenuButton(MenuBox, TEXT("Exit Game"));

	if (ResumeButton)
	{
		ResumeButton->OnClicked.AddUniqueDynamic(this, &UBroodPauseMenuWidget::HandleResumeClicked);
	}

	if (RestartButton)
	{
		RestartButton->OnClicked.AddUniqueDynamic(this, &UBroodPauseMenuWidget::HandleRestartClicked);
	}

	if (ExitButton)
	{
		ExitButton->OnClicked.AddUniqueDynamic(this, &UBroodPauseMenuWidget::HandleExitClicked);
	}
}

UButton* UBroodPauseMenuWidget::AddMenuButton(UVerticalBox* Parent, const FString& Label)
{
	if (!WidgetTree || !Parent)
	{
		return nullptr;
	}

	USizeBox* ButtonSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
	ButtonSize->SetWidthOverride(260.0f);
	ButtonSize->SetHeightOverride(48.0f);

	UButton* Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), *FString::Printf(TEXT("%sButton"), *Label.Replace(TEXT(" "), TEXT(""))));
	UTextBlock* ButtonText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	ButtonText->SetText(FText::FromString(Label));
	ButtonText->SetJustification(ETextJustify::Center);
	ButtonText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	Button->AddChild(ButtonText);
	ButtonSize->SetContent(Button);

	UVerticalBoxSlot* ButtonSlot = Parent->AddChildToVerticalBox(ButtonSize);
	ButtonSlot->SetPadding(FMargin(0.0f, 5.0f));

	return Button;
}

void UBroodPauseMenuWidget::HandleResumeClicked()
{
	if (ABroodPlayerController* PlayerController = Cast<ABroodPlayerController>(GetOwningPlayer()))
	{
		PlayerController->SetPause(false);
		PlayerController->RestoreGameplayInputMode();
	}

	RemoveFromParent();
}

void UBroodPauseMenuWidget::HandleRestartClicked()
{
	if (APlayerController* PlayerController = GetOwningPlayer())
	{
		PlayerController->SetPause(false);
		UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
	}
}

void UBroodPauseMenuWidget::HandleExitClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}
