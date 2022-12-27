#ifndef FILEEVENT_H
#define FILEEVENT_H

//(Created, Deleted, Edited,Renamed), file localization and information if changed entry was a file or a folder.

#include <QString>
#include <QFileInfo>
#include <QDateTime>

enum class Event{
    Created,
    Deleted,
    Edited,
    Renamed
};

class FileEvent
{
public:
    FileEvent();
    FileEvent(QFileInfo & fileInfo, QDateTime time, Event event);

    QString print();

private:
    Event event_;
    QString path_;
    QDateTime time_;
    bool isFolder_;
};

#endif // FILEEVENT_H
