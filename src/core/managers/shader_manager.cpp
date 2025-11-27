#include "shader_manager.h"

#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>

#include <glad/glad.h>

#include "core/graphics/shader_program.h"

namespace core::managers {

namespace {

enum class ShaderType {
	kVertexShader,
	kFragmentShader
};

// Reads the shader file from disk and returns its contents as a vector of chars.
std::vector<char> ReadShader(const std::string& path) {
	std::ifstream file(path, std::ios::ate | std::ios::binary);
	size_t fileLen = static_cast<size_t>(file.tellg());

	std::vector<char> buffer(fileLen, 0);
	file.seekg(0);
	file.read(buffer.data(), fileLen);
	file.close();
  
	return std::move(buffer);
}

// Creates and compiles a shader from the given file path and type.
GLuint CreateShader(const std::string& path, ShaderType type) {
	GLuint shader;
	switch (type) {
	case ShaderType::kVertexShader: {
		shader = glCreateShader(GL_VERTEX_SHADER);
		break;
	}
	case ShaderType::kFragmentShader: {
		shader = glCreateShader(GL_FRAGMENT_SHADER);
		break;
	}
	default:
		break;
	}

	auto shaderBuffer = ReadShader(path);
	std::string shaderSourceString = std::string(shaderBuffer.begin(), shaderBuffer.end());
	const GLchar* shaderSource = shaderSourceString.c_str();
	glShaderSource(shader, 1, &shaderSource, nullptr);
	glCompileShader(shader);

	int successStatus;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &successStatus);
	char infoLog[512];
	if (!successStatus) {
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	return shader;
}
} // namespace

void ShaderManager::ParseShaderFolder(const std::string& folder_path) {
	auto dir_it = std::filesystem::directory_iterator(folder_path);

	for (const auto& entry : dir_it) {
		std::string file_path_str = entry.path().string();
		std::replace(file_path_str.begin(), file_path_str.end(), '\\', '/');
		auto filePath = std::filesystem::path(file_path_str);

		if (entry.is_directory()) {
			ParseShaderFolder(file_path_str);
			continue;
		}

		std::string extension = filePath.extension().string();
		GLuint shader_id;
		if (extension == ".vert") {
			shader_id = CreateShader(file_path_str, ShaderType::kVertexShader);
		} else if (extension == ".frag") {
			shader_id = CreateShader(file_path_str, ShaderType::kFragmentShader);
		} else {
			continue;
		}
		std::cout << "Loaded shader: " << file_path_str << " with ID: " << shader_id << std::endl;
		path_to_shader_id_[file_path_str] = shader_id;
	}
}

void ShaderManager::ParseAndLoadDefaultShader() {
	std::string vertex_shader_path = std::string(DEFAULT_SHADER_VERTEX_PATH);
	std::string fragment_shader_path = std::string(DEFAULT_SHADER_FRAGMENT_PATH);

	GLuint vertex_shader_id = CreateShader(vertex_shader_path, ShaderType::kVertexShader);
	GLuint fragment_shader_id = CreateShader(fragment_shader_path, ShaderType::kFragmentShader);

	ShaderProgramID default_program_id = CreateShaderProgram(vertex_shader_id, fragment_shader_id);
	default_shader_program_ = program_id_to_shader_program_.at(default_program_id);
}

ShaderManager::ShaderManager() {
	ParseShaderFolder(std::string(ABSOLUTE_SHADER_DIR));
	ParseAndLoadDefaultShader();
}

ShaderProgramID ShaderManager::CreateShaderProgram(ShaderID vertex_shader_id,
												   ShaderID fragment_shader_id) {
	GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader_id);
	glAttachShader(shader_program, fragment_shader_id);
	glLinkProgram(shader_program);
	

	int successStatus;
	glGetProgramiv(shader_program, GL_LINK_STATUS, &successStatus);
	if (!successStatus) {
		char infoLog[512];
		glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
		std::cerr << infoLog;
	}
	program_id_to_shader_program_[shader_program] = graphics::ShaderProgram(shader_program);
	return shader_program;
}
} // namespace core::managers
