//PC上位机是左上为0点
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


#include <QLabel>
#include <QScrollArea>
#include <QScrollBar>
QT_BEGIN_NAMESPACE

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    void loadMenu();
    bool loadFile(const QString &);
    void setImage(const QImage &);
    bool eventFilter(QObject *obj, QEvent *event);
    void cmdPrtScreen();
private:
    Ui::MainWindow *ui;
    QImage image;
    QLabel *imageLabel;
    QScrollArea *scrollArea;
    double scaleFactor;
    QString picPath;
};

#endif // MAINWINDOW_H
