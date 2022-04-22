#pragma once

#include "Options.hpp"

namespace Ui
{
	class MainWindow;
}

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
	void onExcludesChanged(const QString&);
	void onSearchModeChanged(Options::SearchMode);
	void onCaseSensitivityChanged(bool);
	void onFileSizeOptionChanged(int);
	void onFileSizeFromChanged(int);
	void onFileSizeToChanged(int);
	void onFileEntropyToggled(bool);
	void onFileEntropyChanged(double);
	void onFileTimeOptionChanged(int);
	void onFileTimeFromChanged(const QDateTime&);
	void onFileTimeToChanged(const QDateTime&);
	void onResultModeChanged(Options::ResultMode);

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
	Options _options;
	ResultModel* _model;
	FileSearcher* _searcher;
	FileReplacer* _replacer;
};
