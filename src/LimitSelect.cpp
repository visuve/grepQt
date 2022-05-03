#include "LimitSelect.hpp"
#include "ui_LimitSelect.h"

LimitSelect::LimitSelect(QWidget *parent) :
	QGroupBox(parent),
	_ui(new Ui::LimitSelect)
{
	_ui->setupUi(this);

	connect(_ui->comboBoxFileSize, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &LimitSelect::onFileSizeOptionChanged);
	connect(_ui->spinBoxSizeFrom, &QSpinBox::valueChanged, this, &LimitSelect::onFileSizeFromChanged);
	connect(_ui->spinBoxSizeTo, &QSpinBox::valueChanged, this, &LimitSelect::onFileSizeToChanged);

	connect(_ui->comboBoxLastModified, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &LimitSelect::onFileTimeOptionChanged);
	connect(_ui->dateTimeEditFrom, &QDateTimeEdit::dateTimeChanged, this, &LimitSelect::onFileTimeFromChanged);
	connect(_ui->dateTimeEditTo, &QDateTimeEdit::dateTimeChanged, this, &LimitSelect::onFileTimeToChanged);

	_ui->spinBoxSizeFrom->setLambda([&](const QVariant& v)
	{
		const int from = v.toInt();
		const int to = _ui->spinBoxSizeTo->value();

		if (from > to)
		{
			qDebug() << from << to;
			return QValidator::State::Invalid;
		}

		if (from == to)
		{
			return QValidator::State::Intermediate;
		}

		return QValidator::State::Acceptable;
	});

	_ui->spinBoxSizeTo->setLambda([&](const QVariant& v)
	{
		const int from = _ui->spinBoxSizeFrom->value();
		const int to = v.toInt();

		if (from > to)
		{
			return QValidator::State::Invalid;
		}

		if (from == to)
		{
			return QValidator::State::Intermediate;
		}

		return QValidator::State::Acceptable;
	});

	_ui->dateTimeEditFrom->setLambda([&](const QVariant& v)
	{
		const QDateTime from = v.toDateTime();
		const QDateTime to = _ui->dateTimeEditTo->dateTime();

		if (from > to)
		{
			return QValidator::State::Invalid;
		}

		if (from == to)
		{
			return QValidator::State::Intermediate;
		}

		return QValidator::State::Acceptable;
	});

	_ui->dateTimeEditTo->setLambda([&](const QVariant& v)
	{
		const QDateTime from = _ui->dateTimeEditFrom->dateTime();
		const QDateTime to = v.toDateTime();

		if (from > to)
		{
			return QValidator::State::Invalid;
		}

		if (from == to)
		{
			return QValidator::State::Intermediate;
		}

		return QValidator::State::Acceptable;
	});
}

LimitSelect::~LimitSelect()
{
	qDebug() << "Destroyed";
	delete _ui;
}

void LimitSelect::load(Options* options)
{
	_options = options;

	int x = static_cast<int>(_options->sizeFilterOption());
	_ui->comboBoxFileSize->setCurrentIndex(x);
	_ui->spinBoxSizeFrom->setValue(_options->sizeFilterFrom() / 1024);
	_ui->spinBoxSizeTo->setValue(_options->sizeFilterTo() / 1024);

	int y = static_cast<int>(_options->timeFilterOption());
	_ui->comboBoxLastModified->setCurrentIndex(y);
	_ui->dateTimeEditFrom ->setDateTime(_options->timeFilterFrom());
	_ui->dateTimeEditTo->setDateTime(_options->timeFilterTo());
}

void LimitSelect::onFileSizeOptionChanged(int index)
{
	qDebug() << index;

	switch (static_cast<Options::ComparisonOption>(index))
	{
		case Options::ComparisonOption::Irrelevant:
			_ui->spinBoxSizeFrom->setEnabled(false);
			_ui->spinBoxSizeTo->setEnabled(false);
			_options->setSizeFilterOption(Options::ComparisonOption::Irrelevant);
			break;
		case Options::ComparisonOption::Lesser:
			_ui->spinBoxSizeFrom->setEnabled(false);
			_ui->spinBoxSizeTo->setEnabled(true);
			_options->setSizeFilterOption(Options::ComparisonOption::Lesser);
			break;
		case Options::ComparisonOption::Greater:
			_ui->spinBoxSizeFrom->setEnabled(true);
			_ui->spinBoxSizeTo->setEnabled(false);
			_options->setSizeFilterOption(Options::ComparisonOption::Greater);
			break;
		case Options::ComparisonOption::Between:
			_ui->spinBoxSizeFrom->setEnabled(true);
			_ui->spinBoxSizeTo->setEnabled(true);
			_options->setSizeFilterOption(Options::ComparisonOption::Between);
			break;
	}
}
void LimitSelect::onFileSizeFromChanged(int value)
{
	_options->setSizeFilterFrom(value * 1024);
	_ui->spinBoxSizeTo->check();
}

void LimitSelect::onFileSizeToChanged(int value)
{
	_options->setSizeFilterTo(value * 1024);
	_ui->spinBoxSizeFrom->check();
}

void LimitSelect::onFileTimeOptionChanged(int index)
{
	switch (static_cast<Options::ComparisonOption>(index))
	{
		case Options::ComparisonOption::Irrelevant:
			_ui->dateTimeEditFrom->setEnabled(false);
			_ui->dateTimeEditTo->setEnabled(false);
			_options->setTimeFilterOption(Options::ComparisonOption::Irrelevant);
			break;
		case Options::ComparisonOption::Lesser:
			_ui->dateTimeEditFrom->setEnabled(false);
			_ui->dateTimeEditTo->setEnabled(true);
			_options->setTimeFilterOption(Options::ComparisonOption::Lesser);
			break;
		case Options::ComparisonOption::Greater:
			_ui->dateTimeEditFrom->setEnabled(true);
			_ui->dateTimeEditTo->setEnabled(false);
			_options->setTimeFilterOption(Options::ComparisonOption::Greater);
			break;
		case Options::ComparisonOption::Between:
			_ui->dateTimeEditFrom->setEnabled(true);
			_ui->dateTimeEditTo->setEnabled(true);
			_options->setTimeFilterOption(Options::ComparisonOption::Between);
			break;
	}
}

void LimitSelect::onFileTimeFromChanged(const QDateTime& value)
{
	_options->setTimeFilterFrom(value);
	_ui->dateTimeEditTo->check();
}

void LimitSelect::onFileTimeToChanged(const QDateTime& value)
{
	_options->setTimeFilterTo(value);
	_ui->dateTimeEditFrom->check();
}
