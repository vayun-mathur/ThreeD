#pragma once
#include "ResourceManager.h"

class MaterialManager : public ResourceManager
{
public:
	MaterialManager();
	~MaterialManager();
	MaterialPtr createMaterialFromFile(const wchar_t* file_path);

protected:
	virtual Resource* createResourceFromFileConcrete(const wchar_t* file_path) override;
};

