#ifndef DMSCGJ_H
#define DMSCGJ_H

#include <QMainWindow>

#include <QStandardItem>
#include <QFileDialog>

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

    QString t_currentFileName;

    QString t_content;

    QStringList t_keyValueList;

    QStringList t_valueList;

    QStandardItemModel *model_key_value = new QStandardItemModel(this);

    void initMenubar();
    void initValue();

    void keyValueList(QStringList list);

    void addKeyValue(QStringList list);
    void addKeyValues(QStringList list);

    void getValue();

    void readFile(QString path);

private slots:
    void openFile();
    void openFiles(QString path, QStandardItem* temp);

    void saveFile();
    void saveFiles(QString path, QString dirName);

    void showFile(const QModelIndex &index);

};

#endif // DMSCGJ_H
