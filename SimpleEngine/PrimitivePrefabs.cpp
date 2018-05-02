#define _USE_MATH_DEFINES

#include "PrimitivePrefabs.h"

#include "GLUtils.h"
#include "GLPrimitives.h"
#include "Scene.h"
#include "Entity.h"
#include "ModelUtils.h"
#include "Log.h"
#include "stb_image.h"
#include "VertexFormat.h"
#include "Utils.h"

#include <glm\gtc\matrix_transform.hpp>

#include <memory>
#include <array>

using namespace glm;

namespace Prefabs {

	Entity& createQuad(Scene& scene, const TransformComponent& transform)
	{
		Entity& entity = scene.createEntity(COMPONENT_MODEL | COMPONENT_TRANSFORM);

		entity.transform = transform;


		entity.model = GLPrimitives::getQuadModel();
		entity.model.materials.at(0).shaderParams.glossiness = 0.0f;
		entity.model.materials.at(0).shaderParams.metallicness = 0.0f;
		entity.model.materials.at(0).shaderParams.specBias = 0;

		// Replace default texture
		entity.model.materials.at(0).colorMaps.at(0) = GLUtils::loadTexture("Assets/Textures/dessert-floor.png");

		return entity;
	}

	Entity& createSphere(Scene& scene, const TransformComponent& transform)
	{
		Entity& entity = scene.createEntity(COMPONENT_MODEL | COMPONENT_TRANSFORM);

		entity.transform = transform;

		entity.model = GLPrimitives::getSphereModel();

		return entity;
	}

	Entity& createCylinder(Scene& scene, float radius, float height, const TransformComponent& transform)
	{
		Entity& entity = scene.createEntity(COMPONENT_MODEL | COMPONENT_TRANSFORM);

		entity.transform = transform;
		entity.transform.scale.x *= radius;
		entity.transform.scale.y *= height;
		entity.transform.scale.z *= radius;

		entity.model = GLPrimitives::getCylinderModel();

		return entity;
	}

	Entity& createPyramid(Scene& scene, const TransformComponent& transform)
	{
		Entity& entity = scene.createEntity(COMPONENT_MODEL | COMPONENT_TRANSFORM);

		entity.transform = transform;

		entity.model = GLPrimitives::getPyramidModel();

		return entity;
	}

	Entity& createCube(Scene& scene, const TransformComponent& transform)
	{
		Entity& entity = scene.createEntity(COMPONENT_MODEL | COMPONENT_TRANSFORM);

		entity.transform = transform;

		entity.model = GLPrimitives::getCubeModel();

		return entity;
	}

	Entity& createCamera(Scene& scene, const glm::vec3& pos, const glm::vec3& center, const glm::vec3& up)
	{
		Entity& entity = scene.createEntity(COMPONENT_CAMERA, COMPONENT_TRANSFORM);

		entity.camera.setLookAt(pos, center, up);

		return entity;
	}

	Entity& createSkybox(Scene& scene, const std::vector<std::string>& faceFilenames)
	{
		Entity& entity = scene.createEntity(COMPONENT_MODEL);

		entity.model = GLPrimitives::getCubeModel();

		// Replace default material
		entity.model.materials.at(0) = {};
		entity.model.materials.at(0).shader = &GLUtils::getSkyboxShader();
		entity.model.materials.at(0).colorMaps.push_back(GLUtils::loadCubeMapFaces(faceFilenames));
		entity.model.materials.at(0).willDrawDepth = false;

		return entity;
	}

	Entity& createModel(Scene& scene, const std::string& path, const TransformComponent& transform)
	{
		Entity& entity = scene.createEntity(COMPONENT_MODEL | COMPONENT_TRANSFORM);

		entity.model = ModelUtils::loadModel(path);

		entity.transform = transform;

		return entity;
	}

	// Helper function for creating a tesselated quad for terrain
	void createTessellatedQuadData(GLsizei numVertsX, GLsizei numVertsZ, float width, float height, std::vector<VertexFormat>& vertices, std::vector<GLuint>& indices)
	{
		const GLsizei numCellsX = numVertsX - 1;
		const GLsizei numCellsZ = numVertsZ - 1;
		const GLsizei numIndicesPerCell = 6;
		const GLsizei numIndices = numCellsX * numCellsZ * numIndicesPerCell;
		const float xExtent = width / 2;
		const float zExtent = height / 2;
		const float xSpacing = width / (numVertsX - 1);
		const float zSpacing = height / (numVertsZ - 1);
		vertices.resize(numVertsX * numVertsZ);

		// Create vertices
		for (GLsizei r = 0; r < numVertsZ; ++r) {
			for (GLsizei c = 0; c < numVertsX; ++c) {
				float x = -xExtent + c * xSpacing;
				float z = -zExtent + r * zSpacing;
				vertices[r * numVertsX + c].position = vec3{ x, 0, z };
				vertices[r * numVertsX + c].normal = vec3{ 0, 1, 0 };
				vertices[r * numVertsX + c].texCoord = vec2{ (x + xExtent) / width, (z + zExtent) / height };
			}
		}

		// Create indices
		indices.resize(numIndices);
		for (GLsizei r = 0; r < numCellsZ; ++r) {
			for (GLsizei c = 0; c < numCellsX; ++c) {
				indices[(r * numCellsX + c) * numIndicesPerCell] = r * numVertsX + c;
				indices[(r * numCellsX + c) * numIndicesPerCell + 1] = (r + 1) * numVertsX + c;
				indices[(r * numCellsX + c) * numIndicesPerCell + 2] = (r + 1) * numVertsX + c + 1;

				indices[(r * numCellsX + c) * numIndicesPerCell + 3] = r * numVertsX + c;
				indices[(r * numCellsX + c) * numIndicesPerCell + 4] = (r + 1) * numVertsX + c + 1;
				indices[(r * numCellsX + c) * numIndicesPerCell + 5] = r * numVertsX + c + 1;
			}
		}
	}

	Entity& createTessTerrain(Scene& scene, const std::string& heightMapFile, float size, GLsizei baseTessellation, const glm::vec3& position) 
	{
		const float heightScale = size * 0.1f;

		// Read height map from file
		int numPixelsX, numPixelsY, numChannels;
		unsigned char* heightMapImg = stbi_load(heightMapFile.c_str(), &numPixelsX, &numPixelsY, &numChannels, 0);

		// Convert to array of floats
		std::vector<float> heightMapData(numPixelsX * numPixelsY);
		for (GLsizei r = 0; r < numPixelsY; ++r) {
			for (GLsizei c = 0; c < numPixelsX; ++c) {
				heightMapData[r * numPixelsX + c] = heightMapImg[(r * numPixelsX + c) * numChannels] / 255.0f; // Ignore any extra channels by skipping over them
			}
		}
		Texture heightMap = Texture::Texture2D(numPixelsX, numPixelsY, GL_RED, GL_FLOAT, heightMapData.data());

		stbi_image_free(heightMapImg);

		// Create tesselated quad with save number of vertices as heightmap pixels
		GLsizei numVertsX, numVertsZ;
		numVertsX = numVertsZ = baseTessellation + 1;
		std::vector<VertexFormat> meshVertices;
		std::vector<GLuint> meshIndices;
		createTessellatedQuadData(numVertsX, numVertsZ, size, size, meshVertices, meshIndices);

		// TODO: Move this function somewhere logical so we can use it for follow terrain as well.
		auto terrainPosToHeightMapCoord = [size, numPixelsX, numPixelsY, position](float x, float z, float& outU, float& outV, bool& isOutsideOfTerrain) {
			const float extent = size / 2.0f;
			const float leftEdge = position.x - extent;
			const float topEdge = position.z - extent;
			outU = (x - leftEdge) / size;
			outV = (z - topEdge) / size;

			// Check if inside or outside terrain x,z bounds
			if (outU < 0.0f || outU > 1.0f || outV < 0.0f || outV > 1.0f)
				isOutsideOfTerrain = true;
			else
				isOutsideOfTerrain = false;

			// Scale U and V to absolute pixel coordinates
			outU *= numPixelsX;
			outV *= numPixelsY;
		};

		auto meshIdxToHeightMapIdx = [size, numPixelsX, numPixelsY, numVertsX, numVertsZ](GLsizei vertR, GLsizei vertC, GLsizei& heightMapR, GLsizei& heightMapC) {
			const float extent = size / 2.0f;
			float u = vertC / static_cast<float>(numVertsX - 1);
			float v = vertR / static_cast<float>(numVertsZ - 1);
			heightMapC = static_cast<GLsizei>(glm::round(u * (numPixelsX - 1)));
			heightMapR = static_cast<GLsizei>(glm::round(v * (numPixelsY - 1)));
		};

		auto heightMapIdxToVertPos = [size, numPixelsX, numPixelsY, &heightMapData, heightScale, position](GLsizei r, GLsizei c) -> vec3 {
			const float extent = size / 2;
			const float xSpacing = size / (numPixelsX - 1);
			const float zSpacing = size / (numPixelsY - 1);

			vec3 vertPos;
			vertPos.x = c * xSpacing - extent;
			vertPos.y = heightMapData[r * numPixelsX + c] * heightScale;
			vertPos.z = r * zSpacing - extent;
			vertPos += position; // Offset by overall terrain position
			return vertPos;
		};

		// Compute normal map
		std::vector<vec3> normalMapData(numPixelsX * numPixelsY);
		for (GLsizei r = 0; r < numPixelsY; ++r) {
			for (GLsizei c = 0; c < numPixelsX; ++c) {

				// Compute and accumulate tri normals surrounding vertex
				vec3 normalAccumulator = { 0, 0, 0 };
				GLsizei numSurroundingTris = 0;
				for (int i = -1; i < 1; ++i) {
					for (int j = -1; j < 1; ++j) {
						// Get corresponding cell verts
						std::array<vec3, 4> cellVerts;
						for (GLsizei k = 0; k < cellVerts.size(); ++k) {
							GLsizei pixelR = r + i + (k / 2);
							GLsizei pixelC = c + j + (k % 2);
							pixelR = pixelR >= 0 && pixelR < numPixelsY ? pixelR : r;
							pixelC = pixelC >= 0 && pixelC < numPixelsX ? pixelC : c;
							cellVerts[k] = heightMapIdxToVertPos(pixelR, pixelC);
						}

						// Get vertices for each tri in cell
						// 0--1
						// |\ |
						// | \| 
						// 2--3
						std::array<std::array<vec3, 3>, 2> tris = { { { cellVerts[0], cellVerts[2], cellVerts[3] },
						                                              { cellVerts[0], cellVerts[3], cellVerts[1] } } };

						// Compute and accumulate normals for each tri in cell
						for (auto& triVerts : tris) {
							vec3 edge1 = triVerts[1] - triVerts[0];
							vec3 edge2 = triVerts[2] - triVerts[0];

							vec3 normal = cross(edge1, edge2);
							if (normal.x != 0 || normal.y != 0 || normal.z != 0) { // Can't normalize zero vectors generated at edges
								normalAccumulator += normalize(normal);
								++numSurroundingTris;
							}
						}
					}
				}
				normalMapData[r * numPixelsX + c] = normalize(normalAccumulator / static_cast<float>(numSurroundingTris));
			}
		}
		// Create GPU normalMap Texture
		Texture normalMap = Texture::Texture2D(numPixelsX, numPixelsY, GL_RGB, GL_FLOAT, normalMapData.data());

		// Create GPU mesh
		Mesh mesh{
			0, // Use the first material on the model
			GLUtils::bufferMeshData(meshVertices, meshIndices),
			static_cast<GLsizei>(meshIndices.size())
		};

		// Create the terrain entity
		Entity& terrain = scene.createEntity(COMPONENT_MODEL, COMPONENT_TRANSFORM);
		terrain.transform.position = position;

		// Fill model component with mesh data
		terrain.model.rootNode.meshIDs.push_back(0);
		terrain.model.rootNode.meshIDs.push_back(1);
		terrain.model.meshes.push_back(mesh);
		terrain.model.meshes.push_back(mesh);
		terrain.model.meshes[1].materialIndex = 1;

		// Create terrain material component
		Material terrainMaterial;
		terrainMaterial.shader = &GLUtils::getTerrainShader();
		terrainMaterial.colorMaps.push_back(GLUtils::loadTexture("Assets/Textures/dessert-floor.png"));
		terrainMaterial.normalMaps.push_back(normalMap);
		terrainMaterial.heightMaps.push_back(heightMap);
		terrainMaterial.heightMapScale = heightScale;
		terrainMaterial.willDrawDepth = true;
		terrainMaterial.shaderParams.metallicness = 0;
		terrainMaterial.shaderParams.glossiness = 0;
		terrainMaterial.shaderParams.specBias = 0;
		terrain.model.materials.push_back(std::move(terrainMaterial));

		// Create grass material component
		Material grassMaterial;
		grassMaterial.shader = &GLUtils::getTerrainGrassGeoShader();
		grassMaterial.colorMaps.push_back(GLUtils::loadTexture("Assets/Textures/weedy_grass.png"));
		grassMaterial.colorMaps.push_back(GLUtils::loadTexture("Assets/Textures/vegetation_map.png", false, false));
		grassMaterial.heightMaps.push_back(heightMap);
		grassMaterial.heightMapScale = heightScale;
		grassMaterial.willDrawDepth = true;
		grassMaterial.shaderParams.metallicness = 0;
		grassMaterial.shaderParams.glossiness = 0;
		grassMaterial.shaderParams.specBias = 0;
		terrain.model.materials.push_back(std::move(grassMaterial));

		return terrain;
	}
}