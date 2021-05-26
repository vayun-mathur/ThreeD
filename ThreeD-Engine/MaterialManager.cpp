#include "MaterialManager.h"
#include "Material.h"

MaterialManager::MaterialManager() : ResourceManager()
{
}

MaterialManager::~MaterialManager()
{
}

MaterialPtr MaterialManager::createMaterialFromFile(const wchar_t* file_path)
{
    return std::static_pointer_cast<Material>(createResourceFromFile(file_path));
}

Resource* MaterialManager::createResourceFromFileConcrete(const wchar_t* file_path)
{
    Material* mat = nullptr;
    try {
        mat = new Material(file_path);
    }
    catch (...) {}
    return mat;
}

