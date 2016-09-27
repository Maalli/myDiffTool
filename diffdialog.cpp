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

    ui->tableView->setShowGrid(false);
    ui->tableView_2->setShowGrid(false);
    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView_2->verticalHeader()->setVisible(false);

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


        // Populating module sorted on names
        writeTxtDataToModel(model1, list1);
        writeTxtDataToModel(model2, list2);

        populateColumnIdentifyersByReExp();

        // Sync rows between models
        syncFileNamesToScroll(model1, model2);

        // Handle the automapping suggestions
        autoMapSuggestions();

        // Set and config input files to views
        setAndConfigureView(ui->tableView, model1);
        setAndConfigureView(ui->tableView_2, model2);

        // Connect scroll bars !WORKS BUT NOT SO GOOD!
        QObject::connect(ui->tableView_2->verticalScrollBar(), SIGNAL(valueChanged(int)), ui->tableView->verticalScrollBar(), SLOT(setValue(int)));
        QObject::connect(ui->tableView->verticalScrollBar(), SIGNAL(valueChanged(int)), ui->tableView_2->verticalScrollBar(), SLOT(setValue(int)));

    }
}



diffDialog::~diffDialog()
{
    delete ui;
}


// --------------------------------------------------------------------------------------
// Privat slot
// --------------------------------------------------------------------------------------


void diffDialog::on_connectButton_clicked()
{
    QModelIndexList indexListOne = ui->tableView->selectionModel()->selectedIndexes();
    QModelIndexList indexListTwo = ui->tableView_2->selectionModel()->selectedIndexes();
    if (( indexListOne.isEmpty() && indexListTwo.isEmpty() ) ||( indexListOne.size() > 1 || indexListTwo.size() > 1 )) {
        QMessageBox::information(this, tr("Warning!"), tr("Please select ONE index to connect."));
        return;
    }

    QList<QString> row1, row2;

    // Multiple rows can be selected
    foreach (QModelIndex index, indexListOne) {
        row1.append(QString::number(index.row()));
    }

    // Multiple rows can be selected
    foreach (QModelIndex index, indexListTwo) {
        row2.append(QString::number(index.row()));
    }


    // Appending another row to rules list
    QList<QStandardItem*> newRow;
    QStandardItem *itm = new QStandardItem(1);
    modelListRules->appendRow(itm);

    // Populating the new row with information
    int insertRow = modelListRules->rowCount();
    QModelIndex index;

    if (row1.isEmpty()){
        // Filename
        index = modelListRules->index(insertRow-1, 0, QModelIndex());
        modelListRules->setData(index, model2->data( model2->index(row2.first().toInt(), fileNameColumn, QModelIndex()) ) );

        //SHA
        index = modelListRules->index(insertRow-1, 1, QModelIndex());
        modelListRules->setData(index, "" );

        // First file error msg
        index = modelListRules->index(insertRow-1, 2, QModelIndex());
        modelListRules->setData(index, "" );

        // Row
        index = modelListRules->index(insertRow-1, 3, QModelIndex());
        modelListRules->setData(index, "" );
    } else {
        // Filename
        index = modelListRules->index(insertRow-1, 0, QModelIndex());
        modelListRules->setData(index, model1->data( model1->index(row1.first().toInt(), fileNameColumn, QModelIndex()) ) );

        //SHA
        index = modelListRules->index(insertRow-1, 1, QModelIndex());
        modelListRules->setData(index, model1->data( model1->index(row1.first().toInt(), shaColumn, QModelIndex()) ) );

        // First file error msg
        index = modelListRules->index(insertRow-1, 2, QModelIndex());
        modelListRules->setData(index, model1->data( model1->index(row1.first().toInt(), 3, QModelIndex()) ) );

        // Row
        index = modelListRules->index(insertRow-1, 3, QModelIndex());
        modelListRules->setData(index, model1->data( model1->index(row1.first().toInt(), lineColumn, QModelIndex()) ) );
    }

    if (row2.isEmpty()){
        // SHA
        index = modelListRules->index(insertRow-1, 4, QModelIndex());
        modelListRules->setData(index, "" );

        // Second file error msg
        index = modelListRules->index(insertRow-1, 5, QModelIndex());
        modelListRules->setData(index, "" );

        // Row
        index = modelListRules->index(insertRow-1, 6, QModelIndex());
        modelListRules->setData(index, "" );
    } else {
        // SHA
        index = modelListRules->index(insertRow-1, 4, QModelIndex());
        modelListRules->setData(index, model2->data( model2->index(row2.first().toInt(), shaColumn, QModelIndex()) ) );

        // Second file error msg
        index = modelListRules->index(insertRow-1, 5, QModelIndex());
        modelListRules->setData(index, model2->data( model2->index(row2.first().toInt(), 3, QModelIndex()) ) );

        // Row
        index = modelListRules->index(insertRow-1, 6, QModelIndex());
        modelListRules->setData(index, model2->data( model2->index(row2.first().toInt(), lineColumn, QModelIndex()) ) );
    }

    // Comment
    index = modelListRules->index(insertRow-1, 7, QModelIndex());
    modelListRules->setData(index, ui->lineEdit->text());

    // TP/FP
    index = modelListRules->index(insertRow-1, 8, QModelIndex());
    modelListRules->setData(index, ui->radioTPButton->isChecked() ? "TP" : "FP");

    // Clear input
    ui->tableView->selectionModel()->clear();
    ui->tableView_2->selectionModel()->clear();
    ui->lineEdit->clear();


    // Debug
//    qDebug() << "Row1:" << row1 << "Row2:" << row2;

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

        for ( int row = 0; row < r; ++row )
        {
            for ( int column = 0; column < c; ++column )
            {
                QModelIndex index = modelListRules->index(row, column, QModelIndex());
                out << QString(modelListRules->data(index).toString());
                if (column+1 < c){
                    out << QString("\t");
                } else {
                    out << QString("\n");
                }
            }
        }
        file.flush();
        file.close();
    }
}


void diffDialog::on_tableView_doubleClicked(const QModelIndex &index)
{
    QString fileToOpen = model1->data( model1->index(index.row(), fileNameColumn)).toString();
    QString lineToOpen = model1->data( model1->index(index.row(), lineColumn)).toString();


    if ( fileToOpen.isEmpty() )
    {
        QMessageBox::information(this, tr("Warning"), tr("No filename selected!"));
        return;
    }

    QProcess *process = new QProcess(this);
    process->setEnvironment(QProcess::systemEnvironment());
    QString compFilePath = QDir::homePath() + fileToOpen;

    // Select program to open.
    QString programToStart = QFileDialog::getOpenFileName(this, tr("Select program"), "/home", tr("Executable file (*.exe)"));
    programToStart.replace("/","\\");
    QStringList argList; argList << compFilePath << QString("-n" + lineToOpen);

//    qDebug() << "Executable to open:" << programToStart << "options: " << argList;

    if (process->startDetached(programToStart, argList)){
        process->waitForStarted(5000); // wait max 5sec
        qDebug() << "Error code:" << process->errorString();
    }
}

void diffDialog::on_tableView_2_doubleClicked(const QModelIndex &index)
{
    QString fileToOpen = model2->data( model2->index(index.row(), fileNameColumn)).toString();
    QString lineToOpen = model2->data( model2->index(index.row(), lineColumn)).toString();


    if ( fileToOpen.isEmpty() )
    {
        QMessageBox::information(this, tr("Warning"), tr("No filename selected!"));
        return;
    }

    QProcess *process = new QProcess(this);
    process->setEnvironment(QProcess::systemEnvironment());
    QString compFilePath = QDir::homePath() + fileToOpen;

    // Select program to open.
    QString programToStart = QFileDialog::getOpenFileName(this, tr("Select program"), "/home", tr("Executable file (*.exe)"));
    programToStart.replace("/","\\");
    QStringList argList; argList << compFilePath << QString("-n" + lineToOpen);

//    qDebug() << "Executable to open:" << programToStart << "options: " << argList;

    if (process->startDetached(programToStart, argList)){
        process->waitForStarted(5000); // wait max 5sec
        qDebug() << "Error code:" << process->errorString();
    }
}

// --------------------------------------------------------------------------------------
// Privat
// --------------------------------------------------------------------------------------

void diffDialog::readFilesToLineList(QTextStream &in, QList<QStringList> &list)
{
    QString line;
    do
    {
        line = in.readLine();
        list << line.split(":");
    }while(!line.isNull());
}

void diffDialog::writeTxtDataToModel(QStandardItemModel *model, QList<QStringList> &list)
{
    for ( int row = 0; row < list.size(); ++row )
    {
        for ( int column = 0; column < list[row].size(); ++column )
        {
            QModelIndex index = model->index(row, column, QModelIndex());
            model->setData(index, list[row][column]);
        }
    }
    model->sort(1);

}


int diffDialog::getColumnByRegExp(QStandardItemModel *model, QRegExp &re)
{
    for ( int column = 0; column < model->columnCount(); ++column )
    {
//        qDebug() << "list[0][column]:" << model->data( model->index(0, column, QModelIndex()) ).toString();
        if ( re.indexIn( model->data( model->index(0, column, QModelIndex()) ).toString() ) != -1 )
        {
//            qDebug() << "Found one! Column:" << column;
            return column;
        }
    }
    return -1;
}

void diffDialog::setAndConfigureView(QTableView *tabView, QStandardItemModel *model)
{
    tabView->setModel(model);
    tabView->resizeColumnsToContents();

    for ( int column = 0; column < model->columnCount(); ++column )
    {
        //qDebug() << "Column width:" << tabView->columnWidth(column);
        //qDebug() << "Column:" << column;
        QRegExp shaRe("\\b[0-9a-f]{40}\\b");
        if ( tabView->columnWidth(column) >= 100)
        {
            if ( shaRe.indexIn( model->data( model->index(0, column, QModelIndex()) ).toString() ) != -1 ){
                tabView->setColumnWidth(column, 80);
            }else{
                tabView->setColumnWidth(column, 350);
            }

        }
    }
}


void diffDialog::createConfigureAndSetRuleView(QTableView *tabView)
{
    ui->lineEdit->setPlaceholderText("Add comments...");

    modelListRules = new QStandardItemModel(0, 9, this);
    tabView->setModel(modelListRules);
    tabView->resizeColumnsToContents();
    tabView->setColumnWidth(0, 3*(this->width()/9));
    tabView->setColumnWidth(1, 1*(this->width()/9));
    tabView->setColumnWidth(2, 4*(this->width()/9));
    tabView->setColumnWidth(3, 0.5*(this->width()/9));
    tabView->setColumnWidth(4, 1*(this->width()/9));
    tabView->setColumnWidth(5, 4*(this->width()/9));
    tabView->setColumnWidth(6, 0.5*(this->width()/9));
    tabView->setColumnWidth(7, 3*(this->width()/9));
    tabView->setColumnWidth(8, 0.5*(this->width()/9));

//    tabView->horizontalHeader()->setStretchLastSection(true);

    modelListRules->setHeaderData(0, Qt::Horizontal, "File name");
    modelListRules->setHeaderData(1, Qt::Horizontal, "SHA 1");
    modelListRules->setHeaderData(2, Qt::Horizontal, fileNameOne);
    modelListRules->setHeaderData(3, Qt::Horizontal, "Row");

    modelListRules->setHeaderData(4, Qt::Horizontal, "SHA 2");
    modelListRules->setHeaderData(5, Qt::Horizontal, fileNameTwo);
    modelListRules->setHeaderData(6, Qt::Horizontal, "Row");
    modelListRules->setHeaderData(7, Qt::Horizontal, "Comments");
    modelListRules->setHeaderData(8, Qt::Horizontal, "TP/FP");

}


void diffDialog::autoMapSuggestions()
{
    if (shaColumn == -1 || fileNameColumn == -1 || lineColumn == -1)
    {
        QMessageBox::information(this, tr("Error"), tr("Files does not contain SHA, path or line identifiers. Please specify specify files with the right format."));
    }
    else
    {
//            qDebug() << "Found all requiered columns!";
//            qDebug() << "shaColumn:" << shaColumn << " fileNameColumn:" << fileNameColumn << " lineColumn:" << lineColumn;
    }

    for (int rowmodel1 = 0; rowmodel1 < model1->rowCount(); rowmodel1++)
    {
        for (int rowmodel2 = 0; rowmodel2 < model2->rowCount(); rowmodel2++)
        {
            if (model1->data( model1->index(rowmodel1, fileNameColumn, QModelIndex()) ).toString().isEmpty() ||
                model2->data( model2->index(rowmodel2, fileNameColumn, QModelIndex()) ).toString().isEmpty()){
                // Do nothing the row is empty
            } else {
                if ( model1->data( model1->index(rowmodel1, shaColumn, QModelIndex()) ).toString() ==
                     model2->data( model2->index(rowmodel2, shaColumn, QModelIndex()) ).toString() &&
                     model1->data( model1->index(rowmodel1, fileNameColumn, QModelIndex()) ).toString() ==
                     model2->data( model2->index(rowmodel2, fileNameColumn, QModelIndex()) ).toString() &&
                    ( model1->data( model1->index(rowmodel1, lineColumn, QModelIndex()) ).toInt() >
                      model2->data( model2->index(rowmodel2, lineColumn, QModelIndex()) ).toInt() - 100 &&
                      model1->data( model1->index(rowmodel1, lineColumn, QModelIndex()) ).toInt() <
                      model2->data( model2->index(rowmodel2, lineColumn, QModelIndex()) ).toInt() + 100)
                      )
                {
                    // Debuging mapping suggestions
                    /*
                    qDebug() << "Found one!";
                    qDebug() << "rowmodel1: " << rowmodel1 << " rowmodel2: " << rowmodel2;
                    qDebug() << "SHA 1: " << model1->data( model1->index(rowmodel1, shaColumn, QModelIndex()) ).toString();
                    qDebug() << "SHA 2: " << model2->data( model2->index(rowmodel2, shaColumn, QModelIndex()) ).toString();
                    qDebug() << "Filename 1:" << model1->data( model1->index(rowmodel1, fileNameColumn, QModelIndex()) ).toString();
                    qDebug() << "Filename 2:" << model2->data( model2->index(rowmodel2, fileNameColumn, QModelIndex()) ).toString();
                    qDebug() << "row 1: " << model1->data( model1->index(rowmodel1, lineColumn, QModelIndex()) ).toInt();
                    qDebug() << "row 2: " << model2->data( model2->index(rowmodel2, lineColumn, QModelIndex()) ).toInt();
                    */

                    if (autoMap.contains(rowmodel1)){
                        //TODO: Check if we have a closer match
                    } else {
                        autoMap[rowmodel1] = rowmodel2;

                    }
                }
            }
        }
    }
    int i = 0, myRGB = 150;
    foreach (int k, autoMap.keys()) {
//        qDebug() << "Key:" << k << "Value:" << autoMap[k];
        if (i%2 == 0)
            myRGB = 230;
        else
            myRGB = 160;
        model1->setData( model1->index(k, fileNameColumn, QModelIndex()), QVariant(QBrush(qRgb(myRGB,myRGB,myRGB))), Qt::BackgroundRole );
        model2->setData( model2->index(autoMap[k], fileNameColumn, QModelIndex()), QVariant(QBrush(qRgb(myRGB,myRGB,myRGB))), Qt::BackgroundRole );
        i++;
    }
}



void diffDialog::syncFileNamesToScroll(QStandardItemModel *inModel1, QStandardItemModel *inModel2)
{

    if (shaColumn == -1 || fileNameColumn == -1 || lineColumn == -1)
    {
        QMessageBox::information(this, tr("Error"), tr("Files does not contain SHA, path or line identifiers. Please specify files with the right format."));
        return;
    }
    else
    {
//        qDebug() << "Found all requiered columns!";
//        qDebug() << "shaColumn:" << shaColumn << " fileNameColumn:" << fileNameColumn << " lineColumn:" << lineColumn;
    }

    QPair<int, int> pairM1(0,0), pairM2(0,0);
    while (pairM1.first <= inModel1->rowCount())
    {
        // Debugg
//        qDebug() << "row count:" << inModel1->rowCount();
//        qDebug() << "Data m1:" << inModel1->data( inModel1->index(pairM1.first, fileNameColumn, QModelIndex()) ).toString();
//        qDebug() << "Data m2:" << inModel2->data( inModel2->index(pairM1.first, fileNameColumn, QModelIndex()) ).toString();


       int tempEqual = QString::compare(inModel1->data( inModel1->index(pairM1.first, fileNameColumn, QModelIndex()) ).toString(),
                                        inModel2->data( inModel2->index(pairM2.first, fileNameColumn, QModelIndex()) ).toString(),
                                        Qt::CaseInsensitive);

       QString firstFileName = (tempEqual <= 0) ? inModel1->data( inModel1->index(pairM1.first, fileNameColumn, QModelIndex()) ).toString() :
                                                  inModel2->data( inModel2->index(pairM2.first, fileNameColumn, QModelIndex()) ).toString();


        while(firstFileName == inModel1->data( inModel1->index(pairM1.second, fileNameColumn, QModelIndex()) ).toString())
        {
            pairM1.second++;
            if (pairM1.second > inModel1->columnCount())
                break;
        }

        while(firstFileName == inModel2->data( inModel2->index(pairM2.second, fileNameColumn, QModelIndex()) ).toString())
        {
            pairM2.second++;
            if (pairM2.second > inModel2->columnCount())
                break;
        }

        // Debugg
 //       qDebug() << "tempEqual:" << tempEqual;
 //       qDebug() << "firstFileName:" << firstFileName;
//        qDebug() << "pair m1:" << pairM1 << " pair m2:" << pairM2;

        // Appending rows to sync scroll
        if (pairM1.second - pairM1.first > pairM2.second - pairM2.first)
        {
            inModel2->insertRows(pairM2.second, (pairM1.second - pairM1.first) - (pairM2.second - pairM2.first));
            pairM1.first = pairM1.second;
            pairM1.second = pairM1.second;
            pairM2.first = pairM1.second;
            pairM2.second = pairM1.second;
        } else if (pairM1.second - pairM1.first < pairM2.second - pairM2.first)
        {
            if (pairM1.second == pairM1.first)
                inModel1->insertRows(pairM1.first, (pairM2.second - pairM2.first) - (pairM1.second - pairM1.first));
            else
                inModel1->insertRows(pairM1.second, (pairM2.second - pairM2.first) - (pairM1.second - pairM1.first));
            pairM1.first = pairM2.second;
            pairM1.second = pairM2.second;
            pairM2.first = pairM2.second;
            pairM2.second = pairM2.second;
        } else
        {
            pairM1.first = pairM2.second;
            pairM1.second = pairM2.second;
            pairM2.first = pairM2.second;
            pairM2.second = pairM2.second;
        }
    }
}


void diffDialog::populateColumnIdentifyersByReExp()
{
    // Find column for SHA, filename and line
    // Assuming files have same format
    QRegExp shaRe("\\b[0-9a-f]{40}\\b");
    QRegExp fileRe("^(\\/[^\\/ ]*)+\\/?$");
    QRegExp lineRe("^[0-9]+$");
    shaColumn = getColumnByRegExp(model1, shaRe);
    fileNameColumn = getColumnByRegExp(model1, fileRe);
    lineColumn = getColumnByRegExp(model1, lineRe);
}


