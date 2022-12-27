#include "filewatchercontroller.h"
#include <QDebug>
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
    watcher->addPaths(pathsModel->getPaths());

    connect(watcher, &QFileSystemWatcher::fileChanged, this, &FileWatcherController::fileChanged);
    connect(watcher, &QFileSystemWatcher::directoryChanged, this, &FileWatcherController::directoryChanged);
}

void FileWatcherController::stopWatching()
{
    qDebug()<< "StopWatching";
    watcher->removePaths(watcher->directories());
    watcher->removePaths(watcher->files());
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
}
