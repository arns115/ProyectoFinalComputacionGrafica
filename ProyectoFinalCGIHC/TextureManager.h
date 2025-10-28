#pragma once
#include "AssetConstants.h"
#include "Texture.h"
#include <map>

// Clase para gestionar las texturas
class TextureManager 
{ 
public:

	TextureManager();

	void renderTexture(std::string);

	~TextureManager();

private:	
	std::map<std::string, Texture*> textures;

	void loadPastoTexture();
	void loadLadrilloTexture();
	void loadTierraTexture();
	void loadAgaveTexture();



};