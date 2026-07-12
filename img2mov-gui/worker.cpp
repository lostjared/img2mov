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
    emit logMessage("Starting encoding process…");
    QStringList parts = resolution.split('x');
    int x = 0, y = 0;
    if (parts.size() == 2) {
        x = parts[0].toInt();
        y = parts[1].toInt();
    } else {
        emit completed(false, "Invalid resolution. Use WidthxHeight.");
        return;
    }
    QString total = QString::number(files.size());
    QString frames_per = QString::number(frame_per_image);

    emit logMessage("Encoder: " + codec);
    const QString crfValue = QString::number(crf);
    const QString bitrateValue = QString::number(bitrateKbps);
    FILE *fptr = open_ffmpeg(filename.toStdString().c_str(), codec.toStdString().c_str(), resolution.toStdString().c_str(), resolution.toStdString().c_str(), fps_value.toStdString().c_str(), crfValue.toStdString().c_str(), rateControl.toStdString().c_str(), bitrateValue.toStdString().c_str());
    if(!fptr) {
        emit completed(false, "Could not start FFmpeg. Check that it is installed and available in PATH.");
        return;
    }
    for(int i = 0; i < files.size(); ++i) {
        cv::Mat frame = cv::imread(files[i].toStdString());
        if(frame.empty()) {
            emit logMessage("Skipped unreadable image: " + files[i]);
            continue;
        }
        cv::Mat resized;
        if (stretch) {
            cv::resize(frame, resized, cv::Size(x, y));
        } else {
            resized = resizeKeepAspectRatio(frame, cv::Size(x,y), cv::Scalar(0, 0, 0));
        }
        if(frame_per_image != 0)
            for(int z = 0; z < frame_per_image; ++z) {
                write_ffmpeg(fptr, resized);
            }
        else
            write_ffmpeg(fptr, resized);

        emit logMessage("[" + QString::number(i+1) + "/" + total + "] " + files[i]);
        emit progress(i + 1, files.size());
    }
#ifdef _WIN32
    _pclose(fptr);
#else   
    if(pclose(fptr) < 0) {
        emit completed(false, "FFmpeg exited with an error while finalizing the video.");
        frame_per_image = 0;
        return;
    }
#endif
    frame_per_image = 0;
    emit completed(true, "Video created successfully.");
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

void Worker::setCodec(const QString &codecName) {
    codec = codecName;
}

void Worker::setStretch(bool enabled) {
    stretch = enabled;
}

void Worker::setCrf(int value) {
    crf = value;
}

void Worker::setRateControl(const QString &mode, int bitrate) {
    rateControl = mode;
    bitrateKbps = bitrate;
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
