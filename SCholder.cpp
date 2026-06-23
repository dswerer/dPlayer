#include "SCholder.h"
SCholder::SCholder(QObject *parent,QQmlApplicationEngine *engine)
    : QAbstractListModel(parent),engine(engine),available(false)
{}

void SCholder::loadsc(QString path,dSTFT::LoadMode m){
    dSTFT::SoundFile sf(path.toStdString().c_str());
    dSTFT::Loader ld(sf,1024,2048);
    ld.cdt.doLog=false;
    ld.cdt.boostHF=true;
    ld.setWindow(dSTFT::hann);
    this->sc=ld.Load(m);
    if(!sc.valid){
        return;
    }

    // beginInsertRows(QModelIndex(),0,sc.channel.size()*3);
    beginResetModel();
    emit scReload();
    m_sclist.clear();
    m_scRoster.clear();
    raws.clear();
    for(int i=0;i<sc.channel.size();i++){
        this->raws.push_back(ld.getRawPCM(i));
    }
    for(int i=0;i<sc.channel.size();i++){
        this->m_sclist.push_back(raws[i].get());
        this->m_scRoster.push_back("raw/channel-"+QString::number(i));
        // qDebug()<<"!!!!:"<<m_sclist[0]->meta().binCount;
        this->m_sclist.push_back(sc.channel[i].get());
        this->m_scRoster.push_back("spectrum/channel-"+QString::number(i));
        if(i>=sc.presum.size()) continue;
        this->m_sclist.push_back(sc.presum[i].get());
        this->m_scRoster.push_back("presum/channel-"+QString::number(i));
    }
    endResetModel();
    available=true;

}

void SCholder::loadscFromURL(QUrl u,dSTFT::LoadMode m){
    qDebug()<<u.toLocalFile();

    loadsc(u.toLocalFile(),m);
}

const dSTFT::Spectrum* SCholder::getSpct(int i){
    return m_sclist[i]->copyHandle();
}



int SCholder::rowCount(const QModelIndex& parent) const{
    Q_UNUSED(parent);
    // if(parent.isValid()){
    //     return 0;
    // }
    return m_scRoster.size();
}

QVariant SCholder::data(const QModelIndex &index,int role) const{
    if(!index.isValid()||index.row()<0||index.row()>=rowCount()){
        return QVariant();
    }
    if(role==Qt::DisplayRole||role==Qt::UserRole+1){
        return m_scRoster.at(index.row());
        // return 10;
    }
    return QVariant();
}

QHash<int,QByteArray> SCholder::roleNames() const{
    QHash<int,QByteArray> table;
    table.insert(Qt::UserRole+1,"name");
    return table;
}