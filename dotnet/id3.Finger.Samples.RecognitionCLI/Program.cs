﻿using System;


namespace id3.Finger.Samples.RecognitionCLI
{
    using id3.Finger;

    class Program
    {
        static void Sample_1()
        {
            /*
             * Load sample images from files.
             * Finger images are always loaded to 8 bits grayscale pixel format
             */
            Console.Write("Loading images from files... ");
            FingerImage image1 = FingerImage.FromFile("../../../../data/image1.bmp", PixelFormat.Grayscale8Bits);
            FingerImage image2 = FingerImage.FromFile("../../../../data/image2.bmp", PixelFormat.Grayscale8Bits);
            Console.Write("Done.\n");

            /*
            * Set the resolution of the two previsously loaded finger images.
            * Not doing so would result in a -2006 "Invalid resolution" error during finger template extraction
            * id3 Finger SDK processors uses only 500 dpi images. Images with a different resolution must be rescaled
            * before use.
            */
            image1.SetResolution(500);
            image2.SetResolution(500);

            /*
             * To use the finger minutia detector it is required to instanciate a finger extractor module and 
             * then load the required model files into the RAM of the desired processing unit.
             * 
             * Minutia detection is mandatory and is self-sufficient for creating iso templates.
             * Setting other models will activate the generation of additional proprietary template data. See 
             * SDK user guide for more details.
             */
            Console.Write("Init finger extractor... ");
            FingerExtractor fingerExtractor = new FingerExtractor()
            {
                MinutiaDetectorModel = FingerModel.FingerMinutiaDetector3B, // activate minutiae detection (mandatory)
                FingerAlignerModel = FingerModel.FingerAligner1A, // activate alignment for finger encoding (optional)
                FingerEncoderModel = FingerModel.FingerEncoder1A, // activate proprietary template encoding (optional)
                ThreadCount = 4
            };
            Console.Write("Done.\n");

            /*
            * Create the templates from each image.
            * As we activated proprietary encoding it will be processed at the same time
            */
            Console.Write("Creating templates... ");
            FingerTemplate referenceTemplate = fingerExtractor.CreateTemplate(image1);
            FingerTemplate probeTemplate = fingerExtractor.CreateTemplate(image2);
            Console.Write("Done.\n");

            /*
             * Initialize a finger matcher instance.
             */
            FingerMatcher fingerMatcher = new FingerMatcher();
            /*
             * The matching process returns a score between 0 and 65535. To take a decision this score
             * must be compared to a defined threshold.
             * For 1 to 1 authentication, it is recommended to select a threshold of at least 4000 (FMR=1:10K).
             * Please see documentation to get more information on how to choose the threshold.
             */
            Console.Write("Comparing templates... ");
            int score = fingerMatcher.CompareTemplates(referenceTemplate, probeTemplate);
            Console.Write("Done.\n");
            if (score > (int)FingerMatcherThreshold.Fmr10000)
            {
                Console.WriteLine("Match (minutiae and proprietary): " + score);
            }
            else
            {
                Console.WriteLine("No match (minutiae and proprietary): " + score);
            }

            /*
             * By default the Finger Matcher instance uses all possible data found in the provided templates.
             * However it is possible to force it to only use ISO minutiae data for example.
             */
            fingerMatcher.MinexOnly = true;
            Console.Write("Comparing templates minutiae only... ");
            int scoreMinex = fingerMatcher.CompareTemplates(referenceTemplate, probeTemplate);
            Console.Write("Done.\n");
            if (scoreMinex > (int)FingerMatcherThreshold.Fmr10000)
            {
                Console.WriteLine("Match (minutiae only): " + scoreMinex);
            }
            else
            {
                Console.WriteLine("No match (minutiae only): " + scoreMinex);
            }
        }

        static void Sample_2()
        {
            /*
             * Load fingerprint image from ANSI/NIST-ITL transactions
             * - The SDK can handle ANSI/NIST-ITL 1-2011 Update 2015 transactions both in traditional and xml encoded formats
             * - The SDK will return a FingerImageList containing all found images in records types 04 or 14. 
             * - When available the finger position and the finger resolution will be retrieved
             * 
             * Transactions used in this sample are examples from https://www.nist.gov/itl/iad/image-group/ansinist-itl-standard-references
             */
            Console.WriteLine("Loading images from ANSI/NIST-ITL transactions... ");
            // Load from traditional encoded transation with type 04 records
            FingerImageRecord imageList1 = FingerImageRecord.FromFile(FingerImageRecordFormat.An2k2011TransactionTraditionalEncoding, "../../../../data/an2k-type-04-tpcard.an2");
            Console.WriteLine("Found " + imageList1.Count + " fingerprint images in the an2k-type-04-tpcard.an2 transaction");
            // Load from xml encoded transation with type 14 records
            FingerImageRecord imageList2 = FingerImageRecord.FromFile(FingerImageRecordFormat.An2k2011TransactionTraditionalEncoding, "../../../../data/an2k-type-14-tpcard.an2");
            Console.WriteLine("Found " + imageList2.Count + " fingerprint images in the an2k-type-14-tpcard.an2 transaction");
            Console.WriteLine("Done.");

            Console.Write("Init finger extractor... ");
            FingerExtractor fingerExtractor = new FingerExtractor()
            {
                MinutiaDetectorModel = FingerModel.FingerMinutiaDetector3B, // activate minutiae detection (mandatory)
                ThreadCount = 4
            };
            Console.Write("Done.\n");

            Console.WriteLine($"an2k-type-04-tpcard.an2 resolution:");
            foreach (FingerImage image in imageList1)
            {
                Console.WriteLine($"Image resolution : {image.HorizontalResolution}");
            }

            Console.WriteLine($"an2k-type-14-tpcard.an2 resolution:");
            foreach (FingerImage image in imageList2)
            {
                Console.WriteLine($"Image resolution : {image.HorizontalResolution}");
            }
        }

        static void Main(string[] args)
        {
            Console.WriteLine("---------------------------------");
            Console.WriteLine("id3.Finger.Samples.RecognitionCLI");
            Console.WriteLine("---------------------------------");

            /*
             * This basic sample shows how to extract two finger templates and compare them
             */
            try
            {
                /*
                 * Before calling any function of the SDK you must first check a valid license file.
                 * To get such a file please use the provided activation tool.
                 * It is also possible to use the FingerLicense.Activation(...) APIs
                 */
                FingerLicense.CheckLicense(@"..\..\..\..\id3Finger.lic");
            }
            catch (FingerException ex)
            {
                Console.WriteLine("Error during license check" + ex.Message);
                Environment.Exit(-1);
            }


            /*
             * The Finger SDK heavily relies on deep learning technics and hence requires trained models to run.
             * Fill in the correct path to the downloaded models.
             */
            string modelPath = "../../../../models";
            /*
            * Once a model is loaded in the desired processing unit (CPU or GPU) several instances of the associated processor can be created.
            * For instance in this sample, we load a minutia detector.
            */
            Console.Write("Loading models... ");
            FingerLibrary.LoadModel(modelPath, FingerModel.FingerMinutiaDetector3B, ProcessingUnit.Cpu);
            FingerLibrary.LoadModel(modelPath, FingerModel.FingerAligner1A, ProcessingUnit.Cpu);
            FingerLibrary.LoadModel(modelPath, FingerModel.FingerEncoder1A, ProcessingUnit.Cpu);
            Console.Write("Done.\n");

            //Sample_1();
            Sample_2();

            Console.WriteLine("Sample terminated successfully.");
            //Console.ReadKey();
        }
    }
}
