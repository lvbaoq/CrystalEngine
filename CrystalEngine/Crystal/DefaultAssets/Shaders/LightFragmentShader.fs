#version 330 core

struct Material {
	vec3 diffuseColor;
	vec3 ambientColor;
	vec3 specularColor;

	sampler2D texture_diffuse1;
    sampler2D texture_specular1;
	sampler2D texture_emission;

    float shininess;
}; 

struct DirLight {
    vec3 direction;
	vec3 position;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec3 vColor;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

out vec4 color;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform Material material;
uniform sampler2D shadowMap;

// Function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
float CalcShadow(DirLight light , vec3 normal , vec4 fragPosLightSpace);

void main()
{   
	//float depthValue = texture(shadowMap, TexCoords).r;
    //color = vec4(vec3(depthValue), 1.0);

    // Properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
 
    // Phase 1: Directional lighting
    vec3 result = CalcDirLight(dirLight, norm, viewDir); 
     
    color = vec4(result, 1.0);

}

// Calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - FragPos);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // Combine results
    vec3 ambient = light.ambient * (vec3(texture(material.texture_diffuse1, TexCoords)) * material.ambientColor);
    vec3 diffuse = light.diffuse * diff * (vec3(texture(material.texture_diffuse1, TexCoords)) * material.diffuseColor);
    vec3 specular = light.specular * spec * (vec3(texture(material.texture_specular1,TexCoords)) * material.specularColor);
    vec3 emission = vec3(texture(material.texture_emission,TexCoords));

	//Calculate shadow
	float shadow = CalcShadow(light,normal,FragPosLightSpace);
	shadow = min(shadow, 0.75); // reduce shadow strength a little: allow some diffuse/specular light in shadowed regions
	return (ambient + (1.0f - shadow ) * (diffuse + specular) + emission);
}

float CalcShadow(DirLight light , vec3 normal , vec4 fragPosLightSpace)
{
	// perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 

    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

	vec3 lightDir = normalize(light.position - FragPos);
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // Check whether current frag pos is in shadow
    //float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
	float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

	if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}