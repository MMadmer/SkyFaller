// Copyright 2023 ZSttt, Inc. All Rights Reserved.

#include "HLSL/TokenWatcher.h"
#include "HLSL/HLSLParser.h"
#include "Widgets/SHLSLCodePalette.h"

static TSharedPtr<FHLSLTokenWatcher> TokenWatcherSingleton = nullptr;
TSharedPtr<FHLSLTokenWatcher> FHLSLTokenWatcher::GetTokenWatcher()
{
    if(!TokenWatcherSingleton||!TokenWatcherSingleton.IsValid())
        TokenWatcherSingleton = MakeShareable(new FHLSLTokenWatcher());
    return TokenWatcherSingleton;
}

void FHLSLTokenWatcher::RegisterCodePalette(TWeakObjectPtr<class UMaterial> Material, TWeakPtr<class SHLSLCodePalette>CodePalette)
{
    HLSLTokens.FindOrAdd(Material).CodePalettes.AddUnique(CodePalette);
}

void FHLSLTokenWatcher::UnregisterCodePalette(TWeakObjectPtr<class UMaterial> Material, TWeakPtr<class SHLSLCodePalette>CodePalette)
{
    FToken* Token = HLSLTokens.Find(Material);
    Token->CodePalettes.Remove(CodePalette);
    if(Token->CodePalettes.Num()==0)
        HLSLTokens.Remove(Material);
}

TArray<TWeakPtr<class SHLSLCodePalette>> FHLSLTokenWatcher::GetCodePalettes(TWeakObjectPtr<class UMaterial> Material)
{
    if(FToken* Token = HLSLTokens.Find(Material))
        return Token->CodePalettes;
    return TArray<TWeakPtr<class SHLSLCodePalette>>{};
}

TMap<HLSLSyntax::ETokenType, TArray<FString>> FHLSLTokenWatcher::GetTokens(TWeakObjectPtr<class UMaterial> Material)
{
    if(FToken* Token = HLSLTokens.Find(Material))
        return Token->Tokens;
    return TMap<HLSLSyntax::ETokenType, TArray<FString>>{};
}

TMap<HLSLSyntax::ETokenType, TArray<FString>> FHLSLTokenWatcher::CollectHLSLTokens(const FString& Code)
{
    using ETokenType = HLSLSyntax::ETokenType;

    TMap<ETokenType, TArray<FString>> MatchingTokens;
    FHLSLParser::ParseTokens(Code,
        MatchingTokens.FindOrAdd(ETokenType::Struct),
        MatchingTokens.FindOrAdd(ETokenType::Define),
        MatchingTokens.FindOrAdd(ETokenType::Function), false, false);
    return MatchingTokens;
}

bool FHLSLTokenWatcher::CompareAndUpdateTokens(TWeakObjectPtr<class UMaterial> Material, const TMap<HLSLSyntax::ETokenType, TArray<FString>>& NewTokens)
{
    FToken* MatchingTokens = HLSLTokens.Find(Material);
    if(!MatchingTokens)
    {
        FToken New;
        New.Tokens = NewTokens;

        HLSLTokens.Add(Material, New);
        for(const TPair<HLSLSyntax::ETokenType, TArray<FString>>&Kvp:NewTokens)
        {
            if(Kvp.Value.Num()>0)
                return true;
        }
        return false;
    }
    else
    {
        bool bDirty = false;
        for(const TPair<HLSLSyntax::ETokenType, TArray<FString>>& Kvp:NewTokens)
        {
            if(Kvp.Value.Num() == 0)
                continue;

            TArray<FString>*Tokens = MatchingTokens->Tokens.Find(Kvp.Key);
            if(Kvp.Value.Num()!=Tokens->Num())
            {
                bDirty = true;
                break;
            }
            else
            {
                for(auto It = Kvp.Value.CreateConstIterator(); It; ++It)
                {
                    bool bFound = ((*Tokens).Find(*It)!=INDEX_NONE);
                    if(!bFound)
                    {
                        bDirty = true;
                        break;
                    }
                }
            }
        }

        if(bDirty)
            RecollectTokens(Material);
        return bDirty;
    }
}

bool FHLSLTokenWatcher::RecollectTokens(TWeakObjectPtr<class UMaterial> Material)
{
	if (FToken* Token = HLSLTokens.Find(Material))
	{
		TMap<HLSLSyntax::ETokenType, TArray<FString>> MatchingTokens;

		for (TWeakPtr<SHLSLCodePalette>& CodePalette : Token->CodePalettes)
		{
			FString Code = CodePalette.Pin()->GetCode();
			TMap<HLSLSyntax::ETokenType, TArray<FString>> Tokens = CollectHLSLTokens(Code);
            for (TPair<HLSLSyntax::ETokenType, TArray<FString>>& Kvp : Tokens)
            {
                MatchingTokens.FindOrAdd(Kvp.Key).Append(Kvp.Value);
            }
		}

        Token->Tokens = MatchingTokens;

		return true;
	}

    return false;
}


void FHLSLTokenWatcher::UpdateCodePaletteTokens(TWeakObjectPtr<class UMaterial> Material)
{
    TMap<HLSLSyntax::ETokenType, TArray<FString>> AdditionTokens = GetTokens(Material);
    TArray<TWeakPtr<SHLSLCodePalette>> Palettes = GetCodePalettes(Material);
    for(TWeakPtr<SHLSLCodePalette> Palette:Palettes)
    {
        Palette.Pin()->UpdateCodeTokens(AdditionTokens);
    }
}
