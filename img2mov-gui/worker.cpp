#include "worker.h"
#include <QThread>
#include"ffmpeg_write.h"

cv::Mat resizeKeepAspectRatio(const cv::Mat &input, const cv::Size &dstSize, const cv::Scalar &bgcolor) {
    cv::Mat output;
    double h1 = dstSize.width * (input.rows/(double)input.cols);
    double w2 = dstSize.height * (input.cols/(double)input.rows);
    if(h1 <= dstSize.height)
        cv::resize(input, output, cv::Size(dstSize.width, h1));
    else
        cv::resize(input, output, cv::Size(w2, dstSize.height));
    int top = (dstSize.height-output.rows)/2;
    int down = (dstSize.height-output.rows+1)/2;
    int left = (dstSize.width - output.cols)/2;
    int right = (dstSize.width - output.cols+1)/2;
    cv::copyMakeBorder(output, output, top, down, left, right, cv::BORDER_CONSTANT, bgcolor);
    return output;
}


void Worker::run() {
    emit logMessage("Starting up encode process");
    QStringList parts = resolution.split('x');
    int x = 0, y = 0;
    if (parts.size() == 2) {
        x = parts[0].toInt();
        y = parts[1].toInt();
    } else {
        emit logMessage("Invalid resolution should by WidthxHeight");
        return;
    }
    QString total = QString::number(files.size());
    QString frames_per = QString::number(frame_per_image);

    FILE *fptr = open_ffmpeg(filename.toStdString().c_str(), "libx264", resolution.toStdString().c_str(), resolution.toStdString().c_str(), fps_value.toStdString().c_str(), "24");
    if(!fptr) {
        emit logMessage("Could not open ffmpeg...\n");
        return;
    }
    for(int i = 0; i < files.size(); ++i) {
        cv::Mat frame = cv::imread(files[i].toStdString());
        if(frame.empty()) continue;
        cv::Mat resized = resizeKeepAspectRatio(frame, cv::Size(x,y), cv::Scalar(0, 0, 0));
        if(frame_per_image != 0)
            for(int z = 0; z < frame_per_image; ++z) {
                write_ffmpeg(fptr, resized);
                emit logMessage(" -- [" + QString::number(z+1) + "/" + frames_per + "] frames per image");
            }
        else
            write_ffmpeg(fptr, resized);

        emit logMessage("[" + QString::number(i+1) + "/" + total + "] frame written");
    }
#ifdef _WIN32
    _pclose(fptr);
#else   
    if(pclose(fptr) < 0) {
        std::cerr << "Error closing..\n";
    }
#endif
    frame_per_image = 0;
    emit logMessage("Successfully finished.");
}

void Worker::setFps(float value) {
    fps_value = QString::number(value);
}

void Worker::setFramesPerImage(int frame_count) {
    frame_per_image = frame_count;
    fps_value = "30";
}
    
void Worker::setOutput(const QString &file) {
    filename = file;
}


void Worker::addFile(const QString &file) {
    files.append(file);
}

void Worker::clear() {
    if(!files.isEmpty())
        files.clear();
}

void Worker::setResolution(const QString &res) {
    resolution = res;
}