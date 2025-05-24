#include "ButtonTipManager.h"
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

ButtonTipManager* ButtonTipManager::s_instance = nullptr;
const QString ButtonTipManager::API_ENDPOINT = "https://m1.apifoxmock.com/m1/6237106-5930859-default/app/buttontips";

ButtonTipManager* ButtonTipManager::instance()
{
    if (!s_instance) {
        // Parent to qApp ensures cleanup during application shutdown
        s_instance = new ButtonTipManager(qApp);
    }
    return s_instance;
}

ButtonTipManager::ButtonTipManager(QObject* parent)
    : QObject(parent), m_tipsLoaded(false)
{
    m_networkManager = new QNetworkAccessManager(this);
    
    // Auto-load tips on startup
    loadTipsFromServer();
}

ButtonTipInfo ButtonTipManager::getButtonTip(const QString& buttonId) const
{
    return m_buttonTips.value(buttonId, {"默认标题", "默认描述"});
}

void ButtonTipManager::reloadTips()
{
    m_tipsLoaded = false;
    loadTipsFromServer();
}

void ButtonTipManager::loadTipsFromServer()
{
    QNetworkRequest request(API_ENDPOINT);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    // Add CORS headers for cross-origin requests
    request.setRawHeader("Accept", "application/json");
    
    QNetworkReply* reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, &ButtonTipManager::onNetworkReplyFinished);
    
    qDebug() << "Loading button tips from:" << API_ENDPOINT;
}

void ButtonTipManager::onNetworkReplyFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) {
        return;
    }
    
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        parseTipsResponse(data);
        m_tipsLoaded = true;
        emit tipsLoaded();
        qDebug() << "Button tips loaded successfully";
    } else {
        QString errorMsg = QString("Failed to load button tips: %1").arg(reply->errorString());
        qWarning() << errorMsg;
        emit tipsLoadFailed(errorMsg);
        
        // Load default tips as fallback
        m_buttonTips.clear();
        m_buttonTips["selectionbutton"] = {"选择工具", "可以选择画布上已绘制的图形，方便进行移动、编辑等操作"};
        m_buttonTips["linedrawbutton"] = {"直线绘制", "在画布上绘制直线，按住鼠标左键拖动即可绘制"};
        m_buttonTips["freehandlinedrawbutton"] = {"自由线绘制", "在画布上自由绘制线条，跟随鼠标移动轨迹绘制"};
        m_buttonTips["rectdrawbutton"] = {"矩形绘制", "在画布上绘制矩形，按住鼠标左键拖动即可绘制"};
        m_buttonTips["quadrilateraldrawbutton"] = {"四边形绘制", "在画布上绘制四边形，按住鼠标左键拖动即可绘制"};
        m_buttonTips["pentagondrawbutton"] = {"五边形绘制", "在画布上绘制五边形，按住鼠标左键拖动即可绘制"};
        m_buttonTips["stardrawbutton"] = {"五角星绘制", "在画布上绘制五边形，按住鼠标左键拖动即可绘制"};
        m_buttonTips["zoominbutton"] = {"缩小按钮", "用于缩小画布中的图形，使用滚轮可进行缩放"};
        m_buttonTips["zoomoutbutton"] = {"放大按钮", "用于放大画布中的图形，使用滚轮可以进行缩放"};
        
        m_tipsLoaded = true;
        emit tipsLoaded();
    }
    
    reply->deleteLater();
}

void ButtonTipManager::parseTipsResponse(const QByteArray& data)
{
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << parseError.errorString();
        return;
    }
    
    if (!doc.isObject()) {
        qWarning() << "Invalid JSON format: expected object";
        return;
    }
    
    QJsonObject rootObj = doc.object();
    m_buttonTips.clear();
    
    // Parse each button tip entry
    for (auto it = rootObj.begin(); it != rootObj.end(); ++it) {
        QString buttonId = it.key();
        QJsonValue value = it.value();
        
        if (value.isObject()) {
            QJsonObject tipObj = value.toObject();
            ButtonTipInfo info;
            info.title = tipObj["title"].toString();
            info.text = tipObj["text"].toString();
            
            m_buttonTips[buttonId] = info;
            qDebug() << "Loaded tip for button:" << buttonId << "title:" << info.title;
        }
    }
} 