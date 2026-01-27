
// ChildView.cpp : CChildView class implementation
//

#include "framework.h"
#include "id3FingerCaptureSamples.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void vformat(std::string &str, const char *fmt, va_list arg_list)
{
    int result = vsnprintf_s((char *)str.data(), str.capacity(), 256, fmt, arg_list);
    if (result < (int)str.capacity())
        str.resize(result);
}

std::string format(const char *fmt, ...)
{
    va_list	arg_list;
    va_start(arg_list, fmt);
    std::string str(256, '\0');
    vformat(str, fmt, arg_list);
    va_end(arg_list);
    return str;
}

std::wstring wformat(const wchar_t *fmt, ...)
{
    va_list	list;
    std::wstring wstr(256, '\0');
    va_start(list, fmt);
    int result = _vsnwprintf_s((wchar_t *)wstr.data(), wstr.capacity(), 256, fmt, list);
    if (result < (int)wstr.capacity())
        wstr.resize(result);
    va_end(list);
    return wstr;
}

std::string gErrStr;
const char *getErrorString(int err) {
    auto err_str = id3FingerCapture_GetFingerCaptureErrorString(err);
    if (err_str != nullptr) {
        return err_str;
    }
    err_str = id3Devices_GetDevicesErrorString(err);
    if (err_str != nullptr) {
        return err_str;
    }
    err_str = id3Finger_GetFingerErrorString(err);
    if (err_str != nullptr) {
        return err_str;
    }
    gErrStr = format("Internal error code: %d", err);
    return gErrStr.c_str();
}

void check_sdkerr(int err, const char *msg)
{
	if (err != 0) {
        std::string str = msg;
        str.append(": ");
        str.append(getErrorString(err));
        MessageBoxA(nullptr, str.c_str(), "Capture", MB_OK);
		exit(1);
	}
}

std::string getDeviceName(int device_id)
{
    auto deviceInfo = DeviceManager::getDeviceInfo(device_id);
    return deviceInfo.getName();
}

// C Camera Event Handler
static void CaptureManager_DeviceAddedHandler(void *context, int deviceId)
{
    ((CChildView *)context)->DeviceAddedHandler(deviceId);
}

static void CaptureManager_DeviceRemovedHandler(void* context, int device_id)
{
    ((CChildView *)context)->DeviceRemovedHandler(device_id);
}

static void CaptureChannel_DeviceStatusChangedHandler(void* context, id3FingerCaptureStatus eStatus, int errorCode)
{
    ((CChildView *)context)->DeviceStatusChangedHandler(eStatus, errorCode);
}

static void CaptureChannel_ImagePreviewHandler(void* context)
{
    ((CChildView *)context)->ImagePreviewHandler();
}

static void CaptureChannel_ImageCapturedHandler(void* context)
{
    ((CChildView *)context)->ImageCapturedHandler();
}

// CChildView
BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_ERASEBKGND()
    ON_WM_PAINT()
    ON_WM_CLOSE()
END_MESSAGE_MAP()

CChildView::CChildView()
{
    m_statusBar = nullptr;
    m_lockCurrentPicture = 0;
}

CChildView::~CChildView()
{
}

// C++ Camera Event Handler
void CChildView::DeviceAddedHandler(int device_id)
{
    auto camera_name = getDeviceName(device_id);
    std::string msg = "Found sensor " + camera_name;
    m_statusBar->SetPaneText(0, msg.c_str());
    // Auto select first camera
    int currentDeviceId = m_fingerCapture.getDeviceId();
    if (currentDeviceId == 0) {
        try
        {
            m_fingerCapture.openDevice(device_id);
        }
        catch (FingerCaptureException &ex)
        {
            MessageBoxA(ex.what(), "DeviceAddedHandler", MB_OK);
        }
    }
}

void CChildView::DeviceRemovedHandler(int device_id)
{
    int currentDeviceId = m_fingerCapture.getDeviceId();
    if (device_id == currentDeviceId) {
        auto camera_name = getDeviceName(device_id);
        std::string msg = "Lost camera " + camera_name;
        m_statusBar->SetPaneText(0, msg.c_str());
    }
}

void CChildView::DeviceStatusChangedHandler(id3FingerCaptureStatus eStatus, int errorCode)
{
    if (eStatus == id3DevicesDeviceCaptureStatus_DeviceReady)
    {
        int currentDeviceId = m_fingerCapture.getDeviceId();
        m_statusBar->SetPaneText(0, format("Start senser %d", currentDeviceId).c_str());

        m_fingerCapture.startCapture(id3FingerCaptureFingerPositionFlags_RightIndexFinger);
        Invalidate();
    }
    else if (eStatus == id3DevicesDeviceCaptureStatus_DeviceError)
    {
        int currentDeviceId = m_fingerCapture.getDeviceId();
        m_statusBar->SetPaneText(0, format("sensor %d error %d", currentDeviceId, errorCode).c_str());
    }
}

void CChildView::ImagePreviewHandler()
{
    if (m_lockCurrentPicture == 0)
    {
        if (m_fingerCapture.getCurrentFrame(m_currentPicture))
        {
            int height = m_currentPicture.getHeight();
            if (height > 0)
            {
                Invalidate(FALSE); // FALSE because we don't want to erase background
            }
        }
    }
}

void CChildView::ImageCapturedHandler()
{
    if (m_lockCurrentPicture == 0)
    {
        try
        {
            FingerImage fingerImage;
            m_fingerCapture.getCapturedImage(fingerImage);
            int fingerCount = fingerImage.getFingerCount();
            m_statusBar->SetPaneText(0, format("Finger count = %d", fingerCount).c_str());
            if (fingerCount > 0)
            {
                FingerTemplateRecord templateRecord = fingerImage.getTemplateRecord();
                int templateRecordCount = templateRecord.getCount();
                if (templateRecordCount > 0)
                {
                    FingerTemplate fingerTemplateItem = templateRecord.get(0);
                    id3FingerPosition ePosition = fingerTemplateItem.getPosition();
                    int quality = fingerTemplateItem.getQuality();
                    m_statusBar->SetPaneText(0, format("Finger count = %d, finger position = %s, quality = %d", fingerCount, id3Finger_GetFingerPositionString(ePosition), quality).c_str());
                }
            }
        }
        catch (FingerCaptureException &ex)
        {
        }
    }
}

void CChildView::Initialize(CStatusBar *statusBar)
{
    m_statusBar = statusBar;

    m_lockCurrentPicture++;

    /**
     * Fill in the correct path to the license.
     */
    std::string license_path = "../../id3Finger.lic";
    /**
     * Fill in the correct path to the downloaded models.
     */
    std::string models_dir = "../../models/";

    // Load license in devices sdk
    try {
        DevicesLicense::checkLicense(license_path.c_str());
    }
    catch (DevicesException &ex) {
        MessageBoxA(ex.what(), "DevicesLicense::checkLicense", MB_OK);
    }

    // Load license in finger sdk
    try {
        id3FingerLicense_CheckLicense(license_path.c_str(), nullptr);
        /**
         * To use the finger minutia detector it is required to instanciate a finger extractor module and
         * then load the required model files into the the desired processing unit.
         */
        id3FingerLibrary_LoadModel(models_dir.c_str(), id3FingerModel_FingerMinutiaDetector3B, id3FingerProcessingUnit_Cpu);
         /**
          * Once the instance is initialized, it is now possible to set its parameters.
          * - MinutiaDetectorModel: default value is FingerMinutiaDetector3B which is the one used for latest id3
          * MINEX III submission
          * - ThreadCount: allocating more than 1 thread here can increase the speed of the process.
          */
        m_extractor.setMinutiaDetectorModel(id3FingerModel_FingerMinutiaDetector3B);
        m_extractor.setThreadCount(4);
    }
    catch (FingerException &ex) {
        MessageBoxA(ex.what(), "Finger", MB_OK);
    }

    // Initialize device manager
    try {
        DeviceManager::initialize();
        DeviceManager::configure(id3DevicesMessageLoopMode_ApplicationMessageLoop);
    }
    catch (DevicesException &ex) {
        MessageBoxA(ex.what(), "DeviceManager", MB_OK);
    }

    // Initialize finger scanner
    try {
        m_fingerCapture.initialize();
        // Setup finger scanner parameters
        m_fingerCapture.setAutoCapture(true);
        m_fingerCapture.setAutoProcess(true);
        m_fingerCapture.setFingerExtractor(m_extractor);

        // Setup device manager callback
        m_fingerCapture.setDeviceAddedCallback(CaptureManager_DeviceAddedHandler, this);
        m_fingerCapture.setDeviceRemovedCallback(CaptureManager_DeviceRemovedHandler, this);

        // Setup finger scanner callback
        m_fingerCapture.setDeviceStatusChangedCallback(CaptureChannel_DeviceStatusChangedHandler, this);
        m_fingerCapture.setImagePreviewCallback(CaptureChannel_ImagePreviewHandler, this);
        m_fingerCapture.setImageCapturedCallback(CaptureChannel_ImageCapturedHandler, this);
    }
    catch (FingerCaptureException &ex) {
        MessageBoxA(ex.what(), "FingerCapture", MB_OK);
    }

    // Load Futronic and Morpho Smart FingerScanner plugin
    try {
        DeviceManager::loadPlugin("id3DevicesFutronic");
        DeviceManager::loadPlugin("id3DevicesMorphoSmart");

        // Start device manager
        DeviceManager::start();
    }
    catch (DevicesException &ex) {
        MessageBoxA(ex.what(), "Devices", MB_OK);
    }

    m_statusBar->SetPaneText(0,"Initialization done. sensor plug & play process started.");
    m_lockCurrentPicture--;
}

void CChildView::OnClose() {
    m_lockCurrentPicture++;
    if (m_fingerCapture.getDeviceId() && m_fingerCapture.getIsCapturing()) {
        m_fingerCapture.stopCapture();
        Sleep(100);
    }
    DeviceManager::stop();
    id3FingerLibrary_UnloadModel(id3FingerModel_FingerMinutiaDetector3B, id3FingerProcessingUnit_Cpu);
}

void CChildView::Dispose()
{
    DeviceManager::dispose();
}

void CChildView::LoadImage(CString &sFilePath)
{
    bool captureStarted = (m_fingerCapture.getDeviceId() != 0) && m_fingerCapture.getIsCapturing();
    if (captureStarted == true) {
        MessageBoxA("Unable to load an image when a camera is in used", MB_OK);
        return;
    }
    try
    {
        m_lockCurrentPicture++;
        // To support WSQ image we need to use FingerImage instead of CaptureImage
        FingerImage img;
        img.fromFile(sFilePath.GetString(), id3FingerPixelFormat_Grayscale8Bits);
        m_currentPicture.fromRawBuffer((uint8_t*)img.getPixels(), img.getSizeInBytes(), img.getWidth(), img.getHeight(), img.getStride(), id3DevicesPixelFormat_Grayscale8Bits, id3DevicesPixelFormat_Bgr24Bits);
        m_lockCurrentPicture--;
        Invalidate(TRUE);
    }
    catch (FingerException &ex)
    {
        MessageBoxA(ex.what(), "LoadImage", MB_OK);
    }
    catch (DevicesException &ex)
    {
        MessageBoxA(ex.what(), "LoadImage", MB_OK);
    }
}

// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), nullptr);

	return TRUE;
}

BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
    RECT rect{};
    GetClientRect(&rect);

	// Create a black background
	CBrush backBrush;
	backBrush.CreateSolidBrush(RGB(0,0,0));
	pDC->FillRect(&rect, &backBrush);

    return TRUE;
}

int frameCount = 1;
void CChildView::OnPaint() 
{
    if (m_lockCurrentPicture != 0)
        return;

	CPaintDC dc(this); // device background for painting

    int height = m_currentPicture.getHeight();
    if (height > 0)
    {
        int width = m_currentPicture.getWidth();
        if (m_image.IsNull())
        {
            m_image.Create(width, -height, 24); // -height because need top-down DIB 
        }
        else if ((m_image.GetWidth() != width) || (m_image.GetHeight() != height))
        {
            m_image.Destroy();
            m_image.Create(width, -height, 24); // -height because need top-down DIB 
        }
        auto pixels_src = (unsigned char *)m_currentPicture.getPixels();
        int stride_src = m_currentPicture.getStride();
        int stride_dst = m_image.GetPitch();
        if (stride_dst == stride_src)
        {
            void *pixels_dst = m_image.GetBits();
            memcpy(pixels_dst, pixels_src, stride_src * height);
        }
        else
        {
            // need to change the stride
            for (int y=0; y < height; y++)
            {
                void *pixels_dst = m_image.GetPixelAddress(0, y);
                memcpy(pixels_dst, pixels_src, stride_src);
                pixels_src += stride_src;
            }
        }
    }

    if (!m_image.IsNull())
    {
        RECT rect{};
        GetClientRect(&rect);

        int img_width = m_image.GetWidth();
        int img_height = m_image.GetHeight();
        if (img_height > 0)
        {
            RECT destRect{};
            float r1 = (float)img_width / img_height;
            float r2 = (float)rect.right / rect.bottom;
            int w = rect.right;
            int h = rect.bottom;
            // vertical or horizontal center to maintain aspect ratio
            if (r1 > r2)
            {
                w = rect.right;
                h = (int)(w / r1);
            } else if (r1 < r2)
            {
                h = rect.bottom;
                w = (int)(r1 * h);
            }
            destRect.left = rect.left + (rect.right - w) / 2;
            destRect.top = rect.top + (rect.bottom - h) / 2;
            destRect.right = destRect.left + w;
            destRect.bottom = destRect.top + h;

            CRect boundsRect = CRect();
            boundsRect.OffsetRect(destRect.left, destRect.top);

            dc.SetStretchBltMode(HALFTONE);
            m_image.StretchBlt(dc.GetSafeHdc(), destRect, SRCCOPY);

            dc.SelectStockObject(NULL_BRUSH);
            CPen pen(PS_SOLID, 1, RGB(0, 221, 52));
            dc.SelectObject(pen);
            dc.RoundRect(boundsRect, CPoint(4,4));
        }
    }
	// Ne pas appeler CWnd::OnPaint() pour la peinture des messages
}
