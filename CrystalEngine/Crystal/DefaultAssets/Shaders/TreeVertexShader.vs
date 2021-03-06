#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in mat4 instanceMatrix;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;
out vec3 vColor;
out vec4 FragPosLightSpace;

uniform mat4 view;
uniform mat4 projection;
uniform vec4 bodyColor;
uniform mat4 lightSpaceMatrix;

void main()
{
    gl_Position = projection * view * instanceMatrix * vec4(position, 1.0f);
    FragPos = vec3(instanceMatrix * vec4(position, 1.0f));
    Normal = mat3(transpose(inverse(instanceMatrix))) * normal;  
    vColor = vec3(bodyColor.x,bodyColor.y,bodyColor.z);
	TexCoords = texCoords;
	FragPosLightSpace = lightSpaceMatrix * vec4(FragPos,1.0);
}