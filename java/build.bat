rem Build the sample with the jar from the sdk
javac -cp ".;../sdk/java/eu.id3.finger.jar" RecognitionCLI.java
javac -cp ".;../sdk/java/eu.id3.finger.jar" RecognitionNIST.java

rem Copy native library to current directory
copy ..\\sdk\\bin\\windows\\x64\\id3Finger.dll id3Finger.dll

rem Run the samples
java -cp ".;../sdk/java/eu.id3.finger.jar" RecognitionCLI
java -cp ".;../sdk/java/eu.id3.finger.jar" RecognitionNIST