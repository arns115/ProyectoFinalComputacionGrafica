/*
 * Sphere.h
 *Basado en el código creado por
 *  Created on: 06/09/2016
 *      Author: reynaldo Martell
 */

#ifndef SPHERE_H_
#define SPHERE_H_

#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>

#include "definition.h"

class Sphere {
public:
	//Sphere(float ratio, int slices, int stacks, MODEL_MODE mode);
	Sphere(float ratio, int slices, int stacks);
	void init();
	void load();
	void render();
	
	// Obtener datos de la esfera
	std::vector<GLfloat> getVertices() const;
	std::vector<GLuint> getIndices() const { return index; }
	int getVertexCount() const { return vertices.size(); }
	int getIndexCount() const { return index.size(); }
	
	virtual ~Sphere();
private:
	// Estructura de vértice con posición, normal y coordenadas de textura
	struct VertexData {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoords;
		
		VertexData() : position(0.0f), normal(0.0f), texCoords(0.0f) {}
	};
	
	std::vector<VertexData> vertices;  // Vector de vértices con toda la información
	std::vector<GLuint> index;
	float ratio;
	int slices;
	int stacks;
	//MODEL_MODE mode;

	GLuint VAO, VBO, EBO;
};

#endif /* SPHERE_H_ */
