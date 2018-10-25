#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QStandardPaths>
#include <QImageReader>
#include <QImageWriter>
#include <QMessageBox>
#include <QMouseEvent>
#include <QProcess>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),imageLabel(new QLabel),scrollArea(new QScrollArea),scaleFactor(0.34)
{
    ui->setupUi(this);
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);
    imageLabel->installEventFilter(this);

    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    scrollArea->setVisible(false);
    setCentralWidget(scrollArea);

    loadMenu();
}

MainWindow::~MainWindow()
{
    delete ui;
}

static void initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode)
{
    static bool firstDialog = true;

    if (firstDialog) {
        firstDialog = false;
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    }

    QStringList mimeTypeFilters;
    const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen
        ? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
    foreach (const QByteArray &mimeTypeName, supportedMimeTypes)
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/png");
//    if (acceptMode == QFileDialog::AcceptSave)
//        dialog.setDefaultSuffix("png");
}

void MainWindow::loadMenu()
{
    auto mb = menuBar();

    auto mn = mb->addMenu("操作");

    auto act = mn->addAction("打开");

    connect(act,&QAction::triggered,[=](){
        QFileDialog dialog(this, tr("Open File"));
        initializeImageFileDialog(dialog, QFileDialog::AcceptOpen);
        while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first())) {}

    });

    act = mn->addAction("退出");
    connect(act,&QAction::toggled,[this](){
        this->close();
    });

    act = mb->addAction("截图");
    connect(act,&QAction::triggered,[this](){

//        QProcess p(0);
//        p.start("cmd");
//        QStringList argument;
//        p.waitForStarted();
//         argument<<"/c"<<;
//        p.waitForFinished()

        this->cmdPrtScreen();
    });

    act = mb->addAction("刷新");
    connect(act,&QAction::triggered,[this](){
        if(picPath!=nullptr&&picPath.length()>0){
            loadFile(picPath);
        }
    });
}

void MainWindow::cmdPrtScreen(){

    QProcess::execute("adb shell screencap -p /sdcard/autojump1.png");
    QProcess::execute("adb pull /sdcard/autojump1.png");
}

bool MainWindow::loadFile(const QString &fileName)
{
    picPath = fileName;
    QImageReader reader(fileName);
    reader.setAutoTransform(true);
    const QImage newImage = reader.read();
    if (newImage.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1: %2")
                                 .arg(QDir::toNativeSeparators(fileName), reader.errorString()));
        return false;
    }
    setImage(newImage);

    setWindowFilePath(fileName);

//    const QString message = tr("Opened \"%1\", %2x%3, Depth: %4")
//        .arg(QDir::toNativeSeparators(fileName)).arg(image.width()).arg(image.height()).arg(image.depth());
//    statusBar()->showMessage(message);

    return true;
}

void MainWindow::setImage(const QImage &newImage)
{
    image = newImage;
    imageLabel->setPixmap(QPixmap::fromImage(image));
//! [4]

    imageLabel->x();



    scrollArea->setVisible(true);

    imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == imageLabel)//指定某个QLabel
    {
        if (event->type() == QEvent::MouseButtonPress)//mouse button pressed
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::LeftButton)
            {
                QString strmsg_x(QString::number(mouseEvent->pos().x()/scaleFactor));

                //QString strmsg_y(mouseEvent->pos().y());
                QString strmsg_y(QString::number(mouseEvent->pos().y()/scaleFactor));

                QString strmsg(strmsg_x + " ," + strmsg_y);


                QString cmdStr("adb shell input swipe "+strmsg_x +" " +strmsg_y+" " +strmsg_x +" " +strmsg_y+" 300");
                QProcess::execute(cmdStr);


                cmdPrtScreen();
                loadFile(picPath);


//                QMessageBox::information(NULL,"",strmsg,
//                                         QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        // pass the event on to the parent class
        return QMainWindow::eventFilter(obj, event);
    }
}


