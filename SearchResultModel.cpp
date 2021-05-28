#include "SearchResultModel.hpp"

SearchResultModel::SearchResultModel(QObject *parent) :
	QAbstractTableModel(parent)
{
}

QVariant SearchResultModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	// FIXME: Implement me!
}

int SearchResultModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;

	// FIXME: Implement me!
	return 0;
}

int SearchResultModel::columnCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;

	// FIXME: Implement me!
	return 0;
}

QVariant SearchResultModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	// FIXME: Implement me!
	return QVariant();
}

bool SearchResultModel::insertRows(int row, int count, const QModelIndex &parent)
{
	beginInsertRows(parent, row, row + count - 1);
	// FIXME: Implement me!
	endInsertRows();

	return false;
}

bool SearchResultModel::removeRows(int row, int count, const QModelIndex &parent)
{
	beginRemoveRows(parent, row, row + count - 1);
	// FIXME: Implement me!
	endRemoveRows();

	return false;
}
