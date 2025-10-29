#pragma once

#ifndef EDITOR_APP_H
#define EDITOR_APP_H

#include <QtWidgets/QMainWindow>

namespace Orca
{
	class EditorApp : public QMainWindow
	{
	public:
		explicit EditorApp(QWidget* parent = nullptr);

	private:
		void ApplyDarkTheme();
		void SetupMenuBar();
		void SetupLeftDocks();
		void SetupRightDock();
		void SetupStatusBar();
	};
}

#endif