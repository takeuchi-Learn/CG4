#include "Basic.hlsli"

Texture2D<float4> tex : register(t0);  	// 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      	// 0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
    float4 texcolor = float4(tex.Sample(smp, input.uv));
    float3 normalLight = normalize(light);

    // 光源へのベクトルと法線ベクトルの内積
    float diffuse = saturate(dot(-normalLight, input.normal));
    // アンビエント光を0.3として計算
    float brightness = diffuse + 0.3f;
    // テクスチャとシェーディングによる色を合成
    return float4(texcolor.rgb * brightness, texcolor.a) * color;
}