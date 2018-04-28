#version 420 core

out vec4 outColor;
  
in vec2 texCoord;

uniform sampler2D sceneSampler;

void main()
{ 
    outColor = texture(sceneSampler, texCoord);
}