//
// Created by HXYP on 2023/4/9.
//

#ifndef ISLAND_EXTERNAL_MODEL_H
#define ISLAND_EXTERNAL_MODEL_H
#include <iostream>
#include <stack>
#include <vector>
#include <initializer_list>
#include <unordered_map>
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "shaders.h"
#include "Mesh.h"
#include "texture_util.h"
#include "buffer_util.h"

class Model {
public:
    explicit Model(const char* path);
    explicit Model(const std::vector<Mesh>& meshes);
    void draw(const Shader& shader, const Buffer& transMats) const;
    void drawLightArea(const Shader& shader, const Buffer& transWithLightInfo) const;
    void drawWithColor(const Shader& shader, const Buffer& transWithColor) const;

private:
    std::vector<Mesh> meshes;
    std::unordered_map<std::string, Texture2D> loadedTextures{};
    std::string directory;

    void loadModel(const std::string& path);
    void processNode(const aiNode* root, const aiScene* scene);
    void processMesh(const aiMesh* mesh, const aiScene* scene);
    std::vector<Texture2D> loadTextures(const aiMaterial *material, aiTextureType type);

};


#endif //ISLAND_EXTERNAL_MODEL_H
