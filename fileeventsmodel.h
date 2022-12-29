#ifndef FILEEVENTSMODEL_H
#define FILEEVENTSMODEL_H

#include <QAbstractTableModel>
#include <QFileInfo>
#include "fileevent.h"

class FileEventsModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit FileEventsModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void add(const QFileInfo & info, Event event);
    void add(const QFileInfoList & list, Event event);
    void clear();

private:
    QList<FileEvent> fileEvents;
    QList<QVariant> headers;

};

#endif // FILEEVENTSMODEL_H
