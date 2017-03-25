#version 330 core

struct Material {
	vec3 diffuseColor;
	vec3 ambientColor;
	vec3 specularColor;

	sampler2D diffuse;
    sampler2D specular;
	sampler2D emission;

    float shininess;
}; 

struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec3 vColor;
in vec2 TexCoords;

out vec4 color;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform Material material;

// Function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);

void main()
{    
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
    vec3 lightDir = normalize(-light.direction);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // Combine results
    vec3 ambient = light.ambient * material.ambientColor * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * material.diffuseColor* diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular *  material.specularColor *spec * vec3(texture(material.specular,TexCoords));
    vec3 emission = vec3(texture(material.emission,TexCoords));

	return (ambient + diffuse + specular + emission);
}
