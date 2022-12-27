#include "filewatchercontroller.h"
#include <QDebug>
#include <QFileInfo>

FileWatcherController::FileWatcherController(QObject *parent)
    : QObject{parent}
{
    pathsModel = new WatchedPathsModel();
    scannedModel = new ScannedDataModel();
    watcher = new QFileSystemWatcher();
}

void FileWatcherController::clearTable()
{
    qDebug()<< "Clear Table";
}

void FileWatcherController::startWatching()
{
    qDebug()<< "StartWatching";
    pathsModel->findAllFiles();

    watcher->addPaths(pathsModel->getFilesPaths());

    connect(watcher, &QFileSystemWatcher::fileChanged, this, &FileWatcherController::fileChanged);
    connect(watcher, &QFileSystemWatcher::directoryChanged, this, &FileWatcherController::directoryChanged);
}

void FileWatcherController::stopWatching()
{
    qDebug()<< "StopWatching";

    if(!watcher->directories().isEmpty()){
        watcher->removePaths(watcher->directories());
        qDebug()<< "Dir Empty";
    }

    if(!watcher->files().isEmpty()){
        watcher->removePaths(watcher->files());
        qDebug()<< "Files Empty";
    }
}

WatchedPathsModel *FileWatcherController::getPathsModel()
{
    return pathsModel;
}

ScannedDataModel *FileWatcherController::getScannedModel()
{
    return scannedModel;
}

void FileWatcherController::directoryChanged(const QString &path)
{
    qDebug() << "DIR edited: " << path;
    }

void FileWatcherController::fileChanged(const QString &path)
{
    qDebug() << "Path edited: " << path;

    QFileInfo fileInfo(path);

    if(fileInfo.exists()){
        qDebug() << "File edited." << path;
        return;
    }

    qDebug() << "File renamed or deleted." << path;
}
