#pragma once

namespace Ui
{
	class MainWindow;
}

class Options;
class SearchResultModel;
class FileSearcher;
class FileReplacer;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private:
	void onDirectoryChanged(const QString&);
	void onSearchExpressionChanged(const QString&);
	void onReplacementChanged(const QString&);
	void onWildcardsChanged(const QString&);
	void onCaseSensitivityChanged(bool);
	void onFileSizeOptionChanged(int);
	void onFileTimeOptionChanged(int);

	void onAbout();
	void onOpenDirectoryDialog();
	void onSearch();
	void onReplace();
	void onProcessing(const QString& filePath, int filesProcessed);
	void onCompleted(const QString& directoryPath, int hits, int filesProcessed);
	void createContextMenu(const QPoint& point);
	void openFileWithDefaultAssociation(const QString& filePath);
	void openParentDirectory(const QString& filePath);
	void loadSettings();

	Ui::MainWindow* _ui;
	Options* _options;
	SearchResultModel* _model;
	FileSearcher* _searcher;
	FileReplacer* _replacer;
};
