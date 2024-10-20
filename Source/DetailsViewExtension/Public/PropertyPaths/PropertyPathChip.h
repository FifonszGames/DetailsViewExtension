// Copyright FifonszGames. All Rights Reserved.

#pragma once

#include "Widgets/SCompoundWidget.h"

enum class EPropertyPathChipState
{
	Valid,
	Invalid
};

struct FPropertyPathChipData
{
	explicit FPropertyPathChipData() = default;
	FPropertyPathChipData(const FSlateColor& Color, const FText& TooltipText): Color(Color),TooltipText(TooltipText) {}
	FSlateColor Color;
	FText TooltipText;
};

class ClearButton;

class SPropertyPathChip : public SCompoundWidget
{
public:
	DECLARE_DELEGATE(FOnClearPressed);
	
	SLATE_BEGIN_ARGS(SPropertyPathChip)
		{
		}
		
		SLATE_EVENT(FOnClearPressed, OnClearPressed)
		
		SLATE_ARGUMENT(FText, ChipText)

		SLATE_ARGUMENT(EPropertyPathChipState, ChipState)
		
	SLATE_END_ARGS()
	
	void Construct(const FArguments& InArgs);
	
private:
	inline static const TMap<EPropertyPathChipState, FPropertyPathChipData> StateData = {
		{EPropertyPathChipState::Valid, FPropertyPathChipData(FStyleColors::White, FText::FromString("Valid"))},
		{EPropertyPathChipState::Invalid, FPropertyPathChipData(FStyleColors::AccentRed, FText::FromString("Invalid"))}
	};

	FText ChipText;
	EPropertyPathChipState ChipState = EPropertyPathChipState::Invalid;
	FOnClearPressed OnClearPressed;
	
	TSharedPtr<SButton> ClearButton;
};
