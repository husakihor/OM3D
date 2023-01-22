#ifndef SCENEVIEW_H
#define SCENEVIEW_H

#include <Scene.h>
#include <Camera.h>
#include <TypedBuffer.h>
#include <shader_structs.h>

namespace OM3D {

class SceneView {
    public:
        SceneView(const Scene* scene = nullptr);

        Camera& camera();
        const Camera& camera() const;

        void render() const;
        TypedBuffer<shader::FrameData> get_frame_buffer() const;
        TypedBuffer<shader::PointLight> get_light_buffer() const;

    private:
        const Scene* _scene = nullptr;
        Camera _camera;

};

}

#endif // SCENEVIEW_H
