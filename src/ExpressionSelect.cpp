#include "ExpressionSelect.hpp"
#include "ui_ExpressionSelect.h"

ExpressionSelect::ExpressionSelect(QWidget* parent) :
	QGroupBox(parent),
	_ui(new Ui::ExpressionSelect)
{
	_ui->setupUi(this);


	connect(_ui->lineEditSearch, &QLineEdit::textChanged, this, &ExpressionSelect::onSearchExpressionChanged);
	connect(_ui->lineEditReplace, &QLineEdit::textChanged, this, &ExpressionSelect::onReplacementChanged);

	connect(_ui->radioButtonPlain, &QRadioButton::clicked, this, std::bind(&ExpressionSelect::onSearchModeChanged, this, Options::SearchMode::Plain));
	connect(_ui->radioButtonRegex, &QRadioButton::clicked, this, std::bind(&ExpressionSelect::onSearchModeChanged, this, Options::SearchMode::Regex));
	connect(_ui->checkBoxCaseSensitive, &QCheckBox::toggled, this, &ExpressionSelect::onCaseSensitivityChanged);
}

ExpressionSelect::~ExpressionSelect()
{
	delete _ui;
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
	qDebug() << value;
	_options->setSearchExpression(value);

	QPalette palette;

	if (value.isEmpty())
	{
		palette.setColor(QPalette::Window, Qt::red);
	}

	_ui->lineEditSearch->setPalette(palette);

	emit isReady(!value.isEmpty());
}

void ExpressionSelect::onReplacementChanged(const QString& value)
{
	qDebug() << value;
	_options->setReplacementText(value);
}


void ExpressionSelect::onSearchModeChanged(Options::SearchMode value)
{
	qDebug() << value;
	_options->setSearchMode(value);
}

void ExpressionSelect::onCaseSensitivityChanged(bool value)
{
	qDebug() << value;
	_options->setCaseSensitive(value);
}
