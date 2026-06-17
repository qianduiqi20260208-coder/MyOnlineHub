#ifndef SHAREFILEWIDGET_H
#define SHAREFILEWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QTimer>
#include <QFile>
class shareFileWidget : public QWidget
{
    Q_OBJECT
public:
    explicit shareFileWidget(QWidget *parent = nullptr);
private slots:
    void shareFile();
    void sendFile();
signals:
private:
    QListWidget* listWidget;
    QPushButton* shareButton;
    QHBoxLayout* hboxLayout;
    QTimer* timer;
    QFile f;
};

#endif // SHAREFILEWIDGET_H
