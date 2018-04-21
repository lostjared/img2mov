#include "img2mov.hpp"
#include<memory>

std::unique_ptr<img2mov> program;

void control_Handler(int sig) {
    if(program)
        program->stop();
    std::cout << "\nimg2mov: Signal caught stopping...\n";
}

int main(int argc, char **argv) {
    if(argc > 1) {
        int opt;
        std::string dir_name, file_name;
        double fps = 0;
        unsigned int width = 0, height = 0;
        bool no_sort = false;
        bool stretch = false;
        std::string text_file;
        std::string expr;
        while((opt = getopt(argc, argv, "i:o:w:h:f:svnt:r:")) != -1) {
            switch(opt) {
                case 'v':
                    std::cout << argv[0] << " v" << IMG2MOV_VERSION << " written by Jared Bruni\nhttp://lostsidedead.com\n";
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
                default:
                    std::cout << "img2mov: invalid option...\n";
                    exit(0);
                    break;
            }
        }
        if(width == 0  || height == 0 || fps == 0 || (dir_name.length() == 0 && text_file.length() == 0) || file_name.length() == 0) {
            std::cerr << "img2mov: Requires input/output flags..\n";
            std::cerr << argv[0] << " -i directory -o video -w width -h height -f fps\n";
            exit(EXIT_FAILURE);
        }
        struct sigaction sa;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sa.sa_handler = control_Handler;
        if(sigaction(SIGINT, &sa, 0) == -1) {
            std::cerr << "img2mov: Error on sigaction:\n";
            exit(EXIT_FAILURE);
        }
        if(img2mov::toLower(file_name).find("mov") == std::string::npos)
            file_name += ".mov";
        std::cout << "img2mov: Press Ctrl+C to stop processing...\n";
        program.reset(new img2mov(dir_name, file_name,fps,width,height,stretch, no_sort));
        if(text_file.length() > 0)
            program->setList(text_file);
        if(expr.length() > 0)
            program->setRegEx(expr);
        try {
            program->run();
        } catch(std::exception &e) {
            std::cerr << "Exception: " << e.what() << "\n";
            exit(EXIT_FAILURE);
        }
    } else {
        std::cerr << "img2mov: Requires input/output flags..\n";
        std::cerr << argv[0] << " -i directory -o video -f fps -w width -h height\n";
        exit(EXIT_FAILURE);
    }
    return 0;
}

