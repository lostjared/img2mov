/*
 
 img2mov - creates video files from images
 written by Jared Bruni
 
 (C) 2018 by Jared Bruni
 License: GPL v3
 
 */

#include "img2mov.hpp"

namespace video_tool {
    
    std::string img2mov::program_name;
    
    const std::string img2mov::name() {
        return program_name;
    }
    
    void img2mov::setName(const std::string &s) {
        program_name = s;
    }
    
    void img2mov::output() {
        if(output_list == false) {
            std::cerr << name() << ": requires flag to be set...\n";
            exit(EXIT_FAILURE);
        }
        if(quiet == false) std::cout << name() << ": Search Mode: [" << searchMode() << "]\n";
        std::vector<std::string> output_files;
        addDirectory(dirn,output_files);
        if(output_files.size()==0) {
            std::cerr << name() << ": No files found...\n";
        } else {
            if(no_sort == false) {
                if(quiet == false) std::cout << name() << ": sorting list...\n";
                std::sort(output_files.begin(), output_files.end());
            } else
                if(quiet == false) std::cout << name() << ": not sorting list...\n";
            
            std::fstream f;
            f.open(output_list_name, std::ios::out);
            for(unsigned int i = 0; i < output_files.size(); ++i) {
                f << output_files[i] << "\n";
            }
            f.close();
            std::cout << name() << ": saved to file: " << output_list_name << "\n";
        }
    }
    
    void img2mov::run() {
        
        if(toLower(filen).find(".avi") != std::string::npos) {
            video_mode = 1;
        }
        stop_prog = false;
        std::vector<std::string> files_v;
        if(quiet == false) std::cout << name() << ": Search Mode: [" << searchMode() << "]\n";
        if(use_list) {
            std::fstream file;
            file.open(text_file, std::ios::in);
            if(!file.is_open()) {
                std::cerr << name() << ": Error could not open text file: " << text_file << "\n";
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
                    } else if(match_expr.length() > 0) {
                        std::regex statement(match_expr);
                        if(std::regex_match(text, statement)) {
                            files_v.push_back(text);
                        }
                    } else {
                        files_v.push_back(text);
                    }
                }
            }
        } else
            addDirectory(dirn, files_v);
        
        if(files_v.size() == 0) {
            std::cerr << name() << ": No files found...\n";
            exit(0);
        }
        if(no_sort == false) {
            if(quiet == false) std::cout << name() << ": sorting list...\n";
            std::sort(files_v.begin(), files_v.end());
        } else
            if(quiet == false) std::cout << name() << ": not sorting list...\n";
        
        cv::VideoWriter writer;
        if(video_mode == 1)
            writer.open(filen, CV_FOURCC('X', 'V', 'I', 'D'), fps, cv::Size(w,h), true);
        else
            writer.open(filen, CV_FOURCC('m', 'p', '4', 'v'), fps, cv::Size(w, h), true);
        
        if(!writer.isOpened()) {
            std::cerr << name() << ": Failed to open file...\n";
            exit(EXIT_FAILURE);
        }
        if(quiet == false) std::cout << name() << ": Video opened as: " << ((video_mode == 1) ? "XviD" : "MPEG-4") << "\n";
        
        unsigned int frame_count = 0;
        for(unsigned int i = 0; i < files_v.size(); ++i) {
            if(stop_prog == true) break;
            cv::Mat frame;
            frame = cv::imread(files_v[i]);
            if(frame.empty()) {
                std::cerr << name() << ": frame: " << i << " unable to load image: " << files_v[i] << "\n";
                continue;
            }
            cv::Mat image;
            if(stretch_image)
                cv::resize(frame, image, cv::Size(w, h));
            else
                image = resizeKeepAspectRatio(frame, cv::Size(w, h), cv::Scalar(0,0,0));
            writer.write(image);
            float val = i+1;
            float size = files_v.size();
            float percent_complete = 0;
            if(size != 0)
                percent_complete = (val/size)*100;
            struct stat buf;
            lstat(filen.c_str(), &buf);
            if(quiet == false) std::cout << name() << ": Wrote Frame File Size: " << ((buf.st_size/1024)/1024) << " MB - " << static_cast<unsigned int>(percent_complete) << "% - " << files_v[i] << " [" << frame_count+1 << "/" << files_v.size() << "]\n";;
            ++frame_count;
        }
        writer.release();
        std::cout << name() << ": Wrote " << filen << " " << frame_count << " frames at fps: " << fps << "\n";
    }
    
    void img2mov::setOutputList(const std::string &s) {
        output_list_name = s;
        output_list = true;
    }
    
    void img2mov::setList(const std::string &s) {
        text_file = s;
        use_list = true;
    }
    
    void img2mov::setRegEx(const std::string &r) {
        expr = r;
    }
    
    void img2mov::setRegExMatch(const std::string &m) {
        match_expr = m;
    }
    
    void img2mov::setQuiet(bool q) {
        quiet = q;
    }
    
    void img2mov::stop() {
        stop_prog = true;
    }
    
    std::string img2mov::searchMode() {
        if(expr.length() > 0)
            return "regex: search";
        else if(match_expr.length() > 0)
            return "regex: match";
        return "png/jpg search";
    }
    
    void img2mov::addDirectory(std::string path, std::vector<std::string> &files) {
        DIR *dir = opendir(path.c_str());
        if(dir == NULL) {
            std::cerr << name() << ": Error could not open directory: " << path << "\n";
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
                    addDirectory(path+"/"+f_info, files);
                continue;
            }
            if(f_info.length()>0 && f_info[0] != '.') {
                if(expr.length() > 0) {
                    std::regex statement(expr);
                    std::smatch match_;
                    if(std::regex_search(fullpath,match_,statement)) {
                        files.push_back(fullpath);
                    }
                } else if(match_expr.length() > 0) {
                    std::regex statement(match_expr);
                    if(std::regex_match(fullpath, statement)) {
                        files.push_back(fullpath);
                    }
                } else if(toLower(fullpath).find("png") != std::string::npos || toLower(fullpath).find("jpg") != std::string::npos) {
                    files.push_back(fullpath);
                }
            }
        }
        closedir(dir);
    }
    
    cv::Mat img2mov::resizeKeepAspectRatio(const cv::Mat &input, const cv::Size &dstSize, const cv::Scalar &bgcolor) {
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
    
    std::string img2mov::toLower(const std::string &text) {
        std::string temp;
        for(unsigned int i = 0; i < text.length(); ++i) {
            temp += tolower(text[i]);
        }
        return temp;
    }
    
    void img2mov::extractImagesFromFile(FileType ft, const std::string &filename, const std::string file_prefix) {
        unsigned long index = 0;
        unsigned long total_frames = 0;
        
        if(file_prefix == ".") {
            std::cerr << "img2mov: Invalid file prefix, requires path...\n";
            exit(EXIT_FAILURE);
        }
        
        cv::VideoCapture cap(filename);
        if(!cap.isOpened()) {
            std::cerr << name() << ": Error could not open video file: " << filename << "\n";
            exit(EXIT_FAILURE);
        }
        total_frames = cap.get(CV_CAP_PROP_FRAME_COUNT);
        bool active = true;
        
        while(active) {
            cv::Mat frame;
            if(cap.read(frame)==false)
                break;
            std::ostringstream filename_info;
            std::ostringstream total_f;
            total_f << total_frames;
            char prev;
            filename_info << file_prefix;
            filename_info.width(total_f.str().length()+1);
            prev = filename_info.fill('0');
            filename_info << index;;
            filename_info.fill(prev);
            if(ft == FileType::PNG)
                filename_info << ".png";
            else
                filename_info << ".jpg";
            
            double complete = 0;
            double val = index+1;
            double size = total_frames;
            complete = (val/size)*100;
            
            if(!cv::imwrite(filename_info.str(), frame)) {
                std::cerr << name() << ": Could not write output file: " << filename_info.str() << "\n";
                exit(EXIT_FAILURE);
            }
            std::cout << name() << ": Wrote image: " << static_cast<unsigned int>(complete) << "% - [" << (index+1) << "/" << total_frames << "] - " << filename_info.str() << "\n";
            
            ++index;
        }
        
    }
    
}

