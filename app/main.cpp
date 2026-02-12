#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QIcon>
#include <QDebug>
#include <QFile>
#include <QLoggingCategory>

#include "bridge/AppController.h"
#include "bridge/DocumentModel.h"
#include "render/CanvasItem.h"

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);
    app.setApplicationName("Comicos");
    app.setApplicationVersion(COMICOS_VERSION);
    app.setOrganizationName("Comicos");

    // Platform-specific initialization
#if defined(COMICOS_PLATFORM_windows)
    // Windows: High DPI already handled by Qt 6 default
    // Extension point: WinTab driver initialization for tablet
#elif defined(COMICOS_PLATFORM_macos)
    // macOS: handled natively
    // Extension point: NSApplication customization
#elif defined(COMICOS_PLATFORM_ios)
    // iOS: handled natively
    // Extension point: UIApplication setup, Apple Pencil detection
#endif

    // Use Basic style as base (lightweight, customizable)
    QQuickStyle::setStyle("Basic");

    // Register C++ types with QML
    qmlRegisterSingletonType<comicos::AppController>(
        "Comicos", 1, 0, "AppController",
        [](QQmlEngine*, QJSEngine*) -> QObject* {
            return new comicos::AppController();
        });

    qmlRegisterType<comicos::DocumentModel>("Comicos", 1, 0, "DocumentModel");
    qmlRegisterType<comicos::CanvasItem>("Comicos", 1, 0, "CanvasItem");

    // Load QML
    QQmlApplicationEngine engine;

    const QUrl url(QStringLiteral("qrc:/qt/qml/Comicos/qml/Main.qml"));

    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, [&url]() {
            qCritical() << "Failed to create QML object from" << url;
            QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);

    engine.load(url);

    if (engine.rootObjects().isEmpty()) {
        qCritical() << "No root objects created. QML load failed for" << url;
        return -1;
    }

    qDebug() << "Comicos started successfully";
    return app.exec();
}
