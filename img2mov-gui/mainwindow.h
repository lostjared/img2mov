#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QComboBox;
class QCheckBox;
class QLabel;
class QListWidget;
class QProgressBar;
class QPushButton;
class QRadioButton;
class QSpinBox;
class QTextEdit;
class Worker;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void addFiles();
    void removeSelected();
    void clearFiles();
    void startEncoding();
    void logMessage(const QString &message);
    void updateFileCount();
    void updateRateLabel();
    void updateProgress(int current, int total);
    void encodingCompleted(bool success, const QString &message);

private:
    void setEncodingState(bool encoding);
    void loadAvailableCodecs();

    QListWidget *listView;
    QPushButton *addButton;
    QPushButton *removeButton;
    QPushButton *clearButton;
    QPushButton *encodeButton;
    QRadioButton *fpsRadio;
    QRadioButton *fpiRadio;
    QSpinBox *spinBox;
    QComboBox *resolutionComboBox;
    QComboBox *codecComboBox;
    QComboBox *scalingComboBox;
    QComboBox *rateControlComboBox;
    QCheckBox *sortCheckBox;
    QCheckBox *shuffleCheckBox;
    QSpinBox *crfSpinBox;
    QSpinBox *bitrateSpinBox;
    QLabel *qualityLabel;
    QLabel *bitrateLabel;
    QTextEdit *debugLog;
    QLabel *fileCountLabel;
    QLabel *rateLabel;
    QLabel *statusLabel;
    QProgressBar *progressBar;
    Worker *worker;
};

#endif
