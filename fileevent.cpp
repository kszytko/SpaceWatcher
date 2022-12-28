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
    isFile_ = fileInfo.isFile();
}

FileEvent::FileEvent(QString path, QDateTime time, Event event, bool isFile):
    event_(event), path_(path), time_(time), isFile_(isFile)
{

}

QString FileEvent::print()
{
    QString output{"%"};
    output += isFile_ ? "File" : "Folder";
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

    output += " " + path_ + " "+ time_.toString();

    return output;
}
