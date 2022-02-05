#include "Sound.h"

#pragma comment(lib,"xaudio2.lib")

//#include <sstream>
#include <fstream>
#include <cassert>

Sound::SoundCommon::SoundCommon() {
	// XAudio�G���W���̃C���X�^���X�𐶐�
	HRESULT result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(result));

	// �}�X�^�[�{�C�X�𐶐�
	result = xAudio2->CreateMasteringVoice(&masterVoice);
	assert(SUCCEEDED(result));
}

Sound::SoundCommon::~SoundCommon() {
	this->xAudio2.Reset();
}




Sound::Sound(const char* filename, Sound::SoundCommon* soundCommon) {
	//1.�t�@�C���I�[�v��
	//�t�@�C�����̓X�g���[���̃C���X�^���X
	std::ifstream file;
	//.wav�t�@�C�����o�C�i�����[�h�ŊJ��
	file.open(filename, std::ios_base::binary);
	//�t�@�C���I�[�v�����s�����o����
	assert(file.is_open());

	//2.wav�f�[�^�ǂݍ���
	//RIFF�w�b�_�[�̓ǂݍ���
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	//�t�@�C����RIFF���`�F�b�N
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}
	//�^�C�v��WAVE���`�F�b�N
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}
	//Format�`�����N�̓ǂݍ���
	FormatChunk format = {};
	//�`�����N�w�b�_�[�̊m�F
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}
	//�`�����N�{�̂̓ǂݍ���
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);
	//Data�`�����N�̓ǂݍ���
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	//JUNK�`�����N�����o�����ꍇ
	if (strncmp(data.id, "JUNK", 4) == 0) {
		//�ǂݎ��ʒu��JUNK�`�����N�̏I���܂Ői�߂�
		file.seekg(data.size, std::ios_base::cur);
		//�ēǂݍ���
		file.read((char*)&data, sizeof(data));
	}
	if (strncmp(data.id, "data", 4) != 0) {
		assert(0);
	}
	//Data�`�����N�f�[�^�̈ꕔ�i�g�`�f�[�^�j�̓ǂݍ���
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	//3.�t�@�C���N���[�Y
	//Wave�t�@�C�������
	file.close();

	//4.�ǂݍ��񂾉����f�[�^��return
	//retrun���邽�߂̉����f�[�^

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

	//�o�b�t�@�̃����������
	delete[] this->pBuffer;

	this->pBuffer = 0;
	this->bufferSize = 0;
	this->wfex = {};
}

void Sound::createSourceVoice(SoundCommon* soundCommon, Sound* soundData) {
	//�g�`�t�H�[�}�b�g�����Ƃ�SourceVoice�̐���
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
	//�g�`�t�H�[�}�b�g�����Ƃ�SourceVoice�̐���
	createSourceVoice(soundCommon, soundData);

	//�Đ�����g�`�f�[�^�̐ݒ�
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData->pBuffer;
	buf.AudioBytes = soundData->bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.LoopCount = loopCount;

	//�g�`�f�[�^�̍Đ�
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