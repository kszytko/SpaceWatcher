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

    watcher->addPaths(getFilesPaths(files));

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

QStringList FileWatcherController::getFilesPaths(QFileInfoList & fileInfos)
{
    QStringList filePaths;

    for(const auto & file : fileInfos){
        filePaths.append(file.absoluteFilePath());
    }

    return filePaths;
}



void FileWatcherController::fileChanged(const QString &path)
{
    qDebug() << "###FILE###" << path;

    QFileInfo fileInfo(path);

    //Edytowany
    if(fileInfo.exists())
    {
        FileEvent fileEvent(path, QDateTime::currentDateTime(), Event::Edited, true);
        fileEvents.append(fileEvent);
        qDebug() << fileEvent.print();
        return;
    }


    bool fileRenamed = false;

    QFileInfo lastFileInfo;
    getLastFileInfo(path, lastFileInfo);

    auto fileInfoList = lastFileInfo.absoluteDir().entryInfoList(QDir::Files);

    for(const auto & fileInfo : fileInfoList)
    {
        if(lastFileInfo.size() == fileInfo.size() && files.indexOf(fileInfo) == -1)
        {
            watcher->addPath(fileInfo.absoluteFilePath());
            fileRenamed = true;
            break;
        }
    }

    //Renamed
    if(fileRenamed){
        FileEvent fileEvent(path, QDateTime::currentDateTime(), Event::Renamed, true);
        fileEvents.append(fileEvent);
        qDebug() << fileEvent.print();
    }
    else
    {
        FileEvent fileEvent(path, QDateTime::currentDateTime(), Event::Deleted, true);
        fileEvents.append(fileEvent);
        qDebug() << fileEvent.print();
    }

    findAllFiles();
}

void FileWatcherController::directoryChanged(const QString &path)
{
    qDebug() << "###FOLDER###" << path;

    QFileInfo folder(path);

    if(folder.exists()){
        //check for folder / file added
        QDir dir(path);
        QFileInfoList subFiles = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::NoSymLinks);

        auto lastSubFiles = getLastSubFiles(path);


        if(subFiles.size() <= lastSubFiles.size()){
            // Delete or no action, event handled by fileChange or next folderChange
            //qDebug() << "*PASS*" << path;
            return;
        }

        //File or folder added
        //qDebug() << "File or folder added." << path;

        auto newFileList = findNewFiles(subFiles, lastSubFiles);

        for(QFileInfo & file : newFileList){
            FileEvent fileEvent(file.absoluteFilePath(), QDateTime::currentDateTime(), Event::Created, file.isFile());
            fileEvents.append(fileEvent);
            qDebug() << fileEvent.print();
        }

        watcher->addPaths(getFilesPaths(newFileList));
        files.append(newFileList);

        return;
    }

    //Folder rename

    //Folder remove

    bool fileRenamed = false;
    QFileInfo lastFileInfo, newFileInfo;

    auto index = getLastFileInfo(path, lastFileInfo);


    auto fileInfoList = lastFileInfo.absoluteDir().entryInfoList(QDir::Dirs);

    for(const auto & fileInfo : fileInfoList)
    {
        if(lastFileInfo.size() == fileInfo.size() && files.indexOf(fileInfo) == -1)
        {
            newFileInfo = fileInfo;
            fileRenamed = true;
            break;
        }
    }

    files.removeAt(index);

    //Renamed
    if(fileRenamed){
        FileEvent fileEvent(path, QDateTime::currentDateTime(), Event::Renamed, false);
        fileEvents.append(fileEvent);
        qDebug() << fileEvent.print(); 

        watcher->addPath(newFileInfo.absoluteFilePath());
        files.append(newFileInfo);
    }
    else
    {
        FileEvent fileEvent(path, QDateTime::currentDateTime(), Event::Deleted, false);
        fileEvents.append(fileEvent);
        qDebug() << fileEvent.print();
    }




}

qsizetype FileWatcherController::getLastFileInfo(const QString &path, QFileInfo & fileInfo)
{
    //auto file = std::find_if(files.begin(), files.end(),[&](auto fileInfo)
    //{
    //    return fileInfo.absoluteFilePath() == path;
    //});

    qsizetype index = files.indexOf(QFileInfo(path));

    if(index >= 0){
        fileInfo = files[index];
    }

    return index;
}

QFileInfoList FileWatcherController::getLastSubFiles(const QString &path)
{
    QFileInfoList list;

    std::copy_if(files.begin(), files.end(), std::back_inserter(list), [&path](QFileInfo & fileInfo)
    {
        return fileInfo.absoluteDir() == path;
    });

    return list;
}

QFileInfoList FileWatcherController::findNewFiles(const QFileInfoList &subFiles,const QFileInfoList &lastSubFiles)
{
    QFileInfoList list{};

    for(const auto & file : subFiles){
        if(lastSubFiles.indexOf(file) == -1){
            list.append(file);
        }
    }

    return list;
}
