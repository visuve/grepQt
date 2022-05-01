#include "CustomLineEdit.hpp"

CustomLineEdit::CustomLineEdit(QWidget *parent) :
	QLineEdit(parent)
{
}

void CustomLineEdit::setLambda(std::function<QValidator::State(const QString&)> lambda)
{
	Q_ASSERT(_lambda == nullptr);

	_lambda = lambda;

	validate(text());

	connect(this, &QLineEdit::textChanged, this, &CustomLineEdit::validate);
}

QValidator::State CustomLineEdit::state()
{
	return _state;
}

void CustomLineEdit::validate(const QString& text)
{
	auto previous = _state;
	_state = _lambda(text);

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

	setPalette(palette);
	emit stateChanged(_state);
}
