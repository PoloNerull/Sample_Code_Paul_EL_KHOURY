#pragma once

#include "Resource.h"
#include "RHI\RHI.h"
#include <vector>

class Texture : public Resource
{
public:
	Texture(std::string textureName);
	~Texture();

	std::vector<uint32_t*>&				GetTextures();
	void								AddTexture(uint32_t* texture);

private:
	std::vector<uint32_t*>				_textures;
};