#include "SoundCommon.h"
#include <cassert>


SoundCommon::SoundCommon() {
	// XAudio�G���W���̃C���X�^���X�𐶐�
	HRESULT result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(result));

	// �}�X�^�[�{�C�X�𐶐�
	result = xAudio2->CreateMasteringVoice(&masterVoice);
	assert(SUCCEEDED(result));
}

SoundCommon::~SoundCommon() {
	this->xAudio2.Reset();
}