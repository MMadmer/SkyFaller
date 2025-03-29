// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class FHLSLExporter
{
public:
	~FHLSLExporter();
	static void Register();
	static void SaveAsMaterialFunction(class UHLSLMaterialExpression* HLSLExpression);
protected:
	FHLSLExporter();
//private:
	//TArray<TWeakPtr<class IMaterialEditor>>MaterialEditors;
};
