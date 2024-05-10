// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Http/HttpActor.h"

#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "PKH/Game/FarmLifeGameMode.h"

// Sets default values
AHttpActor::AHttpActor()
{
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AHttpActor::BeginPlay()
{
	Super::BeginPlay();

	GameMode = CastChecked<AFarmLifeGameMode>(GetWorld()->GetAuthGameMode());
}

void AHttpActor::SendSpeech(const FString& SpeechFileName, const FString& SpeechFilePath)
{
	const FString& FullURL = BaseURL + EndPoint_SendSpeech;

	// HTTP Request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetURL(FullURL);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &AHttpActor::SendSpeechComplete);

	// 양식 주의할 것(웹 서버쪽의 양식과 정확하게 일치해야 함)
	FString JsonBody = FString::Printf(TEXT("{\"file_name\": \"%s\",\"file_path\" : \"%s\"}"), *SpeechFileName, *SpeechFilePath);
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
		UE_LOG(LogTemp, Warning, TEXT("ReqTest Complete : %s"), *ResultText);
	}
	else
	{
		if (Request->GetStatus() == EHttpRequestStatus::Succeeded)
		{
			UE_LOG(LogTemp, Warning, TEXT("Response Failed...%d"), Response->GetResponseCode());
		}
	}
}
