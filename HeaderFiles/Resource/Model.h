#pragma once

#include <vector>
#include <map>
#include "Mesh.h"
#include "Resource\Resource.h"

class Model : public Resource
{
public:
	Model(std::string modelName);
	~Model() = default;

	std::vector<Mesh*>&			GetMeshes();
	Mesh&						GetMesh(std::string meshName);

private:
	std::vector<Mesh*>			_meshes;
};