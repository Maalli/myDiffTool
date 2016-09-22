#ifndef DIFFDIALOG_H
#define DIFFDIALOG_H

#include <QDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QStringList>
#include <QTableWidget>
#include <QRegularExpression>
#include <QtCore>
#include <QtGui>
#include <QScrollBar>
#include <QFileDialog>
#include <QMessageBox>

namespace Ui {
class diffDialog;
}

class diffDialog : public QDialog
{
    Q_OBJECT

public:
    explicit diffDialog(QWidget *parent = 0,  QString filename1 = 0, QString filename2 = 0);
    ~diffDialog();

private slots:

    void on_connectButton_clicked();

    void on_deleteButton_clicked();

    void on_exportButton_clicked();


private:
    Ui::diffDialog *ui;
    QStandardItemModel *model1;
    QStandardItemModel *model2;
    QStandardItemModel *modelListRules;

    void readFilesToLineList(QTextStream &in, QList<QStringList> &list);
    void writeTxtDataToModule(QStandardItemModel *module, QList<QStringList> &list);
    int getColumnByRegExp(QList<QStringList> &list, QRegExp  &re);
    void setAndConfigureView(QTableView *tabView, QStandardItemModel *module);
    void createConfigureAndSetRuleView(QTableView *tabView);

    QString fileNameOne;
    QString fileNameTwo;

    int shaColumn, fileNameColumn, lineColumn = -1;
};

#endif // DIFFDIALOG_H
