#include <app\graphics.h>

using namespace crystal;

//Pre-set materials
const Material Material::defaultMaterial(Vector3(1.0f, 1.0f, 1.0f),
	Vector3(1.0f, 1.0f, 1.0f), Vector3(0.1f, 0.1f, 0.1f), 0.5f);

const Material Material::emerald(Vector3(0.022f, 0.174f, 0.022f),
	Vector3(0.076f, 0.614f, 0.076f), Vector3(0.633f, 0.728f, 0.633f), 0.6f);

const Material Material::jade(Vector3(0.135f, 0.223f, 0.158f),
	Vector3(0.54f, 0.89f, 0.63f), Vector3(0.316f, 0.316f, 0.316f), 0.1f);

const Material Material::obsidian(Vector3(0.054f, 0.05f, 0.066f),
	Vector3(0.183f, 0.17f, 0.23f), Vector3(0.33f, 0.33f, 0.35), 0.3f);

const Material Material::pearl(Vector3(0.25f, 0.207f, 0.021f),
	Vector3(1.0f, 0.829f, 0.829f), Vector3(0.297f, 0.297f, 0.297f), 0.088f);

const Material Material::ruby(Vector3(0.175f, 0.012f, 0.012f),
	Vector3(0.614f, 0.041f, 0.041f), Vector3(0.728f, 0.627f, 0.627f), 0.6f);

void crystal::Material::setDiffuseMap(const char * const path)
{
	diffuseMap.createTexture(path,true);//Diffuse map is usually in srgb format
}

void crystal::Material::setSpecularMap(const char * const path)
{
	specularMap.createTexture(path);
}

void crystal::Material::setEmissionMap(const char * const path)
{
	emissionMap.createTexture(path,true);//Emission map is usually in srgb format
}

void crystal::Material::initTextures()
{
	if (!diffuseMap.isCreated())
		diffuseMap.createTexture(DEFAULT_TEXTURE_WHITE_PATH);
	if (!specularMap.isCreated())
		specularMap.createTexture(DEFAULT_TEXTURE_BLACK_PATH);
	if (!emissionMap.isCreated())
		emissionMap.createTexture(DEFAULT_TEXTURE_BLACK_PATH);
}

Material Material::pureColorMaterial(Vector3 color)
{
	Material m(color, color, color, 0.5f);
	return m;
}

void Texture::createTexture(const char* const location,bool isSRGB)
{
	path = location;
	GLuint type = isSRGB ? GL_SRGB:GL_RGB;
	glGenTextures(1, &texture);
	unsigned char* image;
	image = SOIL_load_image(location, &width, &height, 0, SOIL_LOAD_RGBA);
	//Set texture
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	//Set texture wrapping and filtering model
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,wrap_s_model);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t_model);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,min_filter_model);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,mag_filter_model);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	created = true;
}

void crystal::SkyBox::loadCubeMap()
{
	glGenTextures(1, &textureCube);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureCube);
	//Load images for all the faces of skybox
	unsigned char* image;
	for (GLuint i = 0; i < SKYBOX_FACES_NUMBER; i++)
	{
		image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
			GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
	}
	//Set texture attributes
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	glBindTexture(GL_TEXTURE_CUBE_MAP,0);
}

crystal::SkyBox::SkyBox()
{
	//Init face locations
	faces[0] = DEFAULT_SKYBOX_FACE_RIGHT;
	faces[1] = DEFAULT_SKYBOX_FACE_LEFT;
	faces[2] = DEFAULT_SKYBOX_FACE_TOP;
	faces[3] = DEFAULT_SKYBOX_FACE_BOTTOM;
	faces[4] = DEFAULT_SKYBOX_FACE_BACK;
	faces[5] = DEFAULT_SKYBOX_FACE_FRONT;
}

crystal::PostProcessing::PostProcessing(const char * fragmentShaderPath):
	isKernel(false),shaderinitialized(false),shader(false)
{
	kernelOffset = DEFAULT_KERNEL_OFFSET;
	fShaderPath = fragmentShaderPath;
}

crystal::PostProcessing::PostProcessing(GLfloat * kernelValues):
	isKernel(true),shaderinitialized(false),shader(false)
{
	fShaderPath = POST_PROCESSING_FRAG_PATH_KERNEL;
	kernelOffset = DEFAULT_KERNEL_OFFSET;
	for (int i = 0; i < KERNEL_VALUE_NUM; i++)
	{
		kernel[i] = kernelValues[i];
	}
}

void crystal::PostProcessing::initShader()
{
	shader.initShader(POST_PROCESSING_VSHADER_PATH, fShaderPath);
	shaderinitialized = true;
}

void crystal::PostProcessing::useShader()
{
	if (!isKernel)
	{
		shader.useShader();
		return;
	}
	//Is kernel effect
	//Use program
	shader.useShader();
	//Set uniforms
	glUniform1f(glGetUniformLocation(shader.program, KERNEL_OFFSET_UNIFORM), kernelOffset);
	glUniform1fv(glGetUniformLocation(shader.program, KERNEL_UNIFORM), KERNEL_VALUE_NUM, kernel);
}

PostProcessing PostProcessing::Inversion(POST_PROCESSING_FRAG_PATH_INVERSION);
PostProcessing PostProcessing::Grayscale(POST_PROCESSING_FRAG_PATH_GRAYSCALE);

GLfloat sharpenKernel[9] = {
	-1, -1, -1,
	-1, 9, -1,
	-1, -1, -1
};
PostProcessing PostProcessing::Sharpen(sharpenKernel);

GLfloat blurKernel[9] = {
	1.0 / 16, 2.0 / 16, 1.0 / 16,
	2.0 / 16, 4.0 / 16, 2.0 / 16,
	1.0 / 16, 2.0 / 16, 1.0 / 16
};
PostProcessing PostProcessing::Blur(blurKernel);

GLfloat edKernel[9] = {
	1, 1, 1,
	1, -8, 1,
	1, 1,  1
};
PostProcessing PostProcessing::EdgeDetection(edKernel);

GLfloat noEffectKernel[9] = {
	1, 1, 1,
	1, 1, 1,
	1, 1, 1
};
PostProcessing PostProcessing::KernelEffect(noEffectKernel);