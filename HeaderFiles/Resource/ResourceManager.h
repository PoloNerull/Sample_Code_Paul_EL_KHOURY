#pragma once

#include <unordered_map>
#include <string>
#include "Resource\Model.h"
#include "Resource\Texture.h"
#include "Singleton.h"
#include "Loader.h"
#include "SceneObject.h"

using namespace Entities;

class DLLEXPORT ResourceManager : public Singleton<ResourceManager>
{
public:
	ResourceManager();
	~ResourceManager();

	SceneObject*								LoadModel(const char* pathname);
	SceneObject*								LoadModel(const char* pathname, Vector3D position);
	SceneObject*								LoadModel(const char* pathname, Vector3D position, SceneObject* parent);
	
	void										LoadSpecificObject();
	SceneObject*								CreateCubeObj();

	Model*										CreateModel(const char* pathname, std::string modelname);
	Model*										FindModel(const char* pathname);

	void										CreateTextureFromFile(Texture* textureInst, const char* path, TextureType type, ImageFormat format);
	void										CreateTexture(Texture* textureInst, const char* path, std::string textureName, TextureType type, ImageFormat format);

	// SETTERS
	void										SetAssetsPathName(std::string name);

private:
	Loader										_loader;

	std::unordered_map<std::string, Model*>		_models;
	std::unordered_map<std::string, uint32_t*>	_textures;
	std::string									_assetsPathName;
};

DLLEXPORT extern ResourceManager* s_ResourceManager;