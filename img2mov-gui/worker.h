#ifndef WORKER_H
#define WORKER_H

#include <QThread>
#include <QString>
#include <QVector>

class Worker : public QThread {
    Q_OBJECT

public:
    Worker(QObject *parent) : QThread(parent) {}
    void run() override;
    void addFile(const QString &file);
    void setResolution(const QString &res);
    void setFps(float value);
    void setFramesPerImage(int frame_count);
    void setOutput(const QString &file);
    void clear();
signals:
    void logMessage(const QString &message);
private:
    QVector<QString> files;
    QString resolution;
    QString fps_value;
    QString filename;
    int frame_per_image = 0;
};

#endif