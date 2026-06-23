#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "SCholder.h"
#include "youreyes.h"
#include <QSurfaceFormat>
#include "shadermanager.h"
#include "sampler.h"
#include <QFontDatabase>
// #include<QQuickStyle>

int main(int argc, char *argv[])
{

    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);
    QSurfaceFormat format;
    format.setVersion(3, 3);  // 请求 OpenGL 3.3
    format.setProfile(QSurfaceFormat::CoreProfile); // 核心模式 (现代 OpenGL)
    QSurfaceFormat::setDefaultFormat(format);


    QGuiApplication app(argc, argv);
    qmlRegisterType<OpenGLBridge>("YourEyes",1,0,"OpenGLBridge");
    // qmlRegisterType<ShaderManager>("YourEyes",1,0,"ShaderManager");
    // qmlRegisterType<SCholder>("YourEyes",1,0,"SCholder");

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("visual_studio", "Main");
    if(!QFile::exists(":/fonts/font/font_ali/iconfont.ttf")) qDebug()<<"FONT NOT FOUND";
    int fontiD=QFontDatabase::addApplicationFont(":/fonts/font/font_ali/iconfont.ttf");
    qDebug() << "Font ID:" << fontiD;
    QStringList FAMILY=QFontDatabase::applicationFontFamilies(fontiD);
    for(auto& s :FAMILY){
        qDebug()<<"!#!#!"<<s;
    }

    WindowManager wm(&engine);
    engine.rootContext()->setContextProperty("windowManager",&wm);
    ShaderManager sm(&engine);
    engine.rootContext()->setContextProperty("shaderManager",&sm);
    SCholder sh(&engine);
    engine.rootContext()->setContextProperty("sc",&sh);
    SamplerManager ssmm(&engine,&sh);
    // ssmm.scholder=&sh;
    engine.rootContext()->setContextProperty("sm",&ssmm);

    QObject::connect(&sh,&SCholder::scReload,&wm,&WindowManager::deleteAll);
    QObject::connect(&sh,&SCholder::scReload,&ssmm,&SamplerManager::resetAll);


    return QCoreApplication::exec();
}
