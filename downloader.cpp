#include "downloader.h"
#include <QTime>

Downloader::Downloader(const QString & path, QObject * parent) :
    QObject(parent), QRunnable()
{
    auto time = QDateTime::currentDateTimeUtc();
    QString sTime = time.toString("hhmmss");

    m_path = path + QString("/kitty_%1.jfif").arg(sTime);
}

Downloader::~Downloader()
{
    shutdownNetworkReply();
    shutdownSaveFile();
}

void Downloader::downloadFile(const QString & destination, const QUrl &url)
{
    QNetworkRequest request(url);

    m_networkAccessManager = new QNetworkAccessManager();
    m_networkReply = m_networkAccessManager->get(request);

    m_saveFile = new QSaveFile(destination);

    if (!m_saveFile->open(QIODevice::WriteOnly)) {
        shutdownSaveFile();
        shutdownNetworkReply();
        return;
    }

    connect(m_networkReply, &QNetworkReply::readyRead, this, &Downloader::networkReplyReadyRead);
    connect(m_networkReply, &QNetworkReply::finished, this, &Downloader::networkReplyFinished);
}

void Downloader::run()
{
    downloadFile(m_path, KITTY_URL);
}

void Downloader::networkReplyReadyRead()
{
    if(m_networkReply->error() != QNetworkReply::NoError){
        shutdownNetworkReply();
        return;
    }

    m_saveFile->write(m_networkReply->readAll());
}

void Downloader::networkReplyFinished()
{
    //shutdownNetworkReply();

    if(!m_saveFile->commit())
    {
        m_saveFile->cancelWriting();
    }

    emit finished();
}

void Downloader::shutdownNetworkReply()
{
    disconnect(m_networkReply, &QNetworkReply::readyRead, this, &Downloader::networkReplyReadyRead);
    disconnect(m_networkReply, &QNetworkReply::finished, this, &Downloader::networkReplyFinished);

    m_networkReply->deleteLater();
    m_networkAccessManager->deleteLater();
}

void Downloader::shutdownSaveFile()
{
    m_saveFile->commit();
    m_saveFile->deleteLater();
}

DownloadController::DownloadController(){
}

DownloadController::~DownloadController() {
}

void DownloadController::startDownload(const QString & destination)
{
    QThread *workerThread = new QThread();
    Downloader *worker = new Downloader(destination);

    worker->moveToThread(workerThread);
    connect(worker, &Downloader::finished, worker, &QObject::deleteLater);
    //connect(workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(workerThread, &QThread::finished, workerThread, &QThread::quit);
    workerThread->start();
    worker->run();
}
