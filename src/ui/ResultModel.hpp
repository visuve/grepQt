#pragma once

class Node;

class ResultModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	explicit ResultModel(QObject *parent = nullptr);
	~ResultModel();

	QModelIndex index(int row, int column, const QModelIndex& parentIndex = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex& childIndex) const override;

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	void addResult(const QString& filePath, int lineNumber, const QString& lineContent);
	void clear();

private:
	Node* _root;
};
