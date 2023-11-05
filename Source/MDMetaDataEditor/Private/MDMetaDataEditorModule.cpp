﻿// Copyright Dylan Dumesnil. All Rights Reserved.

#include "BlueprintEditorModule.h"
#include "Config/MDMetaDataEditorConfig.h"
#include "Customizations/MDMetaDataEditorFunctionCustomization.h"
#include "Customizations/MDMetaDataEditorPropertyTypeCustomization.h"
#include "Customizations/MDMetaDataEditorVariableCustomization.h"
#include "K2Node_CustomEvent.h"
#include "K2Node_FunctionEntry.h"
#include "K2Node_Tunnel.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"

class FMDMetaDataEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override
	{
		FBlueprintEditorModule& BlueprintEditorModule = FModuleManager::LoadModuleChecked<FBlueprintEditorModule>("Kismet");
		VariableCustomizationHandle = BlueprintEditorModule.RegisterVariableCustomization(FProperty::StaticClass(), FOnGetVariableCustomizationInstance::CreateStatic(&FMDMetaDataEditorVariableCustomization::MakeInstance));
		LocalVariableCustomizationHandle = BlueprintEditorModule.RegisterLocalVariableCustomization(FProperty::StaticClass(), FOnGetVariableCustomizationInstance::CreateStatic(&FMDMetaDataEditorVariableCustomization::MakeInstance));
		FunctionCustomizationHandle = BlueprintEditorModule.RegisterFunctionCustomization(UK2Node_FunctionEntry::StaticClass(), FOnGetFunctionCustomizationInstance::CreateStatic(&FMDMetaDataEditorFunctionCustomization::MakeInstance));
		TunnelCustomizationHandle = BlueprintEditorModule.RegisterFunctionCustomization(UK2Node_Tunnel::StaticClass(), FOnGetFunctionCustomizationInstance::CreateStatic(&FMDMetaDataEditorFunctionCustomization::MakeInstance));
		EventCustomizationHandle = BlueprintEditorModule.RegisterFunctionCustomization(UK2Node_CustomEvent::StaticClass(), FOnGetFunctionCustomizationInstance::CreateStatic(&FMDMetaDataEditorFunctionCustomization::MakeInstance));

		FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyEditorModule.RegisterCustomPropertyTypeLayout(FMDMetaDataEditorPropertyType::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FMDMetaDataEditorPropertyTypeCustomization::MakeInstance));
	}

	virtual void ShutdownModule() override
	{
		if (FBlueprintEditorModule* BlueprintEditorModule = FModuleManager::GetModulePtr<FBlueprintEditorModule>("Kismet"))
		{
			BlueprintEditorModule->UnregisterVariableCustomization(FProperty::StaticClass(), VariableCustomizationHandle);
			BlueprintEditorModule->UnregisterLocalVariableCustomization(FProperty::StaticClass(), LocalVariableCustomizationHandle);
			BlueprintEditorModule->UnregisterFunctionCustomization(UK2Node_FunctionEntry::StaticClass(), FunctionCustomizationHandle);
			BlueprintEditorModule->UnregisterFunctionCustomization(UK2Node_Tunnel::StaticClass(), TunnelCustomizationHandle);
			BlueprintEditorModule->UnregisterFunctionCustomization(UK2Node_CustomEvent::StaticClass(), EventCustomizationHandle);
		}

		if (FPropertyEditorModule* PropertyEditorModule = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor"))
		{
			PropertyEditorModule->UnregisterCustomPropertyTypeLayout(FMDMetaDataEditorPropertyType::StaticStruct()->GetFName());
		}
	}

private:
	FDelegateHandle VariableCustomizationHandle;
	FDelegateHandle LocalVariableCustomizationHandle;
	FDelegateHandle FunctionCustomizationHandle;
	FDelegateHandle TunnelCustomizationHandle;
	FDelegateHandle EventCustomizationHandle;
};

IMPLEMENT_MODULE(FMDMetaDataEditorModule, MDMetaDataEditor)