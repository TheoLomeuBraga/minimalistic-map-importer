
#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#define MAP_TEXTURE_RESOLUTION_SIZE 32
#include "mmapi/map.h"

int main()
{

    MAPFile map;
    Entity *entities = NULL;

    if (!map.Load("map.map", &entities))
    {
        return 0;
    }

    std::ofstream objFile("obj.obj");
    if (!objFile.is_open())
    {
        return 0;
    }

    int vertexIndex = 1; 

    int eid = 0;
    std::string last_texture;

    Entity *e = entities;

    objFile << "o " << eid << "\n";
    eid++;

    while (e != NULL)
    {

        e->CalculateEntityCenter();
        Poly *po = e->GetPolys();
        while (po != NULL)
        {
            objFile << "# Texture: " << po->TextureID << "\n";

            if (last_texture != po->TextureID)
            {
                objFile << "o " << eid << "\n";
                eid++;
                last_texture = po->TextureID;
            }

            for (const Triangle &t : po->convert_to_triangles())
            {

                for (int i = 0; i < 3; ++i)
                {
                    const Vertex &v = t.vertex[i];

                    objFile << "v " << v.p.x << " " << v.p.y << " " << v.p.z << "\n";

                    objFile << "vt " << v.tex[0] << " " << v.tex[1] << "\n";
                }

                objFile << "f "
                        << vertexIndex << "/" << vertexIndex << " "
                        << (vertexIndex + 1) << "/" << (vertexIndex + 1) << " "
                        << (vertexIndex + 2) << "/" << (vertexIndex + 2) << "\n";

                vertexIndex += 3;
            }

            po = po->GetNext();
        }

        e = e->GetNext();
    }

    objFile.close();
    return 0;
}
