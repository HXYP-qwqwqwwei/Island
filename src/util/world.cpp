//
// Created by HXYP on 2023/7/19.
//

#include "util/world.h"

static std::unordered_map<RenderType, std::vector<const ModelInfo*>> Models;
static std::vector<PointLight> pointLights;
static DirectionalLight directLight;


void InitWorld() {

}

void putModelInfo(RenderType type, const ModelInfo* modelInfo) {
    Models[type].push_back(modelInfo);
}

void ModelInfo::addInstance(glm::mat4 transMtx) {
    this->transMatrices.push_back(transMtx);
}

void ModelInfo::addInstance(std::initializer_list<glm::mat4> transMtx) {
    this->transMatrices.reserve(this->transMatrices.size() + transMtx.size());
    this->transMatrices.insert(this->transMatrices.end(), transMtx.begin(), transMtx.end());
}
