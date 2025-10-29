#include "EditorApp.h"
#include "../Panel/SceneViewport.h"
#include "../Panel/HierarchyPanel.h"
#include "../Panel/InspectorPanel.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtCore/QDateTime>

namespace Orca
{
	EditorApp::EditorApp(QWidget* parent) : QMainWindow(parent)
	{
		this->setWindowTitle("Orca(R) Studio");
		this->setMinimumSize(860, 640);

		ApplyDarkTheme();

		SetupMenuBar();

		SceneViewport* viewport = new SceneViewport(this);
		this->setCentralWidget(viewport);

		SetupLeftDocks();
		SetupRightDock();
        SetupStatusBar();

		QDockWidget* hierarchyDock = this->findChild<QDockWidget*>("HierarchyDock");
		QDockWidget* projectDock = this->findChild<QDockWidget*>("ProjectDock");
		if (hierarchyDock && projectDock)
		{
			this->tabifyDockWidget(hierarchyDock, projectDock);
			hierarchyDock->raise();
		}
	}

	void EditorApp::ApplyDarkTheme()
	{
		QString style = R"(QMainWindow { background-color: #1e1e1e; }
        QMenuBar { background-color: #2d2d2d; color: #ccc; border-bottom: 1px solid #333333; }
        QMenuBar::item:selected { background-color: #007acc; color: white; }
        QDockWidget { border: 1px solid #333333; }
        QDockWidget::title { background: #2d2d2d; padding: 5px; color: #ccc; border-bottom: 1px solid #333333; }
        QWidget { background-color: #252526; color: #ccc; font-size: 10pt; }
        QToolBar { background-color: #2d2d2d; border: none; padding: 2px; }
        QToolButton { color: #ccc; padding: 5px; margin: 2px; }
        QToolButton:hover { background-color: #3a3a3a; }
        
        QTreeWidget { background-color: #252526; border: none; }
        QTreeWidget::item { padding: 3px 0; }
        QTreeWidget::item:selected { background-color: #007acc; color: white; }

        QLineEdit, QComboBox, QSlider { 
            background-color: #333333; 
            border: 1px solid #444444; 
            padding: 2px;
            border-radius: 3px;
            color: white;
        }
        QPushButton { 
            background-color: #4a4a4a; 
            border: 1px solid #5a5a5a; 
            color: white; 
            padding: 5px; 
            border-radius: 4px;
        }
        QPushButton:hover { background-color: #555555; }

        QScrollBar:vertical, QScrollBar:horizontal { border: 1px solid #444444; background: #2d2d2d; width: 8px; height: 8px; }
        QScrollBar::handle:vertical, QScrollBar::handle:horizontal { background: #555555; border-radius: 4px; min-height: 20px; }
        QScrollBar::add-line, QScrollBar::sub-line { border: none; background: none; })";

        this->setStyleSheet(style);
	}

    void EditorApp::SetupMenuBar()
    {
        QMenuBar* menuBar = new QMenuBar(this);

        menuBar->addMenu(tr("&File"));
        menuBar->addMenu(tr("&Edit"));
        menuBar->addMenu(tr("&Project"));
        menuBar->addMenu(tr("&Window"));
        menuBar->addMenu(tr("&Help"));
        setMenuBar(menuBar);

        QToolBar* toolBar = new QToolBar(tr("Controls"), this);
        toolBar->setMovable(false);
        toolBar->setFloatable(false);
        toolBar->setStyleSheet("QToolBar { background-color: #2d2d2d; border-bottom: 1px solid #333333; padding: 0; }");
        addToolBar(Qt::TopToolBarArea, toolBar);

        toolBar->addSeparator();
    }

    void EditorApp::SetupLeftDocks()
    {
        QDockWidget* hierarchyDock = new QDockWidget(tr("Hierarchy"), this);
        hierarchyDock->setObjectName("HierarchyDock");
        hierarchyDock->setMinimumWidth(200);
        hierarchyDock->setAllowedAreas(Qt::LeftDockWidgetArea);

        QTreeWidget* hierarchyTree = new QTreeWidget();
        hierarchyTree->setHeaderHidden(true);
        hierarchyTree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
        hierarchyTree->setStyleSheet("QTreeWidget { background-color: #252526; }");

        QTreeWidgetItem* scene = new QTreeWidgetItem(hierarchyTree, QStringList() << "SampleScene");
        new QTreeWidgetItem(scene, QStringList() << "Main Camera");
        QTreeWidgetItem* env = new QTreeWidgetItem(scene, QStringList() << "Environment");
        QTreeWidgetItem* terrain = new QTreeWidgetItem(env, QStringList() << "Terrain");
        terrain->setFlags(terrain->flags() | Qt::ItemIsUserCheckable);
        terrain->setCheckState(0, Qt::Checked);
        new QTreeWidgetItem(terrain, QStringList() << "Building 1");

        hierarchyDock->setWidget(hierarchyTree);
        addDockWidget(Qt::LeftDockWidgetArea, hierarchyDock);


        QDockWidget* projectDock = new QDockWidget(tr("Project"), this);
        projectDock->setObjectName("ProjectDock");
        projectDock->setMinimumWidth(200);
        projectDock->setAllowedAreas(Qt::LeftDockWidgetArea);

        QTreeWidget* projectTree = new QTreeWidget();
        projectTree->setHeaderHidden(true);
        new QTreeWidgetItem(projectTree, QStringList() << "Assets");
        new QTreeWidgetItem(projectTree, QStringList() << "Managers");
        new QTreeWidgetItem(projectTree, QStringList() << "Prefabs");
        new QTreeWidgetItem(projectTree, QStringList() << "Resources");

        projectDock->setWidget(projectTree);
        addDockWidget(Qt::LeftDockWidgetArea, projectDock);
    }

    void EditorApp::SetupRightDock()
    {
        QDockWidget* inspectorDock = new QDockWidget(tr("Inspector"), this);
        inspectorDock->setMinimumWidth(250);
        inspectorDock->setAllowedAreas(Qt::RightDockWidgetArea);

        QWidget* inspectorContent = new QWidget();
        QVBoxLayout* mainLayout = new QVBoxLayout(inspectorContent);
        mainLayout->setAlignment(Qt::AlignTop);
        mainLayout->setContentsMargins(10, 10, 10, 10);
        mainLayout->setSpacing(15);

        auto createTransformWidget = [=](const QString& label, const QString& value) -> QWidget* 
        {
            QWidget* w = new QWidget();
            QHBoxLayout* hLayout = new QHBoxLayout(w);
            hLayout->setContentsMargins(0, 0, 0, 0);
            hLayout->setSpacing(5);

            QLabel* l = new QLabel(label);
            l->setFixedWidth(60);

            QLineEdit* x = new QLineEdit(value);
            x->setStyleSheet("QLineEdit { color: #ff6666; }");

            QLineEdit* y = new QLineEdit(value);
            y->setStyleSheet("QLineEdit { color: #66ff66; }");

            QLineEdit* z = new QLineEdit(value);
            z->setStyleSheet("QLineEdit { color: #6666ff; }");

            hLayout->addWidget(l);
            hLayout->addWidget(x);
            hLayout->addWidget(y);
            hLayout->addWidget(z);
            return w;
        };

        mainLayout->addWidget(new QLabel("<h4>Static</h4>"));
        QWidget* tagLayer = new QWidget();
        QGridLayout* gLayout = new QGridLayout(tagLayer);
        gLayout->setContentsMargins(0, 0, 0, 0);

        gLayout->addWidget(new QLabel("Tag"), 0, 0);
        QComboBox* tagCombo = new QComboBox();
        tagCombo->addItem("Nothing");
        gLayout->addWidget(tagCombo, 0, 1);

        gLayout->addWidget(new QLabel("Layer"), 1, 0);
        QComboBox* layerCombo = new QComboBox();
        layerCombo->addItem("Default");
        gLayout->addWidget(layerCombo, 1, 1);
        mainLayout->addWidget(tagLayer);

        mainLayout->addWidget(new QLabel("<h4>Transform</h4>"));
        mainLayout->addWidget(createTransformWidget("Position", "0.0"));
        mainLayout->addWidget(createTransformWidget("Rotation", "0.0"));
        mainLayout->addWidget(createTransformWidget("Scale", "1.0"));

        mainLayout->addWidget(new QLabel("<h4>Terrain</h4>"));

        QLabel* pixelErrorLabel = new QLabel("Pixel Error: 50");
        QSlider* pixelErrorSlider = new QSlider(Qt::Horizontal);
        pixelErrorSlider->setRange(1, 500);
        pixelErrorSlider->setValue(50);

        QObject::connect(pixelErrorSlider, &QSlider::valueChanged, [=](int value)
        {
            pixelErrorLabel->setText(QString("Pixel Error: %1").arg(value));
        });

        mainLayout->addWidget(pixelErrorLabel);
        mainLayout->addWidget(pixelErrorSlider);

        QPushButton* addComponent = new QPushButton("Add Component");
        mainLayout->addWidget(addComponent);

        inspectorDock->setWidget(inspectorContent);
        addDockWidget(Qt::RightDockWidgetArea, inspectorDock);
    }

    void EditorApp::SetupStatusBar()
    {
        QStatusBar* statusBar = new QStatusBar(this);
        statusBar->setStyleSheet("QStatusBar { background-color: #2d2d2d; border-top: 1px solid #333333; color: #ccc; }");

        QLabel* statusReady = new QLabel("<span style='color: #00b000;'>Ready</span>");
        QLabel* statusTime = new QLabel(QDateTime::currentDateTime().toString("hh:mm AP MM/dd/yyyy"));

        QLineEdit* commandInput = new QLineEdit();
        commandInput->setPlaceholderText("Search for documentation or tools...");
        commandInput->setFixedWidth(300);
        commandInput->setStyleSheet("QLineEdit { background-color: #3a3a3a; border: 1px solid #444444; }");

        statusBar->addWidget(statusReady);
        statusBar->addWidget(commandInput);
        statusBar->addPermanentWidget(statusTime);

        this->setStatusBar(statusBar);
    }
}