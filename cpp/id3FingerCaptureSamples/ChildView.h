
// ChildView.h : interface de la classe CChildView
//


#pragma once
#include <id3DevicesLib.h>
#include <id3Devices/helpers/id3DevicesCpp.h>
#include "id3FingerCpp.h"
#include <id3FingerLib.h>
#include <string>
#include <vector>


// fenêtre de CChildView

class CChildView : public CWnd
{
// Construction
public:
	CChildView();

// Attributs
public:

protected:
    CStatusBar *m_statusBar{};
    CImage m_image;
    FingerScanner m_fingerCapture;
    CapturedImage m_currentPicture{};
    FingerExtractor m_extractor;
    std::vector<uint8_t> m_pixels;
    CRect m_facialRect{};
    volatile int m_lockCurrentPicture;

// Opérations
public:
    void Initialize(CStatusBar *statusBar);
    void Dispose();
    void DeviceAddedHandler(int device_id);
    void DeviceRemovedHandler(int device_id);
    void DeviceStatusChangedHandler(id3FingerCaptureStatus eStatus, int errorCode);
    void ImagePreviewHandler();
    void ImageCapturedHandler();

    void LoadImage(CString &sFilePath);

// Substitutions
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implémentation
public:
	virtual ~CChildView();

	// Fonctions générées de la table des messages
protected:
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
    afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()
};

