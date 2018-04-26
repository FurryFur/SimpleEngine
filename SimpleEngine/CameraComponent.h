#pragma once

#include <glm\glm.hpp>

struct CameraComponent {
public:
	void setLookAt(const glm::vec3& pos, const glm::vec3& center, const glm::vec3& up);
	glm::mat4 getView() const;
	glm::vec3 getPosition() const;

private:
	glm::mat4 m_viewMat;
	glm::vec3 m_position;
};