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

	UFUNCTION(BlueprintImplementableEvent, Category = "Item|Score|Feedback")
	void OnScoreCollectedFeedback(
		AActor* Activator,
		int32 BaseScore,
		int32 FinalScore);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Score",
		meta = (ClampMin = "0"))
	int32 ScoreValue = 50;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Score|Feedback",
		meta = (ToolTip = "점수 아이템 자체의 획득 피드백입니다. 천복 캐릭터의 Score Pickup Feedback이 설정되어 있으면 캐릭터 쪽 피드백을 우선 사용하고, 이 값은 fallback으로 사용됩니다."))
	FCh03FeedbackCue ScorePickupFeedback;
};
