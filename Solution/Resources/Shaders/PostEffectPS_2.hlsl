#include "PostEffect.hlsli"

Texture2D<float4> tex0 : register(t0);   // 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
Texture2D<float4> tex1 : register(t1);   // 1�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0);         // 0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[



float Gaussian(float2 drawUV, float2 pickUV, float sigma)
{
    float d = distance(drawUV, pickUV);
    return exp(-(d * d) / (2 * sigma * sigma));
}

float4 GaussianBlur(Texture2D<float4> tex, float2 uv, float sigma = 0.005, float stepWidth = 0.001)
{
    float _ShiftWidth = 0.005f;
    float _ShiftNum = 3;
    float4 col = float4(0, 0, 0, 1);
    float totalWeight = 0;
    for (float py = -sigma * 2; py <= sigma * 2; py += stepWidth)
    {
        for (float px = -sigma * 2; px <= sigma * 2; px += stepWidth)
        {
            float2 pickUV = uv + float2(px, py);
            float weight = Gaussian(uv, pickUV, sigma);
            col += tex.Sample(smp, pickUV) * weight;
            totalWeight += weight;
        }
    }
    col.rgb /= totalWeight;
    return col;
}

float4 main(VSOutput input) : SV_TARGET
{
    // �K�E�X�ڂ���
    float4 texColor0 = GaussianBlur(tex0, input.uv);
    float4 texColor1 = GaussianBlur(tex1, input.uv);

	// tex0��tex1�̉��ȂɂȂ�悤�ɕ`��
	float4 col = lerp(texColor0, texColor1, step(0.05f, fmod(input.uv.y, 0.1f)));

	return col;
}
