//
//  Particle.h
//
//  Created by Ryan Cajigal on 2/23/19.
//

#ifndef Particle_h
#define Particle_h

#include "Core.h"

class Particle {
public:
    Particle(float mass, glm::vec3 position, float smoothing_radius, float height, float square);
    ~Particle();

    void Update(float delta_time);
    void Draw(const glm::mat4 &view_proj_mtx, uint shader);
    void ApplyForce(glm::vec3 force);
    void ComputeForce();
    float KernelFunc(Particle * j);
    void ComputePressureAndDensity();
    void ComputeVStar(float delta_time);
    void ComputePressureAndDensityStar(float delta_time);
    void ComputePressureForce();

    void ClearNeighbors();
    void AddNeighbor(Particle * neighbor);
    std::vector<Particle *> GetNeighbors();
    void SetPosition(glm::vec3 position);
    glm::vec3 GetPosition();
    void SetVelocity(glm::vec3 velocity);
    glm::vec3 GetVelocity();
    void SetLocalDensity(float local_density);
    float GetLocalDensity();
    void SetPressure(float pressure);
    float GetPressure();
    void SetMass(float mass);
    float GetMass();
    void SetVelocityStar(glm::vec3 velocity_star);
    glm::vec3 GetVelocityStar();
    void SetLocalDensityStar(float local_density_star);
    float GetLocalDensityStar();

private:
    float mass; // Constant
    float local_density;
    float pressure;
    float smoothing_radius;
    float local_density_star;
    float height;
    float square;

    glm::vec3 position; // Evolves frame to frame
    glm::vec3 velocity; // Evolves frame to frame
    glm::vec3 force; // Reset and re-computed each frame
    glm::vec3 velocity_star;
    glm::vec3 force_pressure;

    std::vector<Particle *> neighbors;
};

#endif /* Particle_h */
