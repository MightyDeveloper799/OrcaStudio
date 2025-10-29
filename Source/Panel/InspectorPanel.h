#pragma once

#ifndef INSPECTOR_PANEL_H
#define INSPECTOR_PANEL_H

#include "Panel.h"
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QGroupBox>
#include <memory>

namespace Orca { class Scene; }

namespace Orca::Editor
{
	class InspectorPanel : public Panel
	{
		Q_OBJECT

	public:
		explicit InspectorPanel(QWidget* parent = nullptr);
		~InspectorPanel() override = default;

		QWidget* GetWidget() override { return this; }
		void Update(float deltaTime) override;

	public slots:
		void SetSelectedEntity(int entityID);

	private:
		void DrawEntityProperties();

		QGroupBox* DrawTransformComponent(const QString& entityName);

		QGroupBox* DrawMeshRendererComponent();

		QWidget* CreatePropertyRow(const QString& label, const QVariant& value = QVariant());

	private:
		int m_selectedEntityID = 0;

		QVBoxLayout* m_mainLayout;
		QScrollArea* m_scrollArea;

		QWidget* m_contentWidget;
		QVBoxLayout* m_contentLayout;
	};
}

#endif