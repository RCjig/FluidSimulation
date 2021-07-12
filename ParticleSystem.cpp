//
//  ParticleSystem.cpp
//
//  Created by Ryan Cajigal on 3/9/19.
//

#include "ParticleSystem.h"

float PRIME1 = 73856093.0f;
float PRIME2 = 19349663.0f;
float PRIME3 = 83492791.0f;
float BLOB_RADIUS = 0.75f;
float SMOOTHING_RADIUS_H = 0.1f;
float NUM_PARTICLES_FLOAT = 7.0f;
float NUM_PARTICLES_SPHERE = 500.0f;
float HEIGHT = 3.0f;
float SQUARE = 1.5f;
float DROP_HEIGHT = 0.0f;
int MODE = 0;

ParticleSystem::ParticleSystem() {
    this->num_particles = NUM_PARTICLES_FLOAT;
    this->smoothing_radius = SMOOTHING_RADIUS_H;
    this->support_radius = 2.0f * smoothing_radius;

    CreateParticleSystem();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
}

ParticleSystem::~ParticleSystem() {
    for (auto p : particles) delete p;

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void ParticleSystem::CreateParticleSystem() {
    if (MODE == 1) MakeSphere();
    else {
        for (float i = -1.0f * num_particles * support_radius / 2.0f; i < num_particles * support_radius / 2.0f; i += support_radius) {
            for (float j = -1.0f * num_particles * support_radius / 2.0f; j < num_particles * support_radius / 2.0f; j += support_radius) {
                for (float k = -1.0f * num_particles * support_radius / 2.0f; k < num_particles * support_radius / 2.0f; k += support_radius) {
                    particles.push_back(new Particle(pow(smoothing_radius, 3.0f) * 1000.0f, glm::vec3(i, j + DROP_HEIGHT, k), smoothing_radius, HEIGHT, SQUARE));
                }
            }
        }
//        for (float i = -1.0f * num_particles * support_radius / 2.0f; i < num_particles * support_radius / 2.0f; i += 1.175f * pow(((4.0f * M_PI / 3.0f * pow(smoothing_radius, 3.0f)) / 1.0f), (1.0f / 3.0f))) {
//            for (float j = -1.0f * num_particles * support_radius / 2.0f; j < num_particles * support_radius / 2.0f; j += 1.175f * pow(((4.0f * M_PI / 3.0f * pow(smoothing_radius, 3.0f)) / 1.0f), (1.0f / 3.0f))) {
//                for (float k = -1.0f * num_particles * support_radius / 2.0f; k < num_particles * support_radius / 2.0f; k += 1.175f * pow(((4.0f * M_PI / 3.0f * pow(smoothing_radius, 3.0f)) / 1.0f), (1.0f / 3.0f))) {
//                    particles.push_back(new Particle(/*pow(smoothing_radius, 3.0f) * 1000.0f*/ 4.0f * M_PI / 3.0f * pow(smoothing_radius, 3.0f), glm::vec3(i, j + DROP_HEIGHT, k), smoothing_radius, HEIGHT, SQUARE));
//                }
//            }
//        }
    }
//    else {
//        for (int i = 0; i < num_particles; i++) {
//            float x = (static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/BLOB_RADIUS))) - (BLOB_RADIUS / 2.0f);
//            float y = (static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/BLOB_RADIUS))) - (BLOB_RADIUS / 2.0f);
//            float z = (static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/BLOB_RADIUS))) - (BLOB_RADIUS / 2.0f);
//            particles.push_back(new Particle(pow(smoothing_radius, 3.0f) * 1000.0f, glm::vec3(x, y, z), smoothing_radius, HEIGHT, SQUARE));
//        }
//    }

    for (auto p : particles) {
        spatial_map[ParticleHash(p->GetPosition())].push_front(p);
        vertices.push_back(p->GetPosition());
    }
}

void ParticleSystem::Update() {
    // Calculate time step
    float delta_time = 0.0f;
    float max_velocity = 0.0f;

    for (auto p : particles) if (glm::length(p->GetVelocity()) > max_velocity) max_velocity = glm::length(p->GetVelocity());
    delta_time = max_velocity == 0.0f ? (1.0f / 600.0f) : 0.2f * (smoothing_radius / max_velocity);
    if (delta_time > 1.0f / 120.0f || delta_time < 0.0000001f) delta_time = 1.0f / 120.0f;
    //std::cout << delta_time << std::endl;
    //delta_time = 1.0f / 60.0f; //0.01f;

    // alg2

    // Find nearest neighbors
    for (auto p : particles) {
        p->ClearNeighbors();
        for (float i = 0.0f; i <= support_radius; i += support_radius) {
            for (float j = 0.0f; j <= support_radius; j += support_radius) {
                for (float k = 0.0f; k <= support_radius; k += support_radius) {
                    for (auto n : spatial_map[ParticleHash(p->GetPosition() + glm::vec3(i, j, k))]) if (n != p) p->AddNeighbor(n);
                }
            }
        }
    }

    // Calculate v*
    for (auto p : particles) p->ComputeVStar(delta_time);


    // Calculate pressure and density*
    for (auto p : particles) p->ComputePressureAndDensityStar(delta_time);

    // Calculate fp
    for (auto p : particles) p->ComputePressureForce();

    // Update
    for (auto p : particles) {
        if (!spatial_map[ParticleHash(p->GetPosition())].empty()) spatial_map[ParticleHash(p->GetPosition())].clear();
        p->Update(delta_time);
        spatial_map[ParticleHash(p->GetPosition())].push_front(p);
    }

    // Update vertices
    vertices.clear();
    for (auto p : particles) vertices.push_back(p->GetPosition());
}

void ParticleSystem::Draw(const glm::mat4 &view_proj_mtx, uint shader) {

    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "ModelViewProjMtx"), 1,false, (float*) &view_proj_mtx);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat) * 3, vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Set up state
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    // Draw geometry
    glDrawArrays(GL_POINTS, 0, vertices.size());

    // Clean up state
    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glUseProgram(0);
}

void ParticleSystem::Reset() {
    particles.clear();
    vertices.clear();
    CreateParticleSystem();
}

void ParticleSystem::ModifyNumParticles(bool increase) {
    if (MODE != 1) {
        if (increase) NUM_PARTICLES_FLOAT += 0.1f;
        else if (NUM_PARTICLES_FLOAT > 4.5f) NUM_PARTICLES_FLOAT -= 0.1f;

        this->num_particles = NUM_PARTICLES_FLOAT;

        std::cout << "Next number of particles: " << pow((NUM_PARTICLES_FLOAT), 3.0f) << std::endl;
    }
    else {
        if (increase) NUM_PARTICLES_SPHERE += 25.0f;
        else if (NUM_PARTICLES_SPHERE > 100.0f) NUM_PARTICLES_SPHERE -= 25.0f;

        this->num_particles = NUM_PARTICLES_SPHERE;

        std::cout << "Next number of particles: " << NUM_PARTICLES_SPHERE << std::endl;
    }
}

void ParticleSystem::ModifySmoothingRadius(bool increase) {
    if (increase) SMOOTHING_RADIUS_H += 0.005f;
    else if (SMOOTHING_RADIUS_H > 0.005f) SMOOTHING_RADIUS_H -= 0.005f;

    this->smoothing_radius = SMOOTHING_RADIUS_H;

    std::cout << "Next smoothing radius: " << SMOOTHING_RADIUS_H << std::endl;
}

void ParticleSystem::ModifyBox(bool increase) {
    if (increase) SQUARE += 0.25f;
    else if (SQUARE > 0.25f) SQUARE -= 0.25f;

    std::cout << "Next square size: " << SQUARE << std::endl;
}

void ParticleSystem::ModifyHeight(bool increase) {
    if (increase) HEIGHT += 0.25f;
    else if (HEIGHT > 0.5f) HEIGHT -= 0.25f;

    std::cout << "Next height: " << HEIGHT << std::endl;
}

void ParticleSystem::ModifyDropHeight(bool increase) {
    if (increase) DROP_HEIGHT += 0.25f;
    else if (DROP_HEIGHT > -2.0f) DROP_HEIGHT -= 0.25f;

    std::cout << "Next drop height: " << DROP_HEIGHT << std::endl;
}

void ParticleSystem::ChangeMode() {
    std::string mode_string = "";
    if (MODE == 0) {
        MODE = 1;
        mode_string = "Random Sphere";
        num_particles = NUM_PARTICLES_SPHERE;
    }
    else if (MODE == 1) {
        MODE = 0;
        mode_string = "Uniform Cube";
        num_particles = NUM_PARTICLES_FLOAT;
    }

    std::cout << "Next mode: " << mode_string << std::endl;
}

void ParticleSystem::MakeSphere() {
    float diameter = 1.5f;
    glm::vec3 center(0.0f, DROP_HEIGHT, 0.0f);

    for (float i = 0.0f; i < num_particles; i += 1.0f) {
        float u = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        float v = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        float theta = u * 2.0 * M_PI;
        float phi = acos(2.0 * v - 1.0);
        float r = cbrt(static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);
        float sinPhi = sin(phi);
        float cosPhi = cos(phi);
        float x = r * sinPhi * cosTheta;
        float y = r * sinPhi * sinTheta;
        float z = r * cosPhi;
        particles.push_back(new Particle(pow(smoothing_radius, 3.0f) * 1000.0f, glm::vec3(x, y, z), smoothing_radius, HEIGHT, SQUARE));
    }
}

glm::vec3 ParticleSystem::ParticleHash(glm::vec3 position) {
    float x = position.x;
    float y = position.y;
    float z = position.z;

    return glm::vec3(floor(x / support_radius) * PRIME1, floor(y / support_radius) * PRIME2, floor(z / support_radius) * PRIME3);
}
