#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QLabel>
#include <QProgressBar>
#include <QFileDialog>
#include <QMainWindow>
#include <QStringList>

namespace Ui {
class MainWindow;
}

/*Класс работы с файлами*/
class ParseFileIteris:public QObject
{
Q_OBJECT

QStringList *strList;
QString name,fileName,filePath;
quint64 sizeFile;
quint64 countSizeFile;
    /*check Text number*/
    bool checkNumber(QChar smb)
    {
    return ((smb>QChar('0'))&&(smb<=QChar('9')))? true:false;
    }
    /* check One String data */
    bool checkStr(QString &str)
    {
    int index;
    QStringList strVideo;
    strVideo<<" No Video"<<" Video OK"<<" Video Low Contrast";
    countSizeFile += str.size(); // count size
    if((str.isEmpty())||(str.size()<59)) return false;
        if((str.at(0) ==QChar('0'))&&
           (str.at(1) ==QChar('0'))&&
           (checkNumber(str.at(2)))&&
           (str.at(3) ==QChar(','))&&
           (str.at(4) ==QChar(' '))&&
           (str.at(9) ==QChar('-'))&&
           (str.at(12)==QChar('-'))&&
           (str.at(15)==QChar(' '))&&
           (str.at(18)==QChar(':'))&&
           (str.at(21)==QChar(':'))&&
           (str.at(24)==QChar(','))&&
           (str.at(25)==QChar(' '))
          ){
           // проверить на символы 0x0D, 0x0A
           if(str.contains("\r", Qt::CaseInsensitive))return false;
           if(str.contains("\n", Qt::CaseInsensitive))return false;
           // проверка на 15шт. запятых
           int count = 0,inx=0;
               do{
                count=str.indexOf(',',count);
                if((count==0)||(count==-1))break;
                inx++;
                // две запятые вместе
                QChar pr(str.at(count-1));
                QChar sl(str.at(count+1));
                if((pr==QChar(','))&&(sl==QChar(',')))return false;
                count++;
                }while(true);
           if(inx!=15)return false;
           // провекра конца строки
           foreach (const QString &stv, strVideo)
            {
            index = str.indexOf(stv);
            if(index!=-1){
                index+=stv.size();
                if(str.size()>index)str.remove(index,str.size()-index);
                strList->append(str);
                qDebug() << str;
                return true;
                }
            }
           }
    return false;
    }
    /* save new files */
    void SaveFile(void)
    {
    const QString flPath = filePath.isEmpty()?name+"_cor.txt":filePath+"\\"+name+"_cor.txt";

    QFile file(flPath);
    if (file.open(QIODevice::WriteOnly)){
        emit isSendPath(QFileInfo(flPath).absoluteFilePath());
        QTextStream stream(&file);
        foreach (const QString &str, *strList)
            {
            stream<<str+"\r\n";
            emit isProgress(100);
            }
        file.close();
        if (stream.status() != QTextStream::Ok){
            qDebug() << "Ошибка записи файла";
            }
        }
    }
public:
    explicit ParseFileIteris(QObject *parent = 0,
                             const QString flNm="",
                             const QString flPth=""):QObject(parent),fileName(flNm),filePath(flPth)
                {strList = new QStringList;}
    ~ParseFileIteris(){
        qDebug("By in Thread!");emit isActive(false);delete strList;}
    /* open file */
    void OpenFile(void)
    {
    QFileInfo fi(fileName);
    name = fi.baseName();
    sizeFile = fi.size();
    countSizeFile = 0;
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly)){
        QTextStream stream(&file);
        QString str;
        while (!stream.atEnd())
            {
            str = stream.readLine();
            checkStr(str);
            countSizeFile +=str.size();
            const int lengh  =  (int)((countSizeFile*90)/sizeFile);
            emit isProgress(lengh);
            }
        if(stream.status()!= QTextStream::Ok){
            qDebug() << "Ошибка чтения файла";
            }
        file.close();
        SaveFile(); // новый файл
        }
    }
signals:
    void isFinished(void); // сигнал завершение работы
    void isProgress(int);
    void isActive(bool);
    void isSendPath(QString);
public slots:
    void processRun(void){
        emit isProgress(0);
        emit isActive(true);
        qDebug("Hello World in Thread!");
        OpenFile();
        emit isFinished();
        }

};
/*Основной класс main*/
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QString fileName;
    QString filePath;
    QProgressBar *progress_status;
    void CreateStyle();
private slots:
    void on_OpenFile();
    void on_OpenPath();
    void on_ConvertFile();

};

#endif // MAINWINDOW_H
