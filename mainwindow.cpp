#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
/*connection signals*/
connect(ui->tButOpen,SIGNAL(clicked()),this,SLOT(on_OpenFile()));
connect(ui->tbPath,SIGNAL(clicked()),this,SLOT(on_OpenPath()));
connect(ui->pButConvert,SIGNAL(clicked()),this,SLOT(on_ConvertFile()));
/*Create progress bar*/
progress_status = new QProgressBar(this);
progress_status->setVisible(false);
progress_status->setTextVisible(true);
ui->statusBar->addPermanentWidget(progress_status,0);
/*Set message in status bar*/
QLabel *p = new QLabel(this);
p->setText(tr("version 1.1"));
p->setOpenExternalLinks(true);
ui->statusBar->addPermanentWidget(p);
ui->statusBar->showMessage(tr("Clean text files"),5000);
/*toop tip message*/
ui->tButOpen->setToolTip(tr("Select file"));
ui->tbPath->setToolTip(tr("Select path"));
/*Create Style*/
//CreateStyle();
}

MainWindow::~MainWindow()
{
    delete ui;
}
/*Create Style*/
void MainWindow::CreateStyle()
{
/*set Style Sweet the windows and button*/
QFile styleFile(":/QSS/QSS/style_files001.qss");
bool result = styleFile.open(QFile::ReadOnly);
if(result){
    QByteArray style = styleFile.readAll();
    qApp->setStyleSheet(style);
    }
}
/*open file */
void MainWindow::on_OpenFile()
{
fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"/home",tr("Text (*.txt)"));
ui->lineEditSource->setText(fileName);
}
/*open path*/
void MainWindow::on_OpenPath()
{
filePath = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                             "/home",
                                             QFileDialog::ShowDirsOnly
                                             | QFileDialog::DontResolveSymlinks);
ui->lineEditPath->setText(filePath);
}
/*Convert file in thread*/
void MainWindow::on_ConvertFile()
{
QThread *thread = new QThread;//Создаем поток для порта платы
ParseFileIteris *parserIteris = new ParseFileIteris(0,fileName,filePath);//Создаем обьект по классу
parserIteris->moveToThread(thread);//помешаем класс  в поток
// connects
connect(thread, SIGNAL(started()), parserIteris, SLOT(processRun()));//Переназначения метода run
connect(parserIteris, SIGNAL(isFinished()),thread, SLOT(quit()));      //Переназначение метода выход
connect(thread, SIGNAL(finished()), parserIteris, SLOT(deleteLater()));//Удалить к чертям поток
connect(parserIteris, SIGNAL(isFinished),thread, SLOT(deleteLater()));//Удалить к чертям поток
connect(parserIteris,SIGNAL(isActive(bool)),ui->pButConvert,SLOT(setDisabled(bool)));
connect(parserIteris,SIGNAL(isSendPath(QString)),ui->lineEditPath,SLOT(setText(QString)));
connect(parserIteris,SIGNAL(isActive(bool)),progress_status,SLOT(setVisible(bool)));
connect(parserIteris,SIGNAL(isProgress(int)),progress_status,SLOT(setValue(int)));
// start tread
thread->start();
}
