

#include <iostream>

#include "map.h"


#include <vector>
#include <fstream>
#include <cmath>



int main()
{
    std::cout << "Hello World!\n";
    
    MAPFile map;
    
    Entity *entitys = NULL;

    if(!map.Load("map.map",&entitys)){
        std::cout << "error!\n";
        return 0;
    }

    
    Entity *e = entitys;
    while(e != NULL){

        std::cout << "{\n";


        //Property
        Property *p = e->GetProperties();

        while(p != NULL){
            std::cout << "  " << p->GetName() << " " << p->GetValue() << std::endl;
            
            
            p = p->GetNext();
        }

        //poly
        Poly *po = e->GetPolys();

        while(po != NULL){
            
            std::cout << "{ texture: " << po->TextureID << "} ";

            for(unsigned int i = 0; i < po->GetNumberOfVertices() ; i++){
                Vertex v = po->verts[i];
                std::cout << "{ position: " << v.p.x << " , " << v.p.y << " , " << v.p.z << " , " << "} ";
                std::cout << "{ uv: " << v.tex[0] << " , " << v.tex[1] << "} ";
            }
            
            po = po->GetNext();
        }

        e = e->GetNext();

        std::cout << "}\n";
    }

    std::cout << "Baye World!\n";
    
    return 0;
}