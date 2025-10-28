#include "TextureManager.h"

// Carga todas las texturas al inicializar el TextureManager
TextureManager::TextureManager() 
{
	loadTexture(AssetConstants::TextureNames::AGAVE, AssetConstants::TexturePaths::AGAVE_PATH);
	loadTexture(AssetConstants::TextureNames::PASTO, AssetConstants::TexturePaths::PASTO_PATH);
	loadTexture(AssetConstants::TextureNames::LADRILLO, AssetConstants::TexturePaths::LADRILLO_PATH);
	loadTexture(AssetConstants::TextureNames::TIERRA, AssetConstants::TexturePaths::TIERRA_PATH);
}

// Renderiza una textura por su nombre
void TextureManager::renderTexture(const std::string& textureName) 
{
	auto it = textures.find(textureName);
	if (it != textures.end()) {
		it->second->UseTexture();
	}
}

void TextureManager::loadTexture(const std::string& textureName, const std::string& texturePath)
{
	Texture* texture = new Texture(texturePath.c_str());
	if (texture->LoadTextureA()) {
		textures[textureName] = texture;
	}
}

TextureManager::~TextureManager() 
{
}