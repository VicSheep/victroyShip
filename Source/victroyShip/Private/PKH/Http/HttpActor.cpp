// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Http/HttpActor.h"

#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "PKH/Game/FarmLifeGameMode.h"
#include "PKH/Library/JsonParserLibrary.h"

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
		UE_LOG(LogTemp, Warning, TEXT("Ask Complete"));
		ReqTextFromSpeech();
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
	const FString& FullURL = BaseURL + EndPoint_GetText;

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