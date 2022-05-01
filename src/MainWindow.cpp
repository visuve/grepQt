#include "ui_MainWindow.h"

#include "MainWindow.hpp"
#include "Options.hpp"
#include "ResultModel.hpp"
#include "FileSearcher.hpp"
#include "FileReplacer.hpp"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	_ui(new Ui::MainWindow()),
	_model(new ResultModel(this)),
	_searcher(new FileSearcher(_options, this)),
	_replacer(new FileReplacer(_options, this))
{
	_ui->setupUi(this);

	_ui->actionOpen->setIcon(QApplication::style()->standardIcon(QStyle::SP_DirOpenIcon));
	connect(_ui->actionOpen, &QAction::triggered, _ui->groupBoxTarget, &TargetSelect::onOpenDirectoryDialog);

	_ui->actionExit->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogCloseButton));
	connect(_ui->actionExit, &QAction::triggered, qApp, &QApplication::quit);

	_ui->actionAbout->setIcon(QApplication::style()->standardIcon(QStyle::SP_MessageBoxInformation));
	connect(_ui->actionAbout, &QAction::triggered, this, &MainWindow::onAbout);

	_ui->actionLicenses->setIcon(QApplication::style()->standardIcon(QStyle::SP_TitleBarMenuButton));
	connect(_ui->actionLicenses, &QAction::triggered, [this]()
	{
		QMessageBox::aboutQt(this, "grepQt");
	});

	connect(_ui->pushButtonSearch, &QPushButton::clicked, this, &MainWindow::onSearch);
	connect(_ui->pushButtonReplace, &QPushButton::clicked, this, &MainWindow::onReplace);

	// auto proxyModel = new QSortFilterProxyModel(this);
	// proxyModel->setSourceModel(_model);
	_ui->treeViewResults->setModel(_model);

	connect(_ui->treeViewResults, &QTreeView::customContextMenuRequested, this, &MainWindow::createContextMenu);
	connect(_searcher, &FileSearcher::processing, this, &MainWindow::onProcessing);
	connect(_searcher, &FileSearcher::completed, this, &MainWindow::onSearchCompleted);
	connect(_searcher, &FileSearcher::matchFound, _model, &ResultModel::addResult);

	connect(_replacer, &FileReplacer::processing, this, &MainWindow::onProcessing);
	connect(_replacer, &FileReplacer::replaceCompleted, this, &MainWindow::onReplaceCompleted);
	connect(_replacer, &FileReplacer::lineReplaced, _model, &ResultModel::addResult);

	connect(_ui->groupBoxTarget, &TargetSelect::stateChanged, [this](int state)
	{
		auto previous = _state;

		if (state == UiState::Ready)
		{
			_state.set(1, true);
		}
		else
		{
			_state.set(1, false);
		}

		if (previous != _state)
		{
			update();
		}
	});

	connect(_ui->groupBoxExpression, &ExpressionSelect::stateChanged, [this](int state)
	{
		auto previous = _state;

		if (state == UiState::Ready)
		{
			_state.set(2, true);
		}
		else
		{
			_state.set(2, false);
		}

		if (previous != _state)
		{
			update();
		}
	});

	update();

#ifdef _DEBUG
	QTimer::singleShot(1500, this, &MainWindow::loadOptions);
#elif
	loadOptions();
#endif
}

MainWindow::~MainWindow()
{
	delete _ui;
	qDebug() << "Destroyed";
}

void MainWindow::onAbout()
{
	QStringList text;
	text << "grepQt - File Content Finder & Replacer version 0.1.";
	text << "";
	text << "grepQt is open source (GPLv2) and written in Qt (C++) see Licenses for more details.";
	text << "";

	QMessageBox::about(this, "grepQt", text.join('\n'));
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
	if (QMessageBox::question(
		this,
		"Replace file contents?", "Are you sure you want to replace file contents?\n",
		QMessageBox::Yes | QMessageBox::No,
		QMessageBox::No) != QMessageBox::Yes)
	{
		return;
	}

	_replacer->requestInterruption();
	_model->clear();
	_replacer->wait();
	_replacer->start();
}

void MainWindow::createContextMenu(const QPoint& pos)
{
	const QModelIndex selection = _ui->treeViewResults->indexAt(pos);

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
	menu.exec(_ui->treeViewResults->mapToGlobal(pos));
}

void MainWindow::onProcessing(const QString& filePath, int filesProcessed)
{
	const QString message = QString("%1 Processing: %2. Processed %3 files.")
		.arg(QTime::currentTime().toString())
		.arg(filePath)
		.arg(filesProcessed);

	_ui->statusbar->showMessage(message);
}

void MainWindow::onSearchCompleted(const QString& directoryPath, int filesProcessed)
{
	_ui->statusbar->clearMessage();

	const QString message = QString("%1 Finished searching: %2. Files processed: %3.")
		.arg(QTime::currentTime().toString())
		.arg(directoryPath)
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
void MainWindow::loadOptions()
{
	const QStringList args = QCoreApplication::arguments();

	if (args.count() == 2)
	{
		_options.setPath(args[1]);
	}
	_ui->groupBoxTarget->load(&_options);
	_ui->groupBoxExpression->load(&_options);
	_ui->groupBoxLimit->load(&_options);
}

void MainWindow::update()
{
	qDebug() << "State" << _state.to_ulong();

	enum InternalState : unsigned long
	{
		Snafu = 0,
		NoSearch = 2,
		NoPath = 4,
		Ready = 6
	};

	switch(_state.to_ulong())
	{
		case InternalState::Snafu:
			_ui->groupBoxExpression->setEnabled(false);
			_ui->groupBoxLimit->setEnabled(false);

			_ui->pushButtonSearch->setEnabled(false);
			_ui->pushButtonReplace->setEnabled(false);
			break;
		case InternalState::NoSearch:
			_ui->groupBoxExpression->setEnabled(true);
			_ui->groupBoxLimit->setEnabled(false);

			_ui->pushButtonSearch->setEnabled(false);
			_ui->pushButtonReplace->setEnabled(false);
			break;
		case InternalState::NoPath:
			_ui->groupBoxExpression->setEnabled(false);
			_ui->groupBoxLimit->setEnabled(false);

			_ui->pushButtonSearch->setEnabled(false);
			_ui->pushButtonReplace->setEnabled(false);
			break;
		case InternalState::Ready:
			_ui->groupBoxExpression->setEnabled(true);
			_ui->groupBoxLimit->setEnabled(true);

			_ui->pushButtonSearch->setEnabled(true);
			_ui->pushButtonReplace->setEnabled(true);
			break;
	}
}
