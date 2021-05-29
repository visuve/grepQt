#include "PCH.hpp"
#include "./ui_MainWindow.h"

#include "MainWindow.hpp"
#include "SearchResultModel.hpp"
#include "FileSearcher.hpp"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	_ui(new Ui::MainWindow()),
	_model(new SearchResultModel(this))
{
	_ui->setupUi(this);

	connect(_ui->comboBoxFileSize, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index)
	{
		_ui->spinBoxFileSize->setEnabled(index != 0);
	});

	connect(_ui->comboBoxLastModified, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index)
	{
		_ui->dateTimeEditLastModified->setEnabled(index != 0);
	});

	connect(_ui->lineEditLocation, &QLineEdit::textChanged, [this](const QString& text)
	{
		const QFileInfo info(text);
		QPalette palette;

		if (!info.isDir())
		{
			palette.setColor(text.isEmpty() ? QPalette::Window : QPalette::Text, Qt::red);
			_ui->pushButtonSearch->setEnabled(false);
			_ui->pushButtonReplace->setEnabled(false);
		}
		else
		{
			bool hasSearchCriteria = !_ui->lineEditSearch->text().isEmpty();
			_ui->pushButtonSearch->setEnabled(hasSearchCriteria);
			_ui->pushButtonReplace->setEnabled(hasSearchCriteria);
		}

		_ui->lineEditLocation->setPalette(palette);
	});

	connect(_ui->lineEditSearch, &QLineEdit::textChanged, [this](const QString& text)
	{
		QPalette palette;

		if (text.isEmpty())
		{
			palette.setColor(QPalette::Window, Qt::red);
			_ui->pushButtonSearch->setEnabled(false);
			_ui->pushButtonReplace->setEnabled(false);
		}
		else
		{
			const QString path = _ui->lineEditLocation->text();
			const QFileInfo info(path);

			bool hasDir = info.isDir();
			_ui->pushButtonSearch->setEnabled(hasDir);
			_ui->pushButtonReplace->setEnabled(hasDir);
		}

		_ui->lineEditSearch->setPalette(palette);
	});

	connect(_ui->toolButtonBrowse, &QToolButton::clicked, this, &MainWindow::onOpenDirectoryDialog);

	_ui->tableViewResults->setModel(_model);
}

MainWindow::~MainWindow()
{
	delete _ui;
}

void MainWindow::onOpenDirectoryDialog()
{
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::Directory);
	dialog.setOption(QFileDialog::ShowDirsOnly, true);

	if (dialog.exec() == QFileDialog::Accepted)
	{
		const QString directory = dialog.selectedFiles().first();
		_ui->lineEditLocation->setText(QDir::toNativeSeparators(directory));
	}
}
