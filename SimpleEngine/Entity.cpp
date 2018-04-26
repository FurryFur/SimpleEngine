#include "Entity.h"

#include "EntityEventListener.h"

#include <glm\glm.hpp>

#include "Log.h"

Entity::Entity(std::vector<EntityEventListener*>& eventListeners)
	: m_componentTypeMask{ 0 }
	, m_eventListeners{ eventListeners }
	, transform{}
	, physics{}
	, model{}
	, movement{}
	, input{}
	, inputMap{}
	, camera{}
	, pickup{}
	, playerStats{}
	, snakeTail{}
	, controlVars{}
{
}

void Entity::destroy()
{
	triggerAboutToRemoveComponentsEvent(m_componentTypeMask);

	m_componentTypeMask = 0;
}

bool Entity::operator==(const Entity& rhs) const
{
	return this == &rhs;
}

bool Entity::hasComponents(size_t componentTypeMask) const
{
	return (m_componentTypeMask & componentTypeMask) == componentTypeMask;
}

bool Entity::hasComponentsAny(size_t componentTypeMask) const
{
	return (m_componentTypeMask & componentTypeMask) > 0;
}

bool Entity::hasComponents() const
{
	return m_componentTypeMask != 0;
}

bool Entity::matches(size_t lhscomponentTypeMask, size_t rhscomponentTypeMask)
{
	return (lhscomponentTypeMask & rhscomponentTypeMask) == rhscomponentTypeMask;
}

bool Entity::matchesAny(size_t lhscomponentTypeMask, size_t rhscomponentTypeMask)
{
	return (lhscomponentTypeMask & rhscomponentTypeMask) > 0;
}

void Entity::triggerAddedComponentsEvent(size_t componentTypeMask)
{
	for (auto eventListener : m_eventListeners) {
		eventListener->onAddedComponents(*this, componentTypeMask);
	}
}

void Entity::triggerAboutToRemoveComponentsEvent(size_t componentTypeMask)
{
	for (auto eventListener : m_eventListeners) {
		eventListener->onBeforeRemoveComponents(*this, componentTypeMask);
	}
}

void Entity::addComponents(size_t componentTypeMask)
{
	m_componentTypeMask |= componentTypeMask;

	//if (matches(componentTypeMask, COMPONENT_TRANSFORM)) {
	//	transform = {};
	//}
	//if (matches(componentTypeMask, COMPONENT_PHYSICS)) {
	//	physics = {};
	//}
	//if (matches(componentTypeMask, COMPONENT_MODEL)) {
	//	model = {};
	//}
	//if (matches(componentTypeMask, COMPONENT_CAMERA)) {
	//	camera = {};
	//}
	//if (matches(componentTypeMask, COMPONENT_MOVEMENT)) {
	//	movement = {};
	//}
	//if (matches(componentTypeMask, COMPONENT_INPUT)) {
	//	input = {};
	//}
	//if (matches(componentTypeMask, COMPONENT_INPUT_MAP)) {
	//	inputMap = {};
	//}

	triggerAddedComponentsEvent(componentTypeMask);
}

void Entity::removeComponents(size_t componentTypeMask)
{
	triggerAboutToRemoveComponentsEvent(componentTypeMask);
	m_componentTypeMask &= (~componentTypeMask);
}

size_t Entity::assembleComponentTypeMask(size_t componentTypeMask)
{
	return componentTypeMask;
}
