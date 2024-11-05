#pragma once
#include <string>

////////////////////////////////////////////////////////////////////
// Class definitions
////////////////////////////////////////////////////////////////////

class Face
{
private:
	Face	*m_pNext;

public:
	Plane	plane;
	float	texOffset[ 2 ];
	float   texRotation;
	float	texScale[ 2 ];
	std::string pTexture;

	Face *GetNext ( ) const { return m_pNext; }

	void AddFace ( Face *pFace_ );
	void SetNext ( Face *pFace_ );

	Poly *GetPolys ( );

	bool IsLast ( ) const;

	Face ( );
	~Face ( );
};


class Brush
{
private:
	Vector3	min, max;	// AABB around brush used to quickly reject far off brushes in CSG
	Brush	*m_pNext;
	Poly	*m_pPolys;

public:
	Brush *GetNext ( ) const { return m_pNext; }
	Poly *GetPolys ( ) { return m_pPolys; }
	Brush *CopyList ( ) const;

	void SetNext ( Brush *pBrush_ );
	void AddPoly ( Poly *pPoly_ );

	Poly *MergeList ( );
	void ClipToBrush ( Brush *pBrush_, bool bClipOnPlane_ );
	void CalculateAABB ( );

	unsigned int GetNumberOfPolys ( ) const;
	unsigned int GetNumberOfBrushes ( ) const;

	bool IsLast ( ) const;
	bool AABBIntersect ( Brush *pBrush_ );

	Brush ( );
	~Brush ( );
};