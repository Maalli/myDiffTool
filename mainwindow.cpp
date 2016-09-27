#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "diffdialog.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lineEdit_file1->setText("C:/Users/martinl/Desktop/160920/clang1.txt");
    ui->lineEdit_file2->setText("C:/Users/martinl/Desktop/160920/lint1.txt");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_file1_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open Text file"), "/home", tr("Text Files (*.txt)"));
    ui->lineEdit_file1->setText(filename);
}

void MainWindow::on_pushButton__file2_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open Text file"), "/home", tr("Text Files (*.txt)"));
    ui->lineEdit_file2->setText(filename);
}

void MainWindow::on_actionClear_all_triggered()
{
    foreach(QLineEdit* le, findChildren<QLineEdit*>()) {
        le->clear();
    }
}

void MainWindow::on_pushButton_clicked()
{
    foreach(QLineEdit* le, findChildren<QLineEdit*>()) {
        if (le->text().isEmpty())
        {
            QMessageBox::information(this, tr("Not valid"), tr("Please fill in two file paths."));
            return;
        }
    }

    diffDialog diffD(this, ui->lineEdit_file1->text(), ui->lineEdit_file2->text());
    diffD.setWindowState(diffD.windowState() | Qt::WindowMaximized);
    diffD.setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint);
    diffD.setWindowFlags(windowFlags() | Qt::WindowMaximizeButtonHint);
    diffD.setModal(true);
    diffD.exec();

}
