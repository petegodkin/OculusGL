#pragma once
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <vector>
#include <GL/glew.h>
#include "Material.h"
#include "BoundingBox.h"
#include <map>

struct TextureData
{
	GLuint id;
	GLuint type;
};

class Mesh
{
public:
	Mesh(std::vector<glm::vec3>* vd, std::vector<glm::vec3>* nd, std::vector<GLuint>* id, aiMaterial* material,
		std::vector<TextureData>* td = NULL, std::vector<glm::vec2>* tc = NULL, std::vector<aiBone>* boneData = NULL,
		std::vector<glm::ivec4>* boneIdData1 = NULL, std::vector<glm::vec4>* boneWeightData1 = NULL,
		std::vector<aiAnimation*>* animationData = NULL, glm::vec3 diffuse = glm::vec3(1));
	~Mesh();
	GLuint VAO;
	GLuint VBO;
	GLuint VBO2;
	GLuint VBO3;
	GLuint VBO_BoneID;
	GLuint VBO_BoneWeight;
	GLuint IND;

	std::vector<glm::vec3> verts;
	std::vector<glm::vec3> normals;
	std::vector<TextureData> textures;
	std::vector<unsigned int> indices;
	std::vector<glm::vec2> texCoords;

	std::vector<aiBone> bones;
	std::vector<aiNode*> boneTreePtrs;
	std::vector<glm::ivec4> boneIds;
	std::vector<glm::vec4> boneWeights;
	std::vector<aiAnimation*>* animations;

	utility::BoundingBox calcBoundingBox() const;
	//utility::BoundingBox getBoundingBox();


	Material mat;
	Material bMat;

	std::vector<glm::mat4> getBoneTransformations();

	virtual void bindVAO() const { glBindVertexArray(VAO); }
	virtual void unbindVAO() const { glBindVertexArray(0); }

	void draw() const;

	glm::vec3 diffuse;

private:
	bool checkError(std::string msg);
	//utility::BoundingBox m_boundingBox;
	std::vector<glm::mat4> boneTransformations;
};