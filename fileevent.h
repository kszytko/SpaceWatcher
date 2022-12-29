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
    Renamed,
    Undefined
};

class FileEvent
{
public:
    FileEvent();
    FileEvent(const QFileInfo & fileInfo, Event event);
    FileEvent(const QString path, Event event, bool isFile);

    QString print();

private:
    Event event_ = Event::Undefined;
    QString path_{};
    bool isFile_ = false;
    QDateTime time_ = QDateTime::currentDateTime();
};

#endif // FILEEVENT_H
