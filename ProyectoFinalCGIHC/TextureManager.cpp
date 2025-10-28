#include "TextureManager.h"

// Carga todas las texturas al inicializar el TextureManager
TextureManager::TextureManager() 
{
	loadPastoTexture();
	loadLadrilloTexture();
	loadTierraTexture();
	loadAgaveTexture();
}

// Renderiza una textura por su nombre
void TextureManager::renderTexture(std::string textureName) 
{
	auto it = textures.find(textureName);
	if (it != textures.end()) {
		it->second->UseTexture();
	}
}

// Carga la textura de pasto
void TextureManager::loadPastoTexture() 
{
	Texture* pastoTexture = new Texture(AssetConstants::TexturePaths::PASTO_PATH.c_str());
	pastoTexture->LoadTextureA();
	textures[AssetConstants::TextureNames::PASTO] = pastoTexture;
}

// Carga la textura de ladrillo
void TextureManager::loadLadrilloTexture() 
{
	Texture* ladrilloTexture = new Texture(AssetConstants::TexturePaths::LADRILLO_PATH.c_str());
	ladrilloTexture->LoadTextureA(); 
	textures[AssetConstants::TextureNames::LADRILLO] = ladrilloTexture;
}

// Carga la textura de tierra
void TextureManager::loadTierraTexture() 
{
	Texture* tierraTexture = new Texture(AssetConstants::TexturePaths::TIERRA_PATH.c_str());
	tierraTexture->LoadTextureA();
	textures[AssetConstants::TextureNames::TIERRA] = tierraTexture;
}


// Carga la textura de agave
void TextureManager::loadAgaveTexture() 
{
	Texture* agaveTexture = new Texture(AssetConstants::TexturePaths::AGAVE_PATH.c_str());
	agaveTexture->LoadTextureA();
	textures[AssetConstants::TextureNames::AGAVE] = agaveTexture;
}

TextureManager::~TextureManager() 
{
}