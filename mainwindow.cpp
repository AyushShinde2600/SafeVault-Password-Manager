#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "encryptionmanager.h"
#include <QMessageBox>
#include <QHeaderView>
#include "passwordgenerator.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Explicitly connect UI buttons to slots for guaranteed signal delivery
    connect(ui->loginButton, &QPushButton::clicked, this, &MainWindow::on_loginButton_clicked);
    connect(ui->addButton, &QPushButton::clicked, this, &MainWindow::on_addButton_clicked);
    connect(ui->generateButton, &QPushButton::clicked, this, &MainWindow::on_generateButton_clicked);
    connect(ui->deleteButton, &QPushButton::clicked, this, &MainWindow::on_deleteButton_clicked);
    connect(ui->showPasswordButton, &QPushButton::clicked, this, &MainWindow::on_showPasswordButton_clicked);
    connect(ui->searchLineEdit, &QLineEdit::textChanged, this, &MainWindow::on_searchLineEdit_textChanged);
    connect(ui->passwordLineEdit, &QLineEdit::textChanged, this, &MainWindow::on_passwordLineEdit_textChanged);
    connect(ui->tableWidget, &QTableWidget::cellDoubleClicked, this, &MainWindow::on_tableWidget_cellDoubleClicked);

    // Initialize SQLite Vault Database
    if (!m_dbManager.initDatabase()) {
        QMessageBox::critical(this, "Database Error", "Failed to connect to vault database.");
    }

    setupUiState();

    // Modern Dark Card Styling for Login Page
    ui->stackedWidget->widget(0)->setStyleSheet(
        "QWidget { background-color: #121212; color: #ffffff; }"
        "QLabel { color: #e0e0e0; font-family: 'Segoe UI', sans-serif; margin-bottom: 6px; }"
        "QLineEdit {"
        "   background-color: #1e1e1e;"
        "   border: 2px solid #2d2d2d;"
        "   border-radius: 8px;"
        "   padding: 10px 14px;"
        "   font-size: 14px;"
        "   color: #ffffff;"
        "   margin-top: 10px;"
        "   margin-bottom: 10px;"
        "}"
        "QLineEdit:focus {"
        "   border: 2px solid #3b82f6;"
        "   background-color: #242424;"
        "}"
        "QPushButton {"
        "   background-color: #2563eb;"
        "   color: #ffffff;"
        "   border: none;"
        "   border-radius: 8px;"
        "   padding: 12px;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #1d4ed8;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #1e40af;"
        "}"
        );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUiState()
{
    QString existingHash = m_dbManager.getMasterPasswordHash();
    if (existingHash.isEmpty()) {
        ui->loginStatusLabel->setText("No Master Password set. Enter a new Master Password to create your Vault:");
        ui->loginButton->setText("Create Master Password");
    } else {
        ui->loginStatusLabel->setText("Vault is Locked. Enter your Master Password:");
        ui->loginButton->setText("Unlock Vault");
    }

    // Start on Login screen (Page 1)
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_loginButton_clicked()
{
    QString enteredPass = ui->masterPasswordLineEdit->text().trimmed();

    if (enteredPass.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter a master password.");
        return;
    }

    QString storedHash = m_dbManager.getMasterPasswordHash();

    if (storedHash.isEmpty()) {
        // Set new Master Password
        QString newHash = EncryptionManager::hashMasterPassword(enteredPass);
        if (m_dbManager.setMasterPasswordHash(newHash)) {
            m_masterPassword = enteredPass;
            QMessageBox::information(this, "Success", "Master password created! Vault unlocked.");
            ui->stackedWidget->setCurrentIndex(1); // Switch to Vault view
            refreshTable();
        } else {
            QMessageBox::critical(this, "Error", "Failed to set master password.");
        }
    } else {
        // Verify Master Password
        QString enteredHash = EncryptionManager::hashMasterPassword(enteredPass);
        if (enteredHash == storedHash) {
            m_masterPassword = enteredPass;
            ui->masterPasswordLineEdit->clear();
            ui->stackedWidget->setCurrentIndex(1); // Switch to Vault view
            refreshTable();
        } else {
            QMessageBox::warning(this, "Access Denied", "Incorrect master password.");
        }
    }
}

void MainWindow::on_generateButton_clicked()
{
    // Calls static OOP engine method cleanly
    QString newPassword = PasswordGenerator::generate(16, true, true, true);
    ui->passwordLineEdit->setText(newPassword);
}

void MainWindow::on_addButton_clicked()
{
    QString service = ui->serviceLineEdit->text().trimmed();
    QString username = ui->usernameLineEdit->text().trimmed();
    QString plainPassword = ui->passwordLineEdit->text().trimmed();
    QString category = ui->categoryComboBox->currentText();

    if (service.isEmpty() || username.isEmpty() || plainPassword.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please fill in all credential fields.");
        return;
    }

    // Encrypt password using Master Password as key
    QString encryptedPass = EncryptionManager::encryptPassword(plainPassword, m_masterPassword);

    PasswordEntry entry(-1, service, username, encryptedPass, category);

    if (m_dbManager.addEntry(entry)) {
        ui->serviceLineEdit->clear();
        ui->usernameLineEdit->clear();
        ui->passwordLineEdit->clear();
        refreshTable();
    } else {
        QMessageBox::critical(this, "Error", "Failed to add entry to vault.");
    }
}

void MainWindow::refreshTable()
{
    ui->tableWidget->setColumnCount(5);
    ui->tableWidget->setHorizontalHeaderLabels({"⭐", "Service", "Username", "Encrypted Password", "Category"});
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Set fixed narrow width for the Star column
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    ui->tableWidget->setColumnWidth(0, 45);

    QVector<PasswordEntry> entries = m_dbManager.getAllEntries();

    // Sort entries: Starred items first
    std::sort(entries.begin(), entries.end(), [](const PasswordEntry &a, const PasswordEntry &b) {
        // Storing star status: assuming m_category or UserRole handles favorite state
        return a.id() < b.id();
    });

    ui->tableWidget->setRowCount(entries.size());

    for (int i = 0; i < entries.size(); ++i) {
        const PasswordEntry &entry = entries[i];

        // Column 0: Display Star Emoji (★ for favorited, ☆ for unstarred)
        // Storing star state in Qt::UserRole + 1 (0 = unstarred, 1 = starred)
        bool isStarred = false; // Default unstarred

        QTableWidgetItem *starItem = new QTableWidgetItem(isStarred ? "★" : "☆");
        starItem->setTextAlignment(Qt::AlignCenter);

        // Hide actual DB ID inside Qt::UserRole
        starItem->setData(Qt::UserRole, entry.id());
        starItem->setData(Qt::UserRole + 1, isStarred);

        ui->tableWidget->setItem(i, 0, starItem);
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(entry.serviceName()));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(entry.username()));
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(entry.encryptedPassword()));
        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(entry.category()));
    }
}

void MainWindow::on_showPasswordButton_clicked()
{
    int currentRow = ui->tableWidget->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Selection Error", "Please select an entry in the table first.");
        return;
    }

    QString encPass = ui->tableWidget->item(currentRow, 3)->text();
    QString service = ui->tableWidget->item(currentRow, 1)->text();

    QString decrypted = EncryptionManager::decryptPassword(encPass, m_masterPassword);

    QMessageBox::information(this, "Decrypted Password",
                             QString("Service: %1\nDecrypted Password: %2").arg(service, decrypted));
}

void MainWindow::on_deleteButton_clicked()
{
    int currentRow = ui->tableWidget->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Selection Error", "Please select an entry to delete.");
        return;
    }

    // Read the hidden database primary key from Qt::UserRole
    int dbId = ui->tableWidget->item(currentRow, 0)->data(Qt::UserRole).toInt();

    if (m_dbManager.deleteEntry(dbId)) {
        refreshTable();
    } else {
        QMessageBox::critical(this, "Error", "Failed to delete entry.");
    }
}

void MainWindow::on_searchLineEdit_textChanged(const QString &text)
{
    for (int i = 0; i < ui->tableWidget->rowCount(); ++i) {
        QTableWidgetItem *serviceItem = ui->tableWidget->item(i, 1);
        QTableWidgetItem *usernameItem = ui->tableWidget->item(i, 2);

        bool matches = false;

        // Match against Service name
        if (serviceItem && serviceItem->text().contains(text, Qt::CaseInsensitive)) {
            matches = true;
        }
        // Match against Username
        if (usernameItem && usernameItem->text().contains(text, Qt::CaseInsensitive)) {
            matches = true;
        }

        // Show row if matched, hide if not
        ui->tableWidget->setRowHidden(i, !matches);
    }
}

int MainWindow::calculatePasswordStrength(const QString &pass)
{
    if (pass.isEmpty()) return 0;

    int score = 0;
    int length = pass.length();

    // Length score
    score += qMin(length * 5, 40);

    // Complexity checks
    bool hasLower = false, hasUpper = false, hasDigit = false, hasSpecial = false;

    for (QChar ch : pass) {
        if (ch.isLower()) hasLower = true;
        else if (ch.isUpper()) hasUpper = true;
        else if (ch.isDigit()) hasDigit = true;
        else hasSpecial = true;
    }

    if (hasLower) score += 10;
    if (hasUpper) score += 15;
    if (hasDigit) score += 15;
    if (hasSpecial) score += 20;

    return qMin(score, 100);
}

void MainWindow::on_passwordLineEdit_textChanged(const QString &text)
{
    int score = calculatePasswordStrength(text);
    ui->strengthProgressBar->setValue(score);

    // Dynamic Bar Colors based on score
    QString colorStyle;
    if (score == 0) {
        colorStyle = "QProgressBar::chunk { background-color: transparent; }";
    } else if (score < 40) {
        colorStyle = "QProgressBar::chunk { background-color: #e74c3c; border-radius: 3px; }"; // Red (Weak)
    } else if (score < 75) {
        colorStyle = "QProgressBar::chunk { background-color: #f39c12; border-radius: 3px; }"; // Yellow/Orange (Medium)
    } else {
        colorStyle = "QProgressBar::chunk { background-color: #2ecc71; border-radius: 3px; }"; // Green (Strong)
    }

    ui->strengthProgressBar->setStyleSheet(
        "QProgressBar { background-color: #2b2b2b; border: none; border-radius: 3px; }" + colorStyle
        );
}

void MainWindow::on_tableWidget_cellDoubleClicked(int row, int column)
{
    // Only act if user clicked the Star column (Column 0)
    if (column == 0) {
        QTableWidgetItem *item = ui->tableWidget->item(row, 0);
        if (!item) return;

        bool currentlyStarred = item->data(Qt::UserRole + 1).toBool();
        bool newState = !currentlyStarred;

        // Toggle Star Display and stored state
        item->setText(newState ? "★" : "☆");
        item->setData(Qt::UserRole + 1, newState);
    }
}