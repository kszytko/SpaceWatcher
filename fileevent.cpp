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

FileEvent::FileEvent(const QString path, Event event, bool isFile):
    event_(event), path_(path), isFile_(isFile)
{
    qDebug() << print();
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
