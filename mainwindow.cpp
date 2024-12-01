#include "barchartwidget.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "barchartwidget.h"
#include "serialcommunication.h"
#include <QtDebug>
#include <QPrinter>
#include <QPainter>
#include <QMessageBox>
#include <QTextDocument>
#include <QFileDialog>
#include <QTextCursor>
#include <QProcess>
#include <QInputDialog>
#include <QFile>
#include <QFileInfo>
#include <QMainWindow>
#include <QByteArray>
#include <QSqlError>
#include <QSerialPort>
#include <QDebug>




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      serial(new QSerialPort(this))
{
    ui->setupUi(this);
    afficher(); // Load products on startup

    // Open the serial port
    serial->setPortName("COM5");  // Set the port name (adjust COM5 if needed)
    serial->setBaudRate(QSerialPort::Baud9600);  // Set baud rate
    serial->setDataBits(QSerialPort::Data8);  // Set data bits
    serial->setParity(QSerialPort::NoParity);  // Set parity
    serial->setStopBits(QSerialPort::OneStop);  // Set stop bits
    serial->setFlowControl(QSerialPort::NoFlowControl);  // Set flow control

    // Try to open the serial port
    if (serial->open(QIODevice::ReadWrite)) {
        qDebug() << "Serial port opened successfully!";
    } else {
        qDebug() << "Failed to open serial port!";
    }
}

MainWindow::~MainWindow()
{
    if (serial->isOpen()) {
            serial->close(); }
    delete ui;
    delete serial;
}

void MainWindow::on_pushButton_ajouter_clicked()
{
    // Retrieve inputs from the UI
    QString idText = ui->idLineEdit->text();
    QString nom = ui->nomLineEdit->text();
    QString prixText = ui->prixLineEdit->text();
    QString quantiteText = ui->quantiteLineEdit->text();
    QString categorie = ui->categorieLineEdit->text();
    QDate date = ui->dateEdit->date();

    // Input validation
    if (idText.isEmpty() || prixText.isEmpty() || quantiteText.isEmpty() || nom.isEmpty() || categorie.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please fill in all fields.");
        return;
    }

    bool idIsInt, prixIsInt, quantiteIsInt;
    int id = idText.toInt(&idIsInt);
    int prix = prixText.toInt(&prixIsInt);
    int quantite = quantiteText.toInt(&quantiteIsInt);

    if (!idIsInt || !prixIsInt || !quantiteIsInt) {
        QMessageBox::warning(this, "Input Error", "ID, Prix, and Quantite must be integers.");
        return;
    }

    if (!date.isValid()) {
        QMessageBox::warning(this, "Input Error", "Please provide a valid date.");
        return;
    }

    Produit newProduct(id, prix, quantite, date, nom, categorie);

    if (newProduct.ajouter()) {
        QMessageBox::information(this, "Success", "Product added successfully.");
        afficher(); // Refresh displayed products
    } else {
        QMessageBox::warning(this, "Error", "Failed to add product.");
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    // Get the text input from the user and remove leading/trailing spaces
    QString idText = ui->lineEdit->text().trimmed();

    // Debug: Show the text entered
    qDebug() << "ID Text entered: [" << idText << "]"; // The brackets show if there are spaces

    bool idIsInt = false;
    int id = idText.toInt(&idIsInt);  // Try to convert the input to an integer

    // Debug: Show if the conversion was successful
    qDebug() << "Conversion success: " << idIsInt << ", ID: " << id;

    // Empty input check: if the input is empty, show an error
    if (idText.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please provide an ID.");
        return;
    }

    // Conversion check: if the conversion fails or ID is less than or equal to 0, show an error
    if (!idIsInt || id <= 0) {
        QMessageBox::warning(this, "Input Error", "Please provide a valid numeric ID.");
        return;
    }

    // Check if the product with the given ID exists in the database
    if (!currentProduit.doesProductExist(id)) {
        QMessageBox::warning(this, "Error", "Product with the given ID does not exist.");
        return;
    }

    // Proceed with deletion if ID exists
    if (currentProduit.supprimer(id)) {
        QMessageBox::information(this, "Success", "Product deleted successfully.");
        afficher(); // Refresh displayed products
    } else {
        QMessageBox::warning(this, "Error", "Failed to delete product.");
    }
}


void MainWindow::on_updateButton_clicked()
{
    // Retrieve inputs from the UI and trim any leading/trailing spaces
    QString idText = ui->idLineEdit->text().trimmed();
    QString prixText = ui->prixLineEdit->text().trimmed();
    QString quantiteText = ui->quantiteLineEdit->text().trimmed();
    QString nom = ui->nomLineEdit->text().trimmed();
    QString categorie = ui->categorieLineEdit->text().trimmed();
    QDate date = ui->dateEdit->date();

    // Debugging: Print out the retrieved input values
    qDebug() << "ID:" << idText << "Prix:" << prixText << "Quantité:" << quantiteText;

    // Input validation
    if (idText.isEmpty() || prixText.isEmpty() || quantiteText.isEmpty() || nom.isEmpty() || categorie.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please fill in all fields.");
        return;
    }

    bool idIsInt, prixIsInt, quantiteIsInt;
    int id = idText.toInt(&idIsInt);
    int prix = prixText.toInt(&prixIsInt);
    int quantite = quantiteText.toInt(&quantiteIsInt);

    // Check if all the inputs are valid integers
    if (!idIsInt || !prixIsInt || !quantiteIsInt || id == 0 || prix == 0 || quantite == 0) {
        QMessageBox::warning(this, "Input Error", "ID, Prix, and Quantite must be valid integers greater than zero.");
        return;
    }

    // Validate the date
    if (!date.isValid()) {
        QMessageBox::warning(this, "Input Error", "Please provide a valid date.");
        return;
    }

    // Set values to the currentProduit object
    currentProduit.setIdProduit(id);
    currentProduit.setNom(nom);
    currentProduit.setPrix(prix);
    currentProduit.setQuantite(quantite);
    currentProduit.setCategorie(categorie);
    currentProduit.setDate(date);

    // Try to modify the product
    if (currentProduit.modifier(id)) {
        QMessageBox::information(this, "Success", "Product updated successfully.");
        afficher(); // Refresh the product list after update
    } else {
        QMessageBox::warning(this, "Error", "Failed to update product.");
    }
}

void MainWindow::afficher()
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);

    QSqlQueryModel* model = currentProduit.afficher();

    int rowCount = model->rowCount();
    int columnCount = model->columnCount();
    ui->tableWidget->setRowCount(rowCount);
    ui->tableWidget->setColumnCount(columnCount);

    for (int row = 0; row < rowCount; ++row) {
        for (int col = 0; col < columnCount; ++col) {
            QTableWidgetItem* item = new QTableWidgetItem(model->data(model->index(row, col)).toString());
            ui->tableWidget->setItem(row, col, item);
        }
    }

    QStringList headers = { "ID", "Nom", "Prix","Quantite" ,"Categorie","Date"  };
    ui->tableWidget->setHorizontalHeaderLabels(headers);
}

void MainWindow::on_searchButton_clicked() {
    // Get the ID entered by the user
    QString idText = ui->searchLineEdit->text();  // The ID input field
    bool idIsInt;
    int id = idText.toInt(&idIsInt);  // Convert to integer
    qDebug() << "ID Text entered: [" << idText << "]";
    qDebug() << "Conversion success: " << idIsInt << ", ID: " << id;

    // Check if the input is a valid ID
    if (!idIsInt || id <= 0) {
        QMessageBox::warning(this, "Input Error", "Please provide a valid numeric ID.");
        return;
    }

    // Call the recherche function to search for the product
    QSqlQueryModel *model = currentProduit.recherche(id);

    // If the model is null or there are no results, show an error
    if (!model || model->rowCount() == 0) {
        qDebug() << "No product found.";
        QMessageBox::warning(this, "Not Found", "No product found with the given ID.");
        return;
    }

    qDebug() << "Product found!";

    // Set the model to the QTableView
    ui->tableView->setModel(model);  // Assuming your QTableView is named 'tableView'
}

void MainWindow::on_exporterButton_clicked() {
    // Step 1: Prompt the user to select a file path
    QString filePath = QFileDialog::getSaveFileName(this, "Save as PDF", "", "PDF Files (*.pdf)");
    if (filePath.isEmpty()) {
        return;  // User canceled the dialog
    }

    // Ensure the file name has a .pdf extension
    if (!filePath.endsWith(".pdf", Qt::CaseInsensitive)) {
        filePath += ".pdf";
    }

    // Step 2: Create an HTML structure for the PDF
    QString html;
    html += "<h2 style='text-align:center;'>Products Table Export</h2>";
    html += "<table border='1' cellspacing='0' cellpadding='4' style='width:100%; text-align:center;'>";
    html += "<thead><tr>";

    // Add table headers from the QTableWidget
    for (int col = 0; col < ui->tableWidget->columnCount(); ++col) {
        QString header = ui->tableWidget->horizontalHeaderItem(col)->text();
        html += QString("<th>%1</th>").arg(header);
    }
    html += "</tr></thead>";

    // Add table rows
    html += "<tbody>";
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        html += "<tr>";
        for (int col = 0; col < ui->tableWidget->columnCount(); ++col) {
            QTableWidgetItem *item = ui->tableWidget->item(row, col);
            QString cellData = item ? item->text() : ""; // Handle null pointers gracefully
            html += QString("<td>%1</td>").arg(cellData);
        }
        html += "</tr>";
    }
    html += "</tbody></table>";

    // Step 3: Generate the PDF using QTextDocument
    QTextDocument document;
    document.setHtml(html);

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filePath);

    // Print the document to the selected file path
    document.print(&printer);

    // Step 4: Notify the user about success or failure
    QMessageBox::information(this, "Export Successful", "The table has been exported to PDF successfully!");
}

void MainWindow::on_sortByIdButton_clicked()
{
    // Assuming the ID column is the first column (index 0)
    int idColumnIndex = 0;

    // Sort items in ascending order by ID
    ui->tableWidget->sortItems(idColumnIndex, Qt::AscendingOrder);

    QMessageBox::information(this, "Sorting", "Products have been sorted by ID.");
}

void MainWindow::on_exportToCSVButton_clicked() {
    // Step 1: Prompt the user to select a file path
    QString filePath = QFileDialog::getSaveFileName(this, "Save as CSV File", "", "CSV Files (*.csv)");
    if (filePath.isEmpty()) {
        return;  // User canceled the dialog
    }

    // Ensure the file has the .csv extension
    if (!filePath.endsWith(".csv", Qt::CaseInsensitive)) {
        filePath += ".csv";
    }

    // Step 2: Open the file for writing
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "Error", "Could not open file for writing.");
        return;
    }

    QTextStream stream(&file);

    // Step 3: Write headers
    QStringList headers;
    for (int col = 0; col < ui->tableWidget->columnCount(); ++col) {
        headers << ui->tableWidget->horizontalHeaderItem(col)->text();
    }
    stream << headers.join(",") << "\n"; // Join headers with commas and write to the file

    // Step 4: Write data rows
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        QStringList rowData;
        for (int col = 0; col < ui->tableWidget->columnCount(); ++col) {
            QTableWidgetItem *item = ui->tableWidget->item(row, col);
            QString cellData = item ? item->text() : ""; // Handle null pointers
            rowData << cellData.replace(",", " "); // Escape commas in cell data
        }
        stream << rowData.join(",") << "\n"; // Join row data with commas and write to the file
    }

    file.close();

    // Notify the user
    QMessageBox::information(this, "Export Successful", "The table has been exported to CSV successfully!");
}

void MainWindow::on_pushButton_showStats_clicked() {
    // Create an instance of the Produit class
    Produit produit;

    // Get the statistics from the Produit class (this returns QMap<QString, int>)
    QMap<QString, int> stats = produit.getCategorieStats();  // Replace with the actual method you use to get stats

    // Create a BarChartWidget and pass the statistics data
    BarChartWidget *chartWidget = new BarChartWidget(stats, this);

    // Get the layout of the "frame_chart" widget (assuming you have a QFrame named frame_chart in your UI)
    QLayout *layout = ui->frame_chart->layout();
    if (!layout) {
        layout = new QVBoxLayout(ui->frame_chart);  // If no layout exists, create a new one
        ui->frame_chart->setLayout(layout);
    }

    // Clear any existing widgets from the layout (removes old charts)
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        delete child->widget();  // Remove the old widget
        delete child;
    }

    // Add the new chart widget to the layout
    layout->addWidget(chartWidget);
    chartWidget->show();

    // Switch to the "Statistics" tab
    ui->tabWidget->setCurrentWidget(ui->tab_5);  // Adjust the tab name accordingly
}

void MainWindow::on_checkStockButton_clicked() {
    // Make sure the database is open
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        QMessageBox::critical(this, "Database Error", "Database is not open.");
        return;
    }

    // Execute the query to check stock levels
    QSqlQuery query;
    query.prepare("SELECT id_produit, quantite FROM PRODUIT WHERE quantite = 0");

    if (!query.exec()) {
        QMessageBox::warning(this, "Query Error", "Failed to execute query.");
        qDebug() << query.lastError();
        return;
    }

    // Process the result set
    QString outOfStockMessage;
    bool outOfStockFound = false;
    while (query.next()) {
        int id = query.value(0).toInt();  // Get the product ID
        int quantite = query.value(1).toInt();  // Get the quantity

        // If quantite is 0, add product to out of stock message
        if (quantite == 0) {
            outOfStockMessage += QString("Product with ID %1 is out of stock.\n").arg(id);
            outOfStockFound = true;
        }
    }

    // Display the message
    if (outOfStockFound) {
        QMessageBox::information(this, "Out of Stock", outOfStockMessage);

        // Send command to Arduino to activate vibrator (e.g., "1" for ON)
        if (serial->isOpen()) {
            serial->write("1");  // "1" to activate vibrator
            qDebug() << "Sent command to Arduino to activate vibrator.";
        } else {
            qDebug() << "Serial port is not open!";
        }
    } else {
        QMessageBox::information(this, "Stock Check", "All products are in stock.");
    }
}




SerialCommunication serialComm;

void MainWindow::initSerialPort() {
    serial = new QSerialPort(this);
    serial->setPortName("COM5");  // Update this if needed
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if (!serial->open(QIODevice::WriteOnly)) {
        qDebug() << "Failed to open serial port!";
        qDebug() << "Error: " << serial->errorString();  // Additional error info
    } else {
        qDebug() << "Serial port opened successfully!";
    }
}




void MainWindow::onCheckButtonClicked() {
    serialComm.connectToArduino();  // Establish the connection to Arduino

    // Check products and activate vibrator if necessary
    serialComm.checkProductAndActivateVibrator();
}


