#pragma once

#include "Options.hpp"

#include <QGroupBox>

namespace Ui
{
	class TargetSelect;
}

class TargetSelect : public QGroupBox
{
	Q_OBJECT

public:
	explicit TargetSelect(QWidget* parent = nullptr);
	~TargetSelect();

	void load(Options* options);

	void onOpenDirectoryDialog();
	void onDirectoryChanged(const QString&);
	void onWildcardsChanged(const QString&);
	void onExcludesChanged(const QString&);

	void onPathLineEditStateChanged(QValidator::State);

signals:
	void stateChanged(QValidator::State);

private:
	Options* _options = nullptr;
	Ui::TargetSelect* _ui;
};
