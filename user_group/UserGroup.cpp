#include <regex>

#include "UserGroup.h"

#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "comsuppw.lib")

UserGroup::UserGroup() : m_pWbemServices(NULL), m_userGroupInfoCached(false)
{
    HRESULT hRes;
    hRes = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (SUCCEEDED(hRes)) {
        m_pWbemServices = connectServer();
    }
}

UserGroup::~UserGroup()
{
    if (m_pWbemServices != NULL) {
        m_pWbemServices->Release();
    }
    CoUninitialize();
}

IWbemServices *UserGroup::connectServer()
{
    HRESULT hRes;
    IWbemServices *pWbemServices = NULL;
    IWbemLocator *pIWbemLocator = NULL;

    hRes = CoCreateInstance(
        CLSID_WbemAdministrativeLocator,
        NULL,
        CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER,
        IID_IUnknown,
        (void **)&pIWbemLocator
    );
    if (FAILED(hRes)) {
        return NULL;
    }

    hRes = pIWbemLocator->ConnectServer(
        _bstr_t(L"root\\CIMV2"),        // Namespace
        NULL,                           // Userid
        NULL,                           // PW
        NULL,                           // Locale
        WBEM_FLAG_CONNECT_USE_MAX_WAIT, // flags
        NULL,                           // Authority
        NULL,                           // Context
        &pWbemServices
    );
    pIWbemLocator->Release();
    if (FAILED(hRes)) {
        return NULL;
    }

    hRes = CoSetProxyBlanket(
        pWbemServices,
        RPC_C_AUTHN_WINNT,
        RPC_C_AUTHZ_NONE,
        NULL,
        RPC_C_AUTHN_LEVEL_CALL,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL,
        EOAC_NONE
    );
    if (FAILED(hRes)) {
        pWbemServices->Release();
        return NULL;
    }
    return pWbemServices;
}

HRESULT UserGroup::enumInstance(
    const _bstr_t &className,
    bool (*processClassObject)(IWbemClassObject *, void *context),
    void *context
)
{
    HRESULT hRes;
    IEnumWbemClassObject *pEnumWbemClassObject;

    hRes = m_pWbemServices->CreateInstanceEnum(className, 0, NULL, &pEnumWbemClassObject);
    if (FAILED(hRes)) {
        return hRes;
    }

    IWbemClassObject *pWbemClassObject;
    ULONG ulReturned = 0;
    for (;;) {
        hRes = pEnumWbemClassObject->Next(
            1000,              // timeout
            1,                 // return just one
            &pWbemClassObject, //
            &ulReturned        // number obtained
        );
        if (FAILED(hRes)) {
            break;
        }
        if (ulReturned == 0) {
            pWbemClassObject->Release();
            break;
        }
        if (!processClassObject(pWbemClassObject, context)) {
            pWbemClassObject->Release();
            break;
        }
    }
    pEnumWbemClassObject->Release();
    return S_OK;
}

HRESULT UserGroup::cacheComputerName()
{
    // In C++11, lambda with captures cannot be converted to function pointer
    HRESULT hRes = enumInstance(
        COMPUTER_SYSTEM_CLASS,
        [](IWbemClassObject *obj, void *context) -> bool {
            _variant_t name;
            HRESULT hRes = obj->Get(_bstr_t(L"Name"), 0L, &name, NULL, NULL);
            auto pComputerName = (String *)context;
            *pComputerName = _bstr_t(name);
            return false;
        },
        (void *)&m_computerName
    );
    return hRes;
}

HRESULT UserGroup::cacheAccount(const _bstr_t &cls, std::vector<struct Account> &var)
{
    HRESULT hRes = enumInstance(
        cls,
        [](IWbemClassObject *obj, void *context) -> bool {
            _variant_t domain, name;
            HRESULT hRes = obj->Get(_bstr_t(L"Domain"), 0L, &domain, NULL, NULL);
            hRes = obj->Get(_bstr_t(L"Name"), 0L, &name, NULL, NULL);
            auto accounts = (std::vector<struct Account> *)context;
            accounts->push_back(Account(_bstr_t(domain), _bstr_t(name)));
            return true;
        },
        (void *)&var
    );
    return hRes;
}

HRESULT UserGroup::cacheUserGroup()
{
    HRESULT hRes = enumInstance(
        GROUP_USER_CLASS,
        [](IWbemClassObject *obj, void *context) -> bool {
            _variant_t group, user;
            HRESULT hRes = obj->Get(_bstr_t(L"GroupComponent"), 0L, &group, NULL, NULL);
            hRes = obj->Get(_bstr_t(L"PartComponent"), 0L, &user, NULL, NULL);
            std::wregex re(L"Domain=\"(.*)\",Name=\"(.*)\"");
            std::wsmatch sm;
            // The searched string cannot be temporary.
            String strUser{_bstr_t(user)};
            String strGroup{_bstr_t(group)};
            String userDomain, userAccount, groupDomain, groupName;
            if (std::regex_search(strUser, sm, re)) {
                userDomain = sm[1].str();
                userAccount = sm[2].str();
            }
            if (std::regex_search(strGroup, sm, re)) {
                groupDomain = sm[1];
                groupName = sm[2];
            }
            auto userGroup = (std::map<std::pair<String, String>, std::vector<std::pair<String, String>>> *)context;
            auto key = std::make_pair(userDomain, userAccount);
            if (userGroup->find(key) == userGroup->end()) {
                (*userGroup)[key] = std::vector<std::pair<String, String>>();
            }
            (*userGroup)[key].push_back(std::make_pair(groupDomain, groupName));
            return true;
        },
        (void *)&m_userGroups
    );
    return hRes;
}
