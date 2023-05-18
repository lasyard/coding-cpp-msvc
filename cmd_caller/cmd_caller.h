#ifndef _CMD_CALLER_H_
#define _CMD_CALLER_H_

#include <string>

class CmdCaller
{
public:
    CmdCaller();
    virtual ~CmdCaller();

    HRESULT run(LPTSTR strCmd);

    const std::string &getStdout() const
    {
        return strStdout;
    }

    const std::string &getStderr() const
    {
        return strStderr;
    }

    DWORD getExitCode() const
    {
        return dwExitCode;
    }

protected:
    static const int bufLen;
    char *buf;
    std::string strStdout;
    std::string strStderr;
    DWORD dwExitCode;
};

#endif
