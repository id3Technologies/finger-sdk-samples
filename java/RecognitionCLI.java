import eu.id3.finger.*;

public class RecognitionCLI {

    /*
     * This basic sample shows how to extract two finger templates and compare them
     */

    public static void main(String[] args) {
        System.out.println("---------------------------------");
        System.out.println("id3.Finger.Samples.RecognitionCLI");
        System.out.println("---------------------------------");

        /*
         * Before calling any function of the SDK you must first check a valid license
         * file.
         * To get such a file please use the provided activation tool.
         * It is also possible to use the FingerLicense.Activation(...) APIs
         */
        FingerLicense.checkLicense("../id3Finger.lic");

        /*
         * The Finger SDK heavily relies on deep learning technics and hence requires
         * trained models to run.
         * Fill in the correct path to the downloaded models.
         */
        String modelPath = "../models";
        /*
         * Once a model is loaded in the desired processing unit (CPU or GPU) several
         * instances of the associated processor can be created.
         * For instance in this sample, we load:
         * - a minutia detector to extract iso minutiae data (mandatory)
         * - a fingerprint aligner and encoder to extract proprietary template data for
         * better accuracy (optional)
         */
        System.out.println("Loading models... ");
        FingerLibrary.loadModel(modelPath, FingerModel.FINGER_MINUTIA_DETECTOR_3B, ProcessingUnit.CPU); // mandatory
        System.out.println("Done.\n");

        /*
         * Load sample images from files.
         * Finger images are always loaded to 8 bits grayscale pixel format
         */
        System.out.println("Loading images from files... ");
        FingerImage image1 = FingerImage.fromFile("../data/image1.bmp", PixelFormat.GRAYSCALE_8_BITS);
        FingerImage image2 = FingerImage.fromFile("../data/image2.bmp", PixelFormat.GRAYSCALE_8_BITS);
        System.out.println("Done.\n");

        /*
         * Set the resolution of the two previsously loaded finger images.
         * Not doing so would result in a -2006 "Invalid resolution" error during finger
         * template extraction
         * id3 Finger SDK processors uses only 500 dpi images. Images with a different
         * resolution must be rescaled
         * before use.
         * Note: the setResolution method set both the vertical and the horizontal
         * resolutions values at the same time.
         */
        image1.setResolution(500);
        image2.setResolution(500);

        /*
         * To use the finger minutia detector it is required to instanciate a finger
         * extractor module and
         * then load the required model files into the memory of the desired processing
         * unit.
         * 
         * Minutia detection is mandatory and is self-sufficient for creating iso
         * templates.
         * Setting other models will activate the generation of additional proprietary
         * template data. See
         * SDK user guide for more details.
         */
        System.out.println("Init finger extractor... ");
        FingerExtractor fingerExtractor = new FingerExtractor();
        fingerExtractor.setMinutiaDetectorModel(FingerModel.FINGER_MINUTIA_DETECTOR_3B); // activate minutiae detection
                                                                                         // (mandatory)
        fingerExtractor.setThreadCount(4);
        System.out.println("Done.\n");

        /*
         * Create the templates from each image.
         * As we activated proprietary encoding it will be processed at the same time
         */
        System.out.println("Creating templates... ");
        FingerTemplate referenceTemplate = fingerExtractor.createTemplate(image1);
        FingerTemplate probeTemplate = fingerExtractor.createTemplate(image2);
        System.out.println("Done.\n");

        /*
         * Initialize a finger matcher instance.
         */
        FingerMatcher fingerMatcher = new FingerMatcher();

        /*
         * The matching process returns a score between 0 and 65535. To take a decision
         * this score
         * must be compared to a defined threshold.
         * For 1 to 1 authentication, it is recommended to select a threshold of at
         * least 4000 (FMR=1:10K).
         * Please see documentation to get more information on how to choose the
         * threshold.
         */
        System.out.println("Comparing templates... ");
        int score = fingerMatcher.compareTemplates(referenceTemplate, probeTemplate);
        System.out.println("Done.\n");
        if (score > FingerMatcherThreshold.FMR10000.getValue()) {
            System.out.println("Match (minutiae and proprietary): " + score);
        } else {
            System.out.println("No match (minutiae and proprietary): " + score);
        }

        /*
         * By default the Finger Matcher instance uses all possible data found in the
         * provided templates.
         * Here for exemple it uses both the ISO minutiae data and the proprietary
         * encoded template
         * 
         * However it is possible to force it to only use ISO minutiae data as such.
         */
        fingerMatcher.setMinexOnly(true);

        System.out.println("Comparing templates minutiae only... ");
        int scoreMinex = fingerMatcher.compareTemplates(referenceTemplate, probeTemplate);
        System.out.println("Done.\n");

        if (scoreMinex > FingerMatcherThreshold.FMR10000.getValue()) {
            System.out.println("Match (minutiae only): " + scoreMinex);
        } else {
            System.out.println("No match (minutiae only): " + scoreMinex);
        }

        /*
         * Closing all SDK ressources
         */
        image1.close();
        image2.close();
        fingerMatcher.close();
        fingerExtractor.close();
        FingerLibrary.unloadModel(FingerModel.FINGER_ALIGNER_1A, ProcessingUnit.CPU);
        FingerLibrary.unloadModel(FingerModel.FINGER_ENCODER_1A, ProcessingUnit.CPU);
        FingerLibrary.unloadModel(FingerModel.FINGER_MINUTIA_DETECTOR_3B, ProcessingUnit.CPU);
        System.out.println("Sample terminated successfully.");
    }
}
