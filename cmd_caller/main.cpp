#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <tchar.h>

#include <iostream>

#include "cmd_caller.h"

int main(int argc, char *argv[])
{
    CmdCaller caller;
    TCHAR cmd[] = _T("cmd /c dir");
    if (caller.run(cmd) == S_OK) {
        std::cout << "Cmd exit with code " << caller.getExitCode() << "." << std::endl;
        std::cout << std::endl << "---- The stdout output is:" << std::endl << std::endl;
        std::cout << caller.getStdout();
        std::cout << std::endl << "---- The stderr output is:" << std::endl << std::endl;
        std::cout << caller.getStderr();
    } else {
        std::cout << "Run cmd failed, errno = " << GetLastError() << std::endl;
    }
    return 0;
}
