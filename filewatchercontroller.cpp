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

ScannedDataModel *FileWatcherController::getScannedModel()
{
    return scannedModel;
}



void FileWatcherController::findAllDirItems()
{
    files.clear();
    folders.clear();

    for(const auto & path: pathsModel->getPaths()){
        folders.append(QFileInfo(path));
        checkSubDir(path);
    }

    qDebug() <<"Loaded files";
    //qDebug() << allFiles;
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
    qDebug() << "###FILE###" << path;
    return;

    QFileInfo fileInfo(path);

    //Edytowany
    if(fileInfo.exists())
    {
        fileEvents.emplace_back(path, Event::Edited, true);
        return;
    }


    bool fileRenamed = false;

    QFileInfo newFileInfo;

    int index;
    QFileInfo lastFileInfo = getLastFileInfo(path, index);

    if(index == FILE_NOT_FOUND) return;

    auto fileInfoList = lastFileInfo.absoluteDir().entryInfoList(QDir::Files);

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
        fileEvents.emplace_back(path, Event::Renamed, true);

        watcher->addPath(newFileInfo.absoluteFilePath());
        files.append(newFileInfo);
    }
    else
    {
        fileEvents.emplace_back(path, Event::Deleted, true);
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
    QFileInfoList diffFileList = getDifferences(actualFiles, lastFiles);

    //New File
    if(actualFiles.size() > lastFiles.size()){
        //No changes
        if(diffFileList.isEmpty())
            return;

        for(const auto & file : diffFileList){
            fileEvents.emplace_back(file, Event::Created);
        }

        watcher->addPaths(getPathsFromInfoList(diffFileList));
        files.append(diffFileList);

        return;
    }


    //Check for new folder
    QFileInfoList actualEntry = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    QFileInfoList lastEntry = getLastDirContent(path, folders);

    QFileInfoList changedFolders = getDifferences(actualEntry, lastEntry);
    QFileInfoList oldFolders = getDifferences(lastEntry, actualEntry);


    //New folder
    if(actualEntry.size() > lastEntry.size()){
        if(changedFolders.isEmpty())
            return;

        for(const auto & folder : changedFolders)
        {
            fileEvents.emplace_back(folder, Event::Created);
        }

        watcher->addPaths(getPathsFromInfoList(changedFolders));
        folders.append(changedFolders);

        return;
    }


    //Renamed
    if(actualEntry.size() == lastEntry.size()){
        if(oldFolders.isEmpty() || changedFolders.isEmpty())
            return;

        for(const auto & folder : oldFolders)
        {
            fileEvents.emplace_back(folder, Event::Renamed);
        }

        watcher->addPaths(getPathsFromInfoList(changedFolders));
        folders = getDifferences(folders, oldFolders);
        folders.append(changedFolders);

        return;
    }

    //Deleted
    if(actualEntry.size() < lastEntry.size()){
        if(oldFolders.isEmpty())
            return;

        for(const auto & folder : oldFolders)
        {
            fileEvents.emplace_back(folder, Event::Deleted);
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

    //std::set_difference(list.begin(), list.end(), substractList.begin(), substractList.end(),
    //                    std::back_inserter(output));

    for(const auto & item : list){
        if(substractList.indexOf(item) == FILE_NOT_FOUND){
            output.append(item);
        }
    }

    return output;
}
