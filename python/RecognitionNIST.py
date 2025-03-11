import os
import id3finger

print("---------------------------")
print("id3 Finger Recognition NIST")
print("---------------------------")

# Before calling any function of the SDK you must first check a valid license file.
# To get such a file please use the provided activation tool.
# It is also possible to use the FingerLicense.Activation(...) APIs
cw = os.getcwd()
id3finger.FingerLicense.check_license("../id3Finger.lic")

# The Finger SDK heavily relies on deep learning technics and hence requires trained models to run.
# Fill in the correct path to the downloaded models.
modelPath = "../models"

# Once a model is loaded in the desired processing unit (CPU or GPU) several instances of the associated processor can be created.
# For instance in this sample, we load:
# - a minutia detector to extract iso minutiae data (mandatory)
# - a fingerprint aligner and encoder to extract proprietary template data for better accuracy (optional)
print("Loading models... ")
id3finger.FingerLibrary.load_model(modelPath, id3finger.FingerModel.FINGER_MINUTIA_DETECTOR_3B, id3finger.ProcessingUnit.CPU) # mandatory
print("Done.")

# To use the finger minutia detector it is required to instanciate a finger extractor module and 
# then load the required model files into the memory of the desired processing unit.
# 
# Minutia detection is mandatory and is self-sufficient for creating iso templates.
# Setting other models will activate the generation of additional proprietary template data. See 
# SDK user guide for more details.
print("Init finger extractor... ")
fingerExtractor = id3finger.FingerExtractor(
    minutia_detector_model = id3finger.FingerModel.FINGER_MINUTIA_DETECTOR_3B, # setting the model to use is mandatory
    thread_count = 4)
print("Done.")

# Load fingerprint image from ANSI/NIST-ITL transactions
# - The SDK can handle ANSI/NIST-ITL 1-2011 Update 2015 transactions both in traditional and xml encoded formats
# - The SDK will return a FingerImageList containing all found images in records types 04 or 14. 
# - When available the finger position and the finger resolution will be retrieved
# 
# Transactions used in this sample are examples from https://www.nist.gov/itl/iad/image-group/ansinist-itl-standard-references
print("Loading images from ANSI/NIST-ITL transactions... ")
# Load from traditional encoded transation with type 04 records
imageList1 = id3finger.FingerImageRecord.from_file(id3finger.FingerImageRecordFormat.AN2K_2011_TRANSACTION_TRADITIONAL_ENCODING,"../data/an2k-type-04-tpcard.an2")
print(f"Found {imageList1.count} fingerprint images in the an2k-type-04-tpcard.an2 transaction")
# Load from xml encoded transation with type 14 records
imageList2 = id3finger.FingerImageRecord.from_file(id3finger.FingerImageRecordFormat.AN2K_2011_TRANSACTION_TRADITIONAL_ENCODING,"../data/an2k-type-14-tpcard.an2")
print(f"Found {imageList2.count} fingerprint images in the an2k-type-14-tpcard.an2 transaction")
print("Done.")

# Initialize a finger matcher instance.
print("Init finger matcher... ")
fingerMatcher = id3finger.FingerMatcher()
print("Done.")

# Perform matches between the two records.
# Several scenarii can be used, here we will:
# - only compare fingerprint with the same positions
# - skip the two or four finger slaps (which need to be processed using the FingerDetector module to separate individual fingers)
for i in range(imageList1.count):
    image1 = imageList1.get(i)
    position1 = image1.position
    if(position1 == id3finger.FingerPosition.UNKNOWN  or position1 > id3finger.FingerPosition.PLAIN_LEFT_THUMB):
        # if the position is not in [1-12] is either unknown or a slap so we skip it (for this sample scenario)
        del image1
        continue
        
    template1 = fingerExtractor.create_template(image1)
    for j in range(imageList2.count):
        image2 = imageList2.get(j)
        position2 = image2.position
        if(position1 == position2):
            template2 = fingerExtractor.create_template(image2)
            score = fingerMatcher.compare_templates(template1, template2)
            if (score > id3finger.FingerMatcherThreshold.FMR10000):
                print(f"Match position {position1.name}: {score}")
            else:
                print(f"No Match position {position1.name}: {score}")
            
            del template2     
        del image2

    del template1
    del image1

# Closing all SDK ressources
del imageList1
del imageList2
del fingerMatcher
del fingerExtractor
id3finger.FingerLibrary.unload_model(id3finger.FingerModel.FINGER_MINUTIA_DETECTOR_3B, id3finger.ProcessingUnit.CPU)
print("Sample terminated successfully.")
