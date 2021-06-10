#pragma once

#include "FileSearcher.hpp"

class ResultModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	struct Result
	{
		QString filePath;
		int lineNumber;
		QString lineContent;

		inline Result(const QString& path, int line, const QString& content) :
			filePath(QDir::toNativeSeparators(path)),
			lineNumber(line),
			lineContent(content)
		{

		}

		inline QString toString() const
		{
			return QString("%1:%2:%3").arg(filePath).arg(lineNumber).arg(lineContent);
		}
	};

	explicit ResultModel(QObject *parent = nullptr);

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

	void addResult(const QString& filePath, int lineNumber, const QString& lineContent);
	void clear();

private:
	QList<Result> _results;
};

