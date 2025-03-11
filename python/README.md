# Finger SDK Python samples

This repository contains Python samples of **id3 Technologies** Finger SDK.

## Requirements

First of all you must follow the upper README steps to get a license file and install the SDK.

A python 3.9, 3.10 or 3.11 environnment must be available on the system.

## Installation

You can skip the SDK installation, as the python wrapper is delivered separately, but you still need to download the models.
Locate the .whl packages in the /python directory, and then install the package corresponding to your Python version

Once everything is setup you can proceed to the following steps.

```sh
python -m pip install .\id3finger-X.X.X-cpYY-cpYY-PLATFORM.whl
```

Where YY is your version of Python.

## Building the solution

### Filling the license path

Before to build any of the Python samples, you need to fill in the path to your license in the source code. Look for the following line in **RecognitionCLI.py** and replace expression 'your_license_path_here' with your correct path.

### Ensuring models are present

Following models are required to be in the sdk/models/ directory:

- finger_minutia_detector_v3b.id3nn

### Run

 ```sh
python RecognitionCLI.py
python RecognitionNIST.py
 ```
