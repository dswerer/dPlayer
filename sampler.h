#ifndef SAMPLER_H
#define SAMPLER_H
#include<QObject>
#include"SCholder.h"
// #include"youreyes.h"
// class OpenGLBridge;
class Sampler
{
    // Q_OBJECT
    // // Q_PROPERTY(float ratio READ ratio WRITE setRatio)
    // QML_ELEMENT
public:
    Sampler(QObject* parent=nullptr):parent(parent){}
    Sampler(SCholder* p=nullptr);
    // ~Sampler();
    float doSample(int frame);
    void setTarget(int index);
    void setRange(int l,int r){m_rangeL=l;m_rangeR=r;}

    float ratio(){return m_ratio;}
    void setRatio(float r){
        m_ratio=r;
    }

    SCholder* source;
    dSTFT::Spectrum* target;
    int targetChannel;

    float m_rangeL,m_rangeR;
    float m_ratio;

    int frameNow;
    float m_buffer;

    QObject* parent;
};

class SamplerManager:public QAbstractListModel{
    Q_OBJECT
    Q_PROPERTY(float p_ratio READ p_ratio WRITE setP_ratio NOTIFY p_ratioChanged)
    Q_PROPERTY(float p_rangeL READ p_rangeL WRITE setP_rangeL)
    Q_PROPERTY(float p_rangeR READ p_rangeR WRITE setP_rangeR)
    // Q_PROPERTY(int p_target WRITE setP_target)
    Q_PROPERTY(float currentValue READ currentValue NOTIFY currentValueChanged)
    Q_PROPERTY(SamplerManager* ptr READ getself)
    QML_ELEMENT
public:
    enum ROLES{
        NAMES=Qt::UserRole+1
    };

    SamplerManager(QQmlApplicationEngine* e=nullptr,QObject* parent=nullptr)
        :engine(e),picked(nullptr),parent(parent),frameNow(-1){
        registerSampler("sampler_0");
        setSamplerPicked("sampler_0");
    }
    SamplerManager(QQmlApplicationEngine* e=nullptr,SCholder* sch=nullptr,QObject* parent=nullptr)
        :engine(e),picked(nullptr),parent(parent),frameNow(-1),scholder(sch){
        registerSampler("sampler_0");
        setSamplerPicked("sampler_0");
    }
    // SamplerManager(QQmlApplicationEngine* e=nullptr):engine(e),picked(nullptr){}

/*------------listModel--------------*/
    int rowCount(const QModelIndex &parent=QModelIndex()) const override;
    QVariant data(const QModelIndex &index,int role=Qt::DisplayRole) const override;
    QHash<int,QByteArray> roleNames() const override;
/*------------listModel--------------*/

    float p_ratio(){if(!picked) return 0; return picked->m_ratio;}
    void setP_ratio(float i){if(!picked) return; picked->m_ratio=i;}
    float p_rangeL(){if(!picked) return 0; return picked->m_rangeL;}
    void setP_rangeL(float i){if(!picked) return; picked->m_rangeL=i;}
    float p_rangeR(){
        if(!picked) return 0;
        return picked->m_rangeR;}
    void setP_rangeR(float i){if(!picked) return; picked->m_rangeR=i;}
    Q_INVOKABLE void p_target(int ogb){
        if(picked)
        picked->setTarget(ogb);
    }
    // float p_value(){
    //     return ibuffer[picked_key_index];
    // }
    float currentValue(){
        // qDebug()<<ibuffer[picked_key_index];
        return ibuffer[picked_key_index];
    }

    SamplerManager* getself(){
        return this;
    }

    Q_INVOKABLE void printAll(){
        for(QString s:keys){
            qDebug()<<s;
        }
    }
    Q_INVOKABLE void printPicked(){
        qDebug()<<keys[picked_key_index];
    }


    Q_INVOKABLE bool registerSampler(QString name);
    Q_INVOKABLE bool deregisterSampler();
    Q_INVOKABLE bool setSamplerPicked(QString name);
    SCholder* scholder;
    QHash<QString,Sampler*> list;
    QList<QString> keys;
    Sampler* picked;
    int picked_key_index;

    int frameNow;

    std::vector<float> ibuffer;

    QObject* parent;
    QQmlApplicationEngine* engine;

public slots:
    void resync(int index);
    void resetAll();
signals:
    void currentValueChanged();
    void p_ratioChanged();
};

#endif // SAMPLER_H
