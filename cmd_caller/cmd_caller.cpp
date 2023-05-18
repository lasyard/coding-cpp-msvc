#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "cmd_caller.h"

using namespace std;

const int CmdCaller::bufLen = 512;

CmdCaller::CmdCaller() : dwExitCode(0)
{
    buf = new char[bufLen];
}

CmdCaller::~CmdCaller()
{
    delete buf;
}

HRESULT CmdCaller::run(LPTSTR strCmd)
{
    SECURITY_ATTRIBUTES saAttr;
    BOOL bSuccess;
    HANDLE hStdoutWrite, hStdoutRead;
    HANDLE hStderrWrite, hStderrRead;
    STARTUPINFOW startInfo;
    PROCESS_INFORMATION procInfo;

    strStdout.clear();
    strStderr.clear();
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;
    if (!CreatePipe(&hStdoutRead, &hStdoutWrite, &saAttr, 0)) {
        return E_FAIL;
    }
    if (!CreatePipe(&hStderrRead, &hStderrWrite, &saAttr, 0)) {
        CloseHandle(hStdoutRead);
        CloseHandle(hStdoutWrite);
        return E_FAIL;
    }
    DWORD mode = PIPE_NOWAIT;
    SetNamedPipeHandleState(hStdoutRead, &mode, NULL, NULL);
    SetNamedPipeHandleState(hStderrRead, &mode, NULL, NULL);
    memset(&startInfo, 0, sizeof(startInfo));
    memset(&procInfo, 0, sizeof(procInfo));
    GetStartupInfo(&startInfo);
    startInfo.dwFlags = STARTF_USESTDHANDLES;
    startInfo.hStdInput = 0;
    startInfo.hStdOutput = hStdoutWrite;
    startInfo.hStdError = hStderrWrite;

    bSuccess = CreateProcess(
        NULL,
        strCmd,
        NULL,
        NULL,
        TRUE,
        CREATE_NO_WINDOW | NORMAL_PRIORITY_CLASS,
        NULL,
        NULL,
        &startInfo,
        &procInfo
    );
    if (!bSuccess) {
        CloseHandle(hStdoutRead);
        CloseHandle(hStdoutWrite);
        CloseHandle(hStderrRead);
        CloseHandle(hStderrWrite);
        return E_FAIL;
    }
    DWORD dwRead;
    BOOL bExit = FALSE;
    HANDLE hEvents[3];
    hEvents[0] = procInfo.hProcess;
    hEvents[1] = hStdoutRead;
    hEvents[2] = hStderrRead;
    while (!bExit) {
        switch (WaitForMultipleObjects(3, hEvents, FALSE, INFINITE)) {
        case WAIT_OBJECT_0 + 0:
            GetExitCodeProcess(procInfo.hProcess, &dwExitCode);
            CloseHandle(procInfo.hThread);
            CloseHandle(procInfo.hProcess);
            bExit = TRUE;
            // don't put break here, for there may be mutiple objects signaled simultaneously
        case WAIT_OBJECT_0 + 1:
            while (bSuccess = ReadFile(hStdoutRead, buf, bufLen - 1, &dwRead, NULL)) {
                buf[dwRead] = '\0';
                strStdout += buf;
            }
            // don't put break here, for there may be mutiple objects signaled simultaneously
        case WAIT_OBJECT_0 + 2:
            while (bSuccess = ReadFile(hStderrRead, buf, bufLen - 1, &dwRead, NULL)) {
                buf[dwRead] = '\0';
                strStderr += buf;
            }
        default:
            break;
        }
    }
    CloseHandle(hStdoutRead);
    CloseHandle(hStdoutWrite);
    CloseHandle(hStderrRead);
    CloseHandle(hStderrWrite);
    return S_OK;
}
