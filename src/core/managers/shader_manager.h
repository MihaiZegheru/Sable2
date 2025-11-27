#ifndef CORE_MANAGERS_SHADER_MANAGER_H
#define CORE_MANAGERS_SHADER_MANAGER_H

#include <string>
#include <unordered_map>
#include <optional>
#include <iostream>

#include <glad/glad.h>

#include "core/graphics/shader_program.h"

namespace core::managers {

using ShaderID = GLuint;
using ShaderProgramID = GLuint;

class ShaderManager {
public:
	static ShaderManager& GetInstance() {
		static ShaderManager instance;
		return instance;
	}

	// TODO: Change in GetOrCreateShaderProgram for caching

	// Creates a shader program from the given vertex and fragment shader IDs.
	ShaderProgramID CreateShaderProgram(ShaderID vertex_shader_id,
										ShaderID fragment_shader_id);

	inline graphics::ShaderProgram GetDefaultShaderProgram() const {
		return default_shader_program_;
	}

	inline std::optional<ShaderID> GetShaderFromPath(const std::string& path) const {
		const std::string absolute_file_path = std::string(ABSOLUTE_SHADER_DIR) + "/" + path;
		auto it = path_to_shader_id_.find(absolute_file_path);
		if (it != path_to_shader_id_.end()) {
			return it->second;
		}
		return std::nullopt;
	}

private:
	ShaderManager();
	ShaderManager(const ShaderManager&) = delete;
	ShaderManager& operator=(const ShaderManager&) = delete;

	// Recursively loads all shaders from the shaders folder.
	void ParseShaderFolder(const std::string& folder_path);

	// Loads default shaders.
	void ParseAndLoadDefaultShader();

private:
    // Used for user interaction with shaders.
	std::unordered_map<std::string, ShaderID> path_to_shader_id_;
	// Used for engine internal management of shader programs.
	std::unordered_map<ShaderProgramID, graphics::ShaderProgram> program_id_to_shader_program_;

	// Default shader program.
	graphics::ShaderProgram default_shader_program_;

	// Used for assigning unique IDs to shaders.
	ShaderID next_shader_id_ = 1;
	// Used for assigning unique IDs to shader programs.
	ShaderProgramID next_program_id_ = 1;
};
} // namespace core::managers

#endif // CORE_MANAGERS_SHADER_MANAGER_H
