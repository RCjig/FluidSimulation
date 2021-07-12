//
//  ParticleSystem.h
//
//  Created by Ryan Cajigal on 3/9/19.
//

#ifndef ParticleSystem_h
#define ParticleSystem_h

#include "Core.h"
#include "Particle.h"

class ParticleSystem {
public:
    ParticleSystem();
    ~ParticleSystem();

    void CreateParticleSystem();
    void Update();
    void Draw(const glm::mat4 &view_proj_mtx, uint shader);
    void Reset();
    void ModifyNumParticles(bool increase);
    void ModifySmoothingRadius(bool increase);
    void ModifyBox(bool increase);
    void ModifyHeight(bool increase);
    void ModifyDropHeight(bool increase);
    void ChangeMode();
    void MakeSphere();
    glm::vec3 ParticleHash(glm::vec3 position);

private:
    GLuint VAO;
    GLuint VBO;
    float num_particles;
    float smoothing_radius;
    float support_radius;

    std::vector<Particle *> particles;
    std::unordered_map<glm::vec3, std::list<Particle *>> spatial_map;
    std::vector<glm::vec3> vertices;
};

#endif /* ParticleSystem_h */
