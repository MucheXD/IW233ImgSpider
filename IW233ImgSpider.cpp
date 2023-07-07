#include "IW233ImgSpider.h"


IW233ImgSpider::IW233ImgSpider(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    ui.edit_dir1->setText(QApplication::applicationDirPath() + "/img/CHARA/");
    ui.edit_dir2->setText(QApplication::applicationDirPath() + "/img/SCENE/");
    ui.edit_dir3->setText(QApplication::applicationDirPath() + "/img/BKGND/");
    ui.edit_dir4->setText(QApplication::applicationDirPath() + "/img/AVATOR/");
    ui.edit_tmp->setText(QApplication::applicationDirPath() + "/img/TMP/");

    pic_total_count = 0;

    connect(ui.pb_run, &QPushButton::clicked, this, &IW233ImgSpider::startWork);
    connect(ui.pb_dir1, &QPushButton::clicked, this, &IW233ImgSpider::saveToDir1);
    connect(ui.pb_dir2, &QPushButton::clicked, this, &IW233ImgSpider::saveToDir2);
    connect(ui.pb_dir3, &QPushButton::clicked, this, &IW233ImgSpider::saveToDir3);
    connect(ui.pb_dir4, &QPushButton::clicked, this, &IW233ImgSpider::saveToDir4);
    connect(ui.pb_tmp, &QPushButton::clicked, this, &IW233ImgSpider::saveToTemp);
    connect(ui.pb_del, &QPushButton::clicked, this, &IW233ImgSpider::deleteImg);
}

void IW233ImgSpider::startWork()
{
    ui.label_status->setText("Starting work...");
    QDir tmpdir;
    tmpdir.setPath(ui.edit_tmp->text());
    if (!tmpdir.exists())
    {
        if(!tmpdir.mkpath(ui.edit_tmp->text()))
        {
            QMessageBox::critical(this,
                "Cannot create dirpath",
                QString("The dir: \n%1\n does not exists, and we cannot create it.").arg(ui.edit_tmp->text()),
                QMessageBox::Ok);
            return;
        }
    }
    getImage();
    showImage();
}

void IW233ImgSpider::getImage()
{
    QNetworkRequest network_request;
    QNetworkAccessManager* network_manager = new QNetworkAccessManager;
    p_networkers.push_back(network_manager);
    p_networkers.back()->setTransferTimeout(2000);
    network_request.setRawHeader("Referer", "https://weibo.com/");
    network_request.setUrl(QUrl(ui.edit_url->text()));
    connect(p_networkers.back(), &QNetworkAccessManager::finished, this, &IW233ImgSpider::receiveImage);
    QNetworkReply* network_reply = network_manager->get(network_request);
}

void IW233ImgSpider::receiveImage(QNetworkReply * reply)
{
    QNetworkAccessManager* p_manager = qobject_cast<QNetworkAccessManager*>(sender());
    p_manager->deleteLater();
    pic_total_count += 1;
    if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 200)
    {
        ui.label_status->setText(QString("Image #%1 getted successfully!").arg(pic_total_count));
        QByteArray data = reply->readAll();
        p_pic_list.insert(p_pic_list.begin(), data.data_ptr());
    }
    else
    {
        ui.label_status->setText(QString("Network error when getting image #%1... omitted").arg(pic_total_count));
    }
    ui.pg_buffer->setValue(p_pic_list.size());

    //showImage();
    if (p_pic_list.size() < 5 && ui.pb_run->isChecked() == true)
    {
        getImage();
    }
}

void IW233ImgSpider::showImage()
{   
    if (p_pic_list.size() == 0)
    {
        ui.label_status->setText("We are downloading now, Ctrl+Q to try again.");
        return;
    }
    p_pic = p_pic_list.back();
    p_pic_list.pop_back();
    ui.pg_buffer->setValue(p_pic_list.size());
    QPixmap pixmap;
    QByteArray data;
    //data.resize(p_pic.size);
    data.setRawData(p_pic.data(),p_pic.size);
    pixmap.loadFromData(data);
    if (pixmap.height() > pixmap.width())
        pixmap = pixmap.scaledToHeight(ui.label_img->height(), Qt::SmoothTransformation);
    else
        pixmap = pixmap.scaledToWidth(ui.label_img->width(), Qt::SmoothTransformation);
    //pixmap = pixmap.scaledToWidth(ui.label_img->width());
    //pixmap = pixmap.scaledToHeight(ui.label_img->height());
    ui.label_img->setPixmap(pixmap);
    ui.pg_quality->setValue(p_pic.size / 1000);
}

void IW233ImgSpider::saveToDir1() { saveToFile(ui.edit_dir1->text()); }
void IW233ImgSpider::saveToDir2() { saveToFile(ui.edit_dir2->text()); }
void IW233ImgSpider::saveToDir3() { saveToFile(ui.edit_dir3->text()); }
void IW233ImgSpider::saveToDir4() { saveToFile(ui.edit_dir4->text()); }
void IW233ImgSpider::saveToTemp() { saveToFile(ui.edit_tmp->text()); }

void IW233ImgSpider::saveToFile(QString dir)
{
    QString fileName = dir + QString::number(p_pic->size) + ".jpg";
    ui.label_status->setText(QString("Saving file to: %1").arg(fileName));
    QFile file;
    file.setFileName(fileName);
    if (file.exists() == true)
    {
        QMessageBox::warning(this,
            "File does exists",
            QString("The file: \n%1\n does exists, press OK will rewrite it.").arg(fileName),
            QMessageBox::Ok);
    }
    if (!file.open(QIODevice::ReadWrite))
    {
        saveToTemp();
        QMessageBox::critical(this,
            "Cannot open file",
            QString("The file: \n%1\n cannot be open, the image has saved to tempdir.").arg(fileName),
            QMessageBox::Ok);
        p_pic.clear();
        roundFinished(false);
        return;
    }
    file.resize(0);
    file.write(p_pic->data(), p_pic->size);
    file.close();
    p_pic.clear();
    ui.label_img->clear();
    roundFinished(true);
}

void IW233ImgSpider::deleteImg()
{
    p_pic.clear();
    ui.label_img->clear();
    roundFinished(true);
}

void IW233ImgSpider::roundFinished(bool isSuccess)
{
    timestamps.push_back(QDateTime::currentDateTime());
    calcSpeed();
    if (isSuccess)
    {
        showImage();
        getImage();
        return;
    }
    else
    {
        //ui.pb_run->setChecked(false);
        return;
    }
}

void IW233ImgSpider::calcSpeed()
{
    if (timestamps.size() == 0)
        return;
    
    while (timestamps.at(0).secsTo(QDateTime::currentDateTime()) >= 60)
    {
        timestamps.erase(timestamps.begin());
        if (timestamps.size() == 0)
            break;
    }
    ui.pg_speed->setValue(timestamps.size());
}

IW233ImgSpider::~IW233ImgSpider()
{}
