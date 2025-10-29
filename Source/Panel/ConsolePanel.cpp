#include "ConsolePanel.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QScrollBar>
#include <QtCore/QDateTime>
#include <QtCore/QDebug>

namespace Orca::Editor
{
	ConsolePanel::ConsolePanel(QWidget* parent)
		: Panel(parent)
	{
		setWindowTitle("Console");

		m_logOutput = new QTextEdit();
		m_logOutput->setReadOnly(true);	
		m_logOutput->document()->setMaximumBlockCount(10000);

		m_commandInput = new QLineEdit();
		m_commandInput->setPlaceholderText("Enter a command...");

		QVBoxLayout* layout = new QVBoxLayout(this);
		layout->setContentsMargins(5, 5, 5, 5);
		layout->addWidget(m_logOutput);
		layout->addWidget(m_commandInput);
		setLayout(layout);

		connect(m_commandInput, &QLineEdit::returnPressed,
				this, &ConsolePanel::handleCommandInput);
	}

	void ConsolePanel::logMessage(const QString& message, const QString& type = "LOG")
	{
		QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
		QString formattedMessage = QString("[%1] <%2>: %3").arg(timestamp, type, message);

		m_logOutput->append(formattedMessage);
		m_logOutput->verticalScrollBar()->setValue(m_logOutput->verticalScrollBar()->maximum());
	}

	void ConsolePanel::handleCommandInput()
	{
		QString command = m_commandInput->text().trimmed();
		if (command.isEmpty()) return;

		logMessage(command, "COMMAND");

		if (command == "clear") 
		{
			m_logOutput->clear();
			logMessage("Console cleared.", "SYSTEM");
		}
		else if (command.startsWith("echo ")) 
		{
			logMessage(command.mid(5).trimmed(), "SYSTEM");
		}
		else 
		{
			logMessage(QString("Unknown command: %1").arg(command), "ERROR");
		}

		m_commandInput->clear();
	}
}