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

private:
	void onDirectoryChanged(const QString& text);
	void onSearchExpressionChanged(const QString& text);

	void onAbout();
	void onOpenDirectoryDialog();
	void onSearch();
	void onProcessing(const QString& filePath, int filesProcessed);
	void onCompleted(const QString& directoryPath, int hits, int filesProcessed);
	void createContextMenu(const QPoint& point);
	void openFileWithDefaultAssociation(const QString& filePath);
	void openParentDirectory(const QString& filePath);

	QSettings _settings;
	Ui::MainWindow* _ui;
	SearchResultModel* _model;
	FileSearcher* _searcher;
};
