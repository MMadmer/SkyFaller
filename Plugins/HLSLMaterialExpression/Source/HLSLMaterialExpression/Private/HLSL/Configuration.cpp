// Copyright 2023 ZSttt, Inc. All Rights Reserved.

#include "HLSL/Configuration.h"
#include "Policies/PrettyJsonPrintPolicy.h"
#include "Serialization/JsonTypes.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 2
#include "MaterialEditor/PreviewMaterial.h"
#else
#include "UnrealEd/Classes/MaterialEditor/PreviewMaterial.h"
#endif
#include "MaterialShared.h"

#include "HLSL/HLSLParser.h"
#include "HLSLMaterialExpression.h"

static TSharedPtr<HLSLConfig::FConfiguration> Configuration = nullptr;
TSharedPtr<HLSLConfig::FConfiguration> HLSLConfig::GetConfiguration()
{
    if(!Configuration)
        Configuration = MakeShareable(new FConfiguration);
    return Configuration;
}

FString HLSLConfig::GetStyleSetupFile()
{
    return FHLSLMaterialExpressionModule::HME_PluginPath/TEXT("Resources/HLSLStyles.txt");
}

FString HLSLConfig::GetTokensSetupFile()
{
    return FHLSLMaterialExpressionModule::HME_PluginPath / TEXT("Resources/HLSLTokens.txt");
}

void HLSLConfig::SaveStyles()
{
    if(!Configuration)
        return;

    TSharedPtr<FJsonObject> JsonObj(new FJsonObject);

    //
    JsonObj->Values.Add(TEXT("ShowColors"), MakeShareable(new FJsonValueBoolean(Configuration->bShowColors)));
    JsonObj->Values.Add(TEXT("ViewerSize"), MakeShareable(new FJsonValueArray(TArray<TSharedPtr<FJsonValue>>
    {
        MakeShareable(new FJsonValueNumber(Configuration->HLSLViewer.ViewerSize.X)),
        MakeShareable(new FJsonValueNumber(Configuration->HLSLViewer.ViewerSize.Y))
    })));
    JsonObj->Values.Add(TEXT("IsMaximize"), MakeShareable(new FJsonValueBoolean(Configuration->HLSLViewer.bMaximize)));

    //
    auto AddValue = [&](const FString& Key, const FColor&Color)
    {
        JsonObj->Values.Add(Key, MakeShareable(new FJsonValueString(Color.ToHex())));
    };

    AddValue(TEXT("NormalColor"), Configuration->NormalColor);
    AddValue(TEXT("NumberColor"), Configuration->NumberColor);
    AddValue(TEXT("CommentColor"), Configuration->CommentColor);
    AddValue(TEXT("PreProcessorsColor"), Configuration->PreProcessorsColor);
    AddValue(TEXT("OperatorsColor"), Configuration->OperatorsColor);
    AddValue(TEXT("KeywordsColor"), Configuration->KeywordsColor);
    AddValue(TEXT("StringsColor"), Configuration->StringsColor);
    AddValue(TEXT("FunctionsColor"), Configuration->FunctionsColor);
    AddValue(TEXT("StructsColor"), Configuration->StructsColor);
    AddValue(TEXT("DefinesColor"), Configuration->DefinesColor);

    SaveJson(JsonObj,GetStyleSetupFile());
}

void HLSLConfig::SaveTokens()
{
    if(!Configuration)
        return;

    TSharedPtr<FJsonObject> JsonObj(new FJsonObject);

    auto AddTokens = [&] (const FString& Key, const TArray<FString>&Tokens)
    {
        TArray<TSharedPtr<FJsonValue>> JTokens;
        JTokens.Reset(Tokens.Num()+1);
        for(const FString& Token:Tokens)
        {
            JTokens.Add(MakeShareable(new FJsonValueString(Token)));
        }
        JsonObj->Values.Add(Key, MakeShareable(new FJsonValueArray(JTokens)));
    };

    //
    AddTokens(TEXT("Operators"),Configuration->Operators);
    AddTokens(TEXT("Keywords"),Configuration->Keywords);
    AddTokens(TEXT("PreProcessors"),Configuration->PreProcessors);
    AddTokens(TEXT("Defines"),Configuration->Defines);
    AddTokens(TEXT("Functions"),Configuration->Functions);
    AddTokens(TEXT("Structs"),Configuration->Structs);

    SaveJson(JsonObj, GetTokensSetupFile());
}

void HLSLConfig::SaveJson(TSharedPtr<class FJsonObject> JsonObj, const FString File)
{
    typedef TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>> FStringWriter;
    typedef TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>> FStringWriterFactory;

    FString OutputString;
    TSharedRef<FStringWriter> Writer = FStringWriterFactory::Create(&OutputString);
    FJsonSerializer::Serialize(JsonObj.ToSharedRef(), Writer);
    FFileHelper::SaveStringToFile(OutputString, *File, FFileHelper::EEncodingOptions::ForceUTF8);
}

void HLSLConfig::LoadConfig()
{
    GetConfiguration();

    FString StyleFile = GetStyleSetupFile();
    if(!FPaths::FileExists(StyleFile))
    {
        FConfiguration Default = HLSLConfig::GetDefaultConfiguration();
        Configuration->bShowColors = Default.bShowColors;
        Configuration->HLSLViewer.ViewerSize = Default.HLSLViewer.ViewerSize;
        Configuration->HLSLViewer.bMaximize = Default.HLSLViewer.bMaximize;

        Configuration->NormalColor = Default.NormalColor;
        Configuration->NumberColor = Default.NumberColor;
        Configuration->CommentColor = Default.CommentColor;
        Configuration->PreProcessorsColor = Default.PreProcessorsColor;
        Configuration->OperatorsColor = Default.OperatorsColor;
        Configuration->KeywordsColor = Default.KeywordsColor;
        Configuration->StringsColor = Default.StringsColor;
        Configuration->FunctionsColor = Default.FunctionsColor;
        Configuration->StructsColor = Default.StructsColor;
        Configuration->DefinesColor = Default.DefinesColor;
        SaveStyles();
    }
    else
    {
        FString Json;
        if(FFileHelper::LoadFileToString(Json, *FPaths::ConvertRelativePathToFull(StyleFile)))
        {
            TSharedPtr<FJsonObject> JsonObj;
            TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Json);
            if(FJsonSerializer::Deserialize(JsonReader, JsonObj))
            {
                auto Assignment = [&] (const FString& Key, FColor& Color)
                {
                    if(TSharedPtr<FJsonValue>* pJson = JsonObj->Values.Find(Key))
                    {
                        FString JsonStr;
                        if((*pJson)->TryGetString(JsonStr))
                            Color = FColor::FromHex(JsonStr);
                    }
                };

                if(TSharedPtr<FJsonValue>* pJson = JsonObj->Values.Find(TEXT("ShowColors")))
                {
                    bool Boolean = true;
                    if((*pJson)->TryGetBool(Boolean))
                        Configuration->bShowColors = Boolean;
                }
                if(TSharedPtr<FJsonValue>* pJson = JsonObj->Values.Find(TEXT("ViewerSize")))
                {
                    const TArray<TSharedPtr<FJsonValue>>* JsonViewerSize = nullptr;
                    if((*pJson)->TryGetArray(JsonViewerSize))
                    {
                        if(JsonViewerSize->Num()==2)
                        {
                            Configuration->HLSLViewer.ViewerSize.X = (*JsonViewerSize)[0]->AsNumber();
                            Configuration->HLSLViewer.ViewerSize.Y = (*JsonViewerSize)[1]->AsNumber();
                        }
                    }
                }
                if(TSharedPtr<FJsonValue>* pJson = JsonObj->Values.Find(TEXT("IsMaximize")))
                {
                    bool Boolean = true;
                    if((*pJson)->TryGetBool(Boolean))
                        Configuration->HLSLViewer.bMaximize = Boolean;
                }

                Assignment(TEXT("NormalColor"),Configuration->NormalColor );
                Assignment(TEXT("NumberColor"),Configuration->NumberColor );
                Assignment(TEXT("CommentColor"),Configuration->CommentColor );
                Assignment(TEXT("PreProcessorsColor"),Configuration->PreProcessorsColor );
                Assignment(TEXT("OperatorsColor"),Configuration->OperatorsColor );
                Assignment(TEXT("KeywordsColor"),Configuration->KeywordsColor );
                Assignment(TEXT("StringsColor"),Configuration->StringsColor );
                Assignment(TEXT("FunctionsColor"),Configuration->FunctionsColor );
                Assignment(TEXT("StructsColor"),Configuration->StructsColor );
                Assignment(TEXT("DefinesColor"),Configuration->DefinesColor );
            }
        }
    }




    FString TokenFile = GetTokensSetupFile();
    if(!FPaths::FileExists(TokenFile))
    {
        const TArray<FString> Keywords =
        {
            TEXT("in"),
            TEXT("out"),
            TEXT("inout"),
            TEXT("break"),
            TEXT("continue"),
            TEXT("discard"),
            TEXT("do"),
            TEXT("for"),
            TEXT("if"),
            TEXT("else"),
            TEXT("else if"),
            TEXT("true"),
            TEXT("false"),
            TEXT("switch"),
            TEXT("case"),
            TEXT("return"),
            TEXT("default"),
            TEXT("forcecase"),
            TEXT("call"),
            TEXT("branch"),
            TEXT("unroll"),
            TEXT("loop"),
            TEXT("fastopt"),

            TEXT("uniform"),
            TEXT("volatile"),
            TEXT("static"),
            TEXT("extern"),
            TEXT("const"),
            TEXT("typedef"),
            TEXT("struct"),

            TEXT("void"),
            TEXT("bool2"),
            TEXT("bool3"),
            TEXT("bool4"),
            TEXT("bool"),
            TEXT("int2"),
            TEXT("int3"),
            TEXT("int4"),
            TEXT("int"),
            TEXT("uint2"),
            TEXT("uint3"),
            TEXT("uint4"),
            TEXT("uint"),
            TEXT("dword"),
            TEXT("half2"),
            TEXT("half3"),
            TEXT("half4"),
            TEXT("half"),
            TEXT("float2"),
            TEXT("float3"),
            TEXT("float4"),
            TEXT("float"),
            TEXT("double"),
            TEXT("vector"),
            TEXT("matrix"),
            TEXT("MaterialFloat4"),
            TEXT("MaterialFloat3"),
            TEXT("MaterialFloat2"),
            TEXT("MaterialFloat"),

            TEXT("half2x2"),
            TEXT("half2x3"),
            TEXT("half2x4"),

            TEXT("half3x2"),
            TEXT("half3x3"),
            TEXT("half3x4"),

            TEXT("half4x2"),
            TEXT("half4x3"),
            TEXT("half4x4"),

            TEXT("float2x2"),
            TEXT("float2x3"),
            TEXT("float2x4"),

            TEXT("float3x2"),
            TEXT("float3x3"),
            TEXT("float3x4"),

            TEXT("float4x2"),
            TEXT("float4x3"),
            TEXT("float4x4"),

            TEXT("MaterialFloat2x2"),
            TEXT("MaterialFloat2x3"),
            TEXT("MaterialFloat2x4"),

            TEXT("MaterialFloat3x2"),
            TEXT("MaterialFloat3x3"),
            TEXT("MaterialFloat3x4"),

            TEXT("MaterialFloat4x2"),
            TEXT("MaterialFloat4x3"),
            TEXT("MaterialFloat4x4"),
        };

        const TArray<FString>  Operators =
        {
            TEXT("//"),
            TEXT("/*"),
            TEXT("*/"),
            TEXT("\""),
            TEXT("\'"),
            TEXT("+="),
            TEXT("++"),
            TEXT("+"),
            TEXT("--"),
            TEXT("-="),
            TEXT("-"),
            TEXT("("),
            TEXT(")"),
            TEXT("["),
            TEXT("]"),
            TEXT("."),
            TEXT("!="),
            TEXT("!"),
            TEXT("&="),
            TEXT("~"),
            TEXT("&"),
            TEXT("*="),
            TEXT("*"),
            TEXT("/="),
            TEXT("/"),
            TEXT("%="),
            TEXT("%"),
            TEXT("<<="),
            TEXT("<<"),
            TEXT("<="),
            TEXT("<"),
            TEXT(">>="),
            TEXT(">>"),
            TEXT(">="),
            TEXT(">"),
            TEXT("=="),
            TEXT("&&"),
            TEXT("&"),
            TEXT("^="),
            TEXT("^"),
            TEXT("|="),
            TEXT("||"),
            TEXT("|"),
            TEXT("?"),
            TEXT("="),
            TEXT(","),
            TEXT("{"),
            TEXT("}"),
            TEXT(";"),
        };

        const TArray<FString>  PreProcessorKeywords =
        {
            TEXT("#include"),
            TEXT("#define"),
            TEXT("#ifndef"),
            TEXT("#ifdef"),
            TEXT("#if"),
            TEXT("#else"),
            TEXT("#endif"),
            TEXT("#undef"),
            TEXT("#elif"),
            TEXT("#error")
        };

        const TArray<FString> Functions = 
        {
            TEXT("abort"),
            TEXT("abs"),
            TEXT("acos"),
            TEXT("all"),
            TEXT("any"),
            TEXT("asdouble"),
            TEXT("asfloat"),
            TEXT("asin"),
            TEXT("asint"),
            TEXT("asuint"),
            TEXT("atan"),
            TEXT("atan2"),
            TEXT("ceil"),
            TEXT("clamp"),
            TEXT("clip"),
            TEXT("cos"),
            TEXT("cosh"),
            TEXT("countbits"),
            TEXT("ddx"),
            TEXT("ddx_coarse"),
            TEXT("ddx_fine"),
            TEXT("ddy"),
            TEXT("ddy_coarse"),
            TEXT("ddy_fine"),
            TEXT("degrees"),
            TEXT("determinant"),
            TEXT("distance"),
            TEXT("dot"),
            TEXT("dst"),
            TEXT("exp"),
            TEXT("exp2"),
            TEXT("f16tof32"),
            TEXT("f32tof16"),
            TEXT("faceforward"),
            TEXT("firstbithigh"),
            TEXT("firstbitlow"),
            TEXT("floor"),
            TEXT("fma"),
            TEXT("fma"),
            TEXT("fmod"),
            TEXT("frac"),
            TEXT("fwidth"),
            TEXT("isfinite"),
            TEXT("isinf"),
            TEXT("isnan"),
            TEXT("ldexp"),
            TEXT("length"),
            TEXT("lerp"),
            TEXT("lit"),
            TEXT("log"),
            TEXT("log10"),
            TEXT("log2"),
            TEXT("mad"),
            TEXT("max"),
            TEXT("min"),
            TEXT("modf"),
            TEXT("msad4"),
            TEXT("mul"),
            TEXT("noise"),
            TEXT("normalize"),
            TEXT("pow"),
            TEXT("radians"),
            TEXT("rcp"),
            TEXT("reflect"),
            TEXT("refract"),
            TEXT("reversebits"),
            TEXT("round"),
            TEXT("rsqrt"),
            TEXT("saturate"),
            TEXT("sign"),
            TEXT("sin"),
            TEXT("sincos"),
            TEXT("sinh"),
            TEXT("smoothstep"),
            TEXT("sqrt"),
            TEXT("step"),
            TEXT("tan"),
            TEXT("tanh"),
            TEXT("transpose"),
            TEXT("trunc"),

            TEXT("Main"),
        };

        Configuration->PreProcessors = PreProcessorKeywords;
        Configuration->Operators = Operators;
        Configuration->Keywords = Keywords;
        Configuration->Functions = Functions;

        {
            UPreviewMaterial* PreviewMaterial = NewObject<UPreviewMaterial>(GetTransientPackage(), NAME_None, RF_Public);
            PreviewMaterial->bIsPreviewMaterial = true;
            PreviewMaterial->PostLoad();

            FString MaterialTemplateCode;
            PreviewMaterial->GetMaterialResource(GMaxRHIFeatureLevel)->GetMaterialExpressionSource(MaterialTemplateCode);
            FHLSLParser::ParseTokens(MaterialTemplateCode,Configuration->Structs,Configuration->Defines,Configuration->Functions,true,true);
        }

        SaveTokens();
    }
    else
    {
        FString Json;
        if(FFileHelper::LoadFileToString(Json, *FPaths::ConvertRelativePathToFull(TokenFile)))
        {
            TSharedPtr<FJsonObject> JsonObj;
            TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Json);
            if(FJsonSerializer::Deserialize(JsonReader, JsonObj))
            {
                auto Assignment = [&] (const FString& Key, TArray<FString>& Tokens)
                {
                    if(TSharedPtr<FJsonValue>* pJson = JsonObj->Values.Find(Key))
                    {
                        const TArray<TSharedPtr<FJsonValue>>* Array = nullptr;
                        if((*pJson)->TryGetArray(Array))
                        {
                            Tokens.Reset(Array->Num()+1);
                            for(int32 Index = 0; Index<Array->Num(); Index++)
                            {
                                TSharedPtr<FJsonValue> Value = (*Array)[Index];
                                FString StrValue;
                                if(Value->TryGetString(StrValue))
                                    (*new(Tokens) FString()) = StrValue;
                            }
                        }
                    }
                };

                Assignment(TEXT("Operators"), Configuration->Operators);
                Assignment(TEXT("Keywords"), Configuration->Keywords);
                Assignment(TEXT("PreProcessors"), Configuration->PreProcessors);
                Assignment(TEXT("Defines"), Configuration->Defines);
                Assignment(TEXT("Functions"), Configuration->Functions);
                Assignment(TEXT("Structs"), Configuration->Structs);
            }
        }
    }
}

HLSLConfig::FConfiguration HLSLConfig::GetDefaultConfiguration()
{
    FConfiguration Config;
    Config.bShowColors = true;

    Config.HLSLViewer.ViewerSize = FVector2D(600, 500);
    Config.HLSLViewer.bMaximize = false;

    Config.NormalColor = FColor::FromHex(TEXT("FFFF80FF"));
    Config.NumberColor = FColor::FromHex(TEXT("B5CEA8FF"));
    Config.CommentColor = FColor::FromHex(TEXT("57A64AFF"));
    Config.PreProcessorsColor = FColor::FromHex(TEXT("9B9B9BFF"));
    Config.OperatorsColor = FColor::FromHex(TEXT("A4B1B1FF"));
    Config.KeywordsColor = FColor::FromHex(TEXT("569CD6FF"));
    Config.StringsColor = FColor::FromHex(TEXT("D69D85FF"));
    Config.FunctionsColor = FColor::FromHex(TEXT("FF8000FF"));
    Config.StructsColor = FColor::FromHex(TEXT("80FF80FF"));
    Config.DefinesColor = FColor::FromHex(TEXT("F200FCFF"));

    return Config;
}
