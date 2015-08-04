
#include "I_Engine.h"
#include "C_BezierObject.h"

matrix4x4 C_BezierPatch::d_basisMatrix( D3DXMATRIX(
    -1.0f,	 3.0f,	-3.0f,	 1.0f,
	 3.0f,	-6.0f,	 3.0f,	 0.0f,
	-3.0f,	 3.0f,	 0.0f,	 0.0f,
	 1.0f,	 0.0f,	 0.0f,	 0.0f)
);


C_BezierPatch::C_BezierPatch() : d_vertexList(0), d_indexList(0), 
d_uTangList(0), d_vTangList(0), d_pMainVertexBuffer(0), 
d_pMainIndexBuffer(0), d_pNetVertexBuffer(0)
{}

C_BezierPatch::~C_BezierPatch()
{
	Clean();
}

void C_BezierPatch::Init(int size)
{
	Clean();

	d_size = size;

	// Allocate the lists
	d_vertexList = new Vertex[size * size];
	d_indexList = new DWORD[ (size-1) * (size-1) * 6 ];
	d_uTangList = new vector3[size * size];
	d_vTangList = new vector3[size * size];

	Tesselate();

	/*= {
		0, 1, 1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 
		8, 9, 9, 10, 10, 11, 12, 13, 13, 14, 14, 15,
		0, 4, 4, 8, 8, 12, 1, 5, 5, 9, 9, 13,
		2, 6, 6, 10, 10, 14, 3, 7, 7, 11, 11, 15 };*/

	ID3D10Buffer* pNetIndexBuffer = NULL;
	ID3D10Buffer* pNetVertexBuffer = NULL;
}

void C_BezierPatch::Tesselate()
{
	int u, v; // Patch space coordinates
	vector3 p1, p2, p3, p4;

	// These are the four curves
	int numStep = d_size;
	C_FwdDiffIterator mainCurve1( numStep, d_ctrlPoints[0], d_ctrlPoints[4], d_ctrlPoints[8], d_ctrlPoints[12] );
	C_FwdDiffIterator mainCurve2( numStep, d_ctrlPoints[1], d_ctrlPoints[5], d_ctrlPoints[9], d_ctrlPoints[13] );
	C_FwdDiffIterator mainCurve3( numStep, d_ctrlPoints[2], d_ctrlPoints[6], d_ctrlPoints[10], d_ctrlPoints[14]);
	C_FwdDiffIterator mainCurve4( numStep, d_ctrlPoints[3], d_ctrlPoints[7], d_ctrlPoints[11], d_ctrlPoints[15]);

	mainCurve1.Start();
	mainCurve2.Start();
	mainCurve3.Start();
	mainCurve4.Start();

	for(v = 0; v < d_size; v++)
	{
		// Generate the four control points for this curve
		p1 = mainCurve1.GetCurr();
		p2 = mainCurve2.GetCurr();
		p3 = mainCurve3.GetCurr();
		p4 = mainCurve4.GetCurr();

		// Now step along the curve filling in the data
		C_TangentIterator tanIter( numStep, p1, p2, p3, p4 );
		tanIter.Start();
		C_FwdDiffIterator iter( numStep, p1, p2, p3, p4 );
		u = 0;
		for(iter.Start(); !iter.Done(); iter.CalcNext(), u++)
		{
			d_vertexList[d_size*v+u].pos = iter.GetCurr();
			d_vertexList[d_size*v+u].texC = D3DXVECTOR2(0,0);
			d_vertexList[d_size*v+u].normal = vector3(0,0,0);

			d_uTangList[d_size*v+u] = tanIter.GetCurr();
			tanIter.CalcNext();
		}

		mainCurve1.CalcNext();
		mainCurve2.CalcNext();
		mainCurve3.CalcNext();
		mainCurve4.CalcNext();
	}

	// Now Calculate the v-tangents
	// Repeat the process but in the perpendicular direction
	mainCurve1 = C_FwdDiffIterator( numStep,  d_ctrlPoints[0],  d_ctrlPoints[1],  d_ctrlPoints[2],  d_ctrlPoints[3]);
	mainCurve2 = C_FwdDiffIterator( numStep,  d_ctrlPoints[4],  d_ctrlPoints[5],  d_ctrlPoints[6],  d_ctrlPoints[7]);
	mainCurve3 = C_FwdDiffIterator( numStep,  d_ctrlPoints[8],  d_ctrlPoints[9],  d_ctrlPoints[10],  d_ctrlPoints[11]);
	mainCurve4 = C_FwdDiffIterator( numStep,  d_ctrlPoints[12],  d_ctrlPoints[13],  d_ctrlPoints[14],  d_ctrlPoints[15]);

	mainCurve1.Start();
	mainCurve2.Start();
	mainCurve3.Start();
	mainCurve4.Start();

	for(v = 0; v < d_size; v++)
	{
		p1 = mainCurve1.GetCurr();
		p2 = mainCurve2.GetCurr();
		p3 = mainCurve3.GetCurr();
		p4 = mainCurve4.GetCurr();
		
		C_TangentIterator iter( numStep, p1, p2, p3, p4 );
		u = 0;
		for( iter.Start(); !iter.Done(); iter.CalcNext(), u++ )
		{
			d_vTangList[d_size*u+v] = iter.GetCurr();
		}

		mainCurve1.CalcNext();
		mainCurve2.CalcNext();
		mainCurve3.CalcNext();
		mainCurve4.CalcNext();
	}

	int offset;
	for(v = 0; v < d_size; v++)
	{
		// Tesselate across the horizontal bezier
		for(u = 0; u < d_size; u++)
		{
			offset = d_size*v + u;

			vector3 norm;
			norm = d_vTangList[offset].Cross(d_uTangList[offset]);
			norm.Normalize();

			d_vertexList[offset].normal = norm;
			d_vertexList[offset].texC.x = 0;
			d_vertexList[offset].texC.y = 0;
		}
	}

	// Now build the Index list
	offset = 0;
	for(v = 0; v < (d_size-1); v++)
	{
		for(u = 0; u < (d_size-1); u++)
		{
			// 0, 1, 2
			d_indexList[offset+0] = d_size*(v+0) + (u+0);
			d_indexList[offset+1] = d_size*(v+0) + (u+1);
			d_indexList[offset+2] = d_size*(v+1) + (u+1);

			// 2, 3, 0
			d_indexList[offset+3] = d_size*(v+1) + (u+1);
			d_indexList[offset+4] = d_size*(v+1) + (u+0);
			d_indexList[offset+5] = d_size*(v+0) + (u+0);
			offset += 6;
		}
	}
}

void C_BezierPatch::Load(std::ifstream& file)
{
	for(int pointIndex = 0; pointIndex < 4; pointIndex++)
	{
		file >> d_ctrlPoints[pointIndex*4 + 0].x;
		file >> d_ctrlPoints[pointIndex*4 + 0].y;
		file >> d_ctrlPoints[pointIndex*4 + 0].z;
		file >> d_ctrlPoints[pointIndex*4 + 1].x;
		file >> d_ctrlPoints[pointIndex*4 + 1].y;
		file >> d_ctrlPoints[pointIndex*4 + 1].z;
		file >> d_ctrlPoints[pointIndex*4 + 2].x;
		file >> d_ctrlPoints[pointIndex*4 + 2].y;
		file >> d_ctrlPoints[pointIndex*4 + 2].z;
		file >> d_ctrlPoints[pointIndex*4 + 3].x;
		file >> d_ctrlPoints[pointIndex*4 + 3].y;
		file >> d_ctrlPoints[pointIndex*4 + 3].z;
	}

	Init(10);
	Tesselate();
}

void C_BezierPatch::Draw(bool drawNet)
{
	

	if(bFirst)
	{
		D3D10_BUFFER_DESC descBuffer;
		memset(&descBuffer, 0, sizeof(descBuffer));
		descBuffer.Usage = D3D10_USAGE_DEFAULT;
		descBuffer.ByteWidth = sizeof(netIndices);        
		descBuffer.BindFlags = D3D10_BIND_INDEX_BUFFER;
		descBuffer.CPUAccessFlags = 0;
		descBuffer.MiscFlags = 0;

		D3D10_SUBRESOURCE_DATA resData;
		memset(&resData, 0, sizeof(resData));
		resData.pSysMem = netIndices;

		// Get the device from the Application layer
		G_App->GetDevice()->CreateBuffer(&descBuffer, &resData, &pNetIndexBuffer);

	}

	if(!pNetIndexBuffer)
		return;



}


C_BezierObject::C_BezierObject() : d_patchList(0)
{}

C_BezierObject::~C_BezierObject()
{
	delete[] d_patchList;
}

void C_BezierObject::Load(const char* fileName)
{
	const int sz = 1024;
	char buff[1024];

	std::ifstream file(fileName);

	if( !file.is_open() )
		return;

	// Read in the number of patches
	file >> d_numPatches;
	d_patchList = new C_BezierPatch[d_numPatches];

	// read in the blank line
	file.getline(buff, sz);

	for(int i = 0; i < d_numPatches; i++)
	{
		d_patchList[i].Load(file);
		// Read in blank line after patch set
		file.getline(buff, sz);
	}
}

void C_BezierObject::Draw(const matrix4x4& wvp, bool drawNet)
{

}