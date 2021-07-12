//
//  joint.cpp
//
//  Created by Ryan Cajigal on 1/13/19.
//

#include "joint.h"

uint Joint::ID = 0;

Joint::Joint() {
    offset = glm::vec3(0.0f);
    boxmin = glm::vec3(-0.1f);
    boxmax = glm::vec3(0.1f);
    pose = glm::vec3(0.0f);

    rotxlimit = glm::vec2(-100000, 100000);
    rotylimit = glm::vec2(-100000, 100000);
    rotzlimit = glm::vec2(-100000, 100000);

    localMatrix = glm::mat4(1.0f);
    worldMatrix = glm::mat4(1.0f);
}

Joint::~Joint() {
    //for (auto d : DOFs) delete d;
    for (auto c : children) delete c;
}

void Joint::update(glm::mat4 parentMatrix) {
    localMatrix = computeLocalMatrix();
    setWorldMatrix(parentMatrix * localMatrix);

    for (auto c : children) c->update(worldMatrix);
}

bool Joint::load(Tokenizer &token) {
    setJointID(Joint::ID++);

    token.FindToken("{");

    while(1) {
        char temp[256];
        token.GetToken(temp);

        if (strcmp(temp,"offset")==0) {
            offset.x=token.GetFloat();
            offset.y=token.GetFloat();
            offset.z=token.GetFloat();
        }

        else if (strcmp(temp,"balljoint") == 0) {
            Joint *jnt=new Joint;
            jnt->load(token);
            addChild(jnt);
        }

        else if (strcmp(temp, "boxmin") == 0) {
            boxmin.x = token.GetFloat();
            boxmin.y = token.GetFloat();
            boxmin.z = token.GetFloat();
        }

        else if (strcmp(temp, "boxmax") == 0) {
            boxmax.x = token.GetFloat();
            boxmax.y = token.GetFloat();
            boxmax.z = token.GetFloat();
        }

        else if (strcmp(temp, "rotxlimit") == 0) {
            rotxlimit.x = token.GetFloat();
            rotxlimit.y = token.GetFloat();
        }

        else if (strcmp(temp, "rotylimit") == 0) {
            rotylimit.x = token.GetFloat();
            rotylimit.y = token.GetFloat();
        }

        else if (strcmp(temp, "rotzlimit") == 0) {
            rotzlimit.x = token.GetFloat();
            rotzlimit.y = token.GetFloat();
        }

        else if (strcmp(temp, "pose") == 0) {
            pose.x = token.GetFloat();
            pose.y = token.GetFloat();
            pose.z = token.GetFloat();
        }

        else if (strcmp(temp,"}") == 0) {
            finishLoad();
            return true;
        }

        else token.SkipLine(); // Unrecognized token
    }

    return false;
}

void Joint::addChild(Joint * child) {
    children.push_back(child);
}

void Joint::draw(const glm::mat4 &viewProjMtx,uint shader) {
    boneModel.Draw(worldMatrix, viewProjMtx, shader);
    for (auto c : children) c->draw(viewProjMtx, shader);
}

void Joint::reset() {
    localMatrix = glm::mat4(1.0f);
    for (auto c : children) c->reset();
}

//void Joint::createDOFs() {
//    DOF * xDOF = new DOF();
//    DOF * yDOF = new DOF();
//    DOF * zDOF = new DOF();
//
//    xDOF->setMinMax(rotxlimit.x, rotxlimit.y);
//    yDOF->setMinMax(rotylimit.x, rotylimit.y);
//    zDOF->setMinMax(rotzlimit.x, rotzlimit.y);
//
//    DOFs[0] = xDOF;
//    DOFs[1] = yDOF;
//    DOFs[2] = zDOF;
//}

void Joint::setPose(glm::vec3 pose) {
    this->pose = pose;

//    DOFs[0]->setValue(pose.x);
//    DOFs[1]->setValue(pose.y);
//    DOFs[2]->setValue(pose.z);
}

void Joint::setDOF(uint DOFIndex, float channelValue) {
//    DOFs[DOFIndex]->setValue(channelValue);
}

void Joint::finishLoad() {
    //createDOFs();
    setPose(pose);
    boneModel.MakeBox(boxmin, boxmax);
}

void Joint::setJointID(uint jointID) {
    this->jointID = jointID;
}

uint Joint::getJointID() {
    return jointID;
}

void Joint::setWorldMatrix(glm::mat4 worldMatrix) {
    this->worldMatrix = worldMatrix;
}

void Joint::setOffset(glm::vec3 offset) {
    this->offset = offset;
}

glm::mat4 Joint::getWorldMatrix() {
    return worldMatrix;
}

glm::mat4 Joint::getLocalMatrix() {
    return localMatrix;
}

glm::mat4 Joint::computeLocalMatrix() {
    glm::mat4 Lx, Ly, Lz, newLocalMatrix;

//    Lx = glm::rotate(glm::mat4(1.0f), DOFs[0]->getValue(), glm::vec3(1.0f, 0.0f, 0.0f));
//    Ly = glm::rotate(glm::mat4(1.0f), DOFs[1]->getValue(), glm::vec3(0.0f, 1.0f, 0.0f));
//    Lz = glm::rotate(glm::mat4(1.0f), DOFs[2]->getValue(), glm::vec3(0.0f, 0.0f, 1.0f));
//
    newLocalMatrix = Lz * Ly * Lx;
    newLocalMatrix[3] = glm::vec4(offset, 1.0f);

    return newLocalMatrix;
}

Joint * Joint::getIndexedJoint(uint jointID) {
    if (getJointID() == jointID) return this;

    for (auto c : children) {
        Joint * cc = c->getIndexedJoint(jointID);
        if (cc) return cc;
    }

    return NULL;
}

//void Joint::addDOFs(std::vector<DOF *> &DOFList) {
//    DOFList.push_back(DOFs[0]);
//    DOFList.push_back(DOFs[1]);
//    DOFList.push_back(DOFs[2]);
//
//    for (auto c : children) c->addDOFs(DOFList);
//}
