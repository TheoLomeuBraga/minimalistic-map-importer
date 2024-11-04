////////////////////////////////////////////////////////////////////
// Filename:	map.cpp
//
// Author:		Stefan Hajnoczi
//
// Date:		5 April 2001
//
// Description:	Code to load .MAP files.
////////////////////////////////////////////////////////////////////

#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <cstring>

#include "read_file_bytes.h"
#include "map.h"
#include "WAD3.h"

//createFile = read_file
//ReadFile = buffer_read
//SetFilePointer = buffer_jump

bool MAPFile::buffer_jump(unsigned int jump,char *byte = NULL){

	selected_byte += jump;
	if(selected_byte < 0 || selected_byte >= buffer.size()){
		return false;
	}

	if(byte != NULL){
		*byte = buffer[selected_byte];
	}
	
	return true;
}

bool MAPFile::buffer_read(char *byte){

	
	*byte = buffer[selected_byte];
	selected_byte += 1;

	if(selected_byte >= buffer.size()){
		return false;
	}

	return true;
}

MAPFile::Result MAPFile::ParseEntity(Entity **ppEntity_)
{
	//
	// Set up
	//
	if (*ppEntity_ != NULL)
	{
		delete *ppEntity_;
		*ppEntity_ = NULL;
	}

	Brush *pBrushes = NULL;

	//
	// Read {
	//
	Result result = GetToken();

	if (result == RESULT_EOF)
	{
		return RESULT_EOF;
	}
	else if (result == RESULT_FAIL)
	{
		return RESULT_FAIL;
	}

	if (strcmp("{", m_acToken) != 0)
	{
		std::cout << "Expected:\t{\nFound:\t" << m_acToken << std::endl;

		return RESULT_FAIL;
	}

	//
	// Parse properties and brushes
	//
	Entity *pEntity = new Entity;

	while (true)
	{
		char c = 0;
		unsigned int dwRead = 0;

		if (!buffer_read(&c))
		{
			std::cout << "File read error!" << std::endl;

			delete pEntity;
			pEntity = NULL;

			return RESULT_FAIL;
		}

		buffer_jump(-1);

		if (c == '"')
		{ // Property
			Property *pProperty = NULL;

			result = ParseProperty(&pProperty);

			if (result != RESULT_SUCCEED)
			{
				std::cout << "Error parsing property!" << std::endl;

				delete pEntity;
				pEntity = NULL;

				return RESULT_FAIL;
			}

			pEntity->AddProperty(pProperty);
		}
		else if (c == '{')
		{ // Brush
			Brush *pBrush = NULL;

			result = ParseBrush(&pBrush);

			if (result != RESULT_SUCCEED)
			{
				std::cout << "Error parsing brush!" << std::endl;

				delete pEntity;
				pEntity = NULL;

				return RESULT_FAIL;
			}

			if (pBrushes == NULL)
			{
				pBrushes = pBrush;
			}
			else
			{
				Brush *pTmpBrush = pBrushes;

				while (!pTmpBrush->IsLast())
				{
					pTmpBrush = pTmpBrush->GetNext();
				}

				pTmpBrush->SetNext(pBrush);
			}
		}
		else if (c == '}')
		{ // End of entity

			//
			// Perform CSG union
			//
			if (pBrushes != NULL)
			{
				pEntity->AddPoly(pBrushes->MergeList());

				delete pBrushes;

				pBrushes = NULL;
				m_iPolygons += pEntity->GetNumberOfPolys();
			}
			/*	Do not perform CSG union (useful for debugging)
						if ( pBrushes != NULL )
						{
							Brush	*pBrush = pBrushes;

							while ( pBrush != NULL )
							{
								Poly	*pPoly = pBrush->GetPolys ( );

								if ( pPoly != NULL )
								{
									pEntity->AddPoly ( pPoly->CopyList ( ) );
								}

								pBrush = pBrush->GetNext ( );
							}

							delete pBrushes;

							pBrushes	= NULL;
							m_iPolygons	+= pEntity->GetNumberOfPolys ( );
						}
			*/
			break;
		}
		else
		{ // Error
			std::cout << "Expected:\t\", {, or }\nFound:\t" << c << std::endl;

			delete pEntity;
			pEntity = NULL;

			return RESULT_FAIL;
		}
	}

	//
	// Read }
	//
	result = GetToken();

	if (result != RESULT_SUCCEED)
	{
		std::cout << "Error reading entity!" << std::endl;

		delete pEntity;
		pEntity = NULL;

		return RESULT_FAIL;
	}

	*ppEntity_ = pEntity;
	return RESULT_SUCCEED;
}

MAPFile::Result MAPFile::ParseFace(Face **ppFace_)
{
	//
	// Set up
	//
	if (*ppFace_ != NULL)
	{
		delete *ppFace_;
		*ppFace_ = NULL;
	}

	Face *pFace = new Face;

	//
	// Read plane definition
	//
	Result result;
	Vector3 p[3];

	for (int i = 0; i < 3; i++)
	{
		Vector3 v;

		result = ParseVector(v);

		if (result != RESULT_SUCCEED)
		{
			std::cout << "Error reading plane definition!" << std::endl;

			delete pFace;
			pFace = NULL;

			return RESULT_FAIL;
		}

		p[i] = v;
	}

	pFace->plane.PointsToPlane(p[0], p[1], p[2]);

	//
	// Read texture name
	//
	result = GetToken();

	if (result != RESULT_SUCCEED)
	{
		std::cout << "Error reading texture name!" << std::endl;

		delete pFace;
		pFace = NULL;

		return RESULT_FAIL;
	}

	Texture *pTexture = NULL;
	int iWAD = 0;
	bool bFound = false;
	Texture::eGT Result;

	//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
	std::cout << m_acToken << std::string(m_acToken).size() << std::endl;
	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

	if (m_pTextureList == NULL)
	{
		m_pTextureList = new Texture;

		while ((!bFound) && (iWAD < m_iWADFiles))
		{
			pTexture = m_pTextureList->GetTexture(m_acToken, m_pWAD[iWAD], m_pWADSize[iWAD], Result);
			

			if (Result == Texture::eGT::GT_LOADED)
			{
				pTexture->uiID = (unsigned int)m_iTextures;
				m_iTextures++;

				bFound = true;
			}
			else
			{
				iWAD++;
			}
		}

		m_pTextureList->SetNext(NULL);

		delete m_pTextureList;

		m_pTextureList = pTexture;
	}
	else
	{
		while ((!bFound) && (iWAD < m_iWADFiles))
		{
			pTexture = m_pTextureList->GetTexture(m_acToken, m_pWAD[iWAD], m_pWADSize[iWAD], Result);

			if (Result == Texture::eGT::GT_LOADED)
			{
				//
				// Texture had to be loaded from the WAD file
				//
				pTexture->uiID = (unsigned int)m_iTextures;
				m_iTextures++;

				bFound = true;
			}
			else if (Result == Texture::eGT::GT_FOUND)
			{
				//
				// Texture was already in texture list
				//
				bFound = true;
			}
			else
			{
				iWAD++;
			}
		}
	}

	if (!bFound)
	{
		std::cout << "Unable to find texture " << m_acToken << "!" << std::endl;

		delete pFace;
		pFace = NULL;

		return RESULT_FAIL;
	}

	pFace->pTexture = pTexture;

	//
	// Read texture axis
	//
	for (unsigned char i = 0; i < 2; i++)
	{
		Plane p;

		result = ParsePlane(p);

		if (result != RESULT_SUCCEED)
		{
			std::cout << "Error reading texture axis! (Wrong WorldCraft version?)" << std::endl;

			delete pFace;
			pFace = NULL;

			return RESULT_FAIL;
		}

		pFace->texAxis[i] = p;
	}

	//
	// Read rotation
	//
	result = GetToken();

	if (result != RESULT_SUCCEED)
	{
		std::cout << "Error reading rotation!" << std::endl;

		delete pFace;
		pFace = NULL;

		return RESULT_FAIL;
	}

	//
	// Read scale
	//
	result = GetToken();

	if (result != RESULT_SUCCEED)
	{
		std::cout << "Error reading U scale!" << std::endl;

		delete pFace;
		pFace = NULL;

		return RESULT_FAIL;
	}

	pFace->texScale[0] = atof(m_acToken) / scale;

	result = GetToken();

	if (result != RESULT_SUCCEED)
	{
		std::cout << "Error reading V scale!" << std::endl;

		delete pFace;
		pFace = NULL;

		return RESULT_FAIL;
	}

	pFace->texScale[1] = atof(m_acToken) / scale;

	result = GetToken();

	if (result != RESULT_SUCCEED)
	{
		std::cout << "Error reading face!" << std::endl;

		delete pFace;
		pFace = NULL;

		return RESULT_FAIL;
	}

	*ppFace_ = pFace;

	return RESULT_SUCCEED;
}

MAPFile::Result MAPFile::ParseBrush(Brush **ppBrush_)
{
	//
	// Set up
	//
	if (*ppBrush_ != NULL)
	{
		delete *ppBrush_;
		*ppBrush_ = NULL;
	}

	//
	// Read {
	//
	Result result = GetToken();

	if (result != RESULT_SUCCEED)
	{
		std::cout << "Error reading brush!" << std::endl;

		return RESULT_FAIL;
	}

	if (strcmp("{", m_acToken))
	{
		std::cout << "Expected:\t{\nFound:\t" << m_acToken << std::endl;

		return RESULT_FAIL;
	}

	//
	// Parse brush
	//
	Brush *pBrush = new Brush;
	Face *pFaces = NULL;
	unsigned int uiFaces = 0;

	while (true)
	{
		char c = 0;
		unsigned int dwRead = 0;

		if (!buffer_read(&c))
		{
			std::cout << "Error reading brush!" << std::endl;

			delete pBrush;
			pBrush = NULL;

			if (pFaces)
			{
				delete pFaces;
				pFaces = NULL;
			}

			return RESULT_FAIL;
		}

		buffer_jump(-1);

		if (c == '(')
		{ // Face
			Face *pFace = NULL;

			result = ParseFace(&pFace);

			if (result != RESULT_SUCCEED)
			{
				std::cout << "Error parsing face!" << std::endl;

				delete pBrush;
				pBrush = NULL;

				if (pFaces)
				{
					delete pFaces;
					pFaces = NULL;
				}

				return RESULT_FAIL;
			}

			if (pFaces == NULL)
			{
				pFaces = pFace;
			}
			else
			{
				pFaces->AddFace(pFace);
			}

			uiFaces++;
		}
		else if (c == '}')
		{ // End of brush
			break;
		}
		else
		{
			std::cout << "Expected:\t( or }\nFound:\t" << c << std::endl;

			delete pBrush;
			pBrush = NULL;

			if (pFaces)
			{
				delete pFaces;
				pFaces = NULL;
			}

			return RESULT_FAIL;
		}
	}

	result = GetToken();

	if (result != RESULT_SUCCEED)
	{
		std::cout << "Error reading brush!" << std::endl;

		delete pBrush;
		pBrush = NULL;

		return RESULT_FAIL;
	}

	Poly *pPolyList = pFaces->GetPolys();

	//
	// Sort vertices and calculate texture coordinates for every polygon
	//
	Poly *pi = pPolyList;
	Face *pFace = pFaces;

	for (int c = 0; c < uiFaces; c++)
	{
		pi->plane = pFace->plane;
		pi->TextureID = pFace->pTexture->uiID;

		pi->SortVerticesCW();

		pi->CalculateTextureCoordinates(pFace->pTexture->GetWidth(),
										pFace->pTexture->GetHeight(),
										pFace->texAxis, pFace->texScale);

		pFace = pFace->GetNext();
		pi = pi->GetNext();
	}

	pBrush->AddPoly(pPolyList);
	pBrush->CalculateAABB();

	if (pFaces != NULL)
	{
		delete pFaces;
		pFaces = NULL;
	}

	*ppBrush_ = pBrush;

	return RESULT_SUCCEED;
}

MAPFile::Result MAPFile::ParseProperty(Property **ppProperty_)
{
	//
	// Set up
	//
	if (*ppProperty_ != NULL)
	{
		delete *ppProperty_;
		*ppProperty_ = NULL;
	}

	//
	// Read name
	//
	Result result = GetString();

	if (result != RESULT_SUCCEED)
	{
		std::cout << "Error reading property name!" << std::endl;

		return RESULT_FAIL;
	}

	Property *pProperty = new Property;

	if (strcmp("mapversion", m_acToken) == 0)
	{
		//
		// Read value
		//
		result = GetString();

		if (result != RESULT_SUCCEED)
		{
			std::cout << "Error reading value of " << pProperty->GetName() << "!" << std::endl;

			delete pProperty;
			pProperty = NULL;

			return RESULT_FAIL;
		}

		if (strcmp("220", m_acToken) != 0)
		{
			std::cout << "Wrong map version!" << std::endl;

			delete pProperty;
			return RESULT_FAIL;
		}

		delete pProperty;
		*ppProperty_ = NULL;

		return RESULT_SUCCEED;
	}

	if (strcmp("wad", m_acToken) == 0)
	{
		pProperty->SetName(m_acToken);

		//
		// Read value
		//
		result = GetString();

		if (result != RESULT_SUCCEED)
		{
			std::cout << "Error reading value of " << pProperty->GetName() << "!" << std::endl;

			delete pProperty;
			pProperty = NULL;

			return RESULT_FAIL;
		}

		pProperty->SetValue(m_acToken);
		memset(m_acToken, 0, MAX_TOKEN_LENGTH + 1);

		const char *pWAD = pProperty->GetValue();
		int iToken = 0;

		for (int i = 0; i < strlen(pWAD) + 1; i++)
		{
			if ((pWAD[i] == ';') || (pWAD[i] == 0x00))
			{
				if (m_pWAD == NULL)
				{
					m_pWAD = new void *[m_iWADFiles + 1];
					m_pWADSize = new unsigned int[m_iWADFiles + 1];
				}
				else
				{
					void **pOldWAD = new void *[m_iWADFiles];
					memcpy(pOldWAD, m_pWAD, sizeof(void *) * (m_iWADFiles));
					delete m_pWAD;

					m_pWAD = new void *[m_iWADFiles + 1];
					memcpy(m_pWAD, pOldWAD, sizeof(void *) * (m_iWADFiles));

					delete[] pOldWAD;

					unsigned int *pOldSize = new unsigned int[m_iWADFiles];
					memcpy(pOldSize, m_pWADSize, sizeof(unsigned int) * (m_iWADFiles));
					delete m_pWADSize;

					m_pWADSize = new unsigned int[m_iWADFiles + 1];
					memcpy(m_pWADSize, pOldSize, sizeof(unsigned int) * (m_iWADFiles));

					delete[] pOldSize;
				}

				//MapFile(m_acToken, &m_pWAD[m_iWADFiles], &m_pWADSize[m_iWADFiles]);

				std::vector<char> buffer;
				read_file(&m_acToken[0], &buffer);
				m_pWADSize[m_iWADFiles] = buffer.size();
				m_pWAD[m_iWADFiles] = static_cast<void*>(buffer.data());

				

				iToken = 0;
				m_iWADFiles++;
				memset(m_acToken, 0, MAX_TOKEN_LENGTH + 1);
			}
			else
			{
				m_acToken[iToken] = pWAD[i];
				iToken++;
			}
		}

		*ppProperty_ = pProperty;

		return RESULT_SUCCEED;
	}

	pProperty->SetName(m_acToken);

	//
	// Read value
	//
	result = GetString();

	if (result != RESULT_SUCCEED)
	{
		std::cout << "Error reading value of " << pProperty->GetName() << "!" << std::endl;

		delete pProperty;
		pProperty = NULL;

		return RESULT_FAIL;
	}

	pProperty->SetValue(m_acToken);

	*ppProperty_ = pProperty;

	return RESULT_SUCCEED;
}

bool MAPFile::Load(char *pcFile_, Entity **ppEntities_, Texture **ppTextures_)
{
	//
	// Check if parameters are valid
	//
	
	if (pcFile_ == NULL)
	{
		return false;
	}

	if (ppEntities_ == NULL)
	{
		return false;
	}

	if (*ppEntities_ != NULL)
	{
		delete *ppEntities_;
		*ppEntities_ = NULL;
	}

	

	m_pWAD = NULL;
	m_pWADSize = NULL;
	m_pTextureList = NULL;

	m_iEntities = 0;
	m_iPolygons = 0;
	m_iTextures = 0;

	m_iWADFiles = 0;

	//
	// Open .MAP file
	//

	

	selected_byte = 0;
	if (!read_map_file(pcFile_,&buffer))
	{ 
		std::cout << "File read file error!" << std::endl;
		return false;
	}

	

	//
	// Parse file
	//
	Entity *pEntityList = NULL;

	while (true)
	{
		Entity *pEntity = NULL;
		Result result = ParseEntity(&pEntity);

		if (result == RESULT_EOF)
		{
			break;
		}
		else if (result == RESULT_FAIL)
		{

			if (pEntity != NULL)
			{
				delete pEntity;
				pEntity = NULL;
			}

			if (pEntityList != NULL)
			{
				delete pEntityList;
				pEntityList = NULL;
			}

			delete[] m_pWAD;
			delete[] m_pWADSize;
			delete m_pTextureList;

			return false;
		}

		if (pEntityList == NULL)
		{
			pEntityList = pEntity;
		}
		else
		{
			pEntityList->AddEntity(pEntity);
		}

		m_iEntities++;
	}

	//
	// Clean up and return
	//
	std::cout << "Entities: " << m_iEntities << std::endl;
	std::cout << "Polygons: " << m_iPolygons << std::endl;
	std::cout << "Textures: " << m_iTextures << std::endl;

	delete[] m_pWAD;
	delete[] m_pWADSize;

	*ppEntities_ = pEntityList;
	*ppTextures_ = m_pTextureList;

	return true;
}

MAPFile::Result MAPFile::ParsePlane(Plane &p_)
{
	Result result = GetToken();

	if (result != RESULT_SUCCEED)
	{
		return RESULT_FAIL;
	}

	if (strcmp("[", m_acToken))
	{
		return RESULT_FAIL;
	}

	result = GetToken();

	if (result != RESULT_SUCCEED)
	{
		return RESULT_FAIL;
	}

	p_.n.x = atof(m_acToken);

	result = GetToken();

	if (result != RESULT_SUCCEED)
	{
		return RESULT_FAIL;
	}

	p_.n.z = atof(m_acToken);

	result = GetToken();

	if (result != RESULT_SUCCEED)
	{
		return RESULT_FAIL;
	}

	p_.n.y = atof(m_acToken);

	result = GetToken();

	if (result != RESULT_SUCCEED)
	{
		return RESULT_FAIL;
	}

	p_.d = atof(m_acToken);

	result = GetToken();

	if (result != RESULT_SUCCEED)
	{
		return RESULT_FAIL;
	}

	if (strcmp("]", m_acToken))
	{
		return RESULT_FAIL;
	}

	return RESULT_SUCCEED;
}

MAPFile::Result MAPFile::ParseVector(Vector3 &v_)
{
	Result result = GetToken();

	if (result != RESULT_SUCCEED)
	{
		return RESULT_FAIL;
	}

	if (strcmp("(", m_acToken))
	{
		return RESULT_FAIL;
	}

	result = GetToken();

	if (result != RESULT_SUCCEED)
	{
		return RESULT_FAIL;
	}

	v_.x = atof(m_acToken) / scale;

	result = GetToken();

	if (result != RESULT_SUCCEED)
	{
		return RESULT_FAIL;
	}

	v_.z = atof(m_acToken) / scale;

	result = GetToken();

	if (result != RESULT_SUCCEED)
	{
		return RESULT_FAIL;
	}

	v_.y = atof(m_acToken) / scale;

	result = GetToken();

	if (result != RESULT_SUCCEED)
	{
		return RESULT_FAIL;
	}

	if (strcmp(")", m_acToken))
	{
		return RESULT_FAIL;
	}

	return RESULT_SUCCEED;
}

MAPFile::Result MAPFile::GetToken()
{
	unsigned int i = 0;
	char c = 0;
	unsigned int dwRead = 0;

	memset(&m_acToken, 0, sizeof(m_acToken));

	while (i <= MAX_TOKEN_LENGTH)
	{

		if (!buffer_read(&c))
		{
			return RESULT_FAIL;
		}

		//
		// Check for token end
		//
		if (c == ' ')
		{
			break;
		}

		if (c == 0x0A)
		{
			break;
		}

		if (c != 0x0D)
		{
			m_acToken[i] = c;
		}

		i++;
	}

	return RESULT_SUCCEED;
}

MAPFile::Result MAPFile::GetString()
{
	unsigned int i = 0;
	char c = 0;
	bool bFinished = false;
	unsigned int dwRead = 0;

	memset(&m_acToken, 0, sizeof(m_acToken));

	//
	// Read first "
	//
	if (!buffer_read(&c))
	{
		if (dwRead == 0)
		{
			return RESULT_EOF;
		}

		return RESULT_FAIL;
	}

	//
	// Parse rest of string
	//
	while (i <= MAX_TOKEN_LENGTH)
	{
		if (!buffer_read(&c))
		{
			return RESULT_FAIL;
		}

		//
		// Check for token end
		//
		if (c == '"')
		{
			bFinished = true;
		}

		if (bFinished && (c == ' '))
		{
			break;
		}

		if (bFinished && (c == 0x0A))
		{
			break;
		}

		if (!bFinished)
		{
			m_acToken[i] = c;
		}

		i++;
	}

	return RESULT_SUCCEED;
}