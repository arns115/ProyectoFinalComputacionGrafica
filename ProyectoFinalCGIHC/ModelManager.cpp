#include "ModelManager.h"

// Carga todos los modelos al inicializar el ModelManager
ModelManager::ModelManager()
{
	loadModel("cuphead", AssetConstants::ModelPaths::CUPHEAD);
	loadModel(AssetConstants::ModelNames::ISAAC_CUERPO, AssetConstants::ModelPaths::ISAAC_CUERPO);
	loadModel(AssetConstants::ModelNames::ISAAC_CABEZA, AssetConstants::ModelPaths::ISAAC_CABEZA);
	loadModel(AssetConstants::ModelNames::ISAAC_BRAZO_IZQUIERDO, AssetConstants::ModelPaths::ISAAC_BRAZO_IZQUIERDO);
	loadModel(AssetConstants::ModelNames::ISAAC_BRAZO_DERECHO, AssetConstants::ModelPaths::ISAAC_BRAZO_DERECHO);
	loadModel(AssetConstants::ModelNames::ISAAC_PIERNA_IZQUIERDA, AssetConstants::ModelPaths::ISAAC_PIERNA_IZQUIERDA);
	loadModel(AssetConstants::ModelNames::ISAAC_PIERNA_DERECHA, AssetConstants::ModelPaths::ISAAC_PIERNA_DERECHA);

	// Cargar modelos de Gojo (jerarquía)
	loadModel(AssetConstants::ModelNames::GOJO, AssetConstants::ModelPaths::GOJO);
	loadModel(AssetConstants::ModelNames::GOJO_BRAZO_IZQ, AssetConstants::ModelPaths::GOJOBRAZOIZQ);
	loadModel(AssetConstants::ModelNames::GOJO_BRAZO_DER, AssetConstants::ModelPaths::GOJOBRAZODER);
	loadModel(AssetConstants::ModelNames::GOJO_PIERNA_IZQ, AssetConstants::ModelPaths::GOJOPIERNAIZQ);
	loadModel(AssetConstants::ModelNames::GOJO_PIERNA_DER, AssetConstants::ModelPaths::GOJOPIERNADER);
	loadModel(AssetConstants::ModelNames::GOJO_RODILLA_IZQ, AssetConstants::ModelPaths::GOJORODILLAIZQ);
	loadModel(AssetConstants::ModelNames::GOJO_RODILLA_DER, AssetConstants::ModelPaths::GOJORODILLADER);

	//Modelo prueba escenario
	loadModel(AssetConstants::ModelNames::ESCENARIOAZTECA, AssetConstants::ModelPaths::ESCENARIOAZTECA);

	//Modelos para el mercado
	loadModel(AssetConstants::ModelNames::CARPAVACIA, AssetConstants::ModelPaths::CARPAVACIA);
	loadModel(AssetConstants::ModelNames::CARPAYMESA, AssetConstants::ModelPaths::CARPAYMESA);
	loadModel(AssetConstants::ModelNames::CARPABUENA, AssetConstants::ModelPaths::CARPABUENA);
	loadModel(AssetConstants::ModelNames::PUESTOPESCADOS, AssetConstants::ModelPaths::PUESTOPESCADOS);
	loadModel(AssetConstants::ModelNames::PUESTOKEKAS, AssetConstants::ModelPaths::PUESTOKEKAS);
}


// Carga un modelo en el manager
void ModelManager::loadModel(const std::string& modelName, const std::string& modelPath)
{
	Model* model = new Model();
	model->LoadModel(modelPath);
	models[modelName] = model;
}

// Obtiene un modelo por su nombre (retorna puntero)
Model* ModelManager::getModel(const std::string& modelName)
{
	auto it = models.find(modelName);
	if (it != models.end()) {
		return it->second;
	}
	return nullptr;
}

ModelManager::~ModelManager()
{
}
