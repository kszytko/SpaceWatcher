#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QQmlContext>


#include "filewatchercontroller.h"


int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    FileWatcherController controller;

    engine.rootContext()->setContextProperty("controller", &controller);
    engine.rootContext()->setContextProperty("pathsModel", controller.getPathsModel());
    engine.rootContext()->setContextProperty("fileEventsModel", controller.getFileEventsModel());


    auto model = controller.getPathsModel();
    model->append("C:/workspace/FindInFiles/a");
    model->append("C:/workspace/FindInFiles/b");

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
