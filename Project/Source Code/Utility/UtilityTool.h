/*******************************************************************
* Author	: wwyang
* Date		: 2017.12.10
* Copyright : Zhejinag Gongshang University
* Head File :
* Version   : 1.0
*********************************************************************/
// .NAME CUtilityTool -- utility tools

// .SECTION Description
// This class provides utility for various operations, such as loading an object file

// .SECTION See also

#ifndef _UTILITYTOOL_H_
#define _UTILITYTOOL_H_


#include "../Base/Mesh.h"
#include "../Base/varray.h"
#include "../Base/Vec3.h"

class CUtilityTool {
public:

	// Description:
	// Load a mesh model from an obj file
	static CMesh* LoadMeshFromObjFile(const char *filename);

	// Description:
	// Compute the affine transformation between two sets of corresponding points
	static bool ComputeAffineTransformation(const varray<Vec3>& arrayPointInit, const varray<Vec3>& arrayPointTarget, double M[9], double T[3]);
	static bool ComputeRegionOptimalAffineTransformation(const varray<Vec3>& arrayPointSetV1, const varray<Vec3>& arrayPointSetV2, double pLinearTransform[9], double T[3]);
	static bool ComputeRigidTransformation(const varray<Vec3>& arrayPointInit, const varray<Vec3>& arrayPointTarget, double M[9], double T[3]);
	static bool ComputeSimilarityTransformation(const varray<Vec3>& arrayPointInit, const varray<Vec3>& arrayPointTarget, double M[9], double T[3]);

	// Description:
	// Manipulations of 3*3 matrix
	static void TransposeMatrix(const double matrix[9], double transM[9]);
	static void MultiplyMatrix(const double matrix1[9], const double matrix2[9], double mulM[9]);
	static void MultiplyMatrixVector(const double matrix[9], const double v[3], double mulV[3]);

	// Description:
	// Basic math operations
	static double ComputeAngle2D(const Vec2 &vec1, const Vec2 &vec2);

	// Description:
	// Sort the data by bubble sort with increment order.
	// The sort order is in array of sort.
	template<typename T>
	static void BubbleSort(const T* data, int* sort, int size);

	// Description:
	// Template functions for exchange two items 
	template<typename T>
	static void Exchange(T& a, T& b)
	{
		T c = a;
		a = b;
		b = c;
	}
	
};

template<typename T>
void CUtilityTool::BubbleSort(const T* data, int* sort, int size)
{
	bool exchange = false;
	T *pdata = NULL;
	pdata = new T[size];

	int i;
	for (i = 0; i < size; i++) {
		pdata[i] = data[i];
	}
	for (i = 0; i < size; i++) {
		sort[i] = i;
	}
	for (i = 0; i < size; i++) {// move the max to size - i -1  
		exchange = false;
		for (int j = 1; j < size - i; j++) {
			if (pdata[j] < pdata[j - 1]) {
				Exchange(pdata[j], pdata[j - 1]);
				Exchange(sort[j], sort[j - 1]);
				exchange = true;
			}
		}
		if (!exchange) {
			break;
		}
	}
	delete[]pdata;
	pdata = 0;

	return;
}


#endif