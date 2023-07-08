#include "stdafx.h"

#include <dshow.h>

#include "CameraManager.h"

#include "Camera.h"
#include "resource.h"

CCameraManager::CCameraManager(void) : m_camId(0)
{
    if (CoInitializeEx(NULL, COINIT_APARTMENTTHREADED) != S_OK) {
        AfxMessageBox(IDP_COM_INIT_FAILED);
    }
    initEnumDevices();
}

CCameraManager::~CCameraManager(void)
{
    clearCamList();
    releaseEnumDevices();
    CoUninitialize();
}

const list<CCamera *> &CCameraManager::getCamList()
{
    if (m_devChanged) {
        enumCams();
    }
    return m_camList;
}

void CCameraManager::notifyDevChanged()
{
    m_devChanged = true;
}

void CCameraManager::clearCamList()
{
    list<CCamera *>::iterator ipCam;
    for (ipCam = m_camList.begin(); ipCam != m_camList.end(); ++ipCam) {
        delete *ipCam;
    }
    m_camList.clear();
}

CCamera *CCameraManager::findCamById(int camId)
{
    list<CCamera *>::iterator ipCam;
    for (ipCam = m_camList.begin(); ipCam != m_camList.end(); ++ipCam) {
        if ((*ipCam)->id == camId) {
            return *ipCam;
        }
    }
    return NULL;
}

CCamera *CCameraManager::findCamByPath(LPCTSTR path)
{
    list<CCamera *>::iterator ipCam;
    for (ipCam = m_camList.begin(); ipCam != m_camList.end(); ++ipCam) {
        if ((*ipCam)->path == path) {
            return *ipCam;
        }
    }
    return NULL;
}

void CCameraManager::initEnumDevices()
{
    m_devChanged = true;
    m_pCreateDevEnum = NULL;
    if (FAILED(CoCreateInstance(
            CLSID_SystemDeviceEnum,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_ICreateDevEnum,
            reinterpret_cast<void **>(&m_pCreateDevEnum)
        ))) {
        ATLTRACE2("CoCreateInstance(CLSID_SystemDeviceEnum) failed.\n");
    }
}

void CCameraManager::releaseEnumDevices()
{
    if (m_pCreateDevEnum != NULL) {
        m_pCreateDevEnum->Release();
    }
}

void CCameraManager::enumCams()
{
    list<CCamera *>::iterator ipCam, ipCam1;
    for (ipCam = m_camList.begin(); ipCam != m_camList.end(); ++ipCam) {
        (*ipCam)->exist = false;
    }

    IEnumMoniker *pEnumMoniker;
    HRESULT hr;
    if (FAILED(hr = m_pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumMoniker, 0))) {
        ATLTRACE2("CreateClassEnumerator(CLSID_VideoInputDeviceCategory) failed.\n");
        goto enumEnd;
    } else if (hr == S_FALSE) {
        goto enumEnd;
    }

    IMoniker *pMoniker;
    while (pEnumMoniker->Next(1, &pMoniker, NULL) == S_OK) {
        IPropertyBag *pPropertyBag;
        if (FAILED(pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)(&pPropertyBag)))) {
            pMoniker->Release();
            continue;
        }
        VARIANT varName, varDesc, varPath;
        CString name, desc, path;
        VariantInit(&varName);
        VariantInit(&varDesc);
        VariantInit(&varPath);
        if (FAILED(pPropertyBag->Read(_T("FriendlyName"), &varName, NULL))) {
            ATLTRACE2("Read camera name failed.\n");
        } else {
            name = varName.bstrVal;
        }
        if (FAILED(pPropertyBag->Read(_T("Description"), &varDesc, NULL))) {
            ATLTRACE2("Read camera description failed.\n");
        } else {
            desc = varDesc.bstrVal;
        }
        if (FAILED(pPropertyBag->Read(_T("DevicePath"), &varPath, NULL))) {
            ATLTRACE2("Read camera device path failed.\n");
        } else {
            path = varPath.bstrVal;
        }
        CCamera *pCam;
        if ((pCam = findCamByPath(path)) == NULL) {
            pCam = new CCamera(m_camId++, name, desc, path);
            if (m_camId > 99) {
                m_camId = 0;
            }
            pCam->exist = true;
            m_camList.push_back(pCam);
            if (FAILED(pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, reinterpret_cast<void **>(&pCam->filter)))) {
                ATLTRACE2("BindToObject(IID_IBaseFilter) failed.\n");
            }
        } else {
            pCam->exist = true;
        }
        VariantClear(&varName);
        VariantClear(&varDesc);
        VariantClear(&varPath);
        pPropertyBag->Release();
        pMoniker->Release();
    }
    pEnumMoniker->Release();

enumEnd:
    for (ipCam = m_camList.begin(); ipCam != m_camList.end(); ipCam = ipCam1) {
        ipCam1 = ipCam;
        ++ipCam1;
        if (!(*ipCam)->exist) {
            delete (*ipCam);
            m_camList.remove(*ipCam);
        }
    }
}
