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

Material Material::pureColorMaterial(Vector3 color)
{
	Material m(color, color, color, 0.5f);
	return m;
}
