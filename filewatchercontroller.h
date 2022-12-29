#ifndef FILEWATCHERCONTROLLER_H
#define FILEWATCHERCONTROLLER_H

#include <QObject>
#include <QFileSystemWatcher>

#include "watchedpathsmodel.h"
#include "fileeventsmodel.h"

#include "fileevent.h"

constexpr qsizetype FILE_NOT_FOUND = -1;

class FileWatcherController : public QObject
{
    Q_OBJECT


public:
    explicit FileWatcherController(QObject *parent = nullptr);

    Q_INVOKABLE void clearTable();
    Q_INVOKABLE void startWatching();
    Q_INVOKABLE void stopWatching();

    WatchedPathsModel * getPathsModel();
    FileEventsModel * getFileEventsModel();


private slots:
  void directoryChanged(const QString & path);
  void fileChanged(const QString & path);

private:
   QStringList getPathsFromInfoList(QFileInfoList & infoList);

   void findAllDirItems();
   void checkSubDir(QString path);

   QFileInfo getLastFileInfo(const QString & path, int & index);

   QFileInfoList getLastDirContent(const QString & path, QFileInfoList & list);

   QFileInfoList getDifferences(const QFileInfoList & subFiles, const QFileInfoList & lastSubFiles);

private:
  WatchedPathsModel * pathsModel;
  FileEventsModel * fileEventsModel;
  QFileSystemWatcher * watcher;

  QFileInfoList files;
  QFileInfoList folders;


};

#endif // FILEWATCHERCONTROLLER_H
