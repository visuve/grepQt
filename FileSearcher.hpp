#pragma once

#include <QObject>
#include <QThread>
#include <QDir>
#include <QRegularExpression>

#include <functional>

class FileSearcher : public QThread
{
	Q_OBJECT

public:
	FileSearcher(QObject* parent, const QDir& directory, const QRegularExpression& regex);
	FileSearcher(QObject* parent, const QDir& directory, QStringView searchWord, Qt::CaseSensitivity caseSensitive);
	~FileSearcher();

signals:
	void processing(const QString& filePath);

private:
	void run() override;
	const QDir _directory;
	std::function<bool(QStringView)> _matchFunction;
};
