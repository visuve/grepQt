#pragma once

namespace Ui
{
	class MainWindow;
}

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
	void onDirectoryChanged(const QString& directory);
	void onSearchExpressionChanged(const QString& searchExpression, bool isCaseSensitive);
	void onSearchExpressionChanged(const QString& searchExpression);
	void onReplacementChanged(const QString& replacement);
	void onWildcardsChanged(const QString& text);
	void onCaseSensitivityChanged(bool caseSensitive);
	void onFileSizeOptionChanged(int index);
	void onFileTimeOptionChanged(int index);

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
	void saveSettings();

	Ui::MainWindow* _ui;
	QSettings* _settings;
	SearchResultModel* _model;
	FileSearcher* _searcher;
	FileReplacer* _replacer;
};
