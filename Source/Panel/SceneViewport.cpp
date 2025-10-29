#include "SceneViewport.h"
#include <Renderer/Mesh.h>
#include <QtGui/QSurfaceFormat>
#include <QtGui/QVector3D>
#include <Core/Logger.h>

static const float cubeVertices[] = 
{
	-1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f,
	-1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f,
	-1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f,

	 1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	-1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	-1.0f,  1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	 1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	-1.0f,  1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	 1.0f,  1.0f, -1.0f, 0.0f, 1.0f, 0.0f,

	  1.0f, -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
	  1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
	  1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
	  1.0f, -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
	  1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
	  1.0f,  1.0f,  1.0f, 0.0f, 0.0f, 1.0f,

	  -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f,
	  -1.0f, -1.0f,  1.0f, 1.0f, 1.0f, 0.0f,
	  -1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 0.0f,
	  -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f,
	  -1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 0.0f,
	  -1.0f,  1.0f, -1.0f, 1.0f, 1.0f, 0.0f,

	  -1.0f,  1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
	   1.0f,  1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
	   1.0f,  1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
	  -1.0f,  1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
	   1.0f,  1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
	  -1.0f,  1.0f, -1.0f, 0.0f, 1.0f, 1.0f,

	  -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f,
	   1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f,
	   1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 1.0f,
	  -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f,
	   1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 1.0f,
	  -1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 1.0f
};

namespace Orca
{
	static float s_RotationAngle = 0.0f;

	SceneViewport::SceneViewport(QWidget* parent)
		: QOpenGLWidget(parent)
	{
		QSurfaceFormat format;
		format.setDepthBufferSize(24);
		format.setVersion(3, 3);
		format.setProfile(QSurfaceFormat::CoreProfile);
		setFormat(format);

		connect(&m_Timer, &QTimer::timeout, this, [this]()
			{
				this->update();
			});

		setFocusPolicy(Qt::StrongFocus);
		setWindowTitle(tr("Scene Viewport"));
	}

	SceneViewport::~SceneViewport()
	{
		makeCurrent();
		delete m_Program;
		doneCurrent();
	}

	bool SceneViewport::InitializeShaders()
	{
		const char* vertexSrc = 
			"#version 330 core\n"
			"\n"
			"layout (location = 0) in vec3 aPos;\n"
			"layout (location = 1) in vec3 aColor;\n"
			"\n"
			"uniform mat4 model;\n"
			"uniform mat4 view;\n"
			"uniform mat4 projection;\n"
			"\n"
			"out vec3 vColor;\n"
			"\n"
			"void main()\n"
			"{\n"
			"    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
			"    vColor = aColor;\n"
			"}\n";

		const char* fragmentSrc =
			"#version 330 core\n"
			"\n"
			"in vec3 vColor;\n"
			"out vec4 FragColor;\n"
			"\n"
			"void main()\n"
			"{\n"
			"	FragColor = vec4(vColor, 1.0);\n"
			"}\n";

		m_Program = new QOpenGLShaderProgram(this);

		if (!m_Program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexSrc))
		{
			Logger::Log(LogLevel::Warning, "Couldn't compile the vertex shader!");
			return false;
		}
		if (!m_Program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentSrc))
		{
			Logger::Log(LogLevel::Warning, "Couldn't compile the fragment shader!");
			return false;
		}
		if (!m_Program->link())
		{
			Logger::Log(LogLevel::Warning, "Couldn't link the shader program!");
			return false;
		}

		return true;
	}

	void SceneViewport::InitializeGeometry()
	{
		m_VAO.create();
		m_VAO.bind();

		m_VBO.create();
		m_VBO.bind();

		m_VBO.allocate(cubeVertices, sizeof(cubeVertices));

		const int stride = 6 * sizeof(float);

		m_Program->bind();

		m_Program->enableAttributeArray(0);
		m_Program->setAttributeArray(0, GL_FLOAT, 0, 3, stride);

		m_Program->enableAttributeArray(1);
		m_Program->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(float), 3, stride);

		m_VAO.release();
		m_VBO.release();
	}

	void SceneViewport::initializeGL()
	{
		this->initializeOpenGLFunctions();

		Logger::Log(LogLevel::Info, std::string("OpenGL context version: ") + (const char*)this->glGetString(GL_VERSION));

		this->glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		this->glEnable(GL_DEPTH_TEST);

		if (!this->InitializeShaders())
		{
			Logger::Log(LogLevel::Fatal, "Failed to initialize OpenGL Shaders!");
		}
		this->InitializeGeometry();
	}

	void SceneViewport::paintGL()
	{
		if (!m_Program || !m_Program->isLinked()) return;

		this->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_Program->bind();
		m_VAO.bind();

		QMatrix4x4 view;
		view.lookAt(QVector3D(0.0f, 0.0f, 5.0f),
			QVector3D(0.0f, 0.0f, 0.0f),
			QVector3D(0.0f, 1.0f, 0.0f));

		QMatrix4x4 model;

		model.rotate(s_RotationAngle, 0.0f, 1.0f, 0.0f);

		s_RotationAngle += 0.5f;
		if (s_RotationAngle > 360.0f) 
		{
			s_RotationAngle -= 360.0f;
		}

		m_Program->setUniformValue("projection", m_Projection);
		m_Program->setUniformValue("view", view);
		m_Program->setUniformValue("model", model);

		this->glDrawArrays(GL_TRIANGLES, 0, 36);

		m_VAO.release();
		m_Program->release();
	}

	void SceneViewport::resizeGL(int w, int h)
	{
		this->glViewport(0, 0, w, h);

		m_Projection.setToIdentity();

		float aspectRatio = (h > 0) ? (float)w / h : 1.0f;
		m_Projection.perspective(45.0f, aspectRatio, 0.1f, 100.0f);
	}
}