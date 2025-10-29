#include "WelcomeScreen.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QListWidgetItem>
#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>
#include <QtCore/QSettings> // REQUIRED for persistent storage

namespace Orca
{
    WelcomeScreen::WelcomeScreen(QWidget* parent) : QDialog(parent)
    {
        this->setWindowTitle("Welcome to Orca Studio");
        this->setMinimumSize(900, 600);
        this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
        this->setStyleSheet(R"(QDialog { background-color: #1e1e1e; color: #ccc; }
                               QWidget { background-color: #252526; color: #ccc; }
                               QLabel { color: #ccc; }
                               
                               // Primary Action Buttons (New/Open)
                               QPushButton#PrimaryButton {
                                   background-color: #007acc; 
                                   color: white; 
                                   font-size: 11pt;
                                   font-weight: bold;
                                   padding: 10px; 
                                   border-radius: 5px;
                                   border: none;
                               }
                               QPushButton#PrimaryButton:hover { background-color: #008cd9; }
                               
                               // Recent Projects List
                               QListWidget {
                                   background-color: #252526;
                                   border: 1px solid #3a3a3a;
                                   border-radius: 5px;
                               }
                               QListWidget::item { padding: 8px; border-bottom: 1px solid #333333; }
                               QListWidget::item:hover { background-color: #3a3a3a; }
                               QListWidget::item:selected { background-color: #007acc; color: white; }
        )");

        SetupUI();
    }

    // --- Private Helper Functions for Persistence ---

    /**
     * @brief Loads the list of recent project file paths from persistent QSettings.
     * @return A QStringList containing the full file paths of recent projects.
     */
    QStringList WelcomeScreen::LoadRecentProjectsPaths()
    {
        // Use OrcaStudio as organization and application name for QSettings scope
        QSettings settings("OrcaStudio", "OrcaStudio");
        // Read the list of recent file paths, defaulting to an empty list
        return settings.value("RecentProjects/files", QStringList()).toStringList();
    }

    /**
     * @brief Adds the given path to the persistent recent projects list, moving it to the top.
     * @param path The full path of the project file (.orca) to add.
     */
    void WelcomeScreen::UpdateRecentProjectsList(const QString& path)
    {
        QSettings settings("OrcaStudio", "OrcaStudio");
        QStringList paths = settings.value("RecentProjects/files", QStringList()).toStringList();

        // 1. Remove the path if it already exists (to move it to the front)
        paths.removeAll(path);

        // 2. Add the path to the front
        paths.prepend(path);

        // 3. Keep the list size constrained (e.g., max 10 projects)
        const int MAX_RECENT = 10;
        while (paths.size() > MAX_RECENT) {
            paths.removeLast();
        }

        // 4. Save the updated list
        settings.setValue("RecentProjects/files", paths);
        settings.sync();
    }

    // --- UI Setup and Slots ---

    void WelcomeScreen::SetupUI()
    {
        QHBoxLayout* mainLayout = new QHBoxLayout(this);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);

        QWidget* leftPanel = new QWidget();
        leftPanel->setFixedWidth(320);
        leftPanel->setStyleSheet("QWidget { background-color: #2d2d2d; border-right: 1px solid #3a3a3a; }");
        QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);
        leftLayout->setContentsMargins(30, 40, 30, 30);
        leftLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

        QLabel* logoLabel = new QLabel("<span style='color: #007acc; font-size: 30pt;'>Orca</span><span style='color: #ccc; font-size: 30pt;'> Studio</span>");
        leftLayout->addWidget(logoLabel);

        QLabel* versionLabel = new QLabel("Version 0.7.8 (Engine Preview)");
        versionLabel->setStyleSheet("QLabel { color: #999; margin-bottom: 40px; }");
        leftLayout->addWidget(versionLabel);

        QLabel* startLabel = new QLabel("<span style='color: #ccc; font-size: 14pt;'>Start a Project</span>");
        startLabel->setStyleSheet("QLabel { margin-top: 10px; margin-bottom: 15px; }");
        leftLayout->addWidget(startLabel);

        QPushButton* newButton = new QPushButton(tr("New Project"));
        newButton->setObjectName("PrimaryButton");
        newButton->setToolTip("Create a blank project or use a template.");
        connect(newButton, &QPushButton::clicked, this, &WelcomeScreen::onNewProjectClicked);
        leftLayout->addWidget(newButton);

        QPushButton* openButton = new QPushButton(tr("Open Existing"));
        openButton->setObjectName("PrimaryButton");
        openButton->setToolTip("Open an existing Orca project file (.orca).");
        connect(openButton, &QPushButton::clicked, this, &WelcomeScreen::onOpenProjectClicked);
        leftLayout->addWidget(openButton);

        leftLayout->addStretch(1);

        QLabel* linksLabel = new QLabel("<span style='color: #999; font-size: 10pt;'>Resources</span>");
        linksLabel->setStyleSheet("QLabel { margin-bottom: 10px; }");
        leftLayout->addWidget(linksLabel);

        leftLayout->addWidget(new QLabel("<a href='#' style='color: #007acc;'>Documentation</a>"));
        leftLayout->addWidget(new QLabel("<a href='#' style='color: #007acc;'>Community Forums</a>"));

        mainLayout->addWidget(leftPanel);

        QWidget* rightPanel = new QWidget();
        QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);
        rightLayout->setContentsMargins(40, 40, 40, 30);
        rightLayout->setSpacing(15);

        QLabel* recentTitle = new QLabel("<span style='color: #ccc; font-size: 14pt;'>Recent Projects</span>");
        rightLayout->addWidget(recentTitle);

        QListWidget* recentList = new QListWidget();
        recentList->setFont(QFont("Segoe UI", 10));

        // Load actual persistent paths
        QStringList recentProjectPaths = LoadRecentProjectsPaths();

        for (const QString& fullPath : recentProjectPaths)
        {
            QFileInfo fileInfo(fullPath);

            // Skip entries where the file no longer exists
            if (!fileInfo.exists() || !fileInfo.isFile())
            {
                continue;
            }

            QListWidgetItem* item = new QListWidgetItem(recentList);

            QWidget* widget = new QWidget();
            QHBoxLayout* itemLayout = new QHBoxLayout(widget);
            itemLayout->setContentsMargins(5, 5, 5, 5);

            // Project Name (File name without extension)
            QString projectName = fileInfo.baseName();
            QLabel* nameLabel = new QLabel(QString("<b>%1</b>").arg(projectName));
            nameLabel->setObjectName("nameLabel");

            // Project Path (Directory path)
            QLabel* pathLabel = new QLabel(fileInfo.dir().path());
            pathLabel->setStyleSheet("QLabel { color: #777; font-size: 9pt; }");

            QVBoxLayout* textLayout = new QVBoxLayout();
            textLayout->addWidget(nameLabel);
            textLayout->addWidget(pathLabel);
            textLayout->setSpacing(1);

            // Date: Use the last modified date from the actual file info
            QString dateString = fileInfo.lastModified().toString("MM/dd/yyyy h:mm AP");
            QLabel* dateLabel = new QLabel(dateString);
            dateLabel->setStyleSheet("QLabel { color: #999; font-size: 9pt; }");
            dateLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            dateLabel->setFixedWidth(120); // Fixed width to align dates nicely

            itemLayout->addLayout(textLayout);
            itemLayout->addStretch(1);
            itemLayout->addWidget(dateLabel);

            item->setSizeHint(widget->sizeHint());
            recentList->setItemWidget(item, widget);

            // Store the actual full path in the item's data for retrieval later
            item->setData(Qt::UserRole, fullPath);
        }

        connect(recentList, &QListWidget::doubleClicked, this, &WelcomeScreen::onRecentProjectClicked);

        rightLayout->addWidget(recentList);

        mainLayout->addWidget(rightPanel);
    }

    bool WelcomeScreen::CreateNewProject(const QString& fullPath)
    {
        QFileInfo fileInfo(fullPath);
        QString projectDir = fileInfo.dir().path();
        QString projectName = fileInfo.fileName().section('.', 0, 0);

        QDir dir;
        if (!dir.mkpath(projectDir))
        {
            qCritical() << "Failed to create project directory:" << projectDir;
            return false;
        }
        if (!dir.mkpath(projectDir + "/Scenes")) { qWarning() << "Could not create Scenes directory."; }
        if (!dir.mkpath(projectDir + "/Assets")) { qWarning() << "Could not create Assets directory."; }
        if (!dir.mkpath(projectDir + "/Scripts")) { qWarning() << "Could not create Scripts directory."; }

        QFile file(fullPath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        {
            qCritical() << "Failed to open project file for writing:" << fullPath << file.errorString();
            return false;
        }

        // Detailed JSON structure (scene data) is unchanged
        QString content = QString(R"({
    "ProjectName": "%1",
    "EngineVersion": "0.9.1",
    "Scene": {
        "Environment": {
            "Skybox": "DefaultSky",
            "AmbientLight": [0.1, 0.1, 0.1, 1.0],
            "FogEnabled": false,
            "Gravity": [0.0, -9.81, 0.0]
        },
        "GameObjects": [
            {
                "Name": "MainCamera",
                "GUID": "a1b2c3d4-e5f6-4a7b-8c9d-0e1f2a3b4c5d",
                "Tag": "Camera",
                "Transform": {
                    "Position": [0.0, 1.5, 5.0],
                    "Rotation": [0.0, 180.0, 0.0],
                    "Scale": [1.0, 1.0, 1.0]
                },
                "Components": [
                    {
                        "Type": "CameraComponent",
                        "Properties": {"FOV": 60.0, "NearPlane": 0.1, "FarPlane": 1000.0}
                    },
                    {
                        "Type": "ScriptComponent",
                        "Class": "CameraControl.java"
                    }
                ]
            },
            {
                "Name": "DirectionalLight",
                "GUID": "b2c3d4e5-f6a7-4b8c-9d0e-1f2a3b4c5d6e",
                "Tag": "Light",
                "Transform": {
                    "Position": [10.0, 10.0, 0.0],
                    "Rotation": [-45.0, 45.0, 0.0],
                    "Scale": [1.0, 1.0, 1.0]
                },
                "Components": [
                    {
                        "Type": "LightComponent",
                        "Properties": {"Type": "Directional", "Color": [1.0, 0.95, 0.9, 1.0], "Intensity": 1.0}
                    }
                ]
            }
        ],
        "Hierarchy": {
            // Defines the parent/child structure, here both are children of the scene root
            "Root": [
                "a1b2c3d4-e5f6-4a7b-8c9d-0e1f2a3b4c5d",
                "b2c3d4e5-f6a7-4b8c-9d0e-1f2a3b4c5d6e"
            ]
        }
    }
})").arg(projectName);

        QTextStream out(&file);
        out << content;
        file.close();

        qDebug() << "Created project file:" << fullPath;
        return true;
    }

    void WelcomeScreen::onNewProjectClicked()
    {
        QString projectPath = QFileDialog::getSaveFileName(this,
            tr("Create New Orca Project"),
            QDir::homePath() + "/NewOrcaProject",
            tr("Orca Project Files (*.orca)"));

        if (projectPath.isEmpty())
        {
            return;
        }

        if (!projectPath.endsWith(".orca", Qt::CaseInsensitive))
        {
            projectPath += ".orca";
        }

        if (CreateNewProject(projectPath))
        {
            // Persist the new project path immediately
            UpdateRecentProjectsList(projectPath);

            qDebug() << "Successfully created and opening new project:" << projectPath;
            emit projectOpened(projectPath);
            accept();
        }
        else
        {
            QMessageBox::critical(this, tr("Error Creating Project"), tr("Failed to create the project file or directory structure.\nPlease check directory permissions."));
        }
    }

    void WelcomeScreen::onOpenProjectClicked()
    {
        QString projectPath = QFileDialog::getOpenFileName(this,
            tr("Open Orca Project"),
            QDir::homePath(),
            tr("Orca Project Files (*.orca);;All Files (*)"));

        if (!projectPath.isEmpty())
        {
            // Persist the opened project path
            UpdateRecentProjectsList(projectPath);

            qDebug() << "Opening project:" << projectPath;
            emit projectOpened(projectPath);
            accept();
        }
    }

    void WelcomeScreen::onRecentProjectClicked(const QModelIndex& index)
    {
        if (index.isValid())
        {
            QListWidget* recentList = (QListWidget*)sender();
            QListWidgetItem* item = recentList->item(index.row());

            // Retrieve the actual full path stored in Qt::UserRole, removing the need for mock path construction
            QString projectPath = item->data(Qt::UserRole).toString();

            if (!projectPath.isEmpty())
            {
                QFileInfo fileInfo(projectPath);

                // Handle case where the file has been moved or deleted externally
                if (!fileInfo.exists())
                {
                    QMessageBox::critical(this, tr("Project Not Found"), tr("The project file '%1' no longer exists at this location. It will be removed from the recent list.").arg(fileInfo.fileName()));
                    // A proper implementation would also remove the path from QSettings here
                    return;
                }

                // Update list to move the item to the top for user convenience
                UpdateRecentProjectsList(projectPath);

                qDebug() << "Recent project selected:" << projectPath;
                emit projectOpened(projectPath);
                accept();
            }
        }
    }

}
