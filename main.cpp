#include <getopt.h>
#include "bwted.h"

using namespace std;

int main(int argc, char ** argv) {
    std::cout << "Hello, World!" << std::endl;
    int c;
    ifstream input;
    ofstream output;
    ofstream log;
    char mode = 0;
    while ((c = getopt (argc, argv, "i:o:l:cxh")) != -1) {
        switch (c){
            case 'i':
                input = ifstream(optarg);
                if (!input.is_open()){
                    cout << "could not open input file";
                    exit(1);
                }
                break;
            case 'o':
                output = ofstream(optarg);
                if (!output.is_open()){
                    cout << "could not open output file";
                    exit(1);
                }
                break;
            case 'l':
                log = ofstream(optarg);
                break;
            case 'c':
                mode = 1;
                break;
            case 'x':
                mode = 2;
                break;
            case 'h':
                cout << "./bwted (-x|-c) [-i INPUT] [-o OUTPUT] [-l LOGOUT]\n"
                     << "   -x decode input\n"
                     << "   -c encode input\n"
                     << "   -i INPUT path to input file, stdin otherwise\n"
                     << "   -o OUTPUT path to output file, stdout otherwise\n"
                     << "   -l LOGOUT path to log file\n";
                return 0;
            default:
                cerr << "Invalid parameter\n";
                return 1;
        }
    }
    int retval = 0;
    tBWTED result;
    result.codedSize = 1;
    result.uncodedSize = 2;
    if (!mode){
        cerr << "One of (-x|-c) must be set. Please read help:\n"
             << "./bwted -h\n";
        return 1;
    } else if (mode == 1){
        retval = BWTEncoding(&result, input.is_open() ? input : cin, output.is_open() ? output : cout);
    } else {
        retval = BWTDecoding(&result, input.is_open() ? input : cin, output.is_open() ? output : cout);
    }
    if (log.is_open()){
        log << "login = xkondu00\n"
            << "uncodedSize = " << result.uncodedSize << "\n"
            << "codedSize = " << result.codedSize << "\n";
        log.close();
    }
    cout << "retval: " << retval << endl;
    return 0;
}

