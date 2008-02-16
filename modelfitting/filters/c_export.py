#!BPY

"""
Name: 'C++ Model Exporter (.cpp)...'
Blender: 242
Group: 'Export'
Tooltip: 'Export selected mesh to C++ source file'
"""

fileTemplate = """#include "model.h"

int vertexCount = __VERTEX_COUNT__;
Vec4 vertexData[] =
{
	__VERTICES__
};

int initModel()
{
	Model* model = new Model("__MODELNAME__");
	model->setVertices(vertexCount, vertexData);
	model->setMinMax(Vec3(__MIN__), Vec3(__MAX__));
}

int a = initModel(); // Hacky way to get a static code block
"""

__author__ = "David Sansome"

import Blender
import BPyMesh
import os
from struct import pack

def write(filename):
	if not filename.lower().endswith('.cpp'):
		filename += '.cpp'
	modelname = os.path.splitext(os.path.basename(filename))[0]
	
	scene = Blender.Scene.GetCurrent()
	
	# Get list of meshes
	meshes = []
	for o in scene.objects:
		if o.type == 'Mesh':
			mesh = BPyMesh.getMeshFromObject(o, None, True, False, scene)
        		mesh.transform(o.matrixWorld)
			meshes.append(mesh)
	
	# Get list of vertices.  Also calculate min and max points
	verts = []
	min = [float("inf"), float("inf"), float("inf")]
	max = [-float("inf"), -float("inf"), -float("inf")]
	for mesh in meshes:
		for v in mesh.verts:
			verts.append(v.co)
			if v.co[0] < min[0]: min[0] = v.co[0]
			if v.co[1] < min[1]: min[1] = v.co[1]
			if v.co[2] < min[2]: min[2] = v.co[2]
			if v.co[0] > max[0]: max[0] = v.co[0]
			if v.co[1] > max[1]: max[1] = v.co[1]
			if v.co[2] > max[2]: max[2] = v.co[2]
	
	fileData = fileTemplate
	fileData = fileData.replace("__VERTEX_COUNT__", str(len(verts)))
	fileData = fileData.replace("__VERTICES__", ",\n\t".join(["Vec4(%f, %f, %f, 1.0)" % (v[0], v[1], v[2]) for v in verts]))
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
