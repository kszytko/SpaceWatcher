#include "fileeventsmodel.h"
#include <QDebug>

FileEventsModel::FileEventsModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    headers << "Event type";
    headers << "Path";
    headers << "Is Folder";
    headers << "Timestamp";

    fileEvents.emplace_back("TEST1", true, Event::Created);
    fileEvents.emplace_back("TEST2", false, Event::Deleted);
}

QVariant FileEventsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        return headers[section];
    }

    return QVariant();
}

int FileEventsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return fileEvents.size();
}

int FileEventsModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 4;
}


QVariant FileEventsModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(role == Qt::DisplayRole){
        auto event = fileEvents[index.row()];

        switch(index.column()){
        case 0:
            return event.getEventString();
        case 1:
            return event.getPath();
        case 2:
            return event.isFolder();
        case 3:
            return event.getTimeStamp();
        default:
            return QVariant();
        }
    }

    return QVariant();
}

void FileEventsModel::add(const QFileInfo &info, Event event)
{
    beginInsertRows(QModelIndex(), fileEvents.size(), fileEvents.size());
    fileEvents.emplace_back(info, event);
    endInsertRows();
}

void FileEventsModel::clear()
{
    beginRemoveRows(QModelIndex(),0, fileEvents.size());
    fileEvents.clear();
    endRemoveRows();
}
