/*******************************************************************
* Author	: wwyang
* Date		: 2006.3.21
* Copyright : State key CAD&CG Lab, Group of Animation 
* Head File : Structure for triangulated mesh
* Version   : 1.0
*********************************************************************/

#ifndef _MESH_H_
#define _MESH_H_

#include "Base.h"   // For pre-definition
#include "varray.h"	// For array and debugging checking for size range
#include "Vec2.h"   // For vector 
#include "Vec3.h"
#include "memory.h"

namespace base{

//////////////////////////////////////////////////////////////////////////
// Class for vertex
class CVertex{

private:
	Vec3 _pos, _normal;
	varray<int> _arrayAdjEdge, _arrayAdjFace;

public:
	// constructor
	CVertex(){}
	CVertex(const Vec3& pos)
	{ _pos = pos; }
	CVertex(const CVertex& vert)
	{ operator = (vert); }

	// get&set
	const Vec3& Pos() const
	{ return _pos; }
	Vec3& Pos()
	{ return _pos; }
	const Vec3& Normal() const
	{
		return _normal;
	}
	Vec3& Normal()
	{
		return _normal;
	}

	int GetAdjESize() const
	{ return _arrayAdjEdge.size(); }
	int GetAdjFSize() const
	{ return _arrayAdjFace.size(); }

	int GetAdjE(int i) const
	{ return _arrayAdjEdge[i]; }
	int GetAdjF(int i) const
	{ return _arrayAdjFace[i]; }

	int& GetAdjE(int i)
	{ return _arrayAdjEdge[i]; }
	int& GetAdjF(int i)
	{ return _arrayAdjFace[i]; }

	void SetAdjE(const varray<int>& arrayAdjE)
	{ _arrayAdjEdge = arrayAdjE; }
	void SetAdjF(const varray<int>& arrayAdjF)
	{ _arrayAdjFace = arrayAdjF; }

	// operators
	CVertex& operator = (const CVertex& vert)
	{
		_pos = vert._pos;
		_normal = vert._normal;
		_arrayAdjEdge = vert._arrayAdjEdge;
		_arrayAdjFace = vert._arrayAdjFace;
		return *this;
	}
};

//////////////////////////////////////////////////////////////////////////
// Class for edge
class CEdge{
private:
	int _iVIndex[2];
	varray<int> _arrayAdjFace;

public:
	// constructor
	CEdge()
	{_iVIndex[0] = -1; _iVIndex[1] = -1; }
	CEdge(int i, int j)
	{_iVIndex[0] = i; _iVIndex[1] = j; }
	CEdge(int iVIndex[2])
	{_iVIndex[0] = iVIndex[0]; _iVIndex[1] = iVIndex[1]; }
	CEdge(const CEdge& edge)
	{ operator = (edge); }

	// get&set
	int GetVIndex(int i) const
	{ assert(i == 0 || i == 1);  return _iVIndex[i]; }
	int p(int i) const
	{ return GetVIndex(i); }

	int& GetVIndex(int i)
	{ assert(i == 0 || i == 1);  return _iVIndex[i]; }
	int& p(int i)
	{ return GetVIndex(i); }

	int GetAdjFSize() const
	{ return _arrayAdjFace.size(); }
	int GetAdjF(int i) const
	{ return _arrayAdjFace[i]; }
	int& GetAdjF(int i)
	{ return _arrayAdjFace[i]; }

	void SetVIndex(int i, int j)
	{ _iVIndex[0] = i; _iVIndex[1] = j; }
	void SetVIndex(int iVIndex[2])
	{ memcpy(_iVIndex, iVIndex, sizeof(int) * 2); }
	void SetAdjF(const varray<int> arrayAdjFace)
	{ _arrayAdjFace = arrayAdjFace; }

	// operators
	CEdge& operator = (const CEdge& edge)
	{
		memcpy(_iVIndex, edge._iVIndex, sizeof(int) * 2);
		_arrayAdjFace = edge._arrayAdjFace;
		return *this;
	}
};

//////////////////////////////////////////////////////////////////////////
// class for face
class CFace{
private:
	int _iVIndex[3], _iVNormalIndex[3];
	int _iEdgeIndex[3];

public:
	char _iReserved;

public:
	// constructor
	CFace()
	{ memset(_iVIndex, -1, sizeof(int) * 3); memset(_iVNormalIndex, -1, sizeof(int) * 3); }
	CFace(int i, int j, int k)
	{ _iVIndex[0] = i; _iVIndex[1] = j; _iVIndex[2] = k; memset(_iVNormalIndex, -1, sizeof(int) * 3); }
	CFace(int iVIndex[3])
	{ _iVIndex[0] = iVIndex[0]; _iVIndex[1] = iVIndex[1]; _iVIndex[2] = iVIndex[2]; memset(_iVNormalIndex, -1, sizeof(int) * 3); }
	CFace(const CFace& face)
	{ operator = (face); }

	// get&set
	int GetVIndex(int i) const
	{ assert(i == 0 || i == 1 || i == 2); return _iVIndex[i]; }
	int p(int i) const
	{ return GetVIndex(i); }
	int GetVNormalIndex(int i) const
	{ assert(i == 0 || i == 1 || i == 2); return _iVNormalIndex[i]; }
	int GetEdgeIndex(int i) const
	{assert(i == 0 || i == 1 || i == 2); return _iEdgeIndex[i]; }

	int& GetVIndex(int i)
	{ assert(i == 0 || i == 1 || i == 2); return _iVIndex[i]; }
	int& p(int i)
	{ return GetVIndex(i); }
	int& GetVNormalIndex(int i)
	{ assert(i == 0 || i == 1 || i == 2); return _iVNormalIndex[i]; }
	int& GetEdgeIndex(int i)
	{assert(i == 0 || i == 1 || i == 2); return _iEdgeIndex[i]; }


	void SetVIndex(int i, int j, int k)
	{ _iVIndex[0] = i, _iVIndex[1] = j, _iVIndex[2] = k; }
	void SetVIndex(int iVIndex[3])
	{ memcpy(_iVIndex, iVIndex, sizeof(int) * 3); }
	void SetVNormalIndex(int i, int j, int k)
	{ _iVNormalIndex[0] = i, _iVNormalIndex[1] = j, _iVNormalIndex[2] = k; }
	void SetVNormalIndex(int iVNormalIndex[3])
	{ memcpy(_iVNormalIndex, iVNormalIndex, sizeof(int) * 3); }
	void SetEdgeIndex(int i, int j, int k)
	{ _iEdgeIndex[0] = i, _iEdgeIndex[1] = j, _iEdgeIndex[2] = k; }
	void SetEdgeIndex(int iEdgeIndex[3])
	{ memcpy(_iEdgeIndex, iEdgeIndex, sizeof(int) * 3); }

	// operators
	CFace& operator = (const CFace& face)
	{	
		memcpy(_iVIndex, face._iVIndex, sizeof(int) * 3);
		memcpy(_iVNormalIndex, face._iVNormalIndex, sizeof(int) * 3);
		memcpy(_iEdgeIndex, face._iEdgeIndex, sizeof(int) * 3);
		return *this;
	}
};

//////////////////////////////////////////////////////////////////////////
// Class for mesh
class CMesh{
private:
	varray<CVertex> _arrayVertex;
	varray<CEdge> _arrayEdge;
	varray<CFace> _arrayFace;

	// Discrete attributes
	varray<double> _arrayRenderVertexPos;
	varray<double> _arrayRenderTextureCoord;
	varray<double> _arrayRenderVertexDepth;

	Vec3 _meshCenter;

public:
	// constructor & destructor
	CMesh()
	{ Initialize(); }
	CMesh(const varray<CVertex>& arrayVertex, const varray<CEdge>& arrayEdge, const varray<CFace>& arrayFace)
	{ Initialize(arrayVertex, arrayEdge, arrayFace); }
	CMesh(const CMesh& mesh)
	{ operator = (mesh); }
	~CMesh()
	{ Destroy(); }
protected:
	void Initialize(){}
	void Initialize(const varray<CVertex>& arrayVertex, const varray<CEdge>& arrayEdge, const varray<CFace>& arrayFace)
	{
		// Note: canna do it by memcpy because just the pointer stored in varray object
		_arrayVertex = arrayVertex;
		_arrayEdge = arrayEdge;
		_arrayFace = arrayFace;
	}
	void Destroy()
	{
		ClearMesh();
	}

public:
	// get&set
	const CVertex& GetV(int i) const
	{ return _arrayVertex[i]; }
	CVertex& GetV(int i)
	{ return _arrayVertex[i]; }
	const CEdge& GetE(int i) const
	{ return _arrayEdge[i]; }
	CEdge& GetE(int i)
	{ return _arrayEdge[i]; }
	const CFace& GetF(int i) const 
	{ return _arrayFace[i]; }
	CFace& GetF(int i)
	{ return _arrayFace[i]; }

	int GetVSize() const
	{ return _arrayVertex.size(); }
	int GetESize() const
	{ return _arrayEdge.size(); }
	int GetFSize() const
	{ return _arrayFace.size(); }

	void SetVSize(int nSize)
	{ _arrayVertex.resize(nSize); }
	void SetESize(int nSize)
	{ _arrayEdge.resize(nSize); }
	void SetFSize(int nSize)
	{ _arrayFace.resize(nSize); }

	void SetMeshV(const varray<CVertex>& arrayMeshV)
	{ _arrayVertex = arrayMeshV; }
	void SetMeshE(const varray<CEdge>& arrayMeshE)
	{ _arrayEdge = arrayMeshE; }
	void SetMeshF(const varray<CFace>& arrayMeshF)
	{ _arrayFace = arrayMeshF; }
	void SetMesh(const varray<CVertex>& arrayMeshV, const varray<CEdge>& arrayMeshE, const varray<CFace>& arrayMeshF)
	{ _arrayVertex = arrayMeshV; _arrayEdge = arrayMeshE; _arrayFace = arrayMeshF; }

	const varray<double>& GetRenderVertexPos() const
	{
		return _arrayRenderVertexPos;
	}
	varray<double>& GetRenderVertexPos()
	{
		return _arrayRenderVertexPos;
	}
	
	const varray<double>& GetRenderTextureCoord() const
	{
		return _arrayRenderTextureCoord;
	}
	varray<double>& GetRenderTextureCoord()
	{
		return _arrayRenderTextureCoord;
	}

	const varray<double>& GetRenderVertexDepth() const
	{
		return _arrayRenderVertexDepth;
	}
	varray<double>& GetRenderVertexDepth()
	{
		return _arrayRenderVertexDepth;
	}

	// operators
	CMesh& operator = (const CMesh& mesh)
	{
		_arrayVertex = mesh._arrayVertex;
		_arrayEdge = mesh._arrayEdge;
		_arrayFace = mesh._arrayFace;

		_arrayRenderVertexPos = mesh._arrayRenderVertexPos;
		_arrayRenderTextureCoord = mesh._arrayRenderTextureCoord;
		_arrayRenderVertexDepth = mesh._arrayRenderVertexDepth;

		return *this;
	}

	// utilities
	void ClearMesh()
	{
		_arrayVertex.clear();
		_arrayEdge.clear();
		_arrayFace.clear();

		_arrayRenderVertexPos.clear();
		_arrayRenderTextureCoord.clear();
		_arrayRenderVertexDepth.clear();
	}

	void ClearEdge()
	{
		_arrayEdge.clear();
	}

	// Construct the adjacent relations between vertices, edges and faces
	void MakeEdges();

	// Organize the mesh for rendering
	void OrganizeMeshDataForRender();

	// Compute the center of the mesh
	void ComputeCenter();
	Vec3 GetCenter() const;
};

} // namespace base

#endif

//////////////////////////////////////////////////////////////////////////
// tips
// 1. Note: assignment between simple var array can be done by memory copy, 
//          but it may occur errors for complex objects