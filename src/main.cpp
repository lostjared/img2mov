/*
 
 img2mov - creates video files from images
 written by Jared Bruni
 
 (C) 2018 by Jared Bruni
 License: GPL v3
 
 */

#include "img2mov.hpp"
#include<memory>

std::unique_ptr<video_tool::img2mov> program;

void control_Handler(int sig) {
    if(program) {
        program->stop();
        std::cout << "\n" << program->name() << ": Signal caught stopping...\n";
    }
}

void printInfo(const char *program) {
    std::cout << "To use: " << program << "\n\t-v print version info\n\t-i input directory\n\t-l filename.txt (list textfile)\n\t-r \"search with regular expression\"\n\t-m \"match with regular expression\"\n\t-t input file list in text file\n\t-o omakeutput video file mov\n\t-w frame width\n\t-h frame height\n\t-f frames per second\n\t-s stretch image (if not set will resize to keep aspect ratio)\n\t-n do not sort list of files..\n\t-q quiet mode\n\t-I file for images to be extracted\n\t-L file output prefix for file extraction\n";
}

int main(int argc, char **argv) {
    if(argc > 1) {
        int opt;
        std::string dir_name, file_name, file_prefix;
        double fps = 0;
        unsigned int width = 0, height = 0;
        bool no_sort = false;
        bool stretch = false;
        bool output_text = false;
        bool quiet = false;
        bool extract_images = false;
        std::string output_text_name;
        std::string text_file;
        std::string expr;
        std::string match_str;
        std::string extract_file_name;
        while((opt = getopt(argc, argv, "i:o:w:h:f:svnt:r:m:l:qL:I:")) != -1) {
            switch(opt) {
                case 'v':
                    std::cout << argv[0] << " v" << IMG2MOV_VERSION << " written by Jared Bruni\nsite: http://lostsidedead.com\nemail: lostjared@lostsidedead.com\n";
                    exit(EXIT_SUCCESS);
                    break;
                case 'r':
                    expr = optarg;
                    break;
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
                case 't':
                    text_file = optarg;
                    break;
                case 's':
                    stretch = true;
                    break;
                case 'n':
                    no_sort = true;
                    break;
                case 'l':
                    output_text_name = optarg;
                    output_text = true;
                    break;
                case 'm':
                    match_str = optarg;
                    break;
                case 'q':
                    quiet = true;
                    break;
                case 'I':
                    extract_file_name = optarg;
                    break;
                case 'L':
                    extract_images = true;
                    file_prefix = optarg;
                    break;
                default:
                    std::cout << argv[0] << ": invalid option...\n";
                    printInfo(argv[0]);
                    exit(0);
                    break;
            }
        }
        try {
            
            if(extract_file_name.length() > 0 && extract_images == true) {
                std::cout << "Extracting Images from: " << extract_file_name << " ...\n";
                video_tool::img2mov::extractImagesFromFile(extract_file_name, file_prefix);
            } else  if(output_text == false && dir_name.length() == 0 && text_file.length() == 0) {
                std::cerr << argv[0] << ": Requires either directory -i or text file -t or file output mode -l\n";
                printInfo(argv[0]);
                exit(EXIT_FAILURE);
            } else if((output_text == true && expr.length()==0 && match_str.length()==0)) {
                std::cout << argv[0] << ": output list mode requires -i -r or -m arguments.\n";
                printInfo(argv[0]);
                exit(EXIT_FAILURE);
            } else if((output_text == true && dir_name.length() > 0) && (expr.length() > 0 || match_str.length()>0)) {
                program.reset(new video_tool::img2mov(argv[0], dir_name, no_sort));
                program->setOutputList(output_text_name);
                program->setRegEx(expr);
                program->setRegExMatch(match_str);
                program->setQuiet(quiet);
                program->output();
            } else if(width == 0  || height == 0 || fps == 0 || (dir_name.length() == 0 && text_file.length() == 0) || file_name.length() == 0) {
                std::cerr << argv[0] << ": Requires input/output flags..\n";
                printInfo(argv[0]);
                exit(EXIT_FAILURE);
            } else {
                struct sigaction sa;
                sigemptyset(&sa.sa_mask);
                sa.sa_flags = 0;
                sa.sa_handler = control_Handler;
                if(sigaction(SIGINT, &sa, 0) == -1) {
                    std::cerr << argv[0] << ": Error on sigaction:\n";
                    exit(EXIT_FAILURE);
                }
                if(video_tool::img2mov::toLower(file_name).find(".mov") == std::string::npos && video_tool::img2mov::toLower(file_name).find(".avi") == std::string::npos)
                    file_name += ".mov";
                std::cout << argv[0] << ": Press Ctrl+C to stop processing...\n";
                program.reset(new video_tool::img2mov(argv[0], dir_name, file_name,fps,width,height,stretch, no_sort));
                if(text_file.length() > 0)
                    program->setList(text_file);
                if(expr.length() > 0)
                    program->setRegEx(expr);
                if(match_str.length() > 0)
                    program->setRegExMatch(match_str);
                
                program->setQuiet(quiet);
                program->run();
            }
        } catch(std::exception &e) {
            std::cerr << argv[0] << ": Exception: " << e.what() << "\n";
            exit(EXIT_FAILURE);
        } catch(...) {
            std::cerr << argv[0] << ": Error unknown exception...\n";
            exit(EXIT_FAILURE);
        }
    } else {
        std::cerr << argv[0] << ": Requires input/output flags..\n";
        printInfo(argv[0]);
        exit(EXIT_FAILURE);
    }
    return 0;
}

