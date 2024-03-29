#pragma once
#include <memory>

class SwapChain;
class DeviceContext;
class VertexShader;
class PixelShader;
class ComputeShader;
class VertexBuffer;
class IndexBuffer;
class ConstantBuffer;
class StructuredBuffer;
class RWStructuredBuffer;
class RenderSystem;
class GraphicsEngine;
class Resource;
class ResourceManager;
class Texture;
class RWTexture;
class Texture3D;
class RWTexture3D;
class TextureManager;
class AudioSound;
class AudioSoundManager;
class Mesh;
class MeshManager;
class Material;
class SceneObject;
class MeshObject;
class TerrainObject;
class WaterTileObject;
class CameraObject;
class DirectionalLightObject;
class PointLightObject;
class AudioSourceObject;
class ScriptObject;
class NumberObject;
class Vec3Object;
class FrameBuffer;
class VolumeObject;
class PhysicalObject;
class EditableMesh;
class TetrahedralMesh;
class Frame;
class GUIScreen;

typedef std::shared_ptr<SwapChain> SwapChainPtr;
typedef std::shared_ptr<DeviceContext> DeviceContextPtr;
typedef std::shared_ptr<VertexShader> VertexShaderPtr;
typedef std::shared_ptr<PixelShader> PixelShaderPtr;
typedef std::shared_ptr<ComputeShader> ComputeShaderPtr;
typedef std::shared_ptr<VertexBuffer> VertexBufferPtr;
typedef std::shared_ptr<IndexBuffer> IndexBufferPtr;
typedef std::shared_ptr<ConstantBuffer> ConstantBufferPtr;
typedef std::shared_ptr<StructuredBuffer> StructuredBufferPtr;
typedef std::shared_ptr<RWStructuredBuffer> RWStructuredBufferPtr;
typedef std::shared_ptr<Resource> ResourcePtr;
typedef std::shared_ptr<Texture> TexturePtr;
typedef std::shared_ptr<RWTexture> RWTexturePtr;
typedef std::shared_ptr<Texture3D> Texture3DPtr;
typedef std::shared_ptr<RWTexture3D> RWTexture3DPtr;
typedef std::shared_ptr<AudioSound> AudioSoundPtr;
typedef std::shared_ptr<Mesh> MeshPtr;
typedef std::shared_ptr<Material> MaterialPtr;
typedef std::shared_ptr<SceneObject> SceneObjectPtr;
typedef std::shared_ptr<MeshObject> MeshObjectPtr;
typedef std::shared_ptr<TerrainObject> TerrainObjectPtr;
typedef std::shared_ptr<WaterTileObject> WaterTileObjectPtr;
typedef std::shared_ptr<CameraObject> CameraObjectPtr;
typedef std::shared_ptr<DirectionalLightObject> DirectionalLightObjectPtr;
typedef std::shared_ptr<PointLightObject> PointLightObjectPtr;
typedef std::shared_ptr<AudioSourceObject> AudioSourceObjectPtr;
typedef std::shared_ptr<ScriptObject> ScriptObjectPtr;
typedef std::shared_ptr<NumberObject> NumberObjectPtr;
typedef std::shared_ptr<Vec3Object> Vec3ObjectPtr;
typedef std::shared_ptr<FrameBuffer> FrameBufferPtr;
typedef std::shared_ptr<VolumeObject> VolumeObjectPtr;
typedef std::shared_ptr<PhysicalObject> PhysicalObjectPtr;
typedef std::shared_ptr<EditableMesh> EditableMeshPtr;
typedef std::shared_ptr<TetrahedralMesh> TetrahedralMeshPtr;
typedef std::shared_ptr<Frame> FramePtr;
typedef std::shared_ptr<GUIScreen> GUIScreenPtr;

template<typename Base, typename T>
inline bool instanceof(const T* ptr) {
	return dynamic_cast<const Base*>(ptr) != nullptr;
}