#include "TargetSelect.hpp"
#include "ui_TargetSelect.h"

TargetSelect::TargetSelect(QWidget *parent) :
	QGroupBox(parent),
	_ui(new Ui::TargetSelect)
{
	_ui->setupUi(this);

	connect(this, &TargetSelect::stateChanged, [this]()
	{
		QPalette palette =this->palette();

		switch (_state)
		{
			case UiState::Empty:
				palette.setColor(QPalette::Highlight, Qt::black);
				palette.setColor(QPalette::Base, isEnabled() ? activeRed : inactiveRed);

				_ui->lineEditWildcards->setEnabled(false);
				_ui->lineEditExcludes->setEnabled(false);
				break;
			case UiState::Invalid:
				palette.setColor(QPalette::Highlight, isEnabled() ? activeRed : inactiveRed);
				palette.setColor(QPalette::Text, isEnabled() ? activeRed : inactiveRed);
				_ui->lineEditWildcards->setEnabled(false);
				_ui->lineEditExcludes->setEnabled(false);
				break;
			case UiState::Ready:
				palette.setColor(QPalette::Text, isEnabled() ? activeGreen : inactiveGreen);
				_ui->lineEditWildcards->setEnabled(true);
				_ui->lineEditExcludes->setEnabled(true);
				break;
		}

		_ui->lineEditDirectory->setPalette(palette);
	});

	connect(_ui->toolButtonBrowse, &QToolButton::clicked, this, &TargetSelect::onOpenDirectoryDialog);
	connect(_ui->lineEditDirectory, &QLineEdit::textChanged, this, &TargetSelect::onDirectoryChanged);
	connect(_ui->lineEditWildcards, &QLineEdit::textChanged, this, &TargetSelect::onWildcardsChanged);
	connect(_ui->lineEditExcludes, &QLineEdit::textChanged, this, &TargetSelect::onExcludesChanged);

	emit stateChanged(UiState::Empty);
}

TargetSelect::~TargetSelect()
{
	delete _ui;
	qDebug() << "Destroyed";
}

void TargetSelect::load(Options* options)
{
	_options = options;

	_ui->lineEditDirectory->setText(_options->path());
	_ui->lineEditWildcards->setText(_options->wildcards().join('|'));
	_ui->lineEditExcludes->setText(_options->excludes().join('|'));
}

void TargetSelect::onOpenDirectoryDialog()
{
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::Directory);
	dialog.setOption(QFileDialog::ShowDirsOnly, true);

	if (dialog.exec() != QFileDialog::Accepted)
	{
		return;
	}

	const QString directory = dialog.selectedFiles().first();

	_ui->lineEditDirectory->setText(QDir::toNativeSeparators(directory));
}

void TargetSelect::onDirectoryChanged(const QString& value)
{
	_options->setPath(value);

	int previous = _state;;

	if (value.isEmpty())
	{
		_state = UiState::Empty;
	}
	else if (!QDir(value).exists())
	{
		_state = UiState::Invalid;
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

void TargetSelect::onWildcardsChanged(const QString& value)
{
	_options->setWildcards(value.split('|'));
}

void TargetSelect::onExcludesChanged(const QString& value)
{
	_options->setExcludes(value.split('|'));
}

void TargetSelect::setEnabled(bool enabled)
{
	QPalette palette = _ui->lineEditDirectory->palette();

	switch (_state)
	{
		case UiState::Empty:
			palette.setColor(QPalette::Base, enabled ? activeRed : inactiveRed);
			break;
		case UiState::Ready:
			palette.setColor(QPalette::Text, enabled ? activeGreen : inactiveGreen);
			break;
	}

	_ui->lineEditDirectory->setPalette(palette);

	return QGroupBox::setEnabled(enabled);
}
