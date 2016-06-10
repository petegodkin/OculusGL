#pragma once

#include <assimp/scene.h>
#include <vector>
#include "Mesh.h"
#include <string>
#include <map>
#include <unordered_map>
#include "tdogl/Texture.h"
#include "tdogl/Bitmap.h"
#include "BoundingBox.h"

struct BoneInfo
{
	aiMatrix4x4 transformation;
	std::map<aiAnimation*, aiNodeAnim*> *bone_anim;
	aiNode *bone_pos;
	aiMatrix4x4 bone_offset;
	BoneInfo(aiNode *pos) : bone_offset(), bone_pos(pos), bone_anim(new std::map<aiAnimation*, aiNodeAnim*>), transformation()
	{
	}
};

class MeshSet
{
public:
	MeshSet(std::string path, std::string filename, float scale = 1.0, GLuint texInterpolation = GL_LINEAR, GLuint texWrap = GL_REPEAT);
	void processBones(aiNode* node);
	void processAnimations();
	~MeshSet();

	std::vector<Mesh*> getMeshes() const;
	std::vector<aiAnimation*>& getAnimations();
	std::unordered_map<std::string, BoneInfo*> boneInfo;

	aiNode *bone_tree;
	aiMatrix4x4 inverseMat;
	float getScale() const;
	void setDiffuse(glm::vec3 diffuse);
	glm::vec3 getDiffuse() const;
	void calcBoundingBox();
	utility::BoundingBox getBoundingBox();
	float getBoundingRadius() const;

	glm::vec3 rotations = glm::vec3(0);

private:
	void recursiveProcess(aiNode* node, const aiScene* scene, GLuint texInterpolation, GLuint texWrap);
	void processMesh(aiMesh* mesh, const aiScene* scene, GLuint texInterpolation, GLuint texWrap);
	//unsigned int loadTexture(const char* filename, int width, int height);
	std::vector<Mesh*> meshes;
	std::vector<aiAnimation*> animations;
	//to avoid a memory leak 
	std::vector<tdogl::Texture*> texturesToDel;
	static std::map<std::string, GLuint> savedTextures;
	
	utility::BoundingBox m_boundingBox;

	float m_nDefaultScale;
	glm::vec3 m_diffuse;
	std::string m_path;
};