#include "Items/Ch03_DamageItem.h"

#include "Character/Ch03_CheonbokCharacter.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"

ACh03_DamageItem::ACh03_DamageItem()
{
	ItemType = TEXT("DamageItem");
	DamageTypeClass = UDamageType::StaticClass();
}

void ACh03_DamageItem::ActivateItem_Implementation(AActor* Activator)
{
	if (IsConsumed() || !CanBeActivatedBy(Activator) || DamageAmount <= 0.0f)
	{
		return;
	}

	ACh03_CheonbokCharacter* CheonbokCharacter =
		Cast<ACh03_CheonbokCharacter>(Activator);
	if (!CheonbokCharacter || CheonbokCharacter->IsDead())
	{
		return;
	}

	const float AppliedDamage = UGameplayStatics::ApplyDamage(
		CheonbokCharacter,
		DamageAmount,
		CheonbokCharacter->GetController(),
		this,
		DamageTypeClass);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("%s triggered %s. Applied damage: %.1f"),
		*GetNameSafe(Activator),
		*ItemType.ToString(),
		AppliedDamage);

	Super::ActivateItem_Implementation(Activator);
}
