#include <fstream>
#include <string>
#include <vector>
#include <ios>
#include <algorithm>
#include <set>
#include <string.h>
#include <iostream>
#include <map>

class treeNode {
public:
    treeNode *left, *right;
    int count;
    bool terminate;
    char symbol;
    treeNode(){
        left = right = nullptr;
        terminate = false;
        count = 0;
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2)
        return 0;
    int nlength = strlen(argv[1]);
    if (nlength < 5 || argv[1][nlength - 1] != 'h' || argv[1][nlength - 2] != 'r' || argv[1][nlength - 3] != 'a' || argv[1][nlength - 4] != '.') {
        return 0;
    }
    std::ifstream input(argv[1], std::ios::binary);

    unsigned char inputuchar;
    char inputchar;

    if (!input.read((char *)&inputuchar, sizeof(char))) {
        return 0;
    }

    std::string outputname = "";
    int it;
    char argv1[nlength];
    for (int i = 0; i < nlength; i++)
        argv1[i] = argv[1][i];
    for (it = nlength - 1; it >= 0; --it) {
        if (argv[1][it] == '\\' || argv[1][it] == '/')
            break;
    }
    ++it;
    for (int i = 0; i  < it; i++) {
        outputname.push_back(argv[1][i]);
    }

    unsigned int outputnamesize = (unsigned int)inputuchar;
    if (outputnamesize < 0) outputnamesize = 127 - outputnamesize;
    for (int i = 0; i < outputnamesize; i++) {
        if (!input.read((char *)&inputchar, sizeof(char))) {
            return 0;
        }
        outputname.push_back(inputchar);
    }

    std::ofstream output(outputname, std::ios::binary);
    if (!output)
        return 0;



    treeNode* root = new treeNode();
    root -> left = root;
    root -> right = root;
    treeNode* temproot = root;
    unsigned int numofsymb = 0, temp = 0, blocksize = 0u;
    int blocknum;
    char symbolourselves;



    if (!input.read((char *)&inputuchar, sizeof(char))) {
        std::cerr << "Something went wrong...";
        return 0;
    }
    temp = (unsigned int)inputuchar;
    numofsymb<<=8;
    numofsymb|=temp;
    if (!input.read((char *)&inputuchar, sizeof(char))) {
        std::cerr << "Something went wrong...";
        return 0;
    }
    temp = (unsigned int)inputuchar;
    numofsymb<<=8;
    numofsymb|=temp;
    for (unsigned int i = 0u; i < numofsymb; ++i) {
        temproot = root;
        if (!input.read((char *)&symbolourselves, sizeof(char))) {
            std::cerr << "Something went wrong...";
            return 0;
        }
        if (!input.read((char *)&inputuchar, sizeof(char))) {
            std::cerr << "Something went wrong...";
            return 0;
        }
        temp = (unsigned int) inputuchar;

        while (temp > 0u) {
            if (!input.read((char *)&inputuchar, sizeof(char))) {
                std::cerr << "Something went wrong...";
                return 0;
            }
            int iter = 7;
            while (temp > 0u && iter >= 0) {
                if ((inputuchar>>iter)&1) {
                    if (temproot -> right == root) {
                        temproot -> right = new treeNode();
                        temproot -> right -> right = temproot -> right -> left = root;
                    }
                    temproot = temproot -> right;
                } else {
                    if (temproot -> left == root) {
                        temproot -> left = new treeNode();
                        temproot -> left -> right = temproot -> left -> left = root;
                    }
                    temproot = temproot -> left;
                }
                --temp;
                --iter;
            }
        }
        temproot -> terminate = true;
        temproot -> symbol = symbolourselves;
    }

    unsigned long long rawdatasize = 0ull, rawtemp = 0ull;
    for (int i = 0; i < 8; i++) {
        if (!input.read((char *)&inputuchar, sizeof(char))) {
            std::cerr << "Something went wrong...";
            return 0;
        }
        rawtemp = (unsigned long long)inputuchar;
        rawdatasize<<=8;
        rawdatasize|=rawtemp;
    }

    while (rawdatasize > 0ull) {
        if (!input.read((char *)&inputuchar, sizeof(char))) {
            std::cerr << "Something went wrong...";
            return 0;
        }
        int iter = 7;
        while (rawdatasize > 0ull && iter >= 0) {
            if ((inputuchar>>iter)&1) {
                root = root -> right;
            } else {

                root = root -> left;
            }
            if (root->terminate) {
                output << root -> symbol;
                root = root -> left;
            }

            --rawdatasize;
            --iter;
        }
    }
    input.close();
    output.close();
    return 0;
}
