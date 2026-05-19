// Copyright FifonszGames 2025 All Rights Reserved.


#include "PropertyPathNodeTestTypes.h"
#include "Misc/AutomationTest.h"
#include "PropertyPaths/PropertyPathNode.h"
#include "PropertyPaths/PropertyPathsHelpers.h"

BEGIN_DEFINE_SPEC(FPropertyPathNodeSpec, "DetailsViewExtension.PropertyPathNode", EAutomationTestFlags::EngineFilter | EAutomationTestFlags::EditorContext)
	TSharedPtr<FPropertyPathNode> Node;
	FString TestPropertyName{TEXT("TestProperty")};
END_DEFINE_SPEC(FPropertyPathNodeSpec)

void FPropertyPathNodeSpec::Define()
{
	BeforeEach([this]
	{
		Node = MakeShared<FPropertyPathNode>();
	});

	Describe("Editable Only Tests", [this]
	{
		BeforeEach([this]
		{
			Node->Initialize(*FPropertyPathNodeTestStruct::StaticStruct(), TestPropertyName, true);
		});

		It("Should return filtered children", [this]
		{
			TArray<TSharedPtr<FPropertyPathNode>> EditableChildren = Node->GetChildren();
			int32 ExpectedChildrenNum = 4;
			TestEqual("Num of children with empty filter", EditableChildren.Num(), ExpectedChildrenNum);

			EditableChildren = Node->GetChildren(TEXT("editable"));
			ExpectedChildrenNum = 4;
			TestEqual("Num of children filtered by: editable", EditableChildren.Num(), ExpectedChildrenNum);

			EditableChildren = Node->GetChildren(TEXT("struct"));
			ExpectedChildrenNum = 1;
			TestEqual("Num of children filtered by: struct", EditableChildren.Num(), ExpectedChildrenNum);

			const TSharedPtr<FPropertyPathNode> EditableStructNode = EditableChildren[0];
			EditableChildren = EditableStructNode->GetChildren(TEXT("name"));
			ExpectedChildrenNum = 1;
			TestEqual("Num of children filtered by: name, on child node", EditableChildren.Num(), ExpectedChildrenNum);
		});

		It("Should return the same name", [this]
		{
			TestEqual("Retrieved property name", Node->GetPropertyName(), TestPropertyName);
		});

		It("Should pass filters", [this]
		{
			TestTrue("Self filter", Node->PassesFilter(TestPropertyName));
			TestTrue("Filter of child", Node->PassesFilter(GET_MEMBER_NAME_STRING_CHECKED(FPropertyPathNodeTestStruct, bEditableBool)));
			TestFalse("Filter of no editable child", Node->PassesFilter(GET_MEMBER_NAME_STRING_CHECKED(FPropertyPathNodeTestStruct, NonEditableInt)));
		});

		It("Should return correct total path", [this]
		{
			const FString EditableBoolName = GET_MEMBER_NAME_STRING_CHECKED(FPropertyPathNodeTestStruct, bEditableBool);
			const TArray<TSharedPtr<FPropertyPathNode>> Children = Node->GetChildren(EditableBoolName);
			const TSharedPtr<FPropertyPathNode> EditableBoolNode = Children[0];

			TestEqual("Root total path", Node->GetTotalPath(), TestPropertyName);
			TestEqual("Root total path", EditableBoolNode->GetTotalPath(), EditableBoolName);
		});

		It("Should return correct node by path", [this]
		{
			const TSharedPtr<const FPropertyPathNode> ArrayNode = Node->GetPropertyByPath(GET_MEMBER_NAME_STRING_CHECKED(FPropertyPathNodeTestStruct, EditableIntArray));
			TestTrue("Editable Array property node is valid", ArrayNode.IsValid());

			const TSharedPtr<const FPropertyPathNode> IntNode = Node->GetPropertyByPath(GET_MEMBER_NAME_STRING_CHECKED(FPropertyPathNodeTestStruct, NonEditableInt));
			TestFalse("Non-editable int node is valid", IntNode.IsValid());

			const FString EditableStructPropName = GET_MEMBER_NAME_STRING_CHECKED(FPropertyPathNodeTestStruct, EditableStruct);
			const FString EditableNamePropName = GET_MEMBER_NAME_STRING_CHECKED(FPropertyPathNodeInternalTestStruct, EditableName);
			const FString NamePropPath = FString::Join(TArray{EditableStructPropName, EditableNamePropName}, *PropertyPathHelpers::Separator());
			const TSharedPtr<const FPropertyPathNode> NamePropPathNode = Node->GetPropertyByPath(NamePropPath);
			TestTrue("Editable internal name property node is valid", NamePropPathNode.IsValid());
		});

		It("Should add outermost children only", [this]
		{
			TArray<TSharedPtr<FPropertyPathNode>> OutermostEditableChildren;
			Node->FillWithOutermostChildren(OutermostEditableChildren);
			constexpr int32 ExpectedChildrenNum = 4;
			TestEqual("Number of outermost children", OutermostEditableChildren.Num(), ExpectedChildrenNum);
		});
	});

	Describe("Non-editable Tests", [this]
	{
		BeforeEach([this]
		{
			Node->Initialize(*FPropertyPathNodeTestStruct::StaticStruct(), TestPropertyName, false);
		});

		It("Should return filtered children", [this]
		{
			TArray<TSharedPtr<FPropertyPathNode>> EditableChildren = Node->GetChildren();
			int32 ExpectedChildrenNum = 5;
			TestEqual("Num of children with empty filter", EditableChildren.Num(), ExpectedChildrenNum);

			EditableChildren = Node->GetChildren(TEXT("editable"));
			ExpectedChildrenNum = 5;
			TestEqual("Num of children filtered by: editable", EditableChildren.Num(), ExpectedChildrenNum);

			EditableChildren = Node->GetChildren(TEXT("struct"));
			ExpectedChildrenNum = 1;
			TestEqual("Num of children filtered by: struct", EditableChildren.Num(), ExpectedChildrenNum);

			const TSharedPtr<FPropertyPathNode> EditableStructNode = EditableChildren[0];
			EditableChildren = EditableStructNode->GetChildren();
			ExpectedChildrenNum = 2;
			TestEqual("Num of children filtered by: name, on child node", EditableChildren.Num(), ExpectedChildrenNum);
		});

		It("Should pass filters", [this]
		{
			TestTrue("Filter of non-editable child", Node->PassesFilter(GET_MEMBER_NAME_STRING_CHECKED(FPropertyPathNodeTestStruct, NonEditableInt)));
		});

		It("Should return correct total path", [this]
		{
			const FString NonEditableIntName = GET_MEMBER_NAME_STRING_CHECKED(FPropertyPathNodeTestStruct, NonEditableInt);
			const TArray<TSharedPtr<FPropertyPathNode>> Children = Node->GetChildren(NonEditableIntName);
			const TSharedPtr<FPropertyPathNode> NonEditableIntNode = Children[0];

			TestEqual("Root total path", Node->GetTotalPath(), TestPropertyName);
			TestEqual("Root total path", NonEditableIntNode->GetTotalPath(), NonEditableIntName);
		});

		It("Should return correct node by path", [this]
		{
			const TSharedPtr<const FPropertyPathNode> IntNode = Node->GetPropertyByPath(GET_MEMBER_NAME_STRING_CHECKED(FPropertyPathNodeTestStruct, NonEditableInt));
			TestTrue("Non-const editable int node is valid", IntNode.IsValid());

			const FString EditableStructPropName = GET_MEMBER_NAME_STRING_CHECKED(FPropertyPathNodeTestStruct, EditableStruct);
			const FString NonEditableNamePropFloat = GET_MEMBER_NAME_STRING_CHECKED(FPropertyPathNodeInternalTestStruct, NonEditableFloat);
			const FString FloatPropPath = FString::Join(TArray{EditableStructPropName, NonEditableNamePropFloat}, *PropertyPathHelpers::Separator());
			const TSharedPtr<const FPropertyPathNode> FloatPropPathNode = Node->GetPropertyByPath(FloatPropPath);
			TestTrue("Non-editable internal float property node is valid", FloatPropPathNode.IsValid());
		});

		It("Should add outermost children only", [this]
		{
			TArray<TSharedPtr<FPropertyPathNode>> OutermostEditableChildren;
			Node->FillWithOutermostChildren(OutermostEditableChildren);
			constexpr int32 ExpectedChildrenNum = 6;
			TestEqual("Number of outermost children", OutermostEditableChildren.Num(), ExpectedChildrenNum);
		});
	});

	AfterEach([this]
	{
		Node.Reset();
	});
}
