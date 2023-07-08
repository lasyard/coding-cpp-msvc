#pragma once

#include <list>

using namespace std;

class CCamera;

class CCameraManager
{
public:
    CCameraManager(void);
    virtual ~CCameraManager(void);

    const list<CCamera *> &getCamList();
    CCamera *findCamById(int camId);
    CCamera *findCamByPath(LPCTSTR path);
    void notifyDevChanged();

protected:
    list<CCamera *> m_camList;
    int m_camId;
    bool m_devChanged;

    ICreateDevEnum *m_pCreateDevEnum;

    void clearCamList();
    void initEnumDevices();
    void releaseEnumDevices();
    void enumCams();
};
