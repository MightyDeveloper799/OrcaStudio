#pragma once

#ifndef CONSOLE_PANEL_H
#define CONSOLE_PANEL_H

#include "Panel.h"
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QLineEdit>

namespace Orca::Editor
{
    class ConsolePanel : public Panel 
    {
        Q_OBJECT
    public:
        explicit ConsolePanel(QWidget* parent = nullptr);
        ~ConsolePanel() override = default;

    public slots:
        /**
         * @brief Slot to receive and display a new log message.
         * @param message The log message to append.
         */
        void logMessage(const QString& message, const QString& type);

    private slots:
        /**
         * @brief Handles user pressing Enter in the command input line.
         */
        void handleCommandInput();

    private:
        QTextEdit* m_logOutput;
        QLineEdit* m_commandInput;
    };
}

#endif