#ifndef FILEWATCHERCONTROLLER_H
#define FILEWATCHERCONTROLLER_H

#include <QObject>
#include <QFileSystemWatcher>

#include "watchedpathsmodel.h"
#include "scanneddatamodel.h"

class FileWatcherController : public QObject
{
    Q_OBJECT


public:
    explicit FileWatcherController(QObject *parent = nullptr);

    Q_INVOKABLE void clearTable();
    Q_INVOKABLE void startWatching();
    Q_INVOKABLE void stopWatching();

    WatchedPathsModel * getPathsModel();
    ScannedDataModel * getScannedModel();

signals:

private slots:

  void directoryChanged(const QString & path);
  void fileChanged(const QString & path);

private:
  WatchedPathsModel * pathsModel;
  ScannedDataModel * scannedModel;
  QFileSystemWatcher * watcher;

};

#endif // FILEWATCHERCONTROLLER_H
