#!BPY

"""
Name: 'C++ Model Exporter (.cpp)...'
Blender: 242
Group: 'Export'
Tooltip: 'Export selected mesh to C++ source file'
"""

fileTemplate = """#include "model.h"

const int vertexCount = __VERTEX_COUNT__;
Vertex vertexData[] =
{
	__VERTICES__
};

const int materialCount = __MATERIAL_COUNT__;
Material materialData[] =
{
	__MATERIALS__
};

const int triangleCount = __TRIANGLE_COUNT__;
Triangle triangleData[] =
{
	__TRIANGLES__
};

int initModel()
{
	Model* model = new Model("__MODELNAME__");
	model->setVertices(vertexCount, vertexData);
	model->setMaterials(materialCount, materialData);
	model->setTriangles(triangleCount, triangleData);
	model->setMinMax(Vec3(__MIN__), Vec3(__MAX__));
}

int a = initModel(); // Hacky way to get a static code block
"""

__author__ = "David Sansome"

import Blender
import BPyMesh
import os
from struct import pack

def cbool(number):
	if number == 0:
		return "false"
	else:
		return "true"

def write(filename):
	if not filename.lower().endswith('.cpp'):
		filename += '.cpp'
	modelname = os.path.splitext(os.path.basename(filename))[0]
	
	scene = Blender.Scene.GetCurrent()
	
	# Get list of meshes
	meshes = []
	for o in scene.objects:
		if o.type == 'Mesh':
			mesh = o.getData(mesh=1)
        		mesh.transform(o.matrixWorld)
			meshes.append(mesh)
	
	# Calculate min and max points
	min = [float("inf"), float("inf"), float("inf")]
	max = [-float("inf"), -float("inf"), -float("inf")]
	for v in meshes[0].verts:
		if v.co[0] < min[0]: min[0] = v.co[0]
		if v.co[1] < min[1]: min[1] = v.co[1]
		if v.co[2] < min[2]: min[2] = v.co[2]
		if v.co[0] > max[0]: max[0] = v.co[0]
		if v.co[1] > max[1]: max[1] = v.co[1]
		if v.co[2] > max[2]: max[2] = v.co[2]
	
	# Get a mapping of vertices to materials
	materialMapping = {}
	for f in meshes[0].faces:
		for v in f.verts:
			materialMapping[(v.co[0], v.co[1], v.co[2])] = f.mat
	
	# Get a list of triangles - splitting quads if we need to
	triangles = []
	for m in meshes:
		for f in m.faces:
			if len(f.v) == 3:
				triangles.append((
					materialMapping.keys().index((f.v[0].co[0], f.v[0].co[1], f.v[0].co[2])),
					materialMapping.keys().index((f.v[1].co[0], f.v[1].co[1], f.v[1].co[2])),
					materialMapping.keys().index((f.v[2].co[0], f.v[2].co[1], f.v[2].co[2])), f.mat))
			else:
				triangles.append((
					materialMapping.keys().index((f.v[0].co[0], f.v[0].co[1], f.v[0].co[2])),
					materialMapping.keys().index((f.v[1].co[0], f.v[1].co[1], f.v[1].co[2])),
					materialMapping.keys().index((f.v[2].co[0], f.v[2].co[1], f.v[2].co[2])), f.mat))
				triangles.append((
					materialMapping.keys().index((f.v[0].co[0], f.v[0].co[1], f.v[0].co[2])),
					materialMapping.keys().index((f.v[2].co[0], f.v[2].co[1], f.v[2].co[2])),
					materialMapping.keys().index((f.v[3].co[0], f.v[3].co[1], f.v[3].co[2])), f.mat))
	
	fileData = fileTemplate
	fileData = fileData.replace("__VERTEX_COUNT__", str(len(materialMapping)))
	fileData = fileData.replace("__VERTICES__", ",\n\t".join(["Vertex(Vec4(%f, %f, %f, 1.0), %i)" % (v[0], v[1], v[2], materialMapping[v]) for v in materialMapping.keys()]))
	fileData = fileData.replace("__MATERIAL_COUNT__", str(len(meshes[0].materials)))
	fileData = fileData.replace("__MATERIALS__", ",\n\t".join(["Material(Vec3(%f, %f, %f))" % (m.R, m.G, m.B) for m in meshes[0].materials]))
	fileData = fileData.replace("__TRIANGLE_COUNT__", str(len(triangles)))
	fileData = fileData.replace("__TRIANGLES__", ",\n\t".join(["Triangle(%i, %i, %i, %i)" % (t[0], t[1], t[2], t[3]) for t in triangles]))
	fileData = fileData.replace("__MODELNAME__", modelname)
	fileData = fileData.replace("__MIN__", "%f, %f, %f" % (min[0], min[1], min[2]))
	fileData = fileData.replace("__MAX__", "%f, %f, %f" % (max[0], max[1], max[2]))
	
	file = open(filename, 'wb')
	file.write(fileData)
	file.close()


def main():
	Blender.Window.FileSelector(write, 'C Export', Blender.sys.makename(ext='.cpp'))


if __name__=='__main__':
	main()
