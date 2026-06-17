#ifndef BOOK_H
#define BOOK_H

#include <QWidget>
#include<QPushButton>
#include<QListWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "protocol.h"
#include <QFileDialog>
#include<QTimer>
#include "sharefilewidget.h"
class book : public QWidget
{
    Q_OBJECT
public:
    explicit book(QWidget *parent = nullptr);

    void registCreate();
    void updateFile(const PDU* p);

    QString getResFilePath();

private:
        QPushButton* returnButton;
        QListWidget* listWidget;
        QPushButton* createDirButton;
        QPushButton* deleteDirButton;
        QPushButton* renameFileButton;
        QPushButton* flushFileButton;
        QPushButton* uploadButton;
        QPushButton* downloadButton;
        QPushButton* deleteFileButton;
        QPushButton* shareFileButton;

signals:

private slots:
    void uploadData();
    void createDir();
    void uploadFile();
    void flushFile();
    void downloadFile();
    void share();
private:
    QTimer* timer;
    //上传文件的存放位置

    QString filePath;
    //要上传的文件
    QString filePathUpload;
    //下载文件的时候选择的下载位置
    QString resFilePath;
    shareFileWidget* sh;
};

#endif // BOOK_H
