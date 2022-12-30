#ifndef WATCHEDPATHSMODEL_H
#define WATCHEDPATHSMODEL_H

#include <QAbstractListModel>
#include <QUrl>
#include <QList>
#include <QFileInfoList>


class WatchedPathsModel : public QAbstractListModel
{
    Q_OBJECT


public:
    explicit WatchedPathsModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE void remove(int row);
    Q_INVOKABLE void append(const QString &path);

    const QStringList & getPaths();
    bool isEmpty() const;

private:
    QString formatPath(const QString & path);

private:
    QStringList m_paths;

};

#endif // WATCHEDPATHSMODEL_H
