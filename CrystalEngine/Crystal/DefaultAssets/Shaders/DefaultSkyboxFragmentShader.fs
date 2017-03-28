#version 330 core
in vec3 TexCoords;
out vec4 color;

uniform samplerCube skybox;
uniform vec3 dirLightDiffuse;

void main()
{    
    color = vec4(dirLightDiffuse * vec3(texture(skybox, TexCoords)),1.0f);
}