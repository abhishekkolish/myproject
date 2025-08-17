#include <iostream>
#include <fstream>
#include <dirent.h>
#include <elf.h>
#include <cstring>
#include <vector>
#include <sys/stat.h>

using namespace std;

struct LibInfo {
    string name;
    string arch;
};

string GetArchitectureType(const string &filePath) {
    ifstream file(filePath, ios::binary);
    if (!file) return "Unknown";

    unsigned char e_ident[EI_NIDENT];
    file.read((char *)e_ident, EI_NIDENT);
    if (!file || e_ident[0] != 0x7f || e_ident[1] != 'E' ||
        e_ident[2] != 'L' || e_ident[3] != 'F') {
        return "Not ELF";
    }

    file.seekg(0, ios::beg);

    uint16_t machine = 0;
    if (e_ident[EI_CLASS] == ELFCLASS32) {
        Elf32_Ehdr header;
        file.read((char *)&header, sizeof(header));
        machine = header.e_machine;
    } else if (e_ident[EI_CLASS] == ELFCLASS64) {
        Elf64_Ehdr header;
        file.read((char *)&header, sizeof(header));
        machine = header.e_machine;
    }

    switch (machine) {
        case EM_ARM: return "armeabi-v7a";
        case EM_AARCH64: return "arm64-v8a";
        case EM_386: return "x86";
        case EM_X86_64: return "x86-64";
        case EM_MIPS: return "mips";
        default: return "Unknown";
    }
}

bool IsSharedLibrary(const string &filename) {
    return filename.size() > 3 &&
           filename.substr(filename.size() - 3) == ".so";
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <folder_path>\n";
        return 1;
    }

    string folderPath = argv[1];
    DIR *dir = opendir(folderPath.c_str());
    if (!dir) {
        cerr << "Error: Cannot open directory " << folderPath << "\n";
        return 1;
    }

    struct dirent *entry;
    vector<LibInfo> libs;

    while ((entry = readdir(dir)) != nullptr) {
        string filename = entry->d_name;
        if (filename == "." || filename == "..") continue;

        string fullPath = folderPath + "/" + filename;
        struct stat s;
        if (stat(fullPath.c_str(), &s) == 0 && S_ISREG(s.st_mode)) {
            if (IsSharedLibrary(filename)) {
                string arch = GetArchitectureType(fullPath);
                if (arch != "Not ELF") {
                    libs.push_back({filename, arch});
                }
            }
        }
    }
    closedir(dir);

    cout << "Total number of libraries: " << libs.size() << "\n\n";
    cout << "File\tArchType\n";
    cout << "====\t========\n";
    for (auto &lib : libs) {
        cout << lib.name << "\t" << lib.arch << "\n";
    }

    return 0;
}
