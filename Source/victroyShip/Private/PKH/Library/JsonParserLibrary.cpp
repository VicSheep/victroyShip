// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Library/JsonParserLibrary.h"

void UJsonParserLibrary::ParseNPCResponse(const FString& Data, FNPCResponse& Response)
{
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(Data);
	TSharedPtr<FJsonObject> JsonObj = MakeShareable(new FJsonObject());

	// Deserialize JsonGetObjectField
	if (FJsonSerializer::Deserialize(JsonReader, JsonObj))
	{
		Response.Answer = JsonObj->GetStringField(TEXT("answer"));
		Response.Emotion = JsonObj->GetStringField(TEXT("emotion"));
		Response.FilePath = JsonObj->GetStringField(TEXT("file_path"));
		Response.Likeability = JsonObj->GetIntegerField(TEXT("likability"));
	}
}
