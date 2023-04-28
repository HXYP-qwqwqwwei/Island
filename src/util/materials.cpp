//
// Created by HXYP on 2023/3/23.
//

#include "util/materials.h"

void setShaderMaterial(Shader& shader, Material material) {
    shader.uniformVec3(S_AMBIENT,   get<AMBIENT>(material));
    shader.uniformVec3(S_DIFFUSE,   get<DIFFUSE>(material));
    shader.uniformVec3(S_SPECULAR,  get<SPECULAR>(material));
    shader.uniformFloat(S_SHININESS, get<SHININESS>(material));
}

namespace materials {
    Material of(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess) {
        return {ambient, diffuse, specular, shininess};
    }

    Material of(glm::vec3 ambient, float diffuse, float specular, float shininess) {
        return {ambient, glm::vec3(diffuse), glm::vec3(specular), shininess};
    }

    Material of(float ambient, float diffuse, float specular, float shininess) {
        return {glm::vec3(ambient), glm::vec3(diffuse), glm::vec3(specular), shininess};
    }

    Material of(float diffuse, float specular, float shininess) {
        return {glm::vec3(0.1), glm::vec3(diffuse), glm::vec3(specular), shininess};
    }

    Material METAL = of(0.507, 0.508, 0.4);

    Material GOLD = of(
            glm::vec3(0.24725,	0.1995, 	0.0745),    // Ambient
            glm::vec3(0.75164,	0.60648,	0.22648),   // Diffuse
            glm::vec3(0.628281,	0.555802,	0.366065),  // Specular
            0.4                                         // Shininess
    );

    Material EMERALD = of(
            glm::vec3(0.0215,	0.1745, 	0.0215),    // Ambient
            glm::vec3(0.07568,	0.61424,	0.07568),   // Diffuse
            glm::vec3(0.633,	0.727811,	0.633),     // Specular
            0.6                                         // Shininess
    );

    Material PERL = of(
            glm::vec3(0.25, 	0.20725,	0.20725),   // Ambient
            glm::vec3(1,    	0.829,  	0.829),     // Diffuse
            glm::vec3(0.296648,	0.296648,	0.296648),  // Specular
            0.088                                       // Shininess
    );

}



