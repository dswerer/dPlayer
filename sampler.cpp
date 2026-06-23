#include "sampler.h"
#include"youreyes.h"

Sampler::Sampler( SCholder* s):m_ratio(1.),source(s),frameNow(-1),target(nullptr) {}

float Sampler::doSample(int frame){
    if(!target) return 0.;
    if(frame==frameNow) return m_buffer;
    dSTFT::Spectrum& t=*target;
    // return (t[frame][m_rangeR]-t[frame][m_rangeL])*m_ratio;
    const double* tp=t[frame];
    m_buffer=(tp[int(m_rangeR*source->sc.meta.binCount)]-tp[int(m_rangeL*source->sc.meta.binCount)])*m_ratio;
    frameNow=frame;
    // qDebug()<<m_buffer;
    return m_buffer;
}

void Sampler::setTarget(int index){
    target=source->sc.presum[index].get();
    targetChannel=index;
    qDebug()<<"sampler set to channel:"<<index;
}

int SamplerManager::rowCount(const QModelIndex &parent) const{
    Q_UNUSED(parent);
    return this->list.count();
}

QVariant SamplerManager::data(const QModelIndex &index,int role) const{
    // return QVariant::fromValue(0);
    if(!index.isValid()||index.row()<0||index.row()>=rowCount()){
        return QVariant();
    }
    if(role==NAMES||role==Qt::DisplayRole){
        return QVariant::fromValue(keys[index.row()]);
    }
    return QVariant();
}

QHash<int,QByteArray> SamplerManager::roleNames() const{
    return QHash<int,QByteArray>{
        {NAMES,"name"}
    };
}

bool SamplerManager::registerSampler(QString name){
    if(list.contains(name)) return false;
    // beginInsertRows();
    beginResetModel();
    keys.append(name);
    list.insert(name,new Sampler(scholder));
    endResetModel();
    ibuffer.reserve(keys.count());
    return true;
}

bool SamplerManager::deregisterSampler(){
    // if(!list.contains())
    beginResetModel();
    list.remove(keys[picked_key_index]);
    keys.removeAt(picked_key_index);
    picked_key_index=-1;
    delete picked;
    picked=nullptr;
    endResetModel();
    ibuffer.reserve(keys.count());
    return true;
}

bool SamplerManager::setSamplerPicked(QString name){
    if(!list.contains(name)) return false;
    this->picked=list.value(name);
    this->picked_key_index=keys.indexOf(name);
    emit p_ratioChanged();
    return true;
}

void SamplerManager::resync(int frame){
    if(frame==frameNow) return;
    frameNow=frame;
    for(int i=0;i<keys.size();i++){
        ibuffer[i]=list[keys[i]]->doSample(frame);
    }
    emit currentValueChanged();
}

void SamplerManager::resetAll(){
    for(int i=0;i<keys.size();i++){
        list[keys[i]]->target=nullptr;

    }
}



