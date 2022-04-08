cbuffer cbuff0 : register(b0) {
    //float4 color; // 色(RGBA)
    matrix mat; // ３Ｄ変換行列
    float3 light;
};

cbuffer cbuff1 : register(b1) {
    float3 m_ambient : packoffset(c0);  // アンビエント係数
    float3 m_diffuse : packoffset(c1);  // ディフューズ係数
    float3 m_specular : packoffset(c2); // ディフューズ係数
    float m_alpha : packoffset(c2.w);   // アルファ
}

// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput {
    float4 svpos : SV_POSITION; // システム用頂点座標
    float3 normal : NORMAL;
    float2 uv  :TEXCOORD; // uv値
};
