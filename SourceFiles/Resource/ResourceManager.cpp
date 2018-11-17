#include "Resource\ResourceManager.h"
#include "MeshObject.h"
#include "RHI\RHI.h"
#include "Logger\Logger.h"

#include "stb_image.h"
#include <exception>

DLLEXPORT ResourceManager* s_ResourceManager = nullptr;

ResourceManager::ResourceManager()
: _loader(Loader()),
  _assetsPathName("../Assets/")
{
	LoadSpecificObject();
}

ResourceManager::~ResourceManager()
{
	for (std::pair<std::string, Model*> element : _models)
		delete(element.second);

	_models.clear();

	for (std::pair<std::string, uint32_t*> texture : _textures)
		delete(texture.second);

	_textures.clear();
}

SceneObject* ResourceManager::LoadModel(const char* pathname)
{
	return LoadModel(pathname, Vector3D(.0f, .0f, .0f));
}

SceneObject* ResourceManager::LoadModel(const char* pathname, Vector3D position)
{
	SceneObject* parentEmptyObject = new SceneObject(pathname, position);
	return LoadModel(pathname, position, parentEmptyObject);
}

SceneObject* ResourceManager::LoadModel(const char* pathname, Vector3D position, SceneObject* parent)
{
	Model* model = FindModel(pathname);

	std::vector<Mesh*> meshes = model->GetMeshes();

	for (Mesh* mesh : meshes)
	{
		if (parent != nullptr)
		{
			MeshObject* meshChild = new MeshObject(Vector3D(.0f, .0f, .0f), *mesh, parent);
		}
		else
			MeshObject* meshChild = new MeshObject(position, *mesh);
	}

	return parent;
}

void  ResourceManager::LoadSpecificObject()
{
	FindModel("cube/cube.obj");
}

SceneObject* ResourceManager::CreateCubeObj()
{
	Model* model = FindModel("cube/cube.obj");

	Mesh* mesh = model->GetMeshes()[0];

	MeshObject* meshCube = new MeshObject("Cube", Vector3D(.0f, .0f, .0f), *mesh);

	return meshCube;
}

Model* ResourceManager::FindModel(const char* pathname)
{
	std::string tempString = pathname;
	std::size_t found = tempString.find_last_of("/\\");
	std::string modelName = tempString.substr(found + 1);
	modelName = modelName.substr(0, modelName.find_last_of("."));

	auto model = _models.find(modelName);
	if (model != _models.end())
		return model->second;
	else
		return CreateModel(pathname, modelName);
}

Model* ResourceManager::CreateModel(const char* pathname, std::string modelname)
{
	Model* newModel = new Model(modelname);
	std::string finalPath = _assetsPathName + pathname;
	_loader.LoadObj(finalPath.c_str(), newModel->GetMeshes(), modelname);

	_models.insert({ modelname, newModel });

	return newModel;
}

void ResourceManager::CreateTextureFromFile(Texture* textureInst, const char * path, TextureType type, ImageFormat format)
{
	std::string tempString = path;
	std::size_t found = tempString.find_last_of("/\\");
	std::string textureName = tempString.substr(found + 1);
	
	auto texture = _textures.find(textureName);
	if (texture != _textures.end())
		textureInst->AddTexture(texture->second);
	else
		CreateTexture(textureInst, path, textureName, type, format);
}

void ResourceManager::CreateTexture(Texture* textureInst, const char * path, std::string textureName, TextureType type, ImageFormat format)
{
	uint32_t* textureData = new uint32_t();
	int w, h, c;
	uint8_t* imageData = stbi_load(path, &w, &h, &c, STBI_rgb_alpha);

	if (imageData == nullptr) 
	{
		try
		{
			throw std::runtime_error(stbi_failure_reason());
		}
		catch (std::runtime_error& e)
		{
			std::cout <<"STBI_load error: " << e.what() << std::endl;
			LOG(e.what(), LOG_ENUM::LOG_ERROR, s_Logger);
		}
	}

	s_RHI->GenTextures(1, textureData);
	s_RHI->BindTexture(type, *textureData);
	s_RHI->Init2DTexture(type, format, w, h, imageData, TextureParameter::MIN_FILTER, TextureParameterValue::LINEAR);
	s_RHI->UnbindTexture(type);

	stbi_image_free(imageData);

	_textures.insert({ textureName, textureData });

	textureInst->AddTexture(textureData);
}

void ResourceManager::SetAssetsPathName(std::string name)
{
	_assetsPathName = name;
}