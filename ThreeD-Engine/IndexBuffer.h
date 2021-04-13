#pragma once
#include <d3d11.h>

#include "prerequisites.h"

class IndexBuffer
{
public:
	IndexBuffer(void* list_indices, UINT size_list, RenderSystem* system);
	bool load();
	UINT getSizeIndexList();
	~IndexBuffer();
private:
	UINT m_size_list;
private:
	ID3D11Buffer* m_buffer;
private:
	RenderSystem* m_system = nullptr;
private:
	friend class DeviceContext;
};