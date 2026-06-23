#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <QAbstractListModel>
#include<QOpenGLShaderProgram>
#include<QDir>
#include<QFile>
#include<QFileInfo>
#include<QQmlApplicationEngine>

class ShaderManager : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(ShaderManager* ptr READ getself)
    QML_ELEMENT
public:
    explicit ShaderManager(QQmlApplicationEngine *engine=nullptr,QObject *parent = nullptr);

    // explicit ShaderManager(QObject *parent = nullptr,QQmlApplicationEngine *engine=nullptr){

    // };

    QString uniformalVertexShader=R"(
    #version 330
    void main(){
        gl_Position=vec4(4.*(gl_VertexID&2)-1.,4.*(gl_VertexID&1)-1.,0.,1.);
    })";



    Q_INVOKABLE void registerShaderFromFile(QString path);
    Q_INVOKABLE void registerShaderFromSource(QByteArray src,QByteArray name);
    Q_INVOKABLE void registerShaderFromUrl(QUrl url){
        registerShaderFromFile(url.toLocalFile());
    }
    // void deregisterShaderFrom();
    // void deregisterShader(QString name);
    QOpenGLShaderProgram* getShader(int index);
    ShaderManager* getself(){return this;}

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    std::vector<QByteArray> FragShaderList;
    std::vector<QString> shaderRoster;

    QQmlApplicationEngine* engine;
};

#endif // SHADERMANAGER_H
