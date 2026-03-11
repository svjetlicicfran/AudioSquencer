#include "registerdialog.h"

#include "ui_registerdialog.h"

#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qlogging.h>
#include <qnumeric.h>
#include <qurl.h>

RegisterDialog::RegisterDialog(QUrl _serverUrl, QWidget* parent) : QDialog(parent), ui(new Ui::registerdialog), networkManager(new QNetworkAccessManager(this)), serverUrl(_serverUrl)
{
    ui->setupUi(this);
    this->setWindowTitle("Register");

    serverUrl.setPath("/register");

    connect(this->ui->sendRequestButton, &QPushButton::clicked, this, &RegisterDialog::sendRegisterRequest);
    // connect(this->networkManager, &QNetworkAccessManager::finished, this, &RegisterDialog::handleRepy);
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::sendRegisterRequest()
{

    QString username = ui->lineEditUsername->text();
    QString password = ui->lineEditPassword->text();

    QJsonObject postJson{{"username", username}, {"password", password}};

    QNetworkRequest registerRequest(serverUrl);
    registerRequest.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/json");

    QNetworkReply* serverReply = networkManager->post(registerRequest, QJsonDocument(postJson).toJson());
    connect(serverReply, &QNetworkReply::finished, this, [this, serverReply]() { this->handleRepy(serverReply); });
}

void RegisterDialog::handleRepy(QNetworkReply* reply)
{

    if (reply->error() != QNetworkReply::NoError)
    {
        QByteArray response = reply->readAll();
        QMessageBox* errorMsgBox = new QMessageBox(QMessageBox::Warning, "Error", reply->errorString() + "\n" + QString(response));
        errorMsgBox->show();
        reply->deleteLater();
        return;
    }

    QJsonObject jsonReply = QJsonDocument::fromJson(reply->readAll()).object(); 

    QMessageBox* succesMsgBox = new QMessageBox(QMessageBox::Information, "Info", jsonReply["message"].toString());
    succesMsgBox->show();
    reply->deleteLater();
}
