#pragma once

#include "Options.hpp"

#include <QGroupBox>

namespace Ui
{
	class ExpressionSelect;
}

class ExpressionSelect : public QGroupBox
{
	Q_OBJECT

public:
	explicit ExpressionSelect(QWidget *parent = nullptr);
	~ExpressionSelect();

	void load(Options* options);

	void onSearchExpressionChanged(const QString&);
	void onReplacementChanged(const QString&);

	void onSearchModeChanged(Options::SearchMode);
	void onCaseSensitivityChanged(bool);

signals:
	void isReady(bool);

private:
	Options* _options = nullptr;
	Ui::ExpressionSelect* _ui;
};
