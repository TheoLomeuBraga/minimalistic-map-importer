#pragma once

////////////////////////////////////////////////////////////////////
// Constants
////////////////////////////////////////////////////////////////////

const unsigned int MAX_TOKEN_LENGTH		= 512;
const unsigned int MAX_TEXTURE_LENGTH	= 16;


////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>

using namespace std;

#ifdef _DEBUG
#include <crtdbg.h>

#ifdef new
#undef new
#endif

#define new new(_NORMAL_BLOCK,__FILE__, __LINE__)

#endif

#include "math.h"
#include "entity.h"
#include "brush.h"
#include <vector>

////////////////////////////////////////////////////////////////////
// Classes
////////////////////////////////////////////////////////////////////

class MAPFile
{
private:
	enum Result
	{
		RESULT_SUCCEED = 0, RESULT_FAIL, RESULT_EOF
	};

	char	m_acToken[ MAX_TOKEN_LENGTH + 1 ];

	int	m_hFile;

	std::vector<char> buffer;
	unsigned int selected_byte;
	bool buffer_jump(unsigned int jump,char *byte);


	int		m_iWADFiles;
	void	**m_pWAD;
	unsigned int	*m_pWADSize;
	Texture	*m_pTextureList;

	int		m_iEntities;
	int		m_iPolygons;
	int		m_iTextures;

	Result GetToken ( );
	Result GetString ( );

	Result ParseEntity ( Entity **ppEntity_ );
	Result ParseProperty ( Property **ppProperty_ );
	Result ParseBrush ( Brush **ppBrush_ );
	Result ParseFace ( Face **ppFace_ );
	Result ParseVector ( Vector3 &v_ );
	Result ParsePlane ( Plane &p_ );

public:
	bool Load ( char *pcFile_, Entity **ppEntities_, Texture **pTexture_ );
};