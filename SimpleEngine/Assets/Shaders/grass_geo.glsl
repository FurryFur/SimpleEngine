#version 420 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 4) out;

layout (std140) uniform UniformBlock {
    mat4 model;
    mat4 view;
    mat4 projection;
	vec4 cameraPos;
	vec4 spotlightPositions[8];
	vec4 spotlightDirections[8];
	vec4 spotlightColors[8];
	uint numSpotlights;
	float metallicness;
	float glossiness;
	float specBias;
} sh;

in VertexData {
    vec3 normal;
    vec2 texCoord;
	vec3 viewDir;
	vec3 worldPos;
} gs_in[];

out VertexData {
	vec3 normal;
    vec2 texCoord;
	vec3 viewDir;
	vec3 worldPos;
} gs_out;

void main() {
	vec3 triCenter = vec3(0, 0, 0);
	for (int i = 0; i < 3; ++i) {
		triCenter += gs_in[i].worldPos;
	}
	triCenter /= 3;


	vec3 u = normalize(gs_in[1].worldPos - gs_in[0].worldPos);
	vec3 v = gs_in[2].worldPos - gs_in[0].worldPos;
	vec3 n = normalize(cross(u, v));
	v = cross(u, n);
	mat3 tangentSpace = mat3(u, n, v);

	const mat4 vp = sh.projection * sh.view;

	vec3 lowerLeft = triCenter + tangentSpace * vec3(-0.1f, 0, 0);
	vec3 lowerRight = triCenter + tangentSpace * vec3(0.1f, 0, 0);
	vec3 upperLeft = triCenter + tangentSpace * vec3(-0.1f, 0.2f, 0);
	vec3 upperRight = triCenter + tangentSpace * vec3(0.1f, 0.2f, 0);
	vec3 quadNormal = cross(lowerRight - lowerLeft, upperLeft - lowerLeft);

	// Lower Left
	gs_out.normal = quadNormal;
	gs_out.texCoord = vec2(0, 0);
	gs_out.worldPos = lowerLeft;
	gs_out.viewDir = sh.cameraPos.xyz - gs_out.worldPos;
	gl_Position = vp * vec4(gs_out.worldPos, 1);
	EmitVertex();

	// Lower Right
	gs_out.normal = quadNormal;
	gs_out.texCoord = vec2(1, 0);
	gs_out.worldPos = lowerRight;
	gs_out.viewDir = sh.cameraPos.xyz - gs_out.worldPos;
	gl_Position = vp * vec4(gs_out.worldPos, 1);
	EmitVertex();

	// Upper Left
	gs_out.normal = quadNormal;
	gs_out.texCoord = vec2(0, 1);
	gs_out.worldPos = upperLeft;
	gs_out.viewDir = sh.cameraPos.xyz - gs_out.worldPos;
	gl_Position = vp * vec4(gs_out.worldPos, 1);
	EmitVertex();

	// Upper Right
	gs_out.normal = quadNormal;
	gs_out.texCoord = vec2(1, 1);
	gs_out.worldPos = upperRight;
	gs_out.viewDir = sh.cameraPos.xyz - gs_out.worldPos;
	gl_Position = vp * vec4(gs_out.worldPos, 1);
	EmitVertex();

	EndPrimitive();
}
