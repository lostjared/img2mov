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

class img2mov {
public:
    img2mov(std::string d, std::string f, double fps_, unsigned int w_, unsigned int h_, bool stretch_) : filen(f), dirn(d), fps(fps_), w(w_), h(h_), stretch_image(stretch_) {}
    void run();
    void add_directory(std::string path, std::vector<std::string> &files);
private:
    std::string filen, dirn;
    double fps;
    unsigned int w, h;
    bool stretch_image;
};


std::string toLower(const std::string &text);
cv::Mat resizeKeepAspectRatio(const cv::Mat &input, const cv::Size &dstSize, const cv::Scalar &bgcolor);

int main(int argc, char **argv) {
    if(argc > 1) {
        int opt;
        std::string dir_name, file_name;
        double fps = 0;
        unsigned int width = 0, height = 0;
        bool stretch = false;
        while((opt = getopt(argc, argv, "i:o:w:h:f:s")) != -1) {
            switch(opt) {
                case 'i':
                    dir_name = optarg;
                    break;
                case 'o':
                    file_name = optarg;
                    break;
                case 'f':
                    fps = atof(optarg);
                    break;
                case 'w':
                    width = atoi(optarg);
                    break;
                case 'h':
                    height = atoi(optarg);
                    break;
                case 's':
                    stretch = true;
                    break;
                default:
                    std::cout << " use: " << argv[0] << " -i filename\n";
                    exit(0);
                    break;
            }
        }
        if(width == 0  || height == 0 || fps == 0 || dir_name.length() == 0 || file_name.length() == 0) {
            std::cerr << "Requires input/output flags..\n";
            std::cerr << argv[0] << " -i directory -o video -w width -h height -f fps\n";
            exit(EXIT_FAILURE);
        }
        img2mov program(dir_name, file_name,fps,width,height,stretch);
        program.run();
    } else {
        std::cerr << "Requires input/output flags..\n";
        std::cerr << argv[0] << " -i directory -o video\n";
        exit(EXIT_FAILURE);
    }
    return 0;
}

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
