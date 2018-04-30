//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : Utils for shader operations
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#include "ShaderHelper.h"

#include "Log.h"
#include "Shader.h"

#include <fstream>
#include <assert.h>

std::string readShaderFileFromResource(const char* pFileName);
GLuint compileVertexShader(const char* shaderCode);
GLuint compileFragmentShader(const char* shaderCode);
GLuint compileShader(GLenum ShaderType, const char* shaderCode);
GLuint linkProgram(GLuint vertexShaderId, GLuint fragmentShaderId);
GLint validateProgram(GLuint programObjectId);

std::string readShaderFileFromResource(const char* pFileName) {
	std::string outFile;
	try {
		std::ifstream f(pFileName);
		std::string line;
		while (std::getline(f, line)) {
			outFile.append(line);
			outFile.append("\n");
		}
		f.close();
	}
	catch (std::ifstream::failure e) {
		g_log << "Exception opening/reading/closing file\n";
	}
	return outFile;
}

GLuint compileVertexShader(const char* shaderCode) {
	return compileShader(GL_VERTEX_SHADER, shaderCode);
}

GLuint compileFragmentShader(const char* shaderCode) {
	return compileShader(GL_FRAGMENT_SHADER, shaderCode);
}

GLuint compileGeometryShader(const char* shaderCode) {
	return compileShader(GL_GEOMETRY_SHADER, shaderCode);
}

GLuint compileShader(GLenum ShaderType, const char* shaderCode) {
	const  GLuint shaderObjectId = glCreateShader(ShaderType);
	if (shaderObjectId == 0) {
		g_log << "Error creating shader type " << ShaderType << "\n";
		assert(false);
		exit(0);
	}
	const GLchar* p[1];
	p[0] = shaderCode;
	GLint Lengths[1];
	Lengths[0] = static_cast<GLint>(strlen(shaderCode));

	glShaderSource(shaderObjectId, 1, p, Lengths);
	glCompileShader(shaderObjectId);
	GLint compileStatus;
	glGetShaderiv(shaderObjectId, GL_COMPILE_STATUS, &compileStatus);
	if (!compileStatus) {
		GLchar InfoLog[1024];
		glGetShaderInfoLog(shaderObjectId, 1024, NULL, InfoLog);
		g_log << "Error compiling shader type" << ShaderType << "\n" << InfoLog << "\n";
		assert(false);
		exit(1);
	}

	return shaderObjectId;
}

GLuint linkProgram(GLuint vertexShaderId, GLuint fragmentShaderId, bool haveGeometryShader, GLuint geometryShaderId) {
	GLint linkStatus = 0;
	GLchar ErrorLog[1024] = { 0 };
	const GLuint programObjectId = glCreateProgram();

	if (programObjectId == 0) {
		g_log << "Error creating shader program \n";
		assert(false);
		exit(1);
	}

	glAttachShader(programObjectId, vertexShaderId);
	if (haveGeometryShader);
		glAttachShader(programObjectId, geometryShaderId);
	glAttachShader(programObjectId, fragmentShaderId);
	glLinkProgram(programObjectId);

	glGetProgramiv(programObjectId, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == 0) {
		glGetProgramInfoLog(programObjectId, sizeof(ErrorLog), NULL, ErrorLog);
		g_log << "Error linking shader program: \n" << ErrorLog << "\n";
		assert(false);
		exit(1);
	}
	return programObjectId;
}

GLuint linkProgram(GLuint vertexShaderId, GLuint fragmentShaderId) {
	return linkProgram(vertexShaderId, fragmentShaderId, false, 0);
}

GLuint linkProgram(GLuint vertexShaderId, GLuint geometryShaderId, GLuint fragmentShaderId) {
	return linkProgram(vertexShaderId, fragmentShaderId, true, geometryShaderId);
}

GLint validateProgram(GLuint programObjectId) {
	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };
	glValidateProgram(programObjectId);
	glGetProgramiv(programObjectId, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(programObjectId, sizeof(ErrorLog), NULL, ErrorLog);
		g_log << "Invalid shader program: \n" << ErrorLog;
		assert(false);
		exit(1);
	}
	return Success;
}

Shader compileAndLinkShaders(const char* vertexShaderFile, const char* fragmentShaderFile, const char* geometryShaderFile) {
	std::string vertexShaderSource = readShaderFileFromResource(vertexShaderFile);
	std::string fragmentShaderSource = readShaderFileFromResource(fragmentShaderFile);
	GLuint vertexShader = compileVertexShader(vertexShaderSource.c_str());
	GLuint fragmentShader = compileFragmentShader(fragmentShaderSource.c_str());

	GLuint program;
	if (geometryShaderFile) {
		std::string geometryShaderSource = readShaderFileFromResource(geometryShaderFile);
		GLuint geometryShader = compileGeometryShader(geometryShaderSource.c_str());
		program = linkProgram(vertexShader, geometryShader, fragmentShader);
		glDeleteShader(geometryShader);
	}
	else {
		program = linkProgram(vertexShader, fragmentShader);
	}
	
	//validateProgram(program);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return Shader{ program };
}