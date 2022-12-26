#ifndef SCANNEDDATAMODEL_H
#define SCANNEDDATAMODEL_H

#include <QAbstractTableModel>

class ScannedDataModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit ScannedDataModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    // Fetch data dynamically:
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;

    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:

};

#endif // SCANNEDDATAMODEL_H
