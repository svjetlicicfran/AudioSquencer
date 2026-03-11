#include "logindialog.h"

#include "ui_logindialog.h"
#include <qurl.h>

LoginDialog::LoginDialog(QUrl _serverUrl, QWidget* parent) : QDialog(parent), ui(new Ui::LoginDialog), networkManager(new QNetworkAccessManager(this)), serverUrl(_serverUrl)
{
    ui->setupUi(this);
    this->setWindowTitle("Log in");
    
    serverUrl.setPath("/login");

    connect(ui->pushButtonLogin, &QPushButton::clicked, this, &LoginDialog::sendLogInRequest);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::sendLogInRequest()
{
    QString username = ui->lineEditUsername->text();
    QString password = ui->lineEditPassword->text();
    QUrl url(serverUrl);
    /*QUrlQuery query;
    query.addQueryItem("username", username);
    query.addQueryItem("password", password);
    url.setQuery(query);
  */
    QNetworkRequest logInRequest(url);
    logInRequest.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/json");
    QJsonObject loginJson{{"username", username}, {"password", password}};

    QNetworkReply* serverReply = networkManager->post(logInRequest, QJsonDocument(loginJson).toJson());

    connect(serverReply, &QNetworkReply::finished, this, [this, serverReply]() { this->handleReply(serverReply); });
}

void LoginDialog::handleReply(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        QJsonObject jsonReply = QJsonDocument::fromJson(reply->readAll()).object();
        QMessageBox* errorMsgBox =
            new QMessageBox(QMessageBox::Warning, "Error", reply->errorString() + "\n" + QString(jsonReply["error"].toString()));
        errorMsgBox->show();
        reply->deleteLater();
        return;
    }

    QJsonObject jsonReply = QJsonDocument::fromJson(reply->readAll()).object(); 

    if (jsonReply["auth_token"].isNull())
    {
        QMessageBox* errorMsgBox = new QMessageBox(QMessageBox::Information, "Error", QString(jsonReply["error"].toString()));
        errorMsgBox->show();
    }
    else
    {
        QMessageBox* succesMsgBox =
            new QMessageBox(QMessageBox::Information, "Success", QString("Successfuly logged in as ") + ui->lineEditUsername->text());
        succesMsgBox->show();

        emit loginSuccess(jsonReply["auth_token"].toString(), jsonReply["is_admin"].toInt(), ui->lineEditUsername->text());
    }

    reply->deleteLater();
}
