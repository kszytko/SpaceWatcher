#include "fileevent.h"
#include <QString>
#include <QDebug>

FileEvent::FileEvent()
{

}

FileEvent::FileEvent(const QFileInfo &fileInfo, Event event) :
    event_(event)
{
    path_ = fileInfo.absoluteFilePath();
    isFile_ = fileInfo.isFile();

    qDebug() << print();
}

FileEvent::FileEvent(QString path, bool isFile, Event event) :
    event_(event), path_(path), isFile_(isFile)
{

}

QString FileEvent::print()
{
    QString output{"%"};
    output += isFile_ ? "File" : "Folder";
    output += " ";
    output += getEventString();
    output += " " + path_ + " "+ time_.toString();

    return output;
}

QString FileEvent::getEventString()
{
    switch(event_){
    case Event::Edited:
        return "Edited";
    case Event::Deleted:
        return "Deleted";
    case Event::Renamed:
        return "Renamed";
    case Event::Created:
        return "Created";
    default:
        return {};
    }
}

QString FileEvent::getTimeStamp()
{
    return time_.toString(Qt::TextDate);
}


QString FileEvent::getPath()
{
    return path_;
}


QString FileEvent::isFolder()
{
    return !isFile_ ? "Yes" : "No";
}
