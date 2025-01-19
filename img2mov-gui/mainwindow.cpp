#include "mainwindow.h"
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), worker(new Worker(this)) {

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout;

    listView = new QListWidget(this);
    mainLayout->addWidget(listView);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    addButton = new QPushButton("Add Files", this);
    removeButton = new QPushButton("Remove Selected", this);
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(removeButton);
    mainLayout->addLayout(buttonLayout);

    QGroupBox *radioGroup = new QGroupBox("Settings", this);
    QHBoxLayout *radioLayout = new QHBoxLayout;
    fpsRadio = new QRadioButton("Frames per Second", this);
    fpiRadio = new QRadioButton("Frames per Image", this);
    fpsRadio->setChecked(true);
    spinBox = new QSpinBox(this);
    spinBox->setRange(1, 60);
    spinBox->setValue(30);
    radioLayout->addWidget(fpsRadio);
    radioLayout->addWidget(fpiRadio);
    radioLayout->addWidget(new QLabel("Value:", this));
    radioLayout->addWidget(spinBox);
    radioGroup->setLayout(radioLayout);
    mainLayout->addWidget(radioGroup);

    QHBoxLayout *codecLayout = new QHBoxLayout;
    codecComboBox = new QComboBox(this);
    codecComboBox->addItem("x264");
    codecLayout->addWidget(new QLabel("Codec:", this));
    codecLayout->addWidget(codecComboBox);
    mainLayout->addLayout(codecLayout);
    
    QHBoxLayout *resolutionLayout = new QHBoxLayout;
    resEdit = new QLineEdit(this);
    resEdit->setText("1280x720");
    resolutionLayout->addWidget(new QLabel("Resolution:", this));
    resolutionLayout->addWidget(resEdit);
    mainLayout->addLayout(resolutionLayout);
    
    encodeButton = new QPushButton("Encode", this);
    mainLayout->addWidget(encodeButton);

    debugLog = new QTextEdit(this);
    debugLog->setReadOnly(true);
    mainLayout->addWidget(debugLog);

    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    connect(addButton, &QPushButton::clicked, this, &MainWindow::addFiles);
    connect(removeButton, &QPushButton::clicked, this, &MainWindow::removeSelected);
    connect(encodeButton, &QPushButton::clicked, this, &MainWindow::startEncoding);
    connect(worker, &Worker::logMessage, this, &MainWindow::logMessage);
    connect(worker, &Worker::finished, worker, &QObject::deleteLater);
}

MainWindow::~MainWindow() {

}

void MainWindow::addFiles() {
    QStringList files = QFileDialog::getOpenFileNames(this, "Select Image Files", "",
                                                      "Images (*.png *.jpg *.bmp *.tiff *.webp)");
    for (const QString &file : files) {
        listView->addItem(file);
    }
}

void MainWindow::removeSelected() {
    listView->takeItem(listView->currentRow());
}

void MainWindow::startEncoding() {
    worker->clear();
    for(int i = 0; i < listView->count(); ++i) {
        QListWidgetItem *itm = listView->item(i);
        worker->addFile(itm->text());
    }
    if(listView->count() == 0) {
        debugLog->append("No images selected...\n");
        return;
    }
    worker->setResolution(resEdit->text());
    if(fpsRadio->isChecked()) {
        worker->setFps(spinBox->value());
    } else {
        worker->setFramesPerImage(spinBox->value());
    }

    QString outputFilename = QFileDialog::getSaveFileName(this, "Select output video file", "", "Video File (*.mp4)");
    if(outputFilename == "")
        return;

    if(!outputFilename.contains(".mp4")) {
        outputFilename += ".mp4";
    }

    worker->setOutput(outputFilename);
    debugLog->append("Starting encoding process...");
    worker->start();
}

void MainWindow::logMessage(const QString &message) {
    debugLog->append(message);
}
