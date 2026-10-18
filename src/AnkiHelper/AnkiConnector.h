#ifndef ANKICONNECTOR_H
#define ANKICONNECTOR_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>

class CAnkiConnector : public QObject
{
    Q_OBJECT

    enum RequestType
    {
        ReqHeartbeat = 1,
        ReqAddNote = 2,
        ReqGetModelNames = 3,
        ReqGetModelFieldNames = 4,
        ReqGetDeckNames = 5
    };

public:
    explicit CAnkiConnector(QObject *parent = nullptr);
    virtual ~CAnkiConnector();

    void ankiClientConnect();
    void ankiClientDisconnect();

    bool isConnected() const;

    void addNote(const QString &deckName, const QString &modelName, const QVariantMap &fields);
    void addNotes(const QString &deckName, const QString &modelName, const QJsonArray &cardsArray, const QString &tagsStr);

    QMap<QString, QStringList> getModelsAndFields() const;
    QStringList getDeckNames() const;

signals:
    void connected();
    void disconnected();

    void noteAddedSuccess(long long noteId);
    void errorMessage(const QString &errorMsg);

    void readyToWork();

    void notesAddedSuccess();
    void notesAddedWithErrors(const QStringList &errorsList);

private slots:
    void checkHeartbeat();
    void onConnected();

private:
    void sendRequest(const QString &action, const QVariantMap &params, int requestId, const QVariantMap &meta = QVariantMap());
    void fetchModelFields(const QStringList &modelsToFetch);

    void handleHeartbeatResponse(bool networkOk, const QJsonDocument &doc);
    void handleAddNoteResponse(bool networkOk, const QJsonDocument &doc);
    void handleGetModelNamesResponse(bool networkOk, const QJsonDocument &doc);
    void handleGetModelFieldNamesResponse(bool networkOk, const QJsonDocument &doc, const QVariantMap &meta);
    void handleGetDeckNamesResponse(bool networkOk, const QJsonDocument &doc);

private:
    QNetworkAccessManager *m_networkManager = nullptr;
    QTimer *m_heartbeatTimer = nullptr;

    QMap<QString, QStringList> m_modelsFieldsCache = {};

    QStringList m_deckNamesCache = {};
    QStringList m_batchErrors = {};

    const QString m_serverUrl;

    int m_batchExpected = 0;
    int m_batchProcessed = 0;

    bool m_isBatchProcessing = false;
    bool m_isConnected = false;
    bool m_isActive = false;

};

#endif // ANKICONNECTOR_H
