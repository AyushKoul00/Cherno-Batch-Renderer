#pragma once

#include <GLCore.h>
#include <GLCoreUtils.h>



#include <GLCore\Util\OrthographicCameraController.h>
#include <GLCore\Util\Shader.h>
#include <GLCore\Core\Layer.h>

class SandboxLayer : public GLCore::Layer
{
public:
	SandboxLayer();
	virtual ~SandboxLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnEvent(GLCore::Event& event) override;
	virtual void OnUpdate(GLCore::Timestep ts) override;
	virtual void OnImGuiRender() override;
private:
	GLCore::Utils::OrthographicCameraController m_CameraController;
	std::unique_ptr<GLCore::Utils::Shader> m_Shader;
	GLuint m_ChernoTex, m_HazelTex;
	glm::vec2 m_QuadPosition = { 0.0f, 0.0f };
};