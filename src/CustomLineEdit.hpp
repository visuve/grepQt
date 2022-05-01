#pragma once

#include <QLineEdit>

class CustomLineEdit : public QLineEdit
{
	Q_OBJECT

public:
	CustomLineEdit(QWidget *parent = nullptr);

	void setLambda(std::function<QValidator::State(const QString&)> lambda);

	QValidator::State state();

signals:
	void stateChanged(QValidator::State);

private:
	void validate(const QString& text);

	std::function<QValidator::State(const QString&)> _lambda = nullptr;
	QValidator::State _state;
};
