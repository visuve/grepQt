#include "PCH.hpp"
#include "SearchResultModel.hpp"

SearchResultModel::SearchResultModel(QObject *parent) :
	QAbstractTableModel(parent)
{
}

QVariant SearchResultModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Orientation::Horizontal && role == Qt::DisplayRole)
	{
		switch (section)
		{
			case 0:
				return "File Path";
			case 1:
				return "Line Number";
			case 2:
				return "Line Content";
		}
	}

	return QVariant();
}

int SearchResultModel::rowCount(const QModelIndex&) const
{
	return _matches.size();
}

int SearchResultModel::columnCount(const QModelIndex&) const
{
	return 3;
}

QVariant SearchResultModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
	{
		return QVariant();
	}

	if (role == Qt::DisplayRole)
	{
		const int row = index.row();
		const int col = index.column();

		switch (col)
		{
			case 0:
				return _matches[row].filePath;
			case 1:
				return _matches[row].lineNumber;
			case 2:
				return _matches[row].lineContent;
		}
	}

	return QVariant();
}

void SearchResultModel::addMatch(const FileSearcher::Match& match)
{
	qDebug() << match.toString();

	beginInsertRows(QModelIndex(), 0, 0);
	_matches.emplaceBack(match);
	endInsertRows();
}
