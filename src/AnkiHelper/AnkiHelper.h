#ifndef ANKIHELPER_H
#define ANKIHELPER_H

#include <QObject>
#include <QString>
#include <QStringList>

class CAnkiHelper : public QObject
{
    Q_OBJECT

public:
    enum CardType
    {
        Unknown = 0,
        EnglishWord,
        FillBlank,
        Simple
    };
    Q_ENUM(CardType)

public:
    explicit CAnkiHelper(QObject *parent = nullptr);
    virtual ~CAnkiHelper();

    static QString cardTypeToString(CardType type);
    static CardType cardTypeFromString(const QString &typeStr);

    static QStringList getAllowedTypeStrings();

    static QString getFieldSplitter();
    static QString getCardSplitter();

    // todo: rename
    QString processCardText(const QString &rawText, const QString &modelType);
    QString parseToJson(const QString &processedText, const QString &modelType);

signals:
    void errorMessage(const QString &errorMsg);

protected:
    QString processCardTypeEnglishWord(const QString &rawText);
    QString processCardTypeFillBlank(const QString &rawText);
    QString processCardTypeSimple(const QString &rawText);

    QString jsonFieldsEnglishWord(const QString &processedText);
    QString jsonFieldsFillBlank(const QString &processedText);
    QString jsonFieldsSimple(const QString &processedText);
};

#endif // ANKIHELPER_H
