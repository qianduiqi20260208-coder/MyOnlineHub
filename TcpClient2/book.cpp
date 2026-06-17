#include "book.h"
#include "tcpclient2.h"

#include <QInputDialog>
#include <QMessageBox>
book::book(QWidget *parent)
    : QWidget{parent}
{
    returnButton=new QPushButton("返回");
    listWidget=new QListWidget;
    createDirButton=new QPushButton("创建文件夹");
    deleteDirButton=new QPushButton("删除文件夹");
    renameFileButton=new QPushButton("重命名文件");
    flushFileButton=new QPushButton("刷新文件");
    uploadButton=new QPushButton("上传文件");
    downloadButton=new QPushButton("下载文件");
    deleteFileButton=new QPushButton("删除文件");
    shareFileButton=new QPushButton("分享文件");

    //文件操作相关的垂直布局
    QVBoxLayout* operateFile=new QVBoxLayout;
    operateFile->addWidget(returnButton);
    operateFile->addWidget(flushFileButton);
    operateFile->addWidget(createDirButton);
    operateFile->addWidget(deleteDirButton);
    operateFile->addWidget(renameFileButton);

    //传输文件相关的垂直布局
    QVBoxLayout* transferFile=new QVBoxLayout;
    transferFile->addWidget(uploadButton);
    transferFile->addWidget(downloadButton);
    transferFile->addWidget(deleteFileButton);
    transferFile->addWidget(shareFileButton);

        //总体上水平布局
        QHBoxLayout* hl=new QHBoxLayout;
        hl->addWidget(listWidget);
        hl->addLayout(operateFile);
        hl->addLayout(transferFile);

        //设置一下，让元素占满整个窗口
        setLayout(hl);
        connect(createDirButton,SIGNAL(clicked(bool)),this,SLOT(createDir()));
        connect(flushFileButton,SIGNAL(clicked(bool)),this,SLOT(flushFile()));
        connect(uploadButton,SIGNAL(clicked(bool)),this,SLOT(uploadFile()));
        timer=new QTimer;

        connect(timer,SIGNAL(timeout()),this,SLOT(uploadData()));
        //下载文件
        connect(downloadButton,SIGNAL(clicked(bool)),this,SLOT(downloadFile()));
        //分享文件相关的信号与槽
        connect(shareFileButton,SIGNAL(clicked(bool)),this,SLOT(share()));
}
//创建目录
void book::createDir()
{
        QString strPath=TcpClient2::getInstance().getPath();
        QString name=TcpClient2::getInstance().getName();
        //获取用户创建的文件夹的名称
        QString input=QInputDialog::getText(this,"创建文件夹","输入您要创建文件夹的名字");
        if(input.isEmpty())
        {
            QMessageBox::information(this,"创建文件夹","输入的名字不能为空");
            return;
        }
        PDU* p=mkPDU(strPath.size()+100);
        p->uiMsgType=ENUM_CREATE_DIR_CLICKED_REQUEST;
        strncpy((char*)p->caMsg,strPath.toStdString().c_str(),strPath.size());

        strncpy(p->caData,name.toStdString().c_str(),name.size());
        strncpy(p->caData+32,input.toStdString().c_str(),input.size());
        TcpClient2::getInstance().getSocket()->write((char*)p,p->uiMsgLen);
        free(p);
        p=NULL;
}

void book::uploadFile()
{
        //获取文件路径
        filePathUpload=QFileDialog::getOpenFileName();
        //qDebug()<<"文件的路径为："<<filePath;
        //有效性判断
        if(!filePathUpload.isEmpty())
        {
            //使用sprintf将数值类型转换成字符串类型发送出去，将文件的路径拼接好发送出去
            qint64 index=filePathUpload.lastIndexOf('/');
            QString fileName=filePathUpload.right(filePathUpload.size()-index-1);
            QString loginName=TcpClient2::getInstance().getName();
            QFile file(filePathUpload);
            qint64 fileSize=file.size();
            QString path=TcpClient2::getInstance().getPath();
            //拼接好文件的路径
            filePath=path+'/'+loginName+'/'+fileName;
            qDebug()<<filePath;

            PDU* p=mkPDU(filePath.size()+1);
            qDebug()<<1;
            sprintf(p->caData,"%lld",fileSize);
             qDebug()<<2;
            strncpy((char*)p->caMsg,filePath.toStdString().c_str(),filePath.size());
              qDebug()<<3;
            p->uiMsgType=ENUM_UPLOAD_FILE_REQUEST;
               qDebug()<<4;
            TcpClient2::getInstance().getSocket()->write((char*)p,p->uiPDULen);
               qDebug()<<5;
            free(p);
            p=NULL;
            qDebug()<<6;
            timer->start(3000);
            qDebug()<<7;

        }else {
            QMessageBox::information(this,"上传文件","上传文件名字不能为空");
        }

}

void book::flushFile()
{

        QString strPath=TcpClient2::getInstance().getPath();
        QString name=TcpClient2::getInstance().getName();
        PDU* p=mkPDU(strPath.size()+100);
        p->uiMsgType=ENUM_FLUSH_FILE_REQUEST;
        memcpy((char*)p->caMsg,strPath.toStdString().c_str(),strPath.size());
        strncpy(p->caData,name.toStdString().c_str(),name.size());
        TcpClient2::getInstance().getSocket()->write((char*)p,p->uiMsgLen);
        free(p);
        p=NULL;
}

void book::downloadFile()
{
        //拼接地址
        QString downloadDir=QFileDialog::getExistingDirectory();
        qDebug()<<downloadDir;
        if(downloadDir.isEmpty())
        {
            QMessageBox::critical(this,"下载文件","选择的目录不能为空");
            return;
        }

        QString name=TcpClient2::getInstance().getRandomString(5);
        resFilePath=downloadDir+'/'+name;
        qDebug()<<"(downloadFile方法里的打印)resFilePath:"<<resFilePath;

        QString curPath=TcpClient2::getInstance().getPath();
        QString loginName=TcpClient2::getInstance().getName();
        QString fileName=listWidget->currentItem()->text();
        QString path=curPath+'/'+loginName+'/'+fileName;

        PDU* p=mkPDU(path.size()+1);
        p->uiMsgType=ENUM_DOWNLOAD_FILE_REQUEST;
        strncpy((char*)p->caMsg,path.toStdString().c_str(),path.size());
        TcpClient2::getInstance().getSocket()->write((char*)p,p->uiPDULen);
        free(p);
        p=NULL;

}

void book::share()
{
        resFilePath=listWidget->currentItem()->text();
        sh=new shareFileWidget;
        sh->show();
}

 void book::registCreate()
{
        QString strPath=TcpClient2::getInstance().getPath();
        QString name=TcpClient2::getInstance().getName();
        PDU* p=mkPDU(strPath.size()+100);
        p->uiMsgType=ENUM_CREATE_DIR_REQUEST;
        memcpy((char*)p->caMsg,strPath.toStdString().c_str(),strPath.size());
        strncpy(p->caData,name.toStdString().c_str(),name.size());
        TcpClient2::getInstance().getSocket()->write((char*)p,p->uiMsgLen);
        free(p);
        p=NULL;
}
//刷新文件
void book::updateFile(const PDU *p)
{

        if(NULL==p)
        {
            return;
        }
        //刷新之前清除之前刷新出来的内容
        int row=listWidget->count();

        QListWidgetItem* q=NULL;
        while(row>0)
        {
            q=listWidget->item(row-1);
            listWidget->removeItemWidget(q);
            delete q;
            row=row-1;

        }
        q=NULL;

        fileInformation* fileInfo;
        int size=atoi(p->caData);
        for(int i=0;i<size;i++)
        {
            QListWidgetItem* qItem=new QListWidgetItem;
            fileInfo=(fileInformation*)p->caMsg+i;
            //设置图标
            if(fileInfo->isFile==1)
            {
                qItem->setIcon(QIcon(QPixmap(":/map/file.png")));

            }else if(fileInfo==0)
            {
                qItem->setIcon(QIcon(QPixmap(":/map/dir.png")));
            }
            //设置内容
            qItem->setText(fileInfo->info);
            listWidget->addItem(qItem);
        }
}


QString book::getResFilePath()
{
        return resFilePath;
}

void book::uploadData()
{

        timer->stop();
        QFile file(filePathUpload);
        //打开文件，以只读的方式打开
        if(!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::information(this,"上传文件","文件打开失败");
            return;
        }
        char* buffer=new char[4096];
        quint64 ret;
        while(true)
        {
            ret=file.read(buffer,4096);
            if(ret>0&&ret<=4096)
            {
                TcpClient2::getInstance().getSocket()->write(buffer,ret);

            }
            else if(ret==0)
            {
                break;
            }else {
                QMessageBox::warning(this,"上传文件","读取文件失败");
                break;
            }
        }
        file.close();
        delete []buffer;
        buffer=NULL;
}


