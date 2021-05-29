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
	void createContextMenu(const QPoint& point);
	void openFileWithDefaultAssociation(const QString& filePath);
	void openParentDirectory(const QString& filePath);

private:
	QSettings _settings;
	Ui::MainWindow* _ui;
	SearchResultModel* _model;
};
