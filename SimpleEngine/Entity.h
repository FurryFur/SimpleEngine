#pragma once

#include "InputComponent.h"
#include "ModelComponent.h"
#include "MovementComponent.h"
#include "InputMapComponent.h"
#include "PhysicsComponent.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "PickupComponent.h"
#include "PlayerStatsComponent.h"
#include "SnakeTailComponent.h"
#include "ControlComponent.h"

#include <vector>

class EntityEventListener;
 
enum ComponentType {
	COMPONENT_TRANSFORM = 1 << 0,
	COMPONENT_PHYSICS = 1 << 1,
	COMPONENT_MODEL = 1 << 2,
	COMPONENT_CAMERA = 1 << 3,
	COMPONENT_MOVEMENT = 1 << 4,
	COMPONENT_INPUT = 1 << 5,
	COMPONENT_INPUT_MAP = 1 << 6,
	COMPONENT_PICKUP = 1 << 7,
	COMPONENT_PLAYERSTATS = 1 << 8,
	COMPONENT_SNAKETAIL = 1 << 9,
	COMPONENT_CONTROL = 1 << 10
};

class Entity {
	// The scene will handle all entity creation and destruction
	friend class Scene;

public:
	TransformComponent transform;
	PhysicsComponent physics;
	ModelComponent model;
	MovementComponent movement;
	InputComponent input;
	InputMapComponent inputMap;
	CameraComponent camera;
	PickupComponent pickup;
	PlayerStatsComponent playerStats;
	SnakeTailComponent snakeTail;
	ControlComponent controlVars;

	Entity(Entity&&) = default;
	Entity(const Entity&) = delete;
	Entity& operator=(const Entity&) = delete;
	Entity& operator=(Entity&&) = default;

	// Overload equality to check identity equivalence
	bool operator==(const Entity& rhs) const;

	// Returns true if ALL the specified components are present 
	// in the entity.
	template<typename ...ComponentTs>
	bool hasComponents(size_t first, ComponentTs... rest) const;

	// Returns true if ALL the components in the supplied component 
	// mask are present in the entity.
	bool hasComponents(size_t componentTypeMask) const;

	// Returns true if ANY of the specified components are present
	// in the entity.
	template<typename ...ComponentTs>
	bool hasComponentsAny(size_t first, ComponentTs... rest) const;

	// Returns true if ANY of the components in the supplied component
	// mask are present in the entity.
	bool hasComponentsAny(size_t componentTypeMask) const;

	// Returns true if this entity has any components
	bool hasComponents() const;

	// Adds multiple components to the entity
	template<typename ...ComponentTs>
	void addComponents(size_t first, ComponentTs... rest);

	// Adds a component to the entity.
	// A mask can also be specified to add more than one entity at
	// a time i.e. (COMPONENT_NETWORK | COMPONENT_TRANSFORM).
	void addComponents(size_t componentTypeMask);

	// Removes a component from the entity.
	// A mask can also be specified to remove more than one entity at
	// a time i.e. (COMPONENT_NETWORK | COMPONENT_TRANSFORM).
	void removeComponents(size_t componentTypeMask);

	// Removes multiple components from the entity
	template<typename ...ComponentTs>
	void removeComponents(size_t first, ComponentTs... rest);

	// Assembles the component mask from multiple arguments
	template<typename ...ComponentTs>
	static size_t assembleComponentTypeMask(size_t first, ComponentTs... rest);
	static size_t assembleComponentTypeMask(size_t componentTypeMask);

	// Returns true if ALL the specified components are present 
	// in the entity.
	template<typename ...ComponentTs>
	static bool matches(size_t componentTypeMask, size_t first, ComponentTs... rest);

	// Returns true if ALL the components in the supplied component 
	// mask are present in the entity.
	static bool matches(size_t lhsComponentTypeMask, size_t rhsComponentTypeMask);

	// Returns true if ANY of the specified components are present
	// in the entity.
	template<typename ...ComponentTs>
	static bool matchesAny(size_t componentTypeMask, size_t first, ComponentTs... rest);

	// Returns true if ANY of the components in the supplied component
	// mask are present in the entity.
	static bool matchesAny(size_t lhsComponentTypeMask, size_t rhsComponentTypeMask);

	void triggerAddedComponentsEvent(size_t componentTypeMask);
	void triggerAboutToRemoveComponentsEvent(size_t componentTypeMask);

private:
	Entity(std::vector<EntityEventListener*>& eventListeners);

	// Destroys to entity
	void destroy();

	size_t m_componentTypeMask;
	std::vector<EntityEventListener*>& m_eventListeners;
};

template<typename ...ComponentTs>
inline bool Entity::hasComponents(size_t first, ComponentTs... rest) const
{
	return hasComponents(first) && hasComponents(rest...);
}

template<typename ...ComponentTs>
inline bool Entity::hasComponentsAny(size_t first, ComponentTs ...rest) const
{
	return hasComponentsAny(first) || hasComponentsAny(rest...);
}

template<typename ...ComponentTs>
inline void Entity::addComponents(size_t first, ComponentTs... rest)
{
	size_t componentMask = assembleComponentTypeMask(first, rest...);
	addComponents(componentMask);
}

template<typename ...ComponentTs>
inline void Entity::removeComponents(size_t first, ComponentTs ...rest)
{
	size_t componentMask = assembleComponentTypeMask(first, rest...);
	removeComponents(componentMask);
}

template<typename ...ComponentTs>
inline size_t Entity::assembleComponentTypeMask(size_t first, ComponentTs ...rest)
{
	return first | assembleComponentTypeMask(rest...);
}

template<typename ...ComponentTs>
inline bool Entity::matches(size_t componentTypeMask, size_t first, ComponentTs ...rest)
{
	return matches(componentTypeMask, first) && matches(componentTypeMask, rest...);
}

template<typename ...ComponentTs>
inline bool Entity::matchesAny(size_t componentTypeMask, size_t first, ComponentTs ...rest)
{
	return matchesAny(componentTypeMask, first) || matchesAny(componentTypeMask, rest...);
}
