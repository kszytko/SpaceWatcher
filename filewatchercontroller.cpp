#include "filewatchercontroller.h"
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include <algorithm>

FileWatcherController::FileWatcherController(QObject *parent)
    : QObject{parent}
{
    m_pathsModel = new WatchedPathsModel();
    m_eventsModel = new FileEventsModel();
    m_systemWatcher = new QFileSystemWatcher();
    m_downloadController = new DownloadController();
}

void FileWatcherController::clearTable()
{
    m_eventsModel->clear();
}

void FileWatcherController::startWatching()
{
    if(m_pathsModel->isEmpty())
        return;

    if(watcherState())
        return;

    findAllDirItems();

    m_systemWatcher->addPaths(getPathsFromInfoList(m_files));
    m_systemWatcher->addPaths(getPathsFromInfoList(m_folders));

    connect(m_systemWatcher, &QFileSystemWatcher::fileChanged, this, &FileWatcherController::fileChanged);
    connect(m_systemWatcher, &QFileSystemWatcher::directoryChanged, this, &FileWatcherController::directoryChanged);
    connect(this, &FileWatcherController::itemDeleted, this, &FileWatcherController::downloadKitty);

    setwatcherState(true);
}

void FileWatcherController::stopWatching()
{
    if(!m_systemWatcher->directories().isEmpty()){
        m_systemWatcher->removePaths(m_systemWatcher->directories());
    }

    if(!m_systemWatcher->files().isEmpty()){
        m_systemWatcher->removePaths(m_systemWatcher->files());
    }

    setwatcherState(false);
}

WatchedPathsModel *FileWatcherController::getPathsModel()
{
    return m_pathsModel;
}

FileEventsModel *FileWatcherController::getFileEventsModel()
{
    return m_eventsModel;
}

void FileWatcherController::findAllDirItems()
{
    m_files.clear();
    m_folders.clear();

    for(const auto & path: m_pathsModel->getPaths()){
        m_folders.emplace_back(path);
        checkSubDir(path);
    }
}


void FileWatcherController::checkSubDir(QString path)
{
    QDir dir(path);

    for(const auto & item : dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::NoSymLinks))
    {
        if(item.isDir())
        {
            m_folders.append(item);
            checkSubDir(item.absoluteFilePath());
        }
        else
        {
            m_files.append(item);
        }
    }
}

QStringList FileWatcherController::getPathsFromInfoList(const QFileInfoList & infoList)
{
    QStringList paths;

    for(const auto & item : infoList){
        paths.append(item.absoluteFilePath());
    }

    return paths;
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
        if(substractList.indexOf(item) == -1){
            output.append(item);
        }
    }

    return output;
}

void FileWatcherController::differenceLists(QFileInfoList& items, const QFileInfoList& oldItems)
{
    items = getDifferences(items, oldItems);
}

void FileWatcherController::setCreatedEvent(QFileInfoList & items, const QFileInfoList &  changedItems)
{
    if(changedItems.isEmpty())
        return;

    m_eventsModel->add(changedItems, Event::Created);
    m_systemWatcher->addPaths(getPathsFromInfoList(changedItems));
    items.append(changedItems);
}


void FileWatcherController::setRenamedEvent(QFileInfoList & items, const QFileInfoList &  changedItems, const QFileInfoList & oldItems)
{
    if(changedItems.isEmpty() || oldItems.isEmpty())
        return;

    m_eventsModel->add(oldItems, Event::Renamed);
    m_systemWatcher->addPaths(getPathsFromInfoList(changedItems));
    differenceLists(items, oldItems);
    items.append(changedItems);
}

void FileWatcherController::setDeletedEvent(QFileInfoList& items, const QFileInfoList& oldItems)
{
    if(oldItems.isEmpty())
        return;

    m_eventsModel->add(oldItems, Event::Deleted);
    differenceLists(items, oldItems);

    emit itemDeleted();
}

void FileWatcherController::checkChanges(QFileInfoList & items, const QDir & dir, QDir::Filters filters)
{
    auto actualItems = dir.entryInfoList(filters);
    auto lastItems = getLastDirContent(dir.path(), items);
    auto changedItems = getDifferences(actualItems, lastItems);
    auto oldItems = getDifferences(lastItems, actualItems);


    if(actualItems.size() > lastItems.size()){
        setCreatedEvent(items, changedItems);
        return;
    }

    if(actualItems.size() == lastItems.size()){
        setRenamedEvent(items, changedItems, oldItems);
        return;
    }

    if(actualItems.size() < lastItems.size())
        setDeletedEvent(items, oldItems);
}


void FileWatcherController::checkFilesChanges(const QDir & dir)
{
    checkChanges(m_files, dir, QDir::Files);
}


void FileWatcherController::checkFoldersChanges(const QDir & dir)
{

    checkChanges(m_folders, dir, QDir::Dirs | QDir::NoDotAndDotDot);
}

void FileWatcherController::directoryChanged(const QString & path)
{
    QDir dir(path);

    if(!dir.exists())
        return;

    checkFilesChanges(dir);
    checkFoldersChanges(dir);
}

void FileWatcherController::fileChanged(const QString &path)
{
    QFileInfo file(path);

    if(file.exists())
        m_eventsModel->add(file, Event::Edited);
}

void FileWatcherController::downloadKitty()
{
    QString path = m_pathsModel->getPaths()[0];
    m_downloadController->startDownload(path);
}

bool FileWatcherController::watcherState() const
{
    return m_watcherState;
}

void FileWatcherController::setwatcherState(bool newWatcherState)
{
    if (m_watcherState == newWatcherState)
        return;

    m_watcherState = newWatcherState;
    emit watcherStateChanged();
}
