// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Library/JsonParserLibrary.h"

void UJsonParserLibrary::ParseNPCResponse(const FString& Data, FString& OutResponse, FString& OutPath)
{
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(Data);
	TSharedPtr<FJsonObject> JsonObj = MakeShareable(new FJsonObject());

	// Deserialize JsonGetObjectField
	if (FJsonSerializer::Deserialize(JsonReader, JsonObj))
	{
		OutResponse = JsonObj->GetStringField(TEXT("response"));
		OutPath = JsonObj->GetStringField(TEXT("wavPath"));
	}
}
