#pragma once

// WAD3 (Half-Life) Header and mip structs
#include <iostream>

using namespace std;

#define WAD3_TYPE_MIP	0x43
#define WAD3_ID			('W' | 'A' << 8 | 'D' << 16 | '3' << 24)
#define GET_MIP_DATA_SIZE(WIDTH, HEIGHT)		(sizeof(WAD3_MIP) + (WIDTH * HEIGHT) + (WIDTH * HEIGHT / 4) + (WIDTH * HEIGHT / 16) + (WIDTH * HEIGHT / 64))


////////////////////////////////////////////////////////////////////////////////
// WAD3 pseudo-structure:
/*

	WAD3 Header		
	Mip section
		First mip
			Mip header			
			First mip (width * height)
			Second mip (width * height / 4)
			Third mip (width * height / 16)
			Fourth mip (width * height / 64)
			Palette size (WORD)
			Palette (Palette size * 3)
			Padding (WORD)			
		[...]
		Last mip	
	Lump table
		First lump entry
			Lump header
		[...]
		Last lump entry
*/
////////////////////////////////////////////////////////////////////////////////

typedef struct
{
	unsigned int		identification;
	unsigned int		numlumps;
	unsigned int		infotableofs;			// Lump table
} WAD3_HEADER, *LPWAD3_HEADER;

typedef struct
{
	unsigned int		filepos;
	unsigned int		disksize;
	unsigned int		size;					// uncompressed
	unsigned char		type;
	unsigned char		compression;
	unsigned char		pad1, pad2;
	char		name[16];				// must be null terminated
} WAD3_LUMP, *LPWAD3_LUMP;

typedef struct
{
	char		name[16];
	unsigned int		width, height;
	unsigned int		offsets[4];		// four mip maps stored
} WAD3_MIP, *LPWAD3_MIP;

extern void MapFile( const char* szFileName, void **pView, unsigned int pdwFileSize);
extern int CorruptWAD3( const char* szErrorMessage, void* lpView);
/*
class CWADException
{
private:
	unsigned int m_dwError;
	const char* m_szErrorMessage;

public:
	CWADException()
	{
		m_szErrorMessage = NULL;
		m_dwError = ::GetLastError();
	}
	CWADException( unsigned int dwError)
	{
		m_szErrorMessage = NULL;
		m_dwError = dwError;
	}
	CWADException( const char* szErrorMessage)
	{
		m_dwError = 0;
		m_szErrorMessage = szErrorMessage;
	}
	~CWADException() {};

	void PrintError()
	{
		if (m_dwError)
		{
			void* lpMsgBuf;
			FormatMessage( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				m_dwError,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(LPTSTR) &lpMsgBuf,
				0,
				NULL 
			);
			cout << "Whoops, something went wrong.  GetLastError() = " << m_dwError << endl << (LPTSTR)lpMsgBuf << endl;

			LocalFree( lpMsgBuf);
		}

		if (m_szErrorMessage)
		{
			cout << "Whoops, something went wrong. " << endl << m_szErrorMessage << endl;
		}
	}
};
*/