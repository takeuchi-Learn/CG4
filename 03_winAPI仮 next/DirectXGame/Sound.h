#pragma once
#include <xaudio2.h>
#include <cstdint>
#include <wrl.h>

class Sound {
public:
#pragma region チャンク
	// チャンクヘッダ
	struct ChunkHeader {
		char id[4]; // チャンク毎のID
		int32_t size;  // チャンクサイズ
	};

	// RIFFヘッダチャンク
	struct RiffHeader {
		ChunkHeader chunk;   // "RIFF"
		char type[4]; // "WAVE"
	};

	// FMTチャンク
	struct FormatChunk {
		ChunkHeader chunk; // "fmt "
		WAVEFORMATEX fmt; // 波形フォーマット
	};
#pragma endregion

	class XAudio2VoiceCallback : public IXAudio2VoiceCallback {
	public:
		// ボイス処理パスの開始時
		STDMETHOD_(void, OnVoiceProcessingPassStart) (THIS_ UINT32 BytesRequired) {};
		// ボイス処理パスの終了時
		STDMETHOD_(void, OnVoiceProcessingPassEnd) (THIS) {};
		// バッファストリームの再生が終了した時
		STDMETHOD_(void, OnStreamEnd) (THIS) {};
		// バッファの使用開始時
		STDMETHOD_(void, OnBufferStart) (THIS_ void* pBufferContext) {};
		// バッファの末尾に達した時
		STDMETHOD_(void, OnBufferEnd) (THIS_ void* pBufferContext) {};
		// 再生がループ位置に達した時
		STDMETHOD_(void, OnLoopEnd) (THIS_ void* pBufferContext) {};
		// ボイスの実行エラー時
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
	// メンバ変数
	// --------------------
private:
	//波形フォーマット
	WAVEFORMATEX wfex;
	//バッファの先頭アドレス
	BYTE* pBuffer;
	//バッファのサイズ
	unsigned int bufferSize;

	IXAudio2SourceVoice* pSourceVoice = nullptr;



	// --------------------
	// メンバ関数
	// --------------------
public:
	// 音声データの読み込み
	Sound(const char* filename);

	// 音声データの解放
	~Sound();



	// --------------------
	// static関数
	// --------------------
private:
	static void createSourceVoice(SoundCommon& soundCommon, Sound& soundData);

public:
	// 音声再生停止
	static void SoundStopWave(Sound& soundData);

	/// <summary>
	/// 音声再生
	/// </summary>
	/// <param name="loopCount">0で繰り返し無し、XAUDIO2_LOOP_INFINITEで永遠</param>
	/// <param name="volume">0 ~ 1</param>
	static void SoundPlayWave(SoundCommon& soundCommon,
		Sound& soundData,
		int loopCount = 0, float volume = 0.2);

	//再生状態の確認
	static bool checkPlaySound(SoundCommon& soundCommon, Sound& soundData);
};

