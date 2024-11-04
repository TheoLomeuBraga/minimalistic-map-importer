
#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include "map.h"


int main() {
    std::cout << "Iniciando conversão para obj...\n";

    MAPFile map;
    Entity* entities = NULL;

    // Carrega o arquivo .map
    if (!map.Load("map.map", &entities)) {
        std::cout << "Erro ao carregar o arquivo map.map\n";
        return 0;
    }

    // Abre o arquivo .obj para escrita
    std::ofstream objFile("obj.obj");
    if (!objFile.is_open()) {
        std::cerr << "Não foi possível abrir o arquivo obj.obj para escrita.\n";
        return 0;
    }

    int vertexIndex = 1; // Para rastrear o índice do vértice para as faces

    // Itera pelas entidades
    Entity* e = entities;
    while (e != NULL) {

        // Itera pelos polígonos da entidade
        Poly* po = e->GetPolys();
        while (po != NULL) {
            // Escreve a textura como comentário no arquivo OBJ
            objFile << "# Texture: " << po->TextureID << "\n";

            // Converte o polígono em uma lista de triângulos
            for (const Triangle& t : po->convert_to_triangles()) {
                
                // Para cada vértice do triângulo
                for (int i = 0; i < 3; ++i) {
                    const Vertex& v = t.vertex[i];

                    // Escreve a posição do vértice
                    objFile << "v " << v.p.x << " " << v.p.y << " " << v.p.z << "\n";

                    // Escreve as coordenadas de textura do vértice
                    objFile << "vt " << v.tex[0] << " " << v.tex[1] << "\n";
                }

                // Escreve a face usando os índices dos três vértices do triângulo
                objFile << "f " 
                        << vertexIndex << "/" << vertexIndex << " "
                        << (vertexIndex + 1) << "/" << (vertexIndex + 1) << " "
                        << (vertexIndex + 2) << "/" << (vertexIndex + 2) << "\n";

                // Atualiza o índice do próximo vértice
                vertexIndex += 3;
            }

            po = po->GetNext();
        }

        e = e->GetNext();
    }

    // Fecha o arquivo OBJ
    objFile.close();

    std::cout << "Conversão para obj concluída.\n";
    return 0;
}
