#!/bin/bash
# Build the sample with the jar from the sdk
javac -cp ".:../sdk/java/eu.id3.finger.jar" RecognitionCLI.java
javac -cp ".:../sdk/java/eu.id3.finger.jar" RecognitionNIST.java

# add native library to current path
export LD_LIBRARY_PATH=../sdk/bin/linux/x64${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}

# Run the sample
java -cp ".:../sdk/java/eu.id3.finger.jar" RecognitionCLI
java -cp ".:../sdk/java/eu.id3.finger.jar" RecognitionNIST