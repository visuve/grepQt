#pragma once

#include "FileSearcher.hpp"

class SearchResultModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	struct Result
	{
		QString filePath;
		int lineNumber;
		QString lineContent;

		inline QString toString() const
		{
			return QString("%1:%2:%3").arg(filePath).arg(lineNumber).arg(lineContent);
		}
	};

	explicit SearchResultModel(QObject *parent = nullptr);

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

	void addResult(const QString& filePath, int lineNumber, const QString& lineContent);
	void clear();

private:
	QList<Result> _results;
};

