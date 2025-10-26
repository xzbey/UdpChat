#ifndef INFO_H
#define INFO_H

#include <QObject>
#include <QDebug>
#include <QHostAddress>
#include <QAbstractSocket>
#include <QColorDialog>

class Info: public QObject {
    Q_OBJECT

private:
    quint16 myPort;

    QString receiverIp;
    quint16 receiverPort;

public:
    Info(quint16 myPort, QString receiverIp, quint16 receiverPort):
        myPort(myPort), receiverIp(receiverIp), receiverPort(receiverPort), edited_receiverPort(false), edited_receiverIp(false) {}

    bool isValid() const {
        if (myPort == receiverPort) {
            qCritical() << QString("Error: myPort and receiverPort should be different (%1, %2)").arg(myPort).arg(receiverPort);
            return false;
        }

        QHostAddress address(receiverIp);
        if (address.protocol() == QAbstractSocket::UnknownNetworkLayerProtocol) {
            qCritical() << QString("Error: Invalid IP address - %1").arg(receiverIp);
            return false;
        }

        return true;
    }

    quint16 Get_myPort() const { return myPort; }
    quint16 Get_receiverPort() const { return receiverPort; }
    QString Get_receiverIp() const { return receiverIp; }

    void Set_receiverPort(quint16 receiverPort) {
        this->receiverPort = receiverPort;
        edited_receiverPort = false;
    }

    void Set_receiverIp(QString receiverIp) {
        this->receiverIp = receiverIp;
        edited_receiverIp = false;
    }

    bool edited_receiverPort,
        edited_receiverIp;
};


class Datagram {
public:
    Datagram(QString name, QColor color, QString message = "None"):
        name(name), color(color), message(message) {}

    void Set_message(QString message) { this->message = message; }
private:
    QString name;
    QColor color;
    QString message;
};

#endif // INFO_H
