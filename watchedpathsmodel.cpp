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

void WatchedPathsModel::findAllFiles()
{
    allFiles.clear();

    for(auto & path: paths){
        allFiles.append(QFileInfo(path));
        findFilesInDir(path);
    }

    qDebug() <<"Finished";
    qDebug() << allFiles;
}

QFileInfoList &WatchedPathsModel::getFilesInfo()
{
    return allFiles;
}

QStringList WatchedPathsModel::getFilesPaths()
{
    QStringList files;

    for(const auto & file : allFiles){
        files.append(file.absoluteFilePath());
    }

    return files;
}

QString WatchedPathsModel::formatPath(const QString &path)
{
    QString temp{path};
    return temp.replace("file:///", "");
}

void WatchedPathsModel::findFilesInDir(QString path)
{
    qDebug()<< "Iterate: " << path;

    QDir dir(path);
    QFileInfoList fileList = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    allFiles.append(fileList);

    for(const auto & subDir : fileList){
        qDebug()<< "1: " << subDir.filePath();
        if(subDir.isDir()){
            findFilesInDir(subDir.absoluteFilePath());
        }
    }
}



