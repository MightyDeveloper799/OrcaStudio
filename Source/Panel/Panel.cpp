#include "Panel.h"

namespace Orca::Editor
{
	Panel::Panel(const QString& panelName, QWidget* parent)
		: name(panelName), QWidget(parent)
	{
        setStyleSheet(R"(
            QWidget {
                background-color: #2e2e2e;
                color: #dcdcdc; /* Light gray text */
                border: 1px solid #3c3c3c;
                border-radius: 4px;
            }
        )");
	}
}