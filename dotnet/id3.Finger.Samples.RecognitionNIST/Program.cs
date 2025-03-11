using System;

namespace id3.Finger.Samples.RecognitionNIST
{
    using id3.Finger;

    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("----------------------------------");
            Console.WriteLine("id3.Finger.Samples.RecognitionNIST");
            Console.WriteLine("----------------------------------");

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
            Console.Write("Loading model... ");
            FingerLibrary.LoadModel(modelPath, FingerModel.FingerMinutiaDetector3B, ProcessingUnit.Cpu);
            Console.Write("Done.\n");
            
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

            /*
             * Initialize finger extractor
             */
            Console.Write("Init finger extractor... ");
            FingerExtractor fingerExtractor = new FingerExtractor()
            {
                MinutiaDetectorModel = FingerModel.FingerMinutiaDetector3B, // activate minutiae detection (mandatory)
                ThreadCount = 4
            };
            Console.Write("Done.\n");

            // display finger images resolutions
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

            /*
             * Initialize finger extractor
             */
            Console.Write("Init finger matcher... ");
            FingerMatcher fingerMatcher = new FingerMatcher();
            Console.Write("Done.\n");

            /*
             * Perform matches between the two records.
             * Several scenarii can be used, here we will:
             * - only compare fingerprint with the same positions
             * - skip the two or four finger slaps (which need to be processed using the FingerDetector module to separate individual fingers)
             */
            for (int i = 0; i < imageList1.Count; i++)
            {
                FingerImage image1 = imageList1.Get(i);
                FingerPosition position1 = image1.Position;
                if (position1 == FingerPosition.Unknown || position1 > FingerPosition.PlainLeftThumb)
                {
                    // if the position is not in [1-12] is either unknown or a slap so we skip it (for this sample scenario)
                    image1.Dispose();
                    continue;
                }

                FingerTemplate template1 = fingerExtractor.CreateTemplate(image1);
                for (int j = 0; j < imageList2.Count; j++)
                {
                    FingerImage image2 = imageList2.Get(j);
                    FingerPosition position2 = image2.Position;
                    if (position1 == position2)
                    {
                        FingerTemplate template2 = fingerExtractor.CreateTemplate(image2);
                        int score = fingerMatcher.CompareTemplates(template1, template2);
                        if (score > (int)FingerMatcherThreshold.Fmr10000)
                        {
                            Console.Write("Match position " + position1.ToString() + ": " + score + "\n");
                        }
                        else
                        {
                            Console.Write("No Match position " + position1.ToString() + ": " + score + "\n");
                        }
                        template2.Dispose();
                    }
                    image2.Dispose();
                }
                template1.Dispose();
                image1.Dispose();
            }

            Console.WriteLine("Sample terminated successfully.");
            //Console.ReadKey();
        }
    }
}
