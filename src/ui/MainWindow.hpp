#pragma once

#include "Options.hpp"

#include <bitset>

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
	void onAbout();
	void onSearch();
	void onReplace();
	void onProcessing(const QString& filePath, int filesProcessed);
	void onSearchCompleted(const QString& directoryPath, int filesProcessed);
	void onReplaceCompleted(const QString& directoryPath, int filesProcessed);
	void createContextMenu(const QPoint& point);
	void openFileWithDefaultAssociation(const QString& filePath);
	void openParentDirectory(const QString& filePath);

	void loadOptions();
	void update();

	Ui::MainWindow* _ui;
	Options _options;
	ResultModel* _model;
	FileSearcher* _searcher;
	FileReplacer* _replacer;
	std::bitset<4> _state;

signals:
	void stateChanged();
};
