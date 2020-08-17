#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <ios>
#include <set>
#include <iostream>
#include <cstring>

class treeNode {
public:
    treeNode *left, *right;
    int count;
    bool terminate;
    char symbol;
    treeNode() {
        left = right = nullptr;
        terminate = false;
        count = 0;
    }
};

class compare {
public:
    bool operator() (const treeNode * a, const treeNode* b)  const{
        if (a -> count < b -> count)
            return true;
        else if (a -> symbol < b -> symbol)
            return true;
        return false;
    }
};

void getCodes(treeNode* root, std::vector<bool> code[256], std::vector<bool>& path) {
    if (root -> terminate) {
        int num = ((unsigned int)root -> symbol);
        if (num < 0)
        	num = 127 - num;
        code[num] = path;
    }
    else {
        path.push_back(false);
        getCodes(root -> left, code, path);
        path.pop_back();
        path.push_back(true);
        getCodes(root -> right, code, path);
        path.pop_back();
    }
    return;
}

void compress(std::string inputFile, std::string outputFile, const std::string name) {
    std::cerr << "Opened" << name << "\n";
    unsigned char temporary;
    long long cnt[256];
    int counte;
    treeNode * node, *least1, *least2;
    std::multiset<treeNode*, compare> haffList;
    std::vector<bool> code[256];
    bool used[256];

    //initialisation
    counte  = 0;
    std::fill(cnt, cnt + 255, 0);
    std::fill(used, used + 255, 0);

    //files
    std::ifstream input(inputFile, std::ios::binary);

    std::cerr << inputFile;

    if (!input.is_open()) {
        std::cerr << "Something went wrong...";
        return;
    }

    std::ofstream output(outputFile, std::ios::binary);
    if (!output.is_open()) {
        std::cerr << "Something went wrong...";
        return;
    }

	//counting
	while (input.read((char *)&temporary, sizeof(char))) {
        used[(unsigned int) temporary] = true;
        cnt[(unsigned int) temporary]++;
	}
	
	input.close();
	std::cerr << "Opened" << name << "\n";
	//building list to sort
	for (int i = 0; i < 256; i++) {
	    if (used[i]) {
	        ++counte;
	        node = new treeNode();
	        node -> symbol = (char)i;
	        node -> terminate = true;
	        node -> count = cnt[i];
	        haffList.insert(node);
	    }
	}
	
	//bulding Haffman tree
	for (int i = 0; i < counte - 1; ++i) {
	    least1 = *haffList.begin();
	    haffList.erase(haffList.begin());
        least2 = *haffList.begin();
        haffList.erase(haffList.begin());

        node = new treeNode();
        node -> left = least1;
        node -> right = least2;
        node -> count = least1 -> count + least2 -> count;
        node -> symbol = (char)127;
        haffList.insert(node);
	}
	std::vector<bool> pathh = {};
	getCodes(*haffList.begin(), code, pathh);
    std::ifstream input2(inputFile, std::ios::binary);


    //making output file
    output << (unsigned char)name.size();
    for (int i = 0; i < name.size(); i++)
        output << name[i];
    unsigned int blockscount =  0u;
    long long rawOutputLength = 0ll;
    for (int i = 0; i < 256; i++)
        if (used[i])
            blockscount++;
    output << (unsigned char)(blockscount>>8) << (unsigned char)blockscount;
    for (int i = 0; i < 256; i++) {
        if (!used[i])
            continue;
        rawOutputLength += ((long long)cnt[i])*((long long)code[i].size());
        output << (unsigned char)i << (unsigned char)code[i].size();
        unsigned char outchar = 0u;
        int counter = 0;
        for (int j = 0; j < code[i].size(); j++) {
            outchar <<= 1;
            if (code[i][j]) outchar |= 1u;
            counter++;
            if (counter == 8) {
                output << (unsigned char)outchar;
                counter = 0;
            }
        }
        if (counter != 0) {
            output << (unsigned char)(outchar << (8 - counter));
        }
    }
    output << (unsigned char)(rawOutputLength >> 56) << (unsigned char)(rawOutputLength >> 48) << (unsigned char)(rawOutputLength >> 40) << (unsigned char)(rawOutputLength >> 32) 
           << (unsigned char)(rawOutputLength >> 24) << (unsigned char)(rawOutputLength >> 16) << (unsigned char)(rawOutputLength >> 8) << (unsigned char)(rawOutputLength >> 0);
    unsigned char temporaryChar = 0u, inputc;
    unsigned char outchar = 0u;
    int counter = 0;
    while (input2.read((char *)&inputc, sizeof(char))) {
        for (int j = 0; j < code[inputc].size(); j++) {
            outchar <<= 1;

            if (code[inputc][j]) outchar |= 1u;
            counter++;
            if (counter == 8) {
                output << (unsigned char)outchar;
                counter = 0;
            }
        }
    }
    if (counter != 0) {
        output << (unsigned char)(outchar << (8 - counter));
    }
	input2.close();
	output.close();
	std::cerr << "Closed" << name << "\n";

    return;
}

int main(int argc, const char *argv[])
{
    std::string argv1 = std::string(argv[1]);
    if (argc != 2)
        return 0;
    std::string path = "", name = "";
    int nlength = argv1.size(), it;
    std::cerr << argv1 << "\n";
    for (it = nlength - 1; it >= 0; --it) {
        if (argv[1][it] == '\\' || argv[1][it] == '/')
            break;
    }
    ++it;
    for (int i = 0; i  < it; i++) {
        path.push_back(argv1[i]);
    }
    while (it < nlength && argv1[it] != '.') {
        path.push_back(argv1[it]);
        name.push_back(argv1[it]);
        ++it;
    }
    path.push_back('.');
    path.push_back('a');
    path.push_back('r');
    path.push_back('h');

    while (it < nlength) {
        name.push_back(argv1[it]);
        ++it;
    }

    compress(argv1 , path, name);
    return 0;
}