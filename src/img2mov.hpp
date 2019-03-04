/*
 
 img2mov - creates video files from images
 written by Jared Bruni
 
 (C) 2018 by Jared Bruni
 License: GPL v3
 
 */

#ifndef __IMG2MOV___H_
#define __IMG2MOV___H_

#define IMG2MOV_VERSION "1.03"

#include<opencv2/opencv.hpp>
#include<iostream>
#include<vector>
#include<sstream>
#include<string>
#include<algorithm>
#include<fstream>
#include<regex>
#include<iomanip>
#include<cctype>
#include<cstdlib>
#include<unistd.h>
#include<dirent.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<signal.h>
#include<random>
namespace video_tool {
    
    enum FileType {
        PNG,
        JPG,
        BMP
    };
    
    class img2mov {
    public:
        img2mov(std::string name, std::string d, bool sort_list) : dirn(d), no_sort(sort_list), quiet(false), shuffle_(false) {}
        img2mov(std::string name, std::string d, std::string f, double fps_, unsigned int w_, unsigned int h_, bool stretch_, bool ns) : filen(f), dirn(d), fps(fps_), w(w_), h(h_), stretch_image(stretch_), stop_prog(false), no_sort(ns), use_list(false), output_list(false), quiet(false), shuffle_(false), video_mode(0) {}
        static void setName(const std::string &n);
        void setOutputList(const std::string &s);
        void setList(const std::string &s);
        void setRegEx(const std::string &r);
        void setRegExMatch(const std::string &m);
        void setQuiet(bool q);
        void setShuffle(bool b);
        void run();
        void output();
        void addDirectory(std::string path, std::vector<std::string> &files);
        void stop();
        static std::string toLower(const std::string &text);
        static const std::string name();
        std::string searchMode();
        static void extractImagesFromFile(FileType ftype, const std::string &filename, const std::string file_prefix);
        static void extractImage(FileType ftype, const std::string &filename, const int &index, const std::string file_prefix);
    private:
        static std::string program_name;
        std::string filen, dirn;
        double fps;
        unsigned int w, h;
        bool stretch_image;
        bool stop_prog;
        bool no_sort;
        bool use_list;
        bool output_list;
        bool quiet;
        unsigned int video_mode;
        std::string output_list_name;
        std::string expr, match_expr;
        std::string text_file;
        bool shuffle_;
        cv::Mat resizeKeepAspectRatio(const cv::Mat &input, const cv::Size &dstSize, const cv::Scalar &bgcolor);
    };
}

#endif

