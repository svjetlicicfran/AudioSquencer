#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDebug>
#include <QDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <qurl.h>

namespace Ui {
class registerdialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    RegisterDialog(QUrl _serverUrl, QWidget *parent = nullptr);
    ~RegisterDialog();

private slots:
    void sendRegisterRequest();
    void handleRepy(QNetworkReply *reply);

private:
    Ui::registerdialog *ui;
    QNetworkAccessManager *networkManager;
    QUrl serverUrl;
};

#endif // REGISTERDIALOG_H
