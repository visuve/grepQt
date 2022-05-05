#include "ResultModel.hpp"

inline Node* indexToNode(const QModelIndex& index)
{
	return static_cast<Node*>(index.internalPointer());
}

class Node
{
public:
	Node(Node* parent, const QMap<Qt::ItemDataRole, QVariantList>& data) :
		_parent(parent),
		_data(data)
	{
	}

	~Node()
	{
		qDeleteAll(_children);
		qDebug() << _data[Qt::DisplayRole];
	}

	Node* parent() const
	{
		return _parent;
	}

	Node* childAt(int index) const
	{
		return _children.at(index);
	}

	Node* takeChild(int index)
	{
		return _children.takeAt(index);
	}

	QVector<Node*> takeChildren(const std::function<bool(Node*)>& lambda)
	{
		QVector<Node*> result;

		for (int i = 0; i < childCount(); ++i)
		{
			if (lambda(childAt(i)))
			{
				result.append(takeChild(i));
			}
		}

		return result;
	}

	Node* appendChild(const QMap<Qt::ItemDataRole, QVariantList>& data)
	{
		Node* child = new Node(this, data);
		_children.append(child);
		return child;
	}

	QVector<Node*> findChildren(const std::function<bool(Node*)>& lambda) const
	{
		QVector<Node*> results;
		QQueue<Node*> queue;
		queue.enqueue(const_cast<Node*>(this));

		while (!queue.empty())
		{
			Node* node = queue.dequeue();

			for (Node* child : node->_children)
			{
				queue.enqueue(child);
			}

			if (lambda(node))
			{
				results.push_back(node);
			}
		}

		return results;
	}

	Node* findChild(const std::function<bool(Node*)>& lambda) const
	{
		auto children = findChildren(lambda);
		return children.isEmpty() ? nullptr : children.first();
	}

	int parentRow() const
	{
		return _parent ? _parent->_children.indexOf(const_cast<Node*>(this)) : 0;
	}

	int childCount() const
	{
		return _children.size();
	}

	bool hasChildren() const
	{
		return !_children.isEmpty();
	}

	const QVariant& data(Qt::ItemDataRole role, int column) const
	{
		return _data[role].at(column);
	}

private:
	Node* _parent;
	QVector<Node*> _children;
	QMap<Qt::ItemDataRole, QVariantList> _data;
};


ResultModel::ResultModel(QObject *parent) :
	QAbstractItemModel(parent),
	_root(new Node(nullptr, { { Qt::DisplayRole, { "root" } } }))
{
}

ResultModel::~ResultModel()
{
	delete _root;
	qDebug() << "Destroyed";
}

QModelIndex ResultModel::index(int row, int column, const QModelIndex& parentIndex) const
{
	if (!hasIndex(row, column, parentIndex))
	{
		return QModelIndex();
	}

	Node* parent= parentIndex.isValid() ? indexToNode(parentIndex) : _root;
	Node* child = parent ->childAt(row);
	return child ? createIndex(row, column, child) : QModelIndex();
}

QModelIndex ResultModel::parent(const QModelIndex& childIndex) const
{
	if (!childIndex.isValid())
	{
		return QModelIndex();
	}

	Node* parentNode = indexToNode(childIndex)->parent();

	return parentNode != _root ?
		createIndex(parentNode->parentRow(), 0, parentNode) :
		QModelIndex();
}

int ResultModel::rowCount(const QModelIndex& parentIndex) const
{
	return parentIndex.isValid() ?
		indexToNode(parentIndex)->childCount() :
		_root->childCount();
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

	Node* item = indexToNode(index);
	Node* parent = item->parent();

	bool topLevel = parent == _root;
	int column = index.column();
	bool isParent = column == 0 && topLevel;
	bool isChild = column >= 1 && !topLevel;
	bool isPath = column == 0 && !topLevel;

	if (role == Qt::DisplayRole)
	{
		if (isPath)
		{
			// TODO: this is very ugly
			return QFileInfo(parent->data(Qt::DisplayRole, 0).toString()).fileName();
		}

		if (isParent)
		{
			return item->data(Qt::DisplayRole, column);
		}

		if (isChild)
		{
			return item->data(Qt::DisplayRole, column - 1);
		}
	}

	return QVariant();
}

QVariant ResultModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
	{
		return QVariant();
	}

	switch (section)
	{
		case 0:
			return "Path";
		case 1:
			return "Line";
		case 2:
			return "Content";
	}

	return QVariant();
}

void ResultModel::addResult(const QString& filePath, int line, const QString& content)
{
	Node* pathNode = _root->findChild([&](Node* node)
	{
		return node->data(Qt::DisplayRole, 0).toString() == filePath;
	});

	if (!pathNode)
	{
		pathNode = _root->appendChild({ { Qt::DisplayRole, { filePath } } });
	}

	beginInsertRows(QModelIndex(), 0, 1);
	pathNode->appendChild({ { Qt::DisplayRole, { line, content }} });
	endInsertRows();
}

void ResultModel::clear()
{
	beginResetModel();
	delete _root;
	_root = new Node(nullptr, { { Qt::DisplayRole, { "root" } } });
	endResetModel();
}
