#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QPainter>

#define LEN 3*1024*1024
#define P_LEN 1380

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnect_slot()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(read_data()));
    data = new char[LEN];
}

MainWindow::~MainWindow()
{
    delete ui;
    delete [] data;
    delete socket;
}

void MainWindow::paintEvent(QPaintEvent *e)
{
    QPixmap map;

    if ((len_rcv >= len) && (len > 0))
    {
        map.loadFromData((uchar *)data, len);

        QPainter p(this);
        p.drawPixmap(10, 80, 640, 480, map);
        socket->write("new_request");
        len_rcv = 0;
    }

}

void MainWindow::read_data()
{
    int ret;

    ret = socket->read(data+len_rcv, P_LEN);
    if (0 == strncmp("newImage", data+len_rcv, 8))
    {
        len = atoi(data+len_rcv+9);
    }
    else
    {
        len_rcv += ret;
        if (len_rcv >= len)
        {
            update();
            return;
        }
    }
    socket->write("ack");
}

void MainWindow::disconnect_slot()
{
    ui->bnt_connect->setEnabled(true);
}

void MainWindow::on_bnt_connect_clicked()
{
    QString ip = ui->lnd_ip->text();

    socket->connectToHost(ip, 12345);
    if (!socket->waitForConnected(1000))
    {
        QMessageBox::critical(this, "error", "server connection failed");
        return;
    }

    len_rcv = 0;
    socket->write("new_request");
    ui->bnt_connect->setEnabled(false);
}

void MainWindow::on_pushButton_clicked()
{

}
