#include "diffdialog.h"
#include "ui_diffdialog.h"
#include <QDebug>


diffDialog::diffDialog(QWidget *parent, QString filename1, QString filename2) :
    QDialog(parent),
    ui(new Ui::diffDialog),
    fileNameOne(filename1),
    fileNameTwo(filename2)
{
    ui->setupUi(this);
    //this->showMaximized();
    //qDebug() << "Window width:" << this->width();

    // PREP WINDOW ------------------------------------------------------------------------------------

    // Tool tips
    ui->closeButton->setToolTip(tr("Close the comparison window."));
    ui->connectButton->setToolTip(tr("Connects two selected lines and adds it in the rule page"));
    ui->deleteButton->setToolTip(tr("Removes the selected rule."));

    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView_3->setEditTriggers(QAbstractItemView::NoEditTriggers);

    createConfigureAndSetRuleView(ui->tableView_3);

    // ------------------------------------------------------------------------------------------------


    /* Debug retreavel of filename from main window
    qDebug() << "fileNameOne:" << fileNameOne << ", filename1:" << filename1;
    qDebug() << "fileNameTwo:" << fileNameTwo << ", filename2:" << filename2;
    */
    QFile QFileOne(fileNameOne);
    QFile QFileTwo(fileNameTwo);


    if (!QFileOne.open(QIODevice::ReadOnly))
    {
        // TODO - Handle error
        //ui->textBrowser->setText(QFileOne.errorString()); // testBrowser removed, thus do not work any more
    }
    else if (!QFileTwo.open(QIODevice::ReadOnly))
    {
        // TODO - Handle error
        //ui->textBrowser_2->setText(QFileOne.errorString()); // testBrowser removed, thus do not work any more
    }
    else
    {
        // Declaration of variables
        QTextStream in1(&QFileOne), in2(&QFileTwo);
        QList<QStringList> list1, list2;

        // Reading files line by line
        readFilesToLineList(in1, list1);
        readFilesToLineList(in2, list2);

        // Create models
        model1 = new QStandardItemModel( list1.size(), list1[0].size(), this);
        model2 = new QStandardItemModel( list2.size(), list2[0].size(), this);


        // Populating module
        writeTxtDataToModule(model1, list1);
        writeTxtDataToModule(model2, list2);

        // Find column for SHA id
        QRegExp shaRe("\\b[0-9a-f]{40}\\b");
        QRegExp fileRe("^(\\/[^\\/ ]*)+\\/?$");
        QRegExp lineRe("^[0-9]+$");
        shaColumn = getColumnByRegExp(list1, shaRe);
        fileNameColumn = getColumnByRegExp(list1, fileRe);
        lineColumn = getColumnByRegExp(list1, lineRe);
        if (shaColumn == -1)
        {
            QMessageBox::information(this, tr("Error"), tr("Files does not contain SHA, path or line identifiers. Please specify specify files with the right format."));
        }
        else
        {
//            qDebug() << "Found all requiered columns!";
//            qDebug() << "shaColumn:" << shaColumn << " fileNameColumn:" << fileNameColumn << " lineColumn:" << lineColumn;
        }



        // Set and config input files to views
        setAndConfigureView(ui->tableView, model1);
        setAndConfigureView(ui->tableView_2, model2);

        // Connect scroll bars !WORKS BUT NOT SO GOOD!
//        QObject::connect(ui->tableView_2->verticalScrollBar(), SIGNAL(valueChanged(int)), ui->tableView->verticalScrollBar(), SLOT(setValue(int)));
//        QObject::connect(ui->tableView_2->verticalScrollBar(), SLOT(valueChanged(int)), ui->tableView->verticalScrollBar(), SIGNAL(setValue(int)));

    }
}





diffDialog::~diffDialog()
{
    delete ui;
}


void diffDialog::readFilesToLineList(QTextStream &in, QList<QStringList> &list)
{
    QString line;
    do
    {
        line = in.readLine();
        list << line.split(":");
    }while(!line.isNull());
}

void diffDialog::writeTxtDataToModule(QStandardItemModel *module, QList<QStringList> &list)
{
    for ( int row = 0; row < list.size(); ++row )
    {
        for ( int column = 0; column < list[row].size(); ++column )
        {
            QModelIndex index = module->index(row, column, QModelIndex());
            module->setData(index, list[row][column]);
        }
    }
    module->sort(1);
}


int diffDialog::getColumnByRegExp(QList<QStringList> &list, QRegExp &re)
{
    for ( int column = 0; column < list[0].size(); ++column )
    {
//        qDebug() << "list[0][column]:" << list[0][column];
        if ( re.indexIn( list[0][column] ) != -1 )
        {
//            qDebug() << "Found one! Column:" << column;
            return column;
        }
    }
    return -1;
}

void diffDialog::setAndConfigureView(QTableView *tabView, QStandardItemModel *module)
{
    tabView->setModel(module);
    tabView->resizeColumnsToContents();

    for ( int column = 0; column < module->columnCount(); ++column )
    {
        //qDebug() << "Column width:" << tabView->columnWidth(column);
        //qDebug() << "Column:" << column;
        QRegExp shaRe("\\b[0-9a-f]{40}\\b");
        if ( tabView->columnWidth(column) >= 100)
        {
            if ( shaRe.indexIn( module->data( module->index(0, column, QModelIndex()) ).toString() ) != -1 ){
                tabView->setColumnWidth(column, 80);
            }else{
                tabView->setColumnWidth(column, 350);
            }

        }
    }
}

void diffDialog::on_connectButton_clicked()
{
    QModelIndexList indexList = ui->tableView->selectionModel()->selectedIndexes();
    QList<QString> row1, row2;

    // Multiple rows can be selected
    foreach (QModelIndex index, indexList) {
        row1.append(QString::number(index.row()));
    }

    indexList = ui->tableView_2->selectionModel()->selectedIndexes();
    // Multiple rows can be selected
    foreach (QModelIndex index, indexList) {
        row2.append(QString::number(index.row()));
    }

    // Appending another column to rules list
    QList<QStandardItem*> newRow;
    QStandardItem *itm = new QStandardItem(1);
    modelListRules->appendRow(itm);

    // Appending another column and set values to rules list
    int insertRow = modelListRules->rowCount();
    // First file error msg
    QModelIndex index = modelListRules->index(insertRow-1, 0, QModelIndex());
    modelListRules->setData(index, model1->data( model1->index(row1.first().toInt(), 3, QModelIndex()) ) );
    // Second file error msg
    index = modelListRules->index(insertRow-1, 1, QModelIndex());
    modelListRules->setData(index, model2->data( model2->index(row2.first().toInt(), 3, QModelIndex()) ) );
    // Comment
    index = modelListRules->index(insertRow-1, 2, QModelIndex());
    modelListRules->setData(index, ui->lineEdit->text());
    // TP/FP
    index = modelListRules->index(insertRow-1, 3, QModelIndex());
    modelListRules->setData(index, ui->radioTPButton->isChecked() ? "TP" : "FP");

    // Clear input
    ui->tableView->selectionModel()->clear();
    ui->tableView_2->selectionModel()->clear();
    ui->lineEdit->clear();


    // Debug
//    qDebug() << "Row1:" << row1 << "Row2:" << row2;

}

void diffDialog::createConfigureAndSetRuleView(QTableView *tabView)
{
    ui->lineEdit->setPlaceholderText("Add comments...");

    modelListRules = new QStandardItemModel(0, 4, this);
    tabView->setModel(modelListRules);
    tabView->resizeColumnsToContents();
    tabView->setColumnWidth(0, 2*(this->width()/7));    //2*(this->width()/7) or 500
    tabView->setColumnWidth(1, 2*(this->width()/7));    //2*(this->width()/7) or 500
    tabView->setColumnWidth(2, 2*(this->width()/7));    //2*(this->width()/7) or 500
    tabView->setColumnWidth(3, (this->width()/7));     //(this->width()/7) or 80

//    tabView->horizontalHeader()->setStretchLastSection(true);

    modelListRules->setHeaderData(0, Qt::Horizontal, fileNameOne);
    modelListRules->setHeaderData(1, Qt::Horizontal, fileNameTwo);
    modelListRules->setHeaderData(2, Qt::Horizontal, "Comments");
    modelListRules->setHeaderData(3, Qt::Horizontal, "TP/FP");

}

void diffDialog::on_deleteButton_clicked()
{
    QModelIndex currentIndex = ui->tableView_3->selectionModel()->currentIndex();
    ui->tableView_3->selectionModel()->clearCurrentIndex();
    modelListRules->removeRow(currentIndex.row());
}

void diffDialog::on_exportButton_clicked()
{

    int r = modelListRules->rowCount();
    int c = modelListRules->columnCount();
    if (r == 0){
        QMessageBox::information(this, tr("Error!"), tr("Nothing to save! Pleas create a new rule."));
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save connections to file."), "", tr(" (*.txt);;All Files (*)"));

    if (fileName.isEmpty()){
        return;
    } else {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::information(this, tr("Unable to open file"),
            file.errorString());
            return;
        }
        QTextStream out(&file);
//        out.setVersion(QDataStream::Qt_5_7);

        for ( int row = 0; row < r; ++row )
        {
            for ( int column = 0; column < c; ++column )
            {
                QModelIndex index = modelListRules->index(row, column, QModelIndex());
                out << QString(modelListRules->data(index).toString());
                if (column+1 < c){
                    out << QString(" : ");
                } else {
                    out << QString("\n");
                }
            }
        }
        file.flush();
        file.close();
    }
}


