#include "UtilityTool.h"

#include "../3d_Party/OBJ_Loader/OBJ_Loader.h"
#include "opencv2/opencv.hpp"

#include "assert.h"

CMesh* CUtilityTool::LoadMeshFromObjFile(const char *filename)
{
	// Initialize the obj loader
	objl::Loader loader;

	// Load the .obj file of the teeth
	bool bLoaded = loader.LoadFile(filename);

	if (!bLoaded) return 0;
	//assert(loader.LoadedMeshes.size() == 1);

	// Convert the loaded mesh into CMesh structure -- no material properties
	CMesh *pMesh = new CMesh;
	
	// Calculate the mesh vertices and faces
	int nMesh = loader.LoadedMeshes.size();
	int iMesh, i, nTotV = 0, nTotFace = 0;

	for (iMesh = 0; iMesh < nMesh; iMesh++) {
		objl::Mesh &mesh = loader.LoadedMeshes[iMesh];
		int nV = mesh.Vertices.size(), nFace = mesh.Indices.size() / 3;

		nTotV += nV; nTotFace += nFace;
	}

	pMesh->SetVSize(nTotV);
	pMesh->SetFSize(nTotFace);
	
	nTotV = 0; nTotFace = 0;
	for (iMesh = 0; iMesh < nMesh; iMesh++) {
		objl::Mesh &mesh = loader.LoadedMeshes[iMesh];
		int nV = mesh.Vertices.size(), nFace = mesh.Indices.size() / 3;

		int nMeshVIndexSt = nTotV;
		for (i = 0; i < nV; i++, nTotV++) {
			objl::Vector3 &vPos = mesh.Vertices[i].Position, &vNormal = mesh.Vertices[i].Normal;
			pMesh->GetV(nTotV).Pos().Set(vPos.X, vPos.Y, vPos.Z);
			pMesh->GetV(nTotV).Normal().Set(vNormal.X, vNormal.Y, vNormal.Z);

			pMesh->GetV(nTotV).Normal() = pMesh->GetV(nTotV).Normal().Normalize();
		}

		for (i = 0; i < nFace; i++, nTotFace++) {
			pMesh->GetF(nTotFace).SetVIndex(mesh.Indices[3 * i] + nMeshVIndexSt, mesh.Indices[3 * i + 1] + nMeshVIndexSt, mesh.Indices[3 * i + 2] + nMeshVIndexSt);
		}
	}

	//pMesh->ComputeCenter();
	
	return pMesh;

}

bool CUtilityTool::ComputeAffineTransformation(const varray<Vec3>& arrayPointInit, const varray<Vec3>& arrayPointTarget, double pA[9], double T[3])
{
	if (arrayPointInit.size() != 4) {
		return false;
	}

	double x[2][4], y[2][4], z[2][4];
	int i = 0;

	Vec3 center1, center2;
	for (i = 0; i < 4; i++) {
		const Vec3& vPos1 = arrayPointInit[i], &vPos2 = arrayPointTarget[i];
		x[0][i] = vPos1.x(); y[0][i] = vPos1.y(); z[0][i] = vPos1.z();
		x[1][i] = vPos2.x(); y[1][i] = vPos2.y(); z[1][i] = vPos2.z();

		center1 += vPos1;
		center2 += vPos2;
	}

	center1 /= 4.;
	center2 /= 4.;

	double M[9];

	M[0] = x[0][1] - x[0][0]; M[1] = y[0][1] - y[0][0]; M[2] = z[0][1] - z[0][0];
	M[3] = x[0][2] - x[0][0]; M[4] = y[0][2] - y[0][0]; M[5] = z[0][2] - z[0][0];
	M[6] = x[0][3] - x[0][0]; M[7] = y[0][3] - y[0][0]; M[8] = z[0][3] - z[0][0];

	CvMat MT = cvMat(3, 3, CV_64FC1, M), *invT = cvCreateMat(3, 3, CV_64FC1);
	double det = cvDet(&MT);

	cvInvert(&MT, invT);
	double *pInvData = invT->data.db;

	double qx21, qx31, qx41;

	qx21 = x[1][1] - x[1][0]; qx31 = x[1][2] - x[1][0]; qx41 = x[1][3] - x[1][0];
	pA[0] = pInvData[0] * qx21 + pInvData[1] * qx31 + pInvData[2] * qx41;
	pA[1] = pInvData[3] * qx21 + pInvData[4] * qx31 + pInvData[5] * qx41;
	pA[2] = pInvData[6] * qx21 + pInvData[7] * qx31 + pInvData[8] * qx41;

	qx21 = y[1][1] - y[1][0]; qx31 = y[1][2] - y[1][0]; qx41 = y[1][3] - y[1][0];
	pA[3] = pInvData[0] * qx21 + pInvData[1] * qx31 + pInvData[2] * qx41;
	pA[4] = pInvData[3] * qx21 + pInvData[4] * qx31 + pInvData[5] * qx41;
	pA[5] = pInvData[6] * qx21 + pInvData[7] * qx31 + pInvData[8] * qx41;

	qx21 = z[1][1] - z[1][0]; qx31 = z[1][2] - z[1][0]; qx41 = z[1][3] - z[1][0];
	pA[6] = pInvData[0] * qx21 + pInvData[1] * qx31 + pInvData[2] * qx41;
	pA[7] = pInvData[3] * qx21 + pInvData[4] * qx31 + pInvData[5] * qx41;
	pA[8] = pInvData[6] * qx21 + pInvData[7] * qx31 + pInvData[8] * qx41;

	//pA[0] = 1.0; pA[1] = .0; pA[2] = .0;
	//pA[3] = .0; pA[4] = 1.0; pA[5] = .0;
	//pA[6] = .0; pA[7] = .0; pA[8] = 1.0;

	double cx = center1.x(), cy = center1.y(), cz = center1.z();
	T[0] = center2.x() - (pA[0] * cx + pA[1] * cy + pA[2] * cz);
	T[1] = center2.y() - (pA[3] * cx + pA[4] * cy + pA[5] * cz);
	T[2] = center2.z() - (pA[6] * cx + pA[7] * cy + pA[8] * cz);

#ifdef _DEBUG
	// Test the correcteness of the computed affine transformation
	for (i = 0; i < 4; i++) {
		const Vec3 &p = arrayPointInit[i], &q = arrayPointTarget[i];
		//double x = p.x() - center1.x(), y = p.y() - center1.y(), z = p.z()-center1.z();
		double x = p.x(), y = p.y(), z = p.z();
		Vec3 pp;

		//pp.x() = pA[0] * x + pA[1] * y + pA[2] * z + center2.x();// T[0];
		//pp.y() = pA[3] * x + pA[4] * y + pA[5] * z + center2.y();// T[1];
		//pp.z() = pA[6] * x + pA[7] * y + pA[8] * z + center2.z(); // [2];

		pp.x() = pA[0] * x + pA[1] * y + pA[2] * z +  T[0];
		pp.y() = pA[3] * x + pA[4] * y + pA[5] * z +  T[1];
		pp.z() = pA[6] * x + pA[7] * y + pA[8] * z +  T[2];

		double dis = (pp - q).SquareMagnitude();
		if (!FLOAT_EQUAL(dis, 0)) {
			assert(false);
		}
	}

#endif // _DEBUG

	// Extract rotation from pLinearTransformation
	// QS decomposition based on SVD
	CvMat* S = cvCreateMat(3, 3, CV_64FC1);
	CvMat* U = cvCreateMat(3, 3, CV_64FC1);
	CvMat* D = cvCreateMat(3, 3, CV_64FC1);
	CvMat* V = cvCreateMat(3, 3, CV_64FC1);

	// Decompose the affine transformation by QS, i.e., A=QS
	double R[9];

	// Do svd for S
	int j;
	for (j = 0; j < 9; j++) {
		S->data.db[j] = pA[j];
	}

	cvSVD(S, D, U, V, CV_SVD_U_T); // S = U D V^T -- the resulting U corresponding to U^T

								   // VU^T
	R[0] = V->data.db[0] * U->data.db[0] + V->data.db[1] * U->data.db[3] + V->data.db[2] * U->data.db[6];
	R[1] = V->data.db[0] * U->data.db[1] + V->data.db[1] * U->data.db[4] + V->data.db[2] * U->data.db[7];
	R[2] = V->data.db[0] * U->data.db[2] + V->data.db[1] * U->data.db[5] + V->data.db[2] * U->data.db[8];

	R[3] = V->data.db[3] * U->data.db[0] + V->data.db[4] * U->data.db[3] + V->data.db[5] * U->data.db[6];
	R[4] = V->data.db[3] * U->data.db[1] + V->data.db[4] * U->data.db[4] + V->data.db[5] * U->data.db[7];
	R[5] = V->data.db[3] * U->data.db[2] + V->data.db[4] * U->data.db[5] + V->data.db[5] * U->data.db[8];

	R[6] = V->data.db[6] * U->data.db[0] + V->data.db[7] * U->data.db[3] + V->data.db[8] * U->data.db[6];
	R[7] = V->data.db[6] * U->data.db[1] + V->data.db[7] * U->data.db[4] + V->data.db[8] * U->data.db[7];
	R[8] = V->data.db[6] * U->data.db[2] + V->data.db[7] * U->data.db[5] + V->data.db[8] * U->data.db[8];

	CvMat RTT = cvMat(3, 3, CV_64FC1, R);
	det = cvDet(&RTT);

	assert(FLOAT_EQUAL(fabs(det), 1.));

	if (det < 0) {
		V->data.db[2] = -V->data.db[2];
		V->data.db[5] = -V->data.db[5];
		V->data.db[8] = -V->data.db[8];

		R[0] = V->data.db[0] * U->data.db[0] + V->data.db[1] * U->data.db[3] + V->data.db[2] * U->data.db[6];
		R[1] = V->data.db[0] * U->data.db[1] + V->data.db[1] * U->data.db[4] + V->data.db[2] * U->data.db[7];
		R[2] = V->data.db[0] * U->data.db[2] + V->data.db[1] * U->data.db[5] + V->data.db[2] * U->data.db[8];

		R[3] = V->data.db[3] * U->data.db[0] + V->data.db[4] * U->data.db[3] + V->data.db[5] * U->data.db[6];
		R[4] = V->data.db[3] * U->data.db[1] + V->data.db[4] * U->data.db[4] + V->data.db[5] * U->data.db[7];
		R[5] = V->data.db[3] * U->data.db[2] + V->data.db[4] * U->data.db[5] + V->data.db[5] * U->data.db[8];

		R[6] = V->data.db[6] * U->data.db[0] + V->data.db[7] * U->data.db[3] + V->data.db[8] * U->data.db[6];
		R[7] = V->data.db[6] * U->data.db[1] + V->data.db[7] * U->data.db[4] + V->data.db[8] * U->data.db[7];
		R[8] = V->data.db[6] * U->data.db[2] + V->data.db[7] * U->data.db[5] + V->data.db[8] * U->data.db[8];
	}

	printf("%.4f %.4f %.4f\n", R[0], R[1], R[2]);
	printf("%.4f %.4f %.4f\n", R[3], R[4], R[5]);
	printf("%.4f %.4f %.4f\n", R[6], R[7], R[8]);
	printf("%.4f %.4f %.4f\n", T[0], T[1], T[2]);

	for (i = 0; i < 9; i++) {
		pA[i] = R[i];
	}

	return true;
}

bool CUtilityTool::ComputeRegionOptimalAffineTransformation(const varray<Vec3>& arrayPointSetV1, const varray<Vec3>& arrayPointSetV2, double pLinearTransform[9], double T[3])
{
	Vec3 pStar, qStar;

	if (arrayPointSetV1.size() < 4) return false;
	
	// Compute the centroid of the two point sets
	int i = 0, nPointSetV = arrayPointSetV1.size();
	double totW = nPointSetV;

	pStar.Set(0, 0, 0);
	qStar.Set(0, 0, 0);
	for (i = 0; i < nPointSetV; i++) {
		pStar += arrayPointSetV1[i];
		qStar += arrayPointSetV2[i];
	}
	pStar /= totW;
	qStar /= totW;

	// C = Ewi*(qj-qi)T(pj-pi)Inv(Ewi*(pj-pi)T(pj-pi)) = AM
	double pA[9], B[9];
	int nRegionV = arrayPointSetV1.size();

	for (i = 0; i < 9; i++) {
		pA[i] = B[i] = .0;
	}

	for (i = 0; i < nRegionV; i++) {
		Vec3 Qji = arrayPointSetV2[i] - qStar, Pji = arrayPointSetV1[i] - pStar;
		double qx = Qji.x(), qy = Qji.y(), qz = Qji.z(), px = Pji.x(), py = Pji.y(), pz = Pji.z();

		pA[0] += qx*px; pA[1] += qx*py; pA[2] += qx*pz;
		pA[3] += qy*px; pA[4] += qy*py; pA[5] += qy*pz;
		pA[6] += qz*px; pA[7] += qz*py; pA[8] += qz*pz;

		B[0] += px*px; B[1] += px*py; B[2] += px*pz;
		B[3] += py*px; B[4] += py*py; B[5] += py*pz;
		B[6] += pz*px; B[7] += pz*py; B[8] += pz*pz;
	}

	// Check if the inverse of B exists
	double det = B[0] * B[3] - B[1] * B[2];
	if(FLOAT_EQUAL(det, 0)){
		return false;
	}

	// Compute the inverse of T
	CvMat MT = cvMat(3, 3, CV_64FC1, B), *invT = cvCreateMat(3, 3, CV_64FC1);
	cvInvert(&MT, invT);
	double *pInvData = invT->data.db;

	pLinearTransform[0] = pA[0] * pInvData[0] + pA[1] * pInvData[3] + pA[2] * pInvData[6];
	pLinearTransform[1] = pA[0] * pInvData[1] + pA[1] * pInvData[4] + pA[2] * pInvData[7];
	pLinearTransform[2] = pA[0] * pInvData[2] + pA[1] * pInvData[5] + pA[2] * pInvData[8];

	pLinearTransform[3] = pA[3] * pInvData[0] + pA[4] * pInvData[3] + pA[5] * pInvData[6];
	pLinearTransform[4] = pA[3] * pInvData[1] + pA[4] * pInvData[4] + pA[5] * pInvData[7];
	pLinearTransform[5] = pA[3] * pInvData[2] + pA[4] * pInvData[5] + pA[5] * pInvData[8];

	pLinearTransform[6] = pA[6] * pInvData[0] + pA[7] * pInvData[3] + pA[8] * pInvData[6];
	pLinearTransform[7] = pA[6] * pInvData[1] + pA[7] * pInvData[4] + pA[8] * pInvData[7];
	pLinearTransform[8] = pA[6] * pInvData[2] + pA[7] * pInvData[5] + pA[8] * pInvData[8];

	double cx = pStar.x(), cy = pStar.y(), cz = pStar.z();
	T[0] = qStar.x() - (pLinearTransform[0] * cx + pLinearTransform[1] * cy + pLinearTransform[2] * cz);
	T[1] = qStar.y() - (pLinearTransform[3] * cx + pLinearTransform[4] * cy + pLinearTransform[5] * cz);
	T[2] = qStar.z() - (pLinearTransform[6] * cx + pLinearTransform[7] * cy + pLinearTransform[8] * cz);

#ifdef _DEBUG
	// Test the correctness of the computed affine transformation
	if (arrayPointSetV1.size() != 4) return true;

	for (i = 0; i < 4; i++) {
		const Vec3 &p = arrayPointSetV1[i], &q = arrayPointSetV2[i];
		double x = p.x(), y = p.y(), z = p.z();
		Vec3 pp;

		pp.x() = pLinearTransform[0] * x + pLinearTransform[1] * y + pLinearTransform[2] * z + T[0];
		pp.y() = pLinearTransform[3] * x + pLinearTransform[4] * y + pLinearTransform[5] * z + T[1];
		pp.z() = pLinearTransform[6] * x + pLinearTransform[7] * y + pLinearTransform[8] * z + T[2];

		double dis = (pp - q).SquareMagnitude();
		if (!FLOAT_EQUAL(dis, 0)) {
			assert(false);
		}
	}

	printf("%.4f %.4f %.4f\n", pLinearTransform[0], pLinearTransform[1], pLinearTransform[2]);
	printf("%.4f %.4f %.4f\n", pLinearTransform[3], pLinearTransform[4], pLinearTransform[5]);
	printf("%.4f %.4f %.4f\n", pLinearTransform[6], pLinearTransform[7], pLinearTransform[8]);

#endif // _DEBUG

	// Extract rotation from pLinearTransformation
	// QS decomposition based on SVD
	CvMat* S = cvCreateMat(3, 3, CV_64FC1);
	CvMat* U = cvCreateMat(3, 3, CV_64FC1);
	CvMat* D = cvCreateMat(3, 3, CV_64FC1);
	CvMat* V = cvCreateMat(3, 3, CV_64FC1);

	// Decompose the affine transformation by QS, i.e., A=QS
	double Q[9], Qrotate[9];

	// Do svd for S
	int j;
	for (j = 0; j < 9; j++) {
		S->data.db[j] = pLinearTransform[j];
	}

	cvSVD(S, D, U, V, CV_SVD_U_T | CV_SVD_V_T); // S = U D V^T

	// Ri = UV^T
	Q[0] = U->data.db[0] * V->data.db[0] + U->data.db[3] * V->data.db[3] + U->data.db[6] * V->data.db[6];
	Q[1] = U->data.db[0] * V->data.db[1] + U->data.db[3] * V->data.db[4] + U->data.db[6] * V->data.db[7];
	Q[2] = U->data.db[0] * V->data.db[2] + U->data.db[3] * V->data.db[5] + U->data.db[6] * V->data.db[8];

	Q[3] = U->data.db[1] * V->data.db[0] + U->data.db[4] * V->data.db[3] + U->data.db[7] * V->data.db[6];
	Q[4] = U->data.db[1] * V->data.db[1] + U->data.db[4] * V->data.db[4] + U->data.db[7] * V->data.db[7];
	Q[5] = U->data.db[1] * V->data.db[2] + U->data.db[4] * V->data.db[5] + U->data.db[7] * V->data.db[8];

	Q[6] = U->data.db[2] * V->data.db[0] + U->data.db[5] * V->data.db[3] + U->data.db[8] * V->data.db[6];
	Q[7] = U->data.db[2] * V->data.db[1] + U->data.db[5] * V->data.db[4] + U->data.db[8] * V->data.db[7];
	Q[8] = U->data.db[2] * V->data.db[2] + U->data.db[5] * V->data.db[5] + U->data.db[8] * V->data.db[8];

	CvMat MTT = cvMat(3, 3, CV_64FC1, Q);
	double det2 = cvDet(&MTT); //det2 = 1;

	if (det2 < 0) { // Eliminate reflection
				   // Multiple Q by -I [-1, 0, 0, 0, 1, 0, 0, 0, 1]
		Qrotate[0] = -Q[0];
		Qrotate[1] = Q[1];
		Qrotate[2] = Q[2];

		Qrotate[3] = -Q[3];
		Qrotate[4] = Q[4];
		Qrotate[5] = Q[5];

		Qrotate[6] = -Q[6];
		Qrotate[7] = Q[7];
		Qrotate[8] = Q[8];

	}
	else {
		Qrotate[0] = Q[0];
		Qrotate[1] = Q[1];
		Qrotate[2] = Q[2];

		Qrotate[3] = Q[3];
		Qrotate[4] = Q[4];
		Qrotate[5] = Q[5];

		Qrotate[6] = Q[6];
		Qrotate[7] = Q[7];
		Qrotate[8] = Q[8];
	}

	for (j = 0; j < 9; j++) {
		pLinearTransform[j] = Qrotate[j];
	}

	return true;
}

bool CUtilityTool::ComputeRigidTransformation(const varray<Vec3>& arrayPointInit, const varray<Vec3>& arrayPointTarget, double R[9], double T[3])
{
	int nPoint = arrayPointInit.size(), i;

	if (nPoint < 2) return false;

	// Compute the centers of the two point sets
	Vec3 pBar, qBar;

	for (i = 0; i < nPoint; i++) {
		pBar += arrayPointInit[i];
		qBar += arrayPointTarget[i];
	}

	pBar /= (double)nPoint;
	qBar /= (double)nPoint;

	// Compute the covariance matrix
	double LS[9];

	for (i = 0; i < 9; i++) {
		LS[i] = .0;
	}
	for (i = 0; i < nPoint; i++) {
		Vec3 X = arrayPointInit[i] - pBar, Y = arrayPointTarget[i] - qBar;
		double Xx = X.x(), Xy = X.y(), Xz = X.z(), Yx = Y.x(), Yy = Y.y(), Yz = Y.z();

		LS[0] += Xx * Yx; LS[1] += Xx * Yy; LS[2] += Xx * Yz;
		LS[3] += Xy * Yx; LS[4] += Xy * Yy; LS[5] += Xy * Yz;
		LS[6] += Xz * Yx; LS[7] += Xz * Yy; LS[8] += Xz * Yz;
	}

	// Extract rotation from pLinearTransformation
	// QS decomposition based on SVD
	CvMat* S = cvCreateMat(3, 3, CV_64FC1);
	CvMat* U = cvCreateMat(3, 3, CV_64FC1);
	CvMat* D = cvCreateMat(3, 3, CV_64FC1);
	CvMat* V = cvCreateMat(3, 3, CV_64FC1);

	// Decompose the affine transformation by QS, i.e., A=QS
	double Q[9], Qrotate[9];

	// Do svd for S
	int j;
	for (j = 0; j < 9; j++) {
		S->data.db[j] = LS[j];
	}

	cvSVD(S, D, U, V, CV_SVD_U_T); // S = U D V^T -- the resulting U corresponding to U^T

	// VU^T
	R[0] = V->data.db[0] * U->data.db[0] + V->data.db[1] * U->data.db[3] + V->data.db[2] * U->data.db[6];
	R[1] = V->data.db[0] * U->data.db[1] + V->data.db[1] * U->data.db[4] + V->data.db[2] * U->data.db[7];
	R[2] = V->data.db[0] * U->data.db[2] + V->data.db[1] * U->data.db[5] + V->data.db[2] * U->data.db[8];

	R[3] = V->data.db[3] * U->data.db[0] + V->data.db[4] * U->data.db[3] + V->data.db[5] * U->data.db[6];
	R[4] = V->data.db[3] * U->data.db[1] + V->data.db[4] * U->data.db[4] + V->data.db[5] * U->data.db[7];
	R[5] = V->data.db[3] * U->data.db[2] + V->data.db[4] * U->data.db[5] + V->data.db[5] * U->data.db[8];

	R[6] = V->data.db[6] * U->data.db[0] + V->data.db[7] * U->data.db[3] + V->data.db[8] * U->data.db[6];
	R[7] = V->data.db[6] * U->data.db[1] + V->data.db[7] * U->data.db[4] + V->data.db[8] * U->data.db[7];
	R[8] = V->data.db[6] * U->data.db[2] + V->data.db[7] * U->data.db[5] + V->data.db[8] * U->data.db[8];

	CvMat RTT = cvMat(3, 3, CV_64FC1, R);
	double det = cvDet(&RTT); 

	assert(FLOAT_EQUAL(fabs(det), 1.));

	if (det < 0) {
		V->data.db[2] = -V->data.db[2];
		V->data.db[5] = -V->data.db[5];
		V->data.db[8] = -V->data.db[8];

		R[0] = V->data.db[0] * U->data.db[0] + V->data.db[1] * U->data.db[3] + V->data.db[2] * U->data.db[6];
		R[1] = V->data.db[0] * U->data.db[1] + V->data.db[1] * U->data.db[4] + V->data.db[2] * U->data.db[7];
		R[2] = V->data.db[0] * U->data.db[2] + V->data.db[1] * U->data.db[5] + V->data.db[2] * U->data.db[8];

		R[3] = V->data.db[3] * U->data.db[0] + V->data.db[4] * U->data.db[3] + V->data.db[5] * U->data.db[6];
		R[4] = V->data.db[3] * U->data.db[1] + V->data.db[4] * U->data.db[4] + V->data.db[5] * U->data.db[7];
		R[5] = V->data.db[3] * U->data.db[2] + V->data.db[4] * U->data.db[5] + V->data.db[5] * U->data.db[8];

		R[6] = V->data.db[6] * U->data.db[0] + V->data.db[7] * U->data.db[3] + V->data.db[8] * U->data.db[6];
		R[7] = V->data.db[6] * U->data.db[1] + V->data.db[7] * U->data.db[4] + V->data.db[8] * U->data.db[7];
		R[8] = V->data.db[6] * U->data.db[2] + V->data.db[7] * U->data.db[5] + V->data.db[8] * U->data.db[8];
	}

	double cx = pBar.x(), cy = pBar.y(), cz = pBar.z();
	T[0] = qBar.x() - (R[0] * cx + R[1] * cy + R[2] * cz);
	T[1] = qBar.y() - (R[3] * cx + R[4] * cy + R[5] * cz);
	T[2] = qBar.z() - (R[6] * cx + R[7] * cy + R[8] * cz);

	printf("%.4f %.4f %.4f\n", R[0], R[1], R[2]);
	printf("%.4f %.4f %.4f\n", R[3], R[4], R[5]);
	printf("%.4f %.4f %.4f\n", R[6], R[7], R[8]);
	printf("%.4f %.4f %.4f\n", T[0], T[1], T[2]);

	return true;
}

bool CUtilityTool::ComputeSimilarityTransformation(const varray<Vec3>& arrayPointInit, const varray<Vec3>& arrayPointTarget, double R[9], double T[3])
{
	int nPoint = arrayPointInit.size(), i;

	if (nPoint < 2) return false;

	// Compute the centers of the two point sets
	Vec3 pBar, qBar;

	for (i = 0; i < nPoint; i++) {
		pBar += arrayPointInit[i];
		qBar += arrayPointTarget[i];
	}

	pBar /= (double)nPoint;
	qBar /= (double)nPoint;

	// Compute the covariance matrix
	double LS[9];

	for (i = 0; i < 9; i++) {
		LS[i] = .0;
	}
	for (i = 0; i < nPoint; i++) {
		Vec3 X = arrayPointInit[i] - pBar, Y = arrayPointTarget[i] - qBar;
		double Xx = X.x(), Xy = X.y(), Xz = X.z(), Yx = Y.x(), Yy = Y.y(), Yz = Y.z();

		LS[0] += Xx * Yx; LS[1] += Xx * Yy; LS[2] += Xx * Yz;
		LS[3] += Xy * Yx; LS[4] += Xy * Yy; LS[5] += Xy * Yz;
		LS[6] += Xz * Yx; LS[7] += Xz * Yy; LS[8] += Xz * Yz;
	}

	// Extract rotation from pLinearTransformation
	// QS decomposition based on SVD
	CvMat* S = cvCreateMat(3, 3, CV_64FC1);
	CvMat* U = cvCreateMat(3, 3, CV_64FC1);
	CvMat* D = cvCreateMat(3, 3, CV_64FC1);
	CvMat* V = cvCreateMat(3, 3, CV_64FC1);

	// Decompose the affine transformation by QS, i.e., A=QS
	double Q[9], Qrotate[9];

	// Do svd for S
	int j;
	for (j = 0; j < 9; j++) {
		S->data.db[j] = LS[j];
	}

	cvSVD(S, D, U, V, CV_SVD_U_T); // S = U D V^T -- the resulting U corresponding to U^T

								   // VU^T
	R[0] = V->data.db[0] * U->data.db[0] + V->data.db[1] * U->data.db[3] + V->data.db[2] * U->data.db[6];
	R[1] = V->data.db[0] * U->data.db[1] + V->data.db[1] * U->data.db[4] + V->data.db[2] * U->data.db[7];
	R[2] = V->data.db[0] * U->data.db[2] + V->data.db[1] * U->data.db[5] + V->data.db[2] * U->data.db[8];

	R[3] = V->data.db[3] * U->data.db[0] + V->data.db[4] * U->data.db[3] + V->data.db[5] * U->data.db[6];
	R[4] = V->data.db[3] * U->data.db[1] + V->data.db[4] * U->data.db[4] + V->data.db[5] * U->data.db[7];
	R[5] = V->data.db[3] * U->data.db[2] + V->data.db[4] * U->data.db[5] + V->data.db[5] * U->data.db[8];

	R[6] = V->data.db[6] * U->data.db[0] + V->data.db[7] * U->data.db[3] + V->data.db[8] * U->data.db[6];
	R[7] = V->data.db[6] * U->data.db[1] + V->data.db[7] * U->data.db[4] + V->data.db[8] * U->data.db[7];
	R[8] = V->data.db[6] * U->data.db[2] + V->data.db[7] * U->data.db[5] + V->data.db[8] * U->data.db[8];

	CvMat RTT = cvMat(3, 3, CV_64FC1, R);
	double det = cvDet(&RTT);

	assert(FLOAT_EQUAL(fabs(det), 1.));

	if (det < 0) {
		V->data.db[2] = -V->data.db[2];
		V->data.db[5] = -V->data.db[5];
		V->data.db[8] = -V->data.db[8];

		R[0] = V->data.db[0] * U->data.db[0] + V->data.db[1] * U->data.db[3] + V->data.db[2] * U->data.db[6];
		R[1] = V->data.db[0] * U->data.db[1] + V->data.db[1] * U->data.db[4] + V->data.db[2] * U->data.db[7];
		R[2] = V->data.db[0] * U->data.db[2] + V->data.db[1] * U->data.db[5] + V->data.db[2] * U->data.db[8];

		R[3] = V->data.db[3] * U->data.db[0] + V->data.db[4] * U->data.db[3] + V->data.db[5] * U->data.db[6];
		R[4] = V->data.db[3] * U->data.db[1] + V->data.db[4] * U->data.db[4] + V->data.db[5] * U->data.db[7];
		R[5] = V->data.db[3] * U->data.db[2] + V->data.db[4] * U->data.db[5] + V->data.db[5] * U->data.db[8];

		R[6] = V->data.db[6] * U->data.db[0] + V->data.db[7] * U->data.db[3] + V->data.db[8] * U->data.db[6];
		R[7] = V->data.db[6] * U->data.db[1] + V->data.db[7] * U->data.db[4] + V->data.db[8] * U->data.db[7];
		R[8] = V->data.db[6] * U->data.db[2] + V->data.db[7] * U->data.db[5] + V->data.db[8] * U->data.db[8];
	}

	// Compute scale
	double alpha, base = .0, sum = .0;

	for (i = 0; i < nPoint; i++) {
		Vec3 Rpi;
		const Vec3& pi = arrayPointInit[i], &qi = arrayPointTarget[i];

		Rpi.x() = R[0] * pi.x() + R[1] * pi.y() + R[2] * pi.z();
		Rpi.y() = R[3] * pi.x() + R[4] * pi.y() + R[5] * pi.z();
		Rpi.z() = R[6] * pi.x() + R[7] * pi.y() + R[8] * pi.z();

		sum += (Dot(qi, Rpi) - Dot(Rpi, qBar));

		base += (pi.SquareMagnitude() - Dot(pi, pBar));
	}

	alpha = sum / base;

	for (i = 0; i < 9; i++) {
		R[i] *= alpha;
	}

	double cx = pBar.x(), cy = pBar.y(), cz = pBar.z();
	T[0] = qBar.x() - (R[0] * cx + R[1] * cy + R[2] * cz);
	T[1] = qBar.y() - (R[3] * cx + R[4] * cy + R[5] * cz);
	T[2] = qBar.z() - (R[6] * cx + R[7] * cy + R[8] * cz);

	return true;
}

void CUtilityTool::TransposeMatrix(const double matrix[9], double transM[9])
{
	transM[0] = matrix[0];
	transM[1] = matrix[3];
	transM[2] = matrix[6];

	transM[3] = matrix[1];
	transM[4] = matrix[4];
	transM[5] = matrix[7];

	transM[6] = matrix[2];
	transM[7] = matrix[5];
	transM[8] = matrix[8];

	return;
}

void CUtilityTool::MultiplyMatrix(const double matrix1[9], const double matrix2[9], double mulM[9])
{
	int i, j;
 
	int index = 0;
	for (i = 0; i < 3; i++) {
		int p1 = 3 * i;
		for (j = 0; j < 3; j++) {
			int p2 = j;

			mulM[index] = matrix1[p1] * matrix2[p2] + matrix1[p1 + 1] * matrix2[p2 + 3] + matrix1[p1 + 2] * matrix2[p2 + 6];

			index++;
		}
	}

	return;
}

void CUtilityTool::MultiplyMatrixVector(const double matrix[9], const double v[3], double mulV[3])
{
	mulV[0] = matrix[0] * v[0] + matrix[1] * v[1] + matrix[2] * v[2];
	mulV[1] = matrix[3] * v[0] + matrix[4] * v[1] + matrix[5] * v[2];
	mulV[2] = matrix[6] * v[0] + matrix[7] * v[1] + matrix[8] * v[2];

	return;
}

double CUtilityTool::ComputeAngle2D(const Vec2 &vec1, const Vec2 &vec2)
{
	double dot = Dot(vec1, vec2) / (vec1.Magnitude()*vec2.Magnitude());

	if (fabs(dot) >= 1.0) {  // it's very important !!!!!!!
		if (dot < 0)
			dot = -.999999;
		else
			dot = .999999;
	}

	return acos(dot);
}

