#pragma once

namespace Ui
{
	class MainWindow;
}

class SearchResultModel;
class FileSearcher;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

	void onOpenDirectoryDialog();

private:
	Ui::MainWindow* _ui;
	SearchResultModel* _model;
	FileSearcher* _searcher;
};
