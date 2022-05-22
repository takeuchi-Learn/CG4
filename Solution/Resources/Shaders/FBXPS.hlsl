#include "FBX.hlsli"

Texture2D<float4> tex : register(t0);  	// 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0);      	// 0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

PSOutput main(VSOutput input) {
	PSOutput output;

	float4 texcolor = tex.Sample(smp, input.uv);

	// Lambert����
	float3 light = normalize(float3(1, -1, 1));
	float diffuse = saturate(dot(-light, input.normal));
	float brightness = diffuse + 0.3f;
	float4 shadecolor = float4(brightness, brightness, brightness, 1.f);

	output.target0 = shadecolor * texcolor;
	output.target1 = float4(1 - (output.target0).rgb, 1);

	return output;
}