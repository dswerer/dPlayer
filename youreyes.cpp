#include "youreyes.h"
#include<QThread>
#include<algorithm>
OpenGLBridge::OpenGLBridge(){
    // this->source=sch;
    // this->spct=source->m_sclist[i];
    setFlag(ItemHasContents,true);
    m_sm=nullptr;
    shaderId=0;
    m_initialized=0;
    frameNow=0;
    zoom=1.;
    amp=1.;
    mode=SPECTRUM;
    connect(this,&QQuickItem::windowChanged,this,[this]{
        if(QQuickWindow* win=this->window()){
            connect(win,&QQuickWindow::beforeSynchronizing,this,&OpenGLBridge::sync,Qt::DirectConnection);
            connect(win,&QQuickWindow::afterRendering,this,&OpenGLBridge::paint,Qt::DirectConnection);
            win->setColor(Qt::black);
        }
    },Qt::DirectConnection);
}

OpenGLBridge::~OpenGLBridge(){}

void OpenGLBridge::setSpectrum(int i,SCholder* sc){
    this->channelId=i;
    int p=sc->m_scRoster[i].lastIndexOf("-");
    QString ns=sc->m_scRoster[i].mid(p+1);
    this->channelLiteral=ns.toInt();
    this->source=sc;
    this->spct=sc->m_sclist[i];
}
void OpenGLBridge::setShaderManager(ShaderManager* sm){
    m_sm=sm;
}
void OpenGLBridge::setSamplerManager(SamplerManager* smp){
    this->m_splm=smp;
}

void OpenGLBridge::setShader(int ind){
    if(!m_sm){
        qWarning("OpenGLBridge: ShaderManager not accessible");
        return;
    }
    shaderId=ind;
}

void OpenGLBridge::initialize(){
    window()->beginExternalCommands();
    initializeOpenGLFunctions();
    // qDebug()<<"!!!"<<spct->presentFrame();
    glGenTextures(1,&m_texture);
    glBindTexture(GL_TEXTURE_2D,m_texture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    if(mode==SPECTRUM){
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    }else if(mode==WAVE||mode==CUSTOMIZED){
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    }
    glTexImage2D(GL_TEXTURE_2D,0,GL_R32F,spct->meta().binCount,1,0,GL_RED,GL_FLOAT,(void*)0);
    glBindTexture(GL_TEXTURE_2D,0);
    // qDebug()<<"!!!2";

    m_program=m_sm->getShader(shaderId);

    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);

    m_vao.create();
    // qDebug()<<"!!!3";
    m_initialized=true;
    window()->endExternalCommands();
}



int OpenGLBridge::spct_index(){
    return int(m_animationValue*0.001*source->sc.meta.samplerate)/source->sc.meta.stride;
}

void OpenGLBridge::sync(){
    window()->beginExternalCommands();
    if(!spct) setSpectrum(this->channelId,source);
    if(!m_initialized) initialize();

    m_splm->resync(spct_index());

    glViewport(0,0,m_width,m_height);


    if(spct_index()!=frameNow){
        // qDebug()<<"load spct"<<spct_index();
        if(mode==SPECTRUM||mode==WAVE){
            glBindTexture(GL_TEXTURE_2D,m_texture);
            std::vector<float> buf((*spct)[spct_index()],(*spct)[spct_index()]+spct->meta().binCount);//为了转换为float
            glTexSubImage2D(GL_TEXTURE_2D,0,0,0,spct->meta().binCount,1,GL_RED,GL_FLOAT,buf.data());
            frameNow=spct_index();
        }
        else if(mode==CUSTOMIZED){
            glBindTexture(GL_TEXTURE_2D,m_texture);
            std::vector<float> buf(spct->meta().binCount, 0.0f);
            const int n = spct->meta().binCount;
            const int c = source->sc.channel.size();
            const int ind = spct_index();

            for (int i = 0; i < c; ++i) {
                dSTFT::Spectrum& sp = *(source->sc.channel[i].get());
                const double* v = sp[ind];
                // int ns=sp.meta().binCount;
                std::transform(buf.begin(),buf.begin()+n,v,buf.begin(),
                               [](float a, double b) {
                                   return a + static_cast<float>(b);
                               });
            }
            glTexSubImage2D(GL_TEXTURE_2D,0,0,0,spct->meta().binCount,1,GL_RED,GL_FLOAT,buf.data());
            frameNow=spct_index();
        }

        window()->endExternalCommands();

    }
}

void OpenGLBridge::paint(){
    if(!m_program->isLinked()){
        qDebug()<<"#### Shader Program Not Linked ####";
    }
    // qDebug() << "paint() begin";
    QQuickWindow *w = window();
    if (!w) return;
    w->beginExternalCommands();
    glClearColor(1,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);
    m_program->bind();

    m_program->setUniformValue("m_time",m_animationValue);
    m_program->setUniformValue("m_resolution",QVector2D(m_width,m_height));
    m_program->setUniformValue("ampRatio",amp);
    m_program->setUniformValue("zoom",zoom);
    if(mode==CUSTOMIZED){
        for(int i=0;i<m_splm->keys.size();i++){
            m_program->setUniformValue(m_splm->keys[i].toLocal8Bit(),m_splm->ibuffer[i]);
        }
    }
    if(mode==WAVE||mode==SPECTRUM){
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,m_texture);
        m_program->setUniformValue("spct",0);
        if(m_splm->picked) {
            // qDebug()<<m_splm->picked->targetChannel<<"-"<<channelLiteral;
            if(m_splm->picked->targetChannel==channelLiteral){
                m_program->setUniformValue("picked_R",m_splm->p_rangeR());
                // qDebug()<<m_splm->p_rangeR();
                m_program->setUniformValue("picked_L",m_splm->p_rangeL());
            }else{
                m_program->setUniformValue("picked_R",-0.5f);
                // qDebug()<<m_splm->p_rangeR();
                m_program->setUniformValue("picked_L",-0.5f);
            }
        }
        else{
            m_program->setUniformValue("picked_R",-0.5f);
            // qDebug()<<m_splm->p_rangeR();
            m_program->setUniformValue("picked_L",-0.5f);
        }
    }
    m_vao.bind();
    glDrawArrays(GL_TRIANGLES,0,3);

    w->endExternalCommands();
    // qDebug() << "paint() after clear";
}

void OpenGLBridge::CleanUp(){
    delete m_program;
}