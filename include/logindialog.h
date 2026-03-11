#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QAbstractSocket>
#include <QDebug>
#include <QDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTcpSocket>
#include <QUrl>
#include <QUrlQuery>
#include <qurl.h>

QT_BEGIN_NAMESPACE
namespace Ui
{
class LoginDialog;
}
QT_END_NAMESPACE

class LoginDialog : public QDialog
{
    Q_OBJECT
  public:
    LoginDialog(QUrl _serverUrl, QWidget* parent = nullptr);
    ~LoginDialog();

  private slots:
    void sendLogInRequest();
    void handleReply(QNetworkReply* reply);

  signals:
    void loginSuccess(const QString authToken, bool isAdmin, const QString username);

  private:
    Ui::LoginDialog* ui;
    QUrl serverUrl;
    QNetworkAccessManager* networkManager;
};

#endif // LOGINDIALOG_H
