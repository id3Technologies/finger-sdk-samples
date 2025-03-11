using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace id3.Finger.Samples.CaptureWPF
{
    using id3.Devices;
    using id3.Finger;
    using id3.FingerCapture;

    /// <summary>
    /// Logique d'interaction pour MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private FingerScanner fingerScanner;
        private CaptureImage previewImage;
        private CaptureImage captureImage;
        private WriteableBitmap previewBitmap;

        public MainWindow()
        {
            InitializeComponent();

            previewImage = new CaptureImage();
            captureImage = new CaptureImage();

            // id3 Finger SDK objects
            try
            {
                // Before calling any function of the SDK you must first check a valid license file.
                // To get such a file please use the provided activation tool.
                DevicesLicense.CheckLicense(@"..\..\..\..\id3Finger.lic");

                // Initialize, load corresponding sensor plugin, and start DeviceManager
                DeviceManager.Initialize();
                DeviceManager.Configure(MessageLoopMode.ApplicationMessageLoop);
                DeviceManager.LoadPlugin("id3DevicesFutronic");
                DeviceManager.Start();
            }
            catch (DevicesException ex)
            {
                MessageBox.Show("Error during license check: " + ex.Message);
                Environment.Exit(-1);
            }
            catch (FingerException ex)
            {
                MessageBox.Show("Error during license check: " + ex.Message);
                Environment.Exit(-1);
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error during license check: " + ex.Message);
                Environment.Exit(-1);
            }

            // initialize finger scanner
            fingerScanner = new FingerScanner()
            {
                FingerExtractor = new FingerExtractor()
                {
                    MinutiaDetectorModel = FingerModel.FingerMinutiaDetector3B,
                    ThreadCount = 4
                },
                AutoCapture = true,
                AutoProcess = true
            };

            // finger capture events allbacks
            fingerScanner.DeviceAdded += FingerScanner_DeviceAdded;
            fingerScanner.DeviceRemoved += FingerScanner_DeviceRemoved;
            fingerScanner.DeviceStatusChanged += FingerScanner_DeviceStatusChanged;
            fingerScanner.ImagePreview += FingerScanner_ImagePreview;
            fingerScanner.ImageCaptured += FingerScanner_ImageCaptured;
        }

        public void Dispose()
        {
            DeviceManager.Dispose();
        }

        public void StartCapture()
        {
            if (fingerScanner.DeviceState == DeviceState.DeviceReady)
                fingerScanner.StartCapture(FingerPositionFlags.Unknown);
        }

        public void StopCapture()
        {
            if (fingerScanner.DeviceState == DeviceState.CaptureInProgress)
                fingerScanner.StopCapture();
        }

        private void FingerScanner_DeviceAdded(object sender, PlugAndPlayCallbackEventArgs e)
        {
            fingerScanner.OpenDevice(e.DeviceId);
        }

        private void FingerScanner_DeviceRemoved(object sender, PlugAndPlayCallbackEventArgs e)
        {
            DeviceInfo deviceInfo = DeviceManager.GetDeviceInfo(e.DeviceId);
            ResetImage();
        }

        private void FingerScanner_ImagePreview(object sender, EventArgs e)
        {
            if (fingerScanner.GetCurrentFrame(previewImage))
                DisplayImage(previewImage);
        }

        private void FingerScanner_ImageCaptured(object sender, EventArgs e)
        {
            fingerScanner.GetLastSnapshot(previewImage);
            DisplayImage(previewImage);
        }

        private void FingerScanner_DeviceStatusChanged(object sender, FingerCaptureStatusCallbackEventArgs e)
        {
            switch (e.Status)
            {
                case FingerCaptureStatus.CaptureStarted:
                    break;

                case FingerCaptureStatus.DeviceReady:
                    fingerScanner.StartCapture(FingerPositionFlags.Unknown);
                    break;

                case FingerCaptureStatus.CaptureStopped:
                case FingerCaptureStatus.DeviceError:
                case FingerCaptureStatus.PlaceFinger:
                case FingerCaptureStatus.FingerPlaced:
                case FingerCaptureStatus.FingerRemoved:
                case FingerCaptureStatus.RemoveFinger:
                case FingerCaptureStatus.FewerFingers:
                case FingerCaptureStatus.IndexRotated:
                case FingerCaptureStatus.MoveFingerLeft:
                case FingerCaptureStatus.MoveFingerRight:
                case FingerCaptureStatus.MoveFingerUp:
                case FingerCaptureStatus.MoveFingerDown:
                case FingerCaptureStatus.PressFingerHarder:
                case FingerCaptureStatus.RotateHandClockwise:
                case FingerCaptureStatus.RotateHandCounterclockwise:
                case FingerCaptureStatus.SpreadYourFingers:
                case FingerCaptureStatus.ThumbsRotated:
                case FingerCaptureStatus.TightenYourFingers:
                case FingerCaptureStatus.TooManyFingers:
                case FingerCaptureStatus.WrongHand:
                    //MessageBox.Show(e.Status.ToString());
                    break;
            }
        }

        // display capture preview image
        private void DisplayImage(CaptureImage image)
        {
            imagePreview.BeginInit();
            imagePreview.Source = ImageToBitmap(image, ref previewBitmap);
            imagePreview.EndInit();
        }

        // reset preview image
        private void ResetImage()
        {
            imagePreview.BeginInit();
            imagePreview.Source = null;
            imagePreview.EndInit();
        }

        private BitmapSource ImageToBitmap(CaptureImage image, ref WriteableBitmap wbm)
        {
            int w = image.Width;
            int h = image.Height;
            int bufferSize = image.Height * image.Stride;

            if (wbm == null || (wbm.Width != image.Width || wbm.Height != image.Height))
                wbm = new WriteableBitmap(w, h, 96, 96, PixelFormats.Bgr24, null);

            if ((h <= wbm.Height) && (w <= wbm.Width))
            {
                wbm.WritePixels(new Int32Rect(0, 0, w, h), image.GetPixels(), bufferSize, image.Stride);
            }
            return wbm;
        }
    }
}
