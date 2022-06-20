﻿#include "Sound.h"

#pragma comment(lib,"xaudio2.lib")

//#include <sstream>
#include <fstream>
#include <cassert>

Sound::SoundCommon::SoundCommon() {
	// XAudioエンジンのインスタンスを生成
	HRESULT result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(result));

	// マスターボイスを生成
	result = xAudio2->CreateMasteringVoice(&masterVoice);
	assert(SUCCEEDED(result));
}

Sound::SoundCommon::~SoundCommon() {
	this->xAudio2.Reset();
}




Sound::Sound(const char* filename, Sound::SoundCommon* soundCommon) {
	//1.ファイルオープン
	//ファイル入力ストリームのインスタンス
	std::ifstream file;
	//.wavファイルをバイナリモードで開く
	file.open(filename, std::ios_base::binary);
	//ファイルオープン失敗を検出する
	assert(file.is_open());

	//2.wavデータ読み込み
	//RIFFヘッダーの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	//ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}
	//タイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}
	//Formatチャンクの読み込み
	FormatChunk format = {};
	//チャンクヘッダーの確認
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}
	//チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);
	//Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	//JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK", 4) == 0) {
		//読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		//再読み込み
		file.read((char*)&data, sizeof(data));
	}
	if (strncmp(data.id, "data", 4) != 0) {
		assert(0);
	}
	//Dataチャンクデータの一部（波形データ）の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	//3.ファイルクローズ
	//Waveファイルを閉じる
	file.close();

	//4.読み込んだ音声データをreturn
	//retrunするための音声データ

	this->wfex = format.fmt;
	this->pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	this->bufferSize = data.size;

	createSourceVoice(soundCommon, this);
}

Sound::~Sound() {
	SoundStopWave(this);

	if (this->pSourceVoice != nullptr) {
		this->pSourceVoice->DestroyVoice();
	}

	//バッファのメモリを解放
	delete[] this->pBuffer;

	this->pBuffer = 0;
	this->bufferSize = 0;
	this->wfex = {};
}

void Sound::createSourceVoice(SoundCommon* soundCommon, Sound* soundData) {
	//波形フォーマットをもとにSourceVoiceの生成
	HRESULT result = soundCommon->xAudio2->CreateSourceVoice(&soundData->pSourceVoice, &soundData->wfex);
	assert(SUCCEEDED(result));
}

void Sound::SoundStopWave(Sound* soundData) {
	HRESULT result = soundData->pSourceVoice->Stop();
	result = soundData->pSourceVoice->FlushSourceBuffers();
	/*XAUDIO2_BUFFER buf{};
	result = soundData.pSourceVoice->SubmitSourceBuffer(&buf);*/
}

void Sound::SoundPlayWave(SoundCommon* soundCommon, Sound* soundData, int loopCount, float volume) {
	//波形フォーマットをもとにSourceVoiceの生成
	createSourceVoice(soundCommon, soundData);

	//再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData->pBuffer;
	buf.AudioBytes = soundData->bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.LoopCount = loopCount;

	//波形データの再生
	HRESULT result = soundData->pSourceVoice->SubmitSourceBuffer(&buf);
	result = soundData->pSourceVoice->SetVolume(volume);
	result = soundData->pSourceVoice->Start();
}

bool Sound::checkPlaySound(Sound* soundData) {

	if (soundData == nullptr ||
		soundData->pSourceVoice == nullptr) return false;

	XAUDIO2_VOICE_STATE tmp{};
	soundData->pSourceVoice->GetState(&tmp);
	if (tmp.BuffersQueued == 0U) {
		return false;
	}
	return true;
}