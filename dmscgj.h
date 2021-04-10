#ifndef DMSCGJ_H
#define DMSCGJ_H

#include <QMainWindow>

#include <QStandardItem>

namespace Ui {
class DMSCGJ;
}

class DMSCGJ : public QMainWindow
{
    Q_OBJECT

public:
    explicit DMSCGJ(QWidget *parent = 0);
    ~DMSCGJ();

private:
    Ui::DMSCGJ *ui;

    QString t_currentDirName;       //当前打开文件夹目录
    QString t_content;              //当前打开文件内容

    QStringList t_keyList;          //当前key值表
    QStringList t_valueList;        //当前value值表

    QStandardItemModel *model_key_value = new QStandardItemModel(this);   //当前显示key_value表

    void initMenubar();
    void initValue();

    void correctKeyList(QStringList list);

    void addKeyValue(QStringList list);       //用于显示当前打开文件的key值，未使用
    void addKeyValues(QStringList list);

    void setValueList();

    void readFile(QString path);

private slots:
    void openFile();
    void openFiles(QString path, QStandardItem* temp);

    void saveFile();
    void saveFiles(QString path, QString dirName);

    void showFile(const QModelIndex &index);

};

#endif // DMSCGJ_H
