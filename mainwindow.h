#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include "databasemanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_loginButton_clicked();
    void on_addButton_clicked();
    void on_generateButton_clicked();
    void on_deleteButton_clicked();
    void on_showPasswordButton_clicked();
    void on_searchLineEdit_textChanged(const QString &text);
    void on_passwordLineEdit_textChanged(const QString &text);
    void on_tableWidget_cellDoubleClicked(int row, int column);

private:
    Ui::MainWindow *ui;
    DatabaseManager m_dbManager;
    QString m_masterPassword;

    void refreshTable();
    void setupUiState();
    int calculatePasswordStrength(const QString &pass);
};

#endif // MAINWINDOW_H