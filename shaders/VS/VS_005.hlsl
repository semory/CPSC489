// To compile VS: fxc filename.hlsl /Tvs_5_0 /EVS /Fo filename.cso
// To compile PS: fxc filename.hlsl /Tps_5_0 /EPS /Fo filename.cso

struct VShaderInput
{
 float3 pos : POSITION;
 float2 tc1 : TEXCOORD;
 float2 tc2 : TEXCOORD;
};

struct PShaderInput
{
 float4 pos : SV_POSITION;
 float2 tc1 : TEXCOORD;
 float2 tc2 : TEXCOORD;
};

cbuffer perframe : register(b0)
{
 matrix pview;
 matrix wview;
};

cbuffer permodel : register(b1)
{
 matrix mview;
};

PShaderInput VS(VShaderInput input)
{
 PShaderInput psi;
 psi.pos = float4(input.pos, 1.0);
 psi.pos = mul(psi.pos, mview);
 psi.pos = mul(psi.pos, wview);
 psi.pos = mul(psi.pos, pview);
 psi.tc1 = input.tc1;
 psi.tc2 = input.tc2;
 return psi;
}