/**
 * This sample shows how to perform basic finger recognition tasks using id3 Finger SDK.
 */

#include <iostream>
#include "id3FingerLib.h"

void check(int err, const std::string &func_name)
{
	if (err != id3FingerError_Success)
	{
		std::cout << "Error " << err << " in " << func_name.c_str() << std::endl;
		exit(1);
	}
}

int main(int argc, char **argv)
{
	std::string data_dir = "../../../../data/";
	/**
	 * Fill in the correct path to the license.
	 */
	std::string license_path = "../../../../id3Finger.lic";
	/**
	 * Fill in the correct path to the downloaded models.
	 */
	std::string models_dir = "../../../../models/";
	/**
	 * All functions of the API return an error code.
	 */
	int err = id3FingerError_Success;
	/**
	 * The id3 Finger SDK needs a valid license to work.
	 * It is required to provide a path to this license file.
	 * This file can either be retrived using the provided activation tools or the FingerLicense activation APIs.
	 * It is required to call the id3FingerLibrary_CheckLicense() function before calling any other function of the SDK.
	 */
	std::cout << "Checking license" << std::endl;
	err = id3FingerLicense_CheckLicense(license_path.c_str(), nullptr);
	check(err, "id3FingerLibrary_CheckLicense");

	/**
	 * Initialize a finger extractor.
	 */
	ID3_FINGER_EXTRACTOR extractor;
	/**
	 * To use the finger minutia detector it is required to instanciate a finger extractor module and
	 * then load the required model files into the the desired processing unit.
	 */
	std::cout << "Loading finger minutia detector 3B model" << std::endl;
	err = id3FingerLibrary_LoadModel(models_dir.c_str(), id3FingerModel_FingerMinutiaDetector3B, id3FingerProcessingUnit_Cpu);
	check(err, "id3FingerLibrary_LoadModel");
	/**
	 * Once the model is loaded, it is now possible to instantiate an ID3_FINGER_EXTRACTOR object.
	 */
	err = id3FingerExtractor_Initialize(&extractor);
	check(err, "id3FingerExtractor_Initialize");
	/**
	 * Once the instance is initialized, it is now possible to set its parameters.
	 * - MinutiaDetectorModel: default value is FingerMinutiaDetector3B which is the one used for latest id3
	 * MINEX III submission
	 * - ThreadCount: allocating more than 1 thread here can increase the speed of the process.
	 */
	err = id3FingerExtractor_SetMinutiaDetectorModel(extractor, id3FingerModel_FingerMinutiaDetector3B);
	check(err, "id3FingerExtractor_SetModel");
	err = id3FingerExtractor_SetThreadCount(extractor, 4);
	check(err, "id3FingerExtractor_SetThreadCount");

	/**
	 * Load fingerprint image from ANSI/NIST-ITL transactions
	 * - The SDK can handle ANSI/NIST-ITL 1-2011 Update 2015 transactions both in traditional and xml encoded formats
	 * - The SDK will return a FingerImageList containing all found images in records types 04 or 14.
	 * - When available the finger position and the finger resolution will be retrieved
	 *
	 * Transactions used in this sample are examples from https://www.nist.gov/itl/iad/image-group/ansinist-itl-standard-references
	 */
	std::cout << "Loading images from ANSI/NIST-ITL transactions... " << std::endl;
	
	// Load from traditional encoded transation with type 04 records
	ID3_FINGER_IMAGE_RECORD imageList1;
	err = id3FingerImageRecord_Initialize(&imageList1);
	check(err, "id3FingerImageRecord_Initialize");
	std::string imageList1_path = data_dir + "an2k-type-04-tpcard.an2";
	err = id3FingerImageRecord_FromFile(imageList1, id3FingerImageRecordFormat_An2k2011TransactionTraditionalEncoding, imageList1_path.c_str());
	check(err, "id3FingerImageRecord_FromFile");
	int imageList1Count;
	err = id3FingerImageRecord_GetCount(imageList1, &imageList1Count);
	check(err, "id3FingerImageRecord_GetCount");
	std::cout << "Found " << imageList1Count << " fingerprint images in the " << imageList1_path.c_str() << " transaction" << std::endl;

	//// Load from xml encoded transation with type 14 records
	ID3_FINGER_IMAGE_RECORD imageList2;
	err = id3FingerImageRecord_Initialize(&imageList2);
	check(err, "id3FingerImageRecord_Initialize");
	std::string imageList2_path = data_dir + "an2k-type-14-tpcard.an2";
	err = id3FingerImageRecord_FromFile(imageList2, id3FingerImageRecordFormat_An2k2011TransactionTraditionalEncoding, imageList2_path.c_str());
	check(err, "id3FingerImageRecord_FromFile");
	int imageList2Count;
	err = id3FingerImageRecord_GetCount(imageList2, &imageList2Count);
	check(err, "id3FingerImageRecord_GetCount");
	std::cout << "Found " << imageList2Count << " fingerprint images in the " << imageList2_path.c_str() << " transaction" << std::endl;
	std::cout << "Done." << std::endl;

	/**
	 * Initialize a finger matcher.
	 */
	ID3_FINGER_MATCHER matcher;
	err = id3FingerMatcher_Initialize(&matcher);
	check(err, "id3FingerMatcher_Initialize");

	/**
	 * Perform matches between the two records.
	 * Several scenarii can be used, here we will:
	 * - only compare fingerprint with the same positions
	 * - skip the two or four finger slaps (which need to be processed using the FingerDetector module to separate individual fingers)
	 */
	for (int i = 0; i < imageList1Count; i++)
	{
		ID3_FINGER_IMAGE image1;
		err = id3FingerImage_Initialize(&image1);
		check(err, "id3FingerImage_Initialize");
		err = id3FingerImageRecord_Get(imageList1, i, image1);
		check(err, "id3FingerImageRecord_Get");
		id3FingerPosition position1;
		err = id3FingerImage_GetPosition(image1, &position1);
		check(err, "id3FingerImage_GetPosition");
		if (position1 == id3FingerPosition_Unknown || position1 > id3FingerPosition_PlainLeftThumb)
		{
			// if the position is not in [1-12] is either unknown or a slap so we skip it (for this sample scenario)
			err = id3FingerImage_Dispose(&image1);
			check(err, "id3FingerImage_Dispose");
			continue;
		}

		ID3_FINGER_TEMPLATE template1;
		err = id3FingerTemplate_Initialize(&template1);
		check(err, "id3FingerTemplate_Initialize");
		err = id3FingerExtractor_CreateTemplate(extractor, image1, template1);
		check(err, "id3FingerExtractor_CreateTemplate");

		for (int j = 0; j < imageList2Count; j++)
		{
			ID3_FINGER_IMAGE image2;
			err = id3FingerImage_Initialize(&image2);
			check(err, "id3FingerImage_Initialize");
			err = id3FingerImageRecord_Get(imageList2, j, image2);
			check(err, "id3FingerImageRecord_Get");
			id3FingerPosition position2;
			err = id3FingerImage_GetPosition(image2, &position2);
			check(err, "id3FingerImage_GetPosition");
			if (position1 == position2)
			{
				ID3_FINGER_TEMPLATE template2;
				err = id3FingerTemplate_Initialize(&template2);
				check(err, "id3FingerTemplate_Initialize");
				err = id3FingerExtractor_CreateTemplate(extractor, image2, template2);
				check(err, "id3FingerExtractor_CreateTemplate");

				int score = 0;
				err = id3FingerMatcher_CompareTemplates(matcher, template1, template2, &score);
				check(err, "id3FingerMatcher_CompareTemplates");
				if (score > id3FingerMatcherThreshold_Fmr10000)
				{
					std::cout << "Match position " << position1 << ": " << score << std::endl;
				}
				else
				{
					std::cout << "No Match position " << position1 << ": " << score << std::endl;
				}
				err = id3FingerTemplate_Dispose(&template2);
				check(err, "id3FingerTemplate_Dispose");
			}
			err = id3FingerImage_Dispose(&image2);
			check(err, "id3FingerImage_Dispose");
		}
		err = id3FingerTemplate_Dispose(&template1);
		check(err, "id3FingerTemplate_Dispose");
		err = id3FingerImage_Dispose(&image1);
		check(err, "id3FingerImage_Dispose");
	}

	std::cout << std::endl
			  << "Sample terminated successfully" << std::endl;
	/**
	 * Dispose of all objects and unload models.
	 */
	err = id3FingerImageRecord_Dispose(&imageList1);
	err = id3FingerImageRecord_Dispose(&imageList2);
	err = id3FingerMatcher_Dispose(&matcher);
	err = id3FingerExtractor_Dispose(&extractor);
	err = id3FingerLibrary_UnloadModel(id3FingerModel_FingerMinutiaDetector3B, id3FingerProcessingUnit_Cpu);
}
