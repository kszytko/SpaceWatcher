#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QQmlContext>


#include "watchedpathsmodel.h"


int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    WatchedPathsModel watchedPathsModel;
    QUrl a{"C:/eclipse-workspace"};
    QUrl b{"C:/eclipse-workspace/test1"};
    QUrl c{"C:/eclipse-workspace/test2"};

    watchedPathsModel.append(a);
    watchedPathsModel.append(b);
    watchedPathsModel.append(c);

    engine.rootContext()->setContextProperty("watchedPathsModel", &watchedPathsModel);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
