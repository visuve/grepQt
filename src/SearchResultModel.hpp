#pragma once

#include "FileSearcher.hpp"

class SearchResultModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	explicit SearchResultModel(QObject *parent = nullptr);

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

	void addMatch(const FileSearcher::Match& match);
	void clear();

private:
	QList<FileSearcher::Match> _matches;
};

