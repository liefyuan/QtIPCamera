#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_pComboBox = ui->cbBox_resolution;

    cameraList = QCameraInfo::availableCameras();
    foreach(const QCameraInfo &cameraInfo, cameraList)
    {
        qDebug() << "CameraInfo:" << cameraInfo;
        ui->cbBox_cameras->addItem(cameraInfo.description());
    }


    QCameraInfo CameraInfo = cameraList.at(ui->cbBox_cameras->currentIndex());

    m_pCam = new QCamera(CameraInfo);
    m_pCamViewFind = new QCameraViewfinder(ui->showImageLabel);
    m_pCam->setViewfinder(m_pCamViewFind);

    m_pImageCapture = new QCameraImageCapture(m_pCam);

    connect(m_pImageCapture,&QCameraImageCapture::imageCaptured, \
                        this,&MainWindow::onImageCaptured);

    m_pImageCapture->setCaptureDestination(QCameraImageCapture::CaptureToFile);
    m_pCam->setCaptureMode(QCamera::CaptureVideo);
    m_pCam->start();

    ui->btn_IPServer->setCheckable(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_btn_IPServer_toggled(bool checked)
{
    qDebug() << "Toggle";
    if (checked)
    {
        m_pServer = new QTcpServer(this);
        if (!m_pServer->listen(QHostAddress::Any, 12345))
        {
            QMessageBox::critical(this, "error", "listen port failed");
            exit(0);
        }
        qDebug() << "start IP server";

        m_pTimer = new QTimer(this);
        connect(m_pServer, SIGNAL(newConnection()), this, SLOT(new_client()));
        connect(m_pTimer, SIGNAL(timeout()), this, SLOT(timer_slot()));
        m_pTimer->start(100);

        ui->btn_IPServer->setText("close IP service");
    }
    else
    {
        qDebug() << "stop IP server";
        m_pServer->close();
        m_pTimer->stop();
        delete m_pServer;

        ui->btn_IPServer->setText("start IP service");
    }
}

void MainWindow::read_data()
{
    qDebug() << "read_data()";
    QTcpSocket *client = qobject_cast<QTcpSocket *>(sender());
    QString str = client->readAll();
    ImgData *d = (ImgData*)client->userData(0);
    QString s("newImage:%1");

    if (str == "new_request")
    {
        if ((d->totalLen) && (d->stats==0)) //ͼ���С��Ϊ0����ʾ�Ѹ���ͼ��������
        {
            d->stats = TS_RUNNING;
            client->write(s.arg(d->totalLen).toUtf8());
            d->hasSentLen = 0;
        }
        else //ͼ�����ݻ�û�и���
            d->stats = TS_FIRST_DATA; //�ڶ�ʱ���Ĳۺ����﷢��"newImage..."
    }
    if (str == "ack")
    {
        int len_send = P_LEN;
        if (d->hasSentLen >= d->totalLen) //���ͼ���Ѵ������
            return;

        if ((d->hasSentLen + P_LEN) > d->totalLen)
            len_send = d->totalLen - d->hasSentLen;

        d->hasSentLen += client->write(d->data+d->hasSentLen, len_send);
        if (d->hasSentLen >= d->totalLen)
        {
            d->stats = TS_IDLE; //������Ϻ󣬰�״̬��Ϊ�ɸ���
            d->totalLen = 0;
        }
    }
}

void MainWindow::distconnect_client()
{
    QTcpSocket *client = qobject_cast<QTcpSocket *>(sender());
    ImgData *d = (ImgData*)client->userData(0);

    clients.removeOne(client);
}

void MainWindow::new_client()
{
    qDebug() << "new client";
    QTcpSocket *client = m_pServer->nextPendingConnection();

    connect(client, SIGNAL(disconnected()), this, SLOT(distconnect_client()));
    connect(client, SIGNAL(readyRead()), this, SLOT(read_data()));
    client->setUserData(0, (QObjectUserData *)new ImgData);
    clients.append(client);
}

void MainWindow::timer_slot()
{
//    qDebug() << "timeout";
//    m_pImageCapture->capture();

    QString s("newImage:%1");
    QScreen *screen = QApplication::primaryScreen();
    QPixmap map = screen->grabWindow(m_pCamViewFind->winId()); //ֻ��view���ڶ����ϵ�ͼ��Ҳ��������ͷ������

   //�ٱ����jpgͼ��
    QByteArray ba;
    QBuffer    bf(&ba);
    map.save(&bf, "jpg", 50);

    //��ÿ��QTcpSocket�������ͼ������
    for (int i = 0; i < clients.size(); i++)
    {
        ImgData *d = (ImgData*)clients.at(i)->userData(0);
        if (d->stats != TS_RUNNING) // 1��ʾ������
        {
            memcpy(d->data, ba.data(), ba.size());
            d->totalLen = ba.size();

            if (d->stats == TS_FIRST_DATA)
            {
                d->stats = TS_RUNNING; //��Ϊ�����е�״̬
                d->hasSentLen = 0;
                clients.at(i)->write(s.arg(d->totalLen).toUtf8());
            }
        }
    }
}

void MainWindow::onImageCaptured(int id, const QImage &preview)
{
//    qDebug() << "captured";
    ui->showImageLabel->clear();
    m_Image = preview.convertToFormat(QImage::Format_RGB888);
    //ui->showImageLabel->setPixmap(QPixmap::fromImage(m_Image));
}

