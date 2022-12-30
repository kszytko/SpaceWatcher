#include "downloader.h"
#include "qdir.h"

Downloader::Downloader( QObject *parent) :
    QObject(parent)
{
    m_networkAccessManager = new QNetworkAccessManager(this);
}

void Downloader::downloadFile(const QString &destination, const QUrl &url)
{
    QNetworkRequest request(url);
    m_networkReply = m_networkAccessManager->get(request);

    if(m_saveFile)
        m_saveFile->cancelWriting();
    shutdownSaveFile();
    m_saveFile = new QSaveFile(destination);
    if (!m_saveFile->open(QIODevice::WriteOnly)) {
        shutdownSaveFile();
        return;
    }

    connect(m_networkReply, &QNetworkReply::readyRead, this, &Downloader::networkReplyReadyRead);
    connect(m_networkReply, &QNetworkReply::finished, this, &Downloader::networkReplyFinished);
}

void Downloader::downloadKitty(const QString & path)
{
    QString newPath = path + QString("/kitty_%1.jfif").arg(++downloadCounter);
    //auto path = QString("C:/workspace/FindInFiles/a/kitty%1.jfif").arg(++downloadCounter);

    QUrl url("https://cataas.com/cat/says/hello%20world!");

    downloadFile(newPath, url);
}

void Downloader::networkReplyReadyRead()
{
    if(m_networkReply->error() != QNetworkReply::NoError){
        return;
    }

    if (m_saveFile)
        m_saveFile->write(m_networkReply->readAll());
}

void Downloader::networkReplyFinished()
{
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
    if(m_networkReply) {
        disconnect(m_networkReply, &QNetworkReply::readyRead, this, &Downloader::networkReplyReadyRead);
        disconnect(m_networkReply, &QNetworkReply::finished, this, &Downloader::networkReplyFinished);

        m_networkReply->deleteLater();
        m_networkReply = 0;
    }
}

void Downloader::shutdownSaveFile()
{
    if(m_saveFile) {
        m_saveFile->commit();
        delete m_saveFile;
        m_saveFile = 0;
    }
}

DownloadController::DownloadController(){
    Downloader *worker = new Downloader;
    worker->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &DownloadController::download, worker, &Downloader::downloadKitty);
    workerThread.start();
}

DownloadController::~DownloadController() {
    workerThread.quit();
    workerThread.wait();
}
