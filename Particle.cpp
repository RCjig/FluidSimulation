//
//  Particle.cpp
//
//  Created by Ryan Cajigal on 2/23/19.
//

#include <stdio.h>
#include "Particle.h"

float NUM_DIMENSIONS = 3.0f;
float REST_DENSITY = 1000.0f;
float STIFFNESS_K = 1.0f;
float KINEMATIC_VISCOSITY = 0.001f;
glm::vec3 GRAVITY = glm::vec3(0.0f, -9.8f, 0.0f);

Particle::Particle(float mass, glm::vec3 position, float smoothing_radius, float height, float square) {
    this->mass = mass;
    this->position = position;
    this->smoothing_radius = smoothing_radius;
    this->height = height;
    this->square = square;
    local_density_star = REST_DENSITY;
    velocity = glm::vec3(0.0f);
    force = glm::vec3(0.0f);
    pressure = 0.0f;
    //local_density = 0.0f;
}

Particle::~Particle() {

}

void Particle::Update(float delta_time) {
    velocity = velocity_star + (delta_time * force_pressure / mass);
    //velocity += delta_time * force / mass;
    position += delta_time * velocity;

    if (position.y < -1.0f * 3.0f) {
         position.y = (2.0f * -1.0f * 3.0f) - position.y;
         velocity.y = -1.0f * 0.75f * velocity.y;
    }

    if (position.y > height) {
        position.y = (2.0f * height) - position.y;
        velocity.y = -1.0f * 0.05f * velocity.y;
    }

    if (position.x < -1.0f * square) {
        position.x = (2.0f * -1.0f * square) - position.x;
        velocity.x = -1.0f * 0.05f/*0.2f*/ * velocity.x;
    }

    if (position.x > square) {
        position.x = (2.0f * square) - position.x;
        velocity.x = -1.0f * 0.05f/*1.0f*/ * velocity.x;
    }

    if (position.z < -1.0f * square) {
        position.z = (2.0f * -1.0f * square) - position.z;
        velocity.z = -1.0f * 0.05f/*1.0f*/ * velocity.z;
    }

    if (position.z > square) {
        position.z = (2.0f * square) - position.z;
        velocity.z = -1.0f * 0.05f/*1.0f*/ * velocity.z;
    }

    force = glm::vec3(0.0f);
}

void Particle::Draw(const glm::mat4 &view_proj_mtx, uint shader) {

}

void Particle::ApplyForce(glm::vec3 force) {
    this->force += force;
}

void Particle::ComputeForce() {
    glm::vec3 f_pressure, f_viscosity, f_other;

    f_pressure = glm::vec3(0.0f);
    for (auto n : neighbors) {
        //if (local_density == 0.0f) break;
        //if (n->GetLocalDensity() == 0.0f) continue;
        float Wij = KernelFunc(n);
        glm::vec3 delWij = glm::vec3(Wij / position.x, Wij / position.y, Wij / position.z);
        float Api = pressure / (local_density * local_density);
        float Apj = n->GetPressure() / (n->GetLocalDensity() * n->GetLocalDensity());
        glm::vec3 test_pressure = n->GetMass() * (Api + Apj) * delWij;
        //std::cout << n->GetMass() << ", " << Api << ", " << Apj <<  ", " << glm::to_string(delWij) << std::endl;
        //while (glm::length(test_pressure) > 100.0f) test_pressure /= 10.0f;
        //if (glm::length(test_pressure) > 100.0f)  test_pressure /= 100.0f;//std::cout << glm::to_string(position) << ", " << glm::to_string(n->GetPosition()) << std::endl;
        f_pressure += test_pressure;
        //std::cout << glm::to_string(test_pressure) << ", " << glm::length(test_pressure) << std::endl;
    }
    f_pressure *= (-1.0f * mass);

    //if (glm::any(glm::isnan(f_pressure)) || glm::any(glm::isinf(f_pressure))) f_pressure = glm::vec3(0.0f);

    f_viscosity = glm::vec3(0.0f);
    for (auto n : neighbors) {
        float Wij = KernelFunc(n);
        glm::vec3 delWij = glm::vec3(Wij / position.x, Wij / position.y, Wij / position.z);
        float mjpj = n->GetMass() / n->GetLocalDensity();
        glm::vec3 Aij = velocity - n->GetVelocity();
        glm::vec3 xij = position - n->GetPosition();
        glm::vec3 test_viscosity = mjpj * Aij * (glm::dot(xij, delWij) / ((glm::dot(xij, xij)) + (0.01f * smoothing_radius * smoothing_radius)));
        f_viscosity += test_viscosity;
    }
    f_viscosity *= KINEMATIC_VISCOSITY * mass * 2.0f;

    f_other = GRAVITY * mass;

    //std::cout << "hi " << glm::to_string(f_pressure) << ", " << glm::to_string(f_viscosity) << ", " << glm::to_string(f_other) << std::endl;
    //std::cout << "hi" << std::endl;

    ApplyForce(f_pressure + f_viscosity + f_other);
}

float Particle::KernelFunc(Particle * j) {
    float q = glm::length(position - j->GetPosition()) / smoothing_radius;
    float fq = 3.0f / (2.0f * M_PI);
    float hd = pow(smoothing_radius, NUM_DIMENSIONS);

    if (q < 1.0f) fq *= (2.0f / 3.0f) - pow(q, 2.0f) + (0.5f * pow(q, 3.0f));
    else if (q <= 2.0f) fq *= (1.0f / 6.0f) * (pow(2.0f - q, 3.0f));
    else fq = 0.0f;

    return (1.0f / hd) * fq;
}

void Particle::ComputePressureAndDensity() {
    local_density = REST_DENSITY;
    for (auto n : neighbors) {
        local_density += n->GetMass() * KernelFunc(n);
        //std::cout << n->GetMass() << " * " << KernelFunc(n) << " = " << (n->GetMass() * KernelFunc(n)) << std::endl;
    }
    pressure = STIFFNESS_K * (pow(local_density / REST_DENSITY, 7.0f) - 1.0f);

    //std::cout << pressure << ", " << local_density << ", " << neighbors.size() << std::endl;
}

void Particle::ComputeVStar(float delta_time) {
    glm::vec3 f_viscosity, f_other;

    f_viscosity = glm::vec3(0.0f);
    for (auto n : neighbors) {
        if (n->GetLocalDensityStar() == 0.0f) continue;
        float mjpj = n->GetMass() / n->GetLocalDensityStar();
        float Wij = KernelFunc(n);
        glm::vec3 delWij = glm::vec3(Wij / position.x, Wij / position.y, Wij / position.z);
        glm::vec3 Aij = velocity - n->GetVelocity();
        glm::vec3 xij = position - n->GetPosition();
        glm::vec3 test_viscosity = mjpj * Aij * (glm::dot(xij, delWij) / ((glm::dot(xij, xij)) + (0.01f * smoothing_radius * smoothing_radius)));
        f_viscosity += test_viscosity;
    }
    f_viscosity *= KINEMATIC_VISCOSITY * mass * 2.0f;
    f_other = mass * GRAVITY;

    velocity_star = velocity + (delta_time * ((f_viscosity + f_other) / mass));
}

void Particle::ComputePressureAndDensityStar(float delta_time) {
    local_density_star = 0.0f;
    for (auto n : neighbors) {
        float Wij = KernelFunc(n);
        glm::vec3 delWij = glm::vec3(Wij / position.x, Wij / position.y, Wij / position.z);

        local_density_star += n->GetMass() * Wij;
        local_density_star += delta_time * glm::dot((velocity_star - n->GetVelocityStar()), delWij);
    }

    if (local_density_star < 1.0f) local_density_star = 0.0f;
    pressure = STIFFNESS_K * (pow(local_density_star / REST_DENSITY, 7.0f) - 1.0f);

//    if (local_density_star < 1.0f) local_density_star = 0.0f;
//    pressure = STIFFNESS_K * ((local_density / REST_DENSITY) - 1.0f);

//    if (local_density_star < 1.0f) local_density_star = REST_DENSITY;
//    pressure = STIFFNESS_K * (local_density - REST_DENSITY);
}

void Particle::ComputePressureForce() {
    force_pressure = glm::vec3(0.0f);
    if (local_density_star == 0.0f) return;
    for (auto n : neighbors) {
        if (n->GetLocalDensityStar() == 0.0f) continue;
        float Wij = KernelFunc(n);
        glm::vec3 delWij = glm::vec3(Wij / position.x, Wij / position.y, Wij / position.z);
        float Api = pressure / (local_density_star * local_density_star);
        float Apj = n->GetPressure() / (n->GetLocalDensityStar() * n->GetLocalDensityStar());
        glm::vec3 test_pressure = n->GetMass() * (Api + Apj) * delWij;
        if (glm::length(test_pressure) > 5000.0f) {
            //std::cout << ((local_density / REST_DENSITY) - 1.0f) << ", " << local_density_star << ", " << n->GetLocalDensityStar() << ", " << (local_density_star * local_density_star) << ", " << (n->GetLocalDensityStar() * n->GetLocalDensityStar()) << std::endl;
        }
        //std::cout << glm::length(test_pressure) << ", " << glm::to_string(test_pressure) << std::endl;
        //std::cout << pressure << ", " << n->GetPressure() << std::endl;
        force_pressure += test_pressure;
    }
    force_pressure *= -1.0f * mass;

    //std::cout << glm::to_string(force_pressure) << std::endl;
}

void Particle::ClearNeighbors() {
    neighbors.clear();
}

void Particle::AddNeighbor(Particle * neighbor) {
    neighbors.push_back(neighbor);
}

std::vector<Particle *> Particle::GetNeighbors() {
    return neighbors;
}

void Particle::SetPosition(glm::vec3 position) {
    this->position = position;
}

glm::vec3 Particle::GetPosition() {
    return position;
}

void Particle::SetVelocity(glm::vec3 velocity) {
    this->velocity = velocity;
}

glm::vec3 Particle::GetVelocity() {
    return velocity;
}

void Particle::SetLocalDensity(float local_density) {
    this->local_density = local_density;
}

float Particle::GetLocalDensity() {
    return local_density;
}

void Particle::SetPressure(float pressure) {
    this->pressure = pressure;
}

float Particle::GetPressure() {
    return pressure;
}

void Particle::SetMass(float mass) {
    this->mass = mass;
}

float Particle::GetMass() {
    return mass;
}

void Particle::SetVelocityStar(glm::vec3 velocity_star) {
    this->velocity_star = velocity_star;
}

glm::vec3 Particle::GetVelocityStar() {
    return velocity_star;
}

void Particle::SetLocalDensityStar(float local_density_star) {
    this->local_density_star = local_density_star;
}

float Particle::GetLocalDensityStar() {
    return local_density_star;
}
