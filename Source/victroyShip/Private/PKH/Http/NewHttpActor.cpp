

#include "PKH/Http/NewHttpActor.h"

#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PKH/Game/FarmLifeGameMode.h"
#include "PKH/Library/JsonParserLibrary.h"
#include "PKH/NPC/NPCBase.h"

ANewHttpActor::ANewHttpActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ANewHttpActor::BeginPlay()
{
	Super::BeginPlay();

	if (bRunAsLocal)
	{
		BaseURL = LocalURL;
	}

	ExtraPath = UKismetSystemLibrary::GetProjectDirectory() + ExtraFolder;
	MyGameMode = CastChecked<AFarmLifeGameMode>(GetWorld()->GetAuthGameMode());
}

#pragma region NPC conversation
void ANewHttpActor::SendSpeech(const FString& FilePath)
{
	const FString& FullURL = BaseURL + EndPoint_SendSpeech;

	// Read file form path
	TArray<uint8> FileContent;
	if (!FFileHelper::LoadFileToArray(FileContent, *FilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load file %s"), *FilePath);
		return;
	}

	// Set Boundary Value
	FString Boundary = TEXT("BoundaryString123456789012345678901234");
	FString BeginBoundary = TEXT("--") + Boundary + TEXT("\r\n");
	FString EndBoundary = TEXT("\r\n--") + Boundary + TEXT("--\r\n");

	FString Header = BeginBoundary;
	Header.Append(TEXT("Content-Disposition: form-data; name=\"file\"; filename=\""));
	Header.Append(FPaths::GetCleanFilename(FilePath));
	Header.Append(TEXT("\"\r\n"));
	Header.Append(TEXT("Content-Type: audio/wav\r\n\r\n"));

	TArray<uint8> Payload;
	Payload.Append(reinterpret_cast<uint8*>(TCHAR_TO_UTF8(*Header)), Header.Len());
	Payload.Append(FileContent);
	Payload.Append(reinterpret_cast<uint8*>(TCHAR_TO_UTF8(*EndBoundary)), EndBoundary.Len());

	// HTTP Request
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetVerb(TEXT("POST"));
	Request->SetURL(FullURL);
	Request->SetHeader("Content-Type", FString("multipart/form-data; boundary=") + Boundary);
	Request->SetContent(Payload);

	Request->OnProcessRequestComplete().BindUObject(this, &ANewHttpActor::SendSpeechComplete);
	Request->ProcessRequest();

	UE_LOG(LogTemp, Warning, TEXT("Send speech to %s"), *FullURL);
}

void ANewHttpActor::SendSpeechComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
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

void ANewHttpActor::ReqTextFromSpeech()
{
	const FString& FullURL = BaseURL + EndPoint_GetSpeech;

	// HTTP Request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetURL(FullURL);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &ANewHttpActor::ReqTextFromSpeechComplete);

	HttpRequest->ProcessRequest();
	UE_LOG(LogTemp, Warning, TEXT("Req to %s"), *FullURL);
}

void ANewHttpActor::ReqTextFromSpeechComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		const FString& ResultText = Response->GetContentAsString();

		// STT 결과를 UI에 전달하고, 챗봇 답변 요청
		MyGameMode->ShowPlayerText(ResultText.Mid(1, ResultText.Len() - 2));
		if (MyGameMode->GetCurNPC())
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
void ANewHttpActor::SendText(const FString& NPCName, const FString& InputText, int32 Preference)
{
	const FString& FullURL = BaseURL + EndPoint_SendText;

	// HTTP Request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetURL(FullURL);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &ANewHttpActor::SendTextComplete);

	// 양식 주의할 것(웹 서버쪽의 양식과 정확하게 일치해야 함)
	FString JsonBody = FString::Printf(TEXT("{\"npc_name\": \"%s\",\"chat_text\" : \"%s\",\"likeability\" : \"%d\"}"), *NPCName, *InputText, Preference);
	HttpRequest->SetContentAsString(JsonBody);

	HttpRequest->ProcessRequest();

	UE_LOG(LogTemp, Warning, TEXT("Send text to %s"), *FullURL);
}

void ANewHttpActor::SendTextComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
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

void ANewHttpActor::GetText()
{
	const FString& FullURL = BaseURL + EndPoint_GetText;

	// HTTP Request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetURL(FullURL);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &ANewHttpActor::GetTextComplete);

	HttpRequest->ProcessRequest();
	UE_LOG(LogTemp, Warning, TEXT("Req to %s"), *FullURL);
}

void ANewHttpActor::GetTextComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
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
void ANewHttpActor::SendConv(const FString& NPCName, int32 Preference)
{
	const FString& FullURL = BaseURL + EndPoint_SendConv;

	// HTTP Request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetURL(FullURL);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &ANewHttpActor::SendConvComplete);

	// 양식 주의할 것(웹 서버쪽의 양식과 정확하게 일치해야 함)
	FString JsonBody = FString::Printf(TEXT("{\"npc_name\": \"%s\",\"likeability\": \"%d\"}"), *NPCName, Preference);
	HttpRequest->SetContentAsString(JsonBody);

	HttpRequest->ProcessRequest();

	UE_LOG(LogTemp, Warning, TEXT("Send speech to %s"), *FullURL);
}

void ANewHttpActor::SendConvComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
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

void ANewHttpActor::GetConv()
{
	const FString& FullURL = BaseURL + EndPoint_GetConv;

	// HTTP Request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetURL(FullURL);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &ANewHttpActor::GetConvComplete);

	HttpRequest->ProcessRequest();
	UE_LOG(LogTemp, Warning, TEXT("Req to %s"), *FullURL);
}

void ANewHttpActor::GetConvComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
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
void ANewHttpActor::GetTTS()
{
	const FString& FullURL = BaseURL + EndPoint_GetTTS;

	// HTTP Request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetURL(FullURL);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &ANewHttpActor::GetTTSComplete);

	HttpRequest->ProcessRequest();
	UE_LOG(LogTemp, Warning, TEXT("Req to %s"), *FullURL);
}

void ANewHttpActor::GetTTSComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		const TArray<uint8>& Data = Response->GetContent();

		FString FilePath = UKismetSystemLibrary::GetProjectDirectory() + TEXT("TTSFile.wav");
		FFileHelper::SaveArrayToFile(Data, *FilePath);
		UE_LOG(LogTemp, Warning, TEXT("WAV file saved to %s"), *FilePath);

		// Play TTS 
		MyGameMode->PlayTTS(FilePath);
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

#pragma region Greeting
void ANewHttpActor::InitGreeting(const FString& NPCName, int32 Likeability)
{
	const FString& FullURL = BaseURL + EndPoint_InitGreeting;

	// HTTP Request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetURL(FullURL);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &ANewHttpActor::InitGreetingComplete);

	// 양식 주의할 것(웹 서버쪽의 양식과 정확하게 일치해야 함)
	FString JsonBody = FString::Printf(TEXT("{\"npc_name\": \"%s\",\"likeability\": \"%d\"}"), *NPCName, Likeability);
	HttpRequest->SetContentAsString(JsonBody);

	HttpRequest->ProcessRequest();

	UE_LOG(LogTemp, Warning, TEXT("Init Greeting: %s"), *NPCName);
}

void ANewHttpActor::InitGreetingComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		UE_LOG(LogTemp, Warning, TEXT("Init Greeting Complete"));
	}
	else
	{
		if (Request->GetStatus() == EHttpRequestStatus::Succeeded)
		{
			UE_LOG(LogTemp, Warning, TEXT("Response Failed...%d"), Response->GetResponseCode());
		}
	}
}

void ANewHttpActor::RequestGreeting(const FString& NPCName)
{
	const FString& FullURL = BaseURL + EndPoint_PostGreeting;

	// HTTP Request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetURL(FullURL);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &ANewHttpActor::RequestGreetingComplete);

	// 양식 주의할 것(웹 서버쪽의 양식과 정확하게 일치해야 함)
	FString JsonBody = FString::Printf(TEXT("{\"npc_name\": \"%s\"}"), *NPCName);
	HttpRequest->SetContentAsString(JsonBody);

	HttpRequest->ProcessRequest();

	UE_LOG(LogTemp, Warning, TEXT("Request Greeting to %s"), *FullURL);
}

void ANewHttpActor::RequestGreetingComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		UE_LOG(LogTemp, Warning, TEXT("Request Greeting Complete"));
		GetGreetingData();
	}
	else
	{
		if (Request->GetStatus() == EHttpRequestStatus::Succeeded)
		{
			UE_LOG(LogTemp, Warning, TEXT("Response Failed...%d"), Response->GetResponseCode());
		}
	}
}

void ANewHttpActor::GetGreetingData()
{
	const FString& FullURL = BaseURL + EndPoint_GetGreetingData;

	// HTTP Request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetURL(FullURL);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &ANewHttpActor::GetGreetingDataComplete);

	HttpRequest->ProcessRequest();
	UE_LOG(LogTemp, Warning, TEXT("Get Greeting Data From %s"), *FullURL);
}

void ANewHttpActor::GetGreetingDataComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		const FString& ResultText = Response->GetContentAsString();
		FNPCResponse NPCResponse;
		UJsonParserLibrary::ParseNPCResponse(ResultText, NPCResponse);
		UE_LOG(LogTemp, Warning, TEXT("Get Greeting Data Complete: %s"), *NPCResponse.Answer);

		// 경로 수정
		MyGameMode->GreetingToPlayer(NPCResponse);
		GetGreetingTTS();
	}
	else
	{
		if (Request->GetStatus() == EHttpRequestStatus::Succeeded)
		{
			UE_LOG(LogTemp, Warning, TEXT("Response Failed...%d"), Response->GetResponseCode());
		}
	}
}

void ANewHttpActor::GetGreetingTTS()
{
	const FString& FullURL = BaseURL + EndPoint_GetGreetingTTS;

	// HTTP Request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetURL(FullURL);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &ANewHttpActor::GetGreetingTTSComplete);

	HttpRequest->ProcessRequest();
	UE_LOG(LogTemp, Warning, TEXT("Get Greeting TTS From %s"), *FullURL);
}

void ANewHttpActor::GetGreetingTTSComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		const TArray<uint8>& Data = Response->GetContent();

		const FString FilePath = UKismetSystemLibrary::GetProjectDirectory() + TEXT("Extras/WavFiles/TTSFile.wav");
		FFileHelper::SaveArrayToFile(Data, *FilePath);
		UE_LOG(LogTemp, Warning, TEXT("Save Greeting TTS to %s "), *FilePath);

		MyGameMode->PlayTTS(FilePath);
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

#pragma region Present
void ANewHttpActor::RequestPresent(const FString& NPCName, const FString& ItemName,int32 Likeability, bool IsPrefer)
{
	const FString& FullURL = BaseURL + EndPoint_PostPresent;

	// HTTP Request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetURL(FullURL);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &ANewHttpActor::RequestPresentComplete);

	// 양식 주의할 것(웹 서버쪽의 양식과 정확하게 일치해야 함)
	FString JsonBody = FString::Printf(TEXT("{\"npc_name\": \"%s\",\"item\": \"%s\",\"likeability\": \"%d\",\"prefer\": %s}"), 
										*NPCName, *ItemName, Likeability, IsPrefer ? TEXT("true") : TEXT("false"));
	HttpRequest->SetContentAsString(JsonBody);

	HttpRequest->ProcessRequest();

	UE_LOG(LogTemp, Warning, TEXT("Request Present Response to %s"), *FullURL);
}

void ANewHttpActor::RequestPresentComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		UE_LOG(LogTemp, Warning, TEXT("Request Present Response Complete"));
		GetPresentData();
	}
	else
	{
		if (Request->GetStatus() == EHttpRequestStatus::Succeeded)
		{
			UE_LOG(LogTemp, Warning, TEXT("Response Failed...%d"), Response->GetResponseCode());
		}
	}
}

void ANewHttpActor::GetPresentData()
{
	const FString& FullURL = BaseURL + EndPoint_GetPresentData;

	// HTTP Request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetURL(FullURL);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &ANewHttpActor::GetPresentDataComplete);

	HttpRequest->ProcessRequest();
	UE_LOG(LogTemp, Warning, TEXT("Get Present Data From %s"), *FullURL);
}

void ANewHttpActor::GetPresentDataComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		const FString& ResultText = Response->GetContentAsString();
		FNPCResponse NPCResponse;
		UJsonParserLibrary::ParseNPCResponse(ResultText, NPCResponse);
		UE_LOG(LogTemp, Warning, TEXT("Get Present Data Complete: %s"), *NPCResponse.Answer);

		// 경로 수정
		MyGameMode->ResponseToPlayerForPresent(NPCResponse);
		GetPresentTTS();
	}
	else
	{
		if (Request->GetStatus() == EHttpRequestStatus::Succeeded)
		{
			UE_LOG(LogTemp, Warning, TEXT("Response Failed...%d"), Response->GetResponseCode());
		}
	}
}

void ANewHttpActor::GetPresentTTS()
{
	const FString& FullURL = BaseURL + EndPoint_GetPresentTTS;

	// HTTP Request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetURL(FullURL);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &ANewHttpActor::GetPresentTTSComplete);

	HttpRequest->ProcessRequest();
	UE_LOG(LogTemp, Warning, TEXT("Get Present TTS From %s"), *FullURL);
}

void ANewHttpActor::GetPresentTTSComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		const TArray<uint8>& Data = Response->GetContent();

		const FString FilePath = UKismetSystemLibrary::GetProjectDirectory() + TEXT("Extras/WavFiles/TTSFile.wav");
		FFileHelper::SaveArrayToFile(Data, *FilePath);
		UE_LOG(LogTemp, Warning, TEXT("Save Present TTS to %s "), *FilePath);

		MyGameMode->PlayTTS(FilePath);
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
void ANewHttpActor::EndChat(const FString& NPCName)
{
	const FString& FullURL = BaseURL + EndPoint_EndChat;

	// HTTP Request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetURL(FullURL);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &ANewHttpActor::EndChatComplete);

	// 양식 주의할 것(웹 서버쪽의 양식과 정확하게 일치해야 함)
	FString JsonBody = FString::Printf(TEXT("{\"npc_name\": \"%s\"}"), *NPCName);
	HttpRequest->SetContentAsString(JsonBody);

	HttpRequest->ProcessRequest();

	UE_LOG(LogTemp, Warning, TEXT("Request End Chat: %s"), *FullURL);
}

void ANewHttpActor::EndChatComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
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
void ANewHttpActor::TalkToPlant(const FString& FilePath)
{
	const FString& FullURL = BaseURL + EndPoint_TalkToPlant;

	TArray<uint8> FileContent;
	if (!FFileHelper::LoadFileToArray(FileContent, *FilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load file %s"), *FilePath);
		return;
	}

	// Set Boundary Value
	FString Boundary = TEXT("BoundaryString123456789012345678901234");
	FString BeginBoundary = TEXT("--") + Boundary + TEXT("\r\n");
	FString EndBoundary = TEXT("\r\n--") + Boundary + TEXT("--\r\n");

	// Set Header
	FString Header = BeginBoundary;
	Header.Append(TEXT("Content-Disposition: form-data; name=\"file\"; filename=\""));
	Header.Append(FPaths::GetCleanFilename(FilePath));
	Header.Append(TEXT("\"\r\n"));
	Header.Append(TEXT("Content-Type: audio/wav\r\n\r\n"));

	TArray<uint8> Payload;
	Payload.Append(reinterpret_cast<uint8*>(TCHAR_TO_UTF8(*Header)), Header.Len());
	Payload.Append(FileContent);
	Payload.Append(reinterpret_cast<uint8*>(TCHAR_TO_UTF8(*EndBoundary)), EndBoundary.Len());

	// HTTP Request
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetVerb(TEXT("POST"));
	Request->SetURL(FullURL);
	Request->SetHeader("Content-Type", FString("multipart/form-data; boundary=") + Boundary);
	Request->SetContent(Payload);

	Request->OnProcessRequestComplete().BindUObject(this, &ANewHttpActor::TalkToPlantComplete);
	Request->ProcessRequest();

	UE_LOG(LogTemp, Warning, TEXT("Talk to plant : %s"), *FullURL);
}

void ANewHttpActor::TalkToPlantComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
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

void ANewHttpActor::ReqScore()
{
	const FString& FullURL = BaseURL + EndPoint_GetSpeechScore;

	// HTTP Request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetURL(FullURL);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &ANewHttpActor::ReqScoreComplete);

	HttpRequest->ProcessRequest();
	UE_LOG(LogTemp, Warning, TEXT("Req to %s"), *FullURL);
}

void ANewHttpActor::ReqScoreComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		const FString& ResultText = Response->GetContentAsString();
		const int32 Score = FCString::Atoi(*ResultText.Mid(1, ResultText.Len() - 2));
		UE_LOG(LogTemp, Warning, TEXT("Talk to plant score : %d"), Score);
		MyGameMode->SetTalkScore(Score);
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
void ANewHttpActor::TalkToPlantWithText(const FString& InputText)
{
	const FString& FullURL = BaseURL + EndPoint_TextToPlant;

	// HTTP Request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetURL(FullURL);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &ANewHttpActor::TalkToPlantWithTextComplete);

	// 양식 주의할 것(웹 서버쪽의 양식과 정확하게 일치해야 함)
	FString JsonBody = FString::Printf(TEXT("{\"chat_text\" : \"%s\"}"), *InputText);
	HttpRequest->SetContentAsString(JsonBody);

	HttpRequest->ProcessRequest();

	UE_LOG(LogTemp, Warning, TEXT("Talk to plant : %s"), *FullURL);
}

void ANewHttpActor::TalkToPlantWithTextComplete(FHttpRequestPtr Request, FHttpResponsePtr Response,
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

void ANewHttpActor::ReqScoreWithText()
{
	const FString& FullURL = BaseURL + EndPoint_GetTextScore;

	// HTTP Request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetURL(FullURL);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &ANewHttpActor::ReqScoreWithTextComplete);

	HttpRequest->ProcessRequest();
	UE_LOG(LogTemp, Warning, TEXT("Req to %s"), *FullURL);
}

void ANewHttpActor::ReqScoreWithTextComplete(FHttpRequestPtr Request, FHttpResponsePtr Response,
	bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		const FString& ResultText = Response->GetContentAsString();
		const int32 Score = FCString::Atoi(*ResultText.Mid(1, ResultText.Len() - 2));
		UE_LOG(LogTemp, Warning, TEXT("Talk to plant score : %d"), Score);
		MyGameMode->SetTalkScore(Score);
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

void ANewHttpActor::EndGame()
{
	const FString& FullURL = BaseURL + EndPoint_EndGame;

	// HTTP Request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetURL(FullURL);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	HttpRequest->ProcessRequest();
	UE_LOG(LogTemp, Warning, TEXT("Req reset history"));
}