#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>

struct ButtonTipInfo {
    QString title;
    QString text;
};

class ButtonTipManager : public QObject
{
    Q_OBJECT

public:
    static ButtonTipManager* instance();
    
    // Get button tip info by button id
    ButtonTipInfo getButtonTip(const QString& buttonId) const;
    
    // Check if tips are loaded
    bool isTipsLoaded() const { return m_tipsLoaded; }
    
    // Force reload tips from server
    void reloadTips();

signals:
    void tipsLoaded();
    void tipsLoadFailed(const QString& error);

private slots:
    void onNetworkReplyFinished();

private:
    explicit ButtonTipManager(QObject* parent = nullptr);
    ~ButtonTipManager() = default;
    
    void loadTipsFromServer();
    void parseTipsResponse(const QByteArray& data);
    
    static ButtonTipManager* s_instance;
    QNetworkAccessManager* m_networkManager;
    QMap<QString, ButtonTipInfo> m_buttonTips;
    bool m_tipsLoaded;
    
    // API endpoint for button tips
    static const QString API_ENDPOINT;
}; 