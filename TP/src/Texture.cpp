#include "Texture.h"

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <cmath>
#include <algorithm>
#include <iostream>

namespace OM3D {

Result<TextureData> TextureData::from_file(const std::string& file) {
    int width = 0;
    int height = 0;
    int channels = 0;
    u8* img = stbi_load(file.c_str(), &width, &height, &channels, 4);
    DEFER(stbi_image_free(img));
    if(!img || width <= 0 || height <= 0 || channels <= 0) {
        return {false, {}};
    }

    const size_t bytes = width * height * 4;

    TextureData data;
    data.size = glm::uvec2(width, height);
    data.format = ImageFormat::RGBA8_UNORM;
    data.data = std::make_unique<u8[]>(bytes);
    std::copy_n(img, bytes, data.data.get());

    return {true, std::move(data)};
}



static GLuint create_texture_handle() {
    GLuint handle = 0;
    glCreateTextures(GL_TEXTURE_2D, 1, &handle);
    return handle;
}

Texture::Texture(const TextureData& data, bool blurrMipMap) :
    _handle(create_texture_handle()),
    _size(data.size),
    _format(data.format) {

    const ImageFormatGL gl_format = image_format_to_gl(_format);
    auto mip = mip_levels(_size);
    glTextureStorage2D(_handle.get(), mip, gl_format.internal_format, _size.x, _size.y);
    glTextureSubImage2D(_handle.get(), 0, 0, 0, _size.x, _size.y, gl_format.format, gl_format.component_type, data.data.get());
    if (blurrMipMap && gl_format.component_type == GL_UNSIGNED_BYTE && gl_format.internal_format == GL_RGBA8) {
        auto test = data.data.get();

        u8 gaussian[] = {
            1, 4, 6, 4, 1,
            4, 16, 24, 16, 4,
            6, 24, 36, 24, 6,
            4, 16, 24, 16, 4,
            1, 4, 6, 4, 1
        };

        u8 channels = 3;
        if (gl_format.format == GL_RGBA) {
            channels = 4;
        }

        u32 size_x = _size.x;
        u32 size_y = _size.y;
        u8* old_data = new u8[size_x * size_y * channels];
        std::memcpy(old_data, data.data.get(), size_x * size_y * channels);
        for (u32 generation_inc = mip - 1; generation_inc; generation_inc--) {
            u32 new_size_x = size_x / 2;
            if (new_size_x <= 0) {
                new_size_x = 1;
            }
            u32 new_size_y = size_y / 2;
            if (new_size_y <= 0) {
                new_size_y = 1;
            }

            u8* new_data = new u8[new_size_x * new_size_y * channels];
            for (u32 j = 0; j < new_size_y; j++) {
                for (u32 i = 0; i < new_size_x; i++) {

                    i32 old_x = std::clamp<i32>(i * 2, 0, size_x - 1);
                    i32 old_y = std::clamp<i32>(j * 2, 0, size_y - 1);

                    u32 r = 0;
                    u32 g = 0;
                    u32 b = 0;
                    u32 a = 0;
                    auto pos = (new_size_x * channels) * j + i * channels;
                    for (i8 l = -2; l <= 2; l++) {
                        for (i8 k = -2; k <= 2; k++) {

                            u8 kernel = gaussian[5 * (l + 2) + (k + 2)];
                            i32 x = std::clamp<i32>(old_x + k, 0, size_x - 1);
                            i32 y = std::clamp<i32>(old_y + l, 0, size_y - 1);

                            auto old_pos = (size_x * channels) * y + x * channels;
                            r += old_data[old_pos + 0] * kernel;
                            g += old_data[old_pos + 1] * kernel;
                            b += old_data[old_pos + 2] * kernel;
                            if (channels == 4) {
                                a += old_data[old_pos + 3] * kernel;
                            }
                        }
                    }
                    r /= 256;
                    g /= 256;
                    b /= 256;
                    a /= 256;

                    new_data[pos + 0] = std::clamp<u32>(r, 0, 255);
                    new_data[pos + 1] = std::clamp<u32>(g, 0, 255);
                    new_data[pos + 2] = std::clamp<u32>(b, 0, 255);
                    if (channels == 4) {
                        new_data[pos + 3] = std::clamp<u32>(a, 0, 255);
                    }
                }
            }
            delete[] old_data;
            old_data = new_data;
            size_x = new_size_x;
            size_y = new_size_y;
            std::cout << "Generating mipmap" << std::endl;
            glTextureSubImage2D(_handle.get(), mip - generation_inc, 0, 0, new_size_x, new_size_y, gl_format.format, gl_format.component_type, new_data);
            std::cout << "Generated blurred mipmap " << (mip - generation_inc) << std::endl;
        }


        delete[] old_data;
    }
    else {
        glGenerateTextureMipmap(_handle.get());
    }
}

Texture::Texture(const glm::uvec2 &size, ImageFormat format) :
    _handle(create_texture_handle()),
    _size(size),
    _format(format) {

    const ImageFormatGL gl_format = image_format_to_gl(_format);
    glTextureStorage2D(_handle.get(), 1, gl_format.internal_format, _size.x, _size.y);
}

Texture::~Texture() {
    if(auto handle = _handle.get()) {
        glDeleteTextures(1, &handle);
    }
}

void Texture::bind(u32 index) const {
    glBindTextureUnit(index, _handle.get());
}

void Texture::bind_as_image(u32 index, AccessType access) {
    glBindImageTexture(index, _handle.get(), 0, false, 0, access_type_to_gl(access), image_format_to_gl(_format).internal_format);
}

const glm::uvec2& Texture::size() const {
    return _size;
}

// Return number of mip levels needed
u32 Texture::mip_levels(glm::uvec2 size) {
    const float side = float(std::max(size.x, size.y));
    return 1 + u32(std::floor(std::log2(side)));
}

}
