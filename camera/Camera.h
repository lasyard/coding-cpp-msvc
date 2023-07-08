#pragma once

class CCamera
{
public:
    CCamera(int i = -1, LPCTSTR n = NULL, LPCTSTR d = NULL, LPCTSTR p = NULL);
    virtual ~CCamera();

    int id;
    CString name;
    CString desc;
    CString path;

    bool hasWindow() const;
    bool isRunning() const;
    void setWindow(HWND h);
    void setWindowFocus();
    void closeWindow();
    void startCapture();
    void ajustWindowPos(int cx, int cy);
    void stopCapture();
    void endCapture();

protected:
    mutable bool exist;

    bool running;
    int width;
    int height;

    HWND hwnd;

    IBaseFilter *filter;
    IGraphBuilder *graphBuilder;
    ICaptureGraphBuilder2 *captureGraphBuilder2;
    IMediaControl *mediaControl;
    IVideoWindow *videoWindow;

    void initCaptureGraph();
    bool hasCaptureGraph();
    void releaseCaptureGraph();
    void getMediaInfo();

    friend class CCameraManager;
};
