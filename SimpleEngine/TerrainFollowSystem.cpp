#include "TerrainFollowSystem.h"

#include "Terrain.h"
#include "Clock.h"

#include <glm\gtx\compatibility.hpp>

TerrainFollowSystem::TerrainFollowSystem(Scene& scene)
	: System(scene)
{
}

void TerrainFollowSystem::update(Entity& entity)
{
	if (!entity.hasComponents(COMPONENT_TERRAIN_FOLLOW, COMPONENT_TRANSFORM))
		return;

	float terrainHeight;
	bool success = TerrainUtils::castPosToTerrainHeight(*entity.terrainFollow.terrainToFollow, entity.transform.position, terrainHeight);
	if (success) {
		entity.transform.position.y = glm::lerp(entity.transform.position.y, terrainHeight, Clock::getDeltaTime());
	}
}

void TerrainFollowSystem::beginFrame()
{
}

void TerrainFollowSystem::endFrame()
{
}
