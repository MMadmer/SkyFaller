// Copyright 2023 ZSttt, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HLSLUtils.h"
#include "Internationalization/Regex.h"
#include "RHIDefinitions.h"


class FHLSLParser
{
public:
    static void ParseTokens(const FString& Code, TArray<FString>&Structs, TArray<FString>&Defines, TArray<FString>&Functions,
    bool MatchingSimilarDefines = true, bool MatchingInlineFunctions = true);

    static FString GetHLSLExpressionEntryRegexPattern();

    static int32 TrimStart(FString& Code);

    static int32 TrimEnd(FString& Code);

    static void TrimStartAndEnd(FString& Code);

protected:
    //Parameter

    static FHLSLParameter CreateParameter(const FString& Code, FString& Error);

    static void ParseParameter(const FString& Code, FString& Type, FString& Name);

    static int32 IsOutput(const FString& Type);

    static bool TreatAsOutput(FString& Type);
public:
    static EMaterialValueType GetParameterType(const FString& InType);

protected:
    static FString FixupAndMakeParameter(const EMaterialValueType InType, const FString& InName, const bool Output, FString& Errors);

    //Parse Code Context
protected:
    struct FFunction
    {
        int32 Line = 0;
        FString ReturnType = TEXT("");
        FString Name = TEXT("");
        TArray<FString> Arguments = {};
        FString Body = TEXT("");
    };

    enum EParseState
    {
        Context,
        Comment,
        Include,
        Define,
        Struct,
        FunctionName,
        Variable,
        Parameters,
        FunctionBody,
    };

    static TArray<FHLSLParameter> CaptureArguments(const FString& Code, FString& Error);

    static FString CaptureFunction(const FString& Code, FString& ReturnType, FString& Name, FString& Arguments, FString& Body, int32& Beginning, int32& Ending);

    static FString ParseCodeContext(const FString& Code,
        TArray<FString>& Comments,
        TArray<FString>& Defines,
        TArray<FString>& Includes,
        TArray<FString>& Structs,
        TArray<FString>& Variables,
        TArray<FFunction>&Functions);
public:
    static bool ReplaceFunctionArguments(FString&Code, const FString& Type, const FString& Name, const FString& Arguments);
protected:

    static FString GetFunctionCode(const FFunction& Function);

    static TArray<FHLSLParameter> CreateParameters(const FString& FunctionName, const TArray<FString>& Arguments, /*bool bMainExpression, */FRegexPattern* RegexPattern, FString& Errors);

    static FString CreateFunctions(const FString& OutputType, const FString& ExpressionName, const TArray<FHLSLParser::FFunction>& HLSLFunctions, FHLSLFunction&MainFunction, TArray<FHLSLFunction>& Functions);

    static FString MatchedSamplerArguments(FString& Code, TArray<FHLSLParameter>& Parameters);

public:
    static FHLSLContext CreateContext(const FString& OutputType, const FString& ExpressionName, const FString& Code, FString& Errors);

    static FString CreateExpression(const FString& OutputType, const FString& Code, FHLSLFunction& Main, FString& Functions);
};