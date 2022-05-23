#include "PostEffect.hlsli"

Texture2D<float4> tex0 : register(t0);   // 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
Texture2D<float4> tex1 : register(t1);   // 1�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0);        // 0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

float4 main(VSOutput input) : SV_TARGET
{
	// UV�l�̒��S����̋���(�傫��)
	float len = distance(input.uv, 0.5f);

	float vignNum = len * 0.75f;

	// �������̂悤�Ȃ���
	/*float time = 0.f;
	float sinNum = input.uv.y * 100.f + time;
	float sLineNum = sin(sinNum) * sin(sinNum + 0.75f) + 1;
	sLineNum /= 32;*/
	float sLineNum = 0;

	float4 texColor0 = tex0.Sample(smp, input.uv);
	float4 texColor1 = tex1.Sample(smp, input.uv);

	// tex0��tex1�̉��ȂɂȂ�悤�ɕ`��
	float4 col = texColor0;
	if (fmod(input.uv.y, 0.1f) < 0.05f) {
		col = texColor1;
	}

	float4 drawCol = float4(col.rgb - sLineNum - vignNum, 1);

	return drawCol;
}
