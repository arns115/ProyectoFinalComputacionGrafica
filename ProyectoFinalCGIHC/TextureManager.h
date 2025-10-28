#pragma once
#include "AssetConstants.h"
#include "Texture.h"
#include <map>

// Clase para gestionar las texturas
class TextureManager 
{ 
public:

	TextureManager();

	void renderTexture(const std::string& textureName);

	~TextureManager();

private:	
	std::map<std::string, Texture*> textures;

	void loadTexture(const std::string& textureName, const std::string& texturePath);



};