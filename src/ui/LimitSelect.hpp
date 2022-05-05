#pragma once

#include "Options.hpp"

#include <QGroupBox>

namespace Ui
{
	class LimitSelect;
}

class LimitSelect : public QGroupBox
{
	Q_OBJECT

public:
	explicit LimitSelect(QWidget* parent = nullptr);
	~LimitSelect();

	void load(Options* options);

	void onFileSizeOptionChanged(int);
	void onFileSizeFromChanged(int);
	void onFileSizeToChanged(int);

	void onFileTimeOptionChanged(int);
	void onFileTimeFromChanged(const QDateTime&);
	void onFileTimeToChanged(const QDateTime&);

signals:
	void isReady(bool);

private:
	Options* _options = nullptr;
	Ui::LimitSelect* _ui;
};
