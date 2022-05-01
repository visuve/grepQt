#include "TargetSelect.hpp"
#include "ui_TargetSelect.h"

TargetSelect::TargetSelect(QWidget *parent) :
	QGroupBox(parent),
	_ui(new Ui::TargetSelect)
{
	_ui->setupUi(this);

	connect(_ui->toolButtonBrowse, &QToolButton::clicked, this, &TargetSelect::onOpenDirectoryDialog);
	connect(_ui->lineEditDirectory, &QLineEdit::textChanged, this, &TargetSelect::onDirectoryChanged);
	connect(_ui->lineEditWildcards, &QLineEdit::textChanged, this, &TargetSelect::onWildcardsChanged);
	connect(_ui->lineEditExcludes, &QLineEdit::textChanged, this, &TargetSelect::onExcludesChanged);

	_ui->lineEditDirectory->setLambda([](const QString& raw)
	{
		if (raw.isEmpty())
		{
			return QValidator::Invalid;
		}

		if (QDir(raw).exists())
		{
			return QValidator::Acceptable;
		}

		return QValidator::Intermediate;
	});

	_ui->lineEditWildcards->setLambda([](const QString& raw)
	{
		if (raw.isEmpty())
		{
			return QValidator::Acceptable;
		}

		const QStringList values = raw.split('|');

		for (const QString& value : values)
		{
			if (value.isEmpty() || values.count(value) > 1)
			{
				qWarning() << "Duplicate or empty";
				return QValidator::Invalid;
			}

			if (!value.contains('*') && !value.contains('?'))
			{
				qWarning() << "Not a proper wildcard";
				return QValidator::Intermediate;
			}
		}

		return QValidator::Acceptable;
	});

	_ui->lineEditExcludes->setLambda([](const QString& raw)
	{
		if (raw.isEmpty())
		{
			return QValidator::Acceptable;
		}

		const QStringList values = raw.split('|');

		for (const QString& value : values)
		{
			if (value.isEmpty() || values.count(value) > 1)
			{
				qWarning() << "Duplicate or empty";
				return QValidator::Invalid;
			}

			if (value.contains('*') || value.contains('?'))
			{
				qWarning() << "Wildcards not accepted here";
				return QValidator::Intermediate;
			}
		}

		return QValidator::Acceptable;
	});

	connect(_ui->lineEditDirectory, &CustomLineEdit::stateChanged, this, &TargetSelect::onPathLineEditStateChanged);
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
}

void TargetSelect::onWildcardsChanged(const QString& value)
{
	_options->setWildcards(value.split('|'));
}

void TargetSelect::onExcludesChanged(const QString& value)
{
	_options->setExcludes(value.split('|'));
}

void TargetSelect::onPathLineEditStateChanged(QValidator::State state)
{
	switch (state)
	{
		case QValidator::State::Invalid:
		case QValidator::State::Intermediate:
			_ui->lineEditWildcards->setEnabled(false);
			_ui->lineEditExcludes->setEnabled(false);
			break;
		case QValidator::State::Acceptable:
			_ui->lineEditWildcards->setEnabled(true);
			_ui->lineEditExcludes->setEnabled(true);
			break;
	}

	emit stateChanged(state);
}
