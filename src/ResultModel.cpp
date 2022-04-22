#include "ResultModel.hpp"

ResultModel::ResultModel(QObject *parent) :
	QAbstractTableModel(parent)
{
}

QVariant ResultModel::headerData(int section, Qt::Orientation orientation, int role) const
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

int ResultModel::rowCount(const QModelIndex&) const
{
	return _results.size();
}

int ResultModel::columnCount(const QModelIndex&) const
{
	return 3;
}

QVariant ResultModel::data(const QModelIndex& index, int role) const
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

void ResultModel::addResult(const QString& filePath, int lineNumber, const QString& lineContent)
{
	const Result result(filePath, lineNumber, lineContent);
	qDebug() << result.toString();

	beginInsertRows(QModelIndex(), 0, 0);
	_results.emplaceBack(result);
	endInsertRows();
}

void ResultModel::clear()
{
	beginResetModel();
	_results.clear();
	endResetModel();
}
