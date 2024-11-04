#include "map.h"
#include "WAD3.h"
#include <cstring>


////////////////////////////////////////////////////////////////////
// Texture member functions
////////////////////////////////////////////////////////////////////

Texture* Texture::GetTexture ( char *pacTexture_, void* lpView_, unsigned int dwFileSize_, Texture::eGT &rResult_ )
{
	rResult_ = eGT::GT_ERROR;

	//
	// Check if texture already exists
	//
	if ( strncmp ( name, pacTexture_ ,MAX_TEXTURE_LENGTH + 1) == 0 )
	{
		rResult_ = eGT::GT_FOUND;

		return this;
	}

	if ( !IsLast ( ) )
	{
		return m_pNext->GetTexture ( pacTexture_, lpView_, dwFileSize_, rResult_ );
	}

	//
	// Load texture information
	//
	Texture			*pTexture		= new Texture;
	LPWAD3_HEADER	lpHeader		= NULL;
	LPWAD3_LUMP		lpLump			= NULL;
	LPWAD3_MIP		lpMip			= NULL;

	unsigned int			dwNumLumps		= 0;
	unsigned int			dwTableOffset	= 0;
	unsigned int			dwFilePos		= 0;
	unsigned int			dwPaletteOffset	= 0;
	unsigned int			wPaletteSize	= 0;
	unsigned int			dwWidth			= 0;
	unsigned int			dwHeight		= 0;

	// Make sure it's at least big enough to manipulate the header
	if (dwFileSize_ < sizeof(WAD3_HEADER))
	{

		delete pTexture;

		return NULL;
	}

	lpHeader = (LPWAD3_HEADER)lpView_;

	if (lpHeader->identification != WAD3_ID)
	{

		delete pTexture;

		return NULL;
	}

	dwNumLumps = lpHeader->numlumps;
	dwTableOffset = lpHeader->infotableofs;

	// Make sure our table is really there
	if ( ((dwNumLumps * sizeof(WAD3_LUMP)) + dwTableOffset) > dwFileSize_)
	{

		delete pTexture;

		return NULL;
	}

	// Point at the first table entry
	lpLump = (LPWAD3_LUMP)((unsigned char*)lpView_ + dwTableOffset);

	bool	bFound = false;
	unsigned int	j = 0;

	while ( ( !bFound ) && ( j < dwNumLumps ) )
	{		
		if ( lpLump->type == WAD3_TYPE_MIP)
		{
			if ( strncmp ( lpLump->name, pacTexture_,16 ) == 0 )
			{
				// Find out where the MIP actually is
				dwFilePos = lpLump->filepos;
				
				// Make sure it's in bounds
				if ( dwFilePos >= dwFileSize_ )
				{

					delete pTexture;

					return NULL;
				}

				// Point at the mip
				lpMip = ( LPWAD3_MIP )( (unsigned char* )lpView_ + dwFilePos );

				strcpy ( pTexture->name, pacTexture_ );

				pTexture->m_iWidth	= lpMip->width;
				pTexture->m_iHeight	= lpMip->height;
				bFound				= true;
			}
		}

		j++;
		lpLump++;
	}

	if ( !bFound )
	{
		delete pTexture;

		return NULL;
	}

	m_pNext = pTexture;

	rResult_ = eGT::GT_LOADED;

	return pTexture;
}


Texture::Texture ( )
{
	memset ( name, 0, MAX_TEXTURE_LENGTH + 1 );

	m_pNext		= NULL;
	m_iHeight	= 0;
	m_iWidth	= 0;
}


Texture::~Texture ( )
{
	if ( !IsLast ( ) )
	{
		delete m_pNext;
		m_pNext = NULL;
	}
}


void Texture::SetNext ( Texture *pTexture_ )
{
	if ( IsLast ( ) )
	{
		m_pNext = pTexture_;

		return;
	}

	//
	// Insert the given list
	//
	if ( pTexture_ != NULL )
	{
		Texture *pTexture = pTexture_;

		while ( !pTexture->IsLast ( ) )
		{
			pTexture = pTexture->GetNext ( );
		}

		pTexture->SetNext ( m_pNext );
	}

	m_pNext = pTexture_;
}


bool Texture::IsLast ( ) const
{
	if ( m_pNext == NULL )
	{
		return true;
	}

	return false;
}