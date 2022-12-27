#include "filewatchercontroller.h"
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include <algorithm>

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
    findAllFiles();

    watcher->addPaths(getFilesPaths());

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



void FileWatcherController::findAllFiles()
{
    files.clear();

    for(auto & path: pathsModel->getPaths()){
        files.append(QFileInfo(path));
        findFilesInDir(path);
    }

    qDebug() <<"Loaded files";
    //qDebug() << allFiles;
}


void FileWatcherController::findFilesInDir(QString path)
{
    //qDebug()<< "Iterate: " << path;

    QDir dir(path);
    QFileInfoList fileList = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    files.append(fileList);

    for(const auto & subDir : fileList){
        //qDebug()<< "1: " << subDir.filePath();
        if(subDir.isDir()){
            findFilesInDir(subDir.absoluteFilePath());
        }
    }
}

QStringList FileWatcherController::getFilesPaths()
{
    QStringList filePaths;

    for(const auto & file : files){
        filePaths.append(file.absoluteFilePath());
    }

    return filePaths;
}



void FileWatcherController::fileChanged(const QString &path)
{
    qDebug() << "Checking file:" << path;

    QFileInfo lastFileInfo(path);

    //Edytowany
    if(lastFileInfo.exists())
    {
        FileEvent fileEvent(lastFileInfo, QDateTime::currentDateTime(), Event::Edited);
        fileEvents.append(fileEvent);
        qDebug() << fileEvent.print();

        return;
    }
    //Renamed or deleted
    lastFileInfo = getLastFileInfo(path);
    qDebug() << "File removed or renamed";



    QDir dir = lastFileInfo.absoluteDir();
    QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files);

    bool fileRenamed = false;
    for(const auto & fileInfo : fileInfoList){
        if(lastFileInfo.size() == fileInfo.size() && files.indexOf(fileInfo) == -1){
            fileRenamed = true;
            watcher->addPath(fileInfo.absoluteFilePath());
            break;
        }
    }

    if(fileRenamed){
        FileEvent fileEvent(lastFileInfo, QDateTime::currentDateTime(), Event::Renamed);
        fileEvents.append(fileEvent);
        qDebug() << "File Renamed";

    }
    else
    {
        FileEvent fileEvent(lastFileInfo, QDateTime::currentDateTime(), Event::Deleted);
        fileEvents.append(fileEvent);
        qDebug() << "File Removed";
    }


}

void FileWatcherController::directoryChanged(const QString &path)
{
    qDebug() << "Checking folder:" << path;



}

QFileInfo FileWatcherController::getLastFileInfo(const QString &path)
{
    //auto file = std::find_if(files.begin(), files.end(),[&](auto fileInfo)
    //{
    //    return fileInfo.absoluteFilePath() == path;
    //});

    auto index = files.indexOf(QFileInfo(path));

    qDebug() << index;

    if(index >= 0){
        qDebug() << files[index];
        return files[index];
    }

    return QFileInfo();
}
