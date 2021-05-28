#pragma once

#include <QMainWindow>

namespace Ui
{
	class MainWindow;
}

class SearchResultModel;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private:
	Ui::MainWindow* _ui;
	SearchResultModel* _model;
};
