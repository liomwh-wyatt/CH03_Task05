#pragma once

#include "CoreMinimal.h"
#include "Feedback/Ch03_FeedbackCue.h"
#include "Items/Ch03_BaseItem.h"
#include "Ch03_ScoreItem.generated.h"


UCLASS()
class CH03_TASK05_API ACh03_ScoreItem : public ACh03_BaseItem
{
	GENERATED_BODY()
	
public:
	ACh03_ScoreItem();

protected:
	virtual void ActivateItem_Implementation(AActor* Activator) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "아이템|점수|피드백")
	void OnScoreCollectedFeedback(
		AActor* Activator,
		int32 BaseScore,
		int32 FinalScore);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|점수",
		meta = (DisplayName = "점수값", ClampMin = "0"))
	int32 ScoreValue = 50;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "아이템|점수|피드백",
		meta = (DisplayName = "점수 획득 피드백", ToolTip = "아이템 자체의 획득 피드백입니다. 천복이의 점수 획득 피드백이 비어 있을 때 예비로 사용합니다."))
	FCh03FeedbackCue ScorePickupFeedback;
};
