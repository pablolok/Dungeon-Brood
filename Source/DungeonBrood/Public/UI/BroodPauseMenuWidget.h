#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BroodPauseMenuWidget.generated.h"

class UButton;

UCLASS()
class DUNGEONBROOD_API UBroodPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

private:
	UFUNCTION()
	void HandleResumeClicked();

	UFUNCTION()
	void HandleRestartClicked();

	UFUNCTION()
	void HandleExitClicked();

	void BuildMenu();
	UButton* AddMenuButton(class UVerticalBox* Parent, const FString& Label);

	bool bMenuBuilt = false;
};
