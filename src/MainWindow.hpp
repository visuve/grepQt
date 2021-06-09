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
	void onSearchExpressionChanged(const QString& searchExpression);
	void onWildcardsChanged(const QString& text);	
	void onFileSizeOptionChanged(int index);
	void onFileTimeOptionChanged(int index);

	void onAbout();
	void onOpenDirectoryDialog();
	void onSearch();
	void onProcessing(const QString& filePath, int filesProcessed);
	void onCompleted(const QString& directoryPath, int hits, int filesProcessed);
	void createContextMenu(const QPoint& point);
	void openFileWithDefaultAssociation(const QString& filePath);
	void openParentDirectory(const QString& filePath);

	void loadSettings();
	void saveSettings();

	Ui::MainWindow* _ui;
	QSettings* _settings;
	SearchResultModel* _model;
	FileSearcher* _searcher;
};
