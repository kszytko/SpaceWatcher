#include "fileevent.h"
#include <QString>

FileEvent::FileEvent()
{

}

FileEvent::FileEvent(QFileInfo &fileInfo, QDateTime time, Event event) :
    event_(event),
    time_(time)
{
    path_ = fileInfo.absoluteFilePath();
    isFolder_ = !fileInfo.isFile();
}

FileEvent::FileEvent(QString path, QDateTime time, Event event, bool isFolder):
    event_(event), path_(path), time_(time), isFolder_(isFolder)
{

}

QString FileEvent::print()
{
    QString output;
    output = " " + path_ + " "+ time_.toString() + " ";
    output += isFolder_ ? "Folder" : "File";
    output += " ";

    switch(event_){
    case Event::Edited:
        output += "Edited";
        break;
    case Event::Deleted:
        output += "Deleted";
        break;
    case Event::Renamed:
        output += "Renamed";
        break;
    case Event::Created:
        output += "Created";
        break;
    default:
        break;
    }

    return output;
}
