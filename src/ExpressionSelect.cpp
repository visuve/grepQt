#include "ExpressionSelect.hpp"
#include "ui_ExpressionSelect.h"

ExpressionSelect::ExpressionSelect(QWidget* parent) :
	QGroupBox(parent),
	_ui(new Ui::ExpressionSelect)
{
	_ui->setupUi(this);

	QPalette palette =_ui->lineEditReplace->palette();
	palette.setColor(QPalette::Active, QPalette::Text, activeGreen);
	palette.setColor(QPalette::Disabled, QPalette::Text, inactiveGreen);
	_ui->lineEditReplace->setPalette(palette);

	connect(_ui->lineEditSearch, &QLineEdit::textChanged, this, &ExpressionSelect::onSearchExpressionChanged);
	connect(_ui->lineEditReplace, &QLineEdit::textChanged, this, &ExpressionSelect::onReplacementChanged);

	connect(_ui->radioButtonPlain, &QRadioButton::clicked, this, std::bind(&ExpressionSelect::onSearchModeChanged, this, Options::SearchMode::Plain));
	connect(_ui->radioButtonRegex, &QRadioButton::clicked, this, std::bind(&ExpressionSelect::onSearchModeChanged, this, Options::SearchMode::Regex));
	connect(_ui->checkBoxCaseSensitive, &QCheckBox::toggled, this, &ExpressionSelect::onCaseSensitivityChanged);

	_ui->lineEditSearch->setLambda([](const QString& raw)
	{
		return raw.isEmpty()? QValidator::Invalid : QValidator::Acceptable;
	});

	connect(_ui->lineEditSearch, &CustomLineEdit::stateChanged, this, &ExpressionSelect::onLineEditSearchStateChanged);
}

ExpressionSelect::~ExpressionSelect()
{
	delete _ui;

	qDebug() << "Destroyed";
}

void ExpressionSelect::load(Options* options)
{
	_options = options;

	_ui->lineEditSearch->setText(_options->searchExpression());
	_ui->lineEditReplace->setText(_options->replacementText());

	_ui->radioButtonPlain->setChecked(_options->searchMode() == Options::SearchMode::Plain);
	_ui->radioButtonRegex->setChecked(_options->searchMode() == Options::SearchMode::Regex);
	_ui->checkBoxCaseSensitive->setChecked(_options->isCaseSensitive());
}

void ExpressionSelect::onSearchExpressionChanged(const QString& value)
{
	_options->setSearchExpression(value);
}

void ExpressionSelect::onReplacementChanged(const QString& value)
{
	_options->setReplacementText(value);
}

void ExpressionSelect::onSearchModeChanged(Options::SearchMode value)
{
	_options->setSearchMode(value);
}

void ExpressionSelect::onCaseSensitivityChanged(bool value)
{
	_options->setCaseSensitive(value);
}

void ExpressionSelect::onLineEditSearchStateChanged(QValidator::State state)
{
	switch (state)
	{
		case QValidator::State::Invalid:
		case QValidator::State::Intermediate:
			_ui->lineEditReplace->setEnabled(false);
			_ui->radioButtonRegex->setEnabled(false);
			_ui->radioButtonPlain->setEnabled(false);
			_ui->checkBoxCaseSensitive->setEnabled(false);
			break;
		case QValidator::State::Acceptable:
			_ui->lineEditReplace->setEnabled(true);
			_ui->radioButtonRegex->setEnabled(true);
			_ui->radioButtonPlain->setEnabled(true);
			_ui->checkBoxCaseSensitive->setEnabled(true);
			break;
	}

	emit stateChanged(state);
}
