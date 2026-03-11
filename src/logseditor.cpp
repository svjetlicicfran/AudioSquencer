#include "logseditor.h"

#include "ui_logseditor.h"

#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpushbutton.h>
#include <qtypes.h>
#include <qurl.h>

LogsEditor::LogsEditor(QString _token, QUrl _serverUrl, QWidget* parent)
    : QDialog(parent), ui(new Ui::LogsEditor), token(_token), networkAccessManager(new QNetworkAccessManager(this)),
      model(new QStandardItemModel(this)), serverUrl(_serverUrl)
{
    ui->setupUi(this);

    serverUrl.setPath("/logs");

    initLogs();

    model->setHorizontalHeaderLabels({"Timestamp", "Method", "URL", "Client", "Status", "Response Time (ms)"});

    ui->tableView->setModel(model);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);

    connect(ui->deletePushButton, &QPushButton::clicked, this, &LogsEditor::deleteLog);
    connect(ui->donePushButton, &QPushButton::clicked, this, &LogsEditor::doneEditing);
    connect(ui->savePushButton, &QPushButton::clicked, this, &LogsEditor::saveLogs);
    connect(ui->tableView, &QTableView::clicked, this, &LogsEditor::selectLog);

}

LogsEditor::~LogsEditor()
{
    delete ui;
}

void LogsEditor::selectLog(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return;
    }

    int row = index.row();

    QAbstractItemModel* m = ui->tableView->model();

    QString timestamp = m->index(row, 0).data().toString();
    QString method = m->index(row, 1).data().toString();
    QString url = m->index(row, 2).data().toString();
    QString client = m->index(row, 3).data().toString();
    QString status = m->index(row, 4).data().toString();
    QString responseTimeMs = m->index(row, 5).data().toString();

    ui->timeLineEdit->setText(timestamp);
    ui->methodLineEdit->setText(method);
    ui->urlLineEdit->setText(url);
    ui->ipLineEdit->setText(client);
    ui->statusLineEdit->setText(status);
    ui->responseLineEdit->setText(responseTimeMs);
}

void LogsEditor::initLogs()
{
    QNetworkRequest logsRequest(serverUrl);
    logsRequest.setRawHeader("Key", token.toUtf8());

    QNetworkReply* serverReply = networkAccessManager->get(logsRequest);
    connect(serverReply, &QNetworkReply::finished, this, [this, serverReply]() { handleInitResponse(serverReply); });
}

void LogsEditor::handleInitResponse(QNetworkReply* reply)
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

    QJsonArray jsonLogs = QJsonDocument::fromJson(reply->readAll()).array();

    for (const QJsonValue& value : jsonLogs)
    {
        QJsonObject obj = value.toObject();
        QList<QStandardItem*> row;

        row.append(new QStandardItem(obj["timestamp"].toString()));
        row.append(new QStandardItem(obj["method"].toString()));
        row.append(new QStandardItem(obj["url"].toString()));
        row.append(new QStandardItem(obj["clientAddress"].toString()));
        row.append(new QStandardItem(QString::number(obj["status"].toInt())));
        row.append(new QStandardItem(QString::number(obj["responseTimeMs"].toInt())));

        model->appendRow(row);
    }

    reply->deleteLater();
}

void LogsEditor::saveLogs()
{
    QString filePath = createLogsSaveFile();

    if (filePath == "")
    {
        return;
    }

    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qInfo() << "Failed to open a file";
        return;
    }

    QJsonDocument jsonDoc(tableToJson(model));
    file.write(jsonDoc.toJson(QJsonDocument::Indented));
    file.close();

}

QJsonArray LogsEditor::tableToJson(QAbstractItemModel* model)
{

    QJsonArray array;

    int rows = model->rowCount();
    int cols = model->columnCount();

    QStringList headers;
    for (int c = 0; c < cols; ++c)
    {
        headers.append(model->headerData(c, Qt::Horizontal).toString());
    }

    for (int r = 0; r < rows; ++r)
    {
        QJsonObject obj;

        for (int c = 0; c < cols; ++c)
        {
            QString key = headers[c];
            QVariant value = model->data(model->index(r, c));

            obj.insert(key, QJsonValue::fromVariant(value));
        }


        array.append(obj);
    }

    return array;
}

void LogsEditor::handleSaveLogsResponse(QNetworkReply* reply)
{
}

void LogsEditor::doneEditing()
{
    if (!ui->tableView->currentIndex().isValid())
    {
        return;
    }

    QString time = ui->timeLineEdit->text();
    QString method = ui->methodLineEdit->text();
    QString url = ui->urlLineEdit->text();
    QString address = ui->ipLineEdit->text();
    QString status = ui->statusLineEdit->text();
    QString responseTime = ui->responseLineEdit->text();

    int updateRow = ui->tableView->currentIndex().row();

    model->setData(model->index(updateRow, 0), time);
    model->setData(model->index(updateRow, 1), method);
    model->setData(model->index(updateRow, 2), url);
    model->setData(model->index(updateRow, 3), address);
    model->setData(model->index(updateRow, 4), status);
    model->setData(model->index(updateRow, 5), responseTime);
}

void LogsEditor::deleteLog()
{
    if (!ui->tableView->currentIndex().isValid())
    {
        return;
    }

    int deleteRow = ui->tableView->currentIndex().row();

    model->removeRow(deleteRow);
}