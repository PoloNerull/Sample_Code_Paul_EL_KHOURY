#include "Resource\Texture.h"
#include "RHI\RHI.h"

Texture::Texture(std::string textureName)
{
	SetResourceName(textureName);
}

Texture::~Texture()
{
	//for (uint32_t* texture : _textures)
	//	delete(texture);

	_textures.clear();
}

std::vector<uint32_t*>& Texture::GetTextures()
{
	return _textures;
}

void Texture::AddTexture(uint32_t * texture)
{
	if (texture != nullptr)
		_textures.push_back(texture);
}
