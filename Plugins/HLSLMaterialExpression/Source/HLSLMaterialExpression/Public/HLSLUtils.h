// Copyright 2023 ZSttt, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HLSLUtils.generated.h"


//Log
DECLARE_LOG_CATEGORY_EXTERN(LogHLSLExpression, Log, All);

//Identifier
#define HLSL_INPUT_FUNCTIONS_NAME TEXT("HLSL_Entry")
#define HLSL_OUTPUT_PIXEL_NAME TEXT("Reference")
#define HLSL_OUTPUT_VERTEX_NAME TEXT("Offset")
#define HLSL_PIXEL_DEFAULT_CODE FString::Printf(TEXT("    float %s = 1.0;\r\n    return %s;"),HLSL_OUTPUT_PIXEL_NAME,HLSL_OUTPUT_PIXEL_NAME)
#define HLSL_VERTEX_DEFAULT_CODE FString::Printf(TEXT("    return float3(0,0,0);"),HLSL_OUTPUT_VERTEX_NAME,HLSL_OUTPUT_VERTEX_NAME)
#define HLSL_SAMPLER_TYPE_IDENTIFIER TEXT("SamplerState")
#define HLSL_SAMPLER_NAME_IDENTIFIER TEXT("Sampler")
#define HLSL_MAIN_TYPE_IDENTIFIER TEXT("void")
#define HLSL_MAIN_ENTRY_IDENTIFIER TEXT("Main")
#define HLSL_EXPRESSION_NODE_DEFAULT_SIZE FIntPoint(500,300)


USTRUCT()
struct FHLSLCode
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY()
        FString Code;

    UPROPERTY()
        FVector4 Geometry = FVector4(0, 0, 0, 0);//x,y,width,height

    UPROPERTY()
        bool bAutoCompile = false;
};

USTRUCT()
struct FHLSLParameter
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY()
        FString Type;

    UPROPERTY()
        FString Name;

    UPROPERTY()
        bool bOutput = false;

    UPROPERTY()
        FString Code;
};

USTRUCT()
struct FHLSLFunction
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY()
        FString ReturnType = TEXT("");

    UPROPERTY()
        FString Name = TEXT("");

    UPROPERTY()
        TArray<FHLSLParameter> Parameters = {};

    UPROPERTY()
        FString Body = TEXT("");

    UPROPERTY()
        bool bMainExpression = false;

    UPROPERTY()
        FString Code;
};


USTRUCT()
struct FHLSLContext
{
    GENERATED_USTRUCT_BODY()

        FHLSLContext() {}

    UPROPERTY()
        TArray<FString> Comments;
    UPROPERTY()
        TArray<FString> Defines;
    UPROPERTY()
        TArray<FString> Includes;
    UPROPERTY()
        TArray<FString> Structs;
    UPROPERTY()
        TArray<FString> Variables;

    UPROPERTY()
        FHLSLFunction Main;
    UPROPERTY()
        TArray<FHLSLFunction> Functions;
};

USTRUCT()
struct FHLSLExpressions
{
    GENERATED_USTRUCT_BODY()

    FHLSLExpressions() {}

    UPROPERTY()
        FHLSLFunction Main;

    UPROPERTY()
        FString Functions;
};

UENUM(BlueprintType)
enum class EHLSLShaderFrequency : uint8
{
    HSF_Pixel = 0,
    HSF_Vertex,
};

UENUM(BlueprintType)
enum class EHLSLInputType : uint8
{
    HIPT_Float = 0 UMETA(DisplayName = "Float"),
    HIPT_Float2 UMETA(DisplayName = "Float2"),
    HIPT_Float3 UMETA(DisplayName = "Float3"),
    HIPT_Float4 UMETA(DisplayName = "Float4"),
    HIPT_Texture2D UMETA(DisplayName = "Texture2D"),
    HIPT_Texture2DArray UMETA(DisplayName = "Texture2DArray"),
    HIPT_TextureExternal UMETA(DisplayName = "TextureExternal"),
    HIPT_TextureCube UMETA(DisplayName = "TextureCube"),
    HIPT_VolumeTexture UMETA(DisplayName = "VolumeTexture"),
    HIPT_MaterialAttributes UMETA(DisplayName = "MaterialAttributes"),
};

UENUM(BlueprintType)
enum class EHLSLOutputType : uint8
{
    HOPT_Float = 0 UMETA(DisplayName = "Float"),
    HOPT_Float2 UMETA(DisplayName = "Float2"),
    HOPT_Float3 UMETA(DisplayName = "Float3"),
    HOPT_Float4 UMETA(DisplayName = "Float4"),
    HOPT_MaterialAttributes UMETA(DisplayName = "MaterialAttributes")
};

static FString GetHLSLOutputTypeName(EHLSLOutputType Type)
{
	switch (Type)
	{
	case EHLSLOutputType::HOPT_Float:
		return TEXT("float");
	case EHLSLOutputType::HOPT_Float2:
        return TEXT("float2");
	case EHLSLOutputType::HOPT_Float3:
        return TEXT("float3");
	case EHLSLOutputType::HOPT_Float4:
        return TEXT("float4");
	case EHLSLOutputType::HOPT_MaterialAttributes:
        return TEXT("FMaterialAttributes");
	}
    return TEXT("float");
}

static FString GetHLSLParameterCode(const FString& Type, const FString& Name)
{
    return FString::Printf(TEXT("%s %s"), *Type, *Name);
}

static TArray<FString> GetHLSLInputCode(EHLSLInputType Type, const FString& Name)
{
    TArray<FString> Parameters;
    switch(Type)
    {
        case EHLSLInputType::HIPT_Float:
            Parameters.Add(GetHLSLParameterCode(TEXT("float"),Name));
            break;
        case EHLSLInputType::HIPT_Float2:
            Parameters.Add(GetHLSLParameterCode(TEXT("float2"),Name));
            break;
        case EHLSLInputType::HIPT_Float3:
            Parameters.Add(GetHLSLParameterCode(TEXT("float3"),Name));
            break;
        case EHLSLInputType::HIPT_Float4:
            Parameters.Add(GetHLSLParameterCode(TEXT("float4"),Name));
            break;
        case EHLSLInputType::HIPT_Texture2D:
            Parameters.Add(GetHLSLParameterCode(TEXT("Texture2D"), Name));
            Parameters.Add(GetHLSLParameterCode(TEXT("SamplerState"), Name+TEXT("Sampler")));
            break;
        case EHLSLInputType::HIPT_Texture2DArray:
            Parameters.Add(GetHLSLParameterCode(TEXT("Texture2DArray"), Name));
            Parameters.Add(GetHLSLParameterCode(TEXT("SamplerState"), Name+TEXT("Sampler")));
            break;
        case EHLSLInputType::HIPT_TextureExternal:
            Parameters.Add(GetHLSLParameterCode(TEXT("TextureExternal"), Name));
            Parameters.Add(GetHLSLParameterCode(TEXT("SamplerState"), Name+TEXT("Sampler")));
            break;
        case EHLSLInputType::HIPT_TextureCube:
            Parameters.Add(GetHLSLParameterCode(TEXT("TextureCube"), Name));
            Parameters.Add(GetHLSLParameterCode(TEXT("SamplerState"), Name+TEXT("Sampler")));
            break;
        case EHLSLInputType::HIPT_VolumeTexture:
            Parameters.Add(GetHLSLParameterCode(TEXT("Texture3D"),Name));
            Parameters.Add(GetHLSLParameterCode(TEXT("SamplerState"),Name+TEXT("Sampler")));
            break;
        case EHLSLInputType::HIPT_MaterialAttributes:
            Parameters.Add(GetHLSLParameterCode(TEXT("FMaterialAttributes"),Name));
            break;
    }

    return Parameters;
}

static FString GetHLSLOutputCode(EHLSLOutputType Type, const FString& Name)
{
    FString Parameter;
    switch(Type)
    {
        case EHLSLOutputType::HOPT_Float:
            Parameter = GetHLSLParameterCode(TEXT("float"),Name);
            break;
        case EHLSLOutputType::HOPT_Float2:
            Parameter = GetHLSLParameterCode(TEXT("float2"),Name);
            break;
        case EHLSLOutputType::HOPT_Float3:
            Parameter = GetHLSLParameterCode(TEXT("float3"),Name);
            break;
        case EHLSLOutputType::HOPT_Float4:
            Parameter = GetHLSLParameterCode(TEXT("float4"),Name);
            break;
        case EHLSLOutputType::HOPT_MaterialAttributes:
            Parameter = GetHLSLParameterCode(TEXT("FMaterialAttributes"),Name);
            break;
    }

    return Parameter;
}

static EHLSLInputType Convert2InputType(EMaterialValueType ValueType)
{
    switch(ValueType)
    {
        case MCT_Float:
            return EHLSLInputType::HIPT_Float;
        case MCT_Float2:
            return EHLSLInputType::HIPT_Float2;
        case MCT_Float3:
            return EHLSLInputType::HIPT_Float3;
        case MCT_Float4:
            return EHLSLInputType::HIPT_Float4;
        case MCT_Texture2D:
            return EHLSLInputType::HIPT_Texture2D;
        case MCT_Texture2DArray:
            return EHLSLInputType::HIPT_Texture2DArray;
        case MCT_TextureExternal:
            return EHLSLInputType::HIPT_TextureExternal;
        case MCT_TextureCube:
            return EHLSLInputType::HIPT_TextureCube;
        case MCT_VolumeTexture:
            return EHLSLInputType::HIPT_VolumeTexture;
        case MCT_MaterialAttributes:
            return EHLSLInputType::HIPT_MaterialAttributes;
    }
    return EHLSLInputType::HIPT_Float;
}

static EHLSLOutputType Convert2OutputType(EMaterialValueType ValueType)
{
    switch(ValueType)
    {
        case MCT_Float:
            return EHLSLOutputType::HOPT_Float;
        case MCT_Float2:
            return EHLSLOutputType::HOPT_Float2;
        case MCT_Float3:
            return EHLSLOutputType::HOPT_Float3;
        case MCT_Float4:
            return EHLSLOutputType::HOPT_Float4;
        case MCT_MaterialAttributes:
            return EHLSLOutputType::HOPT_MaterialAttributes;
    }
    return EHLSLOutputType::HOPT_Float;
}

static EMaterialValueType GetInputValueType(EHLSLInputType InputType)
{
    switch(InputType)
    {
        case EHLSLInputType::HIPT_Float:
            return EMaterialValueType::MCT_Float;
        case EHLSLInputType::HIPT_Float2:
            return EMaterialValueType::MCT_Float2;
        case EHLSLInputType::HIPT_Float3:
            return EMaterialValueType::MCT_Float3;
        case EHLSLInputType::HIPT_Float4:
            return EMaterialValueType::MCT_Float4;
        case EHLSLInputType::HIPT_Texture2D:
            return EMaterialValueType::MCT_Texture2D;
        case EHLSLInputType::HIPT_Texture2DArray:
            return EMaterialValueType::MCT_Texture2DArray;
        case EHLSLInputType::HIPT_TextureExternal:
            return EMaterialValueType::MCT_TextureExternal;
        case EHLSLInputType::HIPT_TextureCube:
            return EMaterialValueType::MCT_TextureCube;
        case EHLSLInputType::HIPT_VolumeTexture:
            return EMaterialValueType::MCT_VolumeTexture;
        case EHLSLInputType::HIPT_MaterialAttributes:
            return EMaterialValueType::MCT_MaterialAttributes;
    };
    return EMaterialValueType::MCT_Float;
}

static EMaterialValueType GetOutputValueType(EHLSLOutputType OutputType)
{
    switch(OutputType)
    {
        case EHLSLOutputType::HOPT_Float:
            return EMaterialValueType::MCT_Float;
        case EHLSLOutputType::HOPT_Float2:
            return EMaterialValueType::MCT_Float2;
        case EHLSLOutputType::HOPT_Float3:
            return EMaterialValueType::MCT_Float3;
        case EHLSLOutputType::HOPT_Float4:
            return EMaterialValueType::MCT_Float4;
        case EHLSLOutputType::HOPT_MaterialAttributes:
            return EMaterialValueType::MCT_MaterialAttributes;
    };
    return EMaterialValueType::MCT_Float;
}

namespace HLSLUtils
{
    void HLSLCompileError(const FString& Message,TArray<FString> Details = TArray<FString>{});
}

namespace HLSLSyntax
{
    enum ETokenType
    {
        Unknow = 0,
        Operator,
        PreProcessor,
        Keyword,
        Struct,
        Define,
        Function,
        Token_Type_Max
    };

    enum EParseState
    {
        None = 0,
        LookingForString,
        LookingForSingleLineComment,
        LookingForMultiLineComment,
        LookingForSingleLineDefines,
        LookingForNumber,
        Parse_State_Max
    };

    struct FToken
    {
        FToken(const uint32 InType, const struct FTextRange& InRange) : Type(InType), Range(InRange) {}
        uint32 Type;
        struct FTextRange Range;
    };

    struct FTokenizedLine
    {
        struct FTextRange Range;
        TArray<FToken> Tokens;
    };

    struct FRule
    {
        FRule(FString InMatchText, int32 InType) :MatchText(MoveTemp(InMatchText)), Type(InType) {}
        FString MatchText;
        uint32 Type = 0;
        bool operator==(const FRule& R) const
        {
            return /*R.Type==Type&&*/R.MatchText.Equals(MatchText,ESearchCase::CaseSensitive);
        };
    };

    struct FRuleType
    {
        FRuleType() {};
        FRuleType(FString Name, const struct FTextBlockStyle& Style) :RuleName(MoveTemp(Name)), RuleStyle(Style) {}
        FString RuleName;
        struct FTextBlockStyle RuleStyle;
    };

    struct FSyntaxTokenizerRules
    {
        TArray<FRule> Tokens;
        TArray<FRuleType> Types;
        TArray<FRuleType>ParseTypes;
    };

    DECLARE_DELEGATE_RetVal(TArray<FRule>, FGetAdditionTokens);
};