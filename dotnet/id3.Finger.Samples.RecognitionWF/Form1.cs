using System;
using System.Drawing;
using System.IO;
using System.Windows.Forms;

namespace id3.Finger.Samples.RecognitionWF
{
    using id3.Finger;

    public partial class Form1 : Form
    {
        // Constants
        const int kMinutiaCircleRadius = 5;
        const int kMinutiaLineSize = 12;

        // id3 Finger SDK objects
        FingerExtractor fingerExtractor;
        FingerTemplate reference;
        FingerTemplate probe;

        public Form1()
        {
            InitializeComponent();

            // UI elements
            loadReferenceToolStripMenuItem.Click += LoadReferenceToolStripMenuItem_Click;
            loadProbeToolStripMenuItem.Click += LoadProbeToolStripMenuItem_Click;
            matchButton.Click += MatchButton_Click;

            // id3 Finger SDK objects
            try
            {
                // Before calling any function of the SDK you must first check a valid license file.
                // To get such a file please use the provided activation tool.
                // The license file can also be retrieved using the License.activation(...) APIs
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
                // Loading the Finger Minutia Detector 3B (MINEX III compliant)
                // Once a model is loaded in the desired processing unit (CPU) several instances of the associated processor can be created.
                FingerLibrary.LoadModel(modelPath, FingerModel.FingerMinutiaDetector3B, ProcessingUnit.Cpu);

                // Init objects
                fingerExtractor = new FingerExtractor()
                {
                    MinutiaDetectorModel = FingerModel.FingerMinutiaDetector3B, // setting the model to use is mandatory
                };
            }
            catch (FingerException ex)
            {
                MessageBox.Show("Error during finger objects initialization: " + ex.Message);
                Environment.Exit(-1);
            }

        }

        private void LoadReferenceToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                textBox1.Text = openFileDialog.FileName;
            }

            reference = LoadAndProcessImage(textBox1, pictureBox1);
        }

        private void LoadProbeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                textBox2.Text = openFileDialog.FileName;
            }

            probe = LoadAndProcessImage(textBox2, pictureBox2);
        }

        private void MatchButton_Click(object sender, EventArgs e)
        {
            if (reference == null || probe == null)
            {
                return;
            }

            FingerMatcher matcher = new FingerMatcher();
            int score = matcher.CompareTemplates(reference, probe);

            matchScoreLabel.Text = "Match score: " + score.ToString();
            matchDecisionLabel.Text = "Match decision: " + ((score > (int) FingerMatcherThreshold.Fmr10000) ? "Match" : "No match");
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

        private FingerTemplate LoadAndProcessImage(TextBox textBox, PictureBox pictureBox)
        {
            // Load image from disk
            // Setting the resolution of the image is mandatory for fingerprint detection and extraction
            // Not doing so would result in a -2006 "Invalid resolution"
            FingerImage image = FingerImage.FromFile(textBox.Text, PixelFormat.Grayscale8Bits);
            image.SetResolution(500);

            // Create template
            FingerTemplate fingerTemplate = fingerExtractor.CreateTemplate(image);

            // Display
            FingerImage imageToDraw = FingerImage.FromFile(textBox.Text, PixelFormat.Bgr24Bits);

            float scalingRatio = FitImageInBox(imageToDraw, pictureBox);

            Bitmap bitmap = ConvertToBitmap(imageToDraw);
            pictureBox.Image = bitmap;

            // Display minutiae
            for (int i = 0; i < fingerTemplate.MinutiaList.GetCount(); i++)
            {
                FingerMinutia minutia = (FingerMinutia)fingerTemplate.MinutiaList[i];

                using (Graphics gr = Graphics.FromImage(bitmap))
                {
                    var pen = new Pen((minutia.RidgeType == FingerMinutiaRidgeType.Ending) ? Color.Blue : Color.Red, 2);

                    var rect = new System.Drawing.Rectangle(minutia.X - kMinutiaCircleRadius, minutia.Y - kMinutiaCircleRadius, 2 * kMinutiaCircleRadius, 2 * kMinutiaCircleRadius);
                    gr.DrawEllipse(pen, rect);
                    gr.DrawLine(pen, new System.Drawing.Point(minutia.X, minutia.Y), new System.Drawing.Point((int)(minutia.X + kMinutiaLineSize * Math.Cos(minutia.Angle)), (int)(minutia.Y - kMinutiaLineSize * Math.Sin(minutia.Angle))));
                }
            }

            return fingerTemplate;
        }
    }
}
