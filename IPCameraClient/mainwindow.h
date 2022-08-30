#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void paintEvent(QPaintEvent *e);

private slots:
    void on_bnt_connect_clicked();

    void disconnect_slot();

    void read_data();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;

    QTcpSocket *socket;

    char *data;
    int  len;     //图像大小
    int  len_rcv; //接收大小
};
#endif // MAINWINDOW_H
