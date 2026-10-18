#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QShortcut>

CMainWindow::CMainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::CMainWindow)
{
    ui->setupUi(this);

    m_ankiConnector = new CAnkiConnector(this);
    m_ankiHelper = new CAnkiHelper(this);

    connect(m_ankiConnector, &CAnkiConnector::connected, this, &CMainWindow::onConnectorConnected);
    connect(m_ankiConnector, &CAnkiConnector::disconnected, this, &CMainWindow::onConnectorDisconnected);
    connect(m_ankiConnector, &CAnkiConnector::readyToWork, this, &CMainWindow::onConnectorReadyToWork);
    connect(m_ankiConnector, &CAnkiConnector::errorMessage, this, &CMainWindow::onConnectorError);

    connect(m_ankiConnector, &CAnkiConnector::notesAddedSuccess, this, &CMainWindow::onNotesAddedSuccess);
    connect(m_ankiConnector, &CAnkiConnector::notesAddedWithErrors, this, &CMainWindow::onNotesAddedWithErrors);

    connect(m_ankiHelper, &CAnkiHelper::errorMessage, this, &CMainWindow::onHelperError);

    connect(ui->actionShowExamples, &QAction::triggered, this, &CMainWindow::on_actionShowExamples_triggered);

    ui->lblServerState->setText("disconnected");
    ui->btnServerDisconnect->setEnabled(false);
    ui->btnSend->setEnabled(false);
    ui->btnProcess->setEnabled(true);

    // todo: show with debug mode
    ui->tbJson->setVisible(false);

    m_formExamples = new CExamples();
    m_formExamples->hide();

    QShortcut *shortcutField = new QShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Minus), ui->teInput);
    shortcutField->setContext(Qt::WidgetShortcut);
    connect(shortcutField, &QShortcut::activated, this, [this]() {
        ui->teInput->insertPlainText("---");
    });

    QShortcut *shortcutCard = new QShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Equal), ui->teInput);
    shortcutCard->setContext(Qt::WidgetShortcut);
    connect(shortcutCard, &QShortcut::activated, this, [this]() {
        ui->teInput->insertPlainText("===");
    });
}

CMainWindow::~CMainWindow()
{
    delete ui;

    delete m_formExamples;
}

void CMainWindow::on_actionShowExamples_triggered(bool checked)
{
    m_formExamples->show();
}

void CMainWindow::on_btnServerConnect_clicked()
{
    ui->lblServerState->setText("connecting...");
    m_ankiConnector->ankiClientConnect();
}

void CMainWindow::on_btnServerDisconnect_clicked()
{
    m_ankiConnector->ankiClientDisconnect();
}

void CMainWindow::on_btnClear_clicked()
{
    ui->teInput->clear();
    ui->teProcess->clear();
    ui->tbJson->clear();
}

// todo: bad function, it's need to refactor
void CMainWindow::on_btnSend_clicked()
{
    QString processedText = ui->teProcess->toPlainText();
    QString selectedTypeStr = ui->cbTypes->currentText();
    QString selectedDeck = ui->cbDecks->currentText();

    // 1. Проверяем заполнение полей
    if (processedText.isEmpty() || processedText == "-")
    {
        QMessageBox::critical(this, "Ошибка", "Поле обработанного текста пусто. Нажмите Process.");
        return;
    }

    if (selectedDeck.isEmpty())
    {
        QMessageBox::critical(this, "Ошибка", "Выберите целевую колоду.");
        return;
    }

    // 2. Генерируем JSON-массив карточек из teProcess
    QString jsonResult = m_ankiHelper->parseToJson(processedText, selectedTypeStr);
    if (jsonResult.isEmpty() || jsonResult == "-") return;

    QJsonDocument doc = QJsonDocument::fromJson(jsonResult.toUtf8());
    QJsonArray cardsArray = doc.array();
    if (cardsArray.isEmpty()) return;

    // 3. Ищем реальное системное имя модели карточки в Anki
    CAnkiHelper::CardType currentType = CAnkiHelper::cardTypeFromString(selectedTypeStr);
    QStringList systemModelNames = m_ankiConnector->getModelsAndFields().keys();
    QString finalAnkiModelName = "";

    for (const QString &systemName : std::as_const(systemModelNames))
    {
        if (currentType == CAnkiHelper::Simple && systemName.contains("простая", Qt::CaseInsensitive))
        {
            finalAnkiModelName = systemName;
            break;
        }
        else if (currentType == CAnkiHelper::EnglishWord && systemName.contains("английское слово", Qt::CaseInsensitive))
        {
            finalAnkiModelName = systemName;
            break;
        }
        else if (currentType == CAnkiHelper::FillBlank && systemName.contains("задание с пропуском", Qt::CaseInsensitive))
        {
            finalAnkiModelName = systemName;
            break;
        }
    }

    if (finalAnkiModelName.isEmpty())
    {
        finalAnkiModelName = selectedTypeStr;
    }

    // --- ДОРАБОТКА: Проверяем видимость виджета tbJson ---
    if (ui->tbJson->isVisible())
    {
        // Если лог JSON открыт, выводим красивое дерево для отладки
        ui->tbJson->setText(QString::fromUtf8(doc.toJson(QJsonDocument::Indented)));
    }

    // В любом случае (и при скрытом, и при видимом tbJson) отправляем пачку в Anki
    m_ankiConnector->addNotes(selectedDeck, finalAnkiModelName, cardsArray, ui->leTag->text());
}

void CMainWindow::on_btnProcess_clicked()
{
    QString selectedType = ui->cbTypes->currentText();

    if (selectedType.isEmpty())
    {
        QMessageBox::critical(this, "Error", "Select card type");

        return;
    }

    if (ui->teProcess->toPlainText().isEmpty())
    {
        QString rawText = ui->teInput->toPlainText();

        if (rawText.isEmpty())
        {
            QMessageBox::critical(this, "Error", "No input text");

            return;
        }

        QString processedResult = m_ankiHelper->processCardText(rawText, selectedType);

        if (!processedResult.isEmpty() && processedResult != "-")
        {
            ui->teProcess->setPlainText(processedResult);
        }
    }
    else
    {
        QString processedText = ui->teProcess->toPlainText();

        QString jsonResult = m_ankiHelper->parseToJson(processedText, selectedType);

        if (!jsonResult.isEmpty() && jsonResult != "-")
        {
            QJsonDocument doc = QJsonDocument::fromJson(jsonResult.toUtf8());
            ui->tbJson->setText(QString::fromUtf8(doc.toJson(QJsonDocument::Indented)));
        }
    }
}

void CMainWindow::onConnectorConnected()
{
    ui->lblServerState->setText("connected");
    ui->btnServerConnect->setEnabled(false);
    ui->btnServerDisconnect->setEnabled(true);
}

void CMainWindow::onConnectorDisconnected()
{
    ui->lblServerState->setText("disconnected");
    ui->btnServerConnect->setEnabled(true);
    ui->btnServerDisconnect->setEnabled(false);
    ui->btnSend->setEnabled(false);

    ui->cbTypes->clear();
    ui->cbDecks->clear();
}

void CMainWindow::onConnectorReadyToWork()
{
    ui->lblServerState->setText("ready to work");
    ui->btnSend->setEnabled(true);

    ui->cbTypes->blockSignals(true);
    ui->cbDecks->blockSignals(true);

    ui->cbTypes->clear();
    ui->cbDecks->clear();

    const QStringList allowedTypes = CAnkiHelper::getAllowedTypeStrings();
    ui->cbTypes->addItems(allowedTypes);

    ui->cbDecks->addItems(m_ankiConnector->getDeckNames());

    ui->cbTypes->blockSignals(false);
    ui->cbDecks->blockSignals(false);
}

void CMainWindow::onConnectorError(const QString &errorMsg)
{
    QMessageBox::critical(this, "Anki connector error", errorMsg);
}

void CMainWindow::onNotesAddedSuccess()
{
    on_btnClear_clicked();

    QMessageBox::information(this, "Success", "Cards added");
}

void CMainWindow::onNotesAddedWithErrors(const QStringList &errorsList)
{
    QString detailedErrors = errorsList.join("\n");

    QMessageBox::critical(this, "Error", "Cards added with errors:\n\n" + detailedErrors);
}

void CMainWindow::onHelperError(const QString &message)
{
    QMessageBox::critical(this, "Anki helper error", message);
}

