#include "Entidad.h"


EntidadParte::EntidadParte(const std::string& modelo, 
                           glm::vec3 pos, 
                           glm::vec3 rot, 
                           glm::vec3 escala)
    : nombreModelo(modelo), posicionLocal(pos), rotacionLocal(rot), 
      escalaLocal(escala), transformacionLocal(glm::mat4(1.0f)) 
{
    actualizarTransformacion();
}

EntidadParte::~EntidadParte() 
{
    for (auto* hijo : hijos) {
        delete hijo;
    }
    hijos.clear();
}

void EntidadParte::actualizarTransformacion() 
{
    transformacionLocal = glm::mat4(1.0f);
    
    // Aplicar traslación
    transformacionLocal = glm::translate(transformacionLocal, posicionLocal);
    
    // Aplicar rotaciones (en orden Z, Y, X)
    transformacionLocal = glm::rotate(transformacionLocal, 
                                     glm::radians(rotacionLocal.z), 
                                     glm::vec3(0.0f, 0.0f, 1.0f));
    transformacionLocal = glm::rotate(transformacionLocal, 
                                     glm::radians(rotacionLocal.y), 
                                     glm::vec3(0.0f, 1.0f, 0.0f));
    transformacionLocal = glm::rotate(transformacionLocal, 
                                     glm::radians(rotacionLocal.x), 
                                     glm::vec3(1.0f, 0.0f, 0.0f));
    
    // Aplicar escala
    transformacionLocal = glm::scale(transformacionLocal, escalaLocal);
}

void EntidadParte::agregarHijo(EntidadParte* hijo) 
{
    if (hijo != nullptr) {
        hijos.push_back(hijo);
    }
}

Entidad::Entidad(glm::vec3 posicion, 
                 glm::vec3 rotacion, 
                 glm::vec3 escala,
                 EntidadParte* raizModelo)
    : posicionMundo(posicion), rotacionMundo(rotacion), 
      escalaMundo(escala), raiz(raizModelo) 
{
    actualizarTransformacionMundo();
}

Entidad::Entidad(EntidadParte* raizModelo)
    : posicionMundo(0.0f), rotacionMundo(0.0f), 
      escalaMundo(1.0f), raiz(raizModelo) 
{
    actualizarTransformacionMundo();
}

Entidad::Entidad(const std::string& nombreModeloRaiz,
                 glm::vec3 posicion, 
                 glm::vec3 rotacion, 
                 glm::vec3 escala)
    : posicionMundo(posicion), rotacionMundo(rotacion), 
      escalaMundo(escala), raiz(nullptr) 
{
    // Crear automáticamente la raíz si se proporciona un nombre de modelo
    if (!nombreModeloRaiz.empty()) {
        raiz = new EntidadParte(nombreModeloRaiz);
    }
    actualizarTransformacionMundo();
}

Entidad::~Entidad() 
{
    if (raiz != nullptr) {
        delete raiz;
        raiz = nullptr;
    }
}

void Entidad::setRaiz(EntidadParte* nuevaRaiz) 
{
    if (raiz != nullptr) {
        delete raiz;
    }
    raiz = nuevaRaiz;
}

EntidadParte* Entidad::getRaiz() 
{
    return raiz;
}

void Entidad::renderizar(ModelManager& modelManager, GLuint uniformModel) 
{
    if (raiz != nullptr) {
        renderizarRecursivo(raiz, transformacionMundo, modelManager, uniformModel);
    }
}

void Entidad::actualizarTransformacionMundo() 
{
    transformacionMundo = glm::mat4(1.0f);
    
    // Aplicar traslación
    transformacionMundo = glm::translate(transformacionMundo, posicionMundo);
    
    // Aplicar rotaciones
    transformacionMundo = glm::rotate(transformacionMundo, 
                                     glm::radians(rotacionMundo.z), 
                                     glm::vec3(0.0f, 0.0f, 1.0f));
    transformacionMundo = glm::rotate(transformacionMundo, 
                                     glm::radians(rotacionMundo.y), 
                                     glm::vec3(0.0f, 1.0f, 0.0f));
    transformacionMundo = glm::rotate(transformacionMundo, 
                                     glm::radians(rotacionMundo.x), 
                                     glm::vec3(1.0f, 0.0f, 0.0f));
    
    // Aplicar escala
    transformacionMundo = glm::scale(transformacionMundo, escalaMundo);
}

void Entidad::setPosicion(const glm::vec3& pos) 
{
    posicionMundo = pos;
    actualizarTransformacionMundo();
}

void Entidad::setRotacion(const glm::vec3& rot) 
{
    rotacionMundo = rot;
    actualizarTransformacionMundo();
}

void Entidad::setEscala(const glm::vec3& escala) 
{
    escalaMundo = escala;
    actualizarTransformacionMundo();
}

void Entidad::trasladar(const glm::vec3& delta) 
{
    posicionMundo += delta;
    actualizarTransformacionMundo();
}

void Entidad::rotar(const glm::vec3& delta) 
{
    rotacionMundo += delta;
    actualizarTransformacionMundo();
}

void Entidad::escalar(const glm::vec3& factor) 
{
    escalaMundo *= factor;
    actualizarTransformacionMundo();
}

glm::vec3 Entidad::getPosicion() const 
{
    return posicionMundo;
}

glm::vec3 Entidad::getRotacion() const 
{
    return rotacionMundo;
}

glm::vec3 Entidad::getEscala() const 
{
    return escalaMundo;
}

glm::mat4 Entidad::getTransformacion() const 
{
    return transformacionMundo;
}

void Entidad::renderizarRecursivo(EntidadParte* parte, 
                                  const glm::mat4& transformacionPadre, 
                                  ModelManager& modelManager, 
                                  GLuint uniformModel) 
{
    if (parte == nullptr) {
        return;
    }
    
    // Calcular la transformación acumulada
    glm::mat4 transformacionActual = transformacionPadre * parte->transformacionLocal;
    
    // Enviar la matriz al shader
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(transformacionActual));
    
    // Renderizar el modelo de esta parte
    if (!parte->nombreModelo.empty()) {
        modelManager.renderModel(parte->nombreModelo);
    }
    
    // Renderizar recursivamente todas las partes hijas
    for (auto* hijo : parte->hijos) {
        renderizarRecursivo(hijo, transformacionActual, modelManager, uniformModel);
    }
}
