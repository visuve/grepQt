#pragma once

namespace Ui
{
	class MainWindow;
}

class Options;
class ResultModel;
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
	void onPlainToggled(bool);
	void onRegexToggled(bool);
	void onCaseSensitivityChanged(bool);
	void onFileSizeOptionChanged(int);
	void onFileSizeValueChanged(int);
	void onFileTimeOptionChanged(int);
	void onFileTimeValueChanged(const QDateTime&);

	void onAbout();
	void onOpenDirectoryDialog();
	void onSearch();
	void onReplace();
	void onProcessing(const QString& filePath, int filesProcessed);
	void onSearchCompleted(const QString& directoryPath, int hits, int filesProcessed);
	void onReplaceCompleted(const QString& directoryPath, int filesProcessed);
	void createContextMenu(const QPoint& point);
	void openFileWithDefaultAssociation(const QString& filePath);
	void openParentDirectory(const QString& filePath);
	void loadSettings();

	Ui::MainWindow* _ui;
	Options* _options;
	ResultModel* _model;
	FileSearcher* _searcher;
	FileReplacer* _replacer;
};
