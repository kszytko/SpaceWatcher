#ifndef FILEWATCHERCONTROLLER_H
#define FILEWATCHERCONTROLLER_H

#include <QObject>
#include <QFileSystemWatcher>
#include <QDir>

#include "watchedpathsmodel.h"
#include "fileeventsmodel.h"
#include "downloader.h"


class FileWatcherController : public QObject
{
    Q_OBJECT

public:
    explicit FileWatcherController(QObject *parent = nullptr);

    Q_PROPERTY(bool watcherState READ watcherState WRITE setwatcherState NOTIFY watcherStateChanged);

    Q_INVOKABLE void clearTable();
    Q_INVOKABLE void startWatching();
    Q_INVOKABLE void stopWatching();

    WatchedPathsModel * getPathsModel();
    FileEventsModel * getFileEventsModel();

    bool watcherState() const;
    void setwatcherState(bool newWatcherState);

signals:
    void watcherStateChanged();
    void itemDeleted();

private slots:
    void directoryChanged(const QString & path);
    void fileChanged(const QString & path);
    void downloadKitty();

private:
    void findAllDirItems();
    void checkSubDir(QString path);

    QStringList getPathsFromInfoList(const QFileInfoList & infoList);
    QFileInfoList getLastDirContent(const QString & path, QFileInfoList & list);
    QFileInfoList getDifferences(const QFileInfoList & subFiles, const QFileInfoList & lastSubFiles);

    void differenceLists(QFileInfoList & items, const QFileInfoList & oldItems);
    void setCreatedEvent(QFileInfoList & items, const QFileInfoList & changedItems);
    void setRenamedEvent(QFileInfoList & items, const QFileInfoList & oldItems, const QFileInfoList & changedItems);
    void setDeletedEvent(QFileInfoList & items, const QFileInfoList & oldItems);
    void checkChanges(QFileInfoList items, const QDir& dir, QDir::Filters filters);
    void checkFilesChanges(const QDir & dir);
    void checkFoldersChanges(const QDir & dir);

private:
    WatchedPathsModel * m_pathsModel;
    FileEventsModel * m_eventsModel;
    QFileSystemWatcher * m_systemWatcher;

    QFileInfoList m_files;
    QFileInfoList m_folders;

    bool m_watcherState = false;

    DownloadController * m_downloadController;
};

#endif // FILEWATCHERCONTROLLER_H
