#pragma once

#include <vector>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "Entidad.h"
#include "Shader_light.h"
#include "Camera.h"
#include "AssetConstants.h"
#include "Material.h"
#include "Skybox.h"

// Clase para renderizar entidades de la escena
class SceneRenderer {
public:
    SceneRenderer();
    ~SceneRenderer();
    
    // Inicializar el renderer con shaders
    bool inicializar();
    
    // Función principal para renderizar un frame completo
    void renderizarFrame(Skybox* skybox,
                        Camera& camera,
                        const glm::mat4& projectionMatrix,
                        const std::vector<Entidad*>& entidades,
                        DirectionalLight* directionalLight,
                        PointLight* pointLights, unsigned int pointLightCount,
                        SpotLight* spotLights, unsigned int spotLightCount);
    
    // Renderizar una lista de entidades
    void renderizar(const std::vector<Entidad*>& entidades);
    
    // Renderizar una sola entidad y su jerarquía
    void renderizarEntidad(Entidad* entidad,
                          const glm::mat4& transformacionInicial = glm::mat4(1.0f));
    
    // Configurar matrices de vista y proyección
    void configurarMatrices(const Camera& camera, const glm::mat4& projection);
    
    // Configurar luces 
    void configurarLuces(DirectionalLight* directionalLight,
                        PointLight* pointLights, unsigned int pointLightCount,
                        SpotLight* spotLights, unsigned int spotLightCount);
    
    // Configurar material 
    void configurarMaterial(const Material& material);
    
    // Activar shader
    void useShader();
    // Desactivar shader
    void stopShader();

    // Obtener el shader actual
    Shader* getShader() { return shader; }
    

    
private:
    // Shader para renderizado
    Shader* shader;
    
    // Uniform locations
    GLuint uniformModel;
    GLuint uniformProjection;
    GLuint uniformView;
    GLuint uniformEyePosition;
    GLuint uniformColor;
    GLuint uniformSpecularIntensity;
    GLuint uniformShininess;
    
    // Flag de inicialización
    bool inicializado;
    
    // Función recursiva interna para renderizar jerarquía
    void renderizarRecursivo(Entidad* entidad, const glm::mat4& transformacionPadre);
};
