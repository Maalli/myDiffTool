#ifndef DIFFDIALOG_H
#define DIFFDIALOG_H

#include <QDialog>

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

private:
    Ui::diffDialog *ui;

    QString fileNameOne;
    QString fileNameTwo;
};

#endif // DIFFDIALOG_H
