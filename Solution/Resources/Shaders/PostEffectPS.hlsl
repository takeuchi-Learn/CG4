#include "PostEffect.hlsli"

Texture2D<float4> tex : register(t0);   // 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0);        // 0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

float4 main(VSOutput input) : SV_TARGET
{
	// UV�l�̒��S����̋���(�傫��)
	float len = distance(input.uv, 0.5f);

	float vignNum = len * 0.75f;

	// �������̂悤�Ȃ���
	float time = 0.f;
	float sinNum = input.uv.y * 100.f + time;
	float sLineNum = sin(sinNum) * sin(sinNum + 0.75f) + 1;
	sLineNum /= 32;

	float4 texColor = tex.Sample(smp, input.uv);

	return float4(texColor.rgb - sLineNum - vignNum, 1);
}
