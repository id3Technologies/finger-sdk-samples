using System;
using System.Drawing;
using System.IO;
using System.Windows.Forms;

namespace id3.Finger.Samples.SlapSegmentationWF
{
    using id3.Finger;

    public partial class Form1 : Form
    {
        // id3 Finger SDK objects
        FingerDetector fingerDetector;

        public Form1()
        {
            InitializeComponent();

            // UI elements
            FormClosed += Form1_FormClosed;
            openToolStripMenuItem.Click += OpenToolStripMenuItem_Click;

            // id3 Finger SDK objects
            try
            {
                // Before calling any function of the SDK you must first check a valid license file.
                // To get such a file please use the provided activation tool.
                FingerLibrary.CheckLicense(@"your_license_path_here");
            }
            catch (FingerException ex)
            {
                MessageBox.Show("Error during license check: " + ex.Message);
                Environment.Exit(-1);
            }

            // The Finger SDK heavily relies on deep learning technics and hence requires trained models to run.
            // Fill in the correct path to the downloaded models.
            string modelPath = "..\\..\\..\\..\\sdk\\models";

            try
            {
                // Once a model is loaded in the desired processing unit (CPU) several instances of the associated processor can be created.
                FingerLibrary.LoadModel(modelPath, FingerModel.FingerDetector2A, ProcessingUnit.Cpu);

                // Init objects
                fingerDetector = new FingerDetector()
                {
                    ConfidenceThreshold = 70,
                    ThreadCount = 4
                };
            }
            catch (FingerException ex)
            {
                MessageBox.Show("Error during finger objects initialization: " + ex.Message);
                Environment.Exit(-1);
            }

        }

        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            // Unload models
        }

        private void OpenToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog1 = new OpenFileDialog();
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                textBox1.Text = openFileDialog1.FileName;
            }

            // Load image from disk
            // Setting the resolution of the image is mandatory for fingerprint detection and extraction
            // Not doing so would result in a -2006 "Invalid resolution"
            FingerImage image = FingerImage.FromFile(textBox1.Text, PixelFormat.Grayscale8Bits);
            image.SetResolution(500);

            // Detect fingerprints if any
            DetectedFingerList detectedFingerList = fingerDetector.DetectFingers(image);

            // Display
            FingerImage imageToDraw = FingerImage.FromFile(textBox1.Text, PixelFormat.Bgr24Bits);

            float scalingRatio = FitImageInBox(imageToDraw, pictureBox1);

            Bitmap bitmap = ConvertToBitmap(imageToDraw);
            pictureBox1.Image = bitmap;

            // For each detected finger...
            for (int i = 0; i < detectedFingerList.GetCount(); i++)
            {
                DetectedFinger detectedFinger = (DetectedFinger)detectedFingerList[i];

                // ...draw bounds
                using (Graphics gr = Graphics.FromImage(bitmap))
                {
                    var rect = ConvertRectangle(detectedFinger.Bounds, 1.0F / scalingRatio);

                    gr.DrawRectangle(new Pen(Color.Green, 2), rect);
                }
            }
        }

        // Utils

        private Bitmap ConvertToBitmap(FingerImage image)
        {
            byte[] imageData = image.ToBuffer(ImageFormat.Bmp, 0);

            Bitmap bmp;
            using (var ms = new MemoryStream(imageData))
            {
                bmp = new Bitmap(ms);
            }
            return bmp;
        }

        private float FitImageInBox(FingerImage image, PictureBox box)
        {
            float ratioW = image.Width / (float)box.Width;
            float ratioH = image.Height / (float)box.Height;

            float maxRatio = (ratioW > ratioH) ? ratioW : ratioH;

            if (maxRatio > 1.0f)
            {
                // image is too large: resize down
                image.Resize((int)(image.Width / maxRatio), (int)(image.Height / maxRatio));

                return maxRatio;
            }
            else
            {
                // nothing to do
                return 1.0F;
            }
        }

        private System.Drawing.Rectangle ConvertRectangle(Rectangle rectangle, float scale)
        {
            return new System.Drawing.Rectangle(
                (int)(rectangle.TopLeft.X * scale),
                (int)(rectangle.TopLeft.Y * scale),
                (int)((rectangle.TopRight.X - rectangle.TopLeft.X) * scale),
                (int)((rectangle.BottomLeft.Y - rectangle.TopLeft.Y) * scale)
                );
        }
    }
}
