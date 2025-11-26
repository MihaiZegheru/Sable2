#ifndef CORE_GRAPHICS_SHADER_PROGRAM_H
#define CORE_GRAPHICS_SHADER_PROGRAM_H

#include <string_view>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace core::graphics {

struct ShaderProgram {
	GLuint id;

	ShaderProgram() = default;
	explicit ShaderProgram(GLuint shader_program_id) : id(shader_program_id) {}

	inline void SetMat4(const std::string_view name, const GLfloat* value) const {
		GLint location = glGetUniformLocation(id, name.data());
		if (location >= 0) glUniformMatrix4fv(location, 1, GL_FALSE, value);
	}

	inline void SetInt(const std::string_view name, GLint value) const {
		GLint location = glGetUniformLocation(id, name.data());
		if (location >= 0) glUniform1i(location, value);
	}

	inline void SetFloat(const std::string_view name, GLfloat value) const {
		GLint location = glGetUniformLocation(id, name.data());
		if (location >= 0) glUniform1f(location, value);
	}

	inline void SetVec3(const std::string_view name, const GLfloat* value) const {
		GLint location = glGetUniformLocation(id, name.data());
		if (location >= 0) glUniform3fv(location, 1, value);
	}

	inline void Use() const {
		glUseProgram(id);
	}
};
} // namespace core::graphics

#endif // CORE_GRAPHICS_SHADER_PROGRAM_H
