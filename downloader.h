#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSaveFile>
#include <QThread>
#include <QThreadPool>
#include <QRunnable>

const QUrl KITTY_URL("https://cataas.com/cat/says/hello%20world!");

class Downloader : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit Downloader(const QString & path, QObject * parent = 0);
    ~Downloader();

    void downloadFile(const QString & path, const QUrl & url);
    void run() override;

signals:
    void finished();

private slots:
    void networkReplyReadyRead();
    void networkReplyFinished();

private:
    QNetworkAccessManager * m_networkAccessManager;
    QNetworkReply * m_networkReply;
    QSaveFile * m_saveFile;

    void shutdownNetworkReply();
    void shutdownSaveFile();

    QString m_path{};
};

class DownloadController : public QObject
{
    Q_OBJECT


public:
    DownloadController();
    ~DownloadController();

public slots:
    void startDownload(const QString & destination);
};

#endif // DOWNLOADER_H
