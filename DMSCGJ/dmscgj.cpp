#include "dmscgj.h"
#include "ui_dmscgj.h"

#include <QFile>
#include <QFileDialog>
#include <QDirModel>
#include <QStandardItem>
#include <QIcon>

#include <QDebug>

DMSCGJ::DMSCGJ(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DMSCGJ)
{
    ui->setupUi(this);

    initMenubar();

    showValue();

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
 * @brief DMSCGJ::openFile 打开工程文件，并在项目文件区显示
 */
void DMSCGJ::openFile()
{
    ui->textEdit_codeEdit->clear();

    QString currentFileName = QFileDialog::getExistingDirectory();

    QStandardItemModel* model = new QStandardItemModel(this);

    QDir dir(currentFileName);

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
        }
    }

    ui->treeView_filesView->setModel(model);

    //    方法二
    //    QDirModel *model = new QDirModel();
    //    ui->treeView_filesView->setModel(model);
    //    ui->treeView_filesView->setRootIndex(model->index(currentFileName));

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
        }
    }
}

/**
 * @brief DMSCGJ::saveFile
 */
void DMSCGJ::saveFile()  //暂时乱写
{
    QString path;
    path = QDir::currentPath() + "/CscanData/Cscan.bmp";
    QString filename = QFileDialog::getSaveFileName(this, tr("Save As"), path, tr("Image Files (*.bmp)"));
    if(!filename.isNull())
    {
        //一些处理工作，复制
        QFile::copy(path,filename);
    }
    else     return;
}

/**
 * @brief DMSCGJ::showFile  显示文件内容
 * @param index 当前点击树形节点值
 */
void DMSCGJ::showFile(const QModelIndex &index)
{
    //qDebug()<< index.data(Qt::UserRole).toString();
    QString displayString;
    QFile file(index.data(Qt::UserRole).toString());

    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug()<<"Can't open the file!"<<endl;
    }

    while(!file.atEnd())
    {
        QByteArray line = file.readLine();
        //QString str(line);
        QString str = QString::fromLocal8Bit(line);
        displayString.append(str);
    }

    ui->textEdit_codeEdit->clear();
    ui->textEdit_codeEdit->setPlainText(displayString);
}

/**
 * @brief DMSCGJ::showValue
 */
void DMSCGJ::showValue()
{
    QStandardItemModel *model = new QStandardItemModel();

    model->setColumnCount(2);
    model->setHeaderData(0,Qt::Horizontal,QString::fromLocal8Bit("key"));
    model->setHeaderData(1,Qt::Horizontal,QString::fromLocal8Bit("value"));

    ui->tableView_key_value->setModel(model);
    //列宽度自适应,填满表格区域
    ui->tableView_key_value->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

}
