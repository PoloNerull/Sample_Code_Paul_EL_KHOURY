#pragma once

#include <vector>
#include "Mtl.h"
#include "Mesh.h"
#include "Face.h"
#include <unordered_map>

class Loader
{
public:
	Loader();
	~Loader() = default;

	void			LoadObj(const char* filename, std::vector<Mesh*> &meshes, std::string modelName);

private:
	void			ParseVertex(Mesh* mesh, Face* face, unsigned int currentVertex, std::string& str, unsigned int& strIt);
	void			ParseMtl(Mesh* mesh, std::string _mtlFilePath, std::string mtlName);
	unsigned int	CheckVerticesExistence(Vertex vertex);
	void			SetToAPIDataFormat();
	void			DeleteFaces();
	void			DeductCurrentMeshDrawMode();
	void			AddFinalVertex(Vertex vertexToAdd);
	void			InitMeshParsing(unsigned int verticesSize, unsigned int uvsSize, unsigned int normalsSize);
	void			CalculateTangentArray(std::vector<uint16_t> meshIndices);
	
	
	std::vector<Vector3D>							_vertices;
	std::vector<Vector3D>							_normals;
	std::vector<Vector2D>							_uvs;
	std::vector<Face*>								_faces;
	std::vector <Vertex>							_finalVertices;

	Mesh*											_currentMesh;
	bool											_changeMesh;
	unsigned int									_saveVertices;
	unsigned int									_saveNormal;
	unsigned int									_saveUv;
	unsigned int									_nbOfElementsPerPoly;
	std::unordered_map<std::string, unsigned int>	_IdContainer;

	std::string										_mtlFilePath;
};