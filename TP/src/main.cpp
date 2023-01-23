#include <glad/glad.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include <graphics.h>
#include <SceneView.h>
#include <Texture.h>
#include <Framebuffer.h>
#include <ImGuiRenderer.h>

#include <imgui/imgui.h>


using namespace OM3D;

static float delta_time = 0.0f;
const glm::uvec2 window_size(1600, 900);


void glfw_check(bool cond) {
    if(!cond) {
        const char* err = nullptr;
        glfwGetError(&err);
        std::cerr << "GLFW error: " << err << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

void update_delta_time() {
    static double time = 0.0;
    const double new_time = program_time();
    delta_time = float(new_time - time);
    time = new_time;
}

void process_inputs(GLFWwindow* window, Camera& camera) {
    static glm::dvec2 mouse_pos;

    glm::dvec2 new_mouse_pos;
    glfwGetCursorPos(window, &new_mouse_pos.x, &new_mouse_pos.y);

    {
        glm::vec3 movement = {};
        if(glfwGetKey(window, 'W') == GLFW_PRESS) {
            movement += camera.forward();
        }
        if(glfwGetKey(window, 'S') == GLFW_PRESS) {
            movement -= camera.forward();
        }
        if(glfwGetKey(window, 'D') == GLFW_PRESS) {
            movement += camera.right();
        }
        if(glfwGetKey(window, 'A') == GLFW_PRESS) {
            movement -= camera.right();
        }

        float speed = 10.0f;
        if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            speed *= 10.0f;
        }

        if(movement.length() > 0.0f) {
            const glm::vec3 new_pos = camera.position() + movement * delta_time * speed;
            camera.set_view(glm::lookAt(new_pos, new_pos + camera.forward(), camera.up()));
        }
    }

    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        const glm::vec2 delta = glm::vec2(mouse_pos - new_mouse_pos) * 0.01f;
        if(delta.length() > 0.0f) {
            glm::mat4 rot = glm::rotate(glm::mat4(1.0f), delta.x, glm::vec3(0.0f, 1.0f, 0.0f));
            rot = glm::rotate(rot, delta.y, camera.right());
            camera.set_view(glm::lookAt(camera.position(), camera.position() + (glm::mat3(rot) * camera.forward()), (glm::mat3(rot) * camera.up())));
        }

    }

    mouse_pos = new_mouse_pos;
}


std::unique_ptr<Scene> create_default_scene() {
    auto scene = std::make_unique<Scene>();

    // Load default cube model
    auto result = Scene::from_gltf(std::string(data_path) + "cube.glb");
    ALWAYS_ASSERT(result.is_ok, "Unable to load default scene");
    scene = std::move(result.value);
    // Add lights
    {
        PointLight light;
        light.set_position(glm::vec3(1.0f, 2.0f, 4.0f));
        light.set_color(glm::vec3(0.0f, 10.0f, 0.0f));
        light.set_radius(100.0f);
        scene->add_object(std::move(light));
    }
    {
        PointLight light;
        light.set_position(glm::vec3(1.0f, 2.0f, -4.0f));
        light.set_color(glm::vec3(10.0f, 0.0f, 0.0f));
        light.set_radius(50.0f);
        scene->add_object(std::move(light));
    }

    return scene;
}

std::unique_ptr<Scene> create_forest_scene() {
    auto scene = std::make_unique<Scene>();

    // Load default cube model
    auto result = Scene::from_gltf(std::string(data_path) + "forest.glb");
    ALWAYS_ASSERT(result.is_ok, "Unable to load forest scene");
    scene = std::move(result.value);
    // Add lights
    /*{
        PointLight light;
        light.set_position(glm::vec3(1.0f, 2.0f, 4.0f));
        light.set_color(glm::vec3(0.0f, 10.0f, 0.0f));
        light.set_radius(100.0f);
        scene->add_object(std::move(light));
    }*/
    {
        PointLight light;
        light.set_position(glm::vec3(50.0f, 50.0f, 0.0f));
        light.set_color(glm::vec3(500.0f, 0.0f, 0.0f));
        light.set_radius(10000.0f);
        scene->add_object(std::move(light));
    }

    return scene;
}


int main(int, char**) {
    DEBUG_ASSERT([] { std::cout << "Debug asserts enabled" << std::endl; return true; }());

    glfw_check(glfwInit());
    DEFER(glfwTerminate());



    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(window_size.x, window_size.y, "TP window", nullptr, nullptr);
    glfw_check(window);
    DEFER(glfwDestroyWindow(window));

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    init_graphics();

    ImGuiRenderer imgui(window);

    // Skybox (for IBL)
    auto skybox_data = TextureData::from_file(std::string(data_path) + "10-Shiodome_Stairs_8k.jpg");
    if (!skybox_data.is_ok) {
        std::cerr << "Failed to load skybox!" << std::endl;
        std::exit(1);
    }
    auto skybox_stairs = std::make_shared<Texture>(skybox_data.value, true);

    skybox_data = TextureData::from_file(std::string(data_path) + "Factory_Catwalk_Bg.jpg");
    if (!skybox_data.is_ok) {
        std::cerr << "Failed to load skybox!" << std::endl;
        std::exit(1);
    }
    auto skybox_catwalk = std::make_shared<Texture>(skybox_data.value, true);

    skybox_data = TextureData::from_file(std::string(data_path) + "Ridgecrest_Road_4k_Bg.jpg");
    if (!skybox_data.is_ok) {
        std::cerr << "Failed to load skybox!" << std::endl;
        std::exit(1);
    }
    auto skybox_ridgecrest_road = std::make_shared<Texture>(skybox_data.value, true);

    //std::unique_ptr<Scene> scene = create_default_scene(); // DEFAULT SCENE
    std::unique_ptr<Scene> scene = create_forest_scene(); // FOREST SCENE
    SceneView scene_view(scene.get());
    scene_view.camera().set_view(glm::lookAt(glm::vec3(290.0f, 120.0f, 211.0f), glm::vec3(0.0f, 20.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

    auto tonemap_program = Program::from_file("tonemap.comp");
    Texture color(window_size, ImageFormat::RGBA8_UNORM);
    Framebuffer tonemap_framebuffer(nullptr, std::array{ &color });

    auto albedo = std::make_shared<Texture>(window_size, ImageFormat::RGBA8_UNORM);
    auto normal = std::make_shared<Texture>(window_size, ImageFormat::RGBA8_UNORM);
    auto depth = std::make_shared<Texture>(window_size, ImageFormat::Depth32_FLOAT);

    Texture lit(window_size, ImageFormat::RGBA16_FLOAT);
    Framebuffer main_framebuffer(depth.get(), std::array{&lit});

    Framebuffer g_buffer(depth.get(), std::array{ albedo.get(), normal.get() });

    std::shared_ptr<Program> display_programs[] = {
        Program::from_files("lit.frag", "screen.vert"),
        Program::from_files("lit.frag", "screen.vert", {"NORMAL_DISPLAY"}),
        Program::from_files("lit.frag", "screen.vert", {"ALBEDO_DISPLAY"}),
        Program::from_files("lit.frag", "screen.vert", {"IBL_DISPLAY"})
    };

    Material g_buffer_material;
    g_buffer_material.set_program(display_programs[0]);
    g_buffer_material.set_texture(0, albedo);
    g_buffer_material.set_texture(1, normal);
    g_buffer_material.set_texture(2, depth);
    g_buffer_material.set_texture(6, skybox_stairs);
    g_buffer_material.set_depth_mask(false);
    g_buffer_material.set_blend_mode(BlendMode::Alpha);
    g_buffer_material.set_depth_test_mode(DepthTestMode::None);

    bool normal_display = false;
    bool albedo_display = false;
    bool ibl_display = false;

    for(;;) {
        glfwPollEvents();
        if(glfwWindowShouldClose(window) || glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            break;
        }

        update_delta_time();

        if(const auto& io = ImGui::GetIO(); !io.WantCaptureMouse && !io.WantCaptureKeyboard) {
            process_inputs(window, scene_view.camera());
        }

        // Render the scene
        {
            // main_framebuffer.bind();
            g_buffer.bind();
            scene_view.render();
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        {
            g_buffer_material.bind();
            main_framebuffer.bind();

            auto buffer = scene_view.get_frame_buffer();
            buffer.bind(BufferUsage::Uniform, 3);
            auto light_buffer = scene_view.get_light_buffer();
            light_buffer.bind(BufferUsage::Storage, 4);
            TypedBuffer<shader::Window> window_buffer(nullptr, 1);
            {
                auto mapping = window_buffer.map(AccessType::WriteOnly);
                mapping[0].height = float(window_size.y);
                mapping[0].width = float(window_size.x);
            }
            window_buffer.bind(BufferUsage::Uniform, 5);

            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        // Apply a tonemap in compute shader
        {
            tonemap_program->bind();
            lit.bind(0);
            color.bind_as_image(1, AccessType::WriteOnly);
            glDispatchCompute(align_up_to(window_size.x, 8) / 8, align_up_to(window_size.y, 8) / 8, 1);
        }
        // Blit tonemap result to screen
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        tonemap_framebuffer.blit();

        // Disable Back Face Culling for UI display
        glDisable(GL_CULL_FACE);
        // GUI
        imgui.start();
        {
            char buffer[1024] = {};
            if(ImGui::InputText("Load scene", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
                auto result = Scene::from_gltf(std::string(data_path) + buffer);
                if(!result.is_ok) {
                    std::cerr << "Unable to load scene (" << buffer << ")" << std::endl;
                } else {
                    scene = std::move(result.value);
                    scene_view = SceneView(scene.get());
                }
            }
            ImGui::NewLine();

            ImGui::Text("Display debug mode :");
            bool disabled = (albedo_display || ibl_display);
            if (disabled) {
                ImGui::BeginDisabled();
            }
            if (ImGui::Checkbox("Normals", &normal_display) && !disabled) {
                if (!normal_display) {
                    g_buffer_material.set_program(display_programs[0]);
                }
                else {
                    g_buffer_material.set_program(display_programs[1]);
                }
            }
            if (disabled) {
                ImGui::EndDisabled();
            }

            disabled = (normal_display || ibl_display);
            if (disabled) {
                ImGui::BeginDisabled();
            }
            if (ImGui::Checkbox("Albedo", &albedo_display) && !disabled) {
                if (!albedo_display) {
                    g_buffer_material.set_program(display_programs[0]);
                }
                else {
                    g_buffer_material.set_program(display_programs[2]);
                }
            }
            if (disabled) {
                ImGui::EndDisabled();
            }

            disabled = (normal_display || albedo_display);
            if (disabled) {
                ImGui::BeginDisabled();
            }
            if (ImGui::Checkbox("IBL", &ibl_display) && !disabled) {
                if (!ibl_display) {
                    g_buffer_material.set_program(display_programs[0]);
                }
                else {
                    g_buffer_material.set_program(display_programs[3]);
                }
            }
            if (disabled) {
                ImGui::EndDisabled();
            }

            ImGui::Text("IBL :");
            if (!ibl_display) {
                ImGui::BeginDisabled();
            }
            {
                if (ImGui::Button("Shiodome Stairs")) {
                    std::cout << "pressed" << std::endl;
                    g_buffer_material.set_texture(6, skybox_stairs);
                }
                if (ImGui::Button("Factory Catwalk")) {
                    g_buffer_material.set_texture(6, skybox_catwalk);
                }
                if (ImGui::Button("Ridgecrest Road")) {
                    g_buffer_material.set_texture(6, skybox_ridgecrest_road);
                }
            }
            if (!ibl_display) {
                ImGui::EndDisabled();
            }
        }
        imgui.finish();

        glfwSwapBuffers(window);
    }

    scene = nullptr; // destroy scene and child OpenGL objects
}
