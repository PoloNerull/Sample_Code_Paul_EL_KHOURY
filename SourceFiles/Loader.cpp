#include "Loader.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iostream>

Loader*		s_Loader = nullptr;

Loader::Loader()
{
}

void Loader::LoadObj(const char* filename, std::vector<Mesh*> &meshes, std::string modelName)
{
	_mtlFilePath = "";
	InitMeshParsing(0, 0, 0);

	std::ifstream file(filename, std::ios::in);
	if (!file)
		std::cout << "Cannot open: " << filename << std::endl;

	std::string line;
	while (std::getline(file, line))
	{
		std::string IdLine = line.substr(0, 2);

		if (line.substr(0, 6) == "mtllib")
		{
			std::string result = filename;
			std::size_t found = result.find_last_of("/\\");
			_mtlFilePath = result.substr(0, found + 1) + line.substr(7);
		}
		if (IdLine == "v ")
		{
			if (_changeMesh)
			{
				SetToAPIDataFormat();
				meshes.push_back(_currentMesh);
				InitMeshParsing(_vertices.size() + _saveVertices, _uvs.size() + _saveUv, _normals.size() + _saveNormal);
			}
			std::istringstream ssOne(line.substr(1));
			float x, y, z;
			ssOne >> x; ssOne >> y; ssOne >> z;
			_vertices.push_back(Vector3D(x, y, z));
		}
		else if (IdLine == "vt")
		{
			std::istringstream ssTwo(line.substr(2));
			float x, y;
			ssTwo >> x; ssTwo >> y;
			_uvs.push_back(Vector2D(x, y));
		}
		else if (IdLine == "vn")
		{
			std::istringstream ssTwo(line.substr(2));
			float x, y, z;
			ssTwo >> x; ssTwo >> y; ssTwo >> z;
			_normals.push_back(Vector3D(x, y, z));
		}
		else if (IdLine == "f ")
		{
			Face* newFace = new Face();
			std::istringstream ssOne(line.substr(1));
			std::string DataLine = ssOne.str();
			unsigned int length = DataLine.length();
			unsigned int currentPoly = 0;

			for (unsigned int cursor = 0; cursor < length; ++cursor)
			{
				if (DataLine[cursor] == ' ' && DataLine[cursor + 1] != '\0')
				{
					ParseVertex(_currentMesh, newFace, currentPoly, DataLine, cursor);
					currentPoly++;
				}
			}
			_faces.push_back(newFace);
		}
		else if (IdLine == "g ")
		{
			_currentMesh->SetResourceName(modelName + '/' + line.substr(2).c_str());
			_changeMesh = true;
		}
		else if (line.substr(0, 6) == "usemtl")
		{
			ParseMtl(_currentMesh, _mtlFilePath, line.substr(7));
		}
	}

	SetToAPIDataFormat();
	meshes.push_back(_currentMesh);
}

void Loader::InitMeshParsing(unsigned int verticesSize, unsigned int uvsSize, unsigned int normalsSize)
{
	_currentMesh = new Mesh();
	_saveNormal = normalsSize;
	_saveVertices = verticesSize;
	_saveUv = uvsSize;
	_nbOfElementsPerPoly = 3;
	_normals.clear();
	_vertices.clear();
	_uvs.clear();
	_changeMesh = false;
}

void Loader::ParseVertex(Mesh* mesh, Face* face, unsigned int currentVertex, std::string& str, unsigned int& strIt)
{
	std::string buffer;
	Vertex newVertex;
	unsigned int currentVertexInfo = 0;

	if (_normals.size() == 0)
	{
		_nbOfElementsPerPoly--;
		Vector3D normal(0.0f, 0.0f, 0.0f);
		_normals.push_back(normal);
	}

	if (_uvs.size() == 0)
	{
		_nbOfElementsPerPoly--;
		Vector2D uv(0.0f, 0.0f);
		_uvs.push_back(uv);
	}

	while (currentVertexInfo < _nbOfElementsPerPoly)
	{
		strIt++;

		if (str[strIt] == '/' || str[strIt] == ' ' || str[strIt] == '\0')
		{
			newVertex._id += buffer + '/';
			if (currentVertexInfo == 0)
			{
				Vector3D vertices = _vertices[stoi(buffer) - _saveVertices - 1];
				newVertex._data[0] = vertices.GetX();
				newVertex._data[1] = vertices.GetY();
				newVertex._data[2] = vertices.GetZ();

				if (_nbOfElementsPerPoly == 1)
				{
					newVertex._data[3] = _uvs[0].GetX();
					newVertex._data[4] = _uvs[0].GetY();
					newVertex._data[5] = _normals[0].GetX();
					newVertex._data[6] = _normals[0].GetY();
					newVertex._data[7] = _normals[0].GetZ();
				}
			}
			else if (currentVertexInfo == 1)
			{
				if (_uvs.size() > 1)
				{
					Vector2D texCoord = _uvs[stoi(buffer) - _saveUv - 1];
					newVertex._data[3] = texCoord.GetX();
					newVertex._data[4] = texCoord.GetY();
				}
				else if (_normals.size() > 1)
				{
					Vector3D normal = _normals[stoi(buffer) - _saveNormal - 1];
					newVertex._data[3] = _uvs[0].GetX();
					newVertex._data[4] = _uvs[0].GetY();
					newVertex._data[5] = normal.GetX();
					newVertex._data[6] = normal.GetY();
					newVertex._data[7] = normal.GetZ();
				}
			}

			else if (currentVertexInfo == 2)
			{
				Vector3D normal = _normals[stoi(buffer) - _saveNormal - 1];
				newVertex._data[5] = normal.GetX();
				newVertex._data[6] = normal.GetY();
				newVertex._data[7] = normal.GetZ();
			}

			currentVertexInfo++;
			buffer.clear();
		}
		else
			buffer += str[strIt];
	}

	face->_verticesIdx.push_back(CheckVerticesExistence(newVertex));

	strIt--;
}

void Loader::SetToAPIDataFormat()
{
	std::vector<float> meshData;
	std::vector<uint16_t> meshIndices;

	for (unsigned int i = 0; i < _faces.size(); i++)
		for (unsigned int j = 0; j < _faces[i]->_verticesIdx.size(); j++)
			meshIndices.push_back(_faces[i]->_verticesIdx[j]);

	if (_currentMesh->GetMtlData()->_mapKs != "")
		CalculateTangentArray(meshIndices);

	for (unsigned int i = 0; i < _finalVertices.size(); ++i)
	{
		for (unsigned int j = 0; j < 12; ++j)
			meshData.push_back(_finalVertices[i]._data[j]);
	}

	_currentMesh->SetMeshData(meshData);
	_currentMesh->SetMeshDataIndices(meshIndices);

	DeductCurrentMeshDrawMode();
	DeleteFaces();
}

void Loader::DeleteFaces()
{
	int i = _faces.size() - 1;
	for (i; i >= 0; --i)
		delete _faces[i];

	_faces.clear();
	_IdContainer.clear();
	_finalVertices.clear();
}

void Loader::DeductCurrentMeshDrawMode()
{
	if (_faces[0]->_verticesIdx.size() == 3)
		_currentMesh->SetDrawMode(DrawMode::TRIANGLES);
	else
		_currentMesh->SetDrawMode(DrawMode::QUADS);
}

void Loader::AddFinalVertex(Vertex vertexToAdd)
{
	_finalVertices.push_back(vertexToAdd);
}

unsigned int	Loader::CheckVerticesExistence(Vertex vertex)
{
	if (_IdContainer.find(vertex._id) == _IdContainer.end())
	{
		_IdContainer[vertex._id] = _finalVertices.size();
		AddFinalVertex(vertex);
	}
	return _IdContainer[vertex._id];
}

void Loader::ParseMtl(Mesh* mesh, std::string _mtlFilePath, std::string mtlName)
{
	MtlData* mtlData = new MtlData();
	mtlData->_name = mtlName;

	std::ifstream file(_mtlFilePath, std::ios::in);
	if (!file)
		std::cout << "Cannot open: " << _mtlFilePath << std::endl;

	std::string line;
	bool _canRecover = false;
	while (std::getline(file, line))
	{
		//line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
		line.erase(std::remove_if(line.begin(), line.end(), std::isspace), line.end());
		if (line.substr(0, 6) == "newmtl")
		{
			if (line.substr(6) == mtlName)
				_canRecover = true;
			else
				_canRecover = false;
		}
		else if (_canRecover)
		{
			if (line.substr(0, 2) == "Ns")
			{
				std::istringstream ssOne(line.substr(2));
				ssOne >> mtlData->_ns;
			}
			else if (line.substr(0, 2) == "Ni")
			{
				std::istringstream ssOne(line.substr(2));
				ssOne >> mtlData->_ni;
			}
			else if (line.substr(0, 1) == "d")
			{
				std::istringstream ssOne(line.substr(1));
				ssOne >> mtlData->_d;
			}
			else if (line.substr(0, 2) == "Tr")
			{
				std::istringstream ssOne(line.substr(2));
				ssOne >> mtlData->_tr;
			}
			else if (line.substr(0, 2) == "Tf")
			{
				std::istringstream ssOne(line.substr(2));
				float x, y, z;
				ssOne >> x; ssOne >> y; ssOne >> z;
				mtlData->_tf = Vector3D(x, y, z);
			}
			else if (line.substr(0, 5) == "illum")
			{
				std::istringstream ssOne(line.substr(5));
				ssOne >> mtlData->_illum;
			}
			else if (line.substr(0, 2) == "Ka")
			{
				std::istringstream ssOne(line.substr(2));
				float x, y, z;
				ssOne >> x; ssOne >> y; ssOne >> z;
				mtlData->_ka = Vector3D(x, y, z);
			}
			else if (line.substr(0, 2) == "Kd")
			{
				std::istringstream ssOne(line.substr(2));
				float x, y, z;
				ssOne >> x; ssOne >> y; ssOne >> z;
				mtlData->_kd = Vector3D(x, y, z);
			}
			else if (line.substr(0, 2) == "Ks")
			{
				std::istringstream ssOne(line.substr(2));
				float x, y, z;
				ssOne >> x; ssOne >> y; ssOne >> z;
				mtlData->_ks = Vector3D(x, y, z);
			}
			else if (line.substr(0, 2) == "Ke")
			{
				std::istringstream ssOne(line.substr(2));
				float x, y, z;
				ssOne >> x; ssOne >> y; ssOne >> z;
				mtlData->_ke = Vector3D(x, y, z);
			}
			else if (line.substr(0, 6) == "map_Ka")
			{
				std::string result = _mtlFilePath;
				std::size_t found = result.find_last_of("/\\");
				std::istringstream ssOne(line.substr(6));
				std::string mapKa;
				ssOne >> mapKa;
				mtlData->_mapKa = result.substr(0, found + 1) + mapKa;
			}
			else if (line.substr(0, 6) == "map_Kd")
			{
				std::string result = _mtlFilePath;
				std::size_t found = result.find_last_of("/\\");
				std::istringstream ssOne(line.substr(6));
				std::string mapKd;
				ssOne >> mapKd;
				mtlData->_mapKd = result.substr(0, found + 1) + mapKd;
			}
			else if (line.substr(0, 6) == "map_Ks")
			{
				std::string result = _mtlFilePath;
				std::size_t found = result.find_last_of("/\\");
				std::istringstream ssOne(line.substr(6));
				std::string mapKs;
				ssOne >> mapKs;
				mtlData->_mapKs = result.substr(0, found + 1) + mapKs;
			}
		}
	}

	mesh->SetMtlData(mtlData);
}

void Loader::CalculateTangentArray(std::vector<uint16_t> meshIndices)
{
	Vector3D* tan1 = new Vector3D[meshIndices.size() * 2];
	Vector3D* tan2 = tan1 + meshIndices.size();
	std::memset(tan1, 0, sizeof(tan1));

	for (long a = 0; a < meshIndices.size(); a += 3)
	{
		Vector3D v1 = Vector3D(_finalVertices[meshIndices[a]]._data[0], _finalVertices[meshIndices[a]]._data[1], _finalVertices[meshIndices[a]]._data[2]);
		Vector3D v2 = Vector3D(_finalVertices[meshIndices[a + 1]]._data[0], _finalVertices[meshIndices[a + 1]]._data[1], _finalVertices[meshIndices[a + 1]]._data[2]);
		Vector3D v3 = Vector3D(_finalVertices[meshIndices[a + 2]]._data[0], _finalVertices[meshIndices[a + 2]]._data[1], _finalVertices[meshIndices[a + 2]]._data[2]);

		Vector2D w1 = Vector2D(_finalVertices[meshIndices[a]]._data[3], _finalVertices[meshIndices[a]]._data[4]);
		Vector2D w2 = Vector2D(_finalVertices[meshIndices[a + 1]]._data[3], _finalVertices[meshIndices[a + 1]]._data[4]);
		Vector2D w3 = Vector2D(_finalVertices[meshIndices[a + 2]]._data[3], _finalVertices[meshIndices[a + 2]]._data[4]);	
	
		float x1 = v2.GetX() - v1.GetX();
		float x2 = v3.GetX() - v1.GetX();
		float y1 = v2.GetY() - v1.GetY();
		float y2 = v3.GetY() - v1.GetY();
		float z1 = v2.GetZ() - v1.GetZ();
		float z2 = v3.GetZ() - v1.GetZ();

		float s1 = w2.GetX() - w1.GetX();
		float s2 = w3.GetX() - w1.GetX();
		float t1 = w2.GetY() - w1.GetY();
		float t2 = w3.GetY() - w1.GetY();

		float r = 1.0f / (s1 * t2 - s2 * t1);
		Vector3D sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
		Vector3D tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

		tan1[a] += sdir;
		tan1[a + 1] += sdir;
		tan1[a + 2] += sdir;

		tan2[a] += tdir;
		tan2[a + 1] += tdir;
		tan2[a + 2] += tdir;
	}

	for (long a = 0; a < meshIndices.size(); a++)
	{
		Vector3D n = Vector3D(_finalVertices[meshIndices[a]]._data[5], _finalVertices[meshIndices[a]]._data[6], _finalVertices[meshIndices[a]]._data[7]);
		const Vector3D& t = tan1[a];

		Vector4D tangent = Vector4D((t - n * (n.ScalarProduct(t))), 0.0f);
		tangent.Normalize();
		tangent.SetW( ((n.VectorProduct(t)).ScalarProduct(tan2[a]) < 0.0f) ? -1.0f : 1.0f );

		_finalVertices[meshIndices[a]]._data[8] = tangent.GetX();
		_finalVertices[meshIndices[a]]._data[9] = tangent.GetY();
		_finalVertices[meshIndices[a]]._data[10] = tangent.GetZ();
		_finalVertices[meshIndices[a]]._data[11] = tangent.GetW();
	}

	delete[] tan1;
}