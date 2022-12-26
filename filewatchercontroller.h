#ifndef FILEWATCHERCONTROLLER_H
#define FILEWATCHERCONTROLLER_H

#include <QObject>
#include <QFileSystemWatcher>

#include "watchedpathsmodel.h"
#include "scanneddatamodel.h"

class FileWatcherController : public QObject
{
    Q_OBJECT

    WatchedPathsModel *pathsModel;
    ScannedDataModel *filesModel;

    QFileSystemWatcher watcher;

public:
    explicit FileWatcherController(QObject *parent = nullptr);

    Q_INVOKABLE void clearTable();
    Q_INVOKABLE void startWatching();
    Q_INVOKABLE void stopWatching();

signals:

private slots:

  void directoryChanged(const QString & path);
  void fileChanged(const QString & path);

};

#endif // FILEWATCHERCONTROLLER_H
