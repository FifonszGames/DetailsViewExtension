// Copyright FifonszGames All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PropertyPathNodeTestTypes.generated.h"

USTRUCT(NotBlueprintType, meta=(Abstract, Hidden))
struct FPropertyPathNodeInternalTestStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName bEditableName;

	UPROPERTY()
	float NonEditableFloat = 1.0f;
};

USTRUCT(NotBlueprintType, meta=(Abstract, Hidden))
struct FPropertyPathNodeTestStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	bool bEditableBool = true;

	UPROPERTY(EditDefaultsOnly)
	TArray<int32> bEditableIntArray;

	UPROPERTY(EditDefaultsOnly)
	FVector bEditableVector3;

	UPROPERTY(EditDefaultsOnly)
	FPropertyPathNodeInternalTestStruct bEditableStruct;

	UPROPERTY()
	int32 NonEditableInt = 1;
};
