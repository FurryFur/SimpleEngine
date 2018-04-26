#include "CameraComponent.h"

#include <glm\gtc\matrix_transform.hpp>

void CameraComponent::setLookAt(const glm::vec3& pos, const glm::vec3& center, const glm::vec3& up)
{
	m_viewMat = glm::lookAt(pos, center, up);
	m_position = pos;
}

glm::mat4 CameraComponent::getView() const
{
	return m_viewMat;
}

glm::vec3 CameraComponent::getPosition() const
{
	return m_position;
}
