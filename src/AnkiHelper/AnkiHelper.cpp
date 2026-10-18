#include "AnkiHelper.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

CAnkiHelper::CAnkiHelper(QObject *parent) : QObject(parent)  {}

CAnkiHelper::~CAnkiHelper() {}

QString CAnkiHelper::cardTypeToString(CardType type)
{
    switch (type)
    {
        case EnglishWord: return "1. Konsilerin английское слово";
        case FillBlank:   return "2. Konsilerin задание с пропуском";
        case Simple:      return "3. Konsilerin простая";
        default:          return "";
    }
}

CAnkiHelper::CardType CAnkiHelper::cardTypeFromString(const QString &typeStr)
{
    if (typeStr == "1. Konsilerin английское слово")    return EnglishWord;
    if (typeStr == "2. Konsilerin задание с пропуском") return FillBlank;
    if (typeStr == "3. Konsilerin простая")             return Simple;
    return Unknown;
}

QStringList CAnkiHelper::getAllowedTypeStrings()
{
    return {
        cardTypeToString(EnglishWord),
        cardTypeToString(FillBlank),
        cardTypeToString(Simple)
    };
}

QString CAnkiHelper::getFieldSplitter()
{
    return "---";
}

QString CAnkiHelper::getCardSplitter()
{
    return "===";
}

QString CAnkiHelper::processCardText(const QString &rawText, const QString &modelType)
{
    QString result = "";
    CardType type = cardTypeFromString(modelType);

    if (type == Unknown)
    {
        emit errorMessage("Unsupported card type");
        return result;
    }

    switch (type)
    {
        case EnglishWord:
            result = processCardTypeEnglishWord(rawText);
            break;
        case FillBlank:
            result = processCardTypeFillBlank(rawText);
            break;
        case Simple:
            result = processCardTypeSimple(rawText);
            break;
        default:
            break;
    }

    return result;
}

QString CAnkiHelper::parseToJson(const QString &processedText, const QString &modelType)
{
    CardType type = cardTypeFromString(modelType);

    if (type == Unknown)
    {
        emit errorMessage("Cannot parse to JSON: Unsupported card type");
        return QString();
    }

    // 1. ЖЕЛЕЗОБЕТОННО убираем Windows-переносы строк (\r\n -> \n) на самом входе
    QString cleanText = processedText;
    cleanText.replace("\r\n", "\n");

    // 2. Разбиваем весь текст на отдельные карточки по разделителю "==="
    QStringList cardsList = cleanText.split(getCardSplitter());
    QJsonArray finalCardsArray;

    for (const QString &singleCardText : std::as_const(cardsList))
    {
        // Очищаем края карточки от случайных пустых строк
        QString cleanCardText = singleCardText.trimmed();
        if (cleanCardText.isEmpty())
        {
            continue;
        }

        QJsonObject fieldsObj;

        if (type == Simple)
        {
            QString field1 = "";
            QString field2 = "";

            // ПРОВЕРКА: Если в блоке карточки НЕТ явного разделителя "---"
            if (!cleanCardText.contains(getFieldSplitter()))
            {
                // Разрезаем по строкам
                QStringList lines = cleanCardText.split('\n');
                QStringList validLines;

                for (const QString &line : lines)
                {
                    if (!line.trimmed().isEmpty())
                    {
                        validLines.append(line.trimmed());
                    }
                }

                // Если строк строго 2 — это однострочные Вопрос и Ответ без "---"
                if (validLines.size() == 2)
                {
                    field1 = validLines.at(0);
                    field2 = validLines.at(1);
                }
                else
                {
                    emit errorMessage("Ошибка: В карточке нет '---'. Для однострочного формата должно быть строго 2 строки.");
                    return QString("-");
                }
            }
            // СТАНДАРТНЫЙ ВАРИАНТ: Разделитель "---" присутствует явным образом
            else
            {
                QStringList fields = cleanCardText.split(getFieldSplitter());
                field1 = (fields.size() > 0) ? fields.at(0).trimmed() : QString();
                field2 = (fields.size() > 1) ? fields.at(1).trimmed() : QString();
            }

            // ФИНАЛЬНАЯ ЗАМЕНА: Превращаем обычные переносы строк в HTML-теги для Anki
            field1.replace("\n", "<br>");
            field2.replace("\n", "<br>");

            fieldsObj["Front"] = field1;
            fieldsObj["Back"] = field2;
            fieldsObj["Todo"] = "";
            fieldsObj["Answer-note"] = "";
        }
        else if (type == EnglishWord)
        {
            // Для английского слова замена <br> уже встроена внутри jsonFieldsEnglishWord
            QString singleCardJson = jsonFieldsEnglishWord(cleanCardText);

            if (singleCardJson == "-")
            {
                return QString("-");
            }

            QJsonDocument cardDoc = QJsonDocument::fromJson(singleCardJson.toUtf8());
            fieldsObj = cardDoc.object();
        }
        else if (type == FillBlank)
        {
            emit errorMessage("JSON parser for Fill Blank is not implemented yet");
            return QString("-");
        }

        finalCardsArray.append(fieldsObj);
    }

    QJsonDocument doc(finalCardsArray);
    return QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
}

QString CAnkiHelper::processCardTypeEnglishWord(const QString &rawText)
{
    // 1. Нормализуем переносы строк Windows
    QString cleanText = rawText;
    cleanText.replace("\r\n", "\n");

    // 2. Разрезаем текст карточки по разделителю внутренних блоков "-----"
    QStringList blocks = cleanText.split("-----");
    if (blocks.size() < 2)
    {
        emit errorMessage("Ошибка: Текст английского слова должен содержать внутренние блоки, разделенные '-----'");
        return QString("-");
    }

    QStringList formattedBlocks;

    // Первые два блока (заголовок и общий вектор) оставляем как есть, предварительно очистив края
    formattedBlocks.append(blocks.at(0).trimmed());
    formattedBlocks.append(blocks.at(1).trimmed());

    // 3. Обрабатываем циклические блоки со смыслами и примерами (начиная с индекса 2)
    for (int i = 2; i < blocks.size(); ++i)
    {
        QString blockText = blocks.at(i).trimmed();
        if (blockText.isEmpty())
        {
            continue;
        }

        QStringList lines = blockText.split('\n');

        // Ожидаем структуру из 5 строк внутри блока смысла
        if (lines.size() >= 5)
        {
            QString vRu  = lines.at(0).trimmed(); // Строка 1: {1, 0}
            QString wEn  = lines.at(1).trimmed(); // Строка 2: baby
            QString wRu  = lines.at(2).trimmed(); // Строка 3: малыш
            QString exEn = lines.at(3).trimmed(); // Строка 4: Английский пример
            QString exRu = lines.at(4).trimmed(); // Строка 5: Русский пример

            // АВТОМАТИЧЕСКАЯ ЗАМЕНА СЛОВА В АНГЛИЙСКОМ ПРИМЕРЕ НА КВАДРАТНЫЕ СКОБКИ
            if (!wEn.isEmpty() && !exEn.isEmpty())
            {
                // Экранируем спецсимволы в целевом слове
                QString escapedWord = QRegExp::escape(wEn);

                // Ищем слово целиком с границами слова (\b) без учета регистра
                QRegExp rx("\\b" + escapedWord + "\\b", Qt::CaseInsensitive);

                int pos = 0;
                while ((pos = rx.indexIn(exEn, pos)) != -1)
                {
                    // Капсулируем точный регистр совпавшего слова (например, "Baby" или "baby")
                    QString matchedWord = rx.cap(0);
                    QString replacement = "[" + matchedWord + "]";

                    exEn.replace(pos, matchedWord.length(), replacement);
                    pos += replacement.length(); // Сдвигаем позицию поиска с учетом скобок
                }
            }

            // Собираем измененные строки обратно в один блок
            QString formattedBlock = QString("%1\n%2\n%3\n%4\n%5")
                                     .arg(vRu, wEn, wRu, exEn, exRu);

            formattedBlocks.append(formattedBlock);
        }
        else
        {
            // Если строк меньше 5, сохраняем блок как есть, чтобы не потерять данные
            formattedBlocks.append(blockText);
        }
    }

    // 4. Объединяем блоки обратно через стандартный разделитель "-----"
    // Добавляем переносы строк вокруг разделителя для красивого и читаемого вида в teProcess
    return formattedBlocks.join("\n-----\n");
}

QString CAnkiHelper::processCardTypeFillBlank(const QString &rawText)
{
    emit errorMessage("TODO");

    return QString("-");
}

QString CAnkiHelper::processCardTypeSimple(const QString &rawText)
{
    QString result = rawText;
    result.replace("\r\n", "\n");

    return result;
}

QString CAnkiHelper::jsonFieldsEnglishWord(const QString &processedText)
{
    QStringList blocks = processedText.split("-----");

    if (blocks.size() < 2)
    {
        emit errorMessage("Ошибка: Текст английского слова должен содержать внутренние блоки, разделенные '-----'");
        return QString("-");
    }

    QJsonObject jsonFields;

    // 1. Извлекаем заголовок (всегда одна строка)
    QString header = blocks.at(0).trimmed();
    jsonFields["Hidden-main-word"] = header;

    // 2. Извлекаем общий вектор и ДОБАВЛЯЕМ <br> вместо переносов строк
    QString vectorRu = blocks.at(1).trimmed();
    vectorRu.replace("\n", "<br>"); // Заменит переносы между пунктами 1, 2 и т.д.
    jsonFields["Vector-ru"] = vectorRu;

    // 3. Базово инициализируем все 30 под-блоков полей
    for (int i = 1; i <= 30; ++i)
    {
        QString prefix = QString::number(i) + "-";
        jsonFields[prefix + "Vector-ru"]  = "";
        jsonFields[prefix + "Word-en"]    = "";
        jsonFields[prefix + "Word-ru"]    = "";
        jsonFields[prefix + "Example-en"] = "";
        jsonFields[prefix + "Example-ru"] = "";
    }

    // 4. Парсим циклические блоки данных
    int itemIndex = 1;
    for (int i = 2; i < blocks.size() && itemIndex <= 30; ++i)
    {
        QString blockText = blocks.at(i).trimmed();
        if (blockText.isEmpty())
        {
            continue;
        }

        QStringList lines = blockText.split('\n');
        if (lines.size() >= 5)
        {
            QString prefix = QString::number(itemIndex) + "-";

            QString vRu   = lines.at(0).trimmed();
            QString wEn   = lines.at(1).trimmed();
            QString wRu   = lines.at(2).trimmed();
            QString exEn  = lines.at(3).trimmed();
            QString exRu  = lines.at(4).trimmed();

            // ДОБАВИЛИ: Замена переносов на <br> для каждого текстового поля внутри блока
            vRu.replace("\n", "<br>");
            wEn.replace("\n", "<br>");
            wRu.replace("\n", "<br>");
            exEn.replace("\n", "<br>");
            exRu.replace("\n", "<br>");

            jsonFields[prefix + "Vector-ru"]  = vRu;
            jsonFields[prefix + "Word-en"]    = wEn;
            jsonFields[prefix + "Word-ru"]    = wRu;
            jsonFields[prefix + "Example-en"] = exEn;
            jsonFields[prefix + "Example-ru"] = exRu;

            itemIndex++;
        }
    }

    QJsonDocument doc(jsonFields);
    return QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
}

QString CAnkiHelper::jsonFieldsFillBlank(const QString &processedText)
{
    Q_UNUSED(processedText);
    emit errorMessage("TODO");
    return QString("-");
}

QString CAnkiHelper::jsonFieldsSimple(const QString &processedText)
{
    QString cleanCardText = processedText.trimmed();

    QString field1 = "";
    QString field2 = "";

    if (!cleanCardText.contains(getFieldSplitter()))
    {
        QStringList lines = cleanCardText.split('\n');

        QStringList validLines;

        for (const QString &line : std::as_const(lines))
        {
            if (!line.trimmed().isEmpty())
            {
                validLines.append(line.trimmed());
            }
        }

        if (validLines.size() == 2)
        {
            field1 = validLines.at(0);
            field2 = validLines.at(1);
        }
        else
        {
            emit errorMessage("No '---' separator");

            return QString("-");
        }
    }
    else
    {
        QStringList fields = cleanCardText.split(getFieldSplitter());
        field1 = (fields.size() > 0) ? fields.at(0).trimmed() : QString();
        field2 = (fields.size() > 1) ? fields.at(1).trimmed() : QString();
    }

    field1.replace("\n", "<br>");
    field2.replace("\n", "<br>");

    QJsonObject jsonFields;
    jsonFields["Front"] = field1;
    jsonFields["Back"] = field2;
    jsonFields["Todo"] = "";
    jsonFields["Answer-note"] = "";

    QJsonDocument doc(jsonFields);
    QString result = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));

    return result;
}
