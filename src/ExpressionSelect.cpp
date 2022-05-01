#include "ExpressionSelect.hpp"
#include "ui_ExpressionSelect.h"

ExpressionSelect::ExpressionSelect(QWidget* parent) :
	QGroupBox(parent),
	_ui(new Ui::ExpressionSelect)
{
	_ui->setupUi(this);

	connect(this, &ExpressionSelect::stateChanged, [this]()
	{
		QPalette palette = this->palette();

		switch (_state)
		{
			case UiState::Empty:
				palette.setColor(QPalette::Highlight, Qt::black);
				palette.setColor(QPalette::Base, isEnabled() ? activeRed : inactiveRed);
				_ui->lineEditReplace->setEnabled(false);
				_ui->radioButtonRegex->setEnabled(false);
				_ui->radioButtonPlain->setEnabled(false);
				_ui->checkBoxCaseSensitive->setEnabled(false);
				break;
			case UiState::Ready:
				palette.setColor(QPalette::Text, isEnabled() ? activeGreen : inactiveGreen);
				_ui->lineEditReplace->setEnabled(true);
				_ui->radioButtonRegex->setEnabled(true);
				_ui->radioButtonPlain->setEnabled(true);
				_ui->checkBoxCaseSensitive->setEnabled(true);
				break;
		}

		_ui->lineEditSearch->setPalette(palette);
	});

	connect(_ui->lineEditSearch, &QLineEdit::textChanged, this, &ExpressionSelect::onSearchExpressionChanged);
	connect(_ui->lineEditReplace, &QLineEdit::textChanged, this, &ExpressionSelect::onReplacementChanged);

	connect(_ui->radioButtonPlain, &QRadioButton::clicked, this, std::bind(&ExpressionSelect::onSearchModeChanged, this, Options::SearchMode::Plain));
	connect(_ui->radioButtonRegex, &QRadioButton::clicked, this, std::bind(&ExpressionSelect::onSearchModeChanged, this, Options::SearchMode::Regex));
	connect(_ui->checkBoxCaseSensitive, &QCheckBox::toggled, this, &ExpressionSelect::onCaseSensitivityChanged);

	emit stateChanged(UiState::Empty);
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

	int previous = _state;

	if (value.isEmpty())
	{
		_state = UiState::Empty;
	}
	else
	{
		_state = UiState::Ready;
	}

	if (previous != _state)
	{
		emit stateChanged(_state);
	}
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

void ExpressionSelect::setEnabled(bool enabled)
{
	QPalette palette = _ui->lineEditSearch->palette();

	switch (_state)
	{
		case UiState::Empty:
			palette.setColor(QPalette::Base, enabled ? activeRed : inactiveRed);
			break;
		case UiState::Ready:
			palette.setColor(QPalette::Text, enabled ? activeGreen : inactiveGreen);
			break;
	}

	_ui->lineEditSearch->setPalette(palette);

	return QGroupBox::setEnabled(enabled);
}
