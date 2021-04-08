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

    void initMenubar();

private slots:
    void openFile();
    void openFiles(QString path, QStandardItem* temp);

    void saveFile();

    void showFile(const QModelIndex &index);

    void showValue();

};

#endif // DMSCGJ_H
