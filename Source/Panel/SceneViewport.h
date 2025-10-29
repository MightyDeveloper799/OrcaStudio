#pragma once

#ifndef SCENE_VIEWPORT_H
#define SCENE_VIEWPORT_H

#include <QtCore/QTimer>
#include <QtOpenGL/QOpenGLBuffer>
#include <QtGui/QOpenGLFunctions>
#include <QtOpenGL/QOpenGLShaderProgram>
#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QtOpenGL/QOpenGLVertexArrayObject>
#include <QtGui/QMatrix4x4>

namespace Orca
{
	class SceneViewport : public QOpenGLWidget, protected QOpenGLFunctions
	{
	public:
		explicit SceneViewport(QWidget* parent = nullptr);
		~SceneViewport() override;

	protected:
		void initializeGL() override;
		void paintGL() override;
		void resizeGL(int w, int h) override;

	private:
		bool InitializeShaders();
		void InitializeGeometry();

		QOpenGLShaderProgram* m_Program = nullptr;
		QOpenGLBuffer m_VBO;
		QOpenGLVertexArrayObject m_VAO;
		QMatrix4x4 m_Projection;

		QTimer m_Timer;
	};
}

#endif