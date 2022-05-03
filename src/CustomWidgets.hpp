#pragma once

#include <QDateTimeEdit>
#include <QLineEdit>
#include <QSpinBox>

// NOTE: the whole purpose of this stupid wrapper is
// to overcome the limitations of QValidators

template <typename WidgetType>
class LambdaWidget : public WidgetType
{
public:
	inline LambdaWidget(QWidget* parent) :
		WidgetType(parent)
	{
	}

	virtual ~LambdaWidget() = default;

	void setLambda(std::function<QValidator::State (const QVariant&)> lambda)
	{
		Q_ASSERT(_lambda == nullptr);
		_lambda = lambda;
		check();
	}

	virtual void stateChanged(QValidator::State) = 0;
	virtual void check() = 0;

protected:
	// validate is used by some widgets
	virtual void check(const QVariant& v);

	std::function<QValidator::State(const QVariant&)> _lambda;
	QValidator::State _state;
};

class CustomLineEdit : public LambdaWidget<QLineEdit>
{
	Q_OBJECT
public:
	CustomLineEdit(QWidget* parent = nullptr);

	inline void check() override
	{
		return LambdaWidget::check(text());
	}
signals:
	void stateChanged(QValidator::State) override;
};

class CustomSpinBox: public LambdaWidget<QSpinBox>
{
	Q_OBJECT
public:
	CustomSpinBox(QWidget* parent = nullptr);

	inline void check() override
	{
		return LambdaWidget::check(value());
	}
signals:
	void stateChanged(QValidator::State) override;
};

class CustomDateTimeEdit : public LambdaWidget<QDateTimeEdit>
{
	Q_OBJECT
public:
	CustomDateTimeEdit(QWidget* parent = nullptr);

	inline void check() override
	{
		return LambdaWidget::check(dateTime());
	}
signals:
	void stateChanged(QValidator::State) override;
};
