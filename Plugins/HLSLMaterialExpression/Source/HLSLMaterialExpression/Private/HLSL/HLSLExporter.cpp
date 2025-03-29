// Fill out your copyright notice in the Description page of Project Settings.


#include "HLSL/HLSLExporter.h"
#include "HLSL/Expression.h"
#include "HLSLUtils.h"
#include "Materials/MaterialFunction.h"
#include "Materials/MaterialExpressionFunctionInput.h"
#include "Materials/MaterialExpressionFunctionOutput.h"
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 2
#include "Editor/MaterialEditor/Private/MaterialEditor.h"
#else
#include "MaterialEditor/Private/MaterialEditor.h"
#endif

#include "MaterialEditorModule.h"
#include "Subsystems/AssetEditorSubsystem.h"
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 2
#include "DesktopPlatformModule.h"
#else
#include "DesktopPlatform/Public/DesktopPlatformModule.h"
#endif
#include "GenericPlatform/GenericPlatformFile.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "MaterialEditingLibrary.h"
#include "Engine/Selection.h"
#include "Editor.h"

EFunctionInputType Convert2MaterialFunctionInputType(EMaterialValueType InValueType)
{
	EFunctionInputType OutInputType = EFunctionInputType::FunctionInput_Scalar;
	switch (InValueType)
	{
	case MCT_Float1:
		OutInputType = EFunctionInputType::FunctionInput_Scalar;
		break;
	case MCT_Float2:
		OutInputType = EFunctionInputType::FunctionInput_Vector2;
		break;
	case MCT_Float3:
		OutInputType = EFunctionInputType::FunctionInput_Vector3;
		break;
	case MCT_Float4:
		OutInputType = EFunctionInputType::FunctionInput_Vector4;
		break;
	case MCT_Float:
		OutInputType = EFunctionInputType::FunctionInput_Scalar;
		break;
	case MCT_Texture:
		OutInputType = EFunctionInputType::FunctionInput_Texture2D;
		break;
	case MCT_Texture2D:
		OutInputType = EFunctionInputType::FunctionInput_Texture2D;
		break;
	case MCT_TextureCube:
		OutInputType = EFunctionInputType::FunctionInput_TextureCube;
		break;
	case MCT_Texture2DArray:
		OutInputType = EFunctionInputType::FunctionInput_Texture2DArray;
		break;
	case MCT_VolumeTexture:
		OutInputType = EFunctionInputType::FunctionInput_VolumeTexture;
		break;
	case MCT_TextureExternal:
		OutInputType = EFunctionInputType::FunctionInput_TextureExternal;
		break;
	case MCT_MaterialAttributes:
		OutInputType = EFunctionInputType::FunctionInput_MaterialAttributes;
		break;
	}
	return OutInputType;
}

TSharedPtr<FHLSLExporter> HLSLExporter = nullptr;
void FHLSLExporter::Register()
{
	HLSLExporter = MakeShareable(new FHLSLExporter);
// 	IMaterialEditorModule& MaterialEditorModule = FModuleManager::LoadModuleChecked<IMaterialEditorModule>("MaterialEditor");
// 	MaterialEditorModule.OnMaterialEditorOpened().AddLambda([](TWeakPtr<IMaterialEditor> WeakMaterialEditor)
// 	{
// 		HLSLExporter->MaterialEditors.Add(WeakMaterialEditor);
// 	});
}

void FHLSLExporter::SaveAsMaterialFunction(UHLSLMaterialExpression* InHLSLExpression)
{
    if (!HLSLExporter.IsValid() || !InHLSLExpression)
	{
		HLSLUtils::HLSLCompileError(TEXT("Can't Find Material!"));
		return;
	}

	//File Name
	TArray<FString> SaveFilenames;
	const FString DefaultDirectory = FPaths::ProjectContentDir();
	const bool bSaved = FDesktopPlatformModule::Get()->SaveFileDialog
	(
		FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr),
		TEXT("Save HLSL Expression"),
		DefaultDirectory,
		InHLSLExpression->Description.IsEmpty() ?
		TEXT("MF_HLSLExpression") :
		FString::Printf(TEXT("MF_%s"), *(InHLSLExpression->Description)),
		TEXT(""),
		EFileDialogFlags::None,
		SaveFilenames
	);

	if (SaveFilenames.Num() == 0)
	{
		HLSLUtils::HLSLCompileError(TEXT("Invalid file name!"));
		return;
	}

	if(FPaths::FileExists(FPaths::SetExtension(SaveFilenames[0],TEXT("uasset"))))
	{
		HLSLUtils::HLSLCompileError(TEXT("The file already exists!"));
		return;
	}

	//Create Package
	FString FailureReason;
	FString MF_PackageName;
	if (!FPackageName::TryConvertFilenameToLongPackageName(SaveFilenames[0], MF_PackageName, &FailureReason))
	{
		HLSLUtils::HLSLCompileError(TEXT("Invalid save path!"));
		return;
	}

	UPackage* MF_Package = CreatePackage(*MF_PackageName);

	//Create Material Function
	UMaterialFunction* NewMaterialFunction = NewObject<UMaterialFunction>(MF_Package, UMaterialFunction::StaticClass(), *(FPaths::GetBaseFilename(MF_PackageName)), RF_Public | RF_Standalone);
	NewMaterialFunction->StateId = FGuid::NewGuid();
	NewMaterialFunction->Description = TEXT("");
	NewMaterialFunction->LibraryCategoriesText.Add(NSLOCTEXT("HLSL Expression", "Misc", "Misc"));
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 0
	NewMaterialFunction->GetEditorOnlyData()->ExpressionCollection.Empty();
#else
	NewMaterialFunction->FunctionExpressions.Empty();
#endif

	int32 SortPriority = 0;
	//Generate Inputs
	{
		int32 InputNodeHeight = 256;
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 2
		TArrayView<FExpressionInput*> HLSLInputs = InHLSLExpression->GetInputsView();
#else
		TArray<FExpressionInput*> HLSLInputs = InHLSLExpression->GetInputs();
#endif
		for (int32 Index = 0; Index < HLSLInputs.Num(); Index++)
		{
			UMaterialExpressionFunctionInput* InputExpression = NewObject<UMaterialExpressionFunctionInput>(NewMaterialFunction);
			InputExpression->SortPriority = SortPriority;
			InputExpression->InputName = InHLSLExpression->GetInputName(Index);
			InputExpression->InputType = Convert2MaterialFunctionInputType((EMaterialValueType)InHLSLExpression->GetInputType(Index));
			InputExpression->Description = TEXT("");
			InputExpression->MaterialExpressionEditorX = 0;
			InputExpression->MaterialExpressionEditorY = InputNodeHeight * Index;
			InputExpression->PostLoad();
			InputExpression->PostEditChange();
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 0
			NewMaterialFunction->GetEditorOnlyData()->ExpressionCollection.AddExpression(InputExpression);
#else
			NewMaterialFunction->FunctionExpressions.Add(InputExpression);
#endif
			++SortPriority;
		}
	}

	// Generate Entry
	UMaterialExpressionCustom* MF_EntryFunction = NewObject<UMaterialExpressionCustom>(NewMaterialFunction);
	{
		MF_EntryFunction->MaterialExpressionEditorX = 128;
		MF_EntryFunction->MaterialExpressionEditorY = -256;
		MF_EntryFunction->Code = InHLSLExpression->HLSLFunctions->Code;

		MF_EntryFunction->Inputs.Empty();
		MF_EntryFunction->bShowInputs = false;

		MF_EntryFunction->Outputs.Reset(1);
		MF_EntryFunction->Outputs.Add(FExpressionOutput(HLSL_OUTPUT_PIXEL_NAME));
		MF_EntryFunction->AdditionalOutputs.Empty();

		MF_EntryFunction->bShowOutputs = true;
		MF_EntryFunction->bShowOutputNameOnPin = true;

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 0
		NewMaterialFunction->GetEditorOnlyData()->ExpressionCollection.AddExpression(MF_EntryFunction);
#else
		NewMaterialFunction->FunctionExpressions.Add(MF_EntryFunction);
#endif
	}

	// Generate Main
	UMaterialExpressionCustom* MF_MainFunction = NewObject<UMaterialExpressionCustom>(NewMaterialFunction);
	{
		MF_MainFunction->MaterialExpressionEditorX = 512;
		MF_MainFunction->MaterialExpressionEditorY = 0;
		MF_MainFunction->Code = InHLSLExpression->HLSLMain->Code;

		//Inputs
		{
			int32 NumInputs = InHLSLExpression->GetInputs().Num();
			MF_MainFunction->Inputs.Empty();

			FCustomInput& EntryInputRef = MF_MainFunction->Inputs.Emplace_GetRef();
			EntryInputRef.InputName = HLSL_INPUT_FUNCTIONS_NAME;
			EntryInputRef.Input.Expression = nullptr;
			EntryInputRef.Input.OutputIndex = 0;

			for (int32 Index = 0; Index < NumInputs; Index++)
			{
				FCustomInput& InputRef = MF_MainFunction->Inputs.Emplace_GetRef();
				InputRef.InputName = InHLSLExpression->GetInputName(Index);
				InputRef.Input.Expression = nullptr;
				InputRef.Input.OutputIndex = 0;
			}
			MF_MainFunction->bShowInputs = true;
		}

		//Outputs
		{
			TArray<FCustomOutput>& CopyOutputs = InHLSLExpression->HLSLMain->AdditionalOutputs;
			MF_MainFunction->Outputs.Reset(CopyOutputs.Num() + 1);
			MF_MainFunction->Outputs.Add(FExpressionOutput(HLSL_OUTPUT_PIXEL_NAME));
			for (int32 Index = 0; Index < CopyOutputs.Num(); Index++)
			{
				FCustomOutput& OutputRef = CopyOutputs[Index];
				MF_MainFunction->Outputs.Add(FExpressionOutput(OutputRef.OutputName));
			}
			MF_MainFunction->AdditionalOutputs = CopyOutputs;

			MF_MainFunction->bShowOutputs = true;
			MF_MainFunction->bShowOutputNameOnPin = true;
		}

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 0
		NewMaterialFunction->GetEditorOnlyData()->ExpressionCollection.AddExpression(MF_MainFunction);
#else
		NewMaterialFunction->FunctionExpressions.Add(MF_MainFunction);
#endif
	}

	//Generate Outputs
	{
		int32 OutputNodeOffset = 0;
		int32 OutputNodeHeight = 256;
		TArray<FExpressionOutput>& Outputs = InHLSLExpression->GetOutputs();
		for (int32 Index = 0; Index < Outputs.Num(); Index++)
		{
			FExpressionOutput& OutputRef = Outputs[Index];
			if(OutputRef.OutputName.ToString() == HLSL_OUTPUT_PIXEL_NAME)
				continue;

			UMaterialExpressionFunctionOutput* OutputExpression = NewObject<UMaterialExpressionFunctionOutput>(NewMaterialFunction);
			OutputExpression->SortPriority = SortPriority;
			OutputExpression->OutputName = OutputRef.OutputName;
			OutputExpression->Description = TEXT("");
			OutputExpression->MaterialExpressionEditorX = 1000;
			OutputExpression->MaterialExpressionEditorY = OutputNodeOffset * OutputNodeHeight;
			OutputExpression->PostLoad();
			OutputExpression->PostEditChange();

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 0
			NewMaterialFunction->GetEditorOnlyData()->ExpressionCollection.AddExpression(OutputExpression);
#else
			NewMaterialFunction->FunctionExpressions.Add(OutputExpression);
#endif

			++SortPriority;
			++OutputNodeOffset;
		}
	}

	//Connect Entry
	{
		MF_MainFunction->Inputs[0].Input.Connect(0, MF_EntryFunction);
	}

	//Connect Inputs
	{
		for (int32 Index = 1; Index < MF_MainFunction->Inputs.Num(); Index++)
		{
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 0
			MF_MainFunction->Inputs[Index].Input.Connect(0, NewMaterialFunction->GetEditorOnlyData()->ExpressionCollection.Expressions[Index - 1]); 
#else
			MF_MainFunction->Inputs[Index].Input.Connect(0, NewMaterialFunction->FunctionExpressions[Index - 1]);
#endif
		}
	}

	//Connect Outputs
	{
		int32 Start = MF_MainFunction->Inputs.Num() - 1 + 2;
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 0
		for (int32 Index = Start; Index < NewMaterialFunction->GetEditorOnlyData()->ExpressionCollection.Expressions.Num(); Index++)
		{
			NewMaterialFunction->GetEditorOnlyData()->ExpressionCollection.Expressions[Index]->GetInput(0)->Connect(Index - Start + 1, MF_MainFunction);
		}
#else
		for (int32 Index = Start; Index < NewMaterialFunction->FunctionExpressions.Num(); Index++)
		{
			NewMaterialFunction->FunctionExpressions[Index]->GetInput(0)->Connect(Index - Start + 1, MF_MainFunction);
		}
#endif
	}

// 	MF_EntryFunction->PostEditChange();
// 	MF_MainFunction->PostEditChange();

	//Create Asset
	FAssetRegistryModule::AssetCreated(NewMaterialFunction);
	NewMaterialFunction->PostLoad();
	NewMaterialFunction->PostEditChange();
	NewMaterialFunction->MarkPackageDirty();

	TArray<UObject*> ObjectsToSync;
	ObjectsToSync.Add(NewMaterialFunction);
	GEditor->SyncBrowserToObjects(ObjectsToSync);
	GEditor->GetSelectedObjects()->Select(NewMaterialFunction, true);
}

FHLSLExporter::FHLSLExporter()
{
}

FHLSLExporter::~FHLSLExporter()
{
}



