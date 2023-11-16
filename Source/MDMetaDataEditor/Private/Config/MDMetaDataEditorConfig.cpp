// Copyright Dylan Dumesnil. All Rights Reserved.


#include "MDMetaDataEditorConfig.h"

#include "Animation/WidgetAnimation.h"
#include "Components/Widget.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "MDMetaDataEditorModule.h"
#include "Modules/ModuleManager.h"
#include "WidgetBlueprint.h"

#define LOCTEXT_NAMESPACE "MDMetaDataEditor"

UMDMetaDataEditorConfig::UMDMetaDataEditorConfig()
{
	// Setup some useful defaults, with some ugly code

	// Any property
	MetaDataKeys.Append({
		FMDMetaDataKey{ TEXT("EditCondition"), EMDMetaDataEditorKeyType::String, TEXT("Enter a condition to determine whether or not this property can be edited. Supports Bools and Enums.") }.CanBeUsedByFunctions(false),
		FMDMetaDataKey{ TEXT("EditConditionHides"), EMDMetaDataEditorKeyType::Flag, TEXT("If this property's EditCondition is false, it will be hidden.") }.CanBeUsedByFunctions(false).SetRequiredMetaData(TEXT("EditCondition")),
		FMDMetaDataKey{ TEXT("DisplayAfter"), EMDMetaDataEditorKeyType::String, TEXT("In the details panel, this property will be displayed after the property specified here.") }.CanBeUsedByFunctions(false),
		FMDMetaDataKey{ TEXT("DisplayPriority"), EMDMetaDataEditorKeyType::Integer, TEXT("The priority to display this property in the deatils panel, lower values are first.") }.CanBeUsedByFunctions(false),
		FMDMetaDataKey{ TEXT("NoResetToDefault"), EMDMetaDataEditorKeyType::Flag, TEXT("If set, this property will never show the 'Reset to Default' arrow button.") }.CanBeUsedByFunctions(false),
	});

	// Bool properties
	MetaDataKeys.Append({
		FMDMetaDataKey{ TEXT("InlineEditConditionToggle"), EMDMetaDataEditorKeyType::Flag, TEXT("If this bool is an EditCondition for another property, it will be displayed inline.") }.SetSupportedProperty({ UEdGraphSchema_K2::PC_Boolean})
	});

	// Any numeric property
	const TSet<FMDMetaDataEditorPropertyType> NumericTypes = {
		{ UEdGraphSchema_K2::PC_Int },
		{ UEdGraphSchema_K2::PC_Int64 },
		{ UEdGraphSchema_K2::PC_Float },
		{ UEdGraphSchema_K2::PC_Double },
		{ UEdGraphSchema_K2::PC_Real },
	};
	MetaDataKeys.Append({
			FMDMetaDataKey{ TEXT("NoSpinbox"), EMDMetaDataEditorKeyType::Boolean, TEXT("Disables the click and drag functionality for setting the value of this property.") }.SetSupportedProperties(NumericTypes)
	});

	// Integers
	const TSet<FMDMetaDataEditorPropertyType> IntegerTypes = {
		{ UEdGraphSchema_K2::PC_Int },
		{ UEdGraphSchema_K2::PC_Int64 },
	};
	MetaDataKeys.Append({
		FMDMetaDataKey{ TEXT("SliderExponent"), EMDMetaDataEditorKeyType::Integer, TEXT("How fast the value should change while dragging to set the value.") }.SetSupportedProperties(IntegerTypes).SetMinInt(1),
		FMDMetaDataKey{ TEXT("Delta"), EMDMetaDataEditorKeyType::Integer, TEXT("How much to change the value by when dragging.") }.SetSupportedProperties(IntegerTypes),
		FMDMetaDataKey{ TEXT("Multiple"), EMDMetaDataEditorKeyType::Integer, TEXT("Forces the property value to be a multiple of this value.") }.SetSupportedProperties(IntegerTypes),
		FMDMetaDataKey{ TEXT("ArrayClamp"), EMDMetaDataEditorKeyType::String, TEXT("Clamps the valid values that can be entered in the UI to be between 0 and the length of the array specified.") }.SetSupportedProperties(IntegerTypes)
	});

	// Float types
	const TSet<FMDMetaDataEditorPropertyType> FloatTypes = {
		{ UEdGraphSchema_K2::PC_Float },
		{ UEdGraphSchema_K2::PC_Double },
		{ UEdGraphSchema_K2::PC_Real },
	};
	MetaDataKeys.Append({
		FMDMetaDataKey{ TEXT("NoSpinbox"), EMDMetaDataEditorKeyType::Boolean, TEXT("Disables the click and drag functionality for setting the value of this property.") }.SetSupportedProperties(FloatTypes),
		FMDMetaDataKey{ TEXT("SliderExponent"), EMDMetaDataEditorKeyType::Float, TEXT("How fast the value should change while dragging to set the value.") }.SetSupportedProperties(FloatTypes).SetMinFloat(1.f),
		FMDMetaDataKey{ TEXT("Delta"), EMDMetaDataEditorKeyType::Float, TEXT("How much to change the value by when dragging.") }.SetSupportedProperties(FloatTypes),
		FMDMetaDataKey{ TEXT("Multiple"), EMDMetaDataEditorKeyType::Float, TEXT("Forces the property value to be a multiple of this value.") }.SetSupportedProperties(FloatTypes)
	});

	// Non-localized strings
	const TSet<FMDMetaDataEditorPropertyType> NonLocStringTypes = {
		{ UEdGraphSchema_K2::PC_String },
		{ UEdGraphSchema_K2::PC_Name }
	};
	MetaDataKeys.Append({
		FMDMetaDataKey{ TEXT("GetOptions"), EMDMetaDataEditorKeyType::String, TEXT("Specify a function that returns a list of Strings or Names that are valid values for this property. Seems to only support C++ functions since BP functions don't return anything for UFunction::GetReturnProperty().") }.SetSupportedProperties(NonLocStringTypes)
	});

	// Any Array
	MetaDataKeys.Append({
		FMDMetaDataKey{ TEXT("NoElementDuplicate"), EMDMetaDataEditorKeyType::Flag, TEXT("Indicates that the duplicate icon should not be shown for entries of this array in the property panel.") }.SetSupportedProperties({ FMDMetaDataEditorPropertyType{ UEdGraphSchema_K2::PC_Wildcard }.SetContainerType(EMDMetaDataPropertyContainerType::Array) })
	});

	// Any Map
	MetaDataKeys.Append({
		FMDMetaDataKey{ TEXT("ForceInlineRow"), EMDMetaDataEditorKeyType::Flag, TEXT("Force the Key and Value of a TMap to display in the same row.") }.SetSupportedProperties({ FMDMetaDataEditorPropertyType{ UEdGraphSchema_K2::PC_Wildcard }.SetValueType({UEdGraphSchema_K2::PC_Wildcard}).SetContainerType(EMDMetaDataPropertyContainerType::Map) })
	});

	// Array of Any Struct
	MetaDataKeys.Append({
		FMDMetaDataKey{ TEXT("TitleProperty"), EMDMetaDataEditorKeyType::String, TEXT("Specify a child property or FText style format of child properties to use as the summary.") }.SetSupportedProperties({ FMDMetaDataEditorPropertyType{ UEdGraphSchema_K2::PC_Struct }.SetContainerType(EMDMetaDataPropertyContainerType::Array) })
	});

	// Any Struct
	MetaDataKeys.Append({
		FMDMetaDataKey{ TEXT("ShowOnlyInnerProperties"), EMDMetaDataEditorKeyType::Flag, TEXT("Removes the struct layer in the details panel, directly displaying the child properties of the struct.") }.SetSupportedProperty({ UEdGraphSchema_K2::PC_Struct })
	});

	// Any Enum
	const TSet<FMDMetaDataEditorPropertyType> EnumTypes = {
		{ UEdGraphSchema_K2::PC_Enum, NAME_None, UEnum::StaticClass() },
		{ UEdGraphSchema_K2::PC_Byte, NAME_None, UEnum::StaticClass() }
	};
	MetaDataKeys.Append({
		FMDMetaDataKey{ TEXT("ValidEnumValues"), EMDMetaDataEditorKeyType::String, TEXT("Restricts selection to a subset of the enum's values.") }.SetSupportedProperties(EnumTypes),
		FMDMetaDataKey{ TEXT("InvalidEnumValues"), EMDMetaDataEditorKeyType::String, TEXT("Prevents selecting a subset of the enum's values.") }.SetSupportedProperties(EnumTypes)
	});

	// Gameplay Tags and Containers
	const TSet<FMDMetaDataEditorPropertyType> GameplayTagTypes = {
		{ UEdGraphSchema_K2::PC_Struct, NAME_None, FGameplayTag::StaticStruct() },
		{ UEdGraphSchema_K2::PC_Struct, NAME_None, FGameplayTagContainer::StaticStruct() }
	};
	MetaDataKeys.Append({
		FMDMetaDataKey{ TEXT("Categories"), EMDMetaDataEditorKeyType::GameplayTagContainer, TEXT("Limit which gameplay tags may be selected to one or more specific root tags.") }.SetSupportedProperties(GameplayTagTypes).SetDisplayNameOverride(LOCTEXT("Categories_DisplayName","Tag Filter"))
	});

	// Primary Asset IDs
	MetaDataKeys.Append({
		FMDMetaDataKey{ TEXT("AllowedTypes"), EMDMetaDataEditorKeyType::String, TEXT("Limit which Primary Data Assets may be selected to one or more specific Primary Asset Types.") }.SetSupportedProperty({ UEdGraphSchema_K2::PC_Struct, NAME_None, TBaseStructure<FPrimaryAssetId>::Get() })
	});

	// Data Tables
	const TSet<FMDMetaDataEditorPropertyType> DataTableTypes = {
		{ UEdGraphSchema_K2::PC_Struct, NAME_None, FDataTableRowHandle::StaticStruct() },
		{ UEdGraphSchema_K2::PC_Object, NAME_None, UDataTable::StaticClass() },
		{ UEdGraphSchema_K2::PC_SoftObject, NAME_None, UDataTable::StaticClass() }
	};
	MetaDataKeys.Append({
		FMDMetaDataKey{ TEXT("RowType"), EMDMetaDataEditorKeyType::String, TEXT("Limit the selection to a specific data table row struct type.") }.SetSupportedProperties(DataTableTypes),
		FMDMetaDataKey{ TEXT("RequiredAssetDataTags"), EMDMetaDataEditorKeyType::String, TEXT("Limit the selection to data tables with matching asset data tags.") }.SetSupportedProperties(DataTableTypes)
	});

	// Colors
	const TSet<FMDMetaDataEditorPropertyType> ColorTypes = {
		{ UEdGraphSchema_K2::PC_Struct, NAME_None, TBaseStructure<FLinearColor>::Get() },
		{ UEdGraphSchema_K2::PC_Struct, NAME_None, TBaseStructure<FColor>::Get() }
	};
	MetaDataKeys.Append({
		FMDMetaDataKey{ TEXT("HideAlphaChannel"), EMDMetaDataEditorKeyType::Flag, TEXT("Hide the alpha channel from the color picker.") }.SetSupportedProperties(ColorTypes)
	});

	// Any UObject type
	const TSet<FMDMetaDataEditorPropertyType> ObjectTypes = {
		{ UEdGraphSchema_K2::PC_Object, NAME_None, UObject::StaticClass() },
		{ UEdGraphSchema_K2::PC_SoftObject, NAME_None, UObject::StaticClass() },
		{ UEdGraphSchema_K2::PC_Class, NAME_None, UObject::StaticClass() },
		{ UEdGraphSchema_K2::PC_SoftClass, NAME_None, UObject::StaticClass() },
		{ UEdGraphSchema_K2::PC_Struct, NAME_None, TBaseStructure<FSoftObjectPath>::Get() },
		{ UEdGraphSchema_K2::PC_Struct, NAME_None, TBaseStructure<FSoftClassPath>::Get() }
	};
	MetaDataKeys.Append({
		FMDMetaDataKey{ TEXT("DisplayThumbnail"), EMDMetaDataEditorKeyType::Boolean, TEXT("Whether or not to display the asset thumbnail.") }.SetSupportedProperties(ObjectTypes),
		FMDMetaDataKey{ TEXT("NoClear"), EMDMetaDataEditorKeyType::Flag, TEXT("Prevent this propert from being clear/set to none.") }.SetSupportedProperties(ObjectTypes)
	});

	// Any soft reference to a UObject type
	const TSet<FMDMetaDataEditorPropertyType> SoftObjectTypes = {
		{ UEdGraphSchema_K2::PC_SoftObject, NAME_None, UObject::StaticClass() },
		{ UEdGraphSchema_K2::PC_SoftClass, NAME_None, UObject::StaticClass() },
		{ UEdGraphSchema_K2::PC_Struct, NAME_None, TBaseStructure<FSoftObjectPath>::Get() },
		{ UEdGraphSchema_K2::PC_Struct, NAME_None, TBaseStructure<FSoftClassPath>::Get() }
	};
	MetaDataKeys.Append({
		FMDMetaDataKey{ TEXT("AssetBundles"), EMDMetaDataEditorKeyType::String, TEXT("The name of the bundle to store the secondary asset in.") }.SetSupportedProperties(SoftObjectTypes),
		FMDMetaDataKey{ TEXT("Untracked"), EMDMetaDataEditorKeyType::Flag, TEXT("Specify that the soft reference should not be tracked and therefore not automatically cooked or checked during delete or redirector fixup.") }.SetSupportedProperties(SoftObjectTypes)
	});

	// Any UClass type
	const TSet<FMDMetaDataEditorPropertyType> ClassTypes = {
		{ UEdGraphSchema_K2::PC_Class, NAME_None, UObject::StaticClass() },
		{ UEdGraphSchema_K2::PC_SoftClass, NAME_None, UObject::StaticClass() },
		{ UEdGraphSchema_K2::PC_Struct, NAME_None, TBaseStructure<FSoftClassPath>::Get() }
	};
	MetaDataKeys.Append({
		FMDMetaDataKey{ TEXT("AllowAbstract"), EMDMetaDataEditorKeyType::Flag, TEXT("Include abstract classes in the class picker for this property.") }.SetSupportedProperties(ClassTypes),
		FMDMetaDataKey{ TEXT("ShowTreeView"), EMDMetaDataEditorKeyType::Flag, TEXT("Show a tree of class inheritence instead of a list view for the class picker.") }.SetSupportedProperties(ClassTypes),
		FMDMetaDataKey{ TEXT("BlueprintBaseOnly"), EMDMetaDataEditorKeyType::Flag, TEXT("Only allow selecting blueprint classes.") }.SetSupportedProperties(ClassTypes),
		FMDMetaDataKey{ TEXT("ExactClass"), EMDMetaDataEditorKeyType::Flag, TEXT("Only allow selecting specifically from the list of allowed classes, no subclasses.") }.SetSupportedProperties(ClassTypes).SetRequiredMetaData(TEXT("AllowedClasses")),
		FMDMetaDataKey{ TEXT("MustImplement"), EMDMetaDataEditorKeyType::String, TEXT("Only allow classes that inherit the specified interface.") }.SetSupportedProperties(ClassTypes),
		FMDMetaDataKey{ TEXT("AllowedClasses"), EMDMetaDataEditorKeyType::String, TEXT("Only allow selecting specific classes/interfaces or subclasses of them.") }.SetSupportedProperties(ClassTypes)
	});

	// UMG Only
	MetaDataKeys.Append({
		FMDMetaDataKey{ TEXT("DesignerRebuild"), EMDMetaDataEditorKeyType::Flag, TEXT("When this property changes, the widget preview will be rebuilt."), TEXT("UMG") }.SetSupportedBlueprint(UWidgetBlueprint::StaticClass()),
		FMDMetaDataKey{ TEXT("BindWidget"), EMDMetaDataEditorKeyType::Flag, TEXT("This property requires a widget be bound to it in any child Widget Blueprints."), TEXT("UMG|Bind Widget") }.SetSupportedBlueprint(UWidgetBlueprint::StaticClass()).SetSupportedObjectProperty(UWidget::StaticClass()).SetIncompatibleMetaData(TEXT("BindWidgetOptional")),
		FMDMetaDataKey{ TEXT("BindWidgetOptional"), EMDMetaDataEditorKeyType::Flag, TEXT("This property allows a widget be bound to it in any child Widget Blueprints."), TEXT("UMG|Bind Widget") }.SetSupportedBlueprint(UWidgetBlueprint::StaticClass()).SetSupportedObjectProperty(UWidget::StaticClass()).SetIncompatibleMetaData(TEXT("BindWidget")),
		FMDMetaDataKey{ TEXT("BindWidgetAnim"), EMDMetaDataEditorKeyType::Flag, TEXT("This property requires a widget animation be bound to it in any child Widget Blueprints."), TEXT("UMG|Bind Widget Anim") }.SetSupportedBlueprint(UWidgetBlueprint::StaticClass()).SetSupportedObjectProperty(UWidgetAnimation::StaticClass()).SetIncompatibleMetaData(TEXT("BindWidgetAnimOptional")),
		FMDMetaDataKey{ TEXT("BindWidgetAnimOptional"), EMDMetaDataEditorKeyType::Flag, TEXT("This property allows a widget animation be bound to it in any child Widget Blueprints."), TEXT("UMG|Bind Widget Anim") }.SetSupportedBlueprint(UWidgetBlueprint::StaticClass()).SetSupportedObjectProperty(UWidgetAnimation::StaticClass()).SetIncompatibleMetaData(TEXT("BindWidgetAnim"))
	});

	// Functions Only
	MetaDataKeys.Append({
		FMDMetaDataKey{ TEXT("DefaultToSelf"), EMDMetaDataEditorKeyType::String, TEXT("Specify which function parameter should default to \"self\".") }.SetFunctionsOnly()
	});

	// Sort pre-defined keys
	MetaDataKeys.Sort([](const FMDMetaDataKey& A, const FMDMetaDataKey& B)
	{
		return A.Key.Compare(B.Key) < 0;
	});
}

void UMDMetaDataEditorConfig::PostInitProperties()
{
	Super::PostInitProperties();

	if (bSortMetaDataAlphabetically)
	{
		// TODO - This also needs to happen when saving, but we don't want to rearrange the list while it's being edited so we'd need to know when the user leaves the Project settings screen or something
		MetaDataKeys.Sort([](const FMDMetaDataKey& A, const FMDMetaDataKey& B)
		{
			return A.Key.Compare(B.Key) < 0;
		});
	}
}

FText UMDMetaDataEditorConfig::GetSectionText() const
{
	return INVTEXT("Meta Data Editor");
}

void UMDMetaDataEditorConfig::ForEachVariableMetaDataKey(const UBlueprint* Blueprint, const FProperty* Property, const TFunction<void(const FMDMetaDataKey&)>& Func) const
{
	if (!IsValid(Blueprint) || Property == nullptr)
	{
		return;
	}

	for (const FMDMetaDataKey& Key : MetaDataKeys)
	{
		if (!Key.DoesSupportBlueprint(Blueprint))
		{
			continue;
		}

		if (Key.DoesSupportProperty(Property))
		{
			Func(Key);
		}
	}
}

void UMDMetaDataEditorConfig::ForEachFunctionMetaDataKey(const UBlueprint* Blueprint, const TFunction<void(const FMDMetaDataKey&)>& Func) const
{
	if (!IsValid(Blueprint))
	{
		return;
	}

	for (const FMDMetaDataKey& Key : MetaDataKeys)
	{
		if (!Key.bCanBeUsedByFunctions || !Key.DoesSupportBlueprint(Blueprint))
		{
			continue;
		}

		Func(Key);
	}
}

#if WITH_EDITOR
void UMDMetaDataEditorConfig::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (FMDMetaDataEditorModule* Module = FModuleManager::GetModulePtr<FMDMetaDataEditorModule>(TEXT("MDMetaDataEditor")))
	{
		Module->RestartModule();
	}
}
#endif //WITH_EDITOR

TArray<FName> UMDMetaDataEditorConfig::GetMetaDataKeyNames() const
{
	TArray<FName> Result;
	Result.Add(NAME_None);

	Algo::Transform(MetaDataKeys, Result, [](const FMDMetaDataKey& Key)
	{
		return Key.Key;
	});

	return Result;
}

#undef LOCTEXT_NAMESPACE
