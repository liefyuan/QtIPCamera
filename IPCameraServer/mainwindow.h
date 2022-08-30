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
    TS_IDLE,       //����(ͼ�����ݿ��Ը���)
    TS_RUNNING,    //ͼ�����ݴ�����(�������Ը���ͼ������)
    TS_FIRST_DATA, //��Ҫ����ͼ�����ݵĵ�һ����
};

class ImgData {
public:
    char data[LEN] = {0}; //ͼ������
    int  totalLen = 0; //ͼ���С
    int  hasSentLen = 0; //�ѷ��������ݳ���
    TransStatus  stats = TS_IDLE; //����״̬
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

    QCamera *m_pCam; //����ͷ����ָ��
    QCameraViewfinder *m_pCamViewFind; //����ͷ��ȡ����
    QCameraImageCapture *m_pImageCapture; //��ȡ����ͷ��ǰ֡
    QList<QCameraInfo> cameraList; //�洢������豸��Ϣ

    QList<QTcpSocket *> clients;

    QImage m_Image;

    QTimer *m_pTimer;

    QComboBox *m_pComboBox;

    QTcpServer *m_pServer;
};
#endif // MAINWINDOW_H
