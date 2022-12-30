#ifndef FILEEVENT_H
#define FILEEVENT_H

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
    FileEvent(QString path, bool isFile, Event event);

    QString print();
    QString getEventString();
    QString getTimeStamp();
    QString getPath();
    QString isFolder();

private:
    Event event_ = Event::Undefined;
    QString path_{};
    bool isFile_ = false;
    QDateTime time_ = QDateTime::currentDateTime();
};

#endif // FILEEVENT_H
