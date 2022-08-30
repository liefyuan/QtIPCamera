#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCamera>
#include <QCameraInfo>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QMessageBox>
#include <QComboBox>
#include <QTimer>
#include <QScreen>
#include <QBuffer>

#define W  640
#define H  480

#define LEN  (W*H*2)

#define P_LEN  1380

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum TransStatus{
    TS_IDLE,       //空闲(图像数据可以更新)
    TS_RUNNING,    //图像数据传输中(还不可以更新图像数据)
    TS_FIRST_DATA, //需要发出图像数据的第一部分
};

class ImgData {
public:
    char data[LEN] = {0}; //图像数据
    int  totalLen = 0; //图像大小
    int  hasSentLen = 0; //已发出的数据长度
    TransStatus  stats = TS_IDLE; //工作状态
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_btn_IPServer_toggled(bool checked);

    void read_data();

    void new_client();

    void timer_slot();

    void distconnect_client();

    void onImageCaptured(int id, const QImage &preview);

private:
    Ui::MainWindow *ui;

    QCamera *m_pCam; //摄像头对象指针
    QCameraViewfinder *m_pCamViewFind; //摄像头的取景器
    QCameraImageCapture *m_pImageCapture; //获取摄像头当前帧
    QList<QCameraInfo> cameraList; //存储摄像机设备信息

    QList<QTcpSocket *> clients;

    QImage m_Image;

    QTimer *m_pTimer;

    QComboBox *m_pComboBox;

    QTcpServer *m_pServer;
};
#endif // MAINWINDOW_H
