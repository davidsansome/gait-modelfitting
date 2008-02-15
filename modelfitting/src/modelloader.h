#ifndef MODELLOADER_H
#define MODELLOADER_H

#include <QMap>

class Model;

class ModelLoader
{
	friend class Model;
public:
	static ModelLoader* instance();
	
	const Model* model(const QString& name);
	
private:
	ModelLoader() {}
	void addModel(const Model* model);
	
	static ModelLoader* s_instance;
	QMap<QString, const Model*> m_models;
};

#endif
