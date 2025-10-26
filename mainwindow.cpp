#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->message->setEnabled(0);
    ui->send_message->setEnabled(0);
    ui->receiver_ip->setEnabled(0);
    ui->receiver_port->setEnabled(0);
}

MainWindow::~MainWindow()
{
    if (info != nullptr)
        info->deleteLater();

    if (socket != nullptr)
        socket->deleteLater();

    for (auto& process: processes)
        process->deleteLater();

    delete ui;
}


void MainWindow::on_receiver_ip_textEdited(const QString &arg1)
{
    info->edited_receiverIp = true;
}


void MainWindow::on_receiver_port_textChanged(const QString &arg1)
{
    info->edited_receiverPort = true;
}


void MainWindow::on_confirm_port_clicked()
{
    info = new Info(ui->my_port->value(), ui->receiver_ip->text(), ui->receiver_port->value());

    socket = new QUdpSocket(this);
    if (!socket->bind(QHostAddress::Any, info->Get_myPort())) {
        qCritical() << QString("Error: bind failed on myPort: %1").arg(info->Get_myPort());
        return;
    }
    connect(socket, &QUdpSocket::readyRead, this, &MainWindow::ReadDatagrams);

    datagram = new Datagram(ui->my_name->text(), ColorDialog());

    ui->my_port->setEnabled(0);
    ui->confirm_port->setEnabled(0);
    ui->message->setEnabled(1);
    ui->send_message->setEnabled(1);
    ui->receiver_ip->setEnabled(1);
    ui->receiver_port->setEnabled(1);
}


void MainWindow::on_send_message_clicked()
{
    if (info->edited_receiverIp)
        info->Set_receiverIp(ui->receiver_ip->text());
    if (info->edited_receiverPort)
        info->Set_receiverPort(ui->receiver_port->value());
    if (!info->isValid()) {
        qWarning() << "Change myPort/receiverIp/receiverPort to correctly";
        return;
    }

    QByteArray message;
    message.append(ui->message->text().toUtf8());
    socket->writeDatagram(message, QHostAddress(info->Get_receiverIp()), info->Get_receiverPort());

    qInfo() << QString("Message <%1> is sended. // myPort: %2, receiverIp: %3, receiverPort: %4").arg(message).arg(info->Get_myPort()).arg(info->Get_receiverIp()).arg(info->Get_receiverPort());
    ui->chat->insertHtml("Отправлено " + info->Get_receiverIp() + ":" + QString::number(info->Get_receiverPort()) + "<br>" + message + "<br><br>");
}


void MainWindow::ReadDatagrams()
{
    QByteArray buffer;
    buffer.resize(socket->pendingDatagramSize());

    QHostAddress host_address;
    quint16 host_port;
    socket->readDatagram(buffer.data(), buffer.size(), &host_address, &host_port);
    qInfo() << QString("Сообщение <%1> от %2:%3 получено").arg(buffer.data()).arg(host_address.toString()).arg(host_port);

    ui->chat->insertHtml("Получено от " + host_address.toString() + ":" + QString::number(host_port) + "<br>" + buffer.data() + "<br><br>");
}


void MainWindow::on_open_new_window_clicked()
{
    QProcess *process = new QProcess(this);
    processes.append(process);
    process->start(qApp->applicationFilePath());
}


QColor MainWindow::ColorDialog() {
    QColorDialog* ColorDialog;
    ColorDialog = new QColorDialog(this);
    ColorDialog->exec();

    return ColorDialog->selectedColor();
}



