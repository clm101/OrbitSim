float4 main(float2 pos : Position) : SV_Position
{
    return float4(pos.x, pos.y, 1.0f, 1.0f);

}