#include "Basic.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD, float3 light : LIGHT)
{
    VSOutput output; // ピクセルシェーダーに渡す値
    output.svpos = mul(mat, pos); // 座標に行列を乗算
    output.normal = normal;
    output.uv = uv;
    // 入力したベクトルを正規化して格納
    output.light = normalize(light);
    return output;
}
