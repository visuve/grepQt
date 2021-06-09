#include "PCH.hpp"
#include "./ui_MainWindow.h"

#include "MainWindow.hpp"
#include "SearchResultModel.hpp"
#include "FileSearcher.hpp"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	_settings(QSettings::IniFormat, QSettings::UserScope, "visuve", "grepQt"),
	_ui(new Ui::MainWindow()),
	_model(new SearchResultModel(this)),
	_searcher(new FileSearcher(this))
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

	connect(_ui->comboBoxFileSize, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onFileSizeOptionChanged);
	connect(_ui->comboBoxLastModified, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onFileTimeOptionChanged);

	connect(_ui->lineEditDirectory, &QLineEdit::textChanged, this, &MainWindow::onDirectoryChanged);
	connect(_ui->lineEditSearch, &QLineEdit::textChanged, this, &MainWindow::onSearchExpressionChanged);
	connect(_ui->lineEditWildcards, &QLineEdit::textChanged, this, &MainWindow::onWildcardsChanged);
	connect(_ui->toolButtonBrowse, &QToolButton::clicked, this, &MainWindow::onOpenDirectoryDialog);
	connect(_ui->pushButtonSearch, &QPushButton::clicked, this, &MainWindow::onSearch);

	auto proxyModel = new QSortFilterProxyModel(this);
	proxyModel->setSourceModel(_model);
	_ui->tableViewResults->setModel(proxyModel);

	connect(_ui->tableViewResults, &QTableView::customContextMenuRequested, this, &MainWindow::createContextMenu);
	connect(_searcher, &FileSearcher::processing, this, &MainWindow::onProcessing);
	connect(_searcher, &FileSearcher::searchCompleted, this, &MainWindow::onCompleted);
	connect(_searcher, &FileSearcher::matchFound, _model, &SearchResultModel::addMatch);

	loadSettings();
}

MainWindow::~MainWindow()
{
	saveSettings();
	delete _ui;
}

void MainWindow::onDirectoryChanged(const QString& text)
{
	QPalette palette;

	bool isDir = QFileInfo(text).isDir();

	if (!isDir)
	{
		palette.setColor(text.isEmpty() ? QPalette::Window : QPalette::Text, Qt::red);
		_ui->pushButtonSearch->setEnabled(false);
		_ui->pushButtonReplace->setEnabled(false);
	}
	else
	{
		_ui->pushButtonSearch->setEnabled(true);
		_ui->pushButtonReplace->setEnabled(true);
		_searcher->setDirectory(text);
	}

	_ui->lineEditDirectory->setPalette(palette);
}

void MainWindow::onSearchExpressionChanged(const QString& searchExpression)
{
	if (searchExpression.isEmpty())
	{
		QPalette palette;
		palette.setColor(QPalette::Window, Qt::red);
		_ui->lineEditSearch->setPalette(palette);
	}

	const bool caseSensitive = _ui->checkBoxCaseSensitive->isChecked();

	if (_ui->radioButtonPlain->isChecked())
	{
		const Qt::CaseSensitivity options = caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;

		_searcher->setMatchFunction([=](QStringView haystack)
		{
			return haystack.contains(searchExpression, options);
		});

		qDebug() << "Match function set:" << options << searchExpression << "plain";
	}

	if (_ui->radioButtonRegex->isChecked())
	{
		const QRegularExpression::PatternOptions options = !caseSensitive ?
			QRegularExpression::DontCaptureOption | QRegularExpression::CaseInsensitiveOption :
			QRegularExpression::DontCaptureOption;

		const QRegularExpression regex(searchExpression, options);

		_searcher->setMatchFunction([=](QStringView haystack)
		{
			return regex.match(haystack).hasMatch();
		});

		qDebug() << "Match function set:" << options << searchExpression << "regex";
	}
}

void MainWindow::onWildcardsChanged(const QString& text)
{
	const QStringList wildcards = text.split('|');
	_searcher->setWildcards(wildcards);
}

void MainWindow::onFileSizeOptionChanged(int index)
{
	_ui->spinBoxFileSize->setEnabled(index > 0);
}

void MainWindow::onFileTimeOptionChanged(int index)
{
	_ui->dateTimeEditLastModified->setEnabled(index > 0);
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

	const int sizeOption = _ui->comboBoxFileSize->currentIndex();
	const int sizeValue = _ui->spinBoxFileSize->value() * 1024;
	const int modifiedOption = _ui->comboBoxLastModified->currentIndex();
	const QDateTime modifiedValue = _ui->dateTimeEditLastModified->dateTime();

	_searcher->setFilterFunction([=](const QFileInfo& fileInfo)
	{
		bool sizeMatches = true;

		switch (sizeOption)
		{
			case 1:
				sizeMatches = fileInfo.size() < sizeValue;
				break;
			case 2:
				sizeMatches = fileInfo.size() > sizeValue;
				break;
			case 3:
				sizeMatches = fileInfo.size() == sizeValue;
				break;
		}

		bool lastModifiedMatches = true;

		switch (modifiedOption)
		{
			case 1:
				lastModifiedMatches = fileInfo.lastModified() < modifiedValue;
				break;
			case 2:
				lastModifiedMatches = fileInfo.lastModified() > modifiedValue;
				break;
			case 3:
				lastModifiedMatches = fileInfo.lastModified() == modifiedValue;
				break;
		}

		return sizeMatches && lastModifiedMatches;
	});

	_searcher->start();
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

void MainWindow::onCompleted(const QString& directory, int hits, int filesProcessed)
{
	_ui->statusbar->clearMessage();

	const QString message = QString("%1 Finished searching: %2. Hits: %3. Files processed: %4.")
		.arg(QTime::currentTime().toString())
		.arg(directory)
		.arg(hits)
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
	_ui->lineEditSearch->setText(_settings.value("search/word").value<QString>());
	_ui->lineEditReplace->setText(_settings.value("search/replace").value<QString>());
	_ui->radioButtonPlain->setChecked(_settings.value("search/mode").value<QString>() == "plain");
	_ui->radioButtonRegex->setChecked(_settings.value("search/mode").value<QString>() == "regex");
	_ui->checkBoxCaseSensitive->setChecked(_settings.value("search/casesensitive").value<bool>());

	_ui->lineEditWildcards->setText(_settings.value("filter/wildcards").value<QString>());
	_ui->comboBoxFileSize->setCurrentIndex(_settings.value("filter/size_opt").value<int>());
	_ui->spinBoxFileSize->setValue(_settings.value("filter/size").value<int>());
	_ui->comboBoxLastModified->setCurrentIndex(_settings.value("filter/time_opt").value<int>());
	_ui->dateTimeEditLastModified->setDateTime(_settings.value("filter/time").value<QDateTime>());

	const QStringList args = QCoreApplication::arguments();

	if (args.count() == 2)
	{
		_ui->lineEditDirectory->setText(args[1]);
	}
	else
	{
		_ui->lineEditDirectory->setText(_settings.value("path").value<QString>());
	}
}

void MainWindow::saveSettings()
{
	_settings.setValue("path", _ui->lineEditDirectory->text());
	_settings.setValue("search/word", _ui->lineEditSearch->text());
	_settings.setValue("search/replace", _ui->lineEditReplace->text());

	if (_ui->radioButtonPlain->isChecked())
	{
		_settings.setValue("search/mode", "plain");
	}

	if (_ui->radioButtonRegex->isChecked())
	{
		_settings.setValue("search/mode", "regex");
	}

	_settings.setValue("search/casesensitive", _ui->checkBoxCaseSensitive->isChecked());
	_settings.setValue("filter/wildcards", _ui->lineEditWildcards->text());
	_settings.setValue("filter/size_opt", _ui->comboBoxFileSize->currentIndex());
	_settings.setValue("filter/size", _ui->spinBoxFileSize->value());
	_settings.setValue("filter/time_opt", _ui->comboBoxLastModified->currentIndex());
	_settings.setValue("filter/time", _ui->dateTimeEditLastModified->dateTime());
}
