//
//  joint.h
//
//  Created by Ryan Cajigal on 1/13/19.
//

#ifndef joint_h
#define joint_h

#include "Core.h"
//#include "DOF.h"
#include "Tokenizer.h"
#include "Model.h"
#include <vector>

////////////////////////////////////////////////////////////////////////////////
class Joint {
public:
    Joint();
    ~Joint();

    void update(glm::mat4 parentMatrix);
    bool load(Tokenizer &t);
    void addChild(Joint * child);
    void draw(const glm::mat4 &viewProjMtx,uint shader);
    void reset();
    void createDOFs();
    void setPose(glm::vec3 pose);
    void setDOF(uint DOFIndex, float channelValue);
    void finishLoad();
    void setJointID(uint jointID);
    uint getJointID();
    void setWorldMatrix(glm::mat4 worldMatrix);
    void setOffset(glm::vec3 offset);
    glm::mat4 getWorldMatrix();
    glm::mat4 getLocalMatrix();
    glm::mat4 computeLocalMatrix();
    Joint * getIndexedJoint(uint jointID);
//    void addDOFs(std::vector<DOF *> &DOFList);

private:
    // Constants & Variables
    Model boneModel;

    glm::mat4 localMatrix;
    glm::mat4 worldMatrix;

    //DOF * DOFs[3];
    std::vector<Joint *> children;
    uint jointID;
    static uint ID;

    glm::vec3 offset;
    glm::vec3 boxmin;
    glm::vec3 boxmax;
    glm::vec3 pose;

    glm::vec2 rotxlimit;
    glm::vec2 rotylimit;
    glm::vec2 rotzlimit;
};

////////////////////////////////////////////////////////////////////////////////


#endif /* joint_h */
