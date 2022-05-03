#include "CustomWidgets.hpp"

template<typename WidgetType>
void LambdaWidget<WidgetType>::check(const QVariant& v)
{
	if (!_lambda)
	{
		return;
	}

	const QValidator::State previous = _state;
	_state = _lambda(v);

	if (previous == _state)
	{
		return;
	}

	QPalette palette;

	switch(_state)
	{
		case QValidator::Invalid:
			palette.setColor(QPalette::Highlight, errorHighlight);
			palette.setColor(QPalette::Active, QPalette::Base, activeRed);
			palette.setColor(QPalette::Disabled, QPalette::Base, inactiveRed);
			break;
		case QValidator::Intermediate:
			palette.setColor(QPalette::Active, QPalette::Highlight, activeRed);
			palette.setColor(QPalette::Disabled, QPalette::Highlight, inactiveRed);
			palette.setColor(QPalette::Active, QPalette::Text, activeRed);
			palette.setColor(QPalette::Disabled, QPalette::Text, inactiveRed);
			break;
		case QValidator::Acceptable:
			palette.setColor(QPalette::Active, QPalette::Text, activeGreen);
			palette.setColor(QPalette::Disabled, QPalette::Text, inactiveGreen);
			break;
	}

	WidgetType::setPalette(palette);
	stateChanged(_state);
}


CustomLineEdit::CustomLineEdit(QWidget* parent) :
	LambdaWidget<QLineEdit>(parent)
{
	QObject::connect(this, &QLineEdit::textChanged, this, &CustomLineEdit::check);
}


CustomSpinBox::CustomSpinBox(QWidget* parent) :
	LambdaWidget<QSpinBox>(parent)
{
	QObject::connect(this, &QSpinBox::valueChanged, this, &CustomSpinBox::check);
}


CustomDateTimeEdit::CustomDateTimeEdit(QWidget* parent) :
	LambdaWidget<QDateTimeEdit>(parent)
{
	QObject::connect(this, &QDateTimeEdit::dateTimeChanged, this, &CustomDateTimeEdit::check);
}
