#ifndef LOGSEDITOR_H
#define LOGSEDITOR_H

#include <QDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStandardItemModel>
#include <QTableView>
#include <QUrl>
#include <qstandarditemmodel.h>
#include <qtmetamacros.h>
#include <qurl.h>

#include "xmlwriter.h"

namespace Ui
{
class LogsEditor;
}

class LogsEditor : public QDialog
{
    Q_OBJECT

  public:
    explicit LogsEditor(QString _token, QUrl _serverUrl , QWidget* parent = nullptr);
    ~LogsEditor();

  private slots:
    void selectLog(const QModelIndex& index);
    void deleteLog();
    void doneEditing();
    void saveLogs();

  private:
    void initLogs();
    void handleInitResponse(QNetworkReply* reply);

    
    void handleSaveLogsResponse(QNetworkReply* reply);

    QJsonArray tableToJson(QAbstractItemModel* model);

    Ui::LogsEditor* ui;
    QString token;

    QUrl serverUrl;
    QNetworkAccessManager* networkAccessManager;

    QStandardItemModel* model;
};

#endif // LOGSEDITOR_H
