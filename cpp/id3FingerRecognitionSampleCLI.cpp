/**
 * This sample shows how to perform basic finger recognition tasks using id3 Finger SDK.
 */

#include <iostream>
#include "id3FingerLib.h"

void check(int err, const std::string& func_name)
{
	if (err != id3FingerError_Success)
	{
		std::cout << "Error " << err << " in " << func_name.c_str() << std::endl;
		exit(1);
	}
}

int main(int argc, char **argv)
{
	std::string data_dir = "../../../data/";
	/**
   	 * Fill in the correct path to the license.
   	 */
	std::string license_path = "../../../id3Finger.lic";
	/**
   	 * Fill in the correct path to the downloaded models.
   	 */
	std::string models_dir = "../../../models/";
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
   	 * Load two finger images from files. Finger images are always loaded with 8 bits grayscale pixel format.
   	 */
	std::string image1_path = data_dir + "image1.bmp";
	std::cout << "Loading reference image: " << image1_path.c_str() << std::endl;
	ID3_FINGER_IMAGE reference_image;
	err = id3FingerImage_Initialize(&reference_image);
	err = id3FingerImage_FromFile(reference_image, image1_path.c_str(), id3FingerPixelFormat_Grayscale8Bits);
	check(err, "id3FingerImage_FromFile 1");

	std::string image2_path = data_dir + "image2.bmp";
	std::cout << "Loading probe image: " << image2_path.c_str() << std::endl;
	ID3_FINGER_IMAGE probe_image;
	err = id3FingerImage_Initialize(&probe_image);
	err = id3FingerImage_FromFile(probe_image, image2_path.c_str(), id3FingerPixelFormat_Grayscale8Bits);
	check(err, "id3FingerImage_FromFile 2");

	/**
	 * Set the resolution of the two previsously loaded finger images.
	 * Not doing so would result in a -2006 "Invalid resolution" error during finger template extraction
	 */
	err = id3FingerImage_SetResolution(reference_image,500);
	check(err, "id3FingerImage_SetResolution 1");
	err = id3FingerImage_SetResolution(probe_image,500);
	check(err, "id3FingerImage_SetResolution 2");

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

	ID3_FINGER_TEMPLATE reference_template;
	ID3_FINGER_TEMPLATE probe_template;
	/*
   	 * A finger template object contains the finger features that are used to recognize a user.
   	 * It needs to be initialized before to be filled by the function id3FingerExtractor_CreateTemplate().
   	 */
	err = id3FingerTemplate_Initialize(&reference_template);
	check(err, "id3FingerTemplate_Initialize");
	err = id3FingerTemplate_Initialize(&probe_template);
	check(err, "id3FingerTemplate_Initialize");
	/*
   	 * Create the templates
   	 */
	std::cout << "Creating reference template" << std::endl;
	err = id3FingerExtractor_CreateTemplate(extractor, reference_image, reference_template);
	check(err, "id3FingerExtractor_CreateTemplate");

	std::cout << "Creating probe template" << std::endl;
	err = id3FingerExtractor_CreateTemplate(extractor, probe_image, probe_template);
	check(err, "id3FingerExtractor_CreateTemplate");

	/**
   	 * Initialize a finger matcher.
   	 */
	ID3_FINGER_MATCHER matcher;
	err = id3FingerMatcher_Initialize(&matcher);
	check(err, "id3FingerMatcher_Initialize");
	/**
	 * The matching process returns a score between 0 and 65535. To take a decision this score
	 * must be compared to a defined threshold.
	 * For 1 to 1 authentication, it is recommended to select a threshold of at least 4000 (FMR=1:10K).
	 * Please see documentation to get more information on how to choose the threshold.
	 */
	std::cout << std::endl << "Comparing detected finger" << std::endl;
	int score = 0;
	err = id3FingerMatcher_CompareTemplates(matcher, probe_template, reference_template, &score);
	check(err, "id3FingerMatcher_CompareTemplates");
	std::cout << "   Score: " << score << std::endl;
	if (score > 4000)
	{
		std::cout << "   Result: MATCH " << std::endl;
	}
	else
	{
		std::cout << "   Result: NO MATCH " << std::endl;
	}

	/**
	 * Finger templates can be exported directly into a file.
	 */
	std::cout << "Export reference template as file" << std::endl;
	std::string reference_template_path = data_dir + "reference_template.bin";
	err = id3FingerTemplate_ToFile(reference_template,id3FingerTemplateFormat_CompactCardIso19794_2_2005,reference_template_path.c_str());
	check(err, "id3FingerTemplate_ToFile");
	/**
	 * Finger templates can also directly be exported into a buffer.
	 * Please notice how two calls are performed:
	 * - the first one allow to receive the required buffer size
	 * - the second one fill the allocated buffer with the template data
	 */
	std::cout << "Export probe template as buffer" << std::endl;
	int probe_template_buffer_size = 0;
	unsigned char* probe_template_buffer = nullptr;
	err = id3FingerTemplate_ToBuffer(probe_template,id3FingerTemplateFormat_CompactCardIso19794_2_2005,probe_template_buffer,&probe_template_buffer_size);
	if(err == id3FingerError_InsufficientBuffer) // expected error as an empty buffer has been provided
	{
		probe_template_buffer = (unsigned char*)malloc(probe_template_buffer_size);
		err = id3FingerTemplate_ToBuffer(probe_template,id3FingerTemplateFormat_CompactCardIso19794_2_2005,probe_template_buffer,&probe_template_buffer_size);
		check(err, "id3FingerTemplate_ToBuffer with allocated buffer");
		// probe_template_buffer now contains the exported template and could be stored, etc
		free(probe_template_buffer);
	}else 
	{
		check(err, "id3FingerTemplate_ToBuffer with empty buffer");
	}

	std::cout << std::endl << "Press any key..." << std::endl;
	/**
	 * Dispose of all objects and unload models.
	 */
	err = id3FingerMatcher_Dispose(&matcher);
	err = id3FingerTemplate_Dispose(&probe_template);
	err = id3FingerTemplate_Dispose(&reference_template);
	err = id3FingerExtractor_Dispose(&extractor);
	err = id3FingerImage_Dispose(&probe_image);
	err = id3FingerImage_Dispose(&reference_image);
	err = id3FingerLibrary_UnloadModel(id3FingerModel_FingerMinutiaDetector3B, id3FingerProcessingUnit_Cpu);
}
