import eu.id3.finger.*;

public class RecognitionNIST {

    /*
     * This samples will show how to retrieve fingerprint images from ANSI/NIST-ITL 1-2011 transactions and 
     * compare them.
     * 
     * It is recommended to first have a look at the RecognitionCLI sample which deals with simple images before 
     * getting into this sample
     */
    
    public static void main(String[] args) {
        System.out.println("--------------------------------");
        System.out.println("id3.Face.Samples.RecognitionNIST");
        System.out.println("--------------------------------");

        /*
         * Before calling any function of the SDK you must first check a valid license file.
         * To get such a file please use the provided activation tool.
         * It is also possible to use the FingerLicense.Activation(...) APIs
         */
        FingerLibrary.checkLicense("your_license_path_here");

        /*
         * The Finger SDK heavily relies on deep learning technics and hence requires trained models to run.
         * Fill in the correct path to the downloaded models.
         */
        String modelPath = "../sdk/models";

        /*
         * Once a model is loaded in the desired processing unit (CPU or GPU) several instances of the associated processor can be created.
         * For instance in this sample, we will only load a minutia detector to extract iso minutiae data
         */
        System.out.println("Loading models... ");
        FingerLibrary.loadModel(modelPath, FingerModel.FINGER_MINUTIA_DETECTOR_3B, ProcessingUnit.CPU);
        System.out.println("Done.\n");

        /*
         * Initialize fingerprint extractor.
         */
        System.out.println("Init finger extractor... ");
        FingerExtractor fingerExtractor = new FingerExtractor();
        fingerExtractor.setMinutiaDetectorModel(FingerModel.FINGER_MINUTIA_DETECTOR_3B); // activate minutiae detection
        fingerExtractor.setThreadCount(4);
        System.out.println("Done.\n");

        /*
         * Load fingerprint image from ANSI/NIST-ITL transactions
         * - The SDK can handle ANSI/NIST-ITL 1-2011 Update 2015 transactions both in traditional and xml encoded formats
         * - The SDK will return a FingerImageList containing all found images in records types 04 or 14. 
         * - When available the finger position and the finger resolution will be retrieved
         * 
         * Transactions used in this sample are examples from https://www.nist.gov/itl/iad/image-group/ansinist-itl-standard-references
         */
        System.out.println("Loading images from ANSI/NIST-ITL transactions... ");
        // Load from traditional encoded transation with type 04 records
        FingerImageList imageList1 = FingerImageList.fromFile(FingerImageBufferType.AN2K_2011_TRANSACTION_TRADITIONAL_ENCODING,"../data/an2k-type-04-tpcard.an2");
        System.out.println("Found " + imageList1.getCount() + " fingerprint images in the an2k-type-04-tpcard.an2 transaction");
        // Load from xml encoded transation with type 14 records
        FingerImageList imageList2 = FingerImageList.fromFile(FingerImageBufferType.AN2K_2011_TRANSACTION_TRADITIONAL_ENCODING,"../data/an2k-type-14-tpcard.an2");
        System.out.println("Found " + imageList2.getCount() + " fingerprint images in the an2k-type-14-tpcard.an2 transaction");
        System.out.println("Done.\n");

        /*
         * Initialize a finger matcher instance.
         */
        FingerMatcher fingerMatcher = new FingerMatcher();

        /*
         * Perform matches between the two records.
         * Several scenarii can be used, here we will:
         * - only compare fingerprint with the same positions
         * - skip the two or four finger slaps (which need to be processed using the FingerDetector module to separate individual fingers)
         */
        for(int i = 0 ; i < imageList1.getCount() ; i++) {
            FingerImage image1 = imageList1.get(i);
            FingerPosition position1 = image1.getPosition();
            if(position1.getValue() == FingerPosition.UNKNOWN.getValue() 
                || position1.getValue() > FingerPosition.PLAIN_LEFT_THUMB.getValue() ) {
                    // if the position is not in [1-12] is either unknown or a slap so we skip it (for this sample scenario)
                    image1.close();
                    continue;
                }
            FingerTemplate template1 = fingerExtractor.createTemplate(image1);
            for(int j = 0 ; j < imageList2.getCount() ; j++) {
                FingerImage image2 = imageList2.get(j);
                FingerPosition position2 = image2.getPosition();
                if(position1 == position2) {
                    FingerTemplate template2 = fingerExtractor.createTemplate(image2);
                    int score = fingerMatcher.compareTemplates(template1, template2);
                    if (score > FingerMatcherThreshold.FMR10000.getValue()) {
                        System.out.println("Match position " + position1.name() + ": " + score);
                    } else{
                        System.out.println("No Match position " + position1.name() + ": " + score);
                    }
                    template2.close();
                }
                image2.close();
            }
            template1.close();
            image1.close();
        }
        
        /*
         * Closing all SDK ressources
         */
        imageList1.close();
        imageList2.close();
        fingerMatcher.close();
        fingerExtractor.close();
        FingerLibrary.unloadModel(FingerModel.FINGER_MINUTIA_DETECTOR_3B, ProcessingUnit.CPU);
        System.out.println("Sample terminated successfully.");
    }
}
