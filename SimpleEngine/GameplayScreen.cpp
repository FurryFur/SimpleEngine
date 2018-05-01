#define _USE_MATH_DEFINES
#include "GameplayScreen.h"

#include "InputSystem.h"
#include "PhysicsSystem.h"
#include "RenderSystem.h"
#include "PickupSystem.h"
#include "VehicleMovementSystem.h"
#include "PrimitivePrefabs.h"
#include "GLUtils.h"
#include "LevelLoader.h"
#include "CameraSystem.h"
#include "SnakeTailSystem.h"
#include "BasicCameraMovementSystem.h"
#include "Utils.h"

#include <cmath>
#include <list>

GameplayScreen::GameplayScreen()
{
	// Init systems
	m_activeSystems.push_back(std::make_unique<InputSystem>(m_scene));
	auto basicCameraMovementSystem = std::make_unique<BasicCameraMovementSystem>(m_scene);
	auto renderSystem = std::make_unique<RenderSystem>(m_scene);

	// Create environment map / skybox
	Entity& skybox = Prefabs::createSkybox(m_scene, {
		"Assets/Textures/envmap_violentdays/violentdays_rt.tga",
		"Assets/Textures/envmap_violentdays/violentdays_lf.tga",
		"Assets/Textures/envmap_violentdays/violentdays_up.tga",
		"Assets/Textures/envmap_violentdays/violentdays_dn.tga",
		"Assets/Textures/envmap_violentdays/violentdays_bk.tga",
		"Assets/Textures/envmap_violentdays/violentdays_ft.tga",
	});
	Texture irradianceMap = GLUtils::loadDDSTexture("Assets/Textures/envmap_violentdays/violentdays_iem.dds");
	Texture radianceMap = GLUtils::loadDDSTexture("Assets/Textures/envmap_violentdays/violentdays_pmrem.dds");
	renderSystem->setRadianceMap(radianceMap.id);
	renderSystem->setIrradianceMap(irradianceMap.id);

	// Setup the camera
	Entity& cameraEntity = Prefabs::createCamera(m_scene, { 0, 50, 50 }, { 0, 0, 0 }, { 0, 1, 0 });
	renderSystem->setCamera(&cameraEntity);
	basicCameraMovementSystem->setCameraToControl(&cameraEntity);

	//Prefabs::createTerrain(m_scene, "Assets/Textures/Heightmaps/heightmap_2.png", 100, 100);
	Prefabs::createTessTerrain(m_scene, "Assets/Textures/Heightmaps/heightmap_2.png", 1000);

	Entity& reflectiveSphere = Prefabs::createSphere(m_scene);
	reflectiveSphere.transform.position += glm::vec3(0, 50, 0);
	reflectiveSphere.model.materials[0].shader = &GLUtils::getDebugShader();
	reflectiveSphere.model.materials[0].debugColor = glm::vec3(1, 1, 1);
	reflectiveSphere.model.materials[0].shaderParams.glossiness = 1.0f;
	reflectiveSphere.model.materials[0].shaderParams.metallicness = 1.0f;

	Entity& diffuseSphere = Prefabs::createSphere(m_scene);
	diffuseSphere.transform.position += glm::vec3(10, 50, 0);
	diffuseSphere.model.materials[0].shader = &GLUtils::getDebugShader();
	diffuseSphere.model.materials[0].debugColor = glm::vec3(1, 1, 1);
	diffuseSphere.model.materials[0].shaderParams.glossiness = 0.0f;
	diffuseSphere.model.materials[0].shaderParams.metallicness = 0.0f;
	diffuseSphere.model.materials[0].shaderParams.specBias = -0.04f;

	m_activeSystems.push_back(std::move(renderSystem));
	m_activeSystems.push_back(std::move(basicCameraMovementSystem));
}


GameplayScreen::~GameplayScreen()
{
}
