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

	_ui->tableViewResults->setModel(_model);

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
	FileSearcher* searcher = nullptr;
	const QString location = _ui->lineEditLocation->text();
	const QStringList wildcards = _ui->lineEditWildcards->text().split('|');
	const QString searchExpression = _ui->lineEditSearch->text();
	const bool caseSensitive = _ui->checkBoxCaseSensitive->isChecked();

	if (_ui->radioButtonPlain->isChecked())
	{
		const Qt::CaseSensitivity options = caseSensitive ? Qt::CaseInsensitive : Qt::CaseInsensitive;
		searcher = new FileSearcher(nullptr, location, wildcards, searchExpression, options);
	}

	if (_ui->radioButtonRegex->isChecked())
	{
		const QRegularExpression::PatternOptions options = !caseSensitive ?
			QRegularExpression::DontCaptureOption | QRegularExpression::CaseInsensitiveOption :
			QRegularExpression::DontCaptureOption;

		const QRegularExpression regex(searchExpression, options);
		searcher = new FileSearcher(nullptr, location, wildcards, regex);
	}

	Q_ASSERT(searcher);

	connect(searcher, &FileSearcher::processing, [this](const QString& filePath)
	{
		_ui->statusbar->showMessage(QTime::currentTime().toString() + " Processing: " + filePath);
	});

	connect(searcher, &FileSearcher::finished, [=]()
	{
		_ui->statusbar->showMessage(QTime::currentTime().toString() + " Finished searching: " + location);
	});

	connect(searcher, &FileSearcher::matchFound, _model, &SearchResultModel::addMatch);

	connect(searcher, &FileSearcher::finished, searcher, &QObject::deleteLater);

	searcher->start();
}
