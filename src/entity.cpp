#include "map.h"


////////////////////////////////////////////////////////////////////
// Entity member functions
////////////////////////////////////////////////////////////////////

std::vector<Triangle> Poly::convert_to_triangles() {
    std::vector<Triangle> triangles;

    // Verifica se o polígono tem pelo menos três vértices
    if (GetNumberOfVertices() < 3) {
        return triangles; // Retorna vazio se não há vértices suficientes
    }

    // Itera pelos vértices do polígono para formar triângulos em "fã"
    for (int i = 1; i < GetNumberOfVertices() - 1; ++i) {
        Triangle triangle;

        // Primeiro vértice do triângulo é sempre o primeiro vértice do polígono
        triangle.vertex[0] = verts[0];

        // Segundo e terceiro vértices do triângulo são os vértices i e i+1
        triangle.vertex[1] = verts[i];
        triangle.vertex[2] = verts[i + 1];

        // Adiciona o triângulo à lista de triângulos
        triangles.push_back(triangle);
    }

    return triangles;
}

void Entity::WriteEntity ( std::ofstream &ofsFile_ ) const
{
/*	Entity:
	x char		Entity class (zero terminated)
	1 uint		Number of properties
	x Property	Entities properties
	1 uint		Number of polygons
	x Polygon	Polygons */

	ofsFile_ << m_pProperties->GetValue ( ) << ( char )0x00;

	unsigned int ui = GetNumberOfProperties ( ) - 1;

	ofsFile_.write ( ( char * )&ui, sizeof ( ui ) );

	if ( !m_pProperties->IsLast ( ) )
	{
		m_pProperties->GetNext ( )->WriteProperty ( ofsFile_ );
	}

	ui = GetNumberOfPolys ( );

	ofsFile_.write ( ( char * )&ui, sizeof ( ui ) );

	if ( GetNumberOfPolys ( ) > 0 )
	{
		m_pPolys->WritePoly ( ofsFile_ );
	}

	if ( !IsLast ( ) )
	{
		GetNext ( )->WriteEntity ( ofsFile_ );
	}
}


Entity::Entity ( )
{
	m_pNext			= NULL;
	m_pProperties	= NULL;
	m_pPolys		= NULL;
}


Entity::~Entity ( )
{
	if ( m_pProperties != NULL )
	{
		delete m_pProperties;
		m_pProperties = NULL;
	}

	if ( m_pPolys != NULL )
	{
		delete m_pPolys;
		m_pPolys = NULL;
	}

	if ( m_pNext != NULL )
	{
		delete m_pNext;
		m_pNext = NULL;
	}
}


bool Entity::IsLast ( ) const
{
	if ( m_pNext == NULL )
	{
		return true;
	}

	return false;
}


void Entity::AddEntity ( Entity *pEntity_ )
{
	if ( IsLast ( ) )
	{
		m_pNext = pEntity_;

		return;
	}

	Entity *pEntity = m_pNext;

	while ( !pEntity->IsLast ( ) )
	{
		pEntity = pEntity->GetNext ( );
	}

	pEntity->m_pNext = pEntity_;
}


void Entity::AddProperty ( Property *pProperty_ )
{
	if ( m_pProperties == NULL )
	{
		m_pProperties = pProperty_;

		return;
	}

	Property *pProperty = m_pProperties;

	while ( !pProperty->IsLast () )
	{
		pProperty = pProperty->GetNext ( );
	}

	pProperty->SetNext ( pProperty_ );
}


void Entity::AddPoly ( Poly *pPoly_ )
{
	if ( m_pPolys == NULL )
	{
		m_pPolys = pPoly_;

		return;
	}

	Poly *pPoly = m_pPolys;

	while ( !pPoly->IsLast ( ) )
	{
		pPoly = pPoly->GetNext ( );
	}

	pPoly->SetNext ( pPoly_ );
}


unsigned int Entity::GetNumberOfProperties ( ) const
{
	Property		*pProperty	= m_pProperties;
	unsigned int	uiCount		= 0;

	while ( pProperty != NULL )
	{
		pProperty = pProperty->GetNext ( );
		uiCount++;
	}

	return uiCount;
}


unsigned int Entity::GetNumberOfPolys ( ) const
{
	Poly			*pPoly		= m_pPolys;
	unsigned int	uiCount		= 0;

	while ( pPoly != NULL )
	{
		pPoly = pPoly->GetNext ( );
		uiCount++;
	}

	return uiCount;
}