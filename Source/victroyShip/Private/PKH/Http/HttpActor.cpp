// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Http/HttpActor.h"

#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "PKH/Game/FarmLifeGameMode.h"
#include "PKH/Library/JsonParserLibrary.h"

#include "Sound/SoundWave.h"
#include "Audio.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/EditorBulkData.h"
#include "Sound/SoundWaveProcedural.h"

// Sets default values
AHttpActor::AHttpActor()
{
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AHttpActor::BeginPlay()
{
	Super::BeginPlay();

	MyGameMode = CastChecked<AFarmLifeGameMode>(GetWorld()->GetAuthGameMode());
}

#pragma region NPC conversation
void AHttpActor::SendSpeech(const FString& SpeechFileName, const FString& SpeechFilePath, const FString& NPCName)
{
	const FString& FullURL = BaseURL + EndPoint_SendSpeech;

	// HTTP Request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetURL(FullURL);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &AHttpActor::SendSpeechComplete);

	// 양식 주의할 것(웹 서버쪽의 양식과 정확하게 일치해야 함)
	FString JsonBody = FString::Printf(TEXT("{\"file_name\": \"%s\",\"file_path\" : \"%s\",\"npc_name\" : \"%s\"}"), *SpeechFileName, *SpeechFilePath, *NPCName);
	HttpRequest->SetContentAsString(JsonBody);

	HttpRequest->ProcessRequest();

	UE_LOG(LogTemp, Warning, TEXT("Send speech to %s"), *FullURL);
}

void AHttpActor::SendSpeechComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		UE_LOG(LogTemp, Warning, TEXT("Send Speech Complete"));
		ReqTextFromSpeech();
		//GetSpeechFile();
	}
	else
	{
		if (Request->GetStatus() == EHttpRequestStatus::Succeeded)
		{
			UE_LOG(LogTemp, Warning, TEXT("Response Failed...%d"), Response->GetResponseCode());
		}
	}
}

void AHttpActor::ReqTextFromSpeech()
{
	const FString& FullURL = BaseURL + EndPoint_GetSpeech;

	// HTTP Request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetURL(FullURL);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &AHttpActor::ReqTextFromSpeechComplete);

	HttpRequest->ProcessRequest();
	UE_LOG(LogTemp, Warning, TEXT("Req to %s"), *FullURL);
}

void AHttpActor::ReqTextFromSpeechComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		const FString& ResultText = Response->GetContentAsString();
		FString OutResponse;
		FString OutFilePath;
		UJsonParserLibrary::ParseNPCResponse(ResultText, OutResponse, OutFilePath);
		MyGameMode->SetLatestSpeech(OutResponse, OutFilePath);
	}
	else
	{
		if (Request->GetStatus() == EHttpRequestStatus::Succeeded)
		{
			UE_LOG(LogTemp, Warning, TEXT("Response Failed...%d"), Response->GetResponseCode());
		}
	}
}

//void AHttpActor::GetSpeechFile()
//{
//	const FString& FullURL = BaseURL + EndPoint_GetSpeech;
//
//	// HTTP Request
//	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
//	HttpRequest->SetVerb(TEXT("GET"));
//	HttpRequest->SetURL(FullURL);
//	HttpRequest->OnProcessRequestComplete().BindUObject(this, &AHttpActor::GetSpeechFileComplete);
//
//	HttpRequest->ProcessRequest();
//	UE_LOG(LogTemp, Warning, TEXT("Get speech from %s"), *FullURL);
//}
//
//void AHttpActor::GetSpeechFileComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
//{
//	if (bConnectedSuccessfully)
//	{
//		TArray<uint8> WavData = Response->GetContent(); UE_LOG(LogTemp, Warning, TEXT("Get speech Complete"));
//
//		USoundWaveProcedural* SoundWave = NewObject<USoundWaveProcedural>();
//
//		constexpr uint32 SampleRate = 44100;
//		constexpr uint32 NumChannels = 2;
//
//		// 사운드웨이브 설정
//		SoundWave->SetSampleRate(SampleRate);
//		SoundWave->NumChannels = 1;
//		SoundWave->Duration = static_cast<float>(WavData.Num()) / (SampleRate * NumChannels * sizeof(int16));
//
//		SoundWave->QueueAudio(WavData.GetData(), WavData.Num());
//
//		UGameplayStatics::PlaySound2D(GetWorld(), SoundWave);
//	}
//	else
//	{
//		if (Request->GetStatus() == EHttpRequestStatus::Succeeded)
//		{
//			UE_LOG(LogTemp, Warning, TEXT("Response Failed...%d"), Response->GetResponseCode());
//		}
//	}
//}
#pragma endregion

#pragma region NPC Conversation by Text
void AHttpActor::SendText(const FString& NPCName, const FString& InputText)
{
	const FString& FullURL = BaseURL + EndPoint_SendText;

	// HTTP Request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetURL(FullURL);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &AHttpActor::SendTextComplete);

	// 양식 주의할 것(웹 서버쪽의 양식과 정확하게 일치해야 함)
	FString JsonBody = FString::Printf(TEXT("{\"npc_name\": \"%s\",\"chat_text\" : \"%s\"}"), *NPCName, *InputText);
	HttpRequest->SetContentAsString(JsonBody);

	HttpRequest->ProcessRequest();

	UE_LOG(LogTemp, Warning, TEXT("Send text to %s"), *FullURL);
}

void AHttpActor::SendTextComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		UE_LOG(LogTemp, Warning, TEXT("Send text Complete"));
		GetText();
	}
	else
	{
		if (Request->GetStatus() == EHttpRequestStatus::Succeeded)
		{
			UE_LOG(LogTemp, Warning, TEXT("Response Failed...%d"), Response->GetResponseCode());
		}
	}
}

void AHttpActor::GetText()
{
	const FString& FullURL = BaseURL + EndPoint_GetText;

	// HTTP Request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetURL(FullURL);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &AHttpActor::GetTextComplete);

	HttpRequest->ProcessRequest();
	UE_LOG(LogTemp, Warning, TEXT("Req to %s"), *FullURL);
}

void AHttpActor::GetTextComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		const FString& ResultText = Response->GetContentAsString();
		FString OutResponse;
		FString OutFilePath;
		UJsonParserLibrary::ParseNPCResponse(ResultText, OutResponse, OutFilePath);
		MyGameMode->SetLatestSpeech(OutResponse, OutFilePath);
	}
	else
	{
		if (Request->GetStatus() == EHttpRequestStatus::Succeeded)
		{
			UE_LOG(LogTemp, Warning, TEXT("Response Failed...%d"), Response->GetResponseCode());
		}
	}
}
#pragma endregion

#pragma region Talk to Plant

void AHttpActor::TalkToPlant(const FString& SpeechFileName, const FString& SpeechFilePath)
{
	const FString& FullURL = BaseURL + EndPoint_TalkToPlant;

	// HTTP Request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetURL(FullURL);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &AHttpActor::TalkToPlantComplete);

	// 양식 주의할 것(웹 서버쪽의 양식과 정확하게 일치해야 함)
	FString JsonBody = FString::Printf(TEXT("{\"file_name\": \"%s\",\"file_path\" : \"%s\"}"), *SpeechFileName, *SpeechFilePath);
	HttpRequest->SetContentAsString(JsonBody);

	HttpRequest->ProcessRequest();

	UE_LOG(LogTemp, Warning, TEXT("Talk to plant : %s"), *FullURL);
}

void AHttpActor::TalkToPlantComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		UE_LOG(LogTemp, Warning, TEXT("Talk to plant Complete"));
		ReqScore();
	}
	else
	{
		if (Request->GetStatus() == EHttpRequestStatus::Succeeded)
		{
			UE_LOG(LogTemp, Warning, TEXT("Response Failed...%d"), Response->GetResponseCode());
		}
	}
}

void AHttpActor::ReqScore()
{
	const FString& FullURL = BaseURL + EndPoint_GetScore;

	// HTTP Request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetURL(FullURL);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &AHttpActor::ReqScoreComplete);

	HttpRequest->ProcessRequest();
	UE_LOG(LogTemp, Warning, TEXT("Req to %s"), *FullURL);
}

void AHttpActor::ReqScoreComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		const FString& ResultText = Response->GetContentAsString();
		UE_LOG(LogTemp, Warning, TEXT("Talk to plant score : %s"), *ResultText);
		MyGameMode->SetTalkScore(FCString::Atoi(*ResultText));
	}
	else
	{
		if (Request->GetStatus() == EHttpRequestStatus::Succeeded)
		{
			UE_LOG(LogTemp, Warning, TEXT("Response Failed...%d"), Response->GetResponseCode());
		}
	}
}
#pragma endregion