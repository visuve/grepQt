#include "PCH.hpp"
#include "./ui_MainWindow.h"

#include "MainWindow.hpp"
#include "Options.hpp"
#include "ResultModel.hpp"
#include "FileSearcher.hpp"
#include "FileReplacer.hpp"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	_ui(new Ui::MainWindow()),
	_options(new Options(this)),
	_model(new ResultModel(this)),
	_searcher(new FileSearcher(_options, this)),
	_replacer(new FileReplacer(_options, this))
{
	_ui->setupUi(this);

	_ui->actionOpen->setIcon(QApplication::style()->standardIcon(QStyle::SP_DirOpenIcon));
	connect(_ui->actionOpen, &QAction::triggered, this, &MainWindow::onOpenDirectoryDialog);

	_ui->actionExit->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogCloseButton));
	connect(_ui->actionExit, &QAction::triggered, qApp, &QApplication::quit);

	_ui->actionAbout->setIcon(QApplication::style()->standardIcon(QStyle::SP_MessageBoxInformation));
	connect(_ui->actionAbout, &QAction::triggered, this, &MainWindow::onAbout);

	_ui->actionLicenses->setIcon(QApplication::style()->standardIcon(QStyle::SP_TitleBarMenuButton));
	connect(_ui->actionLicenses, &QAction::triggered, [this]()
	{
		QMessageBox::aboutQt(this, "grepQt");
	});

	connect(_ui->lineEditDirectory, &QLineEdit::textChanged, this, &MainWindow::onDirectoryChanged);
	connect(_ui->lineEditSearch, &QLineEdit::textChanged, this, &MainWindow::onSearchExpressionChanged);
	connect(_ui->lineEditReplace, &QLineEdit::textChanged, this, &MainWindow::onReplacementChanged);
	connect(_ui->lineEditWildcards, &QLineEdit::textChanged, this, &MainWindow::onWildcardsChanged);
	connect(_ui->lineEditExcludes, &QLineEdit::textChanged, this, &MainWindow::onExcludesChanged);
	connect(_ui->radioButtonPlain, &QRadioButton::clicked, this, &MainWindow::onPlainToggled);
	connect(_ui->radioButtonRegex, &QRadioButton::clicked, this, &MainWindow::onRegexToggled);
	connect(_ui->checkBoxCaseSensitive, &QCheckBox::clicked, this, &MainWindow::onCaseSensitivityChanged);
	connect(_ui->comboBoxFileSize, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onFileSizeOptionChanged);
	connect(_ui->spinBoxFileSize, &QSpinBox::valueChanged, this, &MainWindow::onFileSizeValueChanged);
	connect(_ui->comboBoxLastModified, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onFileTimeOptionChanged);
	connect(_ui->dateTimeEditLastModified, &QDateTimeEdit::dateTimeChanged, this, &MainWindow::onFileTimeValueChanged);

	connect(_ui->toolButtonBrowse, &QToolButton::clicked, this, &MainWindow::onOpenDirectoryDialog);
	connect(_ui->pushButtonSearch, &QPushButton::clicked, this, &MainWindow::onSearch);
	connect(_ui->pushButtonReplace, &QPushButton::clicked, this, &MainWindow::onReplace);

	auto proxyModel = new QSortFilterProxyModel(this);
	proxyModel->setSourceModel(_model);
	_ui->tableViewResults->setModel(proxyModel);

	connect(_ui->tableViewResults, &QTableView::customContextMenuRequested, this, &MainWindow::createContextMenu);
	connect(_searcher, &FileSearcher::processing, this, &MainWindow::onProcessing);
	connect(_searcher, &FileSearcher::searchCompleted, this, &MainWindow::onSearchCompleted);
	connect(_searcher, &FileSearcher::matchFound, _model, &ResultModel::addResult);

	connect(_replacer, &FileReplacer::processing, this, &MainWindow::onProcessing);
	connect(_replacer, &FileReplacer::replaceCompleted, this, &MainWindow::onReplaceCompleted);
	connect(_replacer, &FileReplacer::lineReplaced, _model, &ResultModel::addResult);

	loadSettings();
}

MainWindow::~MainWindow()
{
	delete _ui;
}

void MainWindow::onDirectoryChanged(const QString& value)
{
	qDebug() << value;

	QPalette palette;

	const bool exists = QFileInfo(value).isDir();

	if (!exists)
	{
		palette.setColor(value.isEmpty() ? QPalette::Window : QPalette::Text, Qt::red);
		_ui->pushButtonSearch->setEnabled(false);
		_ui->pushButtonReplace->setEnabled(false);
	}
	else
	{
		_ui->pushButtonSearch->setEnabled(true);
		_ui->pushButtonReplace->setEnabled(true);
	}

	_ui->lineEditDirectory->setPalette(palette);
	_options->setPath(value);
}

void MainWindow::onSearchExpressionChanged(const QString& value)
{
	qDebug() << value;
	_options->setSearchExpression(value);

	QPalette palette;

	if (value.isEmpty())
	{
		palette.setColor(QPalette::Window, Qt::red);
		_ui->pushButtonSearch->setEnabled(false);
		_ui->pushButtonReplace->setEnabled(false);
	}
	else
	{
		_ui->pushButtonSearch->setEnabled(true);
		_ui->pushButtonReplace->setEnabled(true);
	}
}

void MainWindow::onReplacementChanged(const QString& value)
{
	qDebug() << value;
	_options->setReplacementText(value);
	_ui->pushButtonReplace->setEnabled(!value.isEmpty());
}

void MainWindow::onWildcardsChanged(const QString& value)
{
	qDebug() << value;
	_options->setWildcards(value.split('|'));
}

void MainWindow::onExcludesChanged(const QString& value)
{
	qDebug() << value;
	_options->setExcludes(value.split('|'));
}

void MainWindow::onPlainToggled(bool value)
{
	qDebug() << value;

	if (value)
	{
		_options->setSearchMode(Options::SearchMode::Plain);
	}
}

void MainWindow::onRegexToggled(bool value)
{
	qDebug() << value;

	if (value)
	{
		_options->setSearchMode(Options::SearchMode::Regex);
	}
}

void MainWindow::onCaseSensitivityChanged(bool value)
{
	qDebug() << value;
	_options->setCaseSensitive(value);
}

void MainWindow::onFileSizeOptionChanged(int index)
{
	qDebug() << index;
	_ui->spinBoxFileSize->setEnabled(index > 0);
	_options->setSizeFilterOption(static_cast<Options::ComparisonOption>(index));
}

void MainWindow::onFileSizeValueChanged(int value)
{
	qDebug() << value;
	_options->setSizeFilterValue(value * 1024);
}

void MainWindow::onFileTimeOptionChanged(int index)
{
	qDebug() << index;
	_ui->dateTimeEditLastModified->setEnabled(index > 0);
	_options->setTimeFilterOption(static_cast<Options::ComparisonOption>(index));
}

void MainWindow::onFileTimeValueChanged(const QDateTime& value)
{
	qDebug() << value;
	_options->setTimeFilterValue(value);
}

void MainWindow::onAbout()
{
	QStringList text;
	text << "grepQt - File Content Finder version 0.1.";
	text << "";
	text << "grepQt is yet another grep like file content searched GUI.";
	text << "";
	text << "grepQt is open source (GPLv2) and written in Qt (C++) see Licenses for more details.";
	text << "";

	QMessageBox::about(this, "grepQt", text.join('\n'));
}

void MainWindow::onOpenDirectoryDialog()
{
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::Directory);
	dialog.setOption(QFileDialog::ShowDirsOnly, true);

	if (dialog.exec() == QFileDialog::Accepted)
	{
		const QString directory = dialog.selectedFiles().first();
		_ui->lineEditDirectory->setText(QDir::toNativeSeparators(directory));
	}
}

void MainWindow::onSearch()
{
	_searcher->requestInterruption();
	_model->clear();
	_searcher->wait();
	_searcher->start();
}

void MainWindow::onReplace()
{
	_replacer->requestInterruption();
	_model->clear();
	_replacer->wait();
	_replacer->start();
}

void MainWindow::createContextMenu(const QPoint& pos)
{
	const QModelIndex selection = _ui->tableViewResults->indexAt(pos);

	if (!selection.isValid())
	{
		qDebug() << "Invalid selection";
		return;
	}

	const QVariant variant = _model->data(selection, Qt::WhatsThisRole);

	if (!variant.isValid())
	{
		qDebug() << "Invalid variant";
		return;
	}

	const QString filePath = variant.toString();

	if (filePath.isEmpty())
	{
		qDebug() << "File path is empty / no file selected";
		return;
	}

	auto openFileAction = new QAction("Open file", this);
	connect(openFileAction, &QAction::triggered, std::bind(&MainWindow::openFileWithDefaultAssociation, this, filePath));

	auto openParentDirAction = new QAction("Open parent directory", this);
	connect(openParentDirAction, &QAction::triggered, std::bind(&MainWindow::openParentDirectory, this, filePath));

	QMenu menu(this);
	menu.addActions({ openFileAction, openParentDirAction });
	menu.exec(_ui->tableViewResults->mapToGlobal(pos));
}

void MainWindow::onProcessing(const QString& filePath, int filesProcessed)
{
	const QString message = QString("%1 Processing: %2. Processed %3 files.")
		.arg(QTime::currentTime().toString())
		.arg(filePath)
		.arg(filesProcessed);

	_ui->statusbar->showMessage(message);
}

void MainWindow::onSearchCompleted(const QString& directoryPath, int hits, int filesProcessed)
{
	_ui->statusbar->clearMessage();

	const QString message = QString("%1 Finished searching: %2. Hits: %3. Files processed: %4.")
		.arg(QTime::currentTime().toString())
		.arg(directoryPath)
		.arg(hits)
		.arg(filesProcessed);

	_ui->statusbar->showMessage(message);
}


void MainWindow::onReplaceCompleted(const QString& directoryPath, int filesProcessed)
{
	_ui->statusbar->clearMessage();

	const QString message = QString("%1 Finished replacing: %2. Files processed: %3.")
		.arg(QTime::currentTime().toString())
		.arg(directoryPath)
		.arg(filesProcessed);

	_ui->statusbar->showMessage(message);
}

void MainWindow::openFileWithDefaultAssociation(const QString& filePath)
{
	const QUrl url = QUrl::fromLocalFile(filePath);

	if (!QDesktopServices::openUrl(url))
	{
		QMessageBox::warning(this, "Failed to open", "Failed to open file:\n\n" + filePath + "\n");
	}
}

void MainWindow::openParentDirectory(const QString& filePath)
{
	const QFileInfo fileInfo(filePath);
	const QUrl url = QUrl::fromLocalFile(fileInfo.dir().path());

	if (!QDesktopServices::openUrl(url))
	{
		QMessageBox::warning(this, "Failed to open", "Failed to open directory:\n\n" + filePath + "\n");
	}
}
void MainWindow::loadSettings()
{
	const QStringList args = QCoreApplication::arguments();

	if (args.count() == 2)
	{
		_options->setPath(args[1]);
	}

	_ui->lineEditDirectory->setText(_options->path());
	_ui->lineEditSearch->setText(_options->searchExpression());
	_ui->lineEditReplace->setText(_options->replacementText());
	_ui->radioButtonPlain->setChecked(_options->searchMode() == Options::SearchMode::Plain);
	_ui->radioButtonRegex->setChecked(_options->searchMode() == Options::SearchMode::Regex);
	_ui->checkBoxCaseSensitive->setChecked(_options->isCaseSensitive());

	_ui->lineEditWildcards->setText(_options->wildcards().join('|'));
	_ui->lineEditExcludes->setText(_options->excludes().join('|'));
	_ui->comboBoxFileSize->setCurrentIndex(static_cast<int>(_options->sizeFilterOption()));
	_ui->spinBoxFileSize->setValue(_options->sizeFilterValue() / 1024);
	_ui->comboBoxLastModified->setCurrentIndex(static_cast<int>(_options->timeFilterOption()));
	_ui->dateTimeEditLastModified->setDateTime(_options->timeFilterValue());
}
