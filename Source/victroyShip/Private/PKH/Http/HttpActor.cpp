// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Http/HttpActor.h"

#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "PKH/Game/FarmLifeGameMode.h"
#include "PKH/Library/JsonParserLibrary.h"

#include "Sound/SoundWave.h"
#include "Audio.h"
#include "PKH/NPC/NPCBase.h"
#include "Serialization/EditorBulkData.h"

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
		UE_LOG(LogTemp, Warning, TEXT("Send Speech Complete"));
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

		// STT 결과를 UI에 전달하고, 챗봇 답변 요청
		MyGameMode->ShowPlayerText(ResultText.Mid(1, ResultText.Len() - 2));
		if(MyGameMode->GetCurNPC())
		{
			SendConv(MyGameMode->GetCurNPC()->GetNPCName(), MyGameMode->GetCurNPC()->GetLikeability());
		}
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

#pragma region NPC Conversation by Text
void AHttpActor::SendText(const FString& NPCName, const FString& InputText, int32 Preference)
{
	const FString& FullURL = BaseURL + EndPoint_SendText;

	// HTTP Request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetURL(FullURL);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &AHttpActor::SendTextComplete);

	// 양식 주의할 것(웹 서버쪽의 양식과 정확하게 일치해야 함)
	FString JsonBody = FString::Printf(TEXT("{\"npc_name\": \"%s\",\"chat_text\" : \"%s\",\"preference\" : \"%d\"}"), *NPCName, *InputText, Preference);
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
		FNPCResponse NPCResponse;
		UJsonParserLibrary::ParseNPCResponse(ResultText, NPCResponse);
		MyGameMode->SetLatestSpeech(NPCResponse);

		GetTTS();
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

#pragma region ChatBot Response
void AHttpActor::SendConv(const FString& NPCName, int32 Preference)
{
	const FString& FullURL = BaseURL + EndPoint_SendConv;

	// HTTP Request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetURL(FullURL);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &AHttpActor::SendConvComplete);

	// 양식 주의할 것(웹 서버쪽의 양식과 정확하게 일치해야 함)
	FString JsonBody = FString::Printf(TEXT("{\"npc_name\": \"%s\",\"preference\": \"%d\"}"), *NPCName, Preference);
	HttpRequest->SetContentAsString(JsonBody);

	HttpRequest->ProcessRequest();

	UE_LOG(LogTemp, Warning, TEXT("Send speech to %s"), *FullURL);
}

void AHttpActor::SendConvComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		UE_LOG(LogTemp, Warning, TEXT("Send Conv Complete"));
		GetConv();
	}
	else
	{
		if (Request->GetStatus() == EHttpRequestStatus::Succeeded)
		{
			UE_LOG(LogTemp, Warning, TEXT("Response Failed...%d"), Response->GetResponseCode());
		}
	}
}

void AHttpActor::GetConv()
{
	const FString& FullURL = BaseURL + EndPoint_GetConv;

	// HTTP Request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetURL(FullURL);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &AHttpActor::GetConvComplete);

	HttpRequest->ProcessRequest();
	UE_LOG(LogTemp, Warning, TEXT("Req to %s"), *FullURL);
}

void AHttpActor::GetConvComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		const FString& ResultText = Response->GetContentAsString();
		FNPCResponse NPCResponse;
		UJsonParserLibrary::ParseNPCResponse(ResultText, NPCResponse);

		// 챗봇 답변을 UI에 전달하고, TTS 요청
		MyGameMode->SetLatestSpeech(NPCResponse);
		GetTTS();
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

#pragma region TTS
void AHttpActor::GetTTS()
{
	const FString& FullURL = BaseURL + EndPoint_GetTTS;

	// HTTP Request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetURL(FullURL);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &AHttpActor::GetTTSComplete);

	HttpRequest->ProcessRequest();
	UE_LOG(LogTemp, Warning, TEXT("Req to %s"), *FullURL);
}

void AHttpActor::GetTTSComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		const FString& ResultText = Response->GetContentAsString();

		// TTS 출력
		MyGameMode->PlayTTS(ResultText.Mid(1, ResultText.Len() - 2));
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

#pragma region End Chat
void AHttpActor::EndChat(const FString& NPCName)
{
	const FString& FullURL = BaseURL + EndPoint_EndChat;

	// HTTP Request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetURL(FullURL);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &AHttpActor::EndChatComplete);

	// 양식 주의할 것(웹 서버쪽의 양식과 정확하게 일치해야 함)
	FString JsonBody = FString::Printf(TEXT("{\"npc_name\": \"%s\"}"), *NPCName);
	HttpRequest->SetContentAsString(JsonBody);

	HttpRequest->ProcessRequest();

	UE_LOG(LogTemp, Warning, TEXT("Request End Chat: %s"), *FullURL);
}

void AHttpActor::EndChatComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		UE_LOG(LogTemp, Warning, TEXT("Request End Chat Complete"));
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
	const FString& FullURL = BaseURL + EndPoint_GetSpeechScore;

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

#pragma region Talk to Plant with Text
void AHttpActor::TalkToPlantWithText(const FString& InputText)
{
	const FString& FullURL = BaseURL + EndPoint_TextToPlant;

	// HTTP Request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetURL(FullURL);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &AHttpActor::TalkToPlantWithTextComplete);

	// 양식 주의할 것(웹 서버쪽의 양식과 정확하게 일치해야 함)
	FString JsonBody = FString::Printf(TEXT("{\"chat_text\" : \"%s\"}"), *InputText);
	HttpRequest->SetContentAsString(JsonBody);

	HttpRequest->ProcessRequest();

	UE_LOG(LogTemp, Warning, TEXT("Talk to plant : %s"), *FullURL);
}

void AHttpActor::TalkToPlantWithTextComplete(FHttpRequestPtr Request, FHttpResponsePtr Response,
	bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		UE_LOG(LogTemp, Warning, TEXT("Talk to plant Complete"));
		ReqScoreWithText();
	}
	else
	{
		if (Request->GetStatus() == EHttpRequestStatus::Succeeded)
		{
			UE_LOG(LogTemp, Warning, TEXT("Response Failed...%d"), Response->GetResponseCode());
		}
	}
}

void AHttpActor::ReqScoreWithText()
{
	const FString& FullURL = BaseURL + EndPoint_GetTextScore;

	// HTTP Request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetURL(FullURL);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &AHttpActor::ReqScoreWithTextComplete);

	HttpRequest->ProcessRequest();
	UE_LOG(LogTemp, Warning, TEXT("Req to %s"), *FullURL);
}

void AHttpActor::ReqScoreWithTextComplete(FHttpRequestPtr Request, FHttpResponsePtr Response,
	bool bConnectedSuccessfully)
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
