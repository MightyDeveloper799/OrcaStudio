#include <QtWidgets/QApplication>
#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtWidgets/QMessageBox>
#include "Core/EditorApp.h"
#include "Panel/WelcomeScreen.h"
#include <Core/Logger.h>
#include <QtCore/QObject>

int main(int argc, char* argv[])
{
	QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);

	QApplication app(argc, argv);
    app.setApplicationName("Orca(R) Studio");
    app.setOrganizationName("Orca");
	app.setStyle("Fusion");

	QString projectPath = "";

	Orca::WelcomeScreen w_screen;

	QObject::connect(&w_screen, &Orca::WelcomeScreen::projectOpened,
		[&projectPath](const QString& path) {projectPath = path; });

    if(w_screen.exec() == QDialog::Accepted)
    {
        qDebug() << "Welcome Screen accepted. Opening project:" << projectPath;

        Orca::EditorApp editor;
        editor.show();

        return app.exec();
    }
    else
    {
        qDebug() << "Welcome Screen cancelled. Exiting application.";
        return 0;
    }

	return app.exec();
}