# Finger SDK Python samples

This repository contains Python samples of **id3 Technologies** Finger SDK.

## Requirements

First of all you must follow the upper README steps to get a license file and install the SDK.

A python 3.9 or 3.11 environnment must be available on the system.

Once everything is setup you can proceed to the following steps.

## Building the solution

### Filling the license path

Before to build any of the Python samples, you need to fill in the path to your license in the source code. Look for the following line in **RecognitionCLI.py** and replace expression 'your_license_path_here' with your correct path.

### Ensuring models are present

Following models are required to be in the sdk/models/ directory:
- For RecognitionCLI sample:
    - finger_minutia_detector_v3b.id3nn
    - finger_aligner_v1a.id3nn
    - finger_encoder_v1a.id3nn
- For RecognitionNIST sample:
    - finger_minutia_detector_v3b.id3nn

