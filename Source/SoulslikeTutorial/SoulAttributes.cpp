// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulAttributes.h"
#include "Net/UnrealNetwork.h"

USoulAttributes::USoulAttributes()
{
}

void USoulAttributes::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME_CONDITION_NOTIFY(USoulAttributes, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USoulAttributes, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USoulAttributes, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USoulAttributes, Vigor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USoulAttributes, Mind, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USoulAttributes, Endurance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USoulAttributes, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USoulAttributes, Dexterity, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USoulAttributes, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USoulAttributes, Faith, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USoulAttributes, Luck, COND_None, REPNOTIFY_Always);
}

void USoulAttributes::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USoulAttributes, Health, OldHealth);
}

void USoulAttributes::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USoulAttributes, Mana, OldMana);
}

void USoulAttributes::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USoulAttributes, Stamina, OldStamina);
}

void USoulAttributes::OnRep_Vigor(const FGameplayAttributeData& OldVigor)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USoulAttributes, Vigor, OldVigor);
}

void USoulAttributes::OnRep_Mind(const FGameplayAttributeData& OldMind)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USoulAttributes, Mind, OldMind);	
}

void USoulAttributes::OnRep_Endurance(const FGameplayAttributeData& OldEndurance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USoulAttributes, Endurance, OldEndurance);
}

void USoulAttributes::OnRep_Strength(const FGameplayAttributeData& OldStrength)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USoulAttributes, Strength, OldStrength);
}

void USoulAttributes::OnRep_Dexterity(const FGameplayAttributeData& OldDexterity)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USoulAttributes, Dexterity, OldDexterity);
}

void USoulAttributes::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USoulAttributes, Intelligence, OldIntelligence);
}

void USoulAttributes::OnRep_Faith(const FGameplayAttributeData& OldFaith)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USoulAttributes, Faith, OldFaith);
}

void USoulAttributes::OnRep_Luck(const FGameplayAttributeData& OldLuck)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USoulAttributes, Luck, OldLuck);
}
