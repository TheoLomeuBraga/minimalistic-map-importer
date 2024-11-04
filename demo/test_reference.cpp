

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
    Texture *textures = NULL;

    if(!map.Load("map.map",&entitys,&textures)){
        std::cout << "error!\n";
        return 0;
    }

    
    Entity *e = entitys;
    while(e != NULL){

        std::cout << "{\n";

        //Property

        Property *properties = e->GetProperties();
        Property *p = properties;

        while(p != NULL){
            std::cout << "  " << p->GetName() << " " << p->GetValue() << std::endl;
            p = p->GetNext();
        }

        //

        e = e->GetNext();

        std::cout << "}\n";
    }

    

    std::cout << "Baye World!\n";
    
    return 0;
}