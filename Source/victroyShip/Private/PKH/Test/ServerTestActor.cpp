// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Test/ServerTestActor.h"

#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Sound/SoundWave.h"
#include "AudioDevice.h"
#include "MediaPlayer.h"
#include "MediaSoundComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AServerTestActor::AServerTestActor()
{
	PrimaryActorTick.bCanEverTick = false;

	MediaComp = CreateDefaultSubobject<UMediaSoundComponent>(TEXT("MediaComp"));

}

// Called when the game starts or when spawned
void AServerTestActor::BeginPlay()
{
	Super::BeginPlay();

	// Init MediaPlayer
	MediaPlayer = NewObject<UMediaPlayer>();
	MediaPlayer->OnEndReached.AddDynamic(this, &AServerTestActor::OnPlayEnded);

	const FString DefaultPath = UKismetSystemLibrary::GetProjectDirectory() + TEXT("Extras/WavFiles/Default.wav");
	PlayMediaSound(DefaultPath);

	/*const FString& FilePath = UKismetSystemLibrary::GetProjectDirectory() + TEXT("Extras/WavFiles/Junho.wav");
	UE_LOG(LogTemp, Warning, TEXT("FilePath: %s"), *FilePath);
	Post_STT(FilePath);*/

	FString NPCName = TEXT("이준호");
	FString NPCChat = TEXT("아저씨, 요즘 어떤 무기가 제일 잘 팔려요?");
	Post_TTS(NPCName, NPCChat);
}

#pragma region Play Sound
void AServerTestActor::PlayMediaSound(const FString& FilePath)
{
	if (MediaPlayer->OpenFile(FilePath))
	{
		MediaComp->SetMediaPlayer(MediaPlayer);
		UE_LOG(LogTemp, Warning, TEXT("Open File Success : %s"), *FilePath);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Open File Failed : %s"), *FilePath);
	}
}

void AServerTestActor::OnPlayEnded()
{
	if (false == MediaPlayer->IsClosed())
	{
		MediaPlayer->Close();
	}
}
#pragma endregion

#pragma region wav only
void AServerTestActor::SendWavFile(const FString& FilePath)
{
	FHttpModule* Http = &FHttpModule::Get();
	if (!Http) { return; }

	TArray<uint8> FileContent;
	if (!FFileHelper::LoadFileToArray(FileContent, *FilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load file %s"), *FilePath);
		return;
	}

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

	// 고유한 Boundary 값 설정
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

	// FastAPI 서버 URL
	Request->SetURL("http://127.0.0.1:8000/upload-wav");
	Request->SetVerb("POST");
	Request->SetHeader("Content-Type", FString("multipart/form-data; boundary=") + Boundary);
	Request->SetContent(Payload);

	Request->OnProcessRequestComplete().BindUObject(this, &AServerTestActor::SendWavFileComplete);
	Request->ProcessRequest();
}

void AServerTestActor::SendWavFileComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully && Response.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("File uploaded successfully: %s"), *Response->GetContentAsString());
		GetWavFile();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to upload file"));
	}
}

void AServerTestActor::GetWavFile()
{
	const FString& FullURL = TEXT("http://127.0.0.1:8000/get-wav");

	// HTTP Request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetURL(FullURL);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &AServerTestActor::GetWavFileComplete);

	HttpRequest->ProcessRequest();
}

void AServerTestActor::GetWavFileComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		const TArray<uint8>& Data = Response->GetContent();

		// 원하는 파일 경로로 변경
		FString FilePath = FPaths::ProjectContentDir() + "ReceivedFile.wav";

		// 파일로 저장
		FFileHelper::SaveArrayToFile(Data, *FilePath);

		UE_LOG(LogTemp, Warning, TEXT("WAV file saved to %s"), *FilePath);

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

#pragma region stt
void AServerTestActor::Post_STT(const FString& FilePath)
{
	FHttpModule* Http = &FHttpModule::Get();
	if (!Http) { return; }

	TArray<uint8> FileContent;
	if (!FFileHelper::LoadFileToArray(FileContent, *FilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load file %s"), *FilePath);
		return;
	}

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

	// 고유한 Boundary 값 설정
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

	// FastAPI 서버 URL
	Request->SetURL("http://127.0.0.1:8000/stt-post");
	Request->SetVerb("POST");
	Request->SetHeader("Content-Type", FString("multipart/form-data; boundary=") + Boundary);
	Request->SetContent(Payload);

	Request->OnProcessRequestComplete().BindUObject(this, &AServerTestActor::Post_STTComplete);
	Request->ProcessRequest();
}

void AServerTestActor::Post_STTComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully && Response.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("File uploaded successfully"));
		Get_STT();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to upload file"));
	}
}

void AServerTestActor::Get_STT()
{
	const FString& FullURL = TEXT("http://127.0.0.1:8000/stt-get");

	// HTTP Request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetURL(FullURL);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &AServerTestActor::Get_STTComplete);

	HttpRequest->ProcessRequest();
}

void AServerTestActor::Get_STTComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		FString ResultText = Response->GetContentAsString();
		UE_LOG(LogTemp, Warning, TEXT("%s"), *ResultText.Mid(1, ResultText.Len() - 2));
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
void AServerTestActor::Post_TTS(const FString& NPCName, const FString& NPCChat)
{
	const FString& FullURL = TEXT("http://127.0.0.1:8000/tts-post");

	// HTTP Request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetURL(FullURL);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &AServerTestActor::Post_TTSComplete);

	// 양식 주의할 것(웹 서버쪽의 양식과 정확하게 일치해야 함)
	FString JsonBody = FString::Printf(TEXT("{\"npc_name\": \"%s\",\"npc_chat\" : \"%s\"}"), *NPCName, *NPCChat);
	HttpRequest->SetContentAsString(JsonBody);

	HttpRequest->ProcessRequest();

	UE_LOG(LogTemp, Warning, TEXT("Post tts to %s"), *FullURL);
}

void AServerTestActor::Post_TTSComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		UE_LOG(LogTemp, Warning, TEXT("Post tts complete"));
		Get_TTS();
	}
	else
	{
		if (Request->GetStatus() == EHttpRequestStatus::Succeeded)
		{
			UE_LOG(LogTemp, Warning, TEXT("Response Failed...%d"), Response->GetResponseCode());
		}
	}
}

void AServerTestActor::Get_TTS()
{
	const FString& FullURL = TEXT("http://127.0.0.1:8000/tts-get");

	// HTTP Request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetURL(FullURL);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &AServerTestActor::Get_TTSComplete);

	HttpRequest->ProcessRequest();
}

void AServerTestActor::Get_TTSComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		const TArray<uint8>& Data = Response->GetContent();

		FString FilePath = FPaths::ProjectContentDir() + "TTSResultFile.wav";
		FFileHelper::SaveArrayToFile(Data, *FilePath);
		PlayMediaSound(FilePath);
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
