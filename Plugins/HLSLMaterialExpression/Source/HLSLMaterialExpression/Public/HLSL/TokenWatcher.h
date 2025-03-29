// Copyright 2023 ZSttt, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HLSLUtils.h"

class FHLSLTokenWatcher
{
    struct FToken
    {
        TArray<TWeakPtr<class SHLSLCodePalette>> CodePalettes;
        TMap<HLSLSyntax::ETokenType, TArray<FString>>Tokens;
    };
public:
    ~FHLSLTokenWatcher() {};
    FHLSLTokenWatcher() {};
    static TSharedPtr<FHLSLTokenWatcher> GetTokenWatcher();
public:
    void RegisterCodePalette(TWeakObjectPtr<class UMaterial> Material, TWeakPtr<class SHLSLCodePalette>CodePalette);
    void UnregisterCodePalette(TWeakObjectPtr<class UMaterial> Material, TWeakPtr<class SHLSLCodePalette>CodePalette);

    TArray<TWeakPtr<class SHLSLCodePalette>> GetCodePalettes(TWeakObjectPtr<class UMaterial> Material);
    TMap<HLSLSyntax::ETokenType, TArray<FString>> GetTokens(TWeakObjectPtr<class UMaterial> Material);

    TMap<HLSLSyntax::ETokenType, TArray<FString>> CollectHLSLTokens(const FString& Code);

    bool CompareAndUpdateTokens(TWeakObjectPtr<class UMaterial> Material, const TMap<HLSLSyntax::ETokenType, TArray<FString>>& NewTokens);
    bool RecollectTokens(TWeakObjectPtr<class UMaterial> Material);
    void UpdateCodePaletteTokens(TWeakObjectPtr<class UMaterial> Material);
private:
    TMap<TWeakObjectPtr<class UMaterial>, FToken>HLSLTokens;
};