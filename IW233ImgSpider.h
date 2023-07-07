#pragma once

#include <QtWidgets/QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <vector>
#include <QDateTime>
#include "ui_IW233ImgSpider.h"

class IW233ImgSpider : public QWidget
{
    Q_OBJECT

public:
    IW233ImgSpider(QWidget *parent = nullptr);
    ~IW233ImgSpider();

private:
    Ui::IW233ImgSpiderClass ui;
    std::vector<QNetworkAccessManager*> p_networkers;
    //std::vector<QNetworkAccessManager*> p_network_managers;
    //std::vector<QNetworkReply*> replys;
    //QNetworkAccessManager *network = new QNetworkAccessManager(this);
    //QNetworkRequest *network_request = new QNetworkRequest;
    QByteArray::DataPointer p_pic;
    std::vector<QByteArray::DataPointer> p_pic_list;
    std::vector<QDateTime> timestamps;
    int pic_total_count;

    void startWork();
    void getImage();
    void receiveImage(QNetworkReply*);
    void showImage();
    void saveToDir1();
    void saveToDir2();
    void saveToDir3();
    void saveToDir4();
    void saveToTemp();
    void deleteImg();
    void saveToFile(QString dir);
    void roundFinished(bool isSuccess = true);
    void calcSpeed();
};
