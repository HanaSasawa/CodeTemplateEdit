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
    model_key_value->setColumnCount(2);
    model_key_value->setHeaderData(0,Qt::Horizontal,QString::fromLocal8Bit("key"));
    model_key_value->setHeaderData(1,Qt::Horizontal,QString::fromLocal8Bit("value"));

    ui->tableView_key_value->setModel(model_key_value);
    //列宽度自适应,填满表格区域
    ui->tableView_key_value->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

}

/**
 * @brief DMSCGJ::correctKeyList  添加的key值为唯一值
 * @param list 当前文件中含有的key值
 */
void DMSCGJ::correctKeyList(QStringList list)
{
    for(int i = 0; i<list.size(); i++)
    {
        if(t_keyList.contains(list.at(i)))
        {
            continue;
        }
        else
        {
            t_keyList.append(list.at(i));
        }
    }
}

/**
 * @brief DMSCGJ::addKeyValue  在表格中显示当前打开文件key值
 * @param list 当前打开文件key值表
 */
void DMSCGJ::addKeyValue(QStringList list)
{
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
 * @brief DMSCGJ::addKeyValues  在表格中显示所有可打开文件key值
 * @param list  当前打开所有可读文件key值表
 */
void DMSCGJ::addKeyValues(QStringList list)
{
    for(int i = 0; i < list.size(); i++)
    {
        QStandardItem* item = new QStandardItem(list.at(i));
        model_key_value->appendRow(item);
    }

    initValue();
    ui->tableView_key_value->setModel(model_key_value);
}

/**
 * @brief DMSCGJ::setValueList  从输入表格中获取相对应key值的value值
 */
void DMSCGJ::setValueList()
{
    //    qDebug()<<model_key_value->data(model_key_value->index(0,1)).toString();
    //    qDebug()<<model_key_value->rowCount();
    for(int i = 0; i < model_key_value->rowCount(); i++)
    {
        t_valueList.append(model_key_value->data(model_key_value->index(i,1)).toString());
    }
}

/**
 * @brief DMSCGJ::readFile  读取当前点击文件的内容
 * @param path 当前点击文件路径
 */
void DMSCGJ::readFile(QString path)
{
    QFile file(path);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, tr("My Application"),
                             tr("this file can't open!"),
                             QMessageBox::Ok);
        t_content = "";     //当打开的是文件夹时，返回内容为空
        return;
    }

    //windows，MAC均可识别中文字符
    QTextStream in(&file);
    in.setCodec("UTF-8");
    t_content = in.readAll();

    //        Mac上可识别中文字符
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
    t_currentDirName = QFileDialog::getExistingDirectory();
    if(t_currentDirName == NULL)  //未选择目录操作
    {
        return;
    }

    //刷新界面和存值
    ui->textEdit_codeEdit->clear();
    model_key_value->clear();
    t_keyList.clear();
    t_valueList.clear();
    initValue();
    ui->tableView_key_value->setModel(model_key_value);

    QDir dir(t_currentDirName);
    if (!dir.exists()) return;

    QStandardItemModel* model = new QStandardItemModel(this);  //显示当前项目文件表格

    QStyle* style = QApplication::style();
    QIcon icon = style->standardIcon(QStyle::SP_DirIcon);
    QStandardItem* root = new QStandardItem(icon,dir.dirName());
    model->appendRow(root);

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
            openFiles(info.filePath(),item);  //递归
        }
        else
        {
            QStyle* style = QApplication::style();
            QIcon icon = style->standardIcon(QStyle::SP_FileIcon);
            QStandardItem* fileitem = new QStandardItem(icon,info.fileName());
            fileitem->setData(info.absoluteFilePath(),Qt::UserRole);   //在表格中写入文件路径
            root->appendRow(fileitem);

            //读取当前文件内容的关键字
            readFile(info.absoluteFilePath());
            Replace a(t_content);
            QStringList t_template = a.currTemplate();
            correctKeyList(t_template);
        }
    }

    addKeyValues(t_keyList);
    ui->treeView_filesView->setModel(model);

    //    方法二
    //    QDirModel *model = new QDirModel();
    //    ui->treeView_filesView->setModel(model);
    //    ui->treeView_filesView->setRootIndex(model->index(t_currentDirName));

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

            readFile(info.absoluteFilePath());
            Replace a(t_content);
            QStringList t_template = a.currTemplate();
            correctKeyList(t_template);
        }
    }
}

/**
 * @brief DMSCGJ::saveFile  替换key值，且保存在选择的文件路径
 */
void DMSCGJ::saveFile()
{
    QString saveFileName = QFileDialog::getExistingDirectory();
    if(saveFileName == NULL)  //未选择目录操作
    {
        return;
    }

    setValueList();

    QDir dir(t_currentDirName);

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
            saveFiles(info.filePath(), saveFileName); //递归
        }
        else
        {
            //替换当前内容的关键字
            readFile(info.absoluteFilePath());
            Replace a(t_content);
            QString content = a.replaceTemplates(t_valueList);

            if(content == NULL) //没有替换内容，即跳过
            {
                continue;
            }

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
 * @brief DMSCGJ::saveFiles   递归打开目录下目录
 * @param path  目录路径
 * @param dirName  保存文件夹名
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
            readFile(info.absoluteFilePath());
            Replace a(t_content);
            QString content = a.replaceTemplates(t_valueList);
            if(content == NULL)
            {
                continue;
            }
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

    //    显示当前文件内容的关键字
    //    Replace a(t_content);
    //    QStringList t_template = a.currTemplate();
    //    addKeyValue(t_template);

    ui->textEdit_codeEdit->clear();
    ui->textEdit_codeEdit->setPlainText(t_content);
}
