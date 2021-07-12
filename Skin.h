//
//  Skin.h
//
//  Created by Ryan Cajigal on 1/24/19.
//

#ifndef Skin_h
#define Skin_h

#include <glm/gtc/type_ptr.hpp>

#include "Core.h"
#include "Tokenizer.h"
//#include "Triangle.h"
//#include "skeleton.h"
//#include "BMPImage.h"

////////////////////////////////////////////////////////////////////////////////

class skin {
public:
    skin();
    ~skin();

    //void update(Skeleton * Skel);
    void draw(const glm::mat4 &viewProjMtx,uint shader);
    bool load(const char * file);
    void printMembers();

private:
    // Constants & Variables
    uint VAO, VBO, VBO2, EBO;

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    //std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> skinningVertices;
    std::vector<glm::vec3> skinningNormals;
    std::vector<std::vector<glm::vec2>> jointWeights;
    //std::vector<triangle> triangles;
    std::vector<glm::mat4> bindingInverses;
};

#endif /* Skin_h */
