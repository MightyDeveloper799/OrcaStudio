#pragma once

#ifndef WELCOME_SCREEN_H
#define WELCOME_SCREEN_H

#include <QtWidgets/QDialog>
#include <QtCore/QStringList>
#include <QtCore/QModelIndex>

namespace Orca
{
    class WelcomeScreen : public QDialog
    {
        Q_OBJECT
    public:
        explicit WelcomeScreen(QWidget* parent = nullptr);

    signals:
        void projectOpened(const QString& projectPath);

    private slots:
        void onNewProjectClicked();

        void onOpenProjectClicked();

        void onRecentProjectClicked(const QModelIndex& index);

    private:
        void SetupUI();

        QStringList LoadRecentProjectsPaths();
        void UpdateRecentProjectsList(const QString& path);

        bool CreateNewProject(const QString& path);
    };
}

#endif // WELCOME_SCREEN_H