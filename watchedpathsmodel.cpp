#include "watchedpathsmodel.h"
#include <QHash>
#include <QString>
#include <QDir>
#include <QDirIterator>
#include <QDebug>

WatchedPathsModel::WatchedPathsModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int WatchedPathsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_paths.size();
}

QVariant WatchedPathsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role != Qt::DisplayRole)
        return QVariant();

    if(index.row() >= m_paths.size())
        return QVariant();

    return m_paths[index.row()];
}

Q_INVOKABLE void WatchedPathsModel::remove(int row)
{
    if(row >= m_paths.size())
        return;

    beginRemoveRows(QModelIndex(),row,row);
    m_paths.remove(row);
    endRemoveRows();
}

Q_INVOKABLE void WatchedPathsModel::append(const QString &path)
{
    beginInsertRows(QModelIndex(), m_paths.size(), m_paths.size());
    m_paths.append(formatPath(path));
    endInsertRows();
}

const QStringList & WatchedPathsModel::getPaths()
{
    return m_paths;
}

bool WatchedPathsModel::isEmpty() const
{
    return m_paths.isEmpty();
}


QString WatchedPathsModel::formatPath(const QString &path)
{
    QString temp{path};
    return temp.replace("file:///", "");
}






