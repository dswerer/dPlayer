#ifndef YOUREYES_H
#define YOUREYES_H
#include<QQuickItem>
#include"SCholder.h"
#include<QOpenGLFunctions>
#include<QOpenGLShaderProgram>
#include<QOpenGLVertexArrayObject>
#include<QOpenGLBuffer>
#include<QSize>
#include<QQmlApplicationEngine>
#include"shadermanager.h"
#include<QQuickWindow>
#include<QQuickOpenGLUtils>
#include"sampler.h"


class OpenGLBridge:public QQuickItem,protected QOpenGLFunctions{
    Q_OBJECT
    Q_PROPERTY(float animationValue READ animationValue WRITE setAnimationValue NOTIFY animationValueChanged)
    Q_PROPERTY(float mwidth READ mwidth WRITE setmwidth)
    Q_PROPERTY(float mheight READ mheight WRITE setmheight)
    Q_PROPERTY(float mamp READ mamp WRITE setMamp)
    Q_PROPERTY(float mzoom READ mzoom WRITE setMzoom)
    Q_PROPERTY(OpenGLBridge* ptr READ getself)
    QML_ELEMENT
public:
    enum MODE{
        SPECTRUM,WAVE,CUSTOMIZED
    };

    OpenGLBridge();
    ~OpenGLBridge();

    void setSpectrum(int i,SCholder* sc);
    void setShaderManager(ShaderManager* sm);
    void setShader(int ind);
    void setSamplerManager(SamplerManager* smp);

    float mwidth(){return m_width;}
    void setmwidth(float i){m_width=i;}
    float mheight(){return m_height;}
    void setmheight(float i){m_height=i;}
    float mamp(){return amp;}
    void setMamp(float i){amp=i;}
    float mzoom(){return zoom;}
    void setMzoom(float i){zoom=i;}
    OpenGLBridge* getself(){return this;}


    void setViewportSize(const QSize &size);

    void setAnimationValue(float value){
        m_animationValue=value;
        emit animationValueChanged();

        // 请求重绘窗口
        if (QQuickWindow *win = window()) {
            win->update();
        }
    }
    float animationValue(){return m_animationValue;}

    MODE mode;

    Q_INVOKABLE int getChannelLiteral(){return channelLiteral;}

    int channelId;
    int channelLiteral;
public slots:
    void sync();
    void paint();
signals:
    void animationValueChanged();
private:

    void initialize();
    void CleanUp();
    // void initGeometry();
    int spct_index();

    char m_initialized;

    SCholder* source;
    dSTFT::Spectrum* spct;

    ShaderManager* m_sm;
    int shaderId;

    QOpenGLVertexArrayObject m_vao;
    // GLuint m_vbo,m_ibo;
    QOpenGLBuffer m_vbo;

    SamplerManager* m_splm;


    float m_animationValue;
    GLuint m_texture;

    float m_width,m_height;
    float amp,zoom;

    QOpenGLShaderProgram* m_program;

    int frameNow;

};




class WindowManager:public QObject{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit WindowManager(QQmlApplicationEngine* engine=nullptr,QObject *parent=nullptr)
        :QObject(parent),m_engine(engine){

    }



    Q_INVOKABLE void openSpectrumWindow(int channelId,SCholder* sou,ShaderManager* sm,QObject* mp,SamplerManager* smp){
        qDebug() << "Engine:" << m_engine;
        qDebug() << "Import paths:" << m_engine->importPathList();
        QQmlComponent component(m_engine);
        qDebug() << "Component created";
        component.loadFromModule("visual_studio", "SpectrumWindow");
        qDebug() << "loadFromModule done";

        if (component.isError()) qDebug() << component.errorString();
        QQuickWindow* win = qobject_cast<QQuickWindow*>(component.create());
        if(!win){qWarning("Faled to create window 2");}
        win->setProperty("mediaPlayer",QVariant::fromValue(mp));
        if(win){
            OpenGLBridge *bridge=win->findChild<OpenGLBridge*>();
            win->setTitle(sou->m_scRoster[channelId]);
            if(bridge){
                bridge->setSpectrum(channelId,sou);
                bridge->setShaderManager(sm);
                bridge->setSamplerManager(smp);
                if(sou->m_scRoster[channelId].indexOf("raw")!=-1){
                    bridge->mode=OpenGLBridge::WAVE;
                    bridge->setShader(1);
                }
            }
            m_windows.insert((win));
            // qDebug()<<"#!##!#";
            connect(win,&QObject::destroyed,this,[this,win](){
                m_windows.erase(win);
            });

            win->show();
        }
    }
    Q_INVOKABLE void openShaderWindow(int channelId,SCholder* sou,ShaderManager* sm,QObject* mp,SamplerManager* smp,int shaderId){
        if(!sou->available){
            qDebug()<<"Sound Not Ready";
            return;
        }
        qDebug() << "Engine:" << m_engine;
        qDebug() << "Import paths:" << m_engine->importPathList();
        QQmlComponent component(m_engine);
        qDebug() << "Component created";
        component.loadFromModule("visual_studio", "ShaderWindow");
        qDebug() << "loadFromModule done";

        if (component.isError()) qDebug() << component.errorString();
        QQuickWindow* win = qobject_cast<QQuickWindow*>(component.create());
        if(!win){qWarning("Faled to create window 2");}
        win->setProperty("mediaPlayer",QVariant::fromValue(mp));
        if(win){
            OpenGLBridge *bridge=win->findChild<OpenGLBridge*>();
            win->setTitle(sou->m_scRoster[channelId]);
            if(bridge){
                bridge->mode=OpenGLBridge::CUSTOMIZED;
                bridge->setSpectrum(channelId,sou);
                bridge->setShaderManager(sm);
                bridge->setSamplerManager(smp);
                bridge->setShader(shaderId);
            }
            m_windows.insert((win));
            // qDebug()<<"#!##!#";
            connect(win,&QObject::destroyed,this,[this,win](){
                m_windows.erase(win);
            });

            win->show();
        }
    }

private:
    QQmlApplicationEngine* m_engine;
    std::set<QQuickWindow*> m_windows;

public slots:
    void deleteAll(){
        for(QQuickWindow* win:m_windows){
            auto children=win->findChildren<QQuickWindow*>();
            for(auto& child:children){
                child->destroy();
            }
            win->destroy();
        }
    }
};


#endif // YOUREYES_H
