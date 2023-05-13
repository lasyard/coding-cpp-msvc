#include <fstream>
#include <iostream>
#include <string>

#include <file_lib.h>

using namespace std;

static int g_searchLine = 1;
static string g_searchString;
static string g_destPath;

static void showHelp()
{
    cout << "Move text files satisfying a specified rule to a specified path." << endl
         << "Usage: file_filter [-l <lineNo>] -s <String> <path>\n"
         << "-l number of line to find the string, default 1\n"
         << "-s the string\n";
}

static void searchFile();

static void moveFile(const char *fileName)
{
    if (rename(fileName, (g_destPath + "\\" + fileName).c_str()) != 0) {
        cerr << "Cannot move file \"" << fileName << "\" to \"" << g_destPath << "\"." << endl;
    }
}

static void processFile(const char *fileName)
{
    string buf;
    ifstream file;
    file.open(fileName, ios::in);
    if (!file.is_open()) {
        cerr << "Cannot open the file \"" << fileName << "\"." << endl;
        return;
    }
    for (int i = 0; i < g_searchLine; i++) {
        getline(file, buf);
    }
    file.close();
    if (buf.find(g_searchString) == string::npos) {
        cout << "Skipped." << endl;
        return;
    }
    cout << "Match found. Move file to \"" << g_destPath << "\"." << endl;
    moveFile(fileName);
}

static void processSubDir(const struct _finddata_t &file)
{
    if (is_dir(&file)) {
        if (strcmp(file.name, ".") != 0 && strcmp(file.name, "..") != 0) {
            if (do_chdir(file.name)) {
                cout << "Change to directory \"" << file.name << "\"." << endl;
                searchFile();
                if (do_chdir("..")) {
                    cout << "Change to upper directory." << endl;
                }
            }
        }
    } else {
        const char *p = strrchr(file.name, '.');
        if (p != nullptr && strcmp(p + 1, "txt") == 0) {
            cout << file.name << ": ";
            processFile(file.name);
        }
    }
}

static void searchFile()
{
    struct _finddata_t file;
    intptr_t hFile;
    hFile = _findfirst("*", &file);
    if (hFile == -1L) {
        cout << "No files in current directory." << endl;
    } else {
        processSubDir(file);
        while (_findnext(hFile, &file) == 0) {
            processSubDir(file);
        }
    }
    _findclose(hFile);
}

int main(int argc, char *argv[])
{
    bool getPath = false;
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
            case 'l':
            case 'L':
                ++i;
                g_searchLine = atoi(argv[i]);
                break;
            case 's':
            case 'S':
                ++i;
                g_searchString = argv[i];
                break;
            case '?':
                showHelp();
                return 0;
            default:
                cerr << "Invalid option \"-" << argv[i][1] << "\"." << endl;
                showHelp();
                return -1;
            }
        } else if (!getPath) {
            g_destPath = argv[i];
            getPath = true;
        }
    }
    if (g_searchString.empty() || !getPath) {
        cerr << "Must specify the searching string and destination path." << endl;
        cout << endl;
        showHelp();
        return -1;
    }
    searchFile();
    return 0;
}
