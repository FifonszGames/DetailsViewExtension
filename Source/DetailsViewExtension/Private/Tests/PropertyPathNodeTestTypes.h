// Copyright FifonszGames All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PropertyPathNodeTestTypes.generated.h"

USTRUCT(NotBlueprintType, meta=(Abstract, Hidden))
struct FPropertyPathNodeInternalTestStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName EditableName;

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
	TArray<int32> EditableIntArray;

	UPROPERTY(EditDefaultsOnly)
	FVector EditableVector3;

	UPROPERTY(EditDefaultsOnly)
	FPropertyPathNodeInternalTestStruct EditableStruct;

	UPROPERTY()
	int32 NonEditableInt = 1;
};
