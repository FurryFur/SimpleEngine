#pragma once

#include <glad\glad.h>

#include <unordered_map>
#include <string>

class Shader
{
public:
	Shader(GLuint gpuProgramHandle);
	~Shader();

	// Make this shader the current shader for rendering
	void use() const;
	
	// Returns a handle to the GPU shader object
	GLuint getGPUHandle() const;

	// Returns a handle to the specified uniform object on the GPU
	GLint getUniformLocation(const std::string& uniformName) const;

	// Returns a handle to the specified uniform block on the GPU
	GLuint getUniformBlockIndex(const std::string& uniformBlockName) const;

private:
	GLuint m_gpuHandle;
	mutable std::unordered_map<std::string, GLint> m_uniformLocationCache;
	mutable std::unordered_map<std::string, GLuint> m_uniformBlockIndexCache;
};

