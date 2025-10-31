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
    ui->my_name->setEnabled(0);
    ui->message->setEnabled(1);
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

    datagram->Set_message(ui->message->text());
    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly);
    stream << *datagram;

    socket->writeDatagram(buffer, QHostAddress(info->Get_receiverIp()), info->Get_receiverPort());
    qInfo() << QString(datagram->Get_name() + datagram->Get_color().name() + " : <" + datagram->Get_message() + "> to " + info->Get_receiverIp() + ":" + QString::number(info->Get_receiverPort()));

    //в начале <div style=\"text-align: right;\"> + в конце закрытие </div> : не работает, прижимает раз и навсегда
    ui->chat->insertHtml(QString("<span style=\"color: %1;\"><b>Отправлено (%2)</b></span><br>"
                                 "<span style=\"color: %1;\">%3</span><br><br>").arg(datagram->Get_color().name(), datagram->Get_name(), datagram->Get_message()));
    ui->chat->verticalScrollBar()->setValue(ui->chat->verticalScrollBar()->maximum());
    ui->message->clear();
}


void MainWindow::ReadDatagrams()
{
    QByteArray buffer;
    buffer.resize(socket->pendingDatagramSize());

    QHostAddress host_address;
    quint16 host_port;
    socket->readDatagram(buffer.data(), buffer.size(), &host_address, &host_port);

    if (buffer.isEmpty() or buffer.size() < 10) {
        qInfo() << QString("Ignored empty buffer from %1:%2").arg(host_address.toString()).arg(QString::number(host_port));
        return; }

    QDataStream stream(&buffer, QIODevice::ReadOnly);
    Datagram data;
    stream >> data;

    qInfo() << QString("Received = " + data.Get_name() + data.Get_color().name() + " : <" + data.Get_message() + ">");

    //в начале <div style=\"text-align: left;\"> + в конце закрытие </div> : не работает, прижимает раз и навсегда
    ui->chat->insertHtml(QString("<span style=\"color: %1;\"><b>Получено от %2</b></span><br>"
                                 "<span style=\"color: %1;\">%3</span><br><br>").arg(data.Get_color().name(), data.Get_name(), data.Get_message()));

    ui->chat->verticalScrollBar()->setValue(ui->chat->verticalScrollBar()->maximum());
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


void MainWindow::closeEvent(QCloseEvent*e)
{
    if (info != nullptr)
        info->deleteLater();
    qDebug() << "info deleted";

    if (socket != nullptr)
        socket->deleteLater();
    qDebug() << "socket deleted";

    if (datagram != nullptr)
        delete datagram;
    qDebug() << "datagram deleted";

    for(auto &process : processes)
        process->terminate();
    qDebug() << "processes clear";

    QWidget::closeEvent(e);
}

void MainWindow::on_message_textChanged(const QString &arg1)
{
    ui->send_message->setEnabled(!arg1.isEmpty());
}

