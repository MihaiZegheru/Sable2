#ifndef CORE_GRAPHICS_TEXTURE_H
#define CORE_GRAPHICS_TEXTURE_H

#include <cstdint>

#include <glad/glad.h>

namespace core::graphics {

struct Texture {
	int width;
	int height;
	int channels;

	// TODO: IS this even used?
	uint8_t* data;

	GLuint gl_texture;
};
} // namespace core::graphics

#endif // CORE_GRAPHICS_TEXTURE_H
