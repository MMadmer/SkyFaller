// Copyright 2023 ZSttt, Inc. All Rights Reserved.

#include "HLSL/Expression.h"
#include "HLSL/HLSLParser.h"
#include "MaterialCompiler.h"
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 2
#include "MaterialEditorUtilities.h"
#else
#include "MaterialEditor/Public/MaterialEditorUtilities.h"
#endif

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 0
#include "Shader/PreshaderTypes.h"
#include "Misc/MemStackUtility.h"
#include "HLSLTree/HLSLTree.h"
#include "HLSLTree/HLSLTreeCommon.h"
#include "MaterialHLSLGenerator.h"
#endif

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 0
static UE::Shader::FType GetHLSLOutputType(const FMaterialHLSLGenerator& Generator, EHLSLOutputType Type)
{
	using namespace UE::Shader;
    
	switch (Type)
	{
	case EHLSLOutputType::HOPT_Float: return EValueType::Float1;
	case EHLSLOutputType::HOPT_Float2: return EValueType::Float2;
	case EHLSLOutputType::HOPT_Float3: return EValueType::Float3;
	case EHLSLOutputType::HOPT_Float4: return EValueType::Float4;
	case EHLSLOutputType::HOPT_MaterialAttributes: return Generator.GetMaterialAttributesType();
	default: checkNoEntry(); return EValueType::Void;
	}
}
#endif

static FString HLSL_CUSTOM_MAIN_NAME = TEXT("Main");
static FString HLSL_CUSTOM_FUNC_NAME = TEXT("Funcs");


FName MakeHLSLCustomExpressionName(class UObject* Outer)
{
    return MakeUniqueObjectName(Outer, UMaterialExpressionCustom::StaticClass(), FName("HLSLCustomExpression"));
}

UMaterialExpressionCustom* GenerateCustomExpression(class UObject* Outer,const FString& Name)
{
    UMaterialExpressionCustom* Expression = NewObject<UMaterialExpressionCustom>(Outer, MakeHLSLCustomExpressionName(Outer), RF_Transient);
    Expression->Code = TEXT("");

#if WITH_EDITOR
    Expression->MaterialExpressionGuid = FGuid::NewGuid();
    Expression->bCollapsed = true;
    Expression->OutputType = CMOT_Float1;
    Expression->bShowInputs = false;
    Expression->bShowOutputs = false;
    Expression->bShowOutputNameOnPin = false;
    Expression->MaterialExpressionEditorX = 0;
    Expression->MaterialExpressionEditorY = 0;
    Expression->Inputs.Reset();
    Expression->Outputs.Reset();
#endif

    return Expression;
}

ECustomMaterialOutputType ConvertValueType2OutputType(EMaterialValueType Type)
{
    ECustomMaterialOutputType Output;
    switch(Type)
    {
        case MCT_Float:
        case MCT_Float1:
            Output = CMOT_Float1;
            break;
        case MCT_Float2:
            Output = CMOT_Float2;
            break;
        case MCT_Float3:
            Output = CMOT_Float3;
            break;
        case MCT_Float4:
            Output = CMOT_Float4;
            break;
        case MCT_MaterialAttributes:
            Output = CMOT_MaterialAttributes;
            break;
        case MCT_Texture2D:
        case MCT_TextureCube:
        case MCT_Texture2DArray:
        case MCT_VolumeTexture:
        case MCT_StaticBool:
        case MCT_Unknown:
        case MCT_TextureExternal:
        case MCT_TextureVirtual:
        case MCT_Texture:
        case MCT_VTPageTableResult:
        case MCT_ShadingModel:
        default:
            Output = CMOT_MAX;
            break;
    }
    return Output;
}

#define LOCTEXT_NAMESPACE "MaterialExpression"
UHLSLMaterialExpression::UHLSLMaterialExpression(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
    // Structure to hold one-time initialization
    struct FConstructorStatics
    {
        FText NAME_Custom;
        FConstructorStatics() : NAME_Custom(LOCTEXT("HLSL Expression", "HLSL Expression")) {}
    };
    static FConstructorStatics ConstructorStatics;
#endif // WITH_EDITORONLY_DATA

#if WITH_EDITORONLY_DATA
    MenuCategories.Add(ConstructorStatics.NAME_Custom);
#endif

#if WITH_EDITORONLY_DATA
    bHidePreviewWindow = true;
    bCollapsed = false;
#endif // WITH_EDITORONLY_DATA

    HLSLMain = nullptr;
    HLSLFunctions = nullptr;
    ShaderFrequency = EHLSLShaderFrequency::HSF_Pixel;
    Expression = FHLSLExpressions();
}

bool UHLSLMaterialExpression::FixupExpressionArguments(bool bRebuild)
{
    TArray<FHLSLOutput>ConditionOutputs = GetConditionOutputs();
    bool bArgumentsDirty = bRebuild;

    if (!bArgumentsDirty)
    {
        if (ShaderFrequency == EHLSLShaderFrequency::HSF_Pixel)
            bArgumentsDirty |= (Expression.Main.Parameters.Num() != (HLSLInputs.Num() + ConditionOutputs.Num()));
        else
            bArgumentsDirty |= (Expression.Main.ReturnType != GetHLSLOutputTypeName(EHLSLOutputType::HOPT_Float3));
    }


    int32 ParameterOffset = 0;
    if(!bArgumentsDirty)
    {
        for(int32 Index = 0; Index<HLSLInputs.Num(); Index++)
        {
            const FHLSLInput& Input = HLSLInputs[Index];
            TArray<FString> Codes = GetHLSLInputCode(Input.InputType, Input.InputName.ToString());
            for(FString&InputCode:Codes)
            {
                if(!Expression.Main.Parameters.IsValidIndex(ParameterOffset))
				{
					bArgumentsDirty = true;
					break;
				}

                const FHLSLParameter& Parameter = Expression.Main.Parameters[ParameterOffset];

                if(Parameter.bOutput)
                {
                    bArgumentsDirty = true;
                    break;
                }
                else if (InputCode.StartsWith(HLSL_SAMPLER_TYPE_IDENTIFIER) && Parameter.Type.StartsWith(TEXT("Texture")))
                {
                    if(InputCode != FString::Printf(TEXT("%s %s"), HLSL_SAMPLER_TYPE_IDENTIFIER, *(Parameter.Name+HLSL_SAMPLER_NAME_IDENTIFIER)))
					{
						bArgumentsDirty = true;
						break;
					}
                }
                else if (InputCode != GetHLSLParameterCode(Parameter.Type, Parameter.Name))
                {
					bArgumentsDirty = true;
					break;
                }
            }
            ++ParameterOffset;
        }
    }

    if(!bArgumentsDirty)
    {
        if (ShaderFrequency == EHLSLShaderFrequency::HSF_Pixel)
        {
			for (int32 Index = 0; Index < ConditionOutputs.Num(); Index++)
			{
				if (!Expression.Main.Parameters.IsValidIndex(ParameterOffset))
				{
					bArgumentsDirty = true;
					break;
				}

				const FHLSLOutput& Output = ConditionOutputs[Index];
				const FHLSLParameter& Parameter = Expression.Main.Parameters[ParameterOffset];

				FString OutputCode = GetHLSLOutputCode(Output.OutputType, Output.OutputName.ToString());
				if (!Parameter.bOutput || OutputCode != GetHLSLParameterCode(Parameter.Type, Parameter.Name))
				{
					bArgumentsDirty = true;
					break;
				}
				++ParameterOffset;
			}
        }
        else
        {
            for (FHLSLParameter& Parameter : Expression.Main.Parameters)
            {
                if (Parameter.bOutput)
                {
					bArgumentsDirty = true;
					break;
                }
            }
        }
    }

    if(bArgumentsDirty)
    {
        FString ArgumentsCode;

        for(const FHLSLInput& Input:HLSLInputs)
        {
            TArray<FString> Codes = GetHLSLInputCode(Input.InputType, Input.InputName.ToString());
            for(FString&InputCode:Codes)
            {
                if(!ArgumentsCode.IsEmpty())
                    ArgumentsCode.Append(TEXT(", "));
                ArgumentsCode.Append(InputCode);
            }
        }

        if (ShaderFrequency == EHLSLShaderFrequency::HSF_Pixel)
        {
			for (const FHLSLOutput& Output : ConditionOutputs)
			{
				FString OutputCode = GetHLSLOutputCode(Output.OutputType, Output.OutputName.ToString());
				if (!ArgumentsCode.IsEmpty())
					ArgumentsCode.Append(TEXT(", "));
				ArgumentsCode.Append(TEXT("out ") + OutputCode);
			}
        }

        if(!FHLSLParser::ReplaceFunctionArguments(HLSLCode.Code, *GetConditionOutputTypeName(), HLSL_MAIN_ENTRY_IDENTIFIER, ArgumentsCode))
        {
            HLSLCode.Code.Append
            (
                FString::Printf(TEXT("\r\n\r\n%s %s(%s)\r\n{\r\n%s\r\n}\r\n"), 
                *GetConditionOutputTypeName(), HLSL_MAIN_ENTRY_IDENTIFIER, *ArgumentsCode,*GetConditionDefaultOutputCode())
            );
        }
    }

    return bArgumentsDirty;
}

void UHLSLMaterialExpression::SetExpressions(const FHLSLExpressions& NewExpression)
{
    Expression = NewExpression;
    UpdateExpression();
}

void UHLSLMaterialExpression::UpdateExpression()
{
    CreateExpressionIfNone(false);

    GenerateHLSLExpressions();
}

FString UHLSLMaterialExpression::GetConditionDefaultOutputName()
{
    return ShaderFrequency == EHLSLShaderFrequency::HSF_Pixel ? HLSL_OUTPUT_PIXEL_NAME : HLSL_OUTPUT_VERTEX_NAME;
}

FString UHLSLMaterialExpression::GetConditionDefaultOutputCode()
{
    return ShaderFrequency == EHLSLShaderFrequency::HSF_Pixel ? TEXT("    //Enter Code...") : HLSL_VERTEX_DEFAULT_CODE;
}

FString UHLSLMaterialExpression::GetConditionOutputTypeName()
{
    return ShaderFrequency == EHLSLShaderFrequency::HSF_Pixel ? TEXT("void") : TEXT("float3");
}

EHLSLOutputType UHLSLMaterialExpression::GetConditionHLSLOutputType()
{
    return ShaderFrequency == EHLSLShaderFrequency::HSF_Pixel ? EHLSLOutputType::HOPT_Float : EHLSLOutputType::HOPT_Float3;
}

ECustomMaterialOutputType UHLSLMaterialExpression::GetConditionOutputType()
{
    return ShaderFrequency == EHLSLShaderFrequency::HSF_Pixel ? ECustomMaterialOutputType::CMOT_Float1 : ECustomMaterialOutputType::CMOT_Float3;
}

TArray<FHLSLOutput> UHLSLMaterialExpression::GetConditionOutputs()
{
    return (ShaderFrequency == EHLSLShaderFrequency::HSF_Pixel) ? HLSLOutputs : TArray<FHLSLOutput>{FHLSLOutput(*GetConditionOutputTypeName(),GetConditionHLSLOutputType())};
}


void UHLSLMaterialExpression::CreateExpressionIfNone(bool bForceCreate)
{
    if(!HLSLMain || bForceCreate)
    {
        HLSLMain = GenerateCustomExpression(this, HLSL_CUSTOM_MAIN_NAME);
        HLSLMain->Code = HLSL_VERTEX_DEFAULT_CODE;
    }

    if(!HLSLFunctions || bForceCreate)
    {
        HLSLFunctions = GenerateCustomExpression(this, HLSL_CUSTOM_FUNC_NAME);
        HLSLFunctions->Code = HLSL_PIXEL_DEFAULT_CODE;
    } 
}

void UHLSLMaterialExpression::ResetDefault()
{
    CreateExpressionIfNone(true);
    HLSLFunctions->Inputs.Reset();
    HLSLFunctions->Outputs.Reset();
    HLSLFunctions->AdditionalOutputs.Reset();
    HLSLFunctions->Code = HLSL_PIXEL_DEFAULT_CODE;

    HLSLMain->Inputs.Reset();
    HLSLMain->Outputs.Reset();
    HLSLMain->AdditionalOutputs.Reset();
    HLSLMain->Code = GetConditionDefaultOutputCode();
    HLSLMain->Outputs.Add(FExpressionOutput(*(GetConditionDefaultOutputName())));

    this->HLSLInputs.Reset();
    this->HLSLOutputs.Reset();
    this->Outputs = HLSLMain->Outputs;

#if WITH_EDITOR
    if(GraphNode)
    {
        GraphNode->ReconstructNode();
    }
#endif
}

void UHLSLMaterialExpression::RebuildOutputs()
{
    if(!HLSLMain)
        return;

    auto AddOutput = [this] (const FName& Name, EMaterialValueType Type)
    {
        FCustomOutput& Output = HLSLMain->AdditionalOutputs.Emplace_GetRef();
        Output.OutputName = Name;
        Output.OutputType = ConvertValueType2OutputType(Type);
        HLSLMain->Outputs.Add(FExpressionOutput(Name));
    };

	this->bShowInputs = true;
	this->bShowOutputs = true;
	this->bShowOutputNameOnPin = true;


    TArray<FHLSLOutput>ConditionOutputs = GetConditionOutputs();
    HLSLMain->Outputs.Reset(ConditionOutputs.Num()+1);
    HLSLMain->AdditionalOutputs.Reset(ConditionOutputs.Num()+1);

    HLSLMain->Outputs.Add(FExpressionOutput(*GetConditionDefaultOutputName()));
    if (ShaderFrequency == EHLSLShaderFrequency::HSF_Pixel)
    {
		for (const FHLSLOutput& Output : ConditionOutputs)
		{
			AddOutput(Output.OutputName, GetOutputValueType(Output.OutputType));
		}
    }

    this->Outputs = HLSLMain->Outputs;
}

void UHLSLMaterialExpression::GenerateHLSLExpressions()
{
    //Main
    {
        HLSLMain->Code.Empty();
        HLSLMain->OutputType = GetConditionOutputType();
		HLSLMain->Description = TEXT("");
        if(!Expression.Main.Body.IsEmpty())
        {
            FString ExpressionBody = Expression.Main.Body;
            if(!ExpressionBody.IsEmpty())
            {
                if(ExpressionBody[0]=='{')
                {
                    ExpressionBody.RemoveAt(0);
                    if(ExpressionBody[ExpressionBody.Len()-1]=='}')
                        ExpressionBody.RemoveAt(ExpressionBody.Len()-1);
                }
                HLSLMain->Code = ExpressionBody;
            }
        }
        if (ShaderFrequency == EHLSLShaderFrequency::HSF_Pixel)
            HLSLMain->Code += HLSL_PIXEL_DEFAULT_CODE;
    }


    //defines
    {
        HLSLFunctions->Code.Empty();
        HLSLFunctions->OutputType = ECustomMaterialOutputType::CMOT_Float1;
        HLSLFunctions->Description = TEXT("");

        //if(!Expression.Functions.IsEmpty())
        {
            HLSLFunctions->Code = FString::Printf(TEXT("//%s_Entry_Start\r\nreturn 1;\r\n}\r\n\r\n%s\r\nvoid %s_Entry_End()\r\n{"), *(this->GetName()), *Expression.Functions, *(this->GetName()));
        }
    }
}


void UHLSLMaterialExpression::Serialize(FStructuredArchive::FRecord Record)
{
    Super::Serialize(Record);
    FArchive& UnderlyingArchive = Record.GetUnderlyingArchive();
    UnderlyingArchive.UsingCustomVersion(FRenderingObjectVersion::GUID);
}

void UHLSLMaterialExpression::PostInitProperties()
{
    Super::PostInitProperties();

    HLSLCode.Code = FString::Printf(TEXT("void %s()\r\n{\r\n%s\r\n}\r\n"), HLSL_MAIN_ENTRY_IDENTIFIER, *GetConditionDefaultOutputCode());
    HLSLCode.Geometry = FVector4(0, 0, 0, 0);
    HLSLCode.bAutoCompile = false;

	this->bShowInputs = true;
	this->bShowOutputs = true;
	this->bShowOutputNameOnPin = true;

    ResetDefault();
}

void UHLSLMaterialExpression::PostLoad()
{
    Super::PostLoad();
	this->bShowInputs = true;
	this->bShowOutputs = true;
	this->bShowOutputNameOnPin = true;
    UpdateExpression();
    RebuildOutputs();
}

void UHLSLMaterialExpression::PostEditImport()
{
    Super::PostEditImport();
// 	HLSLMain->Rename(*(MakeHLSLCustomExpressionName(this).ToString()), this, REN_DoNotDirty | REN_DontCreateRedirectors | REN_ForceNoResetLoaders);
// 	HLSLFunctions->Rename(*(MakeHLSLCustomExpressionName(this).ToString()), this, REN_DoNotDirty | REN_DontCreateRedirectors | REN_ForceNoResetLoaders);
     RebuildOutputs();
}
 
bool UHLSLMaterialExpression::CanEditChange(const FProperty* InProperty) const
{
	if (InProperty != nullptr && InProperty->HasMetaData(TEXT("EditConditionHides")))
	{
		return ShaderFrequency != EHLSLShaderFrequency::HSF_Vertex;
	}

	return Super::CanEditChange(InProperty);
}

void UHLSLMaterialExpression::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    FProperty* PropertyThatChanged = PropertyChangedEvent.Property;
    if(PropertyThatChanged && PropertyThatChanged->GetFName()==GET_MEMBER_NAME_CHECKED(FHLSLInput, InputName))
    {
        for(FHLSLInput& Input:HLSLInputs)
        {
            FString InputName = Input.InputName.ToString();
            if(InputName.ReplaceInline(TEXT(" "), TEXT(""))>0)
            {
                Input.InputName = *InputName;
            }
        }
    }
    else if(PropertyThatChanged && PropertyThatChanged->GetFName()==GET_MEMBER_NAME_CHECKED(FHLSLOutput, OutputName))
    {
        for(FHLSLOutput& Output:HLSLOutputs)
        {
            FString OutputName = Output.OutputName.ToString();
            if(OutputName.ReplaceInline(TEXT(" "), TEXT(""))>0)
            {
                Output.OutputName = *OutputName;
            }
        }
    }

    if(PropertyChangedEvent.MemberProperty)
    {
		bool bArgumentsDirty = false;

		bool bPinsDirty = false;

        const FName PropertyName = PropertyChangedEvent.MemberProperty->GetFName();

		if (PropertyThatChanged && PropertyThatChanged->GetFName() == GET_MEMBER_NAME_CHECKED(UHLSLMaterialExpression, ShaderFrequency))
		{
			RebuildOutputs();
			GenerateHLSLExpressions();
			bArgumentsDirty = true;
			bPinsDirty = true;
		}
		else if (PropertyName == GET_MEMBER_NAME_CHECKED(UHLSLMaterialExpression, HLSLInputs))
        {
            bArgumentsDirty = true;
            bPinsDirty = true;
        }
        else if(PropertyName==GET_MEMBER_NAME_CHECKED(UHLSLMaterialExpression, HLSLOutputs))
        {
            RebuildOutputs();
            bArgumentsDirty = true;
            bPinsDirty = true;
        }
        else if(PropertyName==GET_MEMBER_NAME_CHECKED(UHLSLMaterialExpression, ShaderFrequency))
        {
            bArgumentsDirty = true;
        }

        if(bArgumentsDirty)
        {
            FixupExpressionArguments(true);
        }

        if (bPinsDirty && GraphNode)
        {
            bool bNeedUpdate = false;
            for (UEdGraphPin* Pin : GraphNode->Pins)
            {
                if(Pin->PinName == HLSL_INPUT_FUNCTIONS_NAME)
                    continue;

                bool bFind = false;

                for (const FHLSLInput& Input : HLSLInputs)
                {
                    if (Pin->PinName == Input.InputName)
					{
						bFind = true;
						break;
					}
                }

                if(ShaderFrequency == EHLSLShaderFrequency::HSF_Pixel)
				{
					if (!bFind)
					{
						for (const FHLSLOutput& Output : HLSLOutputs)
						{
							if (Pin->PinName == Output.OutputName)
							{
								bFind = true;
								break;
							}
						}
					}
				}

                if (!bFind && Pin->LinkedTo.Num() > 0)
                {
                    Pin->BreakAllPinLinks(true);
                    bNeedUpdate = true;
                }
            }

            GraphNode->ReconstructNode();

            if(bNeedUpdate)
                FMaterialEditorUtilities::UpdateMaterialAfterGraphChange(GraphNode->GetGraph());
        }
        else
        {
            UpdateGraphCodeCallback.ExecuteIfBound();
        }
    }
     
    Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UHLSLMaterialExpression::PreCompile()
{
    {
        HLSLFunctions->Inputs.Reset();
        HLSLFunctions->Outputs.Reset(2);
        HLSLFunctions->AdditionalOutputs.Reset();

        HLSLMain->Inputs.Reset(HLSLInputs.Num()+1);
    }

    {
        FCustomInput& Entry = HLSLMain->Inputs.Emplace_GetRef();
        Entry.InputName = HLSL_INPUT_FUNCTIONS_NAME;

        HLSLFunctions->OutputType = ECustomMaterialOutputType::CMOT_Float1;
        HLSLFunctions->Outputs.Add(FExpressionOutput(Entry.InputName));

        Entry.Input.Connect(0, HLSLFunctions);
    }

    {
        for(int32 Index = 0; Index<HLSLInputs.Num(); Index++)
        {
            FHLSLInput& ExpressionInput = HLSLInputs[Index];
            FCustomInput& CompileInput = HLSLMain->Inputs.Emplace_GetRef();
            CompileInput.InputName = ExpressionInput.InputName;
			CompileInput.Input.Expression = nullptr;
			CompileInput.Input.OutputIndex = 0;

            if (ExpressionInput.Input.IsConnected())
			{
               CompileInput.Input.Connect(ExpressionInput.Input.OutputIndex, ExpressionInput.Input.Expression);
			}
        }
    }
}

int32 UHLSLMaterialExpression::Compile(class FMaterialCompiler* Compiler, int32 OutputIndex)
{
    PreCompile();

    auto HLSLCompile = [Compiler,OutputIndex, Description=this->Description] (UMaterialExpressionCustom* Expression_)->int32
    {
        TArray<int32> CompiledInputs;

        for(int32 i = 0; i<Expression_->Inputs.Num(); i++)
        {
            if(Expression_->Inputs[i].InputName.IsNone())
            {
                CompiledInputs.Add(INDEX_NONE);
            }
            else
            {
                if(!Expression_->Inputs[i].Input.GetTracedInput().Expression)
                {
                    return Compiler->Errorf(TEXT("%s missing input %d (%s)"), *Description, i, *Expression_->Inputs[i].InputName.ToString());
                }
                int32 InputCode = Expression_->Inputs[i].Input.Compile(Compiler);
                if(InputCode<0)
                {
                    return InputCode;
                }
                CompiledInputs.Add(InputCode);
            }
        }

        return Compiler->CustomExpression(Expression_, OutputIndex, CompiledInputs);
    };


    //AllocateSlot
    {
        FRegexPattern RegexPattern(TEXT("\\bParameters.TexCoords\\b\\s*\\[\\s*([0-9]*)\\s*\\]"));
        FRegexMatcher Matcher(RegexPattern, GetCode());
        while(Matcher.FindNext())
        {
            int32 Index = FCString::Atoi(*(Matcher.GetCaptureGroup(1)));
            Compiler->TextureCoordinate(Index, false, false);
        }
    }

    if(HLSLFunctions->Outputs.IsValidIndex(OutputIndex))
        HLSLCompile(HLSLFunctions);

    if(HLSLMain->Outputs.IsValidIndex(OutputIndex))
        return HLSLCompile(HLSLMain);
    else
        return Compiler->Errorf(TEXT("%s missing input %d"), *Description, OutputIndex);
}

void UHLSLMaterialExpression::GetCaption(TArray<FString>& OutCaptions) const
{
    if(!Description.IsEmpty())
        OutCaptions.Add(Description);
    else
        OutCaptions.Add(GetName());
}

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 2
TArrayView<FExpressionInput*> UHLSLMaterialExpression::GetInputsView()
{
    if (CachedInputs.Num() != HLSLInputs.Num())
        CachedInputs.SetNumZeroed(HLSLInputs.Num());
    for (int32 i = 0; i < HLSLInputs.Num(); i++)
    {
        CachedInputs[i] = &HLSLInputs[i].Input;
    }
    return CachedInputs;
}
#else
const TArray<FExpressionInput*> UHLSLMaterialExpression::GetInputs()
{
    TArray<FExpressionInput*> Result;
    for(int32 i = 0; i<HLSLInputs.Num(); i++)
    {
        Result.Add(&HLSLInputs[i].Input);
    }
    return Result;
}
#endif

FExpressionInput* UHLSLMaterialExpression::GetInput(int32 InputIndex)
{
    if(InputIndex<HLSLInputs.Num())
    {
        return &HLSLInputs[InputIndex].Input;
    }
    return nullptr;
}

FName UHLSLMaterialExpression::GetInputName(int32 InputIndex) const
{
    if(InputIndex<HLSLInputs.Num())
    {
        return HLSLInputs[InputIndex].InputName;
    }
    return NAME_None;
}

uint32 UHLSLMaterialExpression::GetInputType(int32 InputIndex)
{
    if(InputIndex<HLSLInputs.Num())
    {
        //EMaterialValueType Type = FHLSLParser::GetParameterType(Expression.Main.Parameters[InputIndex].Type);
        EMaterialValueType Type = GetInputValueType(HLSLInputs[InputIndex].InputType);
        switch(Type)
        {
            case MCT_Texture2D:
                Type = MCT_Texture;
                break;
            case MCT_TextureCube:
                Type = MCT_Texture;
                break;
            case MCT_Texture2DArray:
                Type = MCT_Texture;
                break;
            case MCT_VolumeTexture:
                Type = MCT_Texture;
                break;
            case MCT_TextureExternal:
                Type = MCT_Texture;
                break;
        }
        return Type;
    }
    return MCT_Unknown;
}

TArray<FExpressionOutput>& UHLSLMaterialExpression::GetOutputs()
{
    return HLSLMain->Outputs;
}

uint32 UHLSLMaterialExpression::GetOutputType(int32 OutputIndex)
{
    return HLSLMain->GetOutputType(OutputIndex);
}

bool UHLSLMaterialExpression::IsResultMaterialAttributes(int32 OutputIndex)
{
    if(Outputs.IsValidIndex(OutputIndex))
        return GetOutputType(OutputIndex)==MCT_MaterialAttributes;

    return false;
}

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 0
bool UHLSLMaterialExpression::GenerateHLSLExpression(FMaterialHLSLGenerator& Generator, UE::HLSLTree::FScope& Scope, int32 OutputIndex, UE::HLSLTree::FExpression const*& OutExpression) const
{
    UHLSLMaterialExpression* ConstSelf = const_cast<UHLSLMaterialExpression*>(this);
    ConstSelf->PreCompile();

	return HLSLMain->GenerateHLSLExpression(Generator,Scope,OutputIndex, OutExpression);
}
#endif

#undef LOCTEXT_NAMESPACE