#include <list>

#include "base.h"   // For pre-definition
#include "varray.h"	// For array and debugging checking for size range
#include "math.h" // For math lib

#include "Mesh.h" 

//////////////////////////////////////////////////////////////////////////
// Implementation of CMesh

namespace base{


	void CMesh::MakeEdges()
	{
		// Construct the adjacent relations between vertices, edges and faces
		// v-faces, v-edges, f-edges, e-faces
		int i = 0, j = 0;

#ifdef _DEBUG
		// It is used after the faces and vertices been givens
		assert(_arrayVertex.size() > 0 && _arrayFace.size() > 0);
#endif

		// Make the vertex-faces relation
		int nVSize = _arrayVertex.size(), nFSize = _arrayFace.size();
		varray< varray<int> > arrayVAdjFace;
		arrayVAdjFace.resize(nVSize);

		for(i = 0; i < nFSize; i ++){
			CFace& face = _arrayFace[i];
			int iFaceVID[] = { face.p(0), face.p(1), face.p(2) };
#ifdef _DEBUG
			assert(iFaceVID[0] >= 0 && iFaceVID[0] < nVSize && 
				iFaceVID[1] >= 0 && iFaceVID[1] < nVSize &&
				iFaceVID[2] >= 0 && iFaceVID[2] < nVSize);
#endif
			arrayVAdjFace[iFaceVID[0]].push_back(i);
			arrayVAdjFace[iFaceVID[1]].push_back(i);
			arrayVAdjFace[iFaceVID[2]].push_back(i);

			assert( (iFaceVID[0] != iFaceVID[1]) && (iFaceVID[0] != iFaceVID[2]) && (iFaceVID[2] != iFaceVID[1]) );
		}

		for(i = 0; i < nVSize; i ++){
			CVertex& vert = _arrayVertex[i];
			vert.SetAdjF(arrayVAdjFace[i]);
		}

		// Create the edges and make vertex-edges, face-edges relations
		// Make edges from each face and attach them to vertices
		// During one new edge generated, check whether there already existed on edge
		// that is same as it; if existed, discard it
		int iEdgeIndex = 0;
		varray< varray<int> > arrayVAdjEdge;
		arrayVAdjEdge.resize(nVSize);
		_arrayEdge.clear();

		for(i = 0; i < nFSize; i ++){
			CFace& face = _arrayFace[i];
			int iFaceVID[] = { face.p(0), face.p(1), face.p(2) };
			int iFaceEID[3];

			for(j = 0; j < 3; j ++){
				int k = j + 1;
				int iEdgeVID[2];

				if(k == 3){ k = 0; }
				iEdgeVID[0] = iFaceVID[j];
				iEdgeVID[1] = iFaceVID[k];

				// Try to find same edges in the adjacent edges of iEdgeVID[0]
				int iFoundEdgeID = -1;
				varray<int>& arrayCurVAdjEdge = arrayVAdjEdge[iEdgeVID[0]];
				for(int m = 0; m < arrayCurVAdjEdge.size(); m ++){
					int iAdjEdgeID = arrayCurVAdjEdge[m];
#ifdef _DEBUG
					assert(iAdjEdgeID >= 0 && iAdjEdgeID < _arrayEdge.size());
#endif
					CEdge& adjEdge = _arrayEdge[iAdjEdgeID];
					int iAdjEdgeVID[] = { adjEdge.p(0), adjEdge.p(1) };

					if( ((iAdjEdgeVID[0] == iEdgeVID[0]) && (iAdjEdgeVID[1] == iEdgeVID[1])) || ((iAdjEdgeVID[0] == iEdgeVID[1]) && (iAdjEdgeVID[1] == iEdgeVID[0])) ){
						iFoundEdgeID = iAdjEdgeID;
						break;
					}
				}

#ifdef _DEBUG
				int iFoundEdgeID2 = -1, m2;
				varray<int>& arrayCurVAdjEdge2 = arrayVAdjEdge[iEdgeVID[1]];
				for(m2 = 0; m2 < arrayCurVAdjEdge2.size(); m2 ++){
					int iAdjEdgeID = arrayCurVAdjEdge2[m2];
#ifdef _DEBUG
					assert(iAdjEdgeID >= 0 && iAdjEdgeID < _arrayEdge.size());
#endif
					CEdge& adjEdge = _arrayEdge[iAdjEdgeID];
					int iAdjEdgeVID[] = { adjEdge.p(0), adjEdge.p(1) };

					if( ((iAdjEdgeVID[0] == iEdgeVID[0]) && (iAdjEdgeVID[1] == iEdgeVID[1])) || ((iAdjEdgeVID[0] == iEdgeVID[1]) && (iAdjEdgeVID[1] == iEdgeVID[0])) ){
						iFoundEdgeID2 = iAdjEdgeID;
						break;
					}
				}
#endif

				if(iFoundEdgeID != -1){ // This edge already existed
					iFaceEID[j] = iFoundEdgeID;
					assert(iFoundEdgeID == iFoundEdgeID2);
				}
				else{
					assert(iFoundEdgeID2 == -1);

					// Create one new edge
					CEdge edge;
					edge.SetVIndex(iEdgeVID);
					_arrayEdge.push_back(edge);

					arrayVAdjEdge[iEdgeVID[0]].push_back(iEdgeIndex);
					arrayVAdjEdge[iEdgeVID[1]].push_back(iEdgeIndex);

					iFaceEID[j] = iEdgeIndex;
					iEdgeIndex ++;
				}
			}
			face.SetEdgeIndex(iFaceEID);
			assert((iFaceEID[0] != iFaceEID[1]) && (iFaceEID[1] != iFaceEID[2]) && (iFaceEID[0] != iFaceEID[2]) );
		}

		for(i = 0; i < nVSize; i ++){
			CVertex& vert = _arrayVertex[i];
			vert.SetAdjE(arrayVAdjEdge[i]);
		}

		// make edge-faces relation
		varray< varray<int> > arrayEAdjFace;
		arrayEAdjFace.resize(_arrayEdge.size());

		for(i = 0; i < nFSize; i ++){
			CFace& face = _arrayFace[i];
			int iFaceEdgeID[] = {face.GetEdgeIndex(0), face.GetEdgeIndex(1), face.GetEdgeIndex(2)};

			arrayEAdjFace[iFaceEdgeID[0]].push_back(i);
			arrayEAdjFace[iFaceEdgeID[1]].push_back(i);
			arrayEAdjFace[iFaceEdgeID[2]].push_back(i);

			assert((iFaceEdgeID[0] != iFaceEdgeID[1]) && (iFaceEdgeID[1] != iFaceEdgeID[2]) && (iFaceEdgeID[0] != iFaceEdgeID[2]) );
		}

		for(i = 0; i < _arrayEdge.size(); i ++){
			CEdge& edge = _arrayEdge[i];
#ifdef _DEBUG
			//assert(arrayEAdjFace[i].size() == 1 || arrayEAdjFace[i].size() == 2);
#endif
			edge.SetAdjF(arrayEAdjFace[i]);
		}

#ifdef _DEBUG
		// Test the made adjacent relation
		int nAdjEdgeSize = 0;
		for(i = 0; i < nVSize; i ++){
			CVertex& vert = _arrayVertex[i];
			nAdjEdgeSize += vert.GetAdjESize();
		}

		printf("adj edge size, 2 * total edge size: %d, %d\n", nAdjEdgeSize, 2 * _arrayEdge.size());

		int nOverlapEdgeSize = 0;
		for(i = 0; i < _arrayEdge.size(); i ++){
			CEdge& edge = _arrayEdge[i];
			if(edge.GetAdjFSize() == 2){
				nOverlapEdgeSize ++;
			}
			else{
				if(edge.GetAdjFSize() != 1)
					assert(edge.GetAdjFSize() == 1);
			}
		}

		printf("edges in face, edge count: %d, %d\n", 3 * nFSize - nOverlapEdgeSize, _arrayEdge.size());

		printf("(V+F-E) %d+%d-%d = %d\n", nVSize, nFSize, _arrayEdge.size(), nVSize+nFSize-_arrayEdge.size());
#endif

		return;
	}

	void CMesh::OrganizeMeshDataForRender()
	{
		int nMeshV = _arrayVertex.size();
		if(nMeshV>0){
			if(_arrayRenderVertexPos.size()!=2*nMeshV){
				_arrayRenderVertexPos.clear();
				_arrayRenderVertexPos.resize(nMeshV*2);
			}	
		}
		else{
			_arrayRenderVertexPos.clear();
		}

		int i = 0;
		for(i = 0; i < nMeshV; i ++){
			const Vec3& vPos = _arrayVertex[i].Pos();
			int index = 2*i;

			_arrayRenderVertexPos[index] = vPos.x();
			_arrayRenderVertexPos[index+1] = vPos.y();
		}

		return;
	}

	void CMesh::ComputeCenter()
	{
		int nMeshV = _arrayVertex.size(), i = 0;

		_meshCenter.Set(0, 0, 0);
		for (; i < nMeshV; i++) {
			_meshCenter += _arrayVertex[i].Pos();
		}
		_meshCenter = _meshCenter / (double)nMeshV;
	}

	Vec3 CMesh::GetCenter() const
	{
		return _meshCenter;
	}


} // namespace base
