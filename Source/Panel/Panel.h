#pragma once

#include <QtWidgets/QWidget>
#include <QtCore/QString>
#include <memory>

namespace Orca { class Scene; }

namespace Orca::Editor
{
	class Panel : public QWidget
	{
		Q_OBJECT
	public:
		QString name;

	public:
		explicit Panel(const QString& panelName, QWidget* parent = nullptr);

		virtual ~Panel() override = default;

		virtual QWidget* GetWidget() = 0;

		virtual void Update(float deltaTime) {}
		virtual void SetScene(const std::shared_ptr<Orca::Scene>& scene) {}
	};
}