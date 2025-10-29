#include "InspectorPanel.h"
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QStyleFactory>
#include <iostream>

namespace Orca::Editor
{
	// --- InspectorPanel Constructor & Setup ---

	InspectorPanel::InspectorPanel(QWidget* parent /*= nullptr*/)
		: Panel("Inspector", parent), m_mainLayout(new QVBoxLayout(this))
	{
		m_contentWidget = new QWidget(this);
		m_contentLayout = new QVBoxLayout(m_contentWidget);
		m_contentLayout->setAlignment(Qt::AlignTop);
		m_contentLayout->setContentsMargins(10, 10, 10, 10);
		m_contentLayout->setSpacing(12);

		m_scrollArea = new QScrollArea(this);
		m_scrollArea->setWidgetResizable(true);
		m_scrollArea->setWidget(m_contentWidget);
		m_scrollArea->setFrameShape(QFrame::NoFrame);

		m_scrollArea->setStyleSheet("QScrollArea { background-color: transparent; }");

		// 3. Main Layout
		m_mainLayout->addWidget(m_scrollArea);
		m_mainLayout->setContentsMargins(0, 0, 0, 0);
		this->setLayout(m_mainLayout);

		SetSelectedEntity(0);
	}

	void InspectorPanel::Update(float deltaTime)
	{
		// Logic here for continuous updates, if needed (e.g., polling for external property changes)
	}

	void InspectorPanel::SetSelectedEntity(int entityID)
	{
		if (m_selectedEntityID != entityID)
		{
			m_selectedEntityID = entityID;
			DrawEntityProperties();
		}
	}

	void InspectorPanel::DrawEntityProperties()
	{
		QLayoutItem* item;
		while ((item = m_contentLayout->takeAt(0)) != nullptr) 
		{
			delete item->widget();
			delete item;
		}

		if (m_selectedEntityID <= 0)
		{
			QLabel* placeholder = new QLabel("Select an Entity in the Hierarchy", m_contentWidget);
			placeholder->setAlignment(Qt::AlignCenter);
			placeholder->setStyleSheet("QLabel { color: #808080; font-style: italic; padding: 50px; }");
			m_contentLayout->addWidget(placeholder);
			return;
		}

		QString entityName = QString("Entity_%1").arg(m_selectedEntityID);
		std::cout << "Inspecting Entity ID: " << m_selectedEntityID << " (" << entityName.toStdString() << ")" << std::endl;

		QLabel* nameLabel = new QLabel(entityName, m_contentWidget);
		nameLabel->setStyleSheet("QLabel { font-size: 16pt; font-weight: bold; margin-bottom: 10px; color: #f0f0f0; }");
		m_contentLayout->addWidget(nameLabel);

		m_contentLayout->addWidget(DrawTransformComponent(entityName));

		m_contentLayout->addWidget(DrawMeshRendererComponent());

		QPushButton* addComponentButton = new QPushButton("+ Add Component", m_contentWidget);
		addComponentButton->setStyleSheet(R"(
			QPushButton {
				background-color: #555555;
				border: 1px solid #777777;
				padding: 8px;
				margin-top: 20px;
				border-radius: 4px;
				color: white;
			}
			QPushButton:hover {
				background-color: #6a6a6a;
			}
		)");
		m_contentLayout->addWidget(addComponentButton);
	}

	QWidget* InspectorPanel::CreatePropertyRow(const QString& label, const QVariant& value /*= QVariant()*/)
	{
		QWidget* rowWidget = new QWidget(m_contentWidget);
		QHBoxLayout* rowLayout = new QHBoxLayout(rowWidget);
		rowLayout->setContentsMargins(0, 0, 0, 0);
		rowLayout->setSpacing(10);

		QLabel* labelWidget = new QLabel(label, rowWidget);
		labelWidget->setFixedWidth(80);
		rowLayout->addWidget(labelWidget);

		rowLayout->addStretch();
		return rowWidget;
	}

	QGroupBox* InspectorPanel::DrawTransformComponent(const QString& entityName)
	{
		QGroupBox* groupBox = new QGroupBox("Transform", m_contentWidget);
		QVBoxLayout* groupLayout = new QVBoxLayout(groupBox);

		groupBox->setStyleSheet(R"(
			QGroupBox {
				border: 1px solid #3c3c3c;
				border-radius: 4px;
				margin-top: 10px;
				font-weight: bold;
				padding-top: 10px;
			}
			QGroupBox::title {
				subcontrol-origin: margin;
				subcontrol-position: top left; /* Position the title */
				padding: 0 3px;
				background-color: #2e2e2e; /* Match panel background */
				color: #88c0d0; /* Highlight color for component headers */
			}
		)");

		groupLayout->addWidget(CreatePropertyRow("Position (X,Y,Z)"));
		groupLayout->addWidget(CreatePropertyRow("Rotation (X,Y,Z)"));
		groupLayout->addWidget(CreatePropertyRow("Scale (X,Y,Z)"));

		groupLayout->setContentsMargins(10, 15, 10, 10);
		groupBox->setLayout(groupLayout);
		return groupBox;
	}

	QGroupBox* InspectorPanel::DrawMeshRendererComponent()
	{
		QGroupBox* groupBox = new QGroupBox("Mesh Renderer", m_contentWidget);
		QVBoxLayout* groupLayout = new QVBoxLayout(groupBox);

		groupBox->setStyleSheet(R"(
			QGroupBox {
				border: 2px solid #5d5d5d;
				border-radius: 4px;
				margin-top: 10px;
				font-weight: bold;
				padding-top: 10px;
			}
			QGroupBox::title {
				subcontrol-origin: margin;
				subcontrol-position: top left;
				padding: 0 3px;
				background-color: #2e2e2e;
				color: #88c0d0;
			}
		)");

		QWidget* materialRow = new QWidget(m_contentWidget);
		QHBoxLayout* materialLayout = new QHBoxLayout(materialRow);
		materialLayout->setContentsMargins(0, 0, 0, 0);
		materialLayout->setSpacing(10);
		materialLayout->addWidget(new QLabel("Material:", materialRow));
		QLineEdit* materialInput = new QLineEdit("DefaultMaterial", materialRow);
		materialInput->setReadOnly(true);
		materialInput->setStyleSheet("QLineEdit { background-color: #444444; border: 1px solid #555555; color: #dcdcdc; padding: 2px; border-radius: 2px; }");
		materialLayout->addWidget(materialInput);
		QPushButton* selectButton = new QPushButton("...", materialRow);
		selectButton->setFixedWidth(30);
		materialLayout->addWidget(selectButton);
		materialLayout->addStretch();

		groupLayout->addWidget(materialRow);

		groupLayout->addWidget(new QCheckBox("Cast Shadows", m_contentWidget));

		groupLayout->setContentsMargins(10, 15, 10, 10);
		groupBox->setLayout(groupLayout);
		return groupBox;
	}
}