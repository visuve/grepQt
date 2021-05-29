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

	void onAbout();
	void onOpenDirectoryDialog();
	void onSearch();

private:
	QSettings _settings;
	Ui::MainWindow* _ui;
	SearchResultModel* _model;
};
