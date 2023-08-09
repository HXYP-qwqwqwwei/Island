//
// Created by HXYP on 2023/4/9.
//
#include "util/Model.h"

Model::Model(const char *path) {
    this->loadModel(path);
}


Model::Model(const std::vector<Mesh> &meshes) {
    this->meshes = meshes;
}


void Model::draw(const Shader &shader, const Buffer& transMats) const {
    transMats.bind();
    int instancedAmount = static_cast<int>(transMats.getSize() / sizeof(glm::mat4));
    for (const Mesh& mesh : this->meshes) {
        mesh.bind();

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, SZ_MAT4F, (void*)nullptr);

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, SZ_MAT4F, (void*)(SZ_VEC4F));

        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, SZ_MAT4F, (void*)(2*SZ_VEC4F));

        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, SZ_MAT4F, (void*)(3*SZ_VEC4F));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        mesh.unbind();
    }
    for (const auto& mesh : this->meshes) {
        mesh.drawInstanced(shader, instancedAmount);
    }
    transMats.unbind();
}

void Model::drawLightArea(const Shader &shader, const Buffer& transWithLightInfo) const {
    transWithLightInfo.bind();
    GLint size = SZ_MAT4F + SZ_VEC3F * 3 + SZ_VEC2F;
    int instancedAmount = static_cast<int>(transWithLightInfo.getSize() / size);
    for (const Mesh& mesh : this->meshes) {
        mesh.bind();

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, size, (void*)nullptr);

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, size, (void*)(SZ_VEC4F));

        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, size, (void*)(2*SZ_VEC4F));

        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, size, (void*)(3*SZ_VEC4F));

        glEnableVertexAttribArray(8);
        glVertexAttribPointer(8, 3, GL_FLOAT, GL_FALSE, size, (void*)(SZ_MAT4F));

        glEnableVertexAttribArray(9);
        glVertexAttribPointer(9, 3, GL_FLOAT, GL_FALSE, size, (void*)(SZ_MAT4F + SZ_VEC3F));

        glEnableVertexAttribArray(10);
        glVertexAttribPointer(10, 3, GL_FLOAT, GL_FALSE, size, (void*)(SZ_MAT4F + 2*SZ_VEC3F));

        glEnableVertexAttribArray(11);
        glVertexAttribPointer(11, 2, GL_FLOAT, GL_FALSE, size, (void*)(SZ_MAT4F + 3*SZ_VEC3F));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
        glVertexAttribDivisor(8, 1);
        glVertexAttribDivisor(9, 1);
        glVertexAttribDivisor(10, 1);
        glVertexAttribDivisor(11, 1);

        mesh.unbind();
    }
    transWithLightInfo.unbind();
    for (const auto& mesh : this->meshes) {
        mesh.drawInstanced(shader, instancedAmount);
    }
}


void Model::drawWithColor(const Shader &shader, const Buffer &transWithColor) const {
    transWithColor.bind();
    GLint size = SZ_MAT4F + SZ_VEC3F;
    int instancedAmount = static_cast<int>(transWithColor.getSize() / size);
    for (const Mesh& mesh : this->meshes) {
        mesh.bind();

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, size, (void*)nullptr);

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, size, (void*)(SZ_VEC4F));

        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, size, (void*)(2*SZ_VEC4F));

        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, size, (void*)(3*SZ_VEC4F));

        glEnableVertexAttribArray(9);
        glVertexAttribPointer(9, 3, GL_FLOAT, GL_FALSE, size, (void*)(SZ_MAT4F));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
        glVertexAttribDivisor(9, 1);
        mesh.unbind();
    }
    transWithColor.unbind();
    for (const auto& mesh : this->meshes) {
        mesh.drawInstanced(shader, instancedAmount);
    }

}


void Model::loadModel(const std::string &path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    // empty / incomplete / empty root
    if (scene == nullptr || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << '\n';
        return;
    }
    this->directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);
}

struct ProcessFrame {
    const aiNode* node;
};

void Model::processNode(const aiNode *root, const aiScene *scene) {
    std::stack<ProcessFrame> stk;
    stk.emplace(root);
    while (!stk.empty()) {

        ProcessFrame& frame = stk.top();
        auto node = frame.node;
        stk.pop();
        for (uint i = node->mNumChildren; i > 0; --i) {
            stk.emplace(node->mChildren[i-1]);
        }
        this->meshes.reserve(this->meshes.size() + root->mNumChildren);
        for (uint i = 0; i < node->mNumMeshes; ++i) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            processMesh(mesh, scene);
        }
    }
}

void Model::processMesh(const aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex3D> vertices;
    std::vector<uint> indices;
    std::vector<Texture2D> textures;
    // Vertices
    vertices.reserve(mesh->mNumVertices);
    for (uint i = 0; i < mesh->mNumVertices; ++i) {
        // pos, normal and uv
        auto& pos = mesh->mVertices[i];
        auto& norm = mesh->mNormals[i];
        auto& T = mesh->mTangents[i];
        auto& B = mesh->mBitangents[i];
        auto& uv = mesh->mTextureCoords[0][i];
        float u, v;
        if (mesh->mTextureCoords[0]) {
            u = uv.x;
            v = uv.y;
        } else {
            u = v = 0.0f;
        }
        vertices.push_back({
            {pos.x, pos.y, pos.z},
            {u, v},
            {norm.x, norm.y, norm.z},
            {T.x, T.y, T.z},
        });
    }
    // Indices
    for (uint i = 0; i < mesh->mNumFaces; ++i) {
        const aiFace& face = mesh->mFaces[i];
        for (uint j = 0; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }
    // Textures
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        auto diff = this->loadTextures(material, aiTextureType_DIFFUSE);
        auto spec = this->loadTextures(material, aiTextureType_SPECULAR);
        textures.reserve(diff.size() + spec.size());
        textures.insert(textures.end(), diff.begin(), diff.end());
        textures.insert(textures.end(), spec.begin(), spec.end());
    }
    this->meshes.emplace_back(vertices, indices, textures);
}


std::vector<Texture2D> Model::loadTextures(const aiMaterial *material, aiTextureType type) {
    uint cnt = material->GetTextureCount(type);
    std::vector<Texture2D> textures;
    for (uint i = 0; i < cnt; ++i) {
        aiString path;
        material->GetTexture(type, i, &path);
        Texture2D& loaded = this->loadedTextures[path.C_Str()];
        if (loaded.id != 0) {
            textures.push_back(loaded);
            continue;
        }
        Texture2D tex{load_texture(path.C_Str(), directory), type};
        this->loadedTextures[path.C_Str()] = tex;
        textures.push_back(tex);
    }
    return textures;
}

