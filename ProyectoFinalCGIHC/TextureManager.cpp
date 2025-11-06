#include "TextureManager.h"

// Carga todas las texturas al inicializar el TextureManager
TextureManager::TextureManager() 
{
	loadTexture(AssetConstants::TextureNames::AGAVE, AssetConstants::TexturePaths::AGAVE_PATH);
	loadTexture(AssetConstants::TextureNames::PASTO, AssetConstants::TexturePaths::PASTO_PATH);
	loadTexture(AssetConstants::TextureNames::LADRILLO, AssetConstants::TexturePaths::LADRILLO_PATH);
	loadTexture(AssetConstants::TextureNames::TIERRA, AssetConstants::TexturePaths::TIERRA_PATH);
	loadTexture(AssetConstants::TextureNames::EMPEDRADO, AssetConstants::TexturePaths::EMPEDRADO_PATH);
	loadTexture(AssetConstants::TextureNames::AGUA, AssetConstants::TexturePaths::AGUA_PATH);
	// Cargar texturas de Cuphead
	loadTexture(AssetConstants::TextureNames::CUPHEAD_TEXTURE, AssetConstants::TexturePaths::CUPHEAD_TEXTURE_PATH);
	loadTexture(AssetConstants::TextureNames::POPOTE_ROJO, AssetConstants::TexturePaths::POPOTE_ROJO_PATH);
}

// Obtiene una textura por su nombre 
Texture* TextureManager::getTexture(const std::string& textureName)
{
	auto it = textures.find(textureName);
	if (it != textures.end()) {
		return it->second;
	}
	return nullptr;
}

// Carga una textura en el manager
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