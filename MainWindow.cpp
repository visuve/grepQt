#include "MainWindow.hpp"
#include "./ui_MainWindow.h"
#include "SearchResultModel.hpp"
#include <QFileInfo>

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
			palette.setColor(QPalette::Text, Qt::red);
		}

		_ui->lineEditLocation->setPalette(palette);
	});

	_ui->tableViewResults->setModel(_model);
}

MainWindow::~MainWindow()
{
	delete _ui;
}

