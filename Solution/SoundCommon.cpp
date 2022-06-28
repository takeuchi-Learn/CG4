#include "SoundCommon.h"
#include <cassert>


SoundCommon::SoundCommon() {
	// XAudioエンジンのインスタンスを生成
	HRESULT result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(result));

	// マスターボイスを生成
	result = xAudio2->CreateMasteringVoice(&masterVoice);
	assert(SUCCEEDED(result));
}

SoundCommon::~SoundCommon() {
	this->xAudio2.Reset();
}