#include "PostEffect.hlsli"

Texture2D<float4> tex0 : register(t0);   // 0番スロットに設定されたテクスチャ
Texture2D<float4> tex1 : register(t1);   // 1番スロットに設定されたテクスチャ
SamplerState smp : register(s0);        // 0番スロットに設定されたサンプラー

float fracNoise(float2 coord) {
	return frac(sin(dot(coord, float2(8.7819, 3.255))) * 437.645);
}

float4 main(VSOutput input) : SV_TARGET
{
	// 画面を左右それぞれmosaicNum分割した大きさのモザイクになる
	float mosaicNum = 100.f;
	float2 uv = floor(input.uv * mosaicNum) / mosaicNum;

	// UV値の中心からの距離(大きさ)
	float len = distance(uv, 0.5f);

	float vignIntensity = 0.9f;

	float vignNum = len * vignIntensity;

	// 走査線のようなもの
	float time = nowTime / oneSec * -2.f;
	float sinNum = uv.y * 100.f + time;
	float sLineNum = sin(sinNum) * sin(sinNum + 0.75f) + 1;
	sLineNum *= 0.03125f;	// sLineNum /= 32

	// rgbずらし(rだけだが)
	float4 texColor0 = tex0.Sample(smp, uv);
	texColor0.r = tex0.Sample(smp, uv + 0.01f * sin(time * 100.f));
	/*texColor0.g = tex0.Sample(smp, uv + 0.02f * sin(time * 100.f));
	texColor0.b = tex0.Sample(smp, uv + 0.03f * sin(time * 100.f));*/
	float4 texColor1 = tex1.Sample(smp, uv);
	texColor1.r = tex1.Sample(smp, uv + 0.01f * sin(time * 100.f));

	// tex0とtex1の横縞になるように描画
	float4 col = texColor0;
	/*if (fmod(input.uv.y, 0.1f) < 0.05f) {
		col = texColor1;
	}*/

	float noiseNum = fracNoise(input.uv * time * 10.f) - 0.5f;
	//noiseNum = 0.f;

	float4 drawCol = float4(col.rgb - sLineNum - vignNum + noiseNum, 1);

	return drawCol;
}
