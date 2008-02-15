#include "modelloader.h"
#include "model.h"

ModelLoader* ModelLoader::s_instance = NULL;

ModelLoader* ModelLoader::instance()
{
	if (s_instance == NULL)
		s_instance = new ModelLoader();
	
	return s_instance;
}

void ModelLoader::addModel(const Model* model)
{
	m_models[model->name()] = model;
}

const Model* ModelLoader::model(const QString& name)
{
	return m_models[name];
}
