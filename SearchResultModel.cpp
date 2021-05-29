#include "PCH.hpp"
#include "SearchResultModel.hpp"

SearchResultModel::SearchResultModel(QObject *parent) :
	QAbstractTableModel(parent)
{
}

QVariant SearchResultModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
	{
		return QVariant();
	}

	if (orientation == Qt::Orientation::Horizontal)
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

	if (orientation == Qt::Orientation::Vertical)
	{
		return ++section;
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

	const int row = index.row();

	if (role == Qt::DisplayRole)
	{
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

	if (role == Qt::WhatsThisRole)
	{
		return _matches[row].filePath;
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

void SearchResultModel::clear()
{
	beginResetModel();
	_matches.clear();
	endResetModel();
}
