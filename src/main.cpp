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
        bool stretch = false;
        while((opt = getopt(argc, argv, "i:o:w:h:f:sv")) != -1) {
            switch(opt) {
                case 'v':
                    std::cout << argv[0] << " written by Jared Bruni\nhttp://lostsidedead.com\n";
                    exit(EXIT_SUCCESS);
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
        struct sigaction sa;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sa.sa_handler = control_Handler;
        if(sigaction(SIGINT, &sa, 0) == -1) {
            std::cerr << "Error on sigaction:\n";
            exit(EXIT_FAILURE);
        }
        if(toLower(file_name).find("mov") == std::string::npos)
            file_name += ".mov";
        std::cout << "img2mov: Press Ctrl+C to stop processing...\n";
        program.reset(new img2mov(dir_name, file_name,fps,width,height,stretch));
        program->run();
    } else {
        std::cerr << "Requires input/output flags..\n";
        std::cerr << argv[0] << " -i directory -o video -f fps -w width -h height\n";
        exit(EXIT_FAILURE);
    }
    return 0;
}

