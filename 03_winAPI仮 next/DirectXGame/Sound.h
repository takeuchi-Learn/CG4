#pragma once
#include <xaudio2.h>
#include <cstdint>
#include <wrl.h>

class Sound {
public:
#pragma region �`�����N
	// �`�����N�w�b�_
	struct ChunkHeader {
		char id[4]; // �`�����N����ID
		int32_t size;  // �`�����N�T�C�Y
	};

	// RIFF�w�b�_�`�����N
	struct RiffHeader {
		ChunkHeader chunk;   // "RIFF"
		char type[4]; // "WAVE"
	};

	// FMT�`�����N
	struct FormatChunk {
		ChunkHeader chunk; // "fmt "
		WAVEFORMATEX fmt; // �g�`�t�H�[�}�b�g
	};
#pragma endregion

	class XAudio2VoiceCallback : public IXAudio2VoiceCallback {
	public:
		// �{�C�X�����p�X�̊J�n��
		STDMETHOD_(void, OnVoiceProcessingPassStart) (THIS_ UINT32 BytesRequired) {};
		// �{�C�X�����p�X�̏I����
		STDMETHOD_(void, OnVoiceProcessingPassEnd) (THIS) {};
		// �o�b�t�@�X�g���[���̍Đ����I��������
		STDMETHOD_(void, OnStreamEnd) (THIS) {};
		// �o�b�t�@�̎g�p�J�n��
		STDMETHOD_(void, OnBufferStart) (THIS_ void* pBufferContext) {};
		// �o�b�t�@�̖����ɒB������
		STDMETHOD_(void, OnBufferEnd) (THIS_ void* pBufferContext) {};
		// �Đ������[�v�ʒu�ɒB������
		STDMETHOD_(void, OnLoopEnd) (THIS_ void* pBufferContext) {};
		// �{�C�X�̎��s�G���[��
		STDMETHOD_(void, OnVoiceError) (THIS_ void* pBufferContext, HRESULT Error) {};
	};

	class SoundCommon {
	public:
		Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
		IXAudio2MasteringVoice* masterVoice;
		Sound::XAudio2VoiceCallback voiceCallback;

		SoundCommon();
		~SoundCommon();
	};

	// --------------------
	// �����o�ϐ�
	// --------------------
private:
	//�g�`�t�H�[�}�b�g
	WAVEFORMATEX wfex;
	//�o�b�t�@�̐擪�A�h���X
	BYTE* pBuffer;
	//�o�b�t�@�̃T�C�Y
	unsigned int bufferSize;

	IXAudio2SourceVoice* pSourceVoice = nullptr;



	// --------------------
	// �����o�֐�
	// --------------------
public:
	// �����f�[�^�̓ǂݍ���
	Sound(const char* filename, Sound::SoundCommon* soundCommon);

	// �����f�[�^�̉��
	~Sound();



	// --------------------
	// static�֐�
	// --------------------
private:
	static void createSourceVoice(SoundCommon* soundCommon, Sound* soundData);

public:
	// �����Đ���~
	static void SoundStopWave(Sound* soundData);

	/// <summary>
	/// �����Đ�
	/// </summary>
	/// <param name="loopCount">0�ŌJ��Ԃ������AXAUDIO2_LOOP_INFINITE�ŉi��</param>
	/// <param name="volume">0 ~ 1</param>
	static void SoundPlayWave(SoundCommon* soundCommon,
		Sound* soundData,
		int loopCount = 0, float volume = 0.2);

	//�Đ���Ԃ̊m�F
	static bool checkPlaySound(Sound* soundData);
};

