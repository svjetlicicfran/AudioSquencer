#ifndef DATABASEEDITOR_H
#define DATABASEEDITOR_H

#include <QDebug>
#include <QMessageBox>
#include <QSortFilterProxyModel>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <qcontainerfwd.h>
#include <qnetworkaccessmanager.h>
#include <qurl.h>
#include <QJsonDocument>
#include <QJsonObject>

namespace Ui
{
class DatabaseEditor;
}

class DatabaseEditor : public QDialog
{
    Q_OBJECT

  public:
    explicit DatabaseEditor(QUrl _serverUrl, QString _key, QWidget* parent = nullptr);
    ~DatabaseEditor();

  private slots:
    void addRow();
    void removeRow();
    void selectTable(const QString& tableName);

  private:
    Ui::DatabaseEditor* ui;
    QSqlDatabase dataBase;
    QSqlTableModel* model;

    QString key;

    QUrl serverUrl;

    QNetworkAccessManager* networkManager;

    void initDatabase(QNetworkReply* serverReply);
    void fillComboBox();
};

#endif // DATABASEEDITOR_H
