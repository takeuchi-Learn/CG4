#include "Basic.hlsli"

Texture2D<float4> tex : register(t0);  	// 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      	// 0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
    float4 texcolor = float4(tex.Sample(smp, input.uv));
    float3 normalLight = normalize(light);

    float light_diffuse = saturate(dot(-normalLight, input.normal));
    float3 shade_color = m_ambient;    // アンビエント
    shade_color += m_diffuse * light_diffuse;   // ディフューズ

    return float4(texcolor.rgb * shade_color, texcolor.a * m_alpha);

    //// 光源へのベクトルと法線ベクトルの内積
    //float diffuse = saturate(dot(-normalLight, input.normal));
    //// アンビエント光を0.3として計算
    //float brightness = diffuse + 0.3f;
    //// テクスチャとシェーディングによる色を合成
    //return float4(texcolor.rgb * brightness, texcolor.a) * color;
}
