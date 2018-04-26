#define _USE_MATH_DEFINES
#include "GameplayScreen.h"

#include "InputSystem.h"
#include "PhysicsSystem.h"
#include "RenderSystem.h"
#include "PickupSystem.h"
#include "MovementSystem.h"
#include "PrimitivePrefabs.h"
#include "GLUtils.h"
#include "LevelLoader.h"
#include "CameraSystem.h"
#include "SnakeTailSystem.h"
#include "Utils.h"

#include <cmath>
#include <list>

GameplayScreen::GameplayScreen()
{
	// Init systems
	m_activeSystems.push_back(std::make_unique<InputSystem>(m_scene));
	m_activeSystems.push_back(std::make_unique<MovementSystem>(m_scene));
	m_activeSystems.push_back(std::make_unique<PhysicsSystem>(m_scene));
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
	Texture irradianceMap = GLUtils::loadCubeMapFaces({
		"Assets/Textures/envmap_violentdays/violentdays_irr_c00.bmp",
		"Assets/Textures/envmap_violentdays/violentdays_irr_c01.bmp",
		"Assets/Textures/envmap_violentdays/violentdays_irr_c02.bmp",
		"Assets/Textures/envmap_violentdays/violentdays_irr_c03.bmp",
		"Assets/Textures/envmap_violentdays/violentdays_irr_c04.bmp",
		"Assets/Textures/envmap_violentdays/violentdays_irr_c05.bmp",
	});
	Texture radianceMap = GLUtils::loadDDSTexture("Assets/Textures/envmap_violentdays/violentdays_pmrem.dds");
	renderSystem->setRadianceMap(radianceMap.id);
	renderSystem->setIrradianceMap(irradianceMap.id);

	// Setup the camera
	Entity& cameraEntity = Prefabs::createCamera(m_scene, { 0, 50, 50 }, { 0, 0, 0 }, { 0, 1, 0 });
	renderSystem->setCamera(&cameraEntity);

	// Create Ground
	/*TransformComponent groundTransform{};
	groundTransform.eulerAngles.x = M_PI / 2.0f;
	groundTransform.position.y = -1;
	groundTransform.scale *= 100;
	Prefabs::createQuad(m_scene, groundTransform);*/
	CreateLevel(m_scene,"Assets/Maps/Level2.txt");
	
	TransformComponent pickupTransform{};
	pickupTransform.scale.x = 3;
	pickupTransform.scale.y = 3;
	pickupTransform.scale.z = 3;
	Entity& pickup1 = Prefabs::createModel(m_scene, "Assets/Models/crystal/mese.obj", pickupTransform);
	pickup1.transform.position = glm::vec3(-20, 1, -20);
	pickup1.addComponents(COMPONENT_PICKUP);
	pickup1.pickup.isActive = true;

	
	// Setup player1
	TransformComponent playerTransform{};
	playerTransform.scale.x = 2.0f;
	Entity& player1 = Prefabs::createCube(m_scene, playerTransform);
	player1.addComponents(COMPONENT_INPUT, COMPONENT_INPUT_MAP, COMPONENT_MOVEMENT, COMPONENT_PHYSICS, COMPONENT_PLAYERSTATS);
	player1.inputMap.gamepadIdx = 0; // First gamepad plugged in
	player1.inputMap.turnAxisMap = 0; // Left stick x axis
	player1.inputMap.accelerationBtnMap = 0; // A Button (Xbox controller)
	player1.inputMap.brakeBtnMap = 2;

	//for (int i = 0; i < 10000; ++i) {
	//	TransformComponent transform;
	//	transform.position = { randomReal(-100.0f, 100.0f), randomReal(-100.0f, 100.0f), randomReal(-100.0f, 100.0f) };
	//	Prefabs::createPyramid(m_scene, transform);
	//}

	//// Setup player2
	/*playerTransform.scale.x = 2.0f;
	Entity& player2 = Prefabs::createCube(m_scene, playerTransform);
	player2.addComponents(COMPONENT_INPUT, COMPONENT_INPUT_MAP, COMPONENT_MOVEMENT, COMPONENT_PHYSICS, COMPONENT_PLAYERSTATS);
	player2.inputMap.gamepadIdx = 1; // First gamepad plugged in
	player2.inputMap.turnAxisMap = 0; // Left stick x axis
	player2.inputMap.accelerationBtnMap = 0; // A Button (Xbox controller)
	player2.inputMap.brakeBtnMap = 2;

	// Setup player3
	playerTransform.scale.x = 2.0f;
	Entity& player3 = Prefabs::createCube(m_scene, playerTransform);
	player3.addComponents(COMPONENT_INPUT, COMPONENT_INPUT_MAP, COMPONENT_MOVEMENT, COMPONENT_PHYSICS, COMPONENT_PLAYERSTATS);
	player3.inputMap.gamepadIdx = 2; // First gamepad plugged in
	player3.inputMap.turnAxisMap = 0; // Left stick x axis
	player3.inputMap.accelerationBtnMap = 0; // A Button (Xbox controller)
	player3.inputMap.brakeBtnMap = 2;

	// Setup player4
	playerTransform.scale.x = 2.0f;
	Entity& player4 = Prefabs::createCube(m_scene, playerTransform);
	player4.addComponents(COMPONENT_INPUT, COMPONENT_INPUT_MAP, COMPONENT_MOVEMENT, COMPONENT_PHYSICS, COMPONENT_PLAYERSTATS);
	player4.inputMap.gamepadIdx = 3; // First gamepad plugged in
	player4.inputMap.turnAxisMap = 0; // Left stick x axis
	player4.inputMap.accelerationBtnMap = 0; // A Button (Xbox controller)
	player4.inputMap.brakeBtnMap = 2;*/
	
	m_playerList.push_back (&player1);
	//m_playerList.push_back (&player2);
	//m_playerList.push_back (&player3);
	//m_playerList.push_back (&player4);

	Prefabs::createTerrain(m_scene, "Assets/Textures/Heightmaps/heightmap_2.png", 100, 100);

	m_activeSystems.push_back(std::make_unique<PickupSystem>(m_scene, m_playerList));
	//m_activeSystems.push_back(std::make_unique<CameraSystem>(m_scene, m_playerList));
	m_activeSystems.push_back(std::make_unique<SnakeTailSystem>(m_scene));
	m_activeSystems.push_back(std::move(renderSystem));
}


GameplayScreen::~GameplayScreen()
{
}
