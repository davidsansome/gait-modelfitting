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
	if (!m_models.contains(name))
		qFatal("Model %s not found", name.toAscii().data());
	
	return m_models[name];
}
