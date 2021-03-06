// AppSkeleton.cpp

#ifdef _WIN32
#  define WINDOWS_LEAN_AND_MEAN
#  define NOMINMAX
#  include <windows.h>
#endif
#include <GL/glew.h>

#define MOVE_SPEED 7

#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "AppSkeleton.h"
#include "MatrixFunctions.h"

using namespace glm;

//#include "Collision/ViewFrustumCuller.h"

AppSkeleton::AppSkeleton()
: m_scene()
, m_hydraScene()
#ifdef USE_OCULUSSDK
//, m_ovrScene()
#endif
, m_scenes()

, m_fboScale(1.f)
, m_cinemaScope(0.f)
, m_presentFbo()
, m_presentDistMeshL()
, m_presentDistMeshR()
, m_chassisYaw(0.f)
, m_chassisPitch(0.f)
, m_fm()
, m_hyif()
, m_rtSize(800, 600)
, m_rayHitsScene(false)
, m_spaceCursor()
, m_spaceCursorPos()
, m_keyboardMove(0.f)
, m_joystickMove(0.f)
, m_mouseMove(0.f)
, m_keyboardYaw(0.f)
, m_joystickYaw(0.f)
, m_mouseDeltaYaw(0.f)
, m_keyboardDeltaPitch(0.f)
, m_keyboardDeltaRoll(0.f)
{
    // Add as many scenes here as you like. They will share color and depth buffers,
    // so drawing one after the other should just result in pixel-perfect integration -
    // provided they all do forward rendering. Per-scene deferred render passes will
    // take a little bit more work.
    m_scenes.push_back(&m_scene);
    //m_scenes.push_back(&m_hydraScene);
#ifdef USE_OCULUSSDK
    //m_scenes.push_back(&m_ovrScene);
#endif

    // Give this scene a pointer to get live Hydra data for display
    //m_hydraScene.SetFlyingMousePointer(&m_fm);

    ResetChassisTransformations();
}

AppSkeleton::~AppSkeleton()
{
    m_fm.Destroy();
}

void AppSkeleton::SetFBOScale(float s)
{
    m_fboScale = s;
    m_fboScale = std::max(0.05f, m_fboScale);
    m_fboScale = std::min(1.0f, m_fboScale);
}

void AppSkeleton::ResetChassisTransformations()
{
    m_chassisPos = glm::vec3(0.f, 1.27f, 1.f); // my sitting height
    m_chassisYaw = 0.f;
    m_chassisPitch = 0.f;
    m_chassisRoll = 0.f;
}

glm::mat4 AppSkeleton::makeWorldToChassisMatrix() const
{
    return makeChassisMatrix_glm(m_chassisYaw, m_chassisPitch, m_chassisRoll, m_chassisPos);
}

void AppSkeleton::initGL()
{
    for (std::vector<IScene*>::iterator it = m_scenes.begin();
        it != m_scenes.end();
        ++it)
    {
        IScene* pScene = *it;
        if (pScene != NULL)
        {
            pScene->initGL();
        }
    }

    m_presentFbo.initProgram("presentfbo");
    _initPresentFbo(m_presentFbo);
    m_presentDistMeshL.initProgram("presentmesh");
    m_presentDistMeshR.initProgram("presentmesh");
    // Init the present mesh VAO *after* initVR, which creates the mesh
     //sensible initial value?
	allocateFBO(m_renderBuffer, 1000, 800);
    m_fm.Init();

    m_spaceCursor.initGL();
}


void AppSkeleton::_initPresentFbo(ShaderWithVariables& pres, bool rotateForPortrait)
{
    pres.bindVAO();

    const float verts[] = {
        -1, -1,
        1, -1,
        1, 1,
        -1, 1
    };

    // These tex coords output 1920x1080 aligned to screen pixel coordinates
    // with no transformation. However, since the display scans out along its
    // longer axis, setting the screen to landscape mode in Windows's control
    // panel can introduce extra judder.
    const float texsLandscape[] = {
        0, 0,
        1, 0,
        1, 1,
        0, 1,
    };

    // These tex coords effectively perform a rotation to output a 1920x1080
    // FBO to portrait mode screen at 1080x1920.
    const float texsPortrait[] = {
        0, 1,
        0, 0,
        1, 0,
        1, 1,
    };
    const float* texs = rotateForPortrait ? texsPortrait : texsLandscape;

    GLuint vertVbo = 0;
    glGenBuffers(1, &vertVbo);
    pres.AddVbo("vPosition", vertVbo);
    glBindBuffer(GL_ARRAY_BUFFER, vertVbo);
    glBufferData(GL_ARRAY_BUFFER, 4*2*sizeof(GLfloat), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(pres.GetAttrLoc("vPosition"), 2, GL_FLOAT, GL_FALSE, 0, NULL);

    GLuint texVbo = 0;
    glGenBuffers(1, &texVbo);
    pres.AddVbo("vTex", texVbo);
    glBindBuffer(GL_ARRAY_BUFFER, texVbo);
    glBufferData(GL_ARRAY_BUFFER, 4*2*sizeof(GLfloat), texs, GL_STATIC_DRAW);
    glVertexAttribPointer(pres.GetAttrLoc("vTex"), 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glEnableVertexAttribArray(pres.GetAttrLoc("vPosition"));
    glEnableVertexAttribArray(pres.GetAttrLoc("vTex"));

    glUseProgram(pres.prog());
    {
        glm::mat4 id(1.0f);
        glUniformMatrix4fv(pres.GetUniLoc("mvmtx"), 1, false, glm::value_ptr(id));
        glUniformMatrix4fv(pres.GetUniLoc("prmtx"), 1, false, glm::value_ptr(id));
    }
    glUseProgram(0);

    glBindVertexArray(0);
}

void AppSkeleton::_resetGLState() const
{
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthRangef(0.0f, 1.0f);
    glDepthFunc(GL_LESS);

    glDisable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
}

void AppSkeleton::_DrawScenes(
    const float* pMvWorld,
    const float* pPersp,
    const float* pMvLocal) const
{
    for (std::vector<IScene*>::const_iterator it = m_scenes.begin();
        it != m_scenes.end();
        ++it)
    {
        const IScene* pScene = *it;
        if (pScene != NULL)
        {
            const float* pMv = pScene->m_bChassisLocalSpace ? pMvLocal : pMvWorld;
            pScene->RenderForOneEye(pMv, pPersp);
        }
    }

    // Draw scene hit cursor
    if (m_rayHitsScene)
    {
        const glm::mat4 modelview = glm::make_mat4(pMvWorld);
        glm::mat4 cursMtx = glm::translate(modelview, m_spaceCursorPos);
        m_spaceCursor.RenderForOneEye(glm::value_ptr(cursMtx), pPersp);
    }
}

void AppSkeleton::_checkSceneIntersections(glm::vec3 origin, glm::vec3 dir)
{
    m_rayHitsScene = false;
    for (std::vector<IScene*>::const_iterator it = m_scenes.begin();
        it != m_scenes.end();
        ++it)
    {
        const IScene* pScene = *it;
        if (pScene != NULL)
        {
            float t = 0.f;
            glm::vec3 hit;
            glm::vec3 norm;
            if (pScene->RayIntersects(glm::value_ptr(origin), glm::value_ptr(dir),
                &t, glm::value_ptr(hit), glm::value_ptr(norm)))
            {
                m_rayHitsScene = true;
                m_spaceCursorPos = hit;
            }
        }
    }
}

glm::vec3 findLookAtPt(float pitch, float yaw)
{
	float x, y, z;
	x = 1 * cos(pitch) * cos(yaw);
	y = 1 * sin(pitch);
	z = 1 * cos(pitch) * cos(3.14 / 2.0 - yaw);
	return glm::vec3(x, y, z);
}

glm::mat4 AppSkeleton::getView(glm::vec3 position) const {
	//if (m_pitch > 1.3)
	//	m_pitch = 1.3;
	//else if (m_pitch < -1.3)
	//	m_pitch = -1.3;

	return glm::lookAt(position, position + findLookAtPt(m_pitch, m_yaw),
		glm::vec3(0.0, 1.0, 0.0));
}

glm::mat4 AppSkeleton::makeViewMatrix() {
	return glm::mat4(1.f) * getView(m_chassisPos);
}

vec3 viewMatrixBackward(mat4 viewMatrix)
{
	return vec3(viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2]);
}

vec3 viewMatrixForward(mat4 viewMatrix)
{
	return -viewMatrixBackward(viewMatrix);
}

vec3 viewMatrixRight(mat4 viewMatrix)
{
	return vec3(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
}

vec3 viewMatrixLeft(mat4 viewMatrix)
{
	return -viewMatrixRight(viewMatrix);
}

void AppSkeleton::moveForward(double dt) {
	mat4 viewMat = makeViewMatrix();
	vec3 dir = viewMatrixForward(viewMat);
	dir = normalize(vec3(dir.x, 0, dir.z));
	dir *= MOVE_SPEED * dt;
	m_chassisPos += dir;
}
void AppSkeleton::moveBackward(double dt) {
	mat4 viewMat = makeViewMatrix();
	vec3 dir = viewMatrixBackward(viewMat);
	dir = normalize(vec3(dir.x, 0, dir.z));
	dir *= MOVE_SPEED * dt;
	m_chassisPos += dir;
}
void AppSkeleton::moveLeft(double dt) {
	mat4 viewMat = makeViewMatrix();
	vec3 dir = viewMatrixLeft(viewMat);
	dir = normalize(vec3(dir.x, 0, dir.z));
	dir *= MOVE_SPEED * dt;
	m_chassisPos += dir;
}
void AppSkeleton::moveRight(double dt) {
	mat4 viewMat = makeViewMatrix();
	vec3 dir = viewMatrixRight(viewMat);
	dir = normalize(vec3(dir.x, 0, dir.z));
	dir *= MOVE_SPEED * dt;
	m_chassisPos += dir;
}

void AppSkeleton::_drawSceneMono() const
{
    _resetGLState();
    //glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // This line make no noticeable difference

    const glm::mat4 mvLocal = glm::mat4(1.f);
	const glm::mat4 mvWorld = mvLocal * getView(m_chassisPos);
        //glm::inverse(makeWorldToChassisMatrix());

    const glm::ivec2 vp = getRTSize();
    const glm::mat4 persp = glm::perspective(
        (float)M_PI/2.0f, //Changed this from 90? Says it's in radians, so 90 didn't make sense
        static_cast<float>(vp.x)/static_cast<float>(vp.y),
        0.004f,
        1000.0f);

    _DrawScenes(glm::value_ptr(mvWorld), glm::value_ptr(persp), glm::value_ptr(mvLocal));
}

void AppSkeleton::display_raw() const
{
    const glm::ivec2 vp = getRTSize();
    glViewport(0, 0, vp.x, vp.y);
	//std::cout << "RAWWWWWW" << std::endl;
    _drawSceneMono();
}

void AppSkeleton::display_buffered(bool setViewport) const
{
    bindFBO(m_renderBuffer, m_fboScale);
	//std::cout << "BUFFERED" << std::endl;
    _drawSceneMono();
    unbindFBO();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    if (setViewport)
    {
        const glm::ivec2 vp = getRTSize();
        glViewport(0, 0, vp.x, vp.y);
    }

    // Present FBO to screen
    /*const GLuint prog = m_presentFbo.prog();
    glUseProgram(prog);
    m_presentFbo.bindVAO();
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_renderBuffer.tex);
        glUniform1i(m_presentFbo.GetUniLoc("fboTex"), 0);

        // This is the only uniform that changes per-frame
        glUniform1f(m_presentFbo.GetUniLoc("fboScale"), m_fboScale);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
    glBindVertexArray(0);
    glUseProgram(0);*/
}

void AppSkeleton::timestep(double absTime, double dt)
{
    for (std::vector<IScene*>::iterator it = m_scenes.begin();
        it != m_scenes.end();
        ++it)
    {
        IScene* pScene = *it;
        if (pScene != NULL)
        {
            pScene->timestep(absTime, dt);
        }
    }

    glm::vec3 hydraMove = glm::vec3(0.0f, 0.0f, 0.0f);
#ifdef USE_SIXENSE
    const sixenseAllControllerData& state = m_fm.GetCurrentState();
    for (int i = 0; i<2; ++i)
    {
        const sixenseControllerData& cd = state.controllers[i];
        const float moveScale = pow(10.0f, cd.trigger);

        const FlyingMouse::Hand h = static_cast<FlyingMouse::Hand>(i);
        if (m_fm.IsPressed(h, SIXENSE_BUTTON_JOYSTICK)) ///@note left hand does not work
        {
            m_chassisYaw += cd.joystick_x * .01f * moveScale;
            hydraMove.y += cd.joystick_y * moveScale;
        }
        else
        {
            hydraMove.x += cd.joystick_x * moveScale;
            hydraMove.z -= cd.joystick_y * moveScale;
        }
    }

    // Check all Hydra buttons for HSW dismissal
    if ((m_fm.WasJustPressed(FlyingMouse::Left, 0xFF)) ||
        (m_fm.WasJustPressed(FlyingMouse::Right, 0xFF)))
    {
        DismissHealthAndSafetyWarning();
    }
#endif

	if (keys[0]) {
		moveForward(dt);
	}
	if (keys[1]) {
		moveLeft(dt);
	}
	if (keys[2]) {
		moveBackward(dt);
	}
	if (keys[3]) {
		moveRight(dt);
	}

    // Move in the direction the viewer is facing.
    const glm::vec3 move_dt = (m_keyboardMove + m_joystickMove + m_mouseMove + hydraMove) * static_cast<float>(dt);
    const glm::vec4 mv4 = makeWorldToEyeMatrix() * glm::vec4(move_dt, 0.0f);
    m_chassisPos += glm::vec3(mv4);
    m_chassisYaw += (m_keyboardYaw + m_joystickYaw + m_mouseDeltaYaw) * static_cast<float>(dt);
    m_chassisPitch += m_keyboardDeltaPitch * static_cast<float>(dt);
    m_chassisRoll += m_keyboardDeltaRoll * static_cast<float>(dt);

    m_fm.updateHydraData();
    m_hyif.updateHydraData(m_fm, 1.0f);

    const FlyingMouse::Hand h = FlyingMouse::Right;
    if (m_fm.ControllerIsOnBase(h) == false)
    {
        glm::vec3 origin3;
        glm::vec3 dir3;
        m_fm.GetControllerOriginAndDirection(h, origin3, dir3);

        const glm::mat4 chasMat = makeWorldToChassisMatrix();
        origin3 = glm::vec3(chasMat * glm::vec4(origin3, 1.f));
        dir3 = glm::vec3(chasMat * glm::vec4(dir3, 0.f));

        _checkSceneIntersections(origin3, dir3);
    }
}

void AppSkeleton::resize(int w, int h)
{
    m_rtSize.x = w;
    m_rtSize.y = h;
}

void AppSkeleton::OnMouseButton(int button, int action)
{
    if (action == 1) // glfw button press
    {
        if (button == 1) // glfw right click
        {
            const glm::vec3 sittingHeight = glm::vec3(0.f, 1.27f, 0.f);
            m_chassisPos = m_spaceCursorPos + sittingHeight;
        }
    }
}

void AppSkeleton::OnMouseMove(int x, int y)
{
    // Calculate eye space ray through mouse pointer
    const glm::vec2 uv01 = glm::vec2(
        static_cast<float>(x) / static_cast<float>(m_rtSize.x),
        1.f - static_cast<float>(y) / static_cast<float>(m_rtSize.y)
        );
    const glm::vec2 uv_11 = 2.f*uv01 - glm::vec2(1.f);

    const float aspect = static_cast<float>(m_rtSize.x) / static_cast<float>(m_rtSize.y);
    const float tanHalfFov = 4.f;

    const glm::vec3 fwd(0.f, 0.f, -1.f);
    const glm::vec3 rt(1.f, 0.f, 0.f);
    const glm::vec3 up(0.0f, 1.0f, 0.0f);

    const glm::vec3 localOrigin = glm::vec3(0.f);
    const glm::vec3 localRay = glm::normalize(
        aspect*fwd +
        tanHalfFov * uv_11.x * rt +
        tanHalfFov * uv_11.y * up);

    // Transform ray into world space
    const glm::mat4 mv = makeWorldToEyeMatrix();
    const glm::vec3 origin3 = glm::vec3(mv * glm::vec4(localOrigin,1.f));
    const glm::vec3 dir3 = glm::vec3(mv * glm::vec4(localRay,0.f));

    _checkSceneIntersections(origin3, dir3);

	static glm::vec2 oldPos = glm::vec2(x, y);

	m_pitch -= (y - oldPos.y) * .005;
	m_yaw += (x - oldPos.x) * .005;
	oldPos = glm::vec2(x, y);

}

void AppSkeleton::OnMouseWheel(double x, double /*y*/)
{
    const float rotationIncrement = 30.f * static_cast<float>(M_PI) / 180.f;
    m_chassisYaw += static_cast<float>(x) * rotationIncrement;
}
