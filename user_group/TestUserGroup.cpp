#include <tchar.h>

#include <iostream>

#include "UserGroup.h"

int _tmain(int argc, _TCHAR *argv[])
{
    UserGroup ug;

    if (!ug.cacheUserGroupInfo()) {
        return -1;
    }

    std::wcout << std::endl << L"Computer Name: " << ug.computerName() << std::endl;

    std::wcout << std::endl << L"System Accounts:" << std::endl;
    for (auto i : ug.systemAccounts()) {
        std::wcout << i.domain << L'\\' << i.name << std::endl;
    }

    std::wcout << std::endl << L"User Accounts:" << std::endl;
    for (auto i : ug.userAccounts()) {
        std::wcout << i.domain << L'\\' << i.name << std::endl;
    }

    std::wcout << std::endl << L"Groups:" << std::endl;
    for (auto i : ug.groups()) {
        std::wcout << i.domain << L'\\' << i.name << std::endl;
    }

    std::wcout << std::endl << L"Group User Relationship:" << std::endl;
    for (auto i : ug.userGroups()) {
        std::wcout << i.first.domain << L'\\' << i.first.name << L" is in Group:" << std::endl;
        std::wcout << L'\t';
        for (auto j : i.second) {
            std::wcout << j.domain << L'\\' << j.name << L", ";
        }
        std::wcout << std::endl;
    }

    String user = L"Administrator";
    String group = L"Administrators";

    if (ug.hasLocalUser(user)) {
        std::wcout << L"User " << user << L" exists" << std::endl;
    }
    if (ug.hasLocalGroup(group)) {
        std::wcout << L"Group " << group << L" exists" << std::endl;
    }
    if (ug.isLocalUserInGroup(user, group)) {
        std::wcout << L"User " << user << L" is in Group " << group << std::endl;
    }

    return 0;
}
