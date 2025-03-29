// Copyright 2023 ZSttt, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HLSLUtils.h"
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 2
#include "Materials/MaterialExpressionCustom.h"
#else
#include "Engine/Classes/Materials/MaterialExpressionCustom.h"
#endif
#include "Expression.generated.h"

USTRUCT(BlueprintType)
struct FHLSLInput
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, Category=HLSLExpression)
        FName InputName = TEXT("Param");

    UPROPERTY(EditAnywhere, Category=HLSLExpression)
        EHLSLInputType InputType;

    UPROPERTY() 
        FExpressionInput Input;
};

USTRUCT(BlueprintType)
struct FHLSLOutput
{
    GENERATED_USTRUCT_BODY()

    FHLSLOutput() {};

    FHLSLOutput(const FName& InName, const EHLSLOutputType InType) : OutputName(InName), OutputType(InType) {};
    UPROPERTY(EditAnywhere, Category = HLSLExpression)
        FName OutputName = TEXT("Output");

    UPROPERTY(EditAnywhere, Category = HLSLExpression)
        EHLSLOutputType OutputType;
};

UCLASS(BlueprintType)
class UHLSLMaterialExpression : public UMaterialExpression
{
    GENERATED_UCLASS_BODY()
    DECLARE_DELEGATE(FUpdateGraphCode);
public:
    UPROPERTY()
        FHLSLExpressions Expression;

    UPROPERTY(EditAnywhere, Category = HLSLExpression)
        FString Description;

    UPROPERTY()
        FHLSLCode HLSLCode;

    UPROPERTY(EditAnywhere, Category=HLSLExpression)
        EHLSLShaderFrequency ShaderFrequency;

    UPROPERTY(EditAnywhere, Category = HLSLExpression)
        TArray<FHLSLInput> HLSLInputs;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HLSLExpression, meta = (EditCondition = "ShaderFrequency == EHLSLShaderFrequency::HSF_Pixel", EditConditionHides))
        TArray<FHLSLOutput> HLSLOutputs;

    FUpdateGraphCode UpdateGraphCodeCallback;
public:
    UPROPERTY()
        class UMaterialExpressionCustom* HLSLMain = nullptr;

    UPROPERTY()
        class UMaterialExpressionCustom* HLSLFunctions = nullptr;
public:
#if WITH_EDITOR
    bool FixupExpressionArguments(bool bRebuild);
    void SetCode(const FString& NewCode) { HLSLCode.Code = NewCode; };
    FString GetCode() { return HLSLCode.Code; };

    void SetCodePalettePosition(const FIntPoint Pos) { HLSLCode.Geometry.X = Pos.X; HLSLCode.Geometry.Y = Pos.Y; };
    void SetCodePaletteSize(const FIntPoint Size) { HLSLCode.Geometry.Z = Size.X; HLSLCode.Geometry.W = Size.Y; };
    FIntPoint GetCodePalettePosition() { return FIntPoint(HLSLCode.Geometry.X, HLSLCode.Geometry.Y); };
    FIntPoint GetCodePaletteSize() { return FIntPoint(HLSLCode.Geometry.Z, HLSLCode.Geometry.W); };

    void SetAutomaticCompilation(bool bAuto) { HLSLCode.bAutoCompile = bAuto; };
    bool IsAutomaticCompilation() { return HLSLCode.bAutoCompile; };
#endif

    void SetExpressions(const FHLSLExpressions& NewExpression);
public:
    FString GetConditionDefaultOutputName();
    FString GetConditionDefaultOutputCode();
    FString GetConditionOutputTypeName();
    EHLSLOutputType GetConditionHLSLOutputType();
    ECustomMaterialOutputType GetConditionOutputType();
    TArray<FHLSLOutput> GetConditionOutputs();
protected:
    void CreateExpressionIfNone(bool bForceCreate);
    void ResetDefault();
    void RebuildOutputs();
    void GenerateHLSLExpressions();
    void UpdateExpression();
public:
    virtual void Serialize(FStructuredArchive::FRecord Record) override;
    virtual void PostInitProperties() override;
    virtual void PostLoad() override;
#if WITH_EDITOR
    virtual void PostEditImport() override;
    virtual bool CanEditChange(const FProperty* InProperty) const override;
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    void PreCompile();
    virtual int32 Compile(class FMaterialCompiler* Compiler, int32 OutputIndex) override;
    virtual void GetCaption(TArray<FString>& OutCaptions) const override;
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 2
    virtual TArrayView<FExpressionInput*> GetInputsView()override;
#else
    virtual const TArray<FExpressionInput*> GetInputs() override;
#endif
    virtual FExpressionInput* GetInput(int32 InputIndex) override;
    virtual FName GetInputName(int32 InputIndex) const override;
    virtual uint32 GetInputType(int32 InputIndex) override;
    virtual TArray<FExpressionOutput>& GetOutputs();
    virtual uint32 GetOutputType(int32 OutputIndex) override;
    virtual bool IsResultMaterialAttributes(int32 OutputIndex) override;
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 0
    virtual bool GenerateHLSLExpression(FMaterialHLSLGenerator& Generator, UE::HLSLTree::FScope& Scope, int32 OutputIndex, UE::HLSLTree::FExpression const*& OutExpression) const override;
#endif
#endif
}; 