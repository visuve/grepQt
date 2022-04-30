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
}

TargetSelect::~TargetSelect()
{
	delete _ui;
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
	qDebug() << value;

	QPalette palette;

	const bool exists = QFileInfo(value).isDir();

	if (!exists)
	{
		palette.setColor(value.isEmpty() ? QPalette::Window : QPalette::Text, Qt::red);
	}

	_ui->lineEditDirectory->setPalette(palette);
	_options->setPath(value);

	emit isReady(exists);
}


void TargetSelect::onWildcardsChanged(const QString& value)
{
	qDebug() << value;
	_options->setWildcards(value.split('|'));
}

void TargetSelect::onExcludesChanged(const QString& value)
{
	qDebug() << value;
	_options->setExcludes(value.split('|'));
}
