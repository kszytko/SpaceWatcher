#ifndef WATCHEDPATHSMODEL_H
#define WATCHEDPATHSMODEL_H

#include <qqml.h>
#include <QAbstractListModel>
#include <QUrl>
#include <QList>


class WatchedPathsModel : public QAbstractListModel
{
    Q_OBJECT


public:
    explicit WatchedPathsModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE void remove(int row);
    Q_INVOKABLE void append(const QUrl &path);

private:
    QList<QUrl> paths;

};

#endif // WATCHEDPATHSMODEL_H
