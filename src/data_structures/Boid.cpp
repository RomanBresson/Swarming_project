#include "Boid.h"

#include <random>

Boid::Boid(const Vector3Pos & min_corner, const Vector3Pos & max_corner){
    std::random_device rand_dev;
    std::default_random_engine generator(rand_dev());
    for(int i = 0; i<3; i++){
        std::uniform_real_distribution<float> distrib(min_corner[i], max_corner[i]);
        m_position[i] = distrib(generator);
    }
}

std::string Boid::toString() const {
    std::string s;
    s+= std::string("x: ") + std::to_string(m_position[0]);
    s+= std::string(" --- y: ") + std::to_string(m_position[1]);
    s+= std::string(" --- z: ") + std::to_string(m_position[2]);
    return s;
}

int Boid::get_type() const{
    return 0;
}

Vector3Pos Boid::compute_center_of_mass(const std::vector<Boid> & N){
    Vector3Pos center;
    int same_type = 0;
    for(int i = 0; i<N.size(); i++) {
        if (get_type() == N[i].get_type()){
            same_type++;
            for (int j=0; j<3; j++) {
                center[j] += N[i].m_position[j];
            }
        }
    }
    if (same_type > 0){
        for (int j=0; j<3; j++) {
            center[j] /= same_type;
        }
        return center;
    } else {
        center = {-1,-1,-1};
        return center;
    }
}
