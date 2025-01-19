#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QComboBox>
#include <QTextEdit>
#include <QLineEdit>
#include "worker.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void addFiles();
    void removeSelected();
    void startEncoding();
    void logMessage(const QString &message);

private:
    QListWidget *listView;
    QPushButton *addButton, *removeButton, *encodeButton;
    QRadioButton *fpsRadio, *fpiRadio;
    QSpinBox *spinBox;
    QComboBox *codecComboBox;
    QTextEdit *debugLog;
    QLineEdit *resEdit;

    Worker *worker;
};

#endif 