#include "stdafx.h"

#include <dshow.h>

#include "Camera.h"

CCamera::CCamera(int i, LPCTSTR n, LPCTSTR d, LPCTSTR p)
    : id(i)
    , filter(NULL)
    , graphBuilder(NULL)
    , captureGraphBuilder2(NULL)
    , mediaControl(NULL)
    , videoWindow(NULL)
    , hwnd(NULL)
    , exist(false)
    , running(false)
    , width(320)
    , height(240)
    , name(n)
    , desc(d)
    , path(p)
{
}

CCamera::~CCamera(void)
{
    if (filter != NULL) {
        filter->Release();
    }
    endCapture();
    closeWindow();
}

bool CCamera::hasWindow() const
{
    return hwnd != NULL;
}

bool CCamera::isRunning() const
{
    return running;
}

void CCamera::setWindow(HWND h)
{
    hwnd = h;
}

void CCamera::setWindowFocus()
{
    if (hwnd != NULL) {
        ::SetFocus(hwnd);
    }
}

void CCamera::closeWindow()
{
    if (hwnd != NULL) {
        SendMessage(hwnd, WM_CLOSE, 0, 0);
    }
    hwnd = NULL;
}

void CCamera::startCapture()
{
    if (hwnd == NULL) {
        return;
    }
    if (!hasCaptureGraph()) {
        releaseCaptureGraph();
        initCaptureGraph();
        graphBuilder->AddFilter(filter, _T("BaseFilter"));
        captureGraphBuilder2->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, filter, NULL, NULL);
        getMediaInfo();
    }
    videoWindow->put_Owner((OAHWND)hwnd);
    videoWindow->put_WindowStyle(WS_CHILD);
    videoWindow->put_Visible(OATRUE);
    mediaControl->Run();
    running = true;
}

void CCamera::ajustWindowPos(int cx, int cy)
{
    if (hasCaptureGraph()) {
        int mx, my;
        if ((width << 10) / cx >= (height << 10) / cy) {
            mx = cx - 4;
            my = height * mx / width;
        } else {
            my = cy - 4;
            mx = width * my / height;
        }
        videoWindow->SetWindowPosition((cx - mx) / 2, (cy - my) / 2, mx, my);
    }
}

void CCamera::stopCapture()
{
    if (hasCaptureGraph()) {
        mediaControl->Stop();
        videoWindow->put_Owner(NULL);
        videoWindow->put_Visible(OAFALSE);
        running = false;
    }
}

void CCamera::endCapture()
{
    stopCapture();
    releaseCaptureGraph();
}

void CCamera::initCaptureGraph()
{
    if (FAILED(CoCreateInstance(
            CLSID_FilterGraph,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_IGraphBuilder,
            reinterpret_cast<void **>(&graphBuilder)
        ))) {
        ATLTRACE2("CoCreateInstance(CLSID_FilterGraph) failed.\n");
        return;
    }
    if (FAILED(CoCreateInstance(
            CLSID_CaptureGraphBuilder2,
            NULL,
            CLSCTX_INPROC,
            IID_ICaptureGraphBuilder2,
            reinterpret_cast<void **>(&captureGraphBuilder2)
        ))) {
        ATLTRACE2("CoCreateInstance(CLSID_CaptureGraphBuilder2) failed.\n");
        return;
    }

    captureGraphBuilder2->SetFiltergraph(graphBuilder);

    if (FAILED(graphBuilder->QueryInterface(IID_IMediaControl, reinterpret_cast<void **>(&mediaControl))) ||
        FAILED(graphBuilder->QueryInterface(IID_IVideoWindow, reinterpret_cast<void **>(&videoWindow)))) {
        ATLTRACE2("QueryInterface(IID_IMediaControl IID_IVideoWindow) failed.\n");
        return;
    }
}

bool CCamera::hasCaptureGraph()
{
    return captureGraphBuilder2 != NULL && graphBuilder != NULL && videoWindow != NULL && mediaControl != NULL;
}

void CCamera::releaseCaptureGraph()
{
    if (mediaControl != NULL) {
        mediaControl->Release();
        mediaControl = NULL;
    }
    if (videoWindow != NULL) {
        videoWindow->Release();
        videoWindow = NULL;
    }
    if (captureGraphBuilder2 != NULL) {
        captureGraphBuilder2->Release();
        captureGraphBuilder2 = NULL;
    }
    if (graphBuilder != NULL) {
        graphBuilder->Release();
        graphBuilder = NULL;
    }
}

void CCamera::getMediaInfo()
{
    if (filter == NULL) {
        return;
    }
    IEnumPins *enumPins;
    filter->EnumPins(&enumPins);
    IPin *pin;
    while (enumPins->Next(1, &pin, NULL) == S_OK) {
        AM_MEDIA_TYPE mt;
        ZeroMemory(&mt, sizeof(mt));
        pin->ConnectionMediaType(&mt);
        if (mt.majortype != MEDIATYPE_Video) {
            pin->Release();
            continue;
        }
        if (mt.cbFormat >= sizeof(VIDEOINFOHEADER)) {
#pragma warning(push)
#pragma warning(disable : 6001)
            VIDEOINFOHEADER *pVih = reinterpret_cast<VIDEOINFOHEADER *>(mt.pbFormat);
            width = pVih->bmiHeader.biWidth;
            height = pVih->bmiHeader.biHeight;
#pragma warning(pop)
        }
        if (mt.cbFormat != 0) {
            CoTaskMemFree((PVOID)mt.pbFormat);
        }
        pin->Release();
    }
    enumPins->Release();
}
