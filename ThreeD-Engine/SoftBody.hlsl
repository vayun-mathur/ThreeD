struct node {
	float mass;
	float3 pos;
	float3 vel;
	float3 f;
	bool exists;
};

cbuffer cbuf : register(b0) {
	float ks, kd;
	float rest_length;
	float dt;
	uint size;
};

RWStructuredBuffer<node> nodes : register(u0);

float3 force(uint index_from, uint index_by, float rest_length, float ks, float kd) {
	if (nodes[index_by].exists == false) return 0;
	float3 dir = (nodes[index_by].pos - nodes[index_from].pos);
	float3 f = (dir - normalize(dir) * rest_length) * ks
		+ normalize(dir) * dot(normalize(dir), nodes[index_by].vel - nodes[index_from].vel) * kd;
	return f;
}

uint index(uint x, uint y, uint z) {
	return x * size * size + y * size + z;
}

const static float kmul1 = 0.666;
const static float kmul2 = 0.1;

[numthreads(8, 8, 8)]
void UpdateSprings(uint3 id : SV_DispatchThreadID)
{
	if (id.x >= size || id.y >= size || id.z >= size) return;
	uint idx0 = index(id.x, id.y, id.z);
	if (nodes[idx0].exists == false) return;
	bool x0 = id.x != 0;
	bool y0 = id.y != 0;
	bool z0 = id.z != 0;
	bool x1 = id.x != size - 1;
	bool y1 = id.y != size - 1;
	bool z1 = id.z != size - 1;
	nodes[idx0].f = float3(0, -9.81 * nodes[idx0].mass, 0);

	float rl = rest_length;
	float KS = ks;
	float KD = kd;

	if (x1) nodes[idx0].f += force(idx0, index(id.x + 1, id.y, id.z), rl, KS, KD);
	if (y1) nodes[idx0].f += force(idx0, index(id.x, id.y + 1, id.z), rl, KS, KD);
	if (z1) nodes[idx0].f += force(idx0, index(id.x, id.y, id.z + 1), rl, KS, KD);
	if (x0) nodes[idx0].f += force(idx0, index(id.x - 1, id.y, id.z), rl, KS, KD);
	if (y0) nodes[idx0].f += force(idx0, index(id.x, id.y - 1, id.z), rl, KS, KD);
	if (z0) nodes[idx0].f += force(idx0, index(id.x, id.y, id.z - 1), rl, KS, KD);
	rl = rest_length * sqrt(2);
	KS *= kmul1;
	KD *= kmul1;
	if (x0 && y0) nodes[idx0].f += force(idx0, index(id.x - 1, id.y - 1, id.z), rl, KS, KD);
	if (x0 && z0) nodes[idx0].f += force(idx0, index(id.x - 1, id.y, id.z - 1), rl, KS, KD);
	if (y0 && z0) nodes[idx0].f += force(idx0, index(id.x, id.y - 1, id.z - 1), rl, KS, KD);
	if (x1 && y0) nodes[idx0].f += force(idx0, index(id.x + 1, id.y - 1, id.z), rl, KS, KD);
	if (x1 && z0) nodes[idx0].f += force(idx0, index(id.x + 1, id.y, id.z - 1), rl, KS, KD);
	if (y1 && z0) nodes[idx0].f += force(idx0, index(id.x, id.y + 1, id.z - 1), rl, KS, KD);
	if (x0 && y1) nodes[idx0].f += force(idx0, index(id.x - 1, id.y + 1, id.z), rl, KS, KD);
	if (x0 && z1) nodes[idx0].f += force(idx0, index(id.x - 1, id.y, id.z + 1), rl, KS, KD);
	if (y0 && z1) nodes[idx0].f += force(idx0, index(id.x, id.y - 1, id.z + 1), rl, KS, KD);
	if (x1 && y1) nodes[idx0].f += force(idx0, index(id.x + 1, id.y + 1, id.z), rl, KS, KD);
	if (x1 && z1) nodes[idx0].f += force(idx0, index(id.x + 1, id.y, id.z + 1), rl, KS, KD);
	if (y1 && z1) nodes[idx0].f += force(idx0, index(id.x, id.y + 1, id.z + 1), rl, KS, KD);

	rl = rest_length * sqrt(3);
	KS *= kmul2;
	KD *= kmul2;

	if (x0 && y0 && z0) nodes[idx0].f += force(idx0, index(id.x - 1, id.y - 1, id.z - 1), rl, KS, KD);
	if (x0 && y0 && z1) nodes[idx0].f += force(idx0, index(id.x - 1, id.y - 1, id.z + 1), rl, KS, KD);
	if (x0 && y1 && z0) nodes[idx0].f += force(idx0, index(id.x - 1, id.y + 1, id.z - 1), rl, KS, KD);
	if (x0 && y1 && z1) nodes[idx0].f += force(idx0, index(id.x - 1, id.y + 1, id.z + 1), rl, KS, KD);
	if (x1 && y0 && z0) nodes[idx0].f += force(idx0, index(id.x + 1, id.y - 1, id.z - 1), rl, KS, KD);
	if (x1 && y0 && z1) nodes[idx0].f += force(idx0, index(id.x + 1, id.y - 1, id.z + 1), rl, KS, KD);
	if (x1 && y1 && z0) nodes[idx0].f += force(idx0, index(id.x + 1, id.y + 1, id.z - 1), rl, KS, KD);
	if (x1 && y1 && z1) nodes[idx0].f += force(idx0, index(id.x + 1, id.y + 1, id.z + 1), rl, KS, KD);

	if (nodes[idx0].pos.y <= 19.5) {
		nodes[idx0].vel = reflect(nodes[idx0].vel, normalize(float3(0, 1, 0)));
	}
	nodes[idx0].vel += nodes[idx0].f * dt / nodes[idx0].mass;
	nodes[idx0].pos += nodes[idx0].vel * dt;
}