#include "model.h"

const int vertexCount = 319;
Vertex vertexData[] =
{
	Vertex(Vec4(-0.376747, -0.376746, -2.000000, 1.0), 1),
	Vertex(Vec4(0.532800, -0.000000, -3.200000, 1.0), 1),
	Vertex(Vec4(-0.772740, -0.207057, -1.200000, 1.0), 0),
	Vertex(Vec4(-0.188373, 0.188373, 0.000000, 1.0), 1),
	Vertex(Vec4(-0.207057, 0.772740, -0.800000, 1.0), 0),
	Vertex(Vec4(-0.565686, 0.565684, -1.200000, 1.0), 0),
	Vertex(Vec4(0.207056, -0.772740, -0.800000, 1.0), 0),
	Vertex(Vec4(0.772740, 0.207056, -0.800000, 1.0), 0),
	Vertex(Vec4(-0.376747, 0.376746, -2.800000, 1.0), 1),
	Vertex(Vec4(-0.207057, 0.772740, -1.600000, 1.0), 0),
	Vertex(Vec4(-0.000000, -0.266400, -2.800000, 1.0), 1),
	Vertex(Vec4(0.188373, -0.188373, -2.400000, 1.0), 1),
	Vertex(Vec4(-0.376747, -0.376746, 0.000000, 1.0), 1),
	Vertex(Vec4(-0.266400, -0.000000, -2.800000, 1.0), 1),
	Vertex(Vec4(0.376747, 0.376746, -3.200000, 1.0), 1),
	Vertex(Vec4(0.376747, -0.376747, -4.000000, 1.0), 1),
	Vertex(Vec4(-0.376747, -0.376746, -1.600000, 1.0), 1),
	Vertex(Vec4(-0.188373, -0.188373, -2.400000, 1.0), 1),
	Vertex(Vec4(-0.266400, -0.000000, 0.000000, 1.0), 1),
	Vertex(Vec4(-0.000000, -0.532800, -1.600000, 1.0), 1),
	Vertex(Vec4(-0.207057, 0.772740, -3.200000, 1.0), 0),
	Vertex(Vec4(0.565685, 0.565686, -2.000000, 1.0), 0),
	Vertex(Vec4(0.532800, -0.000000, -2.400000, 1.0), 1),
	Vertex(Vec4(-0.000000, 0.000000, -0.400000, 1.0), 1),
	Vertex(Vec4(-0.207055, -0.772741, -4.000000, 1.0), 0),
	Vertex(Vec4(0.376747, 0.376746, -2.400000, 1.0), 1),
	Vertex(Vec4(-0.532800, -0.000000, -1.600000, 1.0), 1),
	Vertex(Vec4(0.207054, 0.772741, -2.000000, 1.0), 0),
	Vertex(Vec4(0.772741, -0.207054, 0.000000, 1.0), 0),
	Vertex(Vec4(0.772740, 0.207056, -2.400000, 1.0), 0),
	Vertex(Vec4(-0.000000, 0.266400, -2.000000, 1.0), 1),
	Vertex(Vec4(-0.772741, 0.207054, -0.400000, 1.0), 0),
	Vertex(Vec4(-0.000000, 0.000000, -3.600000, 1.0), 1),
	Vertex(Vec4(-0.772740, -0.207057, -3.600000, 1.0), 0),
	Vertex(Vec4(-0.772741, -0.207056, -4.000000, 1.0), 0),
	Vertex(Vec4(-0.207057, 0.772740, 0.000000, 1.0), 0),
	Vertex(Vec4(-0.000000, 0.266400, -3.200000, 1.0), 1),
	Vertex(Vec4(-0.772740, -0.207057, -1.600000, 1.0), 0),
	Vertex(Vec4(-0.188373, 0.188373, -2.400000, 1.0), 1),
	Vertex(Vec4(-0.266400, -0.000000, -2.000000, 1.0), 1),
	Vertex(Vec4(0.207056, -0.772740, 0.000000, 1.0), 0),
	Vertex(Vec4(0.188373, 0.188373, -2.800000, 1.0), 1),
	Vertex(Vec4(-0.188373, -0.188373, 0.000000, 1.0), 1),
	Vertex(Vec4(0.207056, -0.772740, -1.600000, 1.0), 0),
	Vertex(Vec4(0.565686, -0.565685, -1.200000, 1.0), 0),
	Vertex(Vec4(0.565686, -0.565685, -1.600000, 1.0), 0),
	Vertex(Vec4(-0.266400, -0.000000, -1.200000, 1.0), 1),
	Vertex(Vec4(-0.532800, -0.000000, -2.000000, 1.0), 1),
	Vertex(Vec4(0.565685, 0.565686, -3.200000, 1.0), 0),
	Vertex(Vec4(0.565686, -0.565685, -0.400000, 1.0), 0),
	Vertex(Vec4(0.376747, -0.376747, -2.000000, 1.0), 1),
	Vertex(Vec4(0.565686, -0.565685, -3.600000, 1.0), 0),
	Vertex(Vec4(-0.376747, -0.376746, -2.800000, 1.0), 1),
	Vertex(Vec4(0.188373, -0.188373, -1.600000, 1.0), 1),
	Vertex(Vec4(0.772741, -0.207054, -0.800000, 1.0), 0),
	Vertex(Vec4(-0.565685, -0.565686, -2.400000, 1.0), 0),
	Vertex(Vec4(0.376747, -0.376747, -2.800000, 1.0), 1),
	Vertex(Vec4(0.188373, 0.188373, -1.200000, 1.0), 1),
	Vertex(Vec4(0.188373, -0.188373, -3.200000, 1.0), 1),
	Vertex(Vec4(0.532800, -0.000000, -3.600000, 1.0), 1),
	Vertex(Vec4(-0.376747, 0.376746, -1.200000, 1.0), 1),
	Vertex(Vec4(-0.207054, -0.772741, -1.200000, 1.0), 0),
	Vertex(Vec4(0.188373, -0.188373, -1.200000, 1.0), 1),
	Vertex(Vec4(0.772741, -0.207054, -2.400000, 1.0), 0),
	Vertex(Vec4(0.565686, -0.565685, -0.800000, 1.0), 0),
	Vertex(Vec4(0.376747, 0.376746, -1.600000, 1.0), 1),
	Vertex(Vec4(0.207054, 0.772741, -1.600000, 1.0), 0),
	Vertex(Vec4(0.188373, -0.188373, -0.800000, 1.0), 1),
	Vertex(Vec4(-0.000000, 0.266400, -3.600000, 1.0), 1),
	Vertex(Vec4(0.772740, 0.207056, 0.000000, 1.0), 0),
	Vertex(Vec4(-0.772741, 0.207054, -1.200000, 1.0), 0),
	Vertex(Vec4(0.188373, 0.188373, -2.000000, 1.0), 1),
	Vertex(Vec4(-0.532800, -0.000000, -3.600000, 1.0), 1),
	Vertex(Vec4(0.376747, 0.376746, 0.000000, 1.0), 1),
	Vertex(Vec4(-0.000000, 0.266400, -1.200000, 1.0), 1),
	Vertex(Vec4(0.266400, -0.000000, -0.400000, 1.0), 1),
	Vertex(Vec4(-0.772741, 0.207054, 0.000000, 1.0), 0),
	Vertex(Vec4(0.772741, -0.207055, -4.000000, 1.0), 0),
	Vertex(Vec4(-0.376747, 0.376746, 0.000000, 1.0), 1),
	Vertex(Vec4(0.772740, 0.207056, -3.200000, 1.0), 0),
	Vertex(Vec4(0.532800, -0.000000, -1.600000, 1.0), 1),
	Vertex(Vec4(-0.565685, -0.565686, 0.000000, 1.0), 0),
	Vertex(Vec4(0.376747, -0.376747, -1.600000, 1.0), 1),
	Vertex(Vec4(-0.000000, -0.532800, -3.600000, 1.0), 1),
	Vertex(Vec4(0.376747, -0.376747, -0.400000, 1.0), 1),
	Vertex(Vec4(0.188373, 0.188373, -3.200000, 1.0), 1),
	Vertex(Vec4(-0.000000, 0.532800, -2.000000, 1.0), 1),
	Vertex(Vec4(-0.532800, -0.000000, -0.400000, 1.0), 1),
	Vertex(Vec4(-0.000000, 0.000000, -4.000000, 1.0), 1),
	Vertex(Vec4(0.532800, -0.000000, -2.000000, 1.0), 1),
	Vertex(Vec4(-0.207054, -0.772741, -2.800000, 1.0), 0),
	Vertex(Vec4(0.207054, 0.772741, -3.600000, 1.0), 0),
	Vertex(Vec4(-0.376747, 0.376746, -2.000000, 1.0), 1),
	Vertex(Vec4(0.565686, -0.565685, 0.000000, 1.0), 0),
	Vertex(Vec4(-0.266400, -0.000000, -0.400000, 1.0), 1),
	Vertex(Vec4(-0.188373, -0.188373, -2.800000, 1.0), 1),
	Vertex(Vec4(-0.207054, -0.772741, -2.400000, 1.0), 0),
	Vertex(Vec4(0.376747, 0.376746, -4.000000, 1.0), 1),
	Vertex(Vec4(-0.207057, 0.772740, -1.200000, 1.0), 0),
	Vertex(Vec4(-0.000000, -0.532800, -0.800000, 1.0), 1),
	Vertex(Vec4(-0.000000, 0.266400, -2.400000, 1.0), 1),
	Vertex(Vec4(-0.188373, 0.188373, -1.200000, 1.0), 1),
	Vertex(Vec4(-0.207054, -0.772741, 0.000000, 1.0), 0),
	Vertex(Vec4(-0.772741, 0.207054, -2.000000, 1.0), 0),
	Vertex(Vec4(0.207054, 0.772741, -2.800000, 1.0), 0),
	Vertex(Vec4(-0.000000, -0.266400, -3.200000, 1.0), 1),
	Vertex(Vec4(-0.772741, 0.207054, -2.800000, 1.0), 0),
	Vertex(Vec4(-0.000000, -0.266400, -2.000000, 1.0), 1),
	Vertex(Vec4(0.207056, -0.772740, -2.000000, 1.0), 0),
	Vertex(Vec4(-0.565686, 0.565684, -2.400000, 1.0), 0),
	Vertex(Vec4(-0.188373, 0.188373, -2.800000, 1.0), 1),
	Vertex(Vec4(0.188373, -0.188373, -0.400000, 1.0), 1),
	Vertex(Vec4(0.565685, 0.565685, -4.000000, 1.0), 0),
	Vertex(Vec4(-0.772740, -0.207057, -3.200000, 1.0), 0),
	Vertex(Vec4(0.772740, 0.207056, -1.200000, 1.0), 0),
	Vertex(Vec4(-0.376747, 0.376746, -3.200000, 1.0), 1),
	Vertex(Vec4(0.772740, 0.207056, -1.600000, 1.0), 0),
	Vertex(Vec4(0.207055, -0.772741, -4.000000, 1.0), 0),
	Vertex(Vec4(-0.000000, 0.000000, -2.800000, 1.0), 1),
	Vertex(Vec4(0.207056, -0.772740, -2.400000, 1.0), 0),
	Vertex(Vec4(-0.565686, 0.565684, -3.200000, 1.0), 0),
	Vertex(Vec4(-0.000000, -0.532800, -2.400000, 1.0), 1),
	Vertex(Vec4(-0.188373, -0.188373, -4.000000, 1.0), 1),
	Vertex(Vec4(-0.565686, 0.565684, -0.800000, 1.0), 0),
	Vertex(Vec4(0.376747, -0.376747, -2.400000, 1.0), 1),
	Vertex(Vec4(-0.188373, 0.188373, -3.600000, 1.0), 1),
	Vertex(Vec4(-0.376747, -0.376746, -2.400000, 1.0), 1),
	Vertex(Vec4(0.266400, -0.000000, -1.600000, 1.0), 1),
	Vertex(Vec4(-0.772741, 0.207054, -3.600000, 1.0), 0),
	Vertex(Vec4(-0.207057, 0.772740, -2.800000, 1.0), 0),
	Vertex(Vec4(0.376747, 0.376746, -1.200000, 1.0), 1),
	Vertex(Vec4(-0.376747, 0.376746, -4.000000, 1.0), 1),
	Vertex(Vec4(-0.565685, -0.565686, -3.200000, 1.0), 0),
	Vertex(Vec4(0.207054, 0.772741, -0.800000, 1.0), 0),
	Vertex(Vec4(-0.376747, -0.376746, -3.200000, 1.0), 1),
	Vertex(Vec4(-0.188373, -0.188373, -3.200000, 1.0), 1),
	Vertex(Vec4(-0.000000, 0.532800, 0.000000, 1.0), 1),
	Vertex(Vec4(-0.565685, -0.565686, -0.800000, 1.0), 0),
	Vertex(Vec4(-0.000000, -0.266400, -1.600000, 1.0), 1),
	Vertex(Vec4(0.772741, -0.207054, -2.800000, 1.0), 0),
	Vertex(Vec4(-0.000000, 0.532800, -0.400000, 1.0), 1),
	Vertex(Vec4(-0.376747, 0.376746, -2.400000, 1.0), 1),
	Vertex(Vec4(-0.207054, -0.772741, -3.600000, 1.0), 0),
	Vertex(Vec4(-0.000000, 0.266400, -0.800000, 1.0), 1),
	Vertex(Vec4(-0.000000, -0.532800, -0.400000, 1.0), 1),
	Vertex(Vec4(-0.207054, -0.772741, -1.600000, 1.0), 0),
	Vertex(Vec4(0.772740, 0.207056, -2.000000, 1.0), 0),
	Vertex(Vec4(-0.772740, -0.207057, -0.800000, 1.0), 0),
	Vertex(Vec4(-0.532800, -0.000000, -2.800000, 1.0), 1),
	Vertex(Vec4(0.565685, 0.565686, -0.400000, 1.0), 0),
	Vertex(Vec4(-0.565686, 0.565684, -2.800000, 1.0), 0),
	Vertex(Vec4(-0.376747, -0.376746, -4.000000, 1.0), 1),
	Vertex(Vec4(0.376747, -0.376747, -0.800000, 1.0), 1),
	Vertex(Vec4(-0.000000, 0.000000, -2.000000, 1.0), 1),
	Vertex(Vec4(-0.188373, 0.188373, -0.400000, 1.0), 1),
	Vertex(Vec4(-0.266400, -0.000000, -1.600000, 1.0), 1),
	Vertex(Vec4(-0.188373, -0.188373, -3.600000, 1.0), 1),
	Vertex(Vec4(0.376747, 0.376746, -0.800000, 1.0), 1),
	Vertex(Vec4(-0.000000, 0.266400, -4.000000, 1.0), 1),
	Vertex(Vec4(-0.376747, -0.376746, -3.600000, 1.0), 1),
	Vertex(Vec4(-0.000000, 0.532800, -2.400000, 1.0), 1),
	Vertex(Vec4(0.532800, -0.000000, -1.200000, 1.0), 1),
	Vertex(Vec4(-0.188373, 0.188373, -4.000000, 1.0), 1),
	Vertex(Vec4(0.565685, 0.565686, -3.600000, 1.0), 0),
	Vertex(Vec4(-0.188373, 0.188373, -0.800000, 1.0), 1),
	Vertex(Vec4(0.188373, 0.188373, -4.000000, 1.0), 1),
	Vertex(Vec4(-0.772741, 0.207054, -3.200000, 1.0), 0),
	Vertex(Vec4(-0.207057, 0.772740, -0.400000, 1.0), 0),
	Vertex(Vec4(-0.207057, 0.772740, -2.400000, 1.0), 0),
	Vertex(Vec4(-0.188373, -0.188373, -0.400000, 1.0), 1),
	Vertex(Vec4(-0.266400, -0.000000, -0.800000, 1.0), 1),
	Vertex(Vec4(-0.266400, -0.000000, -3.600000, 1.0), 1),
	Vertex(Vec4(-0.000000, 0.000000, -1.200000, 1.0), 1),
	Vertex(Vec4(0.565685, 0.565686, -2.400000, 1.0), 0),
	Vertex(Vec4(-0.000000, -0.532800, -2.800000, 1.0), 1),
	Vertex(Vec4(-0.000000, -0.532800, -4.000000, 1.0), 1),
	Vertex(Vec4(0.207056, -0.772740, -3.200000, 1.0), 0),
	Vertex(Vec4(-0.772740, -0.207057, -2.000000, 1.0), 0),
	Vertex(Vec4(-0.376747, 0.376746, -3.600000, 1.0), 1),
	Vertex(Vec4(-0.188373, 0.188373, -3.200000, 1.0), 1),
	Vertex(Vec4(-0.188373, -0.188373, -0.800000, 1.0), 1),
	Vertex(Vec4(0.266400, -0.000000, -4.000000, 1.0), 1),
	Vertex(Vec4(-0.565685, -0.565686, -3.600000, 1.0), 0),
	Vertex(Vec4(-0.000000, 0.000000, -0.800000, 1.0), 1),
	Vertex(Vec4(-0.000000, 0.532800, -1.600000, 1.0), 1),
	Vertex(Vec4(-0.207054, -0.772741, -2.000000, 1.0), 0),
	Vertex(Vec4(0.266400, -0.000000, -2.400000, 1.0), 1),
	Vertex(Vec4(0.188373, 0.188373, -0.400000, 1.0), 1),
	Vertex(Vec4(-0.000000, 0.532800, -0.800000, 1.0), 1),
	Vertex(Vec4(0.188373, 0.188373, -2.400000, 1.0), 1),
	Vertex(Vec4(-0.565686, 0.565684, -1.600000, 1.0), 0),
	Vertex(Vec4(0.532800, -0.000000, 0.000000, 1.0), 1),
	Vertex(Vec4(0.207054, 0.772741, 0.000000, 1.0), 0),
	Vertex(Vec4(0.207055, 0.772741, -4.000000, 1.0), 0),
	Vertex(Vec4(-0.772740, -0.207057, -2.800000, 1.0), 0),
	Vertex(Vec4(-0.000000, -0.532800, -3.200000, 1.0), 1),
	Vertex(Vec4(0.266400, -0.000000, -3.200000, 1.0), 1),
	Vertex(Vec4(-0.000000, 0.000000, -1.600000, 1.0), 1),
	Vertex(Vec4(0.376747, -0.376747, -3.600000, 1.0), 1),
	Vertex(Vec4(0.207056, -0.772740, -0.400000, 1.0), 0),
	Vertex(Vec4(-0.188373, -0.188373, -1.600000, 1.0), 1),
	Vertex(Vec4(-0.266400, -0.000000, -4.000000, 1.0), 1),
	Vertex(Vec4(-0.772740, -0.207057, -2.400000, 1.0), 0),
	Vertex(Vec4(-0.000000, 0.266400, -0.400000, 1.0), 1),
	Vertex(Vec4(-0.772741, 0.207055, -4.000000, 1.0), 0),
	Vertex(Vec4(0.207054, 0.772741, -3.200000, 1.0), 0),
	Vertex(Vec4(0.565685, 0.565686, -1.600000, 1.0), 0),
	Vertex(Vec4(-0.000000, -0.266400, -3.600000, 1.0), 1),
	Vertex(Vec4(-0.565685, -0.565686, -1.600000, 1.0), 0),
	Vertex(Vec4(-0.565686, 0.565685, -4.000000, 1.0), 0),
	Vertex(Vec4(0.772741, -0.207054, -3.600000, 1.0), 0),
	Vertex(Vec4(-0.376747, 0.376746, -0.400000, 1.0), 1),
	Vertex(Vec4(-0.565685, -0.565686, -2.000000, 1.0), 0),
	Vertex(Vec4(0.188373, -0.188373, -4.000000, 1.0), 1),
	Vertex(Vec4(0.772741, -0.207054, -1.600000, 1.0), 0),
	Vertex(Vec4(0.188373, 0.188373, -0.800000, 1.0), 1),
	Vertex(Vec4(-0.000000, -0.266400, -1.200000, 1.0), 1),
	Vertex(Vec4(0.207054, 0.772741, -2.400000, 1.0), 0),
	Vertex(Vec4(0.565686, -0.565685, -2.400000, 1.0), 0),
	Vertex(Vec4(-0.376747, -0.376746, -0.800000, 1.0), 1),
	Vertex(Vec4(0.376747, 0.376746, -2.000000, 1.0), 1),
	Vertex(Vec4(0.565685, 0.565686, -2.800000, 1.0), 0),
	Vertex(Vec4(-0.207054, -0.772741, -3.200000, 1.0), 0),
	Vertex(Vec4(0.188373, -0.188373, -3.600000, 1.0), 1),
	Vertex(Vec4(0.376747, 0.376746, -0.400000, 1.0), 1),
	Vertex(Vec4(-0.565686, 0.565684, -2.000000, 1.0), 0),
	Vertex(Vec4(0.772740, 0.207056, -2.800000, 1.0), 0),
	Vertex(Vec4(-0.565685, -0.565686, -4.000000, 1.0), 0),
	Vertex(Vec4(-0.376747, 0.376746, -1.600000, 1.0), 1),
	Vertex(Vec4(0.772741, 0.207055, -4.000000, 1.0), 0),
	Vertex(Vec4(-0.565686, 0.565684, -0.400000, 1.0), 0),
	Vertex(Vec4(-0.000000, 0.266400, -2.800000, 1.0), 1),
	Vertex(Vec4(0.565685, -0.565685, -4.000000, 1.0), 0),
	Vertex(Vec4(-0.565685, -0.565686, -0.400000, 1.0), 0),
	Vertex(Vec4(-0.565686, 0.565684, 0.000000, 1.0), 0),
	Vertex(Vec4(-0.532800, -0.000000, -4.000000, 1.0), 1),
	Vertex(Vec4(0.376747, -0.376747, 0.000000, 1.0), 1),
	Vertex(Vec4(0.266400, -0.000000, -0.800000, 1.0), 1),
	Vertex(Vec4(0.376747, 0.376746, -2.800000, 1.0), 1),
	Vertex(Vec4(-0.376747, -0.376746, -0.400000, 1.0), 1),
	Vertex(Vec4(-0.532800, -0.000000, 0.000000, 1.0), 1),
	Vertex(Vec4(0.188373, 0.188373, -1.600000, 1.0), 1),
	Vertex(Vec4(-0.000000, 0.532800, -2.800000, 1.0), 1),
	Vertex(Vec4(-0.000000, -0.266400, 0.000000, 1.0), 1),
	Vertex(Vec4(-0.000000, -0.532800, -1.200000, 1.0), 1),
	Vertex(Vec4(-0.188373, -0.188373, -2.000000, 1.0), 1),
	Vertex(Vec4(-0.772740, -0.207057, 0.000000, 1.0), 0),
	Vertex(Vec4(0.266400, -0.000000, -1.200000, 1.0), 1),
	Vertex(Vec4(0.565685, 0.565686, -1.200000, 1.0), 0),
	Vertex(Vec4(-0.532800, -0.000000, -0.800000, 1.0), 1),
	Vertex(Vec4(0.207054, 0.772741, -1.200000, 1.0), 0),
	Vertex(Vec4(-0.532800, -0.000000, -2.400000, 1.0), 1),
	Vertex(Vec4(-0.207057, 0.772740, -2.000000, 1.0), 0),
	Vertex(Vec4(0.207056, -0.772740, -3.600000, 1.0), 0),
	Vertex(Vec4(-0.000000, -0.266400, -0.400000, 1.0), 1),
	Vertex(Vec4(0.565686, -0.565685, -3.200000, 1.0), 0),
	Vertex(Vec4(-0.207054, -0.772741, -0.800000, 1.0), 0),
	Vertex(Vec4(-0.000000, -0.266400, -4.000000, 1.0), 1),
	Vertex(Vec4(0.207056, -0.772740, -1.200000, 1.0), 0),
	Vertex(Vec4(0.266400, -0.000000, -2.800000, 1.0), 1),
	Vertex(Vec4(0.188373, 0.188373, 0.000000, 1.0), 1),
	Vertex(Vec4(0.266400, -0.000000, -3.600000, 1.0), 1),
	Vertex(Vec4(-0.207054, -0.772741, -0.400000, 1.0), 0),
	Vertex(Vec4(0.532800, -0.000000, -4.000000, 1.0), 1),
	Vertex(Vec4(0.772741, -0.207054, -2.000000, 1.0), 0),
	Vertex(Vec4(0.772741, -0.207054, -0.400000, 1.0), 0),
	Vertex(Vec4(0.532800, -0.000000, -0.400000, 1.0), 1),
	Vertex(Vec4(-0.376747, -0.376746, -1.200000, 1.0), 1),
	Vertex(Vec4(-0.000000, 0.266400, 0.000000, 1.0), 1),
	Vertex(Vec4(0.188373, -0.188373, 0.000000, 1.0), 1),
	Vertex(Vec4(-0.207057, 0.772740, -3.600000, 1.0), 0),
	Vertex(Vec4(0.532800, -0.000000, -0.800000, 1.0), 1),
	Vertex(Vec4(0.532800, -0.000000, -2.800000, 1.0), 1),
	Vertex(Vec4(0.207054, 0.772741, -0.400000, 1.0), 0),
	Vertex(Vec4(-0.207056, 0.772740, -4.000000, 1.0), 0),
	Vertex(Vec4(-0.000000, 0.532800, -4.000000, 1.0), 1),
	Vertex(Vec4(0.565686, -0.565685, -2.800000, 1.0), 0),
	Vertex(Vec4(-0.000000, 0.266400, -1.600000, 1.0), 1),
	Vertex(Vec4(0.772740, 0.207056, -3.600000, 1.0), 0),
	Vertex(Vec4(0.565685, 0.565686, -0.800000, 1.0), 0),
	Vertex(Vec4(-0.772741, 0.207054, -2.400000, 1.0), 0),
	Vertex(Vec4(0.188373, 0.188373, -3.600000, 1.0), 1),
	Vertex(Vec4(-0.532800, -0.000000, -3.200000, 1.0), 1),
	Vertex(Vec4(0.376747, -0.376747, -1.200000, 1.0), 1),
	Vertex(Vec4(-0.772740, -0.207057, -0.400000, 1.0), 0),
	Vertex(Vec4(-0.000000, -0.266400, -2.400000, 1.0), 1),
	Vertex(Vec4(-0.000000, 0.532800, -3.600000, 1.0), 1),
	Vertex(Vec4(0.772741, -0.207054, -3.200000, 1.0), 0),
	Vertex(Vec4(0.207056, -0.772740, -2.800000, 1.0), 0),
	Vertex(Vec4(-0.188373, 0.188373, -1.600000, 1.0), 1),
	Vertex(Vec4(0.772741, -0.207054, -1.200000, 1.0), 0),
	Vertex(Vec4(-0.565685, -0.565686, -1.200000, 1.0), 0),
	Vertex(Vec4(-0.266400, -0.000000, -2.400000, 1.0), 1),
	Vertex(Vec4(-0.000000, 0.000000, 0.000000, 1.0), 1),
	Vertex(Vec4(-0.188373, 0.188373, -2.000000, 1.0), 1),
	Vertex(Vec4(-0.376747, 0.376746, -0.800000, 1.0), 1),
	Vertex(Vec4(-0.000000, 0.000000, -2.400000, 1.0), 1),
	Vertex(Vec4(-0.266400, -0.000000, -3.200000, 1.0), 1),
	Vertex(Vec4(-0.532800, -0.000000, -1.200000, 1.0), 1),
	Vertex(Vec4(0.376747, 0.376746, -3.600000, 1.0), 1),
	Vertex(Vec4(-0.000000, -0.266400, -0.800000, 1.0), 1),
	Vertex(Vec4(0.376747, -0.376747, -3.200000, 1.0), 1),
	Vertex(Vec4(-0.565685, -0.565686, -2.800000, 1.0), 0),
	Vertex(Vec4(-0.772741, 0.207054, -1.600000, 1.0), 0),
	Vertex(Vec4(0.266400, -0.000000, 0.000000, 1.0), 1),
	Vertex(Vec4(-0.000000, 0.532800, -3.200000, 1.0), 1),
	Vertex(Vec4(-0.772741, 0.207054, -0.800000, 1.0), 0),
	Vertex(Vec4(0.772740, 0.207056, -0.400000, 1.0), 0),
	Vertex(Vec4(0.565686, -0.565685, -2.000000, 1.0), 0),
	Vertex(Vec4(-0.188373, -0.188373, -1.200000, 1.0), 1),
	Vertex(Vec4(-0.000000, -0.532800, -2.000000, 1.0), 1),
	Vertex(Vec4(-0.000000, -0.532800, 0.000000, 1.0), 1),
	Vertex(Vec4(-0.565686, 0.565684, -3.600000, 1.0), 0),
	Vertex(Vec4(0.188373, -0.188373, -2.000000, 1.0), 1),
	Vertex(Vec4(-0.000000, 0.532800, -1.200000, 1.0), 1),
	Vertex(Vec4(0.188373, -0.188373, -2.800000, 1.0), 1),
	Vertex(Vec4(0.266400, -0.000000, -2.000000, 1.0), 1),
	Vertex(Vec4(0.565685, 0.565686, 0.000000, 1.0), 0),
	Vertex(Vec4(-0.000000, 0.000000, -3.200000, 1.0), 1)
};

const int materialCount = 2;
Material materialData[] =
{
	Material(Vec3(1.000000, 1.000000, 1.000000)),
	Material(Vec3(0.000000, 0.000000, 0.000000))
};

const int triangleCount = 576;
Triangle triangleData[] =
{
	Triangle(112, 163, 278, 0),
	Triangle(112, 278, 229, 0),
	Triangle(229, 278, 210, 0),
	Triangle(229, 210, 77, 0),
	Triangle(77, 210, 51, 0),
	Triangle(77, 51, 232, 0),
	Triangle(232, 51, 253, 0),
	Triangle(232, 253, 117, 0),
	Triangle(117, 253, 142, 0),
	Triangle(117, 142, 24, 0),
	Triangle(24, 142, 182, 0),
	Triangle(24, 182, 227, 0),
	Triangle(227, 182, 33, 0),
	Triangle(227, 33, 34, 0),
	Triangle(34, 33, 128, 0),
	Triangle(34, 128, 204, 0),
	Triangle(204, 128, 312, 0),
	Triangle(204, 312, 209, 0),
	Triangle(209, 312, 270, 0),
	Triangle(209, 270, 274, 0),
	Triangle(274, 270, 91, 0),
	Triangle(274, 91, 193, 0),
	Triangle(163, 112, 193, 0),
	Triangle(163, 193, 91, 0),
	Triangle(278, 163, 48, 0),
	Triangle(278, 48, 79, 0),
	Triangle(210, 278, 79, 0),
	Triangle(210, 79, 287, 0),
	Triangle(51, 210, 287, 0),
	Triangle(51, 287, 255, 0),
	Triangle(253, 51, 255, 0),
	Triangle(253, 255, 176, 0),
	Triangle(142, 253, 176, 0),
	Triangle(142, 176, 222, 0),
	Triangle(182, 142, 222, 0),
	Triangle(182, 222, 132, 0),
	Triangle(33, 182, 132, 0),
	Triangle(33, 132, 113, 0),
	Triangle(128, 33, 113, 0),
	Triangle(128, 113, 166, 0),
	Triangle(312, 128, 166, 0),
	Triangle(312, 166, 120, 0),
	Triangle(270, 312, 120, 0),
	Triangle(270, 120, 20, 0),
	Triangle(91, 270, 20, 0),
	Triangle(91, 20, 205, 0),
	Triangle(163, 91, 205, 0),
	Triangle(163, 205, 48, 0),
	Triangle(79, 48, 221, 0),
	Triangle(79, 221, 226, 0),
	Triangle(287, 79, 226, 0),
	Triangle(287, 226, 139, 0),
	Triangle(255, 287, 139, 0),
	Triangle(255, 139, 276, 0),
	Triangle(176, 255, 276, 0),
	Triangle(176, 276, 288, 0),
	Triangle(222, 176, 288, 0),
	Triangle(222, 288, 90, 0),
	Triangle(132, 222, 90, 0),
	Triangle(132, 90, 302, 0),
	Triangle(113, 132, 302, 0),
	Triangle(113, 302, 194, 0),
	Triangle(166, 113, 194, 0),
	Triangle(166, 194, 106, 0),
	Triangle(120, 166, 106, 0),
	Triangle(120, 106, 150, 0),
	Triangle(20, 120, 150, 0),
	Triangle(20, 150, 129, 0),
	Triangle(205, 20, 129, 0),
	Triangle(205, 129, 104, 0),
	Triangle(48, 205, 104, 0),
	Triangle(48, 104, 221, 0),
	Triangle(226, 221, 173, 0),
	Triangle(226, 173, 29, 0),
	Triangle(139, 226, 29, 0),
	Triangle(139, 29, 63, 0),
	Triangle(276, 139, 63, 0),
	Triangle(276, 63, 218, 0),
	Triangle(288, 276, 218, 0),
	Triangle(288, 218, 119, 0),
	Triangle(90, 288, 119, 0),
	Triangle(90, 119, 96, 0),
	Triangle(302, 90, 96, 0),
	Triangle(302, 96, 55, 0),
	Triangle(194, 302, 55, 0),
	Triangle(194, 55, 202, 0),
	Triangle(106, 194, 202, 0),
	Triangle(106, 202, 280, 0),
	Triangle(150, 106, 280, 0),
	Triangle(150, 280, 109, 0),
	Triangle(129, 150, 109, 0),
	Triangle(129, 109, 168, 0),
	Triangle(104, 129, 168, 0),
	Triangle(104, 168, 217, 0),
	Triangle(221, 104, 217, 0),
	Triangle(221, 217, 173, 0),
	Triangle(29, 173, 21, 0),
	Triangle(29, 21, 146, 0),
	Triangle(63, 29, 146, 0),
	Triangle(63, 146, 264, 0),
	Triangle(218, 63, 264, 0),
	Triangle(218, 264, 308, 0),
	Triangle(119, 218, 308, 0),
	Triangle(119, 308, 108, 0),
	Triangle(96, 119, 108, 0),
	Triangle(96, 108, 185, 0),
	Triangle(55, 96, 185, 0),
	Triangle(55, 185, 212, 0),
	Triangle(202, 55, 212, 0),
	Triangle(202, 212, 177, 0),
	Triangle(280, 202, 177, 0),
	Triangle(280, 177, 103, 0),
	Triangle(109, 280, 103, 0),
	Triangle(109, 103, 225, 0),
	Triangle(168, 109, 225, 0),
	Triangle(168, 225, 252, 0),
	Triangle(217, 168, 252, 0),
	Triangle(217, 252, 27, 0),
	Triangle(173, 217, 27, 0),
	Triangle(173, 27, 21, 0),
	Triangle(146, 21, 206, 0),
	Triangle(146, 206, 116, 0),
	Triangle(264, 146, 116, 0),
	Triangle(264, 116, 214, 0),
	Triangle(308, 264, 214, 0),
	Triangle(308, 214, 45, 0),
	Triangle(108, 308, 45, 0),
	Triangle(108, 45, 43, 0),
	Triangle(185, 108, 43, 0),
	Triangle(185, 43, 145, 0),
	Triangle(212, 185, 145, 0),
	Triangle(212, 145, 208, 0),
	Triangle(177, 212, 208, 0),
	Triangle(177, 208, 37, 0),
	Triangle(103, 177, 37, 0),
	Triangle(103, 37, 303, 0),
	Triangle(225, 103, 303, 0),
	Triangle(225, 303, 190, 0),
	Triangle(252, 225, 190, 0),
	Triangle(252, 190, 9, 0),
	Triangle(27, 252, 9, 0),
	Triangle(27, 9, 66, 0),
	Triangle(21, 27, 66, 0),
	Triangle(21, 66, 206, 0),
	Triangle(116, 206, 248, 0),
	Triangle(116, 248, 114, 0),
	Triangle(214, 116, 114, 0),
	Triangle(214, 114, 290, 0),
	Triangle(45, 214, 290, 0),
	Triangle(45, 290, 44, 0),
	Triangle(43, 45, 44, 0),
	Triangle(43, 44, 258, 0),
	Triangle(145, 43, 258, 0),
	Triangle(145, 258, 61, 0),
	Triangle(208, 145, 61, 0),
	Triangle(208, 61, 291, 0),
	Triangle(37, 208, 291, 0),
	Triangle(37, 291, 2, 0),
	Triangle(303, 37, 2, 0),
	Triangle(303, 2, 70, 0),
	Triangle(190, 303, 70, 0),
	Triangle(190, 70, 5, 0),
	Triangle(9, 190, 5, 0),
	Triangle(9, 5, 98, 0),
	Triangle(66, 9, 98, 0),
	Triangle(66, 98, 250, 0),
	Triangle(206, 66, 250, 0),
	Triangle(206, 250, 248, 0),
	Triangle(114, 248, 279, 0),
	Triangle(114, 279, 7, 0),
	Triangle(290, 114, 7, 0),
	Triangle(290, 7, 54, 0),
	Triangle(44, 290, 54, 0),
	Triangle(44, 54, 64, 0),
	Triangle(258, 44, 64, 0),
	Triangle(258, 64, 6, 0),
	Triangle(61, 258, 6, 0),
	Triangle(61, 6, 256, 0),
	Triangle(291, 61, 256, 0),
	Triangle(291, 256, 137, 0),
	Triangle(2, 291, 137, 0),
	Triangle(2, 137, 147, 0),
	Triangle(70, 2, 147, 0),
	Triangle(70, 147, 306, 0),
	Triangle(5, 70, 306, 0),
	Triangle(5, 306, 123, 0),
	Triangle(98, 5, 123, 0),
	Triangle(98, 123, 4, 0),
	Triangle(250, 98, 4, 0),
	Triangle(250, 4, 133, 0),
	Triangle(248, 250, 133, 0),
	Triangle(248, 133, 279, 0),
	Triangle(7, 279, 149, 0),
	Triangle(7, 149, 307, 0),
	Triangle(54, 7, 307, 0),
	Triangle(54, 307, 265, 0),
	Triangle(64, 54, 265, 0),
	Triangle(64, 265, 49, 0),
	Triangle(6, 64, 49, 0),
	Triangle(6, 49, 199, 0),
	Triangle(256, 6, 199, 0),
	Triangle(256, 199, 262, 0),
	Triangle(137, 256, 262, 0),
	Triangle(137, 262, 233, 0),
	Triangle(147, 137, 233, 0),
	Triangle(147, 233, 284, 0),
	Triangle(306, 147, 284, 0),
	Triangle(306, 284, 31, 0),
	Triangle(123, 306, 31, 0),
	Triangle(123, 31, 230, 0),
	Triangle(4, 123, 230, 0),
	Triangle(4, 230, 167, 0),
	Triangle(133, 4, 167, 0),
	Triangle(133, 167, 273, 0),
	Triangle(279, 133, 273, 0),
	Triangle(279, 273, 149, 0),
	Triangle(307, 149, 317, 0),
	Triangle(307, 317, 69, 0),
	Triangle(265, 307, 69, 0),
	Triangle(265, 69, 28, 0),
	Triangle(49, 265, 28, 0),
	Triangle(49, 28, 93, 0),
	Triangle(199, 49, 93, 0),
	Triangle(199, 93, 40, 0),
	Triangle(262, 199, 40, 0),
	Triangle(262, 40, 102, 0),
	Triangle(233, 262, 102, 0),
	Triangle(233, 102, 81, 0),
	Triangle(284, 233, 81, 0),
	Triangle(284, 81, 246, 0),
	Triangle(31, 284, 246, 0),
	Triangle(31, 246, 76, 0),
	Triangle(230, 31, 76, 0),
	Triangle(230, 76, 234, 0),
	Triangle(167, 230, 234, 0),
	Triangle(167, 234, 35, 0),
	Triangle(273, 167, 35, 0),
	Triangle(273, 35, 192, 0),
	Triangle(149, 273, 192, 0),
	Triangle(149, 192, 317, 0),
	Triangle(211, 87, 240, 1),
	Triangle(211, 240, 78, 1),
	Triangle(140, 211, 78, 1),
	Triangle(140, 78, 136, 1),
	Triangle(140, 136, 73, 1),
	Triangle(140, 73, 224, 1),
	Triangle(224, 73, 191, 1),
	Triangle(224, 191, 266, 1),
	Triangle(84, 266, 191, 1),
	Triangle(84, 191, 236, 1),
	Triangle(144, 84, 236, 1),
	Triangle(144, 236, 311, 1),
	Triangle(239, 144, 311, 1),
	Triangle(239, 311, 12, 1),
	Triangle(87, 239, 12, 1),
	Triangle(87, 12, 240, 1),
	Triangle(295, 249, 87, 1),
	Triangle(295, 87, 211, 1),
	Triangle(188, 295, 211, 1),
	Triangle(188, 211, 140, 1),
	Triangle(188, 140, 224, 1),
	Triangle(188, 224, 157, 1),
	Triangle(157, 224, 266, 1),
	Triangle(157, 266, 271, 1),
	Triangle(152, 271, 266, 1),
	Triangle(152, 266, 84, 1),
	Triangle(99, 152, 84, 1),
	Triangle(99, 84, 144, 1),
	Triangle(219, 99, 144, 1),
	Triangle(219, 144, 239, 1),
	Triangle(249, 219, 239, 1),
	Triangle(249, 239, 87, 1),
	Triangle(184, 65, 130, 1),
	Triangle(184, 130, 314, 1),
	Triangle(314, 130, 157, 1),
	Triangle(314, 157, 188, 1),
	Triangle(275, 97, 299, 1),
	Triangle(275, 299, 286, 1),
	Triangle(286, 299, 14, 1),
	Triangle(286, 14, 305, 1),
	Triangle(305, 14, 238, 1),
	Triangle(305, 238, 242, 1),
	Triangle(242, 238, 25, 1),
	Triangle(242, 25, 160, 1),
	Triangle(160, 25, 220, 1),
	Triangle(160, 220, 86, 1),
	Triangle(86, 220, 65, 1),
	Triangle(86, 65, 184, 1),
	Triangle(275, 286, 178, 1),
	Triangle(275, 178, 131, 1),
	Triangle(286, 305, 115, 1),
	Triangle(286, 115, 178, 1),
	Triangle(305, 242, 8, 1),
	Triangle(305, 8, 115, 1),
	Triangle(242, 160, 141, 1),
	Triangle(242, 141, 8, 1),
	Triangle(160, 86, 92, 1),
	Triangle(160, 92, 141, 1),
	Triangle(86, 184, 228, 1),
	Triangle(86, 228, 92, 1),
	Triangle(184, 314, 60, 1),
	Triangle(184, 60, 228, 1),
	Triangle(314, 188, 295, 1),
	Triangle(314, 295, 60, 1),
	Triangle(131, 178, 72, 1),
	Triangle(131, 72, 235, 1),
	Triangle(178, 115, 282, 1),
	Triangle(178, 282, 72, 1),
	Triangle(115, 8, 148, 1),
	Triangle(115, 148, 282, 1),
	Triangle(8, 141, 251, 1),
	Triangle(8, 251, 148, 1),
	Triangle(141, 92, 47, 1),
	Triangle(141, 47, 251, 1),
	Triangle(92, 228, 26, 1),
	Triangle(92, 26, 47, 1),
	Triangle(228, 60, 298, 1),
	Triangle(228, 298, 26, 1),
	Triangle(60, 295, 249, 1),
	Triangle(60, 249, 298, 1),
	Triangle(235, 72, 159, 1),
	Triangle(235, 159, 151, 1),
	Triangle(72, 282, 134, 1),
	Triangle(72, 134, 159, 1),
	Triangle(282, 148, 52, 1),
	Triangle(282, 52, 134, 1),
	Triangle(148, 251, 126, 1),
	Triangle(148, 126, 52, 1),
	Triangle(251, 47, 0, 1),
	Triangle(251, 0, 126, 1),
	Triangle(47, 26, 16, 1),
	Triangle(47, 16, 0, 1),
	Triangle(26, 298, 267, 1),
	Triangle(26, 267, 16, 1),
	Triangle(298, 249, 219, 1),
	Triangle(298, 219, 267, 1),
	Triangle(151, 159, 83, 1),
	Triangle(151, 83, 175, 1),
	Triangle(159, 134, 195, 1),
	Triangle(159, 195, 83, 1),
	Triangle(134, 52, 174, 1),
	Triangle(134, 174, 195, 1),
	Triangle(52, 126, 121, 1),
	Triangle(52, 121, 174, 1),
	Triangle(126, 0, 310, 1),
	Triangle(126, 310, 121, 1),
	Triangle(0, 16, 19, 1),
	Triangle(0, 19, 310, 1),
	Triangle(16, 267, 244, 1),
	Triangle(16, 244, 19, 1),
	Triangle(267, 219, 99, 1),
	Triangle(267, 99, 244, 1),
	Triangle(175, 83, 198, 1),
	Triangle(175, 198, 15, 1),
	Triangle(83, 195, 301, 1),
	Triangle(83, 301, 198, 1),
	Triangle(195, 174, 56, 1),
	Triangle(195, 56, 301, 1),
	Triangle(174, 121, 124, 1),
	Triangle(174, 124, 56, 1),
	Triangle(121, 310, 50, 1),
	Triangle(121, 50, 124, 1),
	Triangle(310, 19, 82, 1),
	Triangle(310, 82, 50, 1),
	Triangle(19, 244, 283, 1),
	Triangle(19, 283, 82, 1),
	Triangle(244, 99, 152, 1),
	Triangle(244, 152, 283, 1),
	Triangle(15, 198, 59, 1),
	Triangle(15, 59, 263, 1),
	Triangle(198, 301, 1, 1),
	Triangle(198, 1, 59, 1),
	Triangle(301, 56, 272, 1),
	Triangle(301, 272, 1, 1),
	Triangle(56, 124, 22, 1),
	Triangle(56, 22, 272, 1),
	Triangle(124, 50, 89, 1),
	Triangle(124, 89, 22, 1),
	Triangle(50, 82, 80, 1),
	Triangle(50, 80, 89, 1),
	Triangle(82, 283, 161, 1),
	Triangle(82, 161, 80, 1),
	Triangle(283, 152, 271, 1),
	Triangle(283, 271, 161, 1),
	Triangle(263, 59, 299, 1),
	Triangle(263, 299, 97, 1),
	Triangle(299, 59, 1, 1),
	Triangle(299, 1, 14, 1),
	Triangle(14, 1, 272, 1),
	Triangle(14, 272, 238, 1),
	Triangle(238, 272, 22, 1),
	Triangle(238, 22, 25, 1),
	Triangle(25, 22, 89, 1),
	Triangle(25, 89, 220, 1),
	Triangle(220, 89, 80, 1),
	Triangle(220, 80, 65, 1),
	Triangle(65, 80, 161, 1),
	Triangle(65, 161, 130, 1),
	Triangle(130, 161, 271, 1),
	Triangle(130, 271, 157, 1),
	Triangle(162, 158, 68, 1),
	Triangle(162, 68, 125, 1),
	Triangle(125, 68, 36, 1),
	Triangle(125, 36, 179, 1),
	Triangle(179, 36, 231, 1),
	Triangle(179, 231, 110, 1),
	Triangle(110, 231, 100, 1),
	Triangle(110, 100, 38, 1),
	Triangle(38, 100, 30, 1),
	Triangle(38, 30, 294, 1),
	Triangle(294, 30, 277, 1),
	Triangle(294, 277, 289, 1),
	Triangle(289, 277, 74, 1),
	Triangle(289, 74, 101, 1),
	Triangle(101, 74, 143, 1),
	Triangle(101, 143, 164, 1),
	Triangle(164, 143, 203, 1),
	Triangle(164, 203, 154, 1),
	Triangle(154, 203, 268, 1),
	Triangle(154, 268, 3, 1),
	Triangle(201, 162, 125, 1),
	Triangle(201, 125, 171, 1),
	Triangle(171, 125, 179, 1),
	Triangle(171, 179, 297, 1),
	Triangle(297, 179, 110, 1),
	Triangle(297, 110, 13, 1),
	Triangle(13, 110, 38, 1),
	Triangle(13, 38, 292, 1),
	Triangle(292, 38, 294, 1),
	Triangle(292, 294, 39, 1),
	Triangle(39, 294, 289, 1),
	Triangle(39, 289, 155, 1),
	Triangle(155, 289, 101, 1),
	Triangle(155, 101, 46, 1),
	Triangle(46, 101, 164, 1),
	Triangle(46, 164, 170, 1),
	Triangle(170, 164, 154, 1),
	Triangle(170, 154, 94, 1),
	Triangle(94, 154, 3, 1),
	Triangle(94, 3, 18, 1),
	Triangle(122, 201, 171, 1),
	Triangle(122, 171, 156, 1),
	Triangle(156, 171, 297, 1),
	Triangle(156, 297, 135, 1),
	Triangle(135, 297, 13, 1),
	Triangle(135, 13, 95, 1),
	Triangle(95, 13, 292, 1),
	Triangle(95, 292, 17, 1),
	Triangle(17, 292, 39, 1),
	Triangle(17, 39, 245, 1),
	Triangle(245, 39, 155, 1),
	Triangle(245, 155, 200, 1),
	Triangle(200, 155, 46, 1),
	Triangle(200, 46, 309, 1),
	Triangle(309, 46, 170, 1),
	Triangle(309, 170, 180, 1),
	Triangle(180, 170, 94, 1),
	Triangle(180, 94, 169, 1),
	Triangle(169, 94, 18, 1),
	Triangle(169, 18, 42, 1),
	Triangle(257, 122, 156, 1),
	Triangle(257, 156, 207, 1),
	Triangle(207, 156, 135, 1),
	Triangle(207, 135, 105, 1),
	Triangle(105, 135, 95, 1),
	Triangle(105, 95, 10, 1),
	Triangle(10, 95, 17, 1),
	Triangle(10, 17, 285, 1),
	Triangle(285, 17, 245, 1),
	Triangle(285, 245, 107, 1),
	Triangle(107, 245, 200, 1),
	Triangle(107, 200, 138, 1),
	Triangle(138, 200, 309, 1),
	Triangle(138, 309, 216, 1),
	Triangle(216, 309, 180, 1),
	Triangle(216, 180, 300, 1),
	Triangle(300, 180, 169, 1),
	Triangle(300, 169, 254, 1),
	Triangle(254, 169, 42, 1),
	Triangle(254, 42, 243, 1),
	Triangle(213, 257, 207, 1),
	Triangle(213, 207, 223, 1),
	Triangle(223, 207, 105, 1),
	Triangle(223, 105, 58, 1),
	Triangle(58, 105, 10, 1),
	Triangle(58, 10, 315, 1),
	Triangle(315, 10, 285, 1),
	Triangle(315, 285, 11, 1),
	Triangle(11, 285, 107, 1),
	Triangle(11, 107, 313, 1),
	Triangle(313, 107, 138, 1),
	Triangle(313, 138, 53, 1),
	Triangle(53, 138, 216, 1),
	Triangle(53, 216, 62, 1),
	Triangle(62, 216, 300, 1),
	Triangle(62, 300, 67, 1),
	Triangle(67, 300, 254, 1),
	Triangle(67, 254, 111, 1),
	Triangle(111, 254, 243, 1),
	Triangle(111, 243, 269, 1),
	Triangle(181, 213, 223, 1),
	Triangle(181, 223, 261, 1),
	Triangle(261, 223, 58, 1),
	Triangle(261, 58, 196, 1),
	Triangle(196, 58, 315, 1),
	Triangle(196, 315, 259, 1),
	Triangle(259, 315, 11, 1),
	Triangle(259, 11, 186, 1),
	Triangle(186, 11, 313, 1),
	Triangle(186, 313, 316, 1),
	Triangle(316, 313, 53, 1),
	Triangle(316, 53, 127, 1),
	Triangle(127, 53, 62, 1),
	Triangle(127, 62, 247, 1),
	Triangle(247, 62, 67, 1),
	Triangle(247, 67, 237, 1),
	Triangle(237, 67, 111, 1),
	Triangle(237, 111, 75, 1),
	Triangle(75, 111, 269, 1),
	Triangle(75, 269, 304, 1),
	Triangle(165, 181, 261, 1),
	Triangle(165, 261, 281, 1),
	Triangle(281, 261, 196, 1),
	Triangle(281, 196, 85, 1),
	Triangle(196, 259, 41, 1),
	Triangle(196, 41, 85, 1),
	Triangle(41, 259, 186, 1),
	Triangle(41, 186, 189, 1),
	Triangle(186, 316, 71, 1),
	Triangle(186, 71, 189, 1),
	Triangle(316, 127, 241, 1),
	Triangle(316, 241, 71, 1),
	Triangle(127, 247, 57, 1),
	Triangle(127, 57, 241, 1),
	Triangle(57, 247, 237, 1),
	Triangle(57, 237, 215, 1),
	Triangle(215, 237, 75, 1),
	Triangle(215, 75, 187, 1),
	Triangle(187, 75, 304, 1),
	Triangle(187, 304, 260, 1),
	Triangle(165, 281, 68, 1),
	Triangle(165, 68, 158, 1),
	Triangle(281, 85, 36, 1),
	Triangle(281, 36, 68, 1),
	Triangle(85, 41, 231, 1),
	Triangle(85, 231, 36, 1),
	Triangle(41, 189, 100, 1),
	Triangle(41, 100, 231, 1),
	Triangle(189, 71, 30, 1),
	Triangle(189, 30, 100, 1),
	Triangle(71, 241, 277, 1),
	Triangle(71, 277, 30, 1),
	Triangle(241, 57, 74, 1),
	Triangle(241, 74, 277, 1),
	Triangle(57, 215, 143, 1),
	Triangle(57, 143, 74, 1),
	Triangle(201, 88, 32, 1),
	Triangle(201, 32, 171, 1),
	Triangle(171, 32, 318, 1),
	Triangle(171, 318, 297, 1),
	Triangle(297, 318, 118, 1),
	Triangle(297, 118, 13, 1),
	Triangle(13, 118, 296, 1),
	Triangle(13, 296, 292, 1),
	Triangle(292, 296, 153, 1),
	Triangle(292, 153, 39, 1),
	Triangle(39, 153, 197, 1),
	Triangle(39, 197, 155, 1),
	Triangle(155, 197, 172, 1),
	Triangle(155, 172, 46, 1),
	Triangle(46, 172, 183, 1),
	Triangle(46, 183, 170, 1),
	Triangle(170, 183, 23, 1),
	Triangle(170, 23, 94, 1),
	Triangle(94, 23, 293, 1),
	Triangle(94, 293, 18, 1)
};

int initModel()
{
	Model* model = new Model("thighmodel");
	model->setVertices(vertexCount, vertexData);
	model->setMaterials(materialCount, materialData);
	model->setTriangles(triangleCount, triangleData);
	model->setMinMax(Vec3(-0.772741, -0.772741, -4.000000), Vec3(0.772741, 0.772741, 0.000000));
}

int a = initModel(); // Hacky way to get a static code block