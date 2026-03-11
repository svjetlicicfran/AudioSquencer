#include "databaseeditor.h"

#include "ui_databaseeditor.h"

#include <qcontainerfwd.h>
#include <qlogging.h>
#include <qmath.h>
#include <qnamespace.h>
#include <qnetworkaccessmanager.h>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>
#include <qsqldatabase.h>

DatabaseEditor::DatabaseEditor(QUrl _serverUrl, QString _key, QWidget* parent)
    : QDialog(parent), ui(new Ui::DatabaseEditor), serverUrl(_serverUrl), key(_key), networkManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);

    serverUrl.setPath("/database");

    QNetworkRequest dataBaseRequest(serverUrl);
    dataBaseRequest.setRawHeader("Key", key.toUtf8());

    QNetworkReply* serverReplay = networkManager->get(dataBaseRequest);

    
    connect(serverReplay, &QNetworkReply::finished, this, [this, serverReplay]() { this->initDatabase(serverReplay); });

    // dataBase = QSqlDatabase::addDatabase("QMYSQL");
    // dataBase.setHostName("192.168.1.19");
    // dataBase.setPort(3306);
    // dataBase.setDatabaseName("sound_server");
    // dataBase.setUserName("remote2");
    // dataBase.setPassword("password");
    // if (!dataBase.open())
    //{
    //     QMessageBox(QMessageBox::Icon::Warning, QString("Db error"), QString("Error: ").append(dataBase.lastError().text())).exec();
    // }
    // model = new QSqlTableModel(this, dataBase);
    // model->setEditStrategy(QSqlTableModel::OnFieldChange);
    //
    // ui->tableView->setModel(model);
    // ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    // ui->tableView->setEditTriggers(QAbstractItemView::DoubleClicked);
    // ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    // ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    // ui->tableView->setSortingEnabled(true);
    //// ui->tableView->hideColumn(0);
    //
    // fillComboBox();
    //
    // connect(ui->tableComboBox, &QComboBox::currentTextChanged, this, &DatabaseEditor::selectTable);
    // connect(ui->addRowPushButton, &QPushButton::clicked, this, &DatabaseEditor::addRow);
    // connect(ui->removeSelectedPushButton, &QPushButton::clicked, this, &DatabaseEditor::removeRow);
}

DatabaseEditor::~DatabaseEditor()
{
    dataBase.close();
    delete ui;
}

void DatabaseEditor::addRow()
{
    int row = model->rowCount();
    model->insertRow(row);
    model->submitAll();
}

void DatabaseEditor::removeRow()
{
    QModelIndex index = ui->tableView->currentIndex();

    if (!index.isValid())
    {
        return;
    }

    model->removeRow(ui->tableView->currentIndex().row());
    model->submitAll();
}

void DatabaseEditor::selectTable(const QString& tableName)
{
    model->setTable(tableName);
    model->select();
}

void DatabaseEditor::fillComboBox()
{
    QSqlQuery q;
    q.exec("SHOW TABLES");

    ui->tableComboBox->clear();

    while (q.next())
    {
        QString tableName = q.value(0).toString();
        ui->tableComboBox->addItem(tableName);
    }
}

void DatabaseEditor::initDatabase(QNetworkReply* serverReply)
{
    if (serverReply->error() != QNetworkReply::NoError)
    {
        QJsonObject jsonReply = QJsonDocument::fromJson(serverReply->readAll()).object();
        QMessageBox* errorMsgBox = new QMessageBox(QMessageBox::Warning, "Error", serverReply->errorString() + '\n' + jsonReply["error"].toString());
        errorMsgBox->show();

        serverReply->deleteLater();
        return;
    }

    QJsonObject jsonReply = QJsonDocument::fromJson(serverReply->readAll()).object();

    QString databaseName = jsonReply["databaseName"].toString();
    QString databasePassword = jsonReply["password"].toString();
    QString databaseUser = jsonReply["userName"].toString();
    int databasePort = jsonReply["port"].toInt();

    dataBase = QSqlDatabase::addDatabase("QMYSQL");
    dataBase.setHostName("192.168.1.19");
    dataBase.setPort(databasePort);
    dataBase.setDatabaseName(databaseName);
    dataBase.setUserName(databaseUser);
    dataBase.setPassword(databasePassword);
    
    if (!dataBase.open())
    {
        QMessageBox* errorMsgBox = new QMessageBox(QMessageBox::Icon::Warning, QString("Db error"), QString("Error: ").append(dataBase.lastError().text()));

        errorMsgBox->show();
        serverReply->deleteLater();
        return;
    }

    model = new QSqlTableModel(this, dataBase);
    model->setEditStrategy(QSqlTableModel::OnFieldChange);
    
    ui->tableView->setModel(model);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setEditTriggers(QAbstractItemView::DoubleClicked);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableView->setSortingEnabled(true);
    
    fillComboBox();
    
    connect(ui->tableComboBox, &QComboBox::currentTextChanged, this, &DatabaseEditor::selectTable);
    connect(ui->addRowPushButton, &QPushButton::clicked, this, &DatabaseEditor::addRow);    
    connect(ui->removeSelectedPushButton, &QPushButton::clicked, this, &DatabaseEditor::removeRow);

}