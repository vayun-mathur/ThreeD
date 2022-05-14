StructuredBuffer<float> input1 : register(t0);
StructuredBuffer<float> input2 : register(t1);
RWStructuredBuffer<float> output : register(u0);

[numthreads(128, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	output[DTid.x] = input1[DTid.x] +input2[DTid.x];
}