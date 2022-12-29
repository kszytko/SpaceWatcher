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
    fileEventsModel = new FileEventsModel();
    watcher = new QFileSystemWatcher();
}

void FileWatcherController::clearTable()
{
    fileEventsModel->clear();
    qDebug()<< "Clear Table";
}

void FileWatcherController::startWatching()
{
    qDebug()<< "StartWatching";
    findAllDirItems();

    watcher->addPaths(getPathsFromInfoList(files));
    watcher->addPaths(getPathsFromInfoList(folders));

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

FileEventsModel *FileWatcherController::getFileEventsModel()
{
    return fileEventsModel;
}



void FileWatcherController::findAllDirItems()
{
    files.clear();
    folders.clear();

    for(const auto & path: pathsModel->getPaths()){
        folders.emplace_back(path);
        checkSubDir(path);
    }

    qDebug() <<"Loaded files";
}


void FileWatcherController::checkSubDir(QString path)
{
    QDir dir(path);

    for(const auto & item : dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::NoSymLinks))
    {
        if(item.isDir())
        {
            folders.append(item);
            checkSubDir(item.absoluteFilePath());
        }
        else
        {
            files.append(item);
        }
    }
}

QStringList FileWatcherController::getPathsFromInfoList(QFileInfoList & infoList)
{
    QStringList paths;

    for(const auto & item : infoList){
        paths.append(item.absoluteFilePath());
    }

    return paths;
}



void FileWatcherController::fileChanged(const QString &path)
{
    QFileInfo file(path);

    if(file.exists())
    {
        fileEventsModel->add(file, Event::Edited);
        return;
    }
}

void FileWatcherController::directoryChanged(const QString &path)
{
    QDir dir(path);

    if(!dir.exists())
        return;

    //Check for new file
    QFileInfoList actualFiles = dir.entryInfoList(QDir::Files);
    QFileInfoList lastFiles = getLastDirContent(path, files);

    QFileInfoList changedFiles = getDifferences(actualFiles, lastFiles);
    QFileInfoList oldFiles = getDifferences(lastFiles, actualFiles);

    //New File
    if(actualFiles.size() > lastFiles.size() && !changedFiles.isEmpty()){
        for(const auto & file : changedFiles){
            fileEventsModel->add(file, Event::Created);
        }

        watcher->addPaths(getPathsFromInfoList(changedFiles));
        files.append(changedFiles);

        return;
    }

    //Renamed
    if(actualFiles.size() == lastFiles.size() && !changedFiles.isEmpty() && !oldFiles.isEmpty()){
        for(const auto & file : oldFiles)
        {
            fileEventsModel->add(file, Event::Renamed);
        }

        watcher->addPaths(getPathsFromInfoList(changedFiles));
        files = getDifferences(files, oldFiles);
        files.append(changedFiles);

        return;
    }

    //Deleted
    if(actualFiles.size() < lastFiles.size() && !oldFiles.isEmpty()){
        for(const auto & file : oldFiles)
        {
            fileEventsModel->add(file, Event::Deleted);
        }

        files = getDifferences(files, oldFiles);
    }

    //Check for new folder
    QFileInfoList actualFolders = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    QFileInfoList lastFolders = getLastDirContent(path, folders);

    QFileInfoList changedFolders = getDifferences(actualFolders, lastFolders);
    QFileInfoList oldFolders = getDifferences(lastFolders, actualFolders);


    //New folder
    if(actualFolders.size() > lastFolders.size() && !changedFolders.isEmpty()){
        for(const auto & folder : changedFolders)
        {
            fileEventsModel->add(folder, Event::Created);
        }

        watcher->addPaths(getPathsFromInfoList(changedFolders));
        folders.append(changedFolders);

        return;
    }


    //Renamed
    if(actualFolders.size() == lastFolders.size() && !oldFolders.isEmpty() && !changedFolders.isEmpty()){
        for(const auto & folder : oldFolders)
        {
            fileEventsModel->add(folder, Event::Renamed);
        }

        watcher->addPaths(getPathsFromInfoList(changedFolders));
        folders = getDifferences(folders, oldFolders);
        folders.append(changedFolders);

        return;
    }

    //Deleted
    if(actualFolders.size() < lastFolders.size() && !oldFolders.isEmpty()){
        for(const auto & folder : oldFolders)
        {
            fileEventsModel->add(folder, Event::Deleted);
        }

        folders = getDifferences(folders, oldFolders);
    }
}

QFileInfo FileWatcherController::getLastFileInfo(const QString &path, int & index)
{
    index = files.indexOf(QFileInfo(path));

    if(index == FILE_NOT_FOUND)
        return QFileInfo();

    return files[index];
}



QFileInfoList FileWatcherController::getLastDirContent(const QString & path, QFileInfoList & list)
{
    QFileInfoList output;

    std::copy_if(list.begin(), list.end(), std::back_inserter(output), [&path](QFileInfo & fileInfo)
    {
        return fileInfo.absolutePath() == path;
    });

    return output;
}


QFileInfoList FileWatcherController::getDifferences(const QFileInfoList &list, const QFileInfoList &substractList)
{
    QFileInfoList output;

    for(const auto & item : list){
        if(substractList.indexOf(item) == FILE_NOT_FOUND){
            output.append(item);
        }
    }

    return output;
}
