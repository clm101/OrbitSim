#define PI 3.14159
#define TWO_PI 2 * PI

cbuffer Dimension : register(b0)
{
    float2 dim;
    float fTime;
    float filler;
}

float4 mix(float4 a, float4 b, float fac)
{
    return (1 - fac) * a + fac * b;
}

float3 mix(float3 a, float3 b, float fac) {
    return (1 - fac) * a + fac * b;
}

float3 rgb2hsb(float3 c) {
    const float4 K = float4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    float4 p = mix(float4(c.bg, K.wz), float4(c.gb, K.xy), step(c.b, c.g));
    float4 q = mix(float4(p.xyw, c.r), float4(c.r, p.yzx), step(p.x, c.r));
    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return float3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

float3 hsb2rgb(float3 c) {
    float3 rgb = clamp(abs((c.x * 6.0 + float3(0.0, 4.0, 2.0)) % 6.0 - 3.0) - 1.0, 0.0, 1.0);
    rgb = rgb * rgb * (3.0 - 2.0 * rgb);
    return c.z * mix(float3(1.0, 1.0, 1.0), rgb, c.y);
}

float plot(float2 st, float pct)
{
    const float thickness = 0.02;
    return smoothstep(pct - thickness, pct, st.y) - smoothstep(pct, pct + thickness, st.y);
}

float4 main(float4 fPixelPos : SV_Position) : SV_TARGET
{
    // Make bottom left corner the origin and declare color variable(col)
    const float2 st = float2(fPixelPos.x, -1 * fPixelPos.y + dim.y) / dim;
    float3 col = float3(0.0f, 0.0f, 0.0f);
    
    float2 fToCenter = float2(0.5, 0.5) - st;
    float fAngle = atan2(fToCenter.y, fToCenter.x);
    float fRadius = length(fToCenter) * 2.0;
    
    col = hsb2rgb(float3(((fAngle + fTime % (2 * PI)) / (2 * PI)) + 0.5, fRadius, 1.0));
    return float4(col, 0.0f);
}