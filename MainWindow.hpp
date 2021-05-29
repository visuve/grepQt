#pragma once

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

	void onOpenDirectoryDialog();
	void onSearch();

private:
	Ui::MainWindow* _ui;
	SearchResultModel* _model;
};
