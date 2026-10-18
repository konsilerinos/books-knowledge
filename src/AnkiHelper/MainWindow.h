#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "AnkiConnector.h"
#include "AnkiHelper.h"
#include "Examples.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class CMainWindow;
}
QT_END_NAMESPACE

class CMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    CMainWindow(QWidget *parent = nullptr);
    ~CMainWindow();

private slots:
    void on_actionShowExamples_triggered(bool checked);

    void on_btnServerConnect_clicked();
    void on_btnServerDisconnect_clicked();

    void on_btnClear_clicked();
    void on_btnSend_clicked();
    void on_btnProcess_clicked();

    void onConnectorConnected();
    void onConnectorDisconnected();
    void onConnectorReadyToWork();
    void onConnectorError(const QString &errorMsg);

    void onNotesAddedSuccess();
    void onNotesAddedWithErrors(const QStringList &errorsList);

    void onHelperError(const QString &message);

private:
    Ui::CMainWindow *ui;

    CExamples *m_formExamples = nullptr;

    CAnkiConnector *m_ankiConnector = nullptr;
    CAnkiHelper *m_ankiHelper = nullptr;
};

#endif // MAINWINDOW_H
