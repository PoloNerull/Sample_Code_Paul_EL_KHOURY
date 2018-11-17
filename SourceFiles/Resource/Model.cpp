#include "Resource\Model.h"

Model::Model(std::string modelName)
{
	SetResourceName(modelName);
}

std::vector<Mesh*>&	Model::GetMeshes()
{
	return _meshes;
}

Mesh& Model::GetMesh(std::string meshName)
{
	for (Mesh* mesh : _meshes)
	{
		if (mesh->GetResourceName() == meshName)
			return *mesh;
	}
}