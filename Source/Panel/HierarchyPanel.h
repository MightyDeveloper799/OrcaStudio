#pragma once

#ifndef HIERARCHY_PANEL_H
#define HIERARCHY_PANEL_H

#include "Panel.h"
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <memory>

namespace Orca { class Scene; }

namespace Orca::Editor
{
	class HierarchyPanel : public Panel
	{
		Q_OBJECT

	public:
		explicit HierarchyPanel(QWidget* parent = nullptr);
		~HierarchyPanel() override;

		QWidget* GetWidget() override { return this; }
		void Update(float deltaTime) override;

		void SetScene(const std::shared_ptr<Orca::Scene>& scene) override;
	private slots:
		void onItemSelected(QTreeWidgetItem* item, int column);

	private:
		void RebuildTree();

	private:
		std::shared_ptr<Scene> m_currentScene;
		QTreeWidget* m_treeWidget;
		QVBoxLayout* m_layout;
	};
}

#endif