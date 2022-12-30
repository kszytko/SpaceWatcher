#include "downloader.h"
#include <QTime>

Downloader::Downloader(const QString & path, QObject * parent) :
    QObject(parent), QRunnable()
{
    qDebug() << "Downloader init";

    auto time = QDateTime::currentDateTimeUtc();
    QString sTime = time.toString("hhmmss");

    m_path = path + QString("/kitty_%1.jfif").arg(sTime);
}

Downloader::~Downloader()
{
    shutdownNetworkReply();
    shutdownSaveFile();

}

void Downloader::downloadFile(const QString &destination, const QUrl &url)
{
    qDebug() << "DownloadFile";
    QNetworkAccessManager networkAccessManager;
    QNetworkRequest request(url);

    qDebug() << "SET";

    m_networkReply = networkAccessManager.get(request);

    qDebug() << "File" << destination;

    m_saveFile = new QSaveFile(destination);

    if (!m_saveFile->open(QIODevice::WriteOnly)) {
        qDebug() << "ERROR";
        shutdownSaveFile();

        return;
    }

    qDebug() << "START";

    connect(m_networkReply, &QNetworkReply::readyRead, this, &Downloader::networkReplyReadyRead);
    connect(m_networkReply, &QNetworkReply::finished, this, &Downloader::networkReplyFinished);


}

void Downloader::run()
{
    qDebug() << "Run";
    downloadFile(m_path, KITTY_URL);
}

void Downloader::networkReplyReadyRead()
{
    qDebug() << "NetworkReplyReadyRead";
    if(m_networkReply->error() != QNetworkReply::NoError){
        return;
    }

    m_saveFile->write(m_networkReply->readAll());
}

void Downloader::networkReplyFinished()
{
    qDebug() << "NetworkReplyFinished";
    if(m_saveFile->commit())
    {
        shutdownSaveFile();
        emit finished();
    }
    else
    {
        m_saveFile->cancelWriting();
    }
}

void Downloader::shutdownNetworkReply()
{
    qDebug() << "shutdownNetworkReply";

    disconnect(m_networkReply, &QNetworkReply::readyRead, this, &Downloader::networkReplyReadyRead);
    disconnect(m_networkReply, &QNetworkReply::finished, this, &Downloader::networkReplyFinished);

    m_networkReply->deleteLater();
}

void Downloader::shutdownSaveFile()
{
    qDebug() << "shutdownSaveFile";

    m_saveFile->commit();
    m_saveFile->deleteLater();
}

DownloadController::DownloadController(){
    qDebug() << "Download controller init";
    m_threadPool = new QThreadPool();
    m_threadPool->setMaxThreadCount(4);
}

DownloadController::~DownloadController() {
}

void DownloadController::startDownload(const QString & destination)
{
    qDebug() << "start download" << destination;
    Downloader * newDownload = new Downloader(destination);
    m_threadPool->start(newDownload);
    m_threadPool->start()
    qDebug() << "###" << m_threadPool->activeThreadCount();
}
