#include "SceneObject.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace OM3D {

SceneObject::SceneObject(std::shared_ptr<StaticMesh> mesh, std::shared_ptr<Material> material) :
    _mesh(std::move(mesh)),
    _material(std::move(material)) {
}

void SceneObject::set_transparent() {
    this->_material->set_blend_mode(BlendMode::Add);
}

void SceneObject::render() const {
    if(!_material || !_mesh) {
        return;
    }
    std::cout << _mesh->sphere_radius() << std::endl;
    if (_mesh->sphere_radius() >= 1) _material->set_blend_mode(BlendMode::Add);
    _material->set_uniform(HASH("model"), transform());

    _material->bind();
    _mesh->draw();
    _material->reset_modes();
}

float SceneObject::sphere_radius() const {
    return _mesh->sphere_radius();
}

void SceneObject::set_transform(const glm::mat4& tr) {
    _transform = tr;
}

const glm::mat4& SceneObject::transform() const {
    return _transform;
}

}
