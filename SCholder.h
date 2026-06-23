#ifndef SCHOLDER_H
#define SCHOLDER_H

#include <QObject>
#include <vector>
#include <QString>
#include <QAbstractListModel>
#include "./Dependencies/STFT/stft.h"
#include <QUrl>
#include<QQmlApplicationEngine>



class SCholder : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(std::vector<QString>* scRoster READ scRoster NOTIFY scRosterChanged FINAL)
    Q_PROPERTY(SCholder* ptr READ getself CONSTANT)

public:
    explicit SCholder(QObject *parent = nullptr,QQmlApplicationEngine* engine=nullptr);
    Q_INVOKABLE void loadsc(QString path,dSTFT::LoadMode mode);
    Q_INVOKABLE void loadscFromURL(QUrl u,dSTFT::LoadMode mode);

    std::vector<QString>* scRoster(){return &m_scRoster;}
    SCholder* getself(){return const_cast<SCholder*>(this);}

    std::vector<QString> m_scRoster;
    std::vector<dSTFT::Spectrum*> m_sclist;
    std::vector<std::unique_ptr<dSTFT::Spectrum>> raws;
    dSTFT::SpctCollected sc;

/*-------------------ListModel----------------------*/
    int rowCount(const QModelIndex &parent=QModelIndex()) const override;
    QVariant data(const QModelIndex &index,int role=Qt::DisplayRole) const override;
    QHash<int,QByteArray> roleNames() const override;
/*-------------------ListModel----------------------*/

    Q_INVOKABLE void printList(){
        for(auto& s:m_scRoster){
            qDebug()<<s<<'\n';
        }
    }

    QQmlApplicationEngine *engine;

    bool available;

signals:
    void scRosterChanged();
    void scReload();

public slots:
    const dSTFT::Spectrum* getSpct(int ind);
};



#endif // SCHOLDER_H
