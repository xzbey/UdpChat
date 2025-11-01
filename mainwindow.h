#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QProcess>
#include <QScrollBar>
#include <QTime>
#include "Info.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent*);

private slots:
    void on_confirm_port_clicked();

    void on_send_message_clicked();

    void on_receiver_ip_textEdited(const QString &arg1);

    void on_receiver_port_textChanged(const QString &arg1);

    void on_open_new_window_clicked();

    void on_message_textChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;

    Info* info = 0;
    QUdpSocket* socket = 0;
    QUdpSocket* broadcast_socket = 0;
    Datagram* datagram = 0;

    QList<QProcess*> processes;

    void ReadDatagrams(QUdpSocket*fsocket);

    QColor ColorDialog();

};
#endif // MAINWINDOW_H
