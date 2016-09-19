#include "diffdialog.h"
#include "ui_diffdialog.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QStringList>


diffDialog::diffDialog(QWidget *parent, QString filename1, QString filename2) :
    QDialog(parent),
    ui(new Ui::diffDialog),
    fileNameOne(filename1),
    fileNameTwo(filename2)
{
    ui->setupUi(this);
    /* Debug
    qDebug() << "fileNameOne:" << fileNameOne << ", filename1:" << filename1;
    qDebug() << "fileNameTwo:" << fileNameTwo << ", filename2:" << filename2;
    */
    QFile QFileOne(fileNameOne);
    QFile QFileTwo(fileNameTwo);

    if (!QFileOne.open(QIODevice::ReadOnly))
    {
        //ui->textBrowser->setText(QFileOne.errorString());
    }
    else if (!QFileTwo.open(QIODevice::ReadOnly))
    {
        //ui->textBrowser_2->setText(QFileOne.errorString());
    }
    else
    {
        // Declaration of variables
        QTextStream in1(&QFileOne), in2(&QFileTwo);
        QString line1, line2;
        QList<QStringList> list1, list2;


        // Reading files line by line
        // TODO move to external function
        do
        {
            line1 = in1.readLine();
            list1 << line1.split(":");


//            qDebug() << list1;
        }while(!line1.isNull());

        do
        {
            line2 = in2.readLine();
            list2 << line2.split(":");


//            qDebug() << list2;
        }while(!line2.isNull());


        // Printing Table
        // TODO move to external function
        ui->tableWidget->setRowCount( list1.size() );
        ui->tableWidget->setColumnCount( list1[0].size() );

        for ( int row = 0; row < list1.size(); ++row ) {
            for ( int column = 0; column < list1[row].size(); ++column ) {
                ui->tableWidget->setItem(row, column, new QTableWidgetItem(list1[row][column]));
            }
        }

        ui->tableWidget_2->setRowCount( list2.size() );
        ui->tableWidget_2->setColumnCount( list2[0].size() );

        for ( int row = 0; row < list2.size(); ++row ) {
            for ( int column = 0; column < list2[row].size(); ++column ) {
                ui->tableWidget_2->setItem(row, column, new QTableWidgetItem(list2[row][column]));
            }
        }




        //ui->textBrowser->setText(in1.readAll());
        //ui->textBrowser_2->setText(in2.readAll());
    }
}

diffDialog::~diffDialog()
{
    delete ui;
}
