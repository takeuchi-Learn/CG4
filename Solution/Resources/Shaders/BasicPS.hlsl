#include "Basic.hlsli"

Texture2D<float4> tex : register(t0);  	// 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      	// 0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{

	float3 eyeDir = normalize(cameraPos - input.worldPos.xyz);   // 頂点->視点ベクトル

	const float shininess = 4.f;    // 光沢

	float3 dir2LightDotNormal = dot(dir2Light, input.normal);

	float3 reflect = normalize(-dir2Light + 2 * dir2LightDotNormal * input.normal); // 反射光

	float3 ambient = m_ambient;
	float3 diffuse = dir2LightDotNormal * m_diffuse;
	float3 specular = pow(saturate(dot(reflect, eyeDir)), shininess) * m_specular;  // 鏡面反射光

	float4 shadeColor;
	shadeColor.rgb = (ambient + diffuse + specular) * lightColor;
	shadeColor.a = m_alpha;

    float4 texcolor = float4(tex.Sample(smp, input.uv));
    return shadeColor * texcolor;
}
