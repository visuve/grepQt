#include "PCH.hpp"
#include "./ui_MainWindow.h"

#include "MainWindow.hpp"
#include "SearchResultModel.hpp"
#include "FileSearcher.hpp"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	_ui(new Ui::MainWindow()),
	_model(new SearchResultModel(this)),
	_settings(QSettings::IniFormat, QSettings::UserScope, "visuve", "grepQt")
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

	connect(_ui->pushButtonSearch, &QPushButton::clicked, this, &MainWindow::onSearch);

	auto proxyModel = new QSortFilterProxyModel(this);
	proxyModel->setSourceModel(_model);
	_ui->tableViewResults->setModel(proxyModel);

	connect(_ui->tableViewResults, &QTableView::customContextMenuRequested, this, &MainWindow::createContextMenu);

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

	if (args.count() == 2 && QDir().exists(args[1]))
	{
		const QString& path = args[1];
		_ui->lineEditLocation->setText(path);
	}
	else
	{
		_ui->lineEditLocation->setText(_settings.value("path").value<QString>());
	}
}

MainWindow::~MainWindow()
{
	_settings.setValue("path", _ui->lineEditLocation->text());
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

	delete _ui;
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
		_ui->lineEditLocation->setText(QDir::toNativeSeparators(directory));
	}
}

void MainWindow::onSearch()
{
	_model->clear();

	FileSearcher* searcher = nullptr;
	const QString location = _ui->lineEditLocation->text();
	const QStringList wildcards = _ui->lineEditWildcards->text().split('|');
	const QString searchExpression = _ui->lineEditSearch->text();
	const bool caseSensitive = _ui->checkBoxCaseSensitive->isChecked();
	const int sizeOption = _ui->comboBoxFileSize->currentIndex();
	const int sizeValue = _ui->spinBoxFileSize->value() * 1024;
	const int modifiedOption = _ui->comboBoxLastModified->currentIndex();
	const QDateTime modifiedValue = _ui->dateTimeEditLastModified->dateTime();

	std::function<bool(QStringView)> searchFunction;
	std::function<bool(QFileInfo)> filterFunction;

	if (_ui->radioButtonPlain->isChecked())
	{
		const Qt::CaseSensitivity options = caseSensitive ? Qt::CaseInsensitive : Qt::CaseInsensitive;

		searchFunction = [=](QStringView haystack)
		{
			return haystack.contains(searchExpression, options);
		};
	}

	if (_ui->radioButtonRegex->isChecked())
	{
		const QRegularExpression::PatternOptions options = !caseSensitive ?
			QRegularExpression::DontCaptureOption | QRegularExpression::CaseInsensitiveOption :
			QRegularExpression::DontCaptureOption;

		const QRegularExpression regex(searchExpression, options);

		searchFunction = [=](QStringView haystack)
		{
			return regex.match(haystack).hasMatch();
		};
	}

	filterFunction = [=](const QFileInfo& fileInfo)
	{
		switch (sizeOption)
		{
			case 1:
				return fileInfo.size() < sizeValue;
			case 2:
				return fileInfo.size() > sizeValue;
			case 3:
				return fileInfo.size() == sizeValue;
		}

		switch (modifiedOption)
		{
			case 1:
				return fileInfo.lastModified() < modifiedValue;
			case 2:
				return fileInfo.lastModified() > modifiedValue;
			case 3:
				return fileInfo.lastModified() == modifiedValue;
		}

		return true;
	};

	searcher = new FileSearcher(this, location, wildcards, searchFunction, filterFunction);

	connect(searcher, &FileSearcher::processing, [this](const QString& filePath, int filesProcessed)
	{
		QString message = QString("%1 Processing: %2. Processed %3 files.")
			.arg(QTime::currentTime().toString())
			.arg(filePath)
			.arg(filesProcessed);

		_ui->statusbar->showMessage(message);
	});

	connect(searcher, &FileSearcher::searchCompleted, [=](int hits, int filesProcessed)
	{
		QString message = QString("%1 Finished searching: %2. Hits: %3. Files processed: %4.")
			.arg(QTime::currentTime().toString())
			.arg(location)
			.arg(hits)
			.arg(filesProcessed);

		_ui->statusbar->showMessage(message);
	});

	connect(searcher, &FileSearcher::matchFound, _model, &SearchResultModel::addMatch);

	connect(searcher, &FileSearcher::finished, searcher, &QObject::deleteLater);

	searcher->start();
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
