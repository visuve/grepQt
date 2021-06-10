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

	return QVariant();
}

int SearchResultModel::rowCount(const QModelIndex&) const
{
	return _results.size();
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
				return _results[row].filePath;
			case 1:
				return _results[row].lineNumber;
			case 2:
				return _results[row].lineContent;
		}
	}

	if (role == Qt::WhatsThisRole)
	{
		return _results[row].filePath;
	}

	return QVariant();
}

void SearchResultModel::addResult(const QString& filePath, int lineNumber, const QString& lineContent)
{
	const Result result = {filePath, lineNumber, lineContent};
	qDebug() << result.toString();

	beginInsertRows(QModelIndex(), 0, 0);
	_results.emplaceBack(result);
	endInsertRows();
}

void SearchResultModel::clear()
{
	beginResetModel();
	_results.clear();
	endResetModel();
}
