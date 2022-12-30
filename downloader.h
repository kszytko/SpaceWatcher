#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSaveFile>
#include <QThread>

class Downloader : public QObject
{
    Q_OBJECT
public:
    explicit Downloader(QObject *parent = 0);
    void downloadFile(const QString & path, const QUrl & url);

signals:
    void finished();

public slots:
    void downloadKitty(const QString & path);

private slots:
    void networkReplyReadyRead();
    void networkReplyFinished();

private:
    QNetworkAccessManager * m_networkAccessManager;
    QNetworkReply * m_networkReply;
    QSaveFile * m_saveFile;

    int downloadCounter{0};

    void shutdownNetworkReply();
    void shutdownSaveFile();

};

class DownloadController : public QObject
{
    Q_OBJECT
    QThread workerThread;

public:
    DownloadController();
    ~DownloadController();

signals:
    void download(const QString & path);
};

#endif // DOWNLOADER_H
