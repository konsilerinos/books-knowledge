#include "AnkiConnector.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>

// todo: use ini file

CAnkiConnector::CAnkiConnector(QObject *parent)
    : QObject(parent),
    m_serverUrl("http://localhost:7777"),
    m_isConnected(false),
    m_isActive(false)
{
    m_networkManager = new QNetworkAccessManager(this);

    m_heartbeatTimer = new QTimer(this);
    m_heartbeatTimer->setInterval(3000);
    connect(m_heartbeatTimer, &QTimer::timeout, this, &CAnkiConnector::checkHeartbeat);

    connect(this, &CAnkiConnector::connected, this, &CAnkiConnector::onConnected);
}

CAnkiConnector::~CAnkiConnector()
{
    m_heartbeatTimer->stop();
}

void CAnkiConnector::ankiClientConnect()
{
    if (!m_isActive)
    {
        m_isActive = true;
        m_heartbeatTimer->start();

        checkHeartbeat();
    }
}

void CAnkiConnector::ankiClientDisconnect()
{
    if (m_isActive)
    {
        m_isActive = false;
        m_heartbeatTimer->stop();

        if (m_isConnected)
        {
            m_isConnected = false;

            m_modelsFieldsCache.clear();
            m_deckNamesCache.clear();

            emit disconnected();
        }
    }
}

bool CAnkiConnector::isConnected() const
{
    return m_isConnected;
}

void CAnkiConnector::addNote(const QString &deckName, const QString &modelName, const QVariantMap &fields)
{
    QVariantMap noteObj;
    noteObj["deckName"] = deckName;
    noteObj["modelName"] = modelName;
    noteObj["fields"] = fields;
    noteObj["tags"] = QVariantList{"tsunagi_qt"};

    QVariantMap params;
    params["note"] = noteObj;

    sendRequest("addNote", params, ReqAddNote);
}

void CAnkiConnector::addNotes(const QString &deckName, const QString &modelName, const QJsonArray &cardsArray, const QString &tagsStr)
{
    if (cardsArray.isEmpty()) return;

    m_batchExpected = cardsArray.size();
    m_batchProcessed = 0;
    m_batchErrors.clear();
    m_isBatchProcessing = true;

    // ОБЕРНУЛИ СТРОКУ В СПИСОК: теперь это массив из одного элемента
    QStringList singleTagList;
    if (!tagsStr.trimmed().isEmpty())
    {
        singleTagList.append(tagsStr.trimmed());
    }

    // Отправка в цикле
    for (const QJsonValue &cardValue : std::as_const(cardsArray))
    {
        QVariantMap noteObj;
        noteObj["deckName"] = deckName;
        noteObj["modelName"] = modelName;
        noteObj["fields"] = cardValue.toObject().toVariantMap();

        // Передаем список! В JSON это станет корректным массивом ["Твой::Тег"]
        noteObj["tags"] = singleTagList;

        QVariantMap params;
        params["note"] = noteObj;

        sendRequest("addNote", params, ReqAddNote);
    }
}

QMap<QString, QStringList> CAnkiConnector::getModelsAndFields() const
{
    return m_modelsFieldsCache;
}

QStringList CAnkiConnector::getDeckNames() const
{
    return m_deckNamesCache;
}

void CAnkiConnector::checkHeartbeat()
{
    if (!m_isActive)
    {
        return;
    }

    sendRequest("version", QVariantMap(), ReqHeartbeat);
}

void CAnkiConnector::sendRequest(const QString &action, const QVariantMap &params, int requestId, const QVariantMap &meta)
{
    QJsonObject payload;
    payload["action"] = action;
    payload["version"] = 6;

    if (!params.isEmpty())
    {
        payload["params"] = QJsonObject::fromVariantMap(params);
    }

    QNetworkRequest request((QUrl(m_serverUrl)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QByteArray jsonData = QJsonDocument(payload).toJson();
    QNetworkReply *reply = m_networkManager->post(request, jsonData);

    connect(reply, &QNetworkReply::finished, this, [this, reply, requestId, meta]()
    {
        bool networkOk = (reply->error() == QNetworkReply::NoError);
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());

        switch (requestId)
        {
            case ReqHeartbeat:
                handleHeartbeatResponse(networkOk, doc);
                break;
            case ReqAddNote:
                handleAddNoteResponse(networkOk, doc);
                break;
            case ReqGetModelNames:
                handleGetModelNamesResponse(networkOk, doc);
                break;
            case ReqGetModelFieldNames:
                handleGetModelFieldNamesResponse(networkOk, doc, meta);
                break;
            case ReqGetDeckNames:
                handleGetDeckNamesResponse(networkOk, doc);
                break;
            default:
                break;
        }

        reply->deleteLater();
    });
}

void CAnkiConnector::fetchModelFields(const QStringList &modelsToFetch)
{
    if (modelsToFetch.isEmpty())
    {
        return;
    }

    QStringList queue = modelsToFetch;
    QString currentModel = queue.takeFirst();

    QVariantMap params;
    params["modelName"] = currentModel;

    QVariantMap meta;
    meta["modelName"] = currentModel;
    meta["remainingModels"] = queue;

    sendRequest("modelFieldNames", params, ReqGetModelFieldNames, meta);
}

void CAnkiConnector::handleHeartbeatResponse(bool networkOk, const QJsonDocument &doc)
{
    bool serverResponding = false;

    if (networkOk && !doc.isNull() && doc.object().contains("result"))
    {
        serverResponding = true;
    }

    if (serverResponding && !m_isConnected)
    {
        m_isConnected = true;

        emit connected();
    }
    else if (!serverResponding && m_isConnected)
    {
        m_isConnected = false;

        emit disconnected();
    }
}

void CAnkiConnector::handleAddNoteResponse(bool networkOk, const QJsonDocument &doc)
{
    QString currentError = "";

    if (!networkOk)
    {
        currentError = "network error";
    }
    else
    {
        QJsonObject resObj = doc.object();
        if (resObj.contains("error") && !resObj["error"].isNull())
        {
            currentError = resObj["error"].toString();
        }
    }

    if (m_isBatchProcessing)
    {
        m_batchProcessed++;

        if (!currentError.isEmpty())
        {
            m_batchErrors.append(QString("Карточка %1: %2").arg(QString::number(m_batchProcessed), currentError));
        }

        if (m_batchProcessed >= m_batchExpected)
        {
            m_isBatchProcessing = false;

            if (m_batchErrors.isEmpty())
            {
                emit notesAddedSuccess();
            }
            else
            {
                emit notesAddedWithErrors(m_batchErrors);
            }

            m_batchExpected = 0;
            m_batchProcessed = 0;
            m_batchErrors.clear();
        }
    }
}

void CAnkiConnector::handleGetModelNamesResponse(bool networkOk, const QJsonDocument &doc)
{
    if (!networkOk)
    {
        return;
    }

    QJsonObject resObj = doc.object();

    if (!resObj.contains("error") || resObj["error"].isNull())
    {
        QJsonArray modelsArray = resObj["result"].toArray();
        QStringList modelsToFetch;

        for (const QJsonValue &value : std::as_const(modelsArray))
        {
            QString modelName = value.toString();

            if (modelName.contains("konsilerin", Qt::CaseInsensitive))
            {
                modelsToFetch.append(modelName);
            }
        }

        if (!modelsToFetch.isEmpty())
        {
            fetchModelFields(modelsToFetch);
        }
        else
        {
            sendRequest("deckNames", QVariantMap(), ReqGetDeckNames);
        }
    }
}

void CAnkiConnector::handleGetModelFieldNamesResponse(bool networkOk, const QJsonDocument &doc, const QVariantMap &meta)
{
    if (networkOk)
    {
        QJsonObject resObj = doc.object();

        if (!resObj.contains("error") || resObj["error"].isNull())
        {
            QJsonArray fieldsArray = resObj["result"].toArray();
            QStringList fieldsList = {};

            for (const QJsonValue &value : std::as_const(fieldsArray))
            {
                fieldsList.append(value.toString());
            }

            QString currentModel = meta["modelName"].toString();

            if (!currentModel.isEmpty())
            {
                m_modelsFieldsCache[currentModel] = fieldsList;
            }
        }
    }

    QStringList remainingModels = meta["remainingModels"].toStringList();

    if (!remainingModels.isEmpty())
    {
        fetchModelFields(remainingModels);
    }
    else
    {
        sendRequest("deckNames", QVariantMap(), ReqGetDeckNames);
    }
}

void CAnkiConnector::handleGetDeckNamesResponse(bool networkOk, const QJsonDocument &doc)
{
    if (networkOk)
    {
        QJsonObject resObj = doc.object();

        if (!resObj.contains("error") || resObj["error"].isNull())
        {
            QJsonArray decksArray = resObj["result"].toArray();
            m_deckNamesCache.clear();

            for (const QJsonValue &value : std::as_const(decksArray))
            {
                m_deckNamesCache.append(value.toString());
            }
        }
    }

    emit readyToWork();
}

void CAnkiConnector::onConnected()
{
    m_modelsFieldsCache.clear();

    sendRequest("modelNames", QVariantMap(), ReqGetModelNames);
}
