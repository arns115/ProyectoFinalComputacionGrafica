#include "SceneRenderer.h"

SceneRenderer::SceneRenderer() 
    : shader(nullptr), uniformModel(0), uniformProjection(0), 
      uniformView(0), uniformEyePosition(0), uniformColor(0),
      uniformSpecularIntensity(0), uniformShininess(0), inicializado(false)
{
}

SceneRenderer::~SceneRenderer() 
{
}

bool SceneRenderer::inicializar()
{
    // Crear y compilar el shader
    shader = new Shader();
    if (shader == nullptr) {
        return false;
    }
    
    shader->CreateFromFiles(AssetConstants::ShaderPaths::VERTEX_SHADER.c_str(), 
                           AssetConstants::ShaderPaths::FRAGMENT_SHADER.c_str());
    
    // Obtener las ubicaciones de los uniforms
    uniformModel = shader->GetModelLocation();
    uniformProjection = shader->GetProjectionLocation();
    uniformView = shader->GetViewLocation();
    uniformEyePosition = shader->GetEyePositionLocation();
    uniformColor = shader->getColorLocation();
    uniformSpecularIntensity = shader->GetSpecularIntensityLocation();
    uniformShininess = shader->GetShininessLocation();
    
    inicializado = true;
    return true;
}

void SceneRenderer::useShader()
{
    if (shader != nullptr) {
        shader->UseShader();
    }
}

void SceneRenderer::stopShader()
{
    glUseProgram(0);
}

void SceneRenderer::configurarMatrices(const Camera& camera, const glm::mat4& projection)
{    
    // Configurar matrices de proyección y vista
    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
    
    Camera& cam = const_cast<Camera&>(camera);
    glm::mat4 viewMatrix = cam.calculateViewMatrix();
    glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    
    // Configurar posición de la cámara
    glm::vec3 cameraPos = cam.getCameraPosition();
    glUniform3f(uniformEyePosition, cameraPos.x, cameraPos.y, cameraPos.z);
}

void SceneRenderer::configurarLuces(DirectionalLight* directionalLight,
                                   PointLight* pointLights, unsigned int pointLightCount,
                                   SpotLight* spotLights, unsigned int spotLightCount)
{
    if (shader == nullptr) return;
    
    // Configurar luz direccional
    if (directionalLight != nullptr) {
        shader->SetDirectionalLight(directionalLight);
    }
    
    // Configurar luces puntuales
    if (pointLights != nullptr && pointLightCount > 0) {
        shader->SetPointLights(pointLights, pointLightCount);
    }
    
    // Configurar spotlights
    if (spotLights != nullptr && spotLightCount > 0) {
        shader->SetSpotLights(spotLights, spotLightCount);
    }
}

void SceneRenderer::configurarMaterial(const Material& material)
{    
    Material& mat = const_cast<Material&>(material);
    mat.UseMaterial(uniformSpecularIntensity, uniformShininess);
}

void SceneRenderer::renderizarFrame(Skybox* skybox,
                                   Camera& camera,
                                   const glm::mat4& projectionMatrix,
                                   const std::vector<Entidad*>& entidades,
                                   DirectionalLight* directionalLight,
                                   PointLight* pointLights, unsigned int pointLightCount,
                                   SpotLight* spotLights, unsigned int spotLightCount)
{

    if (!inicializado) return;
    
    // 0. Limpiar buffers
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // 1. Renderizar skybox primero (usa su propio shader)
    if (skybox != nullptr) {
        glm::mat4 viewMatrix = camera.calculateViewMatrix();
        skybox->DrawSkybox(viewMatrix, projectionMatrix);
    }
    
    // 2. Reactivar el shader principal
    useShader();
    
    // 3. Configurar matrices (view, projection, y eye position)
    configurarMatrices(camera, projectionMatrix);
    
    // 4. Inicializar color uniforme (blanco por defecto)
    glUniform3f(uniformColor, 1.0f, 1.0f, 1.0f);
    
    // 5. Configurar luces
    configurarLuces(directionalLight, pointLights, pointLightCount, spotLights, spotLightCount);
    
    // 6. Renderizar todas las entidades 
    renderizar(entidades);

	stopShader();
}

void SceneRenderer::renderizar(const std::vector<Entidad*>& entidades)
{
    // Renderizar todas las entidades
    for (Entidad* entidad : entidades) {
        if (entidad != nullptr) {
            renderizarRecursivo(entidad, glm::mat4(1.0f));
        }
    }
}

void SceneRenderer::renderizarEntidad(Entidad* entidad,
                                     const glm::mat4& transformacionInicial)
{

    if (entidad == nullptr) return;
    
    
    // Renderizar la entidad y su jerarquía
    renderizarRecursivo(entidad, transformacionInicial);
}

void SceneRenderer::renderizarRecursivo(Entidad* entidad, const glm::mat4& transformacionPadre)
{   
    std::cout << "Renderizando entidad: " << entidad->nombreObjeto << std::endl;

    if (entidad == nullptr) {
        return;
    }
	entidad->actualizarTransformacion();
    // Calcular la transformación acumulada
    glm::mat4 transformacionActual = transformacionPadre * entidad->transformacionLocal;
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            std::cout << transformacionActual[i][j] << " ";
        }
        std::cout << std::endl;
	}


    // Se envia el model al shader
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(transformacionActual));
    
    // Configurar material si existe, o usar valores por defecto
    if (entidad->material != nullptr) {
        configurarMaterial(*entidad->material);
    } else {
		configurarMaterial(Material()); // Material por defecto
    }
    
    // APLICAR TEXTURA ANTES DE RENDERIZAR
    if (entidad->texture != nullptr) {
        std::cout << "Usando textura: " << entidad->nombreTextura << std::endl;
        entidad->texture->UseTexture();
    }
    
    // Renderizar según el tipo de geometría
    switch (entidad->TipoObjeto) {
        case TipoObjeto::MODELO:
            // Renderizar el modelo
            if (entidad->modelo != nullptr) {
                entidad->modelo->RenderModel();
            }
            break;
            
        case TipoObjeto::MESH:
            // Renderizar el mesh
            if (entidad->mesh != nullptr) {
                entidad->mesh->RenderMesh();
            }
            break;
    }
    
    // Renderizar recursivamente todas las entidades hijas
    for (auto* hijo : entidad->hijos) {
        renderizarRecursivo(hijo, transformacionActual);
    }
}
