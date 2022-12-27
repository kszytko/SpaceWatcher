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

    return paths.size();
}

QVariant WatchedPathsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role != Qt::DisplayRole)
        return QVariant();

    if(index.row() >= paths.size())
        return QVariant();

    return paths[index.row()];
}

Q_INVOKABLE void WatchedPathsModel::remove(int row)
{
    if(row >= paths.size())
        return;

    beginRemoveRows(QModelIndex(),row,row);

    paths.remove(row);
    qDebug() << "Removed" << row;
    endRemoveRows();
}

Q_INVOKABLE void WatchedPathsModel::append(const QString &path)
{
    beginInsertRows(QModelIndex(), paths.size(), paths.size());
    paths.append(formatPath(path));
    endInsertRows();
}

const QStringList & WatchedPathsModel::getPaths()
{
    return paths;
}


QString WatchedPathsModel::formatPath(const QString &path)
{
    QString temp{path};
    return temp.replace("file:///", "");
}






