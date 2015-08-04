/*-----------------------------------------------------------------
/ Renders an object using Bezier patches.  The loading function
/ loads from a .bez file.  This will need to be modified if
/ that changes.
/-----------------------------------------------------------------*/

#ifndef _BEZIER_
#define _BEZIER_

#include "Vertex.h"
#include <fstream>

class C_BezierPatch
{
private:

	// The size (width, height) of the patch, the amount to tesselate
	int d_size;		

	// The control points for this patch
	vector3 d_ctrlPoints[16];

	// Holds the tesselated points
	Vertex* d_vertexList;

	ID3D10Buffer* d_pMainVertexBuffer;
	ID3D10Buffer* d_pMainIndexBuffer;
	ID3D10Buffer* d_pNetVertexBuffer;

	DWORD* d_indexList;

	vector3* d_uTangList;
	vector3* d_vTangList;

	void Clean()
	{
		// Delete the vertex list
		if(d_vertexList)
			delete[] d_vertexList;

		delete[] d_indexList;
		delete[] d_uTangList;
		delete[] d_vTangList;
	}

	static matrix4x4 d_basisMatrix;

	static short netIndcies[48];

	friend class C_FwdDiffIterator;
	friend class C_TangentIterator;
	friend class C_BezierObject;

public:
	C_BezierPatch();
	~C_BezierPatch();

	void Init(int size);

	// Fill the vert list with the tesselated values
	void Tesselate();

	void Draw(bool drawNet);

	void Load(std::ifstream& file);
};


class C_BezierObject
{
private:
	int d_numPatches;
	C_BezierPatch* d_patchList;

public:

	C_BezierObject();
	~C_BezierObject();
	
	// Increment the level of patch tesselation in the object
	void IncTesselation()
	{
		for(int i = 0; i < d_numPatches; i++)
		{
			if( d_patchList[i].d_size < 10 )
				d_patchList[i].Init( d_patchList[i].d_size+1 );
			else
				d_patchList[i].Init( d_patchList[i].d_size+2 );
		}
	}

	// Decrement the level of patch tesselation in the object
	void DecTesselation()
	{
		for(int i = 0; i < d_numPatches; i++)
		{
			if( d_patchList[i].d_size > 10 )
				d_patchList[i].Init( d_patchList[i].d_size-2 );
			else
				d_patchList[i].Init( d_patchList[i].d_size-1 );
		}
	}

	void Load( const char* fileName );
	void Draw( const matrix4x4& wvp, bool drawNet );
};


class C_TangentIterator
{
	int d_i;				// Current step in the iteration
	int d_numSteps;			// The number of steps
	vector4 d_p[3];			// For x, y and z
	vector3 d_ctrlPoints[4];

	vector3 d_Q;	// The point at the current iteration location

	void CalcQ()
	{
		float t = (float)d_i / (d_numSteps-1);
		vector4 tVec( 3*t*t, 2*t, 1, 0 );

		vector4 temp = tVec * C_BezierPatch::d_basisMatrix;
		d_Q.x = d_p[0].Dot(temp);
		d_Q.y = d_p[1].Dot(temp);
		d_Q.z = d_p[2].Dot(temp);
	}

public:

	C_TangentIterator( int numSteps, vector3 p1, vector3 p2, vector3 p3, vector3 p4 )
	{
		d_ctrlPoints[0] = p1;
		d_ctrlPoints[1] = p2;
		d_ctrlPoints[2] = p3;
		d_ctrlPoints[3] = p4;

		d_numSteps = numSteps;
		d_p[0] = vector4( p1.x, p2.x, p3.x, p4.x );
		d_p[1] = vector4( p1.y, p2.y, p3.y, p4.y );
		d_p[2] = vector4( p1.z, p2.z, p3.z, p4.z );
	}

	void Start()
	{
		d_i = 0;
		CalcQ();
	}

	bool Done()
	{
		return !(d_i < d_numSteps);
	}

	vector3& GetCurr()
	{
		return d_Q;
	}

	operator vector3&()
	{
		return d_Q;
	}

	void CalcNext()
	{
		d_i++;
		CalcQ();
	}
};

class C_FwdDiffIterator
{
	// The current step in the iteration
	int d_i;

	// The number of steps
	int d_numSteps;

	// The 4 control points
	vector3 d_p[4];

	// The point at the current iteration location
	vector3 d_Q;

	vector3 d_dQ;	// The first derivative (initially at zero)
	vector3 d_ddQ;	// The Second derivative (initially zero)
	vector3 d_dddQ;	// The third derivative (constant)

public:
	C_FwdDiffIterator() {}  // Do nothing

	C_FwdDiffIterator( int numSteps, vector3 p1, vector3 p2, vector3 p3, vector3 p4)
	{
		numSteps = numSteps;
		d_p[0] = p1;
		d_p[1] = p2;
		d_p[2] = p3;
		d_p[3] = p4;
	}

	void Start()
	{
		d_i = 0;

		float d = 1.0f/(d_numSteps-1);
		float d2 = d*d;		// d^2
		float d3 = d*d2;	// d^3

		vector4 px( d_p[0].x, d_p[1].x, d_p[2].x, d_p[3].x );
		vector4 py( d_p[0].y, d_p[1].y, d_p[2].y, d_p[3].y );
		vector4 pz( d_p[0].z, d_p[1].z, d_p[2].z, d_p[3].z );

		vector4 cVec[3]; // <a, b, c, d>  for x, y, z
		cVec[0] = px * C_BezierPatch::d_basisMatrix;
		cVec[1] = py * C_BezierPatch::d_basisMatrix;
		cVec[2] = pz * C_BezierPatch::d_basisMatrix;

		d_Q = d_p[0];

		// t = 0
		// for X
		float a = cVec[0].x;
		float b = cVec[0].y;
		float c = cVec[0].z;
		d_dQ.x = a * d3 + b * d2 + c * d;
		d_ddQ.x = 6 * a * d3 + 2 * b * d2;
		d_dddQ.x = 6 * a * d3;

		// for Y
		a = cVec[1].x;
		b = cVec[1].y;
		c = cVec[1].z;
		d_dQ.y = a * d3 + b * d2 + c * d;
		d_ddQ.y = 6 * a * d3 + 2 * b * d2;
		d_dddQ.y = 6 * a * d3;

		// for Z
		a = cVec[2].x;
		b = cVec[2].y;
		c = cVec[2].z;
		d_dQ.z = a * d3 + b * d2 + c * d;
		d_ddQ.z = 6 * a * d3 + 2 * b * d2;
		d_dddQ.z = 6 * a * d3;
	}

	bool Done()
	{
		return !(d_i < d_numSteps);
	}

	vector3& GetCurr()
	{
		return d_Q;
	}

	operator vector3&()
	{
		return d_Q;
	}

	void CalcNext()
	{
		d_Q += d_dQ;
		d_dQ += d_ddQ;
		d_ddQ += d_dddQ;

		d_i++;
	}
};

#endif // _BEZIER_