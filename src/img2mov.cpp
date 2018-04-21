#include "img2mov.hpp"

void img2mov::run() {
    stop_prog = false;
    std::vector<std::string> files_v;
    if(use_list) {
        std::fstream file;
        file.open(text_file, std::ios::in);
        if(!file.is_open()) {
            std::cerr << "img2mov: Error could not open text file: " << text_file << "\n";
            exit(EXIT_FAILURE);
        }
        while(!file.eof()) {
            std::string text;
            std::getline(file, text);
            if(file) {
                if(expr.length() > 0) {
                    std::regex statement(expr);
                    std::smatch match_;
                    if(std::regex_search(text,match_,statement)) {
                        files_v.push_back(text);
                    }
                } else {
                    files_v.push_back(text);
                }
            }
        }
    } else
        add_directory(dirn, files_v);
    if(files_v.size() == 0) {
        std::cerr << "img2mov: No files found...\n";
        exit(0);
    }
    if(no_sort == false)
        std::sort(files_v.begin(), files_v.end());
    
    cv::VideoWriter writer;
    writer.open(filen, CV_FOURCC('m', 'p', '4', 'v'), fps, cv::Size(w, h), true);
    
    if(!writer.isOpened()) {
        std::cout << "img2mov: Failed to open file...\n";
        exit(EXIT_FAILURE);
    }
    
    unsigned int frame_count = 0;
    for(unsigned int i = 0; i < files_v.size(); ++i) {
        if(stop_prog == true) break;
        cv::Mat frame;
        frame = cv::imread(files_v[i]);
        if(frame.empty()) {
            std::cerr << "img2mov: frame: " << i << " unable to failed to load...\n";
            continue;
        }
        cv::Mat image;
        if(stretch_image)
            cv::resize(frame, image, cv::Size(w, h));
        else
            image = resizeKeepAspectRatio(frame, cv::Size(w, h), cv::Scalar(0,0,0));
        
        writer.write(image);
        float val = i;
        float size = files_v.size();
        float percent_complete = 0;
        if(size != 0)
            percent_complete = (val/size)*100;
        
        std::cout << "img2mov: Wrote frame " << percent_complete << "% - " << files_v[i] << " [" << frame_count << "/" << files_v.size() << "]\n";
        ++frame_count;
    }
    writer.release();
    std::cout << "img2mov: Wrote " << filen << " " << frame_count << " frames at fps: " << fps << "\n";
}

void img2mov::setList(const std::string &s) {
    text_file = s;
    use_list = true;
}

void img2mov::setRegEx(const std::string &r) {
    expr = r;
}

void img2mov::stop() {
    stop_prog = true;
}

void img2mov::add_directory(std::string path, std::vector<std::string> &files) {
    DIR *dir = opendir(path.c_str());
    if(dir == NULL) {
        std::cerr << "img2mov: Error could not open directory: " << path << "\n";
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
            if(expr.length() > 0) {
                std::regex statement(expr);
                std::smatch match_;
                if(std::regex_search(fullpath,match_,statement)) {
                    files.push_back(fullpath);
                    std::cout << "added: " << fullpath << "\n";
                }
            } else if(toLower(fullpath).find("png") != std::string::npos || toLower(fullpath).find("jpg") != std::string::npos) {
                files.push_back(fullpath);
            }
        }
    }
    closedir(dir);
}

cv::Mat img2mov::resizeKeepAspectRatio(const cv::Mat &input, const cv::Size &dstSize, const cv::Scalar &bgcolor)
{
    cv::Mat output;
    double h1 = dstSize.width * (input.rows/(double)input.cols);
    double w2 = dstSize.height * (input.cols/(double)input.rows);
    if(h1 <= dstSize.height) {
        cv::resize(input, output, cv::Size(dstSize.width, h1));
    } else {
        cv::resize(input, output, cv::Size(w2, dstSize.height));
    }
    int top = (dstSize.height-output.rows)/2;
    int down = (dstSize.height-output.rows+1)/2;
    int left = (dstSize.width - output.cols)/2;
    int right = (dstSize.width - output.cols+1)/2;
    cv::copyMakeBorder(output, output, top, down, left, right, cv::BORDER_CONSTANT, bgcolor);
    return output;
}

std::string img2mov::toLower(const std::string &text) {
    std::string temp;
    for(unsigned int i = 0; i < text.length(); ++i) {
        temp += tolower(text[i]);
    }
    return temp;
}

