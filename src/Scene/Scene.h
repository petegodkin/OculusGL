// Scene.h

#pragma once

#ifdef _WIN32
#  define WINDOWS_LEAN_AND_MEAN
#  define NOMINMAX
#  include <windows.h>
#endif
#include <stdlib.h>
#include <GL/glew.h>

#include <glm/glm.hpp>

#include "IScene.h"
#include "ShaderWithVariables.h"
#include "Shape.h"
#include "DeferredShader.h"
#include "..\Util\Collision\OctTree.h"
#include "..\Util\Collision\ViewFrustumCuller.h"

///@brief The Scene class renders everything in the VR world that will be the same
/// in the Oculus and Control windows. The RenderForOneEye function is the display entry point.
class Scene : public IScene
{
public:
    Scene();
    virtual ~Scene();

    virtual void initGL();
    virtual void timestep(double absTime, double dt);
    virtual void RenderForOneEye(const float* pMview, const float* pPersp) const;

    virtual bool RayIntersects(
        const float* pRayOrigin,
        const float* pRayDirection,
        float* pTParameter, // [inout]
        float* pHitLocation, // [inout]
        float* pHitNormal // [inout]
        ) const;

protected:
    void DrawColorCube() const;
	void DrawObj() const;
    void DrawGrid() const;
    void DrawOrigin() const;
    void DrawScene(
        const glm::mat4& modelview,
        const glm::mat4& projection,
        const glm::mat4& object) const;

	void Scene::DrawDude(
		const glm::mat4& modelview,
		const glm::mat4& projection,
		glm::vec3 center) const;

protected:
    void _InitCubeAttributes();
	void _InitObjAttributes();
    void _InitPlaneAttributes();

    void _DrawBouncingCubes(
        const glm::mat4& modelview,
        glm::vec3 center,
        float radius,
        float scale) const;
    void _DrawScenePlanes(const glm::mat4& modelview) const;

    ShaderWithVariables m_basic;
    ShaderWithVariables m_plane;

	DeferredShader *m_deferred;

    float m_phaseVal;

	std::vector<Entity*> m_ents;
	std::vector<MeshSet*> m_meshes;
	std::vector<Light*> m_lights;

	Entity *m_skybox;

	void addEntities(MeshSet *mesh, float radius, int amount, float depth = 0.f);
	void addMorphableEntities(std::vector<MeshSet*> meshes, float radius, int amount, float depth = 0.f);
	void addGround();
	void addLights(float radius, int amount);

	OctTree* m_oct;
	ViewFrustumCuller* m_culler;

public:
    float m_amplitude;

	int width() const;
	int height() const;
	int fboID() const;
	int finalTexture() const;

private: // Disallow copy ctor and assignment operator
    Scene(const Scene&);
    Scene& operator=(const Scene&);
};
