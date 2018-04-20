#ifndef __IMG2MOV___H_
#define __IMG2MOV___H_
#include<opencv2/opencv.hpp>
#include<iostream>
#include<vector>
#include<string>
#include<algorithm>
#include<cctype>
#include<cstdlib>
#include<unistd.h>
#include<dirent.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<signal.h>
// Resize X variable
inline int AC_GetFX(int oldw,int x, int nw) {
    float xp = (float)x * (float)oldw / (float)nw;
    return (int)xp;
}
// Resize Y Variable
inline int AC_GetFZ(int oldh, int y, int nh) {
    float yp = (float)y * (float)oldh / (float)nh;
    return (int)yp;
}

std::string toLower(const std::string &text);

class img2mov {
public:
    img2mov(std::string d, std::string f, double fps_, unsigned int w_, unsigned int h_, bool stretch_, bool ns) : filen(f), dirn(d), fps(fps_), w(w_), h(h_), stretch_image(stretch_), stop_prog(false), no_sort(ns) {}
    void run();
    void add_directory(std::string path, std::vector<std::string> &files);
    void stop();
private:
    std::string filen, dirn;
    double fps;
    unsigned int w, h;
    bool stretch_image;
    bool stop_prog;
    bool no_sort;
    cv::Mat resizeKeepAspectRatio(const cv::Mat &input, const cv::Size &dstSize, const cv::Scalar &bgcolor);
};

#endif


