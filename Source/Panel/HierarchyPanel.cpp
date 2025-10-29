#include <QtWidgets/QHeaderView>
#include "HierarchyPanel.h"
#include <Scene/Scene.h>
#include <iostream>

namespace Orca::Editor
{
	HierarchyPanel::HierarchyPanel(QWidget* parent)
		: Panel("Hierarchy", parent), m_treeWidget(new QTreeWidget(this)), m_layout(new QVBoxLayout(this))
	{
		m_treeWidget->setHeaderHidden(true);
		m_treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
		m_treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

		m_layout->addWidget(m_treeWidget);
		this->setLayout(m_layout);

		connect(m_treeWidget, &QTreeWidget::itemClicked, this, &HierarchyPanel::onItemSelected);

		this->setStyleSheet("QTreeWidget { background-color: #2e2e2e; color: %dcdcdc border: 1px solid #3c3c3c; border-radius: 4px }");
	}

	HierarchyPanel::~HierarchyPanel() {}

	void HierarchyPanel::RebuildTree()
	{
		m_treeWidget->clear(); // Clear all previous items

		if (!m_currentScene)
		{
			QTreeWidgetItem* item = new QTreeWidgetItem(m_treeWidget, QStringList() << "No Scene Loaded");
			item->setFlags(item->flags() & ~Qt::ItemIsSelectable); // Not selectable
			return;
		}
	}

	void HierarchyPanel::onItemSelected(QTreeWidgetItem* item, int column)
	{
		QVariant data = item->data(0, Qt::UserRole);

		if (data.isValid())
		{
			int entityID = data.toInt();
			std::cout << "Selected Entity ID: " << entityID << std::endl;

			item->setSelected(true);
		}
		else
		{
			std::cout << "Selected non-entity item: " << item->text(0).toStdString() << std::endl;
		}
	}
}