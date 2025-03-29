// Copyright 2023 ZSttt, Inc. All Rights Reserved.

#include "HLSL/HLSLParser.h"

void FHLSLParser::ParseTokens(const FString& Code, 
TArray<FString>&Structs, TArray<FString>&Defines, TArray<FString>&Functions, 
bool MatchingSimilarDefines /*= true*/, bool MatchingInlineFunctions /*= true*/)
{
    const TCHAR* NamePattern = TEXT("\\b[A-Za-z0-9_]+");
    const TCHAR* ArgumentsPattern = TEXT("\\(([\\S\\s]*)?\\)");

    //Structs
    {
        FString Pattern = FString::Printf(TEXT("struct\\s+(%s)(?=\\s*\\{)"), NamePattern);
        FRegexPattern RegexPattern(Pattern);
        FRegexMatcher Matcher(RegexPattern, Code);
        while(Matcher.FindNext())
        {
            Structs.AddUnique(Matcher.GetCaptureGroup(1));
        }
    }

    //Defines
    {
        //Defined
        {
            FString Pattern = FString::Printf(TEXT("(#define\\s+(%s))"), NamePattern);
            FRegexPattern RegexPattern(Pattern);
            FRegexMatcher Matcher(RegexPattern, Code);
            while(Matcher.FindNext())
            {
                Defines.AddUnique(Matcher.GetCaptureGroup(2));
            }
        }

        //Similar
        if(MatchingSimilarDefines)
        {
            FString Pattern = FString::Printf(TEXT("\\b(([A-Z]{2,}[0-9]?_?)+)"), NamePattern);
            FRegexPattern RegexPattern(Pattern);
            FRegexMatcher Matcher(RegexPattern, Code);
            while(Matcher.FindNext())
            {
                if(Matcher.GetMatchEnding()<=Code.Len()-1)
                {
                    TCHAR Next = Code[Matcher.GetMatchEnding()];
                    if(FChar::IsUpper(Next)||Next=='_'||FChar::IsWhitespace(Next))
                        Defines.AddUnique(Matcher.GetCaptureGroup(1));
                }
            }
        }
    }

    //Functions
    {
        //Defined functions
        {
            //
            FString Pattern = FString::Printf(TEXT("((%s)\\s+(%s)\\s*\\(.*\\)\\s*(?=[\\{]))"), NamePattern, NamePattern);
            FRegexPattern RegexPattern(Pattern);
            FRegexMatcher Matcher(RegexPattern, Code);
            while(Matcher.FindNext())
            {
                Functions.AddUnique(Matcher.GetCaptureGroup(3));
            }
        }

        //Inline Functions
        if(MatchingInlineFunctions)
        {
            const TCHAR* PrePattern = TEXT("[\\s=\\.\\(,\\+\\-\\*/%><!&|~\\^\\?]");
            FString Pattern = FString::Printf(TEXT("((?<=(%s))(%s)\\s*(?=\\s*\\())"), PrePattern, NamePattern);
            FRegexPattern RegexPattern(Pattern);
            FRegexMatcher Matcher(RegexPattern, Code);
            while(Matcher.FindNext())
            {
                Functions.AddUnique(Matcher.GetCaptureGroup(3));
            }
        }
    }
}


FString FHLSLParser::GetHLSLExpressionEntryRegexPattern()
{
    return TEXT("\\bFMaterial([A-Za-z]+)Parameters\\b");
}

int32 FHLSLParser::TrimStart(FString& Code)
{
    int32 Pos = 0;
    while(Pos<Code.Len()&&(FChar::IsWhitespace(Code[Pos])||FChar::IsLinebreak(Code[Pos])))
    {
        Pos++;
    }
    Code.RemoveAt(0, Pos);
    return Pos;
}

int32 FHLSLParser::TrimEnd(FString& Code)
{
    int32 End = Code.Len();
    while(End>0&&(FChar::IsWhitespace(Code[End-1])||FChar::IsLinebreak(Code[End-1])))
    {
        End--;
    }
    int32 TrimCount = Code.Len()-End;
    Code.RemoveAt(End, TrimCount);
    return TrimCount;
}

void FHLSLParser::TrimStartAndEnd(FString& Code)
{
    TrimEnd(Code);
    TrimStart(Code);
}

FHLSLParameter FHLSLParser::CreateParameter(const FString& Code, FString& Error)
{
    if(Code.IsEmpty())
        return FHLSLParameter();

    EMaterialValueType ValueType = EMaterialValueType::MCT_Unknown;

    FHLSLParameter Parameter;

    ParseParameter(Code, Parameter.Type, Parameter.Name);
    if(Parameter.Type.Contains(HLSL_SAMPLER_TYPE_IDENTIFIER))
    {
        Parameter.bOutput = TreatAsOutput(Parameter.Type);
        if(Parameter.bOutput)
        {
            Error = TEXT("Parameter of SamplerState isn't support output.");
        }
        Parameter.Type = HLSL_SAMPLER_TYPE_IDENTIFIER;
        Parameter.Code = FString::Printf(TEXT("%s %s"), HLSL_SAMPLER_TYPE_IDENTIFIER, *Parameter.Name);
        return Parameter;
    }

    Parameter.bOutput = TreatAsOutput(Parameter.Type);
    ValueType = GetParameterType(Parameter.Type);
    Parameter.Code = FixupAndMakeParameter(ValueType, Parameter.Name, Parameter.bOutput, Error);

    if(Error.IsEmpty())
        return Parameter;
    return FHLSLParameter();
}

void FHLSLParser::ParseParameter(const FString& Code, FString& Type, FString& Name)
{
    int32 Index = INDEX_NONE;
    Code.FindLastChar(' ', Index);
    if(Index!=INDEX_NONE)
    {
        Name = Code.Mid(Index);
        TrimStartAndEnd(Name);
        Name.RemoveSpacesInline();
        Type = Code.Mid(0, Index);
    }
    else
    {
        Type = TEXT("");
        Name = TEXT("");
    }
}

int32 FHLSLParser::IsOutput(const FString& Type)
{
    FString Keys[2] = {TEXT("out "), TEXT("inout ")};
    for(FString& Key:Keys)
    {
        if(Type.StartsWith(Key))
        {
            return Key.Len();
        }
    }
    return INDEX_NONE;
}

bool FHLSLParser::TreatAsOutput(FString& Type)
{
    int32 Index = IsOutput(Type);
    if(Index>0)
    {
        Type = Type.Mid(Index);
        TrimStartAndEnd(Type);
        Type.RemoveSpacesInline();
        return true;
    }
    TrimStartAndEnd(Type);
    Type.RemoveSpacesInline();
    return false;
}

EMaterialValueType FHLSLParser::GetParameterType(const FString& InType)
{
    if(InType=="bool"||InType=="float"||InType=="int"||InType=="uint")
    {
        return EMaterialValueType::MCT_Float;
    }
    if(InType=="float2")
    {
        return EMaterialValueType::MCT_Float2;
    }
    if(InType=="float3")
    {
        return EMaterialValueType::MCT_Float3;
    }
    if(InType=="float4")
    {
        return EMaterialValueType::MCT_Float4;
    }
    if(InType=="Texture2D")
    {
        return EMaterialValueType::MCT_Texture2D;
    }
    if(InType=="Texture2DArray")
    {
        return EMaterialValueType::MCT_Texture2DArray;
    }
    if(InType=="TextureExternal")
    {
        return EMaterialValueType::MCT_TextureExternal;
    }
    if(InType=="TextureCube")
    {
        return EMaterialValueType::MCT_TextureCube;
    }
    if(InType=="Texture3D")
    {
        return EMaterialValueType::MCT_VolumeTexture;
    }
    if(InType=="FMaterialAttributes")
    {
        return EMaterialValueType::MCT_MaterialAttributes;
    }
    return EMaterialValueType::MCT_Unknown;
}

FString FHLSLParser::FixupAndMakeParameter(const EMaterialValueType InType, const FString& InName, const bool Output, FString& Errors)
{
    FString ParamDecl;
    if(InType==EMaterialValueType::MCT_Unknown)
    {
        Errors = FString::Printf(TEXT("Invalid type of parameter with \"%s\"."), *InName);
        return TEXT("");
    }

    if(Output)
    {
        switch(InType)
        {
            case MCT_Float:
                ParamDecl += TEXT("MaterialFloat ");
                ParamDecl += InName;
                break;
            case MCT_Float2:
                ParamDecl += TEXT("MaterialFloat2 ");
                ParamDecl += InName;
                break;
            case MCT_Float3:
                ParamDecl += TEXT("MaterialFloat3 ");
                ParamDecl += InName;
                break;
            case MCT_Float4:
                ParamDecl += TEXT("MaterialFloat4 ");
                ParamDecl += InName;
                break;
            case MCT_MaterialAttributes:
                ParamDecl += TEXT("FMaterialAttributes ");
                ParamDecl += InName;
                break;
            default:
                Errors = FString::Printf(TEXT("Invalid output type, only supported float、float2、float3、float4 and FMaterialAttributes."), *InName);
                break;
        }
    }
    else
    {
        switch(InType)
        {
            case MCT_Float:
            case MCT_Float1:
                ParamDecl += TEXT("MaterialFloat ");
                ParamDecl += InName;
                break;
            case MCT_Float2:
                ParamDecl += TEXT("MaterialFloat2 ");
                ParamDecl += InName;
                break;
            case MCT_Float3:
                ParamDecl += TEXT("MaterialFloat3 ");
                ParamDecl += InName;
                break;
            case MCT_Float4:
                ParamDecl += TEXT("MaterialFloat4 ");
                ParamDecl += InName;
                break;
            case MCT_Texture2D:
                ParamDecl += TEXT("Texture2D ");
                ParamDecl += InName;
                break;
            case MCT_TextureCube:
                ParamDecl += TEXT("TextureCube ");
                ParamDecl += InName;
                break;
            case MCT_Texture2DArray:
                ParamDecl += TEXT("Texture2DArray ");
                ParamDecl += InName;
                break;
            case MCT_TextureExternal:
                ParamDecl += TEXT("TextureExternal ");
                ParamDecl += InName;
                break;
            case MCT_VolumeTexture:
                ParamDecl += TEXT("Texture3D ");
                ParamDecl += InName;
                break;
            default:
                Errors = TEXT("Invalid input type.");
                break;
        }
    }
    return ParamDecl;
}

TArray<FHLSLParameter> FHLSLParser::CaptureArguments(const FString& Code, FString& Error)
{
    TArray<FHLSLParameter> Parameters;
    TArray<FString>Arguments;
    Code.ParseIntoArray(Arguments, TEXT(","));

    FString Key = TEXT("(\\b\\S+\\b\\s+)?(\\b\\S+\\b)\\s+(\\b\\S+\\b)");
    FRegexPattern RegexPattern(Key);
    for(int32 Index = 0; Index<Arguments.Num(); Index++)
    {
        FRegexMatcher Matcher(RegexPattern, Arguments[Index]);
        if(Matcher.FindNext())
        {
            FString OutputIdentifier = Matcher.GetCaptureGroup(1);
            bool bOutput = IsOutput(OutputIdentifier)>0;
            TrimStartAndEnd(OutputIdentifier);
            OutputIdentifier += TEXT(" ");

            FString Argument =
                FString::Printf(TEXT("%s %s"), *(bOutput?(OutputIdentifier+Matcher.GetCaptureGroup(2)):Matcher.GetCaptureGroup(2)), *(Matcher.GetCaptureGroup(3)));

            FHLSLParameter Parameter = CreateParameter(Argument, Error);
            if(Error.IsEmpty())
            {
                Parameters.Add(Parameter);
            }
            else
                return TArray<FHLSLParameter>{};
        }
    }

    return Parameters;
}

FString FHLSLParser::CaptureFunction(const FString& Code, FString& ReturnType, FString& Name, FString& Arguments, FString& Body, int32& Beginning, int32& Ending)
{
    const TCHAR* NamePattern = TEXT("\\b[A-Za-z0-9_]+")/*TEXT("(\\S*)")*/;
    FString Key = FString::Printf(TEXT("(^|\\s|\\}|;)%s\\s+%s\\s*\\(\\s*[^\\{]*\\s*\\{\\s*"), ReturnType.IsEmpty()?NamePattern:(*ReturnType), Name.IsEmpty()?NamePattern:(*Name));
    FRegexPattern RegexPattern(Key);
    FRegexMatcher Matcher(RegexPattern, Code);
    if(Matcher.FindNext())
    {
        ReturnType = ReturnType.IsEmpty()?Matcher.GetCaptureGroup(2):ReturnType;
        Name = Name.IsEmpty()?Matcher.GetCaptureGroup(3):Name;

        //Arguments
        {
            Arguments = Code.Mid(Matcher.GetMatchBeginning(), Matcher.GetMatchEnding()-Matcher.GetMatchBeginning());
            FString ArgsKey = TEXT("\\([^\\{]*\\)\\s*\\{");
            FRegexPattern ArgsRegexPattern(ArgsKey);
            FRegexMatcher ArgsMatcher = FRegexMatcher(ArgsRegexPattern, Arguments);

            if(ArgsMatcher.FindNext())
            {
                Arguments = Arguments.Mid(ArgsMatcher.GetMatchBeginning(), ArgsMatcher.GetMatchEnding()-ArgsMatcher.GetMatchBeginning());
                Arguments.RemoveAt(Arguments.Len()-1);
                TrimStartAndEnd(Arguments);

                int32 ArgumentScope = 0;
                if(Arguments[0]==TEXT('('))
                {
                    Arguments.RemoveAt(0);
                    ArgumentScope = 1;
                }
                if(Arguments[Arguments.Len()-1]==TEXT(')'))
                {
                    Arguments.RemoveAt(Arguments.Len()-1);
                    if(ArgumentScope!=1)
                        return FString::Printf(TEXT("Invalid function \"%s\" with an error while parsing parameters for missing '(' from start."), HLSL_MAIN_ENTRY_IDENTIFIER);
                    ++ArgumentScope;
                }
                if(ArgumentScope!=2)
                {
                    return FString::Printf(TEXT("Invalid struct of parameters with function \"%s\"."), HLSL_MAIN_ENTRY_IDENTIFIER);
                }
            }
        }

        //FunctionBody
        {
            Beginning = Matcher.GetMatchBeginning();
            Ending = Matcher.GetMatchEnding();
            while(Ending>0)
            {
                --Ending;
                if(Code[Ending]==TEXT('{'))
                    break;
            }

            int32 CharIndex = Ending;
            int32 CurlyBracketNesting = 0;
            while(CharIndex<Code.Len())
            {
                const TCHAR Char = Code[CharIndex];
                if(Char==TEXT('}'))
                {
                    --CurlyBracketNesting;
                    if(CurlyBracketNesting<0)
                    {
                        return FString::Printf(TEXT("Invalid function with too many '}'. %s"), *Name);
                    }
                    else if(CurlyBracketNesting==0)
                    {
                        ++Ending;
                        return TEXT("");
                    }
                    else
                        Body += Char;
                }
                else
                {
                    if(Char==TEXT('{'))
                    {
                        if(CurlyBracketNesting!=0)
                            Body += Char;
                        ++CurlyBracketNesting;
                    }
                    else
                        Body += Char;
                }
                ++Ending;
                ++CharIndex;
            }
            return FString::Printf(TEXT("Invalid function with too many '{'. %s"), *Name);
        }
    }
    else
        return FString::Printf(TEXT("Can't find matched the function of \"%s %s(..){}\"!"), *ReturnType, *Name);
}


FString FHLSLParser::ParseCodeContext(const FString& Code, 
TArray<FString>& Comments, TArray<FString>& Defines, TArray<FString>& Includes,
TArray<FString>& Structs, TArray<FString>& Variables, TArray<FFunction>&Functions)
{
    EParseState Result = EParseState::Context;
    int32 CharIndex = 0;
    int32 CurlyBracketNesting = 0;
    FFunction* pFunction;
    auto ProcessCodes = [&] (const TCHAR& Char, FString& Codes, FString& Error)->bool
    {
        switch(Result)
        {
            case EParseState::Context:
            {
                if(FChar::IsLinebreak(Char))
                {
                    return true;
                }
                else if(Codes==TEXT("#include"))
                {
                    Result = EParseState::Include;
                    if(!FChar::IsWhitespace(Char)&&Char!='\"')
                        Error = FString::Printf(TEXT("Invalid include code of %s"), *Codes);
                    else
                        Includes.Emplace();
                    return false;
                }
                else if(Codes==TEXT("#define"))
                {
                    Result = EParseState::Define;
                    if(!FChar::IsWhitespace(Char))
                        Error = FString::Printf(TEXT("Invalid define code of %s"), *Codes);
                    else
                        Defines.Emplace();
                    return false;
                }
                else if(Codes==TEXT("struct"))
                {
                    Result = EParseState::Struct;
                    if(!FChar::IsWhitespace(Char))
                        Error = FString::Printf(TEXT("Invalid struct without whitespace. %s"), *Codes);
                    else
                        Structs.Emplace();
                    return false;
                }
                else if(Codes==TEXT("//"))
                {
                    Result = EParseState::Comment;
                    Comments.Emplace();
                }
                else if(FChar::IsWhitespace(Char)&&!Codes.IsEmpty())
                {
                    TrimStartAndEnd(Codes);
                    Codes.RemoveSpacesInline();
                    pFunction = new(Functions) FFunction();
                    pFunction->ReturnType = Codes;
                    Result = EParseState::FunctionName;
                    return true;
                }
                else if(FChar::IsWhitespace(Char))
                    return true;
            }
            break;
            case EParseState::Comment:
            {
                if(FChar::IsLinebreak(Char))
                {
                    Comments.Last() = Codes;
                    Result = EParseState::Context;
                    return true;
                }
            }
            break;
            case EParseState::FunctionName:
            {
                if(Char==TEXT('('))
                {
                    TrimStartAndEnd(Codes);
                    Codes.RemoveSpacesInline();
                    pFunction->Name = Codes;
                    Result = EParseState::Parameters;
                    return true;
                }
                else if(Char==TEXT('='))
                {
                    Codes = FString::Printf(TEXT("%s %s "), *(pFunction->ReturnType), *(pFunction->Name))+Codes;
                    pFunction = nullptr;
                    Functions.RemoveAt(Functions.Num()-1);
                    Result = EParseState::Variable;
                    Variables.Emplace();
                    return false;
                }
            }
            break;
            case EParseState::Variable:
            {
                if(FChar::IsLinebreak(Char))
                {
                    TrimStartAndEnd(Codes);
                    Variables.Last() = Codes;
                    Result = EParseState::Context;
                    return true;
                }
            }
            break;
            case EParseState::Parameters:
            {
                if(Char==TEXT(')'))
                {
                    if(!Codes.IsEmpty())
                    {
                        TrimStartAndEnd(Codes);
                        pFunction->Arguments.Add(Codes);
                    }
                    Result = EParseState::FunctionBody;
                    return true;
                }
                else if(Char==TEXT(','))
                {
                    if(!Codes.IsEmpty())
                    {
                        TrimStartAndEnd(Codes);
                        pFunction->Arguments.Add(Codes);
                    }
                    return true;
                }
                else if(Codes.IsEmpty()&&(FChar::IsWhitespace(Char)||FChar::IsLinebreak(Char)))
                    return true;
            }
            break;
            case EParseState::FunctionBody:
            {
                if(Char==TEXT('{'))
                {
                    if(CurlyBracketNesting==0)
                    {
                        ++CurlyBracketNesting;
                        return true;
                    }
                    ++CurlyBracketNesting;
                }
                else if(Char==TEXT('}'))
                {
                    --CurlyBracketNesting;
                    if(CurlyBracketNesting<0)
                    {
                        Error = FString::Printf(TEXT("Invalid function with too many '}'. %s"), *(pFunction->Name));
                    }
                    else if(CurlyBracketNesting==0)
                    {
                        TrimStartAndEnd(Codes);
                        Functions.Last().Body = Codes;
                        Result = EParseState::Context;
                        CurlyBracketNesting = 0;
                        return true;
                    }
                }
            }
            break;
            case EParseState::Include:
            {
                if(FChar::IsLinebreak(Char))
                {
                    Includes.Last() = Codes;
                    Result = EParseState::Context;
                    return true;
                }
            }
            break;
            case EParseState::Define:
            {
                if(FChar::IsLinebreak(Char))
                {
                    Defines.Last() = Codes;
                    Result = EParseState::Context;
                    return true;
                }
            }
            break;
            case EParseState::Struct:
            {
                if(Char==TEXT('{'))
                {
                    ++CurlyBracketNesting;
                }
                else if(Char==TEXT('}'))
                {
                    --CurlyBracketNesting;
                }
                else if(CurlyBracketNesting==0&&Codes[Codes.Len()-1]==TEXT('}'))
                {
                    if(Char==TEXT(';'))
                    {
                        Structs.Last() = (Codes+Char);
                        Result = EParseState::Context;
                        CurlyBracketNesting = 0;
                        return true;
                    }
                    else
                    {
                        Structs.Last() = (Codes+TEXT(';'));
                        Result = EParseState::Context;
                        return false;
                    }
                }
            }
            break;
        }
        return false;
    };


    FString Error;
    FString Codes;
    while(CharIndex<Code.Len()&&Error.IsEmpty())
    {
        const TCHAR Char = Code[CharIndex];
        if(ProcessCodes(Char, Codes, Error))
            Codes.Empty();
        else
            Codes += Char;

        if(!Error.IsEmpty())
            return Error;
        ++CharIndex;
    };

    if(Result!=EParseState::Context)
    {
        return TEXT("Parsing failed!");
    }
    return TEXT("");
}

bool FHLSLParser::ReplaceFunctionArguments(FString&Code, const FString& Type, const FString& Name, const FString& Arguments)
{
    auto Replace = [&] (int32 StartIndex, int32 EndIndex)
    {
        Code = Code.Mid(0, StartIndex)+Arguments+Code.Mid(EndIndex);
    };

    FString Key = FString::Printf(TEXT("(^|\\s|\\}|;)%s\\s+%s\\s*\\(\\s*[^\\{]*\\s*\\{\\s*"),(*Type), (*Name));

    FRegexPattern RegexPattern(Key);
    FRegexMatcher Matcher(RegexPattern, Code);

    bool bCompleted = false;
    if(Matcher.FindNext())
    {
        bCompleted = true;

        int32 Start = -1;
        int32 End = -1;

        for (int32 Index = Matcher.GetMatchBeginning(); Index < Matcher.GetMatchEnding(); Index++)
        {
            if(Code[Index] == '(')
            {
                Start = Index+1;
                break;
            }
        }
        for(int32 Index = Matcher.GetMatchEnding()-1; Index>Matcher.GetMatchBeginning(); Index--)
        {
            if(Code[Index]==')')
            {
                End = Index;
                break;
            }
        }
        Replace(Start, End);
    }

    return bCompleted;
}

FString FHLSLParser::GetFunctionCode(const FFunction& Function)
{
    FString Inputs;
    for(const FString& Param:Function.Arguments)
    {
        if(!Inputs.IsEmpty()&&!Param.IsEmpty())
            Inputs += TEXT(", ");
        else if(!Param.IsEmpty())
            Inputs += Param;
    }
    return FString::Printf(TEXT("%s %s(%s)\r\n{\r\n%s\r\n}\r\n"), *(Function.ReturnType), *(Function.Name), *Inputs, *(Function.Body));
}

TArray<FHLSLParameter> FHLSLParser::CreateParameters(const FString& FunctionName, const TArray<FString>& Arguments, /*bool bMainExpression, */FRegexPattern* RegexPattern, FString& Errors)
{
    //Regex
    FString FunctionKey = FunctionName;
    FString ParamKey;
    //

    TArray<FHLSLParameter> Parameters;
    for(int32 Index = 0; Index<Arguments.Num(); Index++)
    {
        const FString& Argument = Arguments[Index];
        if(Argument.IsEmpty())
            continue;

        FHLSLParameter Parameter = CreateParameter(Argument,Errors);
        if(!Errors.IsEmpty())
            return TArray<FHLSLParameter>{};

        //             if(Parameter.bOutput&&!bMainExpression)
        //             {
        //                 Errors = FString::Printf(TEXT("The function %s doesn't support returning values with inout or out. line%d"), *(Function.Name), Function.Line);
        //                 return TArray<FHLSLParameter>{};
        //             }

        if(RegexPattern)
        {
            if(Index!=(Arguments.Num()-1))
                ParamKey += TEXT(".*,/b");
            else
                ParamKey += TEXT(".*");
        }

        if(!Parameter.Code.IsEmpty())
        {
            Parameters.Add(Parameter);
        }
    }

    if(RegexPattern)
        *RegexPattern = FRegexPattern(FString::Printf(TEXT("\\b%s\\b\\s*\\(\\b%s\\b\\)*"), *FunctionKey, *ParamKey));
    return Parameters;
}

FString FHLSLParser::CreateFunctions(const FString& OutputType, const FString& ExpressionName, const TArray<FFunction>& HLSLFunctions, FHLSLFunction&MainFunction, TArray<FHLSLFunction>& Functions)
{
    FString Errors;
    auto AssignFunction = [] (FHLSLFunction&Src, const FHLSLParser::FFunction& Dest)
    {
        Src.ReturnType = Dest.ReturnType;
        Src.Name = Dest.Name;
        Src.Body = Dest.Body;
        Src.bMainExpression = Dest.Name==HLSL_MAIN_ENTRY_IDENTIFIER;
        Src.Code = GetFunctionCode(Dest);
    };

    auto FixupFunctionName = [] (FString& Function, const FString Expression)
    {
        Function = FString::Printf(TEXT("%s_%s"), *Function, *Expression);
    };

    struct FExpressionRegexPattern
    {
        FRegexPattern RegexPattern = FRegexPattern(TEXT(""));
        FString Key;
    };
    TArray<FExpressionRegexPattern> RegexPatterns;

    bool bUniqueFunction = false;
    //
    uint8 MainCounts = 0;
    Functions.Reserve(Functions.Num()+HLSLFunctions.Num()+1);
    for(int32 Index = 0; Index<HLSLFunctions.Num(); Index++)
    {
        FHLSLFunction Src;
        const FHLSLParser::FFunction& Dest = HLSLFunctions[Index];

        AssignFunction(Src, Dest);
        if(Src.bMainExpression&&!Src.ReturnType.IsEmpty()&&Src.ReturnType != OutputType)
        {
            return TEXT("Invalid main HLSL expression return type, it must be void!");
        }

        FRegexPattern RegexPattern(TEXT(""));
        Src.Parameters = CreateParameters(Dest.Name, Dest.Arguments,/*Src.bMainExpression,*/&RegexPattern, Errors);

        if(!Errors.IsEmpty())
            return Errors;

        if(Src.bMainExpression)
        {
            if(MainCounts>0)
                return FString::Printf(TEXT("Repetitive function for HLSL main expression."), *(Dest.Name), Dest.Line);
            else
            {
                MainFunction = Src;
                ++MainCounts;
            }
        }
        else
        {
            if(bUniqueFunction)
                FixupFunctionName(Src.Name, ExpressionName);

            Functions.Add(Src);

            RegexPatterns.Emplace();
            RegexPatterns.Last().RegexPattern = RegexPattern;
            RegexPatterns.Last().Key = Dest.Name;
        }
    }

    FString SamplerErrors = MatchedSamplerArguments(MainFunction.Body, MainFunction.Parameters);
    if(!SamplerErrors.IsEmpty())
        return SamplerErrors;

    if(!bUniqueFunction)
        return TEXT("");

    auto FixupFunctionNameReference = [&] (FHLSLFunction& Function)
    {
        for(const FExpressionRegexPattern& RegexPattern:RegexPatterns)
        {
            FRegexMatcher Matcher(RegexPattern.RegexPattern, Function.Body);
            while(Matcher.FindNext())
            {
                int32 Beginning = Matcher.GetMatchBeginning();
                int32 Ending = Matcher.GetMatchEnding();

                FString NewString = Function.Body.Mid(Beginning, Ending-Beginning).Replace(*(RegexPattern.Key), *(FString::Printf(TEXT("%s_%s"), *RegexPattern.Key, *ExpressionName)));
                FString Left = Function.Body.Mid(0, Beginning);
                FString Right = Function.Body.Mid(Ending);
                Function.Body = Left+NewString+Right;
                Matcher = FRegexMatcher(RegexPattern.RegexPattern, Function.Body);
            }
        }
    };

    for(FHLSLFunction& Function:Functions)
    {
        FixupFunctionNameReference(Function);
    }
    FixupFunctionNameReference(MainFunction);
    return TEXT("");
}

FString FHLSLParser::MatchedSamplerArguments(FString& Code, TArray<FHLSLParameter>& Parameters)
{
    for(auto It = Parameters.CreateIterator(); It; ++It)
    {
        if(It->Type==HLSL_SAMPLER_TYPE_IDENTIFIER)
        {
            bool bIncorrect = true;
            for(const FHLSLParameter& Parameter:Parameters)
            {
                if(FString::Printf(TEXT("%s %s"), *(It->Type), *(It->Name))==
                   FString::Printf(TEXT("%s %s%s"), HLSL_SAMPLER_TYPE_IDENTIFIER, *(Parameter.Name), HLSL_SAMPLER_NAME_IDENTIFIER))
                {
                    bIncorrect = false;
                    break;
                }
            }

            if(bIncorrect)
            {
                return FString::Printf(TEXT("Parse argument %s failed, can't find matched texture parameter. Please define Sampler by 'TextureName+%s'"), *(It->Name), HLSL_SAMPLER_NAME_IDENTIFIER);
            }
            It.RemoveCurrent();
        }
    }
    return TEXT("");
}

FHLSLContext FHLSLParser::CreateContext(const FString& OutputType, const FString& ExpressionName, const FString& Code, FString& Errors)
{
    TArray<FHLSLParser::FFunction> HLSLFunctions;
    FHLSLContext Context;
    Errors = ParseCodeContext(Code, Context.Comments, Context.Defines, Context.Includes, Context.Structs, Context.Variables, HLSLFunctions);
    if(!Errors.IsEmpty())
        return FHLSLContext();
    Errors = CreateFunctions(OutputType, ExpressionName, HLSLFunctions, Context.Main, Context.Functions);
    return Context;
}

FString FHLSLParser::CreateExpression(const FString& OutputType, const FString& Code, FHLSLFunction& Main, FString& Functions)
{
    Main.bMainExpression = true;
    Main.ReturnType = OutputType;
    Main.Name = HLSL_MAIN_ENTRY_IDENTIFIER;

    int32 Beginning = -1;
    int32 Ending = -1;
    FString Arguments;
    FString MainError = CaptureFunction(Code, Main.ReturnType, Main.Name, Arguments, Main.Body, Beginning, Ending);


    FString ArgumentsError;
    Main.Parameters = CaptureArguments(Arguments, ArgumentsError);

    FString SamplerError;
    SamplerError = MatchedSamplerArguments(Main.Body, Main.Parameters);

    Functions += Code.Mid(0, Beginning);
    Functions += Code.Mid(Ending);

    FString TotalError = MainError;

	{
		if (!TotalError.IsEmpty() && !ArgumentsError.IsEmpty())
		{
			TotalError += TEXT("\r\n\r\n");
		}
		TotalError += ArgumentsError;
	}

	{
		if (!TotalError.IsEmpty() && !SamplerError.IsEmpty())
		{
			TotalError += TEXT("\r\n\r\n");
		}
		TotalError += SamplerError;
	}

    return TotalError;
}

