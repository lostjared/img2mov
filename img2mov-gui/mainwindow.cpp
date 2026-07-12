#include "mainwindow.h"
#include "worker.h"

#include <algorithm>
#include <QAbstractItemView>
#include <QComboBox>
#include <QCheckBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QFrame>
#include <QGroupBox>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QProgressBar>
#include <QProcess>
#include <QPushButton>
#include <QRadioButton>
#include <QRegularExpression>
#include <QScreen>
#include <QSpinBox>
#include <QStyle>
#include <QTextEdit>
#include <QVBoxLayout>
#include <random>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), worker(new Worker(this)) {
    setWindowTitle("img2mov Studio");
    setMinimumSize(960, 720);
    const QRect availableScreen = QGuiApplication::primaryScreen()->availableGeometry();
    const QSize preferredSize(1180, 900);
    resize(qMin(preferredSize.width(), availableScreen.width() - 40),
           qMin(preferredSize.height(), availableScreen.height() - 40));

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(28, 24, 28, 24);
    mainLayout->setSpacing(18);

    QLabel *title = new QLabel("Create a video from your images", this);
    title->setObjectName("title");
    QLabel *subtitle = new QLabel("Add frames in playback order, choose your output settings, then encode.", this);
    subtitle->setObjectName("subtitle");
    mainLayout->addWidget(title);
    mainLayout->addWidget(subtitle);

    QHBoxLayout *workspaceLayout = new QHBoxLayout;
    workspaceLayout->setSpacing(18);

    QGroupBox *filesGroup = new QGroupBox("1  IMAGE SEQUENCE", this);
    QVBoxLayout *filesLayout = new QVBoxLayout(filesGroup);
    filesLayout->setContentsMargins(16, 24, 16, 16);
    fileCountLabel = new QLabel("No images selected", this);
    fileCountLabel->setObjectName("mutedLabel");
    listView = new QListWidget(this);
    listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    listView->setAlternatingRowColors(true);
    listView->setDragDropMode(QAbstractItemView::InternalMove);
    listView->setDefaultDropAction(Qt::MoveAction);
    listView->setToolTip("Drag items to change playback order");
    filesLayout->addWidget(fileCountLabel);
    filesLayout->addWidget(listView, 1);

    QHBoxLayout *fileButtons = new QHBoxLayout;
    addButton = new QPushButton("Add images", this);
    addButton->setIcon(style()->standardIcon(QStyle::SP_FileDialogNewFolder));
    addButton->setObjectName("secondaryButton");
    removeButton = new QPushButton("Remove", this);
    clearButton = new QPushButton("Clear", this);
    fileButtons->addWidget(addButton);
    fileButtons->addWidget(removeButton);
    fileButtons->addStretch();
    fileButtons->addWidget(clearButton);
    filesLayout->addLayout(fileButtons);

    QHBoxLayout *orderLayout = new QHBoxLayout;
    sortCheckBox = new QCheckBox("Sort alphabetically", this);
    sortCheckBox->setChecked(true);
    shuffleCheckBox = new QCheckBox("Shuffle before encoding", this);
    orderLayout->addWidget(sortCheckBox);
    orderLayout->addWidget(shuffleCheckBox);
    orderLayout->addStretch();
    filesLayout->addLayout(orderLayout);
    workspaceLayout->addWidget(filesGroup, 3);

    QGroupBox *settingsGroup = new QGroupBox("2  OUTPUT SETTINGS", this);
    QVBoxLayout *settingsLayout = new QVBoxLayout(settingsGroup);
    settingsLayout->setContentsMargins(18, 24, 18, 18);
    settingsLayout->setSpacing(12);

    settingsLayout->addWidget(new QLabel("Resolution", this));
    resolutionComboBox = new QComboBox(this);
    resolutionComboBox->setEditable(true);
    resolutionComboBox->addItems({"3840x2160 (4K)", "1920x1080 (Full HD)", "1280x720 (HD)", "1080x1080 (Square)", "1080x1920 (Vertical)"});
    resolutionComboBox->setCurrentIndex(2);
    settingsLayout->addWidget(resolutionComboBox);

    settingsLayout->addWidget(new QLabel("Image scaling", this));
    scalingComboBox = new QComboBox(this);
    scalingComboBox->addItem("Fit — preserve aspect ratio", false);
    scalingComboBox->addItem("Stretch — fill output frame", true);
    settingsLayout->addWidget(scalingComboBox);

    settingsLayout->addSpacing(6);
    settingsLayout->addWidget(new QLabel("Timing mode", this));
    fpsRadio = new QRadioButton("Frames per second", this);
    fpiRadio = new QRadioButton("Hold each image", this);
    fpsRadio->setChecked(true);
    settingsLayout->addWidget(fpsRadio);
    settingsLayout->addWidget(fpiRadio);

    QHBoxLayout *rateLayout = new QHBoxLayout;
    rateLabel = new QLabel("Frame rate", this);
    spinBox = new QSpinBox(this);
    spinBox->setRange(1, 120);
    spinBox->setValue(30);
    spinBox->setSuffix(" fps");
    rateLayout->addWidget(rateLabel);
    rateLayout->addStretch();
    rateLayout->addWidget(spinBox);
    settingsLayout->addLayout(rateLayout);

    settingsLayout->addSpacing(6);
    settingsLayout->addWidget(new QLabel("Video codec", this));
    codecComboBox = new QComboBox(this);
    codecComboBox->setToolTip("Video encoders reported by the installed FFmpeg");
    codecComboBox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    codecComboBox->setMinimumContentsLength(24);
    settingsLayout->addWidget(codecComboBox);

    settingsLayout->addWidget(new QLabel("Rate control", this));
    rateControlComboBox = new QComboBox(this);
    rateControlComboBox->addItem("Quality — CRF / CQ", "quality");
    rateControlComboBox->addItem("CBR — constant bitrate", "cbr");
    rateControlComboBox->addItem("VBR — variable bitrate", "vbr");
    settingsLayout->addWidget(rateControlComboBox);

    QHBoxLayout *qualityLayout = new QHBoxLayout;
    qualityLabel = new QLabel("CRF / CQ quality", this);
    qualityLabel->setToolTip("Used by libx264 and libx265. Lower values increase quality and file size.");
    crfSpinBox = new QSpinBox(this);
    crfSpinBox->setRange(0, 51);
    crfSpinBox->setValue(22);
    crfSpinBox->setToolTip("0 is lossless; 18–28 is a common range");
    qualityLayout->addWidget(qualityLabel);
    qualityLayout->addStretch();
    qualityLayout->addWidget(crfSpinBox);
    settingsLayout->addLayout(qualityLayout);

    QHBoxLayout *bitrateLayout = new QHBoxLayout;
    bitrateLabel = new QLabel("Target bitrate", this);
    bitrateSpinBox = new QSpinBox(this);
    bitrateSpinBox->setRange(100, 200000);
    bitrateSpinBox->setSingleStep(500);
    bitrateSpinBox->setValue(8000);
    bitrateSpinBox->setSuffix(" kbps");
    bitrateSpinBox->setEnabled(false);
    bitrateLabel->setEnabled(false);
    bitrateLayout->addWidget(bitrateLabel);
    bitrateLayout->addStretch();
    bitrateLayout->addWidget(bitrateSpinBox);
    settingsLayout->addLayout(bitrateLayout);
    settingsLayout->addStretch();
    workspaceLayout->addWidget(settingsGroup, 2);
    mainLayout->addLayout(workspaceLayout, 1);

    QFrame *footer = new QFrame(this);
    footer->setObjectName("footer");
    QHBoxLayout *footerLayout = new QHBoxLayout(footer);
    statusLabel = new QLabel("Ready to encode", this);
    statusLabel->setObjectName("statusLabel");
    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setTextVisible(false);
    encodeButton = new QPushButton("Choose output and encode", this);
    encodeButton->setObjectName("primaryButton");
    encodeButton->setMinimumHeight(42);
    footerLayout->addWidget(statusLabel);
    footerLayout->addWidget(progressBar, 1);
    footerLayout->addWidget(encodeButton);
    mainLayout->addWidget(footer);

    debugLog = new QTextEdit(this);
    debugLog->setReadOnly(true);
    debugLog->setMaximumHeight(105);
    debugLog->setPlaceholderText("Encoding details will appear here…");
    mainLayout->addWidget(debugLog);
    setCentralWidget(centralWidget);

    setStyleSheet(R"(
        QMainWindow, QWidget { background: #10141c; color: #e8ecf3; font-size: 13px; }
        QLabel#title { font-size: 26px; font-weight: 700; color: #ffffff; }
        QLabel#subtitle, QLabel#mutedLabel { color: #8f9bad; }
        QGroupBox { background: #171d27; border: 1px solid #293241; border-radius: 10px; margin-top: 10px; font-weight: 700; color: #aab5c5; }
        QGroupBox::title { subcontrol-origin: margin; left: 14px; padding: 0 6px; }
        QListWidget, QTextEdit, QComboBox, QSpinBox { background: #0d1118; border: 1px solid #303a4a; border-radius: 6px; padding: 7px; selection-background-color: #376cfb; }
        QListWidget::item { padding: 7px; border-radius: 4px; }
        QListWidget::item:alternate { background: #131923; }
        QPushButton { background: #252e3c; border: 1px solid #374356; border-radius: 6px; padding: 8px 13px; font-weight: 600; }
        QPushButton:hover { background: #303b4d; border-color: #52617a; }
        QPushButton:disabled { color: #687386; background: #1b222d; }
        QPushButton#primaryButton { background: #376cfb; border-color: #376cfb; color: white; padding: 10px 20px; }
        QPushButton#primaryButton:hover { background: #4779ff; }
        QRadioButton { spacing: 8px; padding: 3px; }
        QRadioButton::indicator:checked { background: #376cfb; border: 3px solid #10141c; }
        QRadioButton::indicator { width: 14px; height: 14px; border-radius: 9px; border: 1px solid #647189; }
        QProgressBar { background: #222a36; border: 0; border-radius: 3px; max-height: 6px; }
        QProgressBar::chunk { background: #4b7cff; border-radius: 3px; }
        QFrame#footer { background: #171d27; border-radius: 9px; }
        QLabel#statusLabel { padding-left: 4px; min-width: 110px; }
    )");

    connect(addButton, &QPushButton::clicked, this, &MainWindow::addFiles);
    connect(removeButton, &QPushButton::clicked, this, &MainWindow::removeSelected);
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::clearFiles);
    connect(encodeButton, &QPushButton::clicked, this, &MainWindow::startEncoding);
    connect(fpsRadio, &QRadioButton::toggled, this, &MainWindow::updateRateLabel);
    connect(sortCheckBox, &QCheckBox::toggled, this, [this](bool enabled) {
        if (enabled) {
            listView->sortItems(Qt::AscendingOrder);
        }
    });
    connect(rateControlComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int) {
        const bool qualityMode = rateControlComboBox->currentData().toString() == "quality";
        qualityLabel->setEnabled(qualityMode);
        crfSpinBox->setEnabled(qualityMode);
        bitrateLabel->setEnabled(!qualityMode);
        bitrateSpinBox->setEnabled(!qualityMode);
    });
    connect(worker, &Worker::logMessage, this, &MainWindow::logMessage);
    connect(worker, &Worker::progress, this, &MainWindow::updateProgress);
    connect(worker, &Worker::completed, this, &MainWindow::encodingCompleted);
    loadAvailableCodecs();
}

MainWindow::~MainWindow() {
    if (worker->isRunning()) {
        worker->wait();
    }
}

void MainWindow::addFiles() {
    const QStringList files = QFileDialog::getOpenFileNames(
        this, "Add image frames", QString(), "Images (*.png *.jpg *.jpeg *.bmp *.tif *.tiff *.webp)");
    for (const QString &file : files) {
        bool alreadyAdded = false;
        for (int i = 0; i < listView->count(); ++i) {
            if (listView->item(i)->data(Qt::UserRole).toString() == file) {
                alreadyAdded = true;
                break;
            }
        }
        if (!alreadyAdded) {
            QListWidgetItem *item = new QListWidgetItem(QFileInfo(file).fileName(), listView);
            item->setData(Qt::UserRole, file);
            item->setToolTip(file);
        }
    }
    if (sortCheckBox->isChecked()) {
        listView->sortItems(Qt::AscendingOrder);
    }
    updateFileCount();
}

void MainWindow::removeSelected() {
    qDeleteAll(listView->selectedItems());
    updateFileCount();
}

void MainWindow::clearFiles() {
    listView->clear();
    updateFileCount();
}

void MainWindow::startEncoding() {
    if (worker->isRunning()) {
        return;
    }
    if (listView->count() == 0) {
        QMessageBox::information(this, "No images", "Add at least one image before encoding.");
        return;
    }

    const QRegularExpression resolutionPattern("^(\\d{2,5})x(\\d{2,5})");
    const QRegularExpressionMatch match = resolutionPattern.match(resolutionComboBox->currentText().trimmed());
    if (!match.hasMatch() || match.captured(1).toInt() < 16 || match.captured(2).toInt() < 16) {
        QMessageBox::warning(this, "Invalid resolution", "Enter a resolution such as 1920x1080.");
        return;
    }

    QString outputFilename = QFileDialog::getSaveFileName(
        this, "Save video", "output.mp4", "MPEG-4 Video (*.mp4)");
    if (outputFilename.isEmpty()) {
        return;
    }
    if (!outputFilename.endsWith(".mp4", Qt::CaseInsensitive)) {
        outputFilename += ".mp4";
    }

    QStringList inputFiles;
    for (int i = 0; i < listView->count(); ++i) {
        inputFiles.append(listView->item(i)->data(Qt::UserRole).toString());
    }
    if (sortCheckBox->isChecked()) {
        std::sort(inputFiles.begin(), inputFiles.end(), [](const QString &a, const QString &b) {
            return a.compare(b, Qt::CaseInsensitive) < 0;
        });
    }
    if (shuffleCheckBox->isChecked()) {
        std::random_device seed;
        std::mt19937 generator(seed());
        std::shuffle(inputFiles.begin(), inputFiles.end(), generator);
    }

    worker->clear();
    for (const QString &file : inputFiles) {
        worker->addFile(file);
    }
    worker->setResolution(match.captured(1) + "x" + match.captured(2));
    if (fpsRadio->isChecked()) {
        worker->setFps(spinBox->value());
    } else {
        worker->setFramesPerImage(spinBox->value());
    }
    worker->setOutput(outputFilename);
    worker->setCodec(codecComboBox->currentData().toString());
    worker->setStretch(scalingComboBox->currentData().toBool());
    worker->setCrf(crfSpinBox->value());
    worker->setRateControl(rateControlComboBox->currentData().toString(), bitrateSpinBox->value());

    debugLog->clear();
    logMessage("Output: " + outputFilename);
    setEncodingState(true);
    worker->start();
}

void MainWindow::logMessage(const QString &message) {
    debugLog->append(message.toHtmlEscaped());
}

void MainWindow::updateFileCount() {
    const int count = listView->count();
    fileCountLabel->setText(count == 0 ? "No images selected" : QString("%1 image%2 • drag to reorder").arg(count).arg(count == 1 ? "" : "s"));
    removeButton->setEnabled(count > 0);
    clearButton->setEnabled(count > 0);
}

void MainWindow::updateRateLabel() {
    const bool fpsMode = fpsRadio->isChecked();
    rateLabel->setText(fpsMode ? "Frame rate" : "Frames per image");
    spinBox->setSuffix(fpsMode ? " fps" : " frames");
    spinBox->setRange(1, fpsMode ? 120 : 900);
    if (fpsMode && spinBox->value() > 120) {
        spinBox->setValue(30);
    }
}

void MainWindow::updateProgress(int current, int total) {
    progressBar->setValue(total > 0 ? (current * 100) / total : 0);
    statusLabel->setText(QString("Encoding %1/%2").arg(current).arg(total));
}

void MainWindow::encodingCompleted(bool success, const QString &message) {
    setEncodingState(false);
    statusLabel->setText(success ? "Complete" : "Encoding failed");
    progressBar->setValue(success ? 100 : 0);
    logMessage(message);
    if (!success) {
        QMessageBox::critical(this, "Encoding failed", message);
    }
}

void MainWindow::setEncodingState(bool encoding) {
    addButton->setEnabled(!encoding);
    removeButton->setEnabled(!encoding && listView->count() > 0);
    clearButton->setEnabled(!encoding && listView->count() > 0);
    encodeButton->setEnabled(!encoding);
    encodeButton->setText(encoding ? "Encoding…" : "Choose output and encode");
    listView->setEnabled(!encoding);
    resolutionComboBox->setEnabled(!encoding);
    codecComboBox->setEnabled(!encoding && codecComboBox->count() > 0);
    scalingComboBox->setEnabled(!encoding);
    sortCheckBox->setEnabled(!encoding);
    shuffleCheckBox->setEnabled(!encoding);
    crfSpinBox->setEnabled(!encoding);
    rateControlComboBox->setEnabled(!encoding);
    const bool qualityMode = rateControlComboBox->currentData().toString() == "quality";
    qualityLabel->setEnabled(!encoding && qualityMode);
    crfSpinBox->setEnabled(!encoding && qualityMode);
    bitrateLabel->setEnabled(!encoding && !qualityMode);
    bitrateSpinBox->setEnabled(!encoding && !qualityMode);
    fpsRadio->setEnabled(!encoding);
    fpiRadio->setEnabled(!encoding);
    spinBox->setEnabled(!encoding);
    if (encoding) {
        statusLabel->setText("Preparing…");
        progressBar->setValue(0);
    }
}

void MainWindow::loadAvailableCodecs() {
    codecComboBox->clear();
    codecComboBox->setEnabled(false);
    codecComboBox->addItem("Detecting FFmpeg encoders…");

    QProcess ffmpeg;
    ffmpeg.setProcessChannelMode(QProcess::MergedChannels);
    ffmpeg.start("ffmpeg", {"-hide_banner", "-encoders"});
    if (!ffmpeg.waitForStarted(2000) || !ffmpeg.waitForFinished(5000)) {
        codecComboBox->clear();
        codecComboBox->addItem("Could not query FFmpeg encoders");
        codecComboBox->setToolTip("Ensure ffmpeg is installed and available in PATH");
        encodeButton->setEnabled(false);
        logMessage("Could not query FFmpeg encoders. Encoding is unavailable.");
        return;
    }

    struct EncoderInfo {
        QString name;
        QString description;
    };
    QList<EncoderInfo> encoders;
    const QString output = QString::fromLocal8Bit(ffmpeg.readAll());
    const QRegularExpression linePattern(
        "^\\s*V[A-Z\\.]{5}\\s+(\\S+)\\s+(.+)$",
        QRegularExpression::MultilineOption);
    QRegularExpressionMatchIterator matches = linePattern.globalMatch(output);
    while (matches.hasNext()) {
        const QRegularExpressionMatch match = matches.next();
        encoders.append({match.captured(1), match.captured(2).trimmed()});
    }

    std::sort(encoders.begin(), encoders.end(), [](const EncoderInfo &a, const EncoderInfo &b) {
        return a.name.compare(b.name, Qt::CaseInsensitive) < 0;
    });

    codecComboBox->clear();
    for (const EncoderInfo &encoder : encoders) {
        codecComboBox->addItem(encoder.name + " — " + encoder.description, encoder.name);
    }
    const int defaultCodecIndex = codecComboBox->findData("libx264");
    if (defaultCodecIndex >= 0) {
        codecComboBox->setCurrentIndex(defaultCodecIndex);
    }
    codecComboBox->setEnabled(!encoders.isEmpty());
    codecComboBox->setToolTip(QString("%1 video encoders reported by FFmpeg. Hardware encoders require compatible drivers and hardware.")
                                  .arg(encoders.size()));
    if (encoders.isEmpty()) {
        codecComboBox->addItem("No FFmpeg video encoders found");
        encodeButton->setEnabled(false);
        logMessage("FFmpeg did not report any video encoders.");
    } else {
        statusLabel->setText(QString("Ready • %1 codecs").arg(encoders.size()));
    }
}
