// Copyright 2023 ZSttt, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace HLSLConfig
{
    struct FKeywordsColor
    {
        TArray<TCHAR*> Keywords;
        FColor Color;
    };

    struct FHLSLCodeViewer
    {
        FVector2D ViewerSize = FVector2D::ZeroVector;
        bool bMaximize = false;
    };

    struct FConfiguration
    {
        bool bShowColors = true;
        FHLSLCodeViewer HLSLViewer;
        FColor NormalColor = FColor::White;
        FColor NumberColor = FColor::White;
        FColor CommentColor = FColor::White;
        FColor PreProcessorsColor = FColor::White;
        FColor OperatorsColor = FColor::White;
        FColor KeywordsColor = FColor::White;
        FColor StringsColor = FColor::White;
        FColor FunctionsColor = FColor::White;
        FColor StructsColor = FColor::White;
        FColor DefinesColor = FColor::White;
        TArray<FString> PreProcessors;
        TArray<FString> Operators;
        TArray<FString> Keywords;
        TArray<FString> Structs;
        TArray<FString> Defines;
        TArray<FString> Functions;

        void CopyColors(const FConfiguration& Dest)
        {
            NormalColor = Dest.NormalColor;
            NumberColor = Dest.NumberColor;
            CommentColor = Dest.CommentColor;
            PreProcessorsColor = Dest.PreProcessorsColor;
            OperatorsColor = Dest.OperatorsColor;
            KeywordsColor = Dest.KeywordsColor;
            StringsColor = Dest.StringsColor;
            FunctionsColor = Dest.FunctionsColor;
            StructsColor = Dest.StructsColor;
            DefinesColor = Dest.DefinesColor;
        }
    };
    TSharedPtr<FConfiguration> GetConfiguration();

    FString GetStyleSetupFile();
    
    FString GetTokensSetupFile();
    
    void SaveStyles();
    
    void SaveTokens();
    
    void SaveJson(TSharedPtr<class FJsonObject> JsonObj, const FString File);
    
    void LoadConfig();

    FConfiguration GetDefaultConfiguration();
}