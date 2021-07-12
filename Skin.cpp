//
//  Skin.cpp
//
//  Created by Ryan Cajigal on 1/24/19.
//

#include <stdio.h>
#include "Skin.h"

skin::skin() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &VBO2);
    glGenBuffers(1, &EBO);
}

skin::~skin() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &VBO2);
    glDeleteBuffers(1, &EBO);
}

//void skin::update(Skeleton * Skel) {
//    // Compute skinning matrix for each joint: M=W·B-1 (you can precompute and store B-1 instead of B)
//    // Loop through vertices and compute blended position & normal
//    skinningVertices.clear();
//    skinningNormals.clear();
//
//    for (int i = 0; i < vertices.size(); i++) {
//        glm::vec3 v = glm::vec3(0.0f);
//        glm::vec3 n = glm::vec3(0.0f);
//
//        for (auto jointWeight : jointWeights[i]) {
//            glm::mat4 skinningMatrix = Skel->getWorldMatrix(jointWeight.x) * bindingInverses[jointWeight.x];
//            v += glm::vec3(jointWeight.y * skinningMatrix * glm::vec4(vertices[i], 1.0f));
//            n += glm::vec3(jointWeight.y * skinningMatrix * glm::vec4(normals[i], 0.0f));
//        }
//
//        skinningVertices.push_back(v);
//        skinningNormals.push_back(glm::normalize(n));
//    }
//}

void skin::draw(const glm::mat4 &viewProjMtx, uint shader) {
    // Set GL matrix state to Identity (world)
    // Loop through triangles and draw using world space positions & normals

    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,skinningVertices.size()*sizeof(GLfloat)*3,skinningVertices.data(),GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    glBindBuffer(GL_ARRAY_BUFFER,VBO2);
    glBufferData(GL_ARRAY_BUFFER,skinningNormals.size()*sizeof(GLfloat)*3,skinningNormals.data(),GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER,triangles.size()*sizeof(GLuint)*3,triangles.data(),GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader,"ModelViewProjMtx"),1,false,(float*)&viewProjMtx);

    // Set up state
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);

    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3 * sizeof(GLfloat),0);

    glBindBuffer(GL_ARRAY_BUFFER,VBO2);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,3 *sizeof(GLfloat),0);

    // Bind VAO
    glBindVertexArray(VAO);

    // Draw geometry
    //glDrawElements(GL_TRIANGLES,triangles.size() * 3,GL_UNSIGNED_INT,0);

    // Clean up state
    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    glUseProgram(0);
}

bool skin::load(const char * file) {
    Tokenizer token;
    token.Open(file);

    token.FindToken("positions");
    token.FindToken("{");

    while (token.CheckChar() != '}'){
        vertices.push_back(glm::vec3(token.GetFloat(), token.GetFloat(), token.GetFloat()));
        token.SkipLine();
    };

    token.FindToken("normals");
    token.FindToken("{");

    while(token.CheckChar() != '}') {
        normals.push_back(glm::vec3(token.GetFloat(), token.GetFloat(), token.GetFloat()));
        token.SkipLine();
    }

//    token.FindToken("texcoords");
//    token.FindToken("{");
//
//    while(token.CheckChar() != '}') {
//        texCoords.push_back(glm::vec2(token.GetFloat(), token.GetFloat()));
//        token.SkipLine();
//    }

    token.FindToken("skinweights");
    token.FindToken("{");

    while(token.CheckChar() != '}') {
        float numAttachments = token.GetFloat();
        std::vector<glm::vec2> jointWeightPairs;
        for (int i = 0; i < numAttachments * 2; i += 2) {
            jointWeightPairs.push_back(glm::vec2(token.GetFloat(), token.GetFloat()));
        }
        jointWeights.push_back(jointWeightPairs);
        token.SkipLine();
    }

//    token.FindToken("material");
//    token.FindToken("{");
//    token.FindToken("texture");
//    token.SkipWhitespace();
//
//    char tmp[256];
//    token.GetToken(tmp);
//
//    BMPImage::LoadGLTextures(tmp);

    token.FindToken("triangles");
    token.FindToken("{");

    while(token.CheckChar() != '}') {
        //triangles.push_back(triangle(token.GetInt(), token.GetInt(), token.GetInt()));
        token.SkipLine();
    }

    token.FindToken("bindings");

    float numBindings = token.GetFloat();

    for (int i = 0; i < numBindings; i++) {
        token.FindToken("matrix {");

        float matrix[16];
        int j = 0;

        while (j < 16) {
            matrix[j] = token.GetFloat();
            matrix[j+1] = token.GetFloat();
            matrix[j+2] = token.GetFloat();

            j += 4;
            token.SkipLine();
        }

        matrix[3] = 0;
        matrix[7] = 0;
        matrix[11] = 0;
        matrix[15] = 1;

        bindingInverses.push_back(glm::inverse(glm::make_mat4(matrix)));
    }

    return token.Close();
}

void skin::printMembers() {
    for (auto v : vertices) std::cout << glm::to_string(v) << std::endl;
    for (auto v : normals) std::cout << glm::to_string(v) << std::endl;
    for (auto v : jointWeights) {
        for (auto p : v) std::cout << glm::to_string(p) << std::endl;
        std::cout << std::endl;
    }
    //for (auto t : triangles) std::cout << t.getPoint(0) << ", " << t.getPoint(1) << ", " << t.getPoint(2) << std::endl;
    for (auto b : bindingInverses) std::cout << glm::to_string(glm::inverse(b)) << std::endl;
}
