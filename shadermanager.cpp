#include "shadermanager.h"

ShaderManager::ShaderManager(QQmlApplicationEngine* eng,QObject *parent)
    : QAbstractListModel(parent),engine(eng)
{
    QString defaultFragmentShader=R"(
    #version 330
    uniform float m_time;
    uniform sampler2D spct;
    uniform vec2 m_resolution;
    uniform float zoom;
    uniform float ampRatio;
    uniform float picked_R;
    uniform float picked_L;
    out vec4 FragColor;
    void main(){
        vec2 uv=gl_FragCoord.xy/m_resolution;
        uv.x*=zoom;
        float mel=uv.x*uv.x;
        float amp=(texture(spct,vec2(mel,0)).r)*20*ampRatio;
        float dense=(gl_FragCoord.y-amp<0)?1.:0.;
        dense=min(dense,1.);
        if(abs(uv.x-picked_R)<=0.001*zoom){
            FragColor=vec4(1.,0.,0.,1.);
            return;
        }
        if(abs(uv.x-picked_L)<=0.001*zoom){
            FragColor=vec4(1.,1.,0.,1.);
            return;
        }

        FragColor=vec4(mix(vec3(0.,0.7,1.),vec3(0.,1.,0.7),uv.y)*dense,1.);
    })";
    registerShaderFromSource(defaultFragmentShader.toUtf8(),"Spectrum_Default_Frag");

    QString WavFragmentShader=R"(
    #version 330
    uniform float m_time;
    uniform sampler2D spct;
    uniform vec2 m_resolution;
    uniform float zoom;
    uniform float ampRatio;
    out vec4 FragColor;

    vec3 Image(vec2 fragC){
        vec2 uv=fragC/m_resolution;
        uv.y-=0.5;
        uv.y*=2.;
        uv.x*=zoom;
        float ampr=(texture(spct,vec2(uv.x+1/m_resolution.x,0)).r)*20*ampRatio;
        float ampl=(texture(spct,vec2(uv.x-1/m_resolution.x,0)).r)*20*ampRatio;
        float damp=dFdx(texture(spct,vec2(uv.x,0.)).r*20*ampRatio);
        float dense=0.005/(max(abs(uv.y-(ampr+ampl)/2.)*(1.+damp)-abs(ampr-ampl)/2.,0.));
        return vec3(0.5,0.7,1.)*min(dense,1.5);
    }
    #define K_S 5
    void main(){
        vec3 o=vec3(0.);
        for(int i=-K_S/2;i<K_S/2;i++){
            for(int j=-K_S/2;j<K_S/2;j++){
                o+=Image(gl_FragCoord.xy+vec2(2*i,2*j));
            }
        }
        FragColor=vec4(o/K_S/K_S+Image(gl_FragCoord.xy),1.);
    }
    )";
    registerShaderFromSource(WavFragmentShader.toUtf8(),"Wave_Defualt_Frag");

    registerShaderFromFile(":/shader/example");
}

void ShaderManager::registerShaderFromFile(QString path){
    beginResetModel();
    QByteArray nsrc;
    QFile f(path);
    if(!f.open(QIODevice::ReadOnly|QIODevice::Text)){
        qDebug()<<"着色器文件不存在";
    }
    nsrc=f.readAll();
    this->FragShaderList.push_back(std::move(nsrc));
    QString fileN=f.fileName().mid(f.fileName().lastIndexOf("/")+1);
    this->shaderRoster.push_back(fileN);
    endResetModel();
}

void ShaderManager::registerShaderFromSource(QByteArray src,QByteArray name){
    beginResetModel();
    this->shaderRoster.push_back(name);
    this->FragShaderList.push_back(QByteArray(src));
    endResetModel();
}

QOpenGLShaderProgram* ShaderManager::getShader(int ind){
    QOpenGLShaderProgram* p=new QOpenGLShaderProgram();
    if(!p->addShaderFromSourceCode(QOpenGLShader::Vertex,this->uniformalVertexShader)){
        qDebug()<<"VERTEX FAILED:"<<p->log();
    }
    if(!p->addShaderFromSourceCode(QOpenGLShader::Fragment,this->FragShaderList[ind])){
        qDebug()<<"FRAGMENT FAILED:"<<p->log();
    }
    if(!p->link()){
        qDebug()<<"LINK FAILED:"<<p->log();
    }
    return p;
}

int ShaderManager::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return FragShaderList.size();

    // FIXME: Implement me!
}

QVariant ShaderManager::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role==Qt::DisplayRole){
        return QVariant::fromValue(shaderRoster[index.row()]);
    }

    // FIXME: Implement me!
    return QVariant();
}
