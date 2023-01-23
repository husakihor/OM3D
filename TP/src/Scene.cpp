#include "Scene.h"

#include <iostream>

namespace OM3D {

Scene::Scene() {
}

TypedBuffer<shader::FrameData> Scene::get_frame_buffer(const Camera& camera) const {
    TypedBuffer<shader::FrameData> buffer(nullptr, 1);
    {
        auto mapping = buffer.map(AccessType::WriteOnly);
        mapping[0].camera.view_proj = camera.view_proj_matrix();
        mapping[0].camera.position = camera.position();
        mapping[0].point_light_count = u32(_point_lights.size());
        mapping[0].sun_color = glm::vec3(1.0f, 1.0f, 1.0f);
        mapping[0].sun_dir = glm::normalize(_sun_direction);
    }
    return buffer;
}

TypedBuffer<shader::PointLight> Scene::get_light_buffer() const {
    TypedBuffer<shader::PointLight> buffer(nullptr, std::max(_point_lights.size(), size_t(1)));
    {
        auto mapping = buffer.map(AccessType::WriteOnly);
        for (size_t i = 0; i != _point_lights.size(); ++i) {
            const auto& light = _point_lights[i];
            mapping[i] = {
                light.position(),
                light.radius(),
                light.color(),
                0.0f
            };
        }
    }
    return buffer;
}

void Scene::add_object(SceneObject obj, int instance) {
    if (_objects_instance.find(instance) == _objects_instance.end()) {
        _objects_instance.emplace(instance, std::move(obj));
    }
    auto& vec = _objects_transform[instance];
    vec.emplace_back(obj.transform());
}

void Scene::add_object(PointLight obj) {
    _point_lights.emplace_back(std::move(obj));
}

void Scene::render(const Camera& camera) const {
    // Fill and bind frame data buffer
    auto buffer = this->get_frame_buffer(camera);
    buffer.bind(BufferUsage::Uniform, 0);

    const auto position = camera.position();
    const auto frustum = camera.build_frustum();

    // Render every object
    for (const auto& instance : _objects_instance) {
        int identifier = instance.first;
        const auto& obj = instance.second;

        const auto& transforms = _objects_transform.at(identifier);
        TypedBuffer<glm::mat4> transform_buffer(nullptr, transforms.size());
        int count = 0;
        {
            auto mapping = transform_buffer.map(AccessType::WriteOnly);
            for (const auto& transform : transforms) {
                const glm::vec3 obj_position = transform * glm::vec4(0, 0, 0, 1);
                if (frustum.intersect(obj_position - position, obj.sphere_radius())) {
                    mapping[count++] = transform;
                }
            }
        }
        transform_buffer.bind(BufferUsage::Storage, 2);
        obj.render(count);
    }
}

}
