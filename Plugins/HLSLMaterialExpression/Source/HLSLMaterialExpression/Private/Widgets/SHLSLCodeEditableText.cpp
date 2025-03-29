// Copyright 2023 ZSttt, Inc. All Rights Reserved.

#include "Widgets/SHLSLCodeEditableText.h"
#include "HLSL/HLSLParser.h"
#include "HLSL/Configuration.h"
#include "HLSL/HLSLStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/SlateTypes.h"
#include "Styling/CoreStyle.h"
#include "Framework/Text/TextLayout.h"
#include "Framework/Text/SyntaxTokenizer.h"
#include "Framework/Text/SyntaxHighlighterTextLayoutMarshaller.h"
#include "Framework/Text/IRun.h"
#include "Framework/Text/ISlateRun.h"
#include "Framework/Text/SlateTextRun.h"
#include "Misc/ExpressionParserTypes.h"
#include "Fonts/FontMeasure.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Text/SlateEditableTextLayout.h"
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 2
#include "InputCoreTypes.h"
#else
#include "InputCore/Classes/InputCoreTypes.h"
#endif
#include "Internationalization/BreakIterator.h"
/*#include "Engine.h"*/

class FHLSLTextRun : public FSlateTextRun
{
public:
    static TSharedRef< FHLSLTextRun > Create(const FRunInfo& InRunInfo, const TSharedRef< const FString >& InText, const FTextBlockStyle& Style, const FTextRange& InRange, int32 Line)
    {
        return MakeShareable(new FHLSLTextRun(InRunInfo, InText, Style, InRange, Line));
    }
    int32 GetLineIndex()const{return LineIndex;};
    TSharedRef< const FString > GetText() const { return Text; };
protected:
    FHLSLTextRun(const FRunInfo& InRunInfo, const TSharedRef< const FString >& InText, const FTextBlockStyle& InStyle, const FTextRange& InRange, int32 Line)
    : FSlateTextRun(InRunInfo, InText, InStyle, InRange), LineIndex(Line){}
private:
    int32 LineIndex = 0;
};

class FWhiteSpaceTextRun : public FHLSLTextRun
{
public:
    static TSharedRef< FWhiteSpaceTextRun > Create(const FRunInfo& InRunInfo, const TSharedRef< const FString >& InText, const FTextBlockStyle& Style, const FTextRange& InRange, int32 Line, int32 NumSpacesPerTab)
    {
        return MakeShareable( new FWhiteSpaceTextRun( InRunInfo, InText, Style, InRange, Line, NumSpacesPerTab ) );
    }
public:
    virtual FVector2D Measure(int32 StartIndex, int32 EndIndex, float Scale, const FRunTextContext& TextContext) const override
    {
        const FVector2D ShadowOffsetToApply((EndIndex==Range.EndIndex)?FMath::Abs(Style.ShadowOffset.X * Scale):0.0f, FMath::Abs(Style.ShadowOffset.Y * Scale));

        if(EndIndex-StartIndex==0)
        {
            return FVector2D(ShadowOffsetToApply.X * Scale, GetMaxHeight(Scale));
        }

        int32 TabCount = 0;
        for(int32 Index = StartIndex; Index<EndIndex; Index++)
        {
            if((*Text)[Index]==TEXT('\t'))
            {
                TabCount++;
            }
        }

        const TSharedRef< FSlateFontMeasure > FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
        FVector2D Size = FontMeasure->Measure(**Text, StartIndex, EndIndex, Style.Font, true, Scale)+ShadowOffsetToApply;

        Size.X -= TabWidth*(float)TabCount * Scale;
        Size.X += SpaceWidth*(float)(TabCount * NumSpacesPerTab) * Scale;

        return Size;
    }
protected:
    FWhiteSpaceTextRun(const FRunInfo& InRunInfo, const TSharedRef< const FString >& InText, const FTextBlockStyle& InStyle, const FTextRange& InRange, int32 Line, int32 InNumSpacesPerTab)
    : FHLSLTextRun(InRunInfo, InText, InStyle, InRange, Line)
	, NumSpacesPerTab(InNumSpacesPerTab)
    {
        const TSharedRef< FSlateFontMeasure > FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
        TabWidth = FontMeasure->Measure(TEXT("\t"), 0, 1, Style.Font, true, 1.0f).X;
        SpaceWidth = FontMeasure->Measure(TEXT(" "), 0, 1, Style.Font, true, 1.0f).X;
    }
private:
    int32 NumSpacesPerTab;

    float TabWidth;

    float SpaceWidth;
};

class FHLSLSyntaxTokenizer
{
private:
    struct FTextRangeWithSentence : public FTextRange
    {
        FTextRangeWithSentence() : FTextRange(){}
        FTextRangeWithSentence(int32 InBeginIndex, int32 InEndIndex) : FTextRange(InBeginIndex, InEndIndex){}
        TArray<int32> Sentences;

        static void CalculateLineSentenceAndRangesFromString(const FString& Input, TArray<FTextRangeWithSentence>& LineRanges)
        {
            int32 LineBeginIndex = 0;

            FTextRangeWithSentence NewTextRange;
            const TCHAR* const InputStart = *Input;
            for (const TCHAR* CurrentChar = InputStart; CurrentChar && *CurrentChar; ++CurrentChar)
            {
                if (*CurrentChar == '.')
                {
                    NewTextRange.Sentences.Add(UE_PTRDIFF_TO_INT32(CurrentChar - InputStart));
                }
                const bool bIsWindowsNewLine = (*CurrentChar == '\r' && *(CurrentChar + 1) == '\n');
                if (bIsWindowsNewLine || FChar::IsLinebreak(*CurrentChar))
                {
                    const int32 LineEndIndex = UE_PTRDIFF_TO_INT32(CurrentChar - InputStart);
                    check(LineEndIndex >= LineBeginIndex);
                    NewTextRange.BeginIndex = LineBeginIndex;
                    NewTextRange.EndIndex = LineEndIndex;
                    LineRanges.Emplace(NewTextRange);
                    NewTextRange.Sentences.Empty();

                    if (bIsWindowsNewLine)
                    {
                        ++CurrentChar;
                    }
                    LineBeginIndex = UE_PTRDIFF_TO_INT32(CurrentChar - InputStart) + 1; 
                }
            }

            if (LineBeginIndex <= Input.Len())
            {
                NewTextRange.BeginIndex = LineBeginIndex;
                NewTextRange.EndIndex = Input.Len();
                LineRanges.Emplace(NewTextRange);
            }
        };
    };
public:
    virtual ~FHLSLSyntaxTokenizer() {};

    static void AddToken(TSharedPtr<HLSLSyntax::FSyntaxTokenizerRules>Tokenizer, FString Token, HLSLSyntax::ETokenType Type)
    {
        FHLSLParser::TrimStartAndEnd(Token);
        if(!Token.IsEmpty())
            Tokenizer->Tokens.AddUnique(HLSLSyntax::FRule(Token, Type));
    }
    static void SortTokens(TArray<HLSLSyntax::FRule>& Tokens)
    {
        Tokens.Sort([] (const HLSLSyntax::FRule&A, const HLSLSyntax::FRule&B)
        {
            return A.MatchText.Len()>B.MatchText.Len();
        });
    }
    static TSharedRef< FHLSLSyntaxTokenizer > Create(const HLSLSyntax::FSyntaxTokenizerRules& InRules)
    {
        return MakeShareable(new FHLSLSyntaxTokenizer(InRules));
    }
    void UpdateTokens(const TArray<HLSLSyntax::FRule>& NewTokens)
    {
        Rules.Tokens = NewTokens;
        SortTokens(Rules.Tokens);
    }
    void Process(TArray<HLSLSyntax::FTokenizedLine>& OutTokenizedLines, const FString& Input)
    {
        TArray<FTextRangeWithSentence> LineRanges;
        FTextRangeWithSentence::CalculateLineSentenceAndRangesFromString(Input, LineRanges);
        TokenizeLineRanges(Input, LineRanges, OutTokenizedLines);
    }
private:
    FHLSLSyntaxTokenizer(const HLSLSyntax::FSyntaxTokenizerRules& InRules) : Rules(InRules) {}
    void TokenizeLineRanges(const FString& Input, const TArray<FTextRangeWithSentence>& LineRanges, TArray<HLSLSyntax::FTokenizedLine>& OutTokenizedLines)
    {
        TSharedRef<IBreakIterator> WBI = FBreakIterator::CreateWordBreakIterator();
        WBI->SetString(Input);

        int32 InputLength = Input.Len();
        for(const FTextRangeWithSentence& LineRange:LineRanges)
        {
            HLSLSyntax::FTokenizedLine TokenizedLine;
            TokenizedLine.Range = LineRange;

            if(TokenizedLine.Range.IsEmpty())
            {
                TokenizedLine.Tokens.Emplace(HLSLSyntax::FToken(0, TokenizedLine.Range));
            }
            else
            {
                int32 CurrentOffset = LineRange.BeginIndex;
                int32 SentenceEndIndex = LineRange.EndIndex;
                for (int32 SentenceIndex = 0; SentenceIndex <= LineRange.Sentences.Num(); SentenceIndex++)
                {
                    bool bIsSentence = LineRange.Sentences.IsValidIndex(SentenceIndex);
                    if (bIsSentence)
                        SentenceEndIndex = LineRange.Sentences[SentenceIndex];
                    else
                        SentenceEndIndex = LineRange.EndIndex;

                    while (CurrentOffset < SentenceEndIndex)
                    {
                        bool bHasMatchedSyntax = false;

                        for (const HLSLSyntax::FRule& Rule : Rules.Tokens)
                        {
                            if (Rule.MatchText == TEXT("clamp"))
                            {
                                bHasMatchedSyntax = FCString::Strncmp(&Input[CurrentOffset], *Rule.MatchText, Rule.MatchText.Len()) == 0;
                            }

                            if (FCString::Strncmp(&Input[CurrentOffset], *Rule.MatchText, Rule.MatchText.Len()) == 0)
                            {
                                const int32 SyntaxTokenEnd = CurrentOffset + Rule.MatchText.Len();
                                bool bFullMatching = Rule.Type > HLSLSyntax::ETokenType::PreProcessor;
                                if (bFullMatching)
                                {
                                    if (SyntaxTokenEnd == InputLength)
                                        bHasMatchedSyntax = true;
                                    else if (/*SyntaxTokenEnd != SentenceEndIndex && */SyntaxTokenEnd < InputLength)
                                    {
                                        const TCHAR& NextChar = Input[SyntaxTokenEnd];
                                        if (FChar::IsWhitespace(NextChar) || FChar::IsLinebreak(NextChar) || (!FChar::IsAlpha(NextChar) && !FChar::IsDigit(NextChar) && NextChar != '_'))
                                        {
                                            bHasMatchedSyntax = true;
                                        }
                                    }
                                }
                                else
                                    bHasMatchedSyntax = true;

                                if (bHasMatchedSyntax)
                                {
                                    TokenizedLine.Tokens.Emplace(HLSLSyntax::FToken(Rule.Type, FTextRange(CurrentOffset, SyntaxTokenEnd)));
                                    check(SyntaxTokenEnd <= SentenceEndIndex);
                                    bHasMatchedSyntax = true;
                                    CurrentOffset = SyntaxTokenEnd;
                                    break;
                                }
                            }
                        }

                        if (bHasMatchedSyntax)
                        {
                            continue;
                        }

                        const int32 NextWordBoundary = WBI->MoveToCandidateAfter(CurrentOffset);
                        const int32 TextTokenEnd = (NextWordBoundary == INDEX_NONE) ? SentenceEndIndex : FMath::Min(NextWordBoundary, SentenceEndIndex);
                        TokenizedLine.Tokens.Emplace(HLSLSyntax::FToken(0, FTextRange(CurrentOffset, TextTokenEnd)));
                        CurrentOffset = TextTokenEnd;

                        if (bIsSentence&& CurrentOffset == SentenceEndIndex)
                        {
                            TokenizedLine.Tokens.Emplace(HLSLSyntax::FToken(HLSLSyntax::ETokenType::Operator, FTextRange(CurrentOffset, CurrentOffset + 1)));
                            ++CurrentOffset;
                        }
                    }
                }
            }

            OutTokenizedLines.Add(TokenizedLine);
        };
    }
public:
    static TSharedPtr<HLSLSyntax::FSyntaxTokenizerRules> MakeSyntaxTokenizerRules()
    {
        using ETokenType = HLSLSyntax::ETokenType;
        TSharedPtr<HLSLSyntax::FSyntaxTokenizerRules> Default = MakeShareable(new HLSLSyntax::FSyntaxTokenizerRules());

        //
        {
            Default->Tokens.Reserve
            (
                50+
                HLSLConfig::GetConfiguration()->Operators.Num()+
                HLSLConfig::GetConfiguration()->Keywords.Num()+
                HLSLConfig::GetConfiguration()->PreProcessors.Num()+
                HLSLConfig::GetConfiguration()->Structs.Num()+
                HLSLConfig::GetConfiguration()->Defines.Num()+
                HLSLConfig::GetConfiguration()->Functions.Num()
            );

            for(const auto& Token:HLSLConfig::GetConfiguration()->Operators)
            {
                if(!Token.IsEmpty())
                    Default->Tokens.Emplace(HLSLSyntax::FRule(Token, ETokenType::Operator));
            }

            for(const auto& Token:HLSLConfig::GetConfiguration()->Keywords)
            {
                if(!Token.IsEmpty())
                    Default->Tokens.Emplace(HLSLSyntax::FRule(Token, ETokenType::Keyword));
            }

            for(const auto& Token:HLSLConfig::GetConfiguration()->PreProcessors)
            {
                if(!Token.IsEmpty())
                    Default->Tokens.Emplace(HLSLSyntax::FRule(Token, ETokenType::PreProcessor));
            }

            for(const auto& Token:HLSLConfig::GetConfiguration()->Structs)
            {
                AddToken(Default, Token, ETokenType::Struct);
            }

            for(const auto& Token:HLSLConfig::GetConfiguration()->Defines)
            {
                AddToken(Default, Token, ETokenType::Define);
            }

            for(const auto& Token:HLSLConfig::GetConfiguration()->Functions)
            {
                AddToken(Default, Token, ETokenType::Function);
            }

            {
                TArray<FString> ShaderObjects =
                {
                    TEXT("TextureCubeSample"),
                    TEXT("TextureExternalSample"),
                    TEXT("Texture2DArraySample"),
                    TEXT("Texture2DSample"),
                    TEXT("Texture3DSample"),

                    TEXT("TextureCube"),
                    TEXT("TextureExternal"),
                    TEXT("Texture2DArray"),
                    TEXT("Texture2D"),
                    TEXT("Texture3D"),

                    TEXT("SamplerState")
                };
                for(FString& Obj:ShaderObjects)
                {
                    Default->Tokens.Emplace(HLSLSyntax::FRule(Obj, ETokenType::Struct));
                }
            }
            SortTokens(Default->Tokens);
        }

        //
        {
            Default->Types.SetNum(ETokenType::Token_Type_Max);

            Default->Types[ETokenType::Unknow] =
                HLSLSyntax::FRuleType(TEXT("SyntaxHighlight.HLSL.Normal"), FHLSLCodePaletteStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.HLSL.Normal"));

            Default->Types[ETokenType::Operator] =
                HLSLSyntax::FRuleType(TEXT("SyntaxHighlight.HLSL.Operator"), FHLSLCodePaletteStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.HLSL.Operator"));

            Default->Types[ETokenType::Keyword] =
                HLSLSyntax::FRuleType(TEXT("SyntaxHighlight.HLSL.Keyword"), FHLSLCodePaletteStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.HLSL.Keyword"));

            Default->Types[ETokenType::PreProcessor] =
                HLSLSyntax::FRuleType(TEXT("SyntaxHighlight.HLSL.PreProcessorKeyword"), FHLSLCodePaletteStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.HLSL.PreProcessorKeyword"));

            Default->Types[ETokenType::Struct] =
                HLSLSyntax::FRuleType(TEXT("SyntaxHighlight.HLSL.Struct"), FHLSLCodePaletteStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.HLSL.Struct"));

            Default->Types[ETokenType::Define] =
                HLSLSyntax::FRuleType(TEXT("SyntaxHighlight.HLSL.Define"), FHLSLCodePaletteStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.HLSL.Define"));

            Default->Types[ETokenType::Function] =
                HLSLSyntax::FRuleType(TEXT("SyntaxHighlight.HLSL.Function"), FHLSLCodePaletteStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.HLSL.Function"));
        }

        //
        {
            Default->ParseTypes.SetNum(HLSLSyntax::EParseState::Parse_State_Max);

            Default->ParseTypes[HLSLSyntax::EParseState::None] =
                HLSLSyntax::FRuleType(TEXT("SyntaxHighlight.HLSL.Normal"), FHLSLCodePaletteStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.HLSL.Normal"));

            Default->ParseTypes[HLSLSyntax::EParseState::LookingForString] =
                HLSLSyntax::FRuleType(TEXT("SyntaxHighlight.HLSL.String"), FHLSLCodePaletteStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.HLSL.String"));

            Default->ParseTypes[HLSLSyntax::EParseState::LookingForSingleLineComment] =
                HLSLSyntax::FRuleType(TEXT("SyntaxHighlight.HLSL.Comment"), FHLSLCodePaletteStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.HLSL.Comment"));

            Default->ParseTypes[HLSLSyntax::EParseState::LookingForMultiLineComment] =
                HLSLSyntax::FRuleType(TEXT("SyntaxHighlight.HLSL.Comment"), FHLSLCodePaletteStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.HLSL.Comment"));

            Default->ParseTypes[HLSLSyntax::EParseState::LookingForSingleLineDefines] =
                HLSLSyntax::FRuleType(TEXT("SyntaxHighlight.HLSL.Define"), FHLSLCodePaletteStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.HLSL.Define"));

            Default->ParseTypes[HLSLSyntax::EParseState::LookingForNumber] =
                HLSLSyntax::FRuleType(TEXT("SyntaxHighlight.HLSL.Number"), FHLSLCodePaletteStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.HLSL.Number"));
        }

        return Default;
    }
    HLSLSyntax::FSyntaxTokenizerRules Rules;
};
static TSharedPtr<HLSLSyntax::FSyntaxTokenizerRules> DEFAULT_HLSL_TOKEN_RULES = nullptr;

class FHLSLTextLayoutMarshaller : public FPlainTextLayoutMarshaller
{
public:
    virtual ~FHLSLTextLayoutMarshaller(){}
    static TSharedPtr<FHLSLTextLayoutMarshaller> Create(const TArray<HLSLSyntax::FRule>& AdditionRules = TArray<HLSLSyntax::FRule>{},
    TFunction<void(FTextLayout*)>RegisterTextLayout = nullptr)
    {
        return MakeShareable(new FHLSLTextLayoutMarshaller(AdditionRules,RegisterTextLayout));
    }
    void SetAdditionTokens(const TArray<HLSLSyntax::FRule>&AdditionTokens)
    {
        TArray<HLSLSyntax::FRule> NewTokens = DEFAULT_HLSL_TOKEN_RULES->Tokens;
        for(const HLSLSyntax::FRule& Rule:AdditionTokens)
        {
            FString Token = Rule.MatchText;
            FHLSLParser::TrimStartAndEnd(Token);
            if(Token.IsEmpty())
                continue;

            NewTokens.AddUnique(HLSLSyntax::FRule(Token,Rule.Type));
        }
        Tokenizer->UpdateTokens(MoveTemp(NewTokens));
    }
    void SetText(const FString& SourceString, FTextLayout& TargetTextLayout) override
    {
        if(RegisterTextLayoutHandler)
            RegisterTextLayoutHandler(&TargetTextLayout);

        TArray<HLSLSyntax::FTokenizedLine> TokenizedLines;
        Tokenizer->Process(TokenizedLines, SourceString);
        ParseTokens(SourceString, TargetTextLayout, TokenizedLines);
    }
    virtual bool RequiresLiveUpdate() const override
    {
        return true;
    }
protected:
    void ParseTokens(const FString& SourceString, FTextLayout& TargetTextLayout, TArray<HLSLSyntax::FTokenizedLine> TokenizedLines)
    {
        using ETokenType = HLSLSyntax::ETokenType;

        TArray<FTextLayout::FNewLineData> LinesToAdd;
        LinesToAdd.Reserve(TokenizedLines.Num());

        HLSLSyntax::EParseState ParseState = HLSLSyntax::EParseState::None;
        for(int32 Line = 0; Line < TokenizedLines.Num(); Line++)
        {
            const HLSLSyntax::FTokenizedLine& TokenizedLine = TokenizedLines[Line];
            TSharedRef<FString> ModelString = MakeShareable(new FString());
            TArray< TSharedRef< IRun > > Runs;

            if(ParseState==HLSLSyntax::EParseState::LookingForSingleLineComment)
            {
                ParseState = HLSLSyntax::EParseState::None;
            }
            if(ParseState==HLSLSyntax::EParseState::LookingForSingleLineDefines)
            {
                ParseState = HLSLSyntax::EParseState::None;
            }

            for(const HLSLSyntax::FToken& Token : TokenizedLine.Tokens)
            {
                const FString TokenText = SourceString.Mid(Token.Range.BeginIndex, Token.Range.Len());

                const FTextRange ModelRange(ModelString->Len(), ModelString->Len()+TokenText.Len());

                HLSLSyntax::FRuleType& UnknowType = Tokenizer->Rules.ParseTypes[HLSLSyntax::EParseState::None];
                FRunInfo RunInfo(UnknowType.RuleName);
                FTextBlockStyle TextBlockStyle = UnknowType.RuleStyle;

                const bool bIsWhitespace = FString(TokenText).TrimEnd().IsEmpty();
                if(!bIsWhitespace)
                {
                    if(ParseState!=HLSLSyntax::EParseState::None)
                    {
                        if(ParseState==HLSLSyntax::EParseState::LookingForString && TokenText==TEXT("\""))
                        {
                            HLSLSyntax::FRuleType ParseType = Tokenizer->Rules.ParseTypes[ParseState];
                            RunInfo.Name = ParseType.RuleName;
                            TextBlockStyle = ParseType.RuleStyle;
                            ParseState = HLSLSyntax::EParseState::None;
                        }
                        else if(ParseState==HLSLSyntax::EParseState::LookingForMultiLineComment && TokenText==TEXT("*/"))
                        {
                            HLSLSyntax::FRuleType ParseType = Tokenizer->Rules.ParseTypes[ParseState];
                            RunInfo.Name = ParseType.RuleName;
                            TextBlockStyle = ParseType.RuleStyle;
                            ParseState = HLSLSyntax::EParseState::None;
                        }
                        else if(ParseState==HLSLSyntax::EParseState::LookingForSingleLineDefines && TokenText==TEXT("\""))
                        {
                            ParseState = HLSLSyntax::EParseState::LookingForString;
                            HLSLSyntax::FRuleType ParseType = Tokenizer->Rules.ParseTypes[ParseState];
                            RunInfo.Name = ParseType.RuleName;
                            TextBlockStyle = ParseType.RuleStyle;
                        }
                        else 
                        {
                            HLSLSyntax::FRuleType ParseType = Tokenizer->Rules.ParseTypes[ParseState];
                            RunInfo.Name = ParseType.RuleName;
                            TextBlockStyle = ParseType.RuleStyle;
                        }

                        ModelString->Append(TokenText);
                        TSharedRef< ISlateRun > Run = FHLSLTextRun::Create(RunInfo, ModelString, TextBlockStyle, ModelRange, Line);
                        Runs.Add(Run);
                    }
                    else
                    {
                        if(Token.Type != ETokenType::Unknow)
                        {
                            if(ParseState == HLSLSyntax::EParseState::None && TokenText == TEXT("\""))
                            {
                                ParseState = HLSLSyntax::EParseState::LookingForString;
                                HLSLSyntax::FRuleType ParseType = Tokenizer->Rules.ParseTypes[ParseState];
                                RunInfo.Name = ParseType.RuleName;
                                TextBlockStyle = ParseType.RuleStyle;
                            }
                            else if(ParseState == HLSLSyntax::EParseState::None && TokenText == TEXT("//"))
                            {
                                ParseState = HLSLSyntax::EParseState::LookingForSingleLineComment;
                                HLSLSyntax::FRuleType ParseType = Tokenizer->Rules.ParseTypes[ParseState];
                                RunInfo.Name = ParseType.RuleName;
                                TextBlockStyle = ParseType.RuleStyle;
                            }
                            else if(ParseState == HLSLSyntax::EParseState::None && TokenText == TEXT("/*"))
                            {
                                ParseState = HLSLSyntax::EParseState::LookingForMultiLineComment;
                                HLSLSyntax::FRuleType ParseType = Tokenizer->Rules.ParseTypes[ParseState];
                                RunInfo.Name = ParseType.RuleName;
                                TextBlockStyle = ParseType.RuleStyle;
                            }
                            else if(ParseState == HLSLSyntax::EParseState::None && TokenText.StartsWith(TEXT("#")))
                            {
                                ParseState = HLSLSyntax::EParseState::LookingForSingleLineDefines;
                                HLSLSyntax::FRuleType ParseType = Tokenizer->Rules.Types[ETokenType::PreProcessor];
                                RunInfo.Name = ParseType.RuleName;
                                TextBlockStyle = ParseType.RuleStyle;
                            }
                            else
                            {
                                HLSLSyntax::FRuleType RuleType = Tokenizer->Rules.Types[Token.Type];
                                RunInfo.Name = RuleType.RuleName;
                                TextBlockStyle = RuleType.RuleStyle;
                                ParseState = HLSLSyntax::EParseState::None;
                            }

                            ModelString->Append(TokenText);
                            TSharedRef< ISlateRun > Run = FHLSLTextRun::Create(RunInfo, ModelString, TextBlockStyle, ModelRange, Line);
                            Runs.Add(Run);
                        }
                        else
                        {
                            FString RunName;
                            int32 CharIndex = 0;
                            uint8 Type = 0;
                            FString LocalToken;
                            FTextBlockStyle LocalTextStyle;
                            while(CharIndex<TokenText.Len())
                            {
                                bool bLastChar = CharIndex==TokenText.Len()-1;
                                FTextBlockStyle CharStyle;
                                uint8 CharType = 0;
                                FString TypeName;
                                TCHAR Char = TokenText[CharIndex];
                                if(TChar<WIDECHAR>::IsDigit(Char))
                                {
                                    CharType = 1;
                                    HLSLSyntax::FRuleType& RuleType = Tokenizer->Rules.ParseTypes[HLSLSyntax::EParseState::LookingForNumber];
                                    CharStyle = RuleType.RuleStyle;
                                    TypeName = RuleType.RuleName;
                                }
                                else if(TChar<WIDECHAR>::IsAlpha(Char))
                                {
                                    CharType = 2;
                                    HLSLSyntax::FRuleType& RuleType = Tokenizer->Rules.Types[ETokenType::Unknow];
                                    CharStyle = RuleType.RuleStyle;
                                    TypeName = RuleType.RuleName;
                                }
                                else
                                {
                                    CharType = 3;
                                    HLSLSyntax::FRuleType& RuleType = Tokenizer->Rules.Types[ETokenType::Operator];
                                    CharStyle = RuleType.RuleStyle;
                                    TypeName = RuleType.RuleName;
                                }

                                if(Type==0)
                                {
                                    Type = CharType;
                                    RunName = TypeName;
                                    LocalTextStyle = CharStyle;
                                }

                                if(CharType!=Type||bLastChar)
                                {
                                    if(bLastChar)
                                        LocalToken += Char;

                                    const FTextRange LocalModelRange(ModelString->Len(), ModelString->Len()+LocalToken.Len());
                                    ModelString->Append(LocalToken);

                                    TSharedRef< ISlateRun > Run = FHLSLTextRun::Create(FRunInfo(RunName), ModelString, LocalTextStyle, LocalModelRange, Line);
                                    Runs.Add(Run);

                                    LocalToken.Empty();

                                    Type = CharType;
                                    RunName = TypeName;
                                    LocalTextStyle = CharStyle;
                                }

                                LocalToken += Char;
                                ++CharIndex;
                            }

                        }
                    }
                }
                else
                {
                    ModelString->Append(TokenText);
                    RunInfo.Name = TEXT("SyntaxHighlight.HLSL.WhiteSpace");
                    TSharedRef< ISlateRun > Run = FWhiteSpaceTextRun::Create(RunInfo, ModelString, TextBlockStyle, ModelRange, Line, 4);
                    Runs.Add(Run);
                }
            }

            LinesToAdd.Emplace(MoveTemp(ModelString), MoveTemp(Runs));
        }

        TargetTextLayout.AddLines(LinesToAdd);
    }

    FHLSLTextLayoutMarshaller(const TArray<HLSLSyntax::FRule>& AdditionRules,TFunction<void(FTextLayout*)>RegisterTextLayout)
    {
        if(!DEFAULT_HLSL_TOKEN_RULES)
        {
            DEFAULT_HLSL_TOKEN_RULES = FHLSLSyntaxTokenizer::MakeSyntaxTokenizerRules();
        }

        HLSLSyntax::FSyntaxTokenizerRules TokenizerRules = *(DEFAULT_HLSL_TOKEN_RULES.Get());

        for(const HLSLSyntax::FRule& Rule:AdditionRules)
        {
            FString Token = Rule.MatchText;
            FHLSLParser::TrimStartAndEnd(Token);
            if(Token.IsEmpty())
                continue;

            TokenizerRules.Tokens.AddUnique(HLSLSyntax::FRule(Token,Rule.Type));
        }


        Tokenizer = FHLSLSyntaxTokenizer::Create(TokenizerRules);

        RegisterTextLayoutHandler = RegisterTextLayout;
    }

    TFunction<void(FTextLayout*)>RegisterTextLayoutHandler = nullptr;

    TSharedPtr< FHLSLSyntaxTokenizer > Tokenizer;
};

class FHLSLTokenHighlighter : public ISlateLineHighlighter
{
public:
    static TSharedRef< FHLSLTokenHighlighter > Create()
    {
        return MakeShareable(new FHLSLTokenHighlighter());
    };

    virtual int32 OnPaint(const FPaintArgs& Args, const FTextLayout::FLineView& Line, const float OffsetX, const float Width, const FTextBlockStyle& DefaultStyle, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override
    {
        const FVector2D Location(Line.Offset.X+OffsetX, Line.Offset.Y);

        FLinearColor HighlightBackgroundColorAndOpacity = /*FLinearColor(FColor::FromHex(TEXT("734100FF")))*/FLinearColor::Red;
        HighlightBackgroundColorAndOpacity.A = 0.7f;

        const float InverseScale = Inverse(AllottedGeometry.Scale);

        const float MinHighlightWidth = (Line.Range.IsEmpty())?4.0f * AllottedGeometry.Scale:0.0f;
        const float HighlightWidth = FMath::Max(Width, MinHighlightWidth);
        if(HighlightWidth>0.0f)
        {
            FSlateDrawElement::MakeBox(
                OutDrawElements,
                ++LayerId,
                AllottedGeometry.ToPaintGeometry(TransformVector(InverseScale, FVector2D(HighlightWidth, FMath::Max(Line.Size.Y, Line.TextHeight))), FSlateLayoutTransform(TransformPoint(InverseScale, Location))),
                &DefaultStyle.HighlightShape,
                ESlateDrawEffect::None,
                HighlightBackgroundColorAndOpacity
            );
        }

        return LayerId;
    }
protected:
    FHLSLTokenHighlighter() {};
};

class FHLSLTextLayout : public FSlateTextLayout
{
public:
    FHLSLTextLayout(SWidget* InOwner, FTextBlockStyle InDefaultTextStyle):FSlateTextLayout(InOwner, InDefaultTextStyle)
    {
        AggregateChildren();
    };
protected:
    int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
    const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override
    {
        return FSlateTextLayout::OnPaint(Args,AllottedGeometry,MyCullingRect,OutDrawElements,LayerId,InWidgetStyle,bParentEnabled);
    };

    int32 OnPaintHighlights(const FPaintArgs& Args, const FTextLayout::FLineView& LineView, const TArray<FLineViewHighlight>& Highlights, const FTextBlockStyle& InDefaultTextStyle,
    const FGeometry& AllottedGeometry, const FSlateRect& ClippingRect, FSlateWindowElementList& OutDrawElements, const int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override
    {
        return FSlateTextLayout::OnPaintHighlights(Args, LineView, Highlights, InDefaultTextStyle, AllottedGeometry, ClippingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
    };
};

class FHLSLEditableTextLayout : public FSlateEditableTextLayout
{
public:
    FHLSLEditableTextLayout
    (
        ISlateEditableTextWidget& InOwnerWidget,
        const TAttribute<FText>& InInitialText,
        FTextBlockStyle InTextStyle,
        const TOptional<ETextShapingMethod>
        InTextShapingMethod,
        const TOptional<ETextFlowDirection> InTextFlowDirection,
        TSharedRef<ITextLayoutMarshaller> InTextMarshaller,
        TSharedRef<ITextLayoutMarshaller> InHintTextMarshaller
    ) :
        FSlateEditableTextLayout
        (
            InOwnerWidget,
            InInitialText,
            InTextStyle,
            InTextShapingMethod,
            InTextFlowDirection,
            FCreateSlateTextLayout::CreateLambda([] (SWidget* InOwningWidget, const FTextBlockStyle& InDefaultTextStyle)->TSharedRef<FSlateTextLayout>
            {
                return MakeShareable(new FHLSLTextLayout(InOwningWidget, InDefaultTextStyle));
            }),
            InTextMarshaller,
            InHintTextMarshaller
        ){};

    void RegisterHLSLTokenHighlighter()
    {
        HLSLTokenHighlighter = FHLSLTokenHighlighter::Create();
    }

    void UnregisterHLSLTokenHighlighter()
    {
        HLSLTokenHighlighter = nullptr;
    }

    void RemoveHLSLTokenHighlighter(FTextLayout* InTextLayout)
    {
        if(!InTextLayout)
            return;

        if(TokenHighlight&&TokenHighlight.IsValid())
            InTextLayout->RemoveLineHighlight(*(TokenHighlight.Get()));
    }

    void UpdateHLSLTokenHighlighter(FTextLayout* InTextLayout)
    {
        if(!this->AnyTextSelected()||!InTextLayout)
            return;

        RemoveHLSLTokenHighlighter(InTextLayout);

        TArray<TSharedRef<const IRun>> Runs = this->GetSelectedRuns();

        for(TSharedRef<const IRun> Run:Runs)
        {
            FString Token;
            Run->AppendTextTo(Token);
            if(Token.Contains(this->GetSearchText().ToString()))
            {
                TSharedRef<const FHLSLTextRun>HLSLTextRun = StaticCastSharedRef<const FHLSLTextRun>(Run);
                TokenHighlight = MakeShareable(new FTextLineHighlight(HLSLTextRun->GetLineIndex(), Run->GetTextRange(), -1, HLSLTokenHighlighter.ToSharedRef()));
                InTextLayout->AddLineHighlight(*(TokenHighlight.Get()));
            }
        }
    }

protected:
    TSharedPtr<FTextLineHighlight> TokenHighlight = nullptr;
    TSharedPtr<FHLSLTokenHighlighter> HLSLTokenHighlighter = nullptr;
};





void SHLSLCodeEditableText::Construct(const FArguments& InArgs, const TArray<HLSLSyntax::FRule>&  AdditionTokens, bool AdvanceDisplay)
{
    GetAdditionTokens = InArgs._GetAdditionTokens;
    HLSLRichTextMarshaller = AdvanceDisplay ? FHLSLTextLayoutMarshaller::Create(AdditionTokens,[this](FTextLayout* InTextLayout){RegisterTextLayout(InTextLayout);}) : nullptr;

    SMultiLineEditableText::FArguments Arguments;
    Arguments.Font(FHLSLCodePaletteStyle::Get().GetWidgetStyle<FTextBlockStyle>("HLSLPalette.NormalText").Font);
    Arguments.TextStyle(&FHLSLCodePaletteStyle::Get().GetWidgetStyle<FTextBlockStyle>("HLSLPalette.NormalText"));
    Arguments.Text(InArgs._Text);
    Arguments.Marshaller(HLSLRichTextMarshaller);
    Arguments.AutoWrapText(false);
    Arguments.Margin(0.0f);
    Arguments.HScrollBar(InArgs._HScrollBar);
    Arguments.VScrollBar(InArgs._VScrollBar);
    Arguments.OnTextChanged(InArgs._OnCodeChanged);
    Arguments.OnTextCommitted(InArgs._OnCodeCommitted);

    SMultiLineEditableText::Construct(Arguments);

    FTextBlockStyle TextStyle = *Arguments._TextStyle;
    if(Arguments._Font.IsSet()||Arguments._Font.IsBound())
    {
        TextStyle.SetFont(Arguments._Font.Get());
    }


    TSharedPtr<ITextLayoutMarshaller> Marshaller = Arguments._Marshaller;
    if(!Marshaller.IsValid())
    {
        Marshaller = FPlainTextLayoutMarshaller::Create();
    }

    EditableTextLayout =
    MakeUnique<FHLSLEditableTextLayout>
    (
        *this,
        Arguments._Text,
        TextStyle,
        Arguments._TextShapingMethod,
        Arguments._TextFlowDirection,
        Marshaller.ToSharedRef(),
		Marshaller.ToSharedRef()
    );

    EditableTextLayout->SetHintText(Arguments._HintText);
    EditableTextLayout->SetSearchText(Arguments._SearchText);
    EditableTextLayout->SetTextWrapping(Arguments._WrapTextAt, Arguments._AutoWrapText, Arguments._WrappingPolicy);
    EditableTextLayout->SetMargin(Arguments._Margin);
    EditableTextLayout->SetJustification(Arguments._Justification);
    EditableTextLayout->SetLineHeightPercentage(Arguments._LineHeightPercentage);

    OnCodeSavedCallback = InArgs._OnCodeSaved;
}

void SHLSLCodeEditableText::RegisterTextLayout(class FTextLayout* InTextLayout)
{
    TextLayout = InTextLayout;
}

void SHLSLCodeEditableText::EnterSearchMode()
{
    if(TextLayout)
    {
        FHLSLEditableTextLayout* HLSLTextLayout = static_cast<FHLSLEditableTextLayout*>(EditableTextLayout.Get());
        HLSLTextLayout->RegisterHLSLTokenHighlighter();
    }
}

void SHLSLCodeEditableText::ExitSearchMode()
{
    if(TextLayout)
    {
        TextLayout->ClearLineHighlights();

        FHLSLEditableTextLayout* HLSLTextLayout = static_cast<FHLSLEditableTextLayout*>(EditableTextLayout.Get());
        HLSLTextLayout->UnregisterHLSLTokenHighlighter();
    }
}

void SHLSLCodeEditableText::RemoveSearchTokenHighlight()
{
    if(TextLayout)
    {
        FHLSLEditableTextLayout* HLSLTextLayout = static_cast<FHLSLEditableTextLayout*>(EditableTextLayout.Get());
        HLSLTextLayout->RemoveHLSLTokenHighlighter(TextLayout);
    }
}

void SHLSLCodeEditableText::UpdateSearchTokenHighlight()
{
    if(TextLayout)
    {
        FHLSLEditableTextLayout* HLSLTextLayout = static_cast<FHLSLEditableTextLayout*>(EditableTextLayout.Get());
        HLSLTextLayout->UpdateHLSLTokenHighlighter(TextLayout);
    }
}

void SHLSLCodeEditableText::SetAdditionTokens(const TArray<HLSLSyntax::FRule>& NewTokens)
{
    if(!HLSLRichTextMarshaller)
        return;
    HLSLRichTextMarshaller->SetAdditionTokens(NewTokens);
    EditableTextLayout->ForceRefreshTextLayout(GetText());
}

TSharedPtr<SHLSLCodeEditableText> SHLSLCodeEditableText::Duplicate(bool AdvanceDisplay)
{
    return
    SNew(SHLSLCodeEditableText,GetAdditionTokens.IsBound()?GetAdditionTokens.Execute():TArray<HLSLSyntax::FRule>{},AdvanceDisplay)
    .HScrollBar(HScrollBar)
    .VScrollBar(VScrollBar)
    .GetAdditionTokens(GetAdditionTokens)
    .OnCodeSaved(OnCodeSavedCallback)
    .OnCodeChanged(OnTextChangedCallback)
    .OnCodeCommitted(OnTextCommittedCallback)
    .Text(GetText());
}

void SHLSLCodeEditableText::ResetDefaultHLSLTokens()
{
    DEFAULT_HLSL_TOKEN_RULES = nullptr;
}

TArray<HLSLSyntax::FRule> SHLSLCodeEditableText::MakeTokens(const TMap<HLSLSyntax::ETokenType, TArray<FString>>& Tokens)
{
    TArray<HLSLSyntax::FRule> NewTokens;
    for(const TPair<HLSLSyntax::ETokenType, TArray<FString>>& Kvp:Tokens)
    {
        for(const FString& Token:Kvp.Value)
        {
            NewTokens.Add(HLSLSyntax::FRule(Token, (uint32)Kvp.Key));
        }
    }
    return NewTokens;
}

FReply SHLSLCodeEditableText::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
    if(!HasKeyboardFocus())
    {
        FSlateApplication::Get().SetKeyboardFocus(SharedThis(this), EFocusCause::SetDirectly);
    }

    return SMultiLineEditableText::OnMouseButtonDoubleClick(InMyGeometry, InMouseEvent);
}

FReply SHLSLCodeEditableText::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyboardEvent)
{
    ClearComposingState();

    FReply Reply = FReply::Unhandled();

    const FKey Key = InKeyboardEvent.GetKey();

    if(Key==EKeys::F && InKeyboardEvent.IsControlDown())
    {
        return FReply::Unhandled();
    }

    if(Key==EKeys::S && InKeyboardEvent.IsControlDown())
    {
        OnCodeSavedCallback.ExecuteIfBound(GetText());
        return FReply::Handled();
    }

    return SMultiLineEditableText::OnKeyDown(MyGeometry, InKeyboardEvent);
}

FReply SHLSLCodeEditableText::OnKeyChar(const FGeometry& MyGeometry, const FCharacterEvent& InCharacterEvent)
{
    FReply Reply = FReply::Unhandled();

    const TCHAR Character = InCharacterEvent.GetCharacter();

    if(Character==TEXT('\t'))
    {
        if(!IsTextReadOnly())
        {
            FString String;
            String.AppendChar(Character);
            InsertTextAtCursor(String);
            Reply = FReply::Handled();
        }
        else
        {
            Reply = FReply::Unhandled();
        }
    }
    else
    {
//         TSharedPtr<const IRun> TextRun = EditableTextLayout->GetRunUnderCursor();
//         FString RunText = StaticCastSharedRef<const FHLSLTextRun>(TextRun.ToSharedRef())->GetText().Get();
//         FTextRange TextRange = TextRun->GetTextRange();
//         RunText = RunText.Mid(TextRange.BeginIndex, TextRange.EndIndex - TextRange.BeginIndex);
//         GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, *RunText);

        Reply = SMultiLineEditableText::OnKeyChar(MyGeometry, InCharacterEvent);
    }

    return Reply;
}

FReply SHLSLCodeEditableText::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
{
    ClearComposingState();
    return SMultiLineEditableText::OnFocusReceived(MyGeometry, InFocusEvent);
}

void SHLSLCodeEditableText::OnFocusLost(const FFocusEvent& InFocusEvent)
{
    ClearComposingState();
    return SMultiLineEditableText::OnFocusLost(InFocusEvent);
}

void SHLSLCodeEditableText::ClearComposingState()
{
	if (EditableTextLayout->GetTextInputMethodContext()->IsComposing())
		EditableTextLayout->GetTextInputMethodContext()->EndComposition();
}
