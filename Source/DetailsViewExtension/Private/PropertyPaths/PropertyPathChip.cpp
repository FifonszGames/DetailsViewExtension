// Copyright FifonszGames. All Rights Reserved.

#include "PropertyPaths/PropertyPathChip.h"

void SPropertyPathChip::Construct(const FArguments& InArgs)
{
	ChipText = InArgs._ChipText;
	OnClearPressed = InArgs._OnClearPressed;
	ChipState = InArgs._ChipState;
	
	TWeakPtr<SPropertyPathChip> WeakSelf = StaticCastWeakPtr<SPropertyPathChip>(AsWeak());

	ChildSlot
	[
		SNew(SBox)
		.HeightOverride(18.0f)
		[
			SNew(SBorder)
			.Padding(0.0f)
			.BorderBackgroundColor_Lambda([WeakSelf]()
			{
				TSharedPtr<SPropertyPathChip> Self = WeakSelf.Pin();
				return Self.IsValid() ? Self->StateData[Self->ChipState].Color : FStyleColors::White;
			})
			.ToolTipText_Lambda([WeakSelf]()
			{
				const TSharedPtr<SPropertyPathChip> Self = WeakSelf.Pin();
				return Self.IsValid() ? Self->StateData[Self->ChipState].TooltipText : FText::GetEmpty();
			})
			.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
			[
				SNew(SHorizontalBox)
				
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Left)
				.AutoWidth()
				[
					SNew(STextBlock)
					.Font(FAppStyle::GetFontStyle( TEXT("PropertyWindow.NormalFont")))
					.Text_Lambda([WeakSelf]()
					{
						const TSharedPtr<SPropertyPathChip> Self = WeakSelf.Pin();
						return Self.IsValid() ? Self->ChipText : FText::GetEmpty();
					})
				]
				
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Center)
				.Padding(2.f,0.f,0.f,0.f)
				[
					SNew(SButton)
					.ButtonStyle( FAppStyle::Get(), "SimpleButton" )
					.OnClicked( FOnClicked::CreateSPLambda(this, [WeakSelf]
					{
						TSharedPtr<SPropertyPathChip> Self = WeakSelf.Pin();
						if(Self.IsValid() && Self->OnClearPressed.IsBound())
						{
							Self->OnClearPressed.Execute();
						}
						return FReply::Handled();
					}))
					.ContentPadding(0.0f)
					.IsFocusable(false)
					[ 
						SNew( SImage )
						.Image( FAppStyle::GetBrush("Icons.XCircle") )
						.ColorAndOpacity( FSlateColor::UseForeground() )
					]
				]
			]
		]
	];
}
