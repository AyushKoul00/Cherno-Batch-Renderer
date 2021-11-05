#include "SandboxLayer.h"
#include "Renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image\stb_image.h>

using namespace GLCore;
using namespace GLCore::Utils;

SandboxLayer::SandboxLayer()
	: m_CameraController(16.0f/9.0f)
{
}

SandboxLayer::~SandboxLayer()
{
}


static GLuint LoadTexture(const std::string& path)
{
	int w, h, bits;
	
	stbi_set_flip_vertically_on_load(true);
	auto* pixels = stbi_load(path.c_str(), &w, &h, &bits, STBI_rgb);
	if (!pixels)
	{
		std::cout << "Image error\n";
		exit(-1);
	}
	GLuint textureID;
	glCreateTextures(GL_TEXTURE_2D, 1, &textureID);

	
	//glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	stbi_image_free(pixels);

	return textureID;
}

//unsigned int VAO, VBO;
void SandboxLayer::OnAttach()
{
	EnableGLDebugging();

	m_Shader = std::unique_ptr<Shader>(Shader::FromGLSLTextFiles(
		"assets/shader.glsl.vert",
		"assets/shader.glsl.frag"
	));

	glUseProgram(m_Shader->GetRendererID());
	auto loc = glGetUniformLocation(m_Shader->GetRendererID(), "u_Textures");
	int samplers[32];
	for (int i = 0; i < 32; ++i)
		samplers[i] = i;
	glUniform1iv(loc, 32, samplers);

	//glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClearColor(.0f, .0f, .0f, 1.0f);

	Renderer::Init();

	glActiveTexture(GL_TEXTURE1);
	m_ChernoTex = LoadTexture("assets/Cherno.png");
	glActiveTexture(GL_TEXTURE2);
	m_HazelTex = LoadTexture("assets/Cherno.png");


	//float test[] = {
	//	-0.5f, 0.0f, 0.0f,
	//	 0.0f, 1.0f, 0.0f,
	//	 0.5f, 0.0f, 0.0f,
	//};


	//glCreateVertexArrays(1, &VAO);
	//glBindVertexArray(VAO);

	//glCreateBuffers(1, &VBO);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(test), test, GL_DYNAMIC_DRAW);

	//glEnableVertexArrayAttrib(VAO, 0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);
}

void SandboxLayer::OnDetach()
{
	// Shutdown here
	Renderer::Shutdown();
}

void SandboxLayer::OnEvent(Event& event)
{
	// Events here
	m_CameraController.OnEvent(event);

	if (event.GetEventType() == EventType::WindowResize)
	{
		WindowResizeEvent& e = (WindowResizeEvent&)event;
		glViewport(0, 0, e.GetWidth(), e.GetHeight());
	}
}

static void SetUniformMat4(uint32_t shader, const char* name, const glm::mat4& matrix)
{
	int loc = glGetUniformLocation(shader, name);
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
}

void SandboxLayer::OnUpdate(Timestep ts)
{
	// Render here
	m_CameraController.OnUpdate(ts);

	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(m_Shader->GetRendererID());

	const auto& vp = m_CameraController.GetCamera().GetViewProjectionMatrix();
	SetUniformMat4(m_Shader->GetRendererID(), "u_ViewProj", vp);
	SetUniformMat4(
		m_Shader->GetRendererID(),
		"u_Tranform",
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f))
	);

	//glBindVertexArray(VAO);
	//glDrawArrays(GL_TRIANGLES, 0, 3);

	Renderer::ResetStats();
	Renderer::BeginBatch();

	for (float y = -10.0f; y < 10.0f; y += 0.25f)
	{
		for (float x = -10.0f; x < 10.0f; x += 0.25f)
		{
			glm::vec4 color = { (x + 10) / 20.0f, 0.2f , (y + 10) / 20.f, 1.0f };
			//glm::vec4 color = { 1.0f, 0.0f, 1.0f, 1.0f };
			Renderer::DrawQuad({ x, y }, { 0.25f, 0.25f }, color);
		}
	}

	for (int y = 0; y < 5; ++y)
	{
		for (int x = 0; x < 5; ++x)
		{
			GLuint tex = (x + y) & 1 ? m_HazelTex : m_ChernoTex;
			Renderer::DrawQuad({ x, y }, { 1.0f, 1.0f }, tex);
		}
	}

	Renderer::DrawQuad(m_QuadPosition, { 1.0f, 1.0f }, m_ChernoTex);
	Renderer::EndBatch();

	Renderer::Flush();
}

void SandboxLayer::OnImGuiRender()
{
	// ImGui here
	ImGui::Begin("Controls");
	ImGui::DragFloat2("Quad Position", glm::value_ptr(m_QuadPosition), 0.1f);
	ImGui::Text("Quads: %d", Renderer::GetStats().QuadCount);
	ImGui::Text("Draw Calls: %d", Renderer::GetStats().DrawCount);
	ImGui::End();
}
