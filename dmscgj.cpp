#include "dmscgj.h"
#include "ui_dmscgj.h"

#include <QIcon>
#include <QFile>
#include <QDirModel>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardItem>

#include "replace.h"

#include <QDebug>

DMSCGJ::DMSCGJ(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DMSCGJ)
{
    ui->setupUi(this);

    initMenubar();

    initValue();

    connect(ui->treeView_filesView,SIGNAL(clicked(const QModelIndex &)),this,SLOT(showFile(const QModelIndex &)));

}

DMSCGJ::~DMSCGJ()
{
    delete ui;
}

/**
 * @brief DMSCGJ::initMenubar 初始化菜单栏功能
 */
void DMSCGJ::initMenubar()
{
    QMenu* fileMenu = ui->menuBar->addMenu("项目");

    QAction* action;
    action = new QAction("打开项目",this);
    connect(action,SIGNAL(triggered()),this,SLOT(openFile()));
    fileMenu->addAction(action);

    action = new QAction("保存项目",this);
    connect(action,SIGNAL(triggered()),this,SLOT(saveFile()));
    fileMenu->addAction(action);
}

/**
 * @brief DMSCGJ::initVlaue 初始化key_value表格
 */
void DMSCGJ::initValue()
{
    //QStandardItemModel *model = new QStandardItemModel(this);

    model_key_value->setColumnCount(2);
    model_key_value->setHeaderData(0,Qt::Horizontal,QString::fromLocal8Bit("key"));
    model_key_value->setHeaderData(1,Qt::Horizontal,QString::fromLocal8Bit("value"));

    ui->tableView_key_value->setModel(model_key_value);
    //列宽度自适应,填满表格区域
    ui->tableView_key_value->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

}

/**
 * @brief DMSCGJ::keyValueList
 * @param list
 */
void DMSCGJ::keyValueList(QStringList list)
{
    for(int i = 0; i<list.size(); i++)
    {
        if(t_keyValueList.contains(list.at(i)))
        {
            continue;
        }
        else
        {
            t_keyValueList.append(list.at(i));
        }
    }
}

/**
 * @brief DMSCGJ::addKeyValue
 * @param list
 */
void DMSCGJ::addKeyValue(QStringList list)
{
    //QStandardItemModel *model = new QStandardItemModel(this);

    model_key_value->clear();
    initValue();
    ui->tableView_key_value->setModel(model_key_value);

    for(int i = 0; i < list.size(); i++)
    {
        QStandardItem* item = new QStandardItem(list.at(i));
        model_key_value->appendRow(item);
    }

    initValue();
    ui->tableView_key_value->setModel(model_key_value);
}

/**
 * @brief DMSCGJ::addKeyValues
 * @param list
 */
void DMSCGJ::addKeyValues(QStringList list)
{
    //QStandardItemModel *model = new QStandardItemModel(this);

    for(int i = 0; i < list.size(); i++)
    {
        QStandardItem* item = new QStandardItem(list.at(i));
        model_key_value->appendRow(item);
    }

    initValue();
    ui->tableView_key_value->setModel(model_key_value);
}

/**
 * @brief DMSCGJ::getValue
 */
void DMSCGJ::getValue()
{
    qDebug()<< ui->tableView_key_value; //获得当前选中的列数
    qDebug()<<model_key_value->data(model_key_value->index(0,1)).toString();
    qDebug()<<model_key_value->rowCount();
    for(int i = 0; i < model_key_value->rowCount(); i++)
    {
        t_valueList.append(model_key_value->data(model_key_value->index(i,1)).toString());
    }
}

/**
 * @brief DMSCGJ::readFile
 * @param path
 */
void DMSCGJ::readFile(QString path)
{
    QFile file(path);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        //        qDebug()<<"Can't open the file!"<<endl;

        QMessageBox::warning(this, tr("My Application"),
                             tr("this file can't open!"),
                             QMessageBox::Ok);
        t_content = "";

        return;
    }

    //windows，MAC均可识别中文字符
    QTextStream in(&file);
    in.setCodec("UTF-8");
    t_content = in.readAll();

    //        MAC上可识别中文字符
    //        while(!file.atEnd())
    //        {
    //            QByteArray line = file.readLine();
    //            //QString str(line);
    //            QString str = QString::fromLocal8Bit(line);
    //            t_content.append(str);
    //        }

    file.close();
}

/**
 * @brief DMSCGJ::openFile 打开工程文件，并在项目文件区显示
 */
void DMSCGJ::openFile()
{
    t_currentFileName = QFileDialog::getExistingDirectory();

    QStandardItemModel* model = new QStandardItemModel(this);

    if(t_currentFileName == NULL)
    {
        return;
    }

    ui->textEdit_codeEdit->clear();
    model_key_value->clear();
    t_keyValueList.clear();
    t_valueList.clear();
    initValue();
    ui->tableView_key_value->setModel(model_key_value);

    QDir dir(t_currentFileName);

    QStyle* style = QApplication::style();
    QIcon icon = style->standardIcon(QStyle::SP_DirIcon);
    QStandardItem* root = new QStandardItem(icon,dir.dirName());
    model->appendRow(root);

    if (!dir.exists()) return;
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoSymLinks);

    QFileInfoList list = dir.entryInfoList();

    int size = list.size();
    for (int i = 0; i < size; i++)
    {
        QFileInfo info = list.at(i);
        if (info.fileName() == "." || info.fileName() == "..")
        {
            continue;
        }
        if (info.isDir())
        {
            QStyle* style = QApplication::style();
            QIcon icon = style->standardIcon(QStyle::SP_DirIcon);
            QStandardItem* item = new QStandardItem(icon,info.fileName());
            root->appendRow(item);
            openFiles(info.filePath(),item);
        }
        else
        {
            QStyle* style = QApplication::style();
            QIcon icon = style->standardIcon(QStyle::SP_FileIcon);
            QStandardItem* fileitem = new QStandardItem(icon,info.fileName());
            fileitem->setData(info.absoluteFilePath(),Qt::UserRole);   //写入文件路径
            root->appendRow(fileitem);

            //显示当前内容的关键字
            readFile(info.absoluteFilePath());
            Replace a(t_content);
            QStringList t_template = a.currTemplate();
            keyValueList(t_template);
        }
    }

    addKeyValues(t_keyValueList);
    ui->treeView_filesView->setModel(model);

    //    方法二
    //    QDirModel *model = new QDirModel();
    //    ui->treeView_filesView->setModel(model);
    //    ui->treeView_filesView->setRootIndex(model->index(t_currentFileName));

}

/**
 * @brief DMSCGJ::openFiles 递归打开目录下目录
 * @param path  目录路径
 * @param temp  树形显示节点
 */
void DMSCGJ::openFiles(QString path, QStandardItem*temp)
{
    QDir dir(path);
    if (!dir.exists()) return;
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoSymLinks);
    QFileInfoList list = dir.entryInfoList();
    int size = list.size();
    for (int i = 0; i < size; i++)
    {
        QFileInfo info = list.at(i);
        if (info.fileName() == "." || info.fileName() == "..")
        {
            continue;
        }
        if (info.isDir())
        {
            QStyle* style = QApplication::style();
            QIcon icon = style->standardIcon(QStyle::SP_DirIcon);
            QStandardItem* item = new QStandardItem(icon,info.fileName());
            temp->appendRow(item);
            openFiles(info.filePath(),item);
        }
        else
        {
            QStyle* style = QApplication::style();
            QIcon icon = style->standardIcon(QStyle::SP_FileIcon);
            QStandardItem* fileitem = new QStandardItem(icon,info.fileName());
            fileitem->setData(info.absoluteFilePath(),Qt::UserRole);
            temp->appendRow(fileitem);

            //显示当前内容的关键字
            readFile(info.absoluteFilePath());
            Replace a(t_content);
            QStringList t_template = a.currTemplate();
            keyValueList(t_template);
        }
    }
}

/**
 * @brief DMSCGJ::saveFile
 */
void DMSCGJ::saveFile()
{
    getValue();

    QString saveFileName = QFileDialog::getExistingDirectory();

    QDir dir(t_currentFileName);

    if (!dir.exists()) return;
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoSymLinks);

    QFileInfoList list = dir.entryInfoList();

    int size = list.size();
    for (int i = 0; i < size; i++)
    {
        QFileInfo info = list.at(i);
        if (info.fileName() == "." || info.fileName() == "..")
        {
            continue;
        }
        if (info.isDir())
        {
            saveFiles(info.filePath(), saveFileName);
        }
        else
        {
            //替换当前内容的关键字
            readFile(info.absoluteFilePath());
            Replace a(t_content);
            QString content = a.replaceTemplates(t_valueList);

            if(content == NULL)
            {
                continue;
            }
            //
            //写入文件
            QString filePath = QString("%1/%2").arg(saveFileName).arg(info.fileName());
            QFile fileWrite(filePath);
            if (!fileWrite.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QMessageBox::warning(this, tr("My Application"),
                                     tr("file open error"),
                                     QMessageBox::Ok);
                return;
            }
            QTextStream out(&fileWrite);
            out << content;
            qDebug()<<"Save Success";
            fileWrite.close();
        }
    }
}

/**
 * @brief DMSCGJ::saveFiles
 * @param path
 */
void DMSCGJ::saveFiles(QString path ,QString dirName)
{
    QDir dir(path);
    if (!dir.exists()) return;
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoSymLinks);
    QFileInfoList list = dir.entryInfoList();
    int size = list.size();
    for (int i = 0; i < size; i++)
    {
        QFileInfo info = list.at(i);
        if (info.fileName() == "." || info.fileName() == "..")
        {
            continue;
        }
        if (info.isDir())
        {
            saveFiles(info.filePath(), dirName);
        }
        else
        {
            //替换当前内容的关键字
            readFile(info.absoluteFilePath());
            Replace a(t_content);
            QString content = a.replaceTemplates(t_valueList);

            if(content == NULL)
            {
                continue;
            }
            //写入文件
            QString filePath = QString("%1/%2").arg(dirName).arg(info.fileName());
            QFile fileWrite(filePath);
            if (!fileWrite.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QMessageBox::warning(this, tr("My Application"),
                                     tr("file open error"),
                                     QMessageBox::Ok);
                return;
            }
            QTextStream out(&fileWrite);
            out << content;
            qDebug()<<"Save Success";
            fileWrite.close();
        }
    }
}

/**
 * @brief DMSCGJ::showFile  显示文件内容
 * @param index 当前点击树形节点值
 */
void DMSCGJ::showFile(const QModelIndex &index)
{
    //qDebug()<< index.data(Qt::UserRole).toString();
    QString path = index.data(Qt::UserRole).toString();
    readFile(path);

    //    //显示当前文件内容的关键字
    //    Replace a(t_content);
    //    QStringList t_template = a.currTemplate();
    //    addKeyValue(t_template);

    ui->textEdit_codeEdit->clear();
    ui->textEdit_codeEdit->setPlainText(t_content);
}
