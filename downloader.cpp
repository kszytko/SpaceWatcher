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
    qDebug() << "Jebać";
    shutdownNetworkReply();
    shutdownSaveFile();
}

void Downloader::downloadFile(const QString &destination, const QUrl &url)
{



}

void Downloader::run()
{
    qDebug() << "DownloadFile";
    m_networkAccessManager = new QNetworkAccessManager();
    QNetworkRequest request(KITTY_URL);

    qDebug() << "SET";

    m_networkReply = m_networkAccessManager->get(request);

    qDebug() << "File" << m_path;

    m_saveFile = new QSaveFile(m_path);

    if (!m_saveFile->open(QIODevice::WriteOnly)) {
        qDebug() << "ERROR";
        shutdownSaveFile();

        return;
    }

    qDebug() << "START";

    connect(m_networkReply, &QNetworkReply::readyRead, this, &Downloader::networkReplyReadyRead);
    connect(m_networkReply, &QNetworkReply::finished, this, &Downloader::networkReplyFinished);

    qDebug() << "FIN";
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
    shutdownNetworkReply();

    if(m_saveFile->commit())
    {
        shutdownSaveFile();
        emit finished();
    }
    else
    {
        m_saveFile->cancelWriting();
        shutdownSaveFile();
    }

}

void Downloader::shutdownNetworkReply()
{
    qDebug() << "shutdownNetworkReply";

    disconnect(m_networkReply, &QNetworkReply::readyRead, this, &Downloader::networkReplyReadyRead);
    disconnect(m_networkReply, &QNetworkReply::finished, this, &Downloader::networkReplyFinished);

    m_networkReply->deleteLater();
    m_networkAccessManager->deleteLater();
}

void Downloader::shutdownSaveFile()
{
    qDebug() << "shutdownSaveFile";

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
    //connect(worker, &Downloader::finished, workerThread, &QObject::deleteLater);
    connect(workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(workerThread, &QThread::finished, workerThread, &QThread::quit);
    workerThread->start();
    worker->run();
}
