#include "img2mov.hpp"

void img2mov::run() {
    std::vector<std::string> files_v;
    add_directory(dirn, files_v);
    if(files_v.size() == 0) {
        std::cerr << "No files found...\n";
        exit(0);
    }
    std::sort(files_v.begin(), files_v.end());
    cv::VideoWriter writer;
    writer.open(filen, CV_FOURCC('m', 'p', '4', 'v'), fps, cv::Size(w, h), true);
    
    if(!writer.isOpened()) {
        std::cout << "Failed to open file...\n";
        exit(EXIT_FAILURE);
    }
    
    unsigned int frame_count = 0;
    for(unsigned int i = 0; i < files_v.size(); ++i) {
        cv::Mat frame;
        frame = cv::imread(files_v[i]);
        if(frame.empty()) {
            std::cerr << "frame: " << i << " unable to failed to load...\n";
            continue;
        }
        
        cv::Mat image;
        if(stretch_image) {
            image.create(cvSize(w, h), CV_8UC3);
            for(unsigned int z = 0; z < image.rows; ++z) {
                for(unsigned int i = 0; i < image.cols; ++i) {
                    int cX = AC_GetFX(frame.cols, i, image.cols);
                    int cY = AC_GetFZ(frame.rows, z, image.rows);
                    cv::Vec3b &pixel = image.at<cv::Vec3b>(z, i);
                    pixel = frame.at<cv::Vec3b>(cY, cX);
                }
            }
        } else {
            image = resizeKeepAspectRatio(frame, cv::Size(w, h), cv::Scalar(0,0,0));
        }
        writer.write(image);
        std::cout << "Wrote frame: " << files_v[i] << " [" << frame_count << "/" << files_v.size() << "]\n";
        ++frame_count;
    }
    writer.release();
    std::cout << "Wrote: " << frame_count << " frames at fps: " << fps << "\n";
}

void img2mov::add_directory(std::string path, std::vector<std::string> &files) {
    DIR *dir = opendir(path.c_str());
    if(dir == NULL) {
        std::cerr << "Error could not open directory: " << path << "\n";
        return;
    }
    dirent *file_info;
    while( (file_info = readdir(dir)) != 0 ) {
        std::string f_info = file_info->d_name;
        if(f_info == "." || f_info == "..")  continue;
        std::string fullpath=path+"/"+f_info;
        struct stat s;
        
        lstat(fullpath.c_str(), &s);
        if(S_ISDIR(s.st_mode)) {
            if(f_info.length()>0 && f_info[0] != '.')
                add_directory(path+"/"+f_info, files);
            
            continue;
        }
        if(f_info.length()>0 && f_info[0] != '.') {
            if(toLower(fullpath).find("png") != std::string::npos || toLower(fullpath).find("jpg") != std::string::npos) {
                files.push_back(fullpath);
            }
        }
    }
    closedir(dir);
}

std::string toLower(const std::string &text) {
    std::string temp;
    for(unsigned int i = 0; i < text.length(); ++i) {
        temp += tolower(text[i]);
    }
    return temp;
}

cv::Mat resizeKeepAspectRatio(const cv::Mat &input, const cv::Size &dstSize, const cv::Scalar &bgcolor)
{
    cv::Mat output;
    double h1 = dstSize.width * (input.rows/(double)input.cols);
    double w2 = dstSize.height * (input.cols/(double)input.rows);
    if( h1 <= dstSize.height) {
        cv::resize( input, output, cv::Size(dstSize.width, h1));
    } else {
        cv::resize( input, output, cv::Size(w2, dstSize.height));
    }
    int top = (dstSize.height-output.rows) / 2;
    int down = (dstSize.height-output.rows+1) / 2;
    int left = (dstSize.width - output.cols) / 2;
    int right = (dstSize.width - output.cols+1) / 2;
    cv::copyMakeBorder(output, output, top, down, left, right, cv::BORDER_CONSTANT, bgcolor );
    return output;
}

