

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

    map.Load("map.map",&entitys,&textures);
    
    return 0;
}