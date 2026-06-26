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
		meta = (ToolTip = "점수를 주는 사료 아이템을 획득했을 때 재생되는 공통 피드백입니다. 작은 사료, 큰 사료처럼 점수량이 달라도 같은 이펙트를 공유할 수 있습니다."))
	FCh03FeedbackCue ScorePickupFeedback;
};
