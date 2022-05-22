cbuffer cbuff0 : register(b0) {
	matrix viewproj;	// ビュープロジェクション行列
	matrix world;		// ワールド行列	
	float3 cameraPos;	// カメラ座標(ワールド座標)
};

// ボーンの最大数(FbxObj3d.hの定数と合わせる)
static const int MAX_BONES = 32;

// ボーンのスキニング行列が入る
cbuffer skinning : register(b3) {
	matrix matSkinning[MAX_BONES];
}

struct VSInput {
	float4 pos : POSITION;	// 位置
	float3 normal : NORMAL;	// 頂点の法線
	float2 uv : TEXCOORD;	// テクスチャ―座標
	uint4 boneIndices : BONEINDICES;	// ボーンの番号
	float4 boneWeights : BONEWEIGHTS;	// ボーンのスキンウェイト
};

struct VSOutput {
	float4 svpos : SV_POSITION;	// システム用頂点座標
	float3 normal : NORMAL;		// 法線
	float2 uv : TEXCOORD;		// uv
};

// レンダーターゲットの数は2つ
// undone PostEffectクラスのRenderTargetNumは此処の個数(2つ)に合わせる
// 配列にすればいけるのでは?
struct PSOutput {
	float4 target0 : SV_TARGET0;
	float4 target1 : SV_TARGET1;
};