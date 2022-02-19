#include <ETOD.h>

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>

class ExampleLayer : public ETOD::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f)   //, m_SquarePosition(0.0f)
	{
		// 顶点数组

		m_VertexArray.reset(ETOD::VertexArray::Create());

		// 顶点缓存

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};

		std::shared_ptr<ETOD::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(ETOD::VertexBuffer::Create(vertices, sizeof(vertices)));
		ETOD::BufferLayout layout = {
			{ ETOD::ShaderDataType::Float3, "a_Position" },
			{ ETOD::ShaderDataType::Float4, "a_Color" }
		};

		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		// 索引缓冲

		uint32_t indices[3] = { 0, 1, 2 };
		std::shared_ptr<ETOD::IndexBuffer> indexBuffer;
		indexBuffer.reset(ETOD::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_SquareVA.reset(ETOD::VertexArray::Create());

		float squareVertices[3 * 4] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f,  0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f
		};

		std::shared_ptr<ETOD::VertexBuffer> squareVB;
		squareVB.reset(ETOD::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

		squareVB->SetLayout({
			{ ETOD::ShaderDataType::Float3, "a_Position" }
		});

		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<ETOD::IndexBuffer> squareIB;
		squareIB.reset(ETOD::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);

		std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}
		)";

		m_Shader.reset(new ETOD::Shader(vertexSrc, fragmentSrc));

		std::string flatColorShaderVertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";

		std::string flatColorShaderFragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			uniform vec4 u_Color;

			void main()
			{
				color = u_Color;
			}
		)";

		m_FlatColorShader.reset(new ETOD::Shader(flatColorShaderVertexSrc, flatColorShaderFragmentSrc));
	}


	void OnUpdate(ETOD::Timestep ts) override
	{
		if (ETOD::Input::IsKeyPressed(ETOD_KEY_LEFT))
			m_CameraPosition.x -= m_CameraMoveSpeed * ts;

		else if (ETOD::Input::IsKeyPressed(ETOD_KEY_RIGHT))
			m_CameraPosition.x += m_CameraMoveSpeed * ts;

		if (ETOD::Input::IsKeyPressed(ETOD_KEY_UP))
			m_CameraPosition.y += m_CameraMoveSpeed * ts;

		else if (ETOD::Input::IsKeyPressed(ETOD_KEY_DOWN))
			m_CameraPosition.y -= m_CameraMoveSpeed * ts;

		if (ETOD::Input::IsKeyPressed(ETOD_KEY_A))
			m_CameraRotation += m_CameraRotationSpeed * ts;

		if (ETOD::Input::IsKeyPressed(ETOD_KEY_D))
			m_CameraRotation -= m_CameraRotationSpeed * ts;

		//ETOD_INFO("实例层::更新"); // ExampleLayer::Update
		ETOD::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		ETOD::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		ETOD::Renderer::BeginScene(m_Camera);

		static glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		glm::vec4 redColor(0.8f, 0.2f, 0.3f, 1.0f);
		glm::vec4 blueColor(0.2f, 0.3f, 0.8f, 1.0f);

		for (int y = 0; y < 20; y++)
		{
			for (int x = 0; x < 20; x++)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				if (x % 2 == 0)
				{
					m_FlatColorShader->UploadUniformFloat4("u_Color", redColor);
				}
				else
				{
					m_FlatColorShader->UploadUniformFloat4("u_Color", blueColor);
				}
				ETOD::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
			}
		}

		ETOD::Renderer::Submit(m_Shader, m_VertexArray);

		ETOD::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{

	}

	void OnEvent(ETOD::Event& event) override
	{
	}
private:
	std::shared_ptr<ETOD::Shader> m_Shader;
	std::shared_ptr<ETOD::VertexArray> m_VertexArray;

	std::shared_ptr<ETOD::Shader> m_FlatColorShader;
	std::shared_ptr<ETOD::VertexArray> m_SquareVA;

	ETOD::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 5.0f;

	float m_CameraRotation = 0.0f;
	float m_CameraRotationSpeed = 180.0f;
};

class Sandbox : public ETOD::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox()
	{

	}

};

ETOD::Application* ETOD::CreateApplication()
{
	return new Sandbox();
}