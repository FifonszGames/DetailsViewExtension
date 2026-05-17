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

		It("Should return the same name", [this]
		{
			TestEqual("Retrieved property name", Node->GetPropertyName(), TestPropertyName);
		});

		It("Should pass filters", [this]
		{
			TestTrue("Self filter", Node->PassesFilter(TestPropertyName));
			TestTrue("Filter of child", Node->PassesFilter(GET_MEMBER_NAME_STRING_CHECKED(FPropertyPathNodeTestStruct, bEditableBool)));
			TestFalse("Filter of no editable child", Node->PassesFilter(GET_MEMBER_NAME_STRING_CHECKED(FPropertyPathNodeTestStruct, bNonEditableInt)));
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
			const TSharedPtr<FPropertyPathNode> ArrayNode = Node->GetPropertyByPath(GET_MEMBER_NAME_STRING_CHECKED(FPropertyPathNodeTestStruct, bEditableIntArray));
			TestTrue("Editable Array property node is valid", ArrayNode.IsValid());

			const TSharedPtr<FPropertyPathNode> IntNode = Node->GetPropertyByPath(GET_MEMBER_NAME_STRING_CHECKED(FPropertyPathNodeTestStruct, bNonEditableInt));
			TestFalse("Non-editable int node is valid", IntNode.IsValid());
			
			const FString EditableStructPropName = GET_MEMBER_NAME_STRING_CHECKED(FPropertyPathNodeTestStruct, bEditableStruct);
			const FString EditableNamePropName = GET_MEMBER_NAME_STRING_CHECKED(FPropertyPathNodeInternalTestStruct, bEditableName);
			const FString NamePropPath = FString::Join(TArray{EditableStructPropName, EditableNamePropName}, *PropertyPathHelpers::Separator());
			const TSharedPtr<FPropertyPathNode> NamePropPathNode = Node->GetPropertyByPath(NamePropPath);
			TestTrue("Editable internal name property node is valid", NamePropPathNode.IsValid());
		});
	});

	Describe("Non Editable-Only Tests", [this]
	{
		BeforeEach([this]
		{
			Node->Initialize(*FPropertyPathNodeTestStruct::StaticStruct(), TestPropertyName, false);
		});

		//TODO:: implement tests
	});

	AfterEach([this]
	{
		Node.Reset();
	});
}
