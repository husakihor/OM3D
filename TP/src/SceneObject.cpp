#include "SceneObject.h"

#include <glm/gtc/matrix_transform.hpp>

namespace OM3D {

SceneObject::SceneObject(std::shared_ptr<StaticMesh> mesh, std::shared_ptr<Material> material) :
    _mesh(std::move(mesh)),
    _material(std::move(material)) {
}

void SceneObject::render(int count) const {
    if (!_material || !_mesh) {
        return;
    }

   _material->bind();
   _mesh->draw(count);
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
