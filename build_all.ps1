param($language = "all")

$cmake = "C:\Program Files\CMake\bin\cmake"
$msbuild17 = "C:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\Professional\\MSBuild\\15.0\\Bin\\msbuild.exe"
$msbuild22 = "C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\MSBuild\\Current\\Bin\\msbuild.exe"

function BuildCpp_id3FingerRecognitionSampleCLI {
    try
    {
        Write-Output "==============================================================================="
        Write-Output "Build cpp id3FingerRecognitionSampleCLI"
        Write-Output "==============================================================================="
        # c/c++ build
        Push-Location
        Copy-Item C:\ProgramData\id3\id3FingerToolkit_v3.lic id3Finger.lic
        Set-Location cpp/id3FingerRecognitionSampleCLI
        mkdir -Force build
        Set-Location build
        Remove-Item -Recurse -Force *
        # build x64
        & $cmake -G "Visual Studio 17 2022" -DWINDOWS_BUILD=ON ..
        & $cmake --build . --config Release
        if (-not $?)
        {
            throw
        }
        Set-Location Release
        .\id3FingerRecognitionSampleCLI.exe
        if ($LastExitCode -ne 0)
        {
            throw
        }
    }
    catch
    {
        Write-Output "Error in cpp id3FingerRecognitionSampleCLI"
        throw
    }
    finally
    {
        Pop-Location
    }
}

function BuildCpp_id3FingerRecognitionSampleNIST {
    try
    {
        Write-Output "==============================================================================="
        Write-Output "Build cpp id3FingerRecognitionSampleNIST"
        Write-Output "==============================================================================="
        # c/c++ build
        Push-Location
        Copy-Item C:\ProgramData\id3\id3FingerToolkit_v3.lic id3Finger.lic
        Set-Location cpp/id3FingerRecognitionSampleNIST
        mkdir -Force build
        Set-Location build
        Remove-Item -Recurse -Force *
        # build x64
        & $cmake -G "Visual Studio 17 2022" -DWINDOWS_BUILD=ON ..
        & $cmake --build . --config Release
        if (-not $?)
        {
            throw
        }
        Set-Location Release
        .\id3FingerRecognitionSampleNIST.exe
        if ($LastExitCode -ne 0)
        {
            throw
        }
    }
    catch
    {
        Write-Output "Error in cpp id3FingerRecognitionSampleNIST"
        throw
    }
    finally
    {
        Pop-Location
    }
}

function BuildCpp_id3FingerCaptureSamples {
    try
    {
        Write-Output "==============================================================================="
        Write-Output "Build cpp id3FingerCaptureSamples"
        Write-Output "==============================================================================="
        # c/c++ build
        Push-Location
        Copy-Item C:\ProgramData\id3\id3FingerToolkit_v3.lic id3Finger.lic
        Set-Location cpp
        & $msbuild17 id3FingerCaptureSamples.sln /t:Build /p:Configuration=Release
        if (-not $?)
        {
            throw
        }
    }
    catch
    {
        Write-Output "Error in cpp id3FingerCaptureSamples"
        throw
    }
    finally
    {
        Pop-Location
    }
}

function BuildCppAll {
    BuildCpp_id3FingerRecognitionSampleCLI
    BuildCpp_id3FingerRecognitionSampleNIST
    BuildCpp_id3FingerCaptureSamples
}

function BuildCppMFC {
    try
    {
        Write-Output "==============================================================================="
        Write-Output "Build cpp MFC"
        Write-Output "==============================================================================="
        # c/c++ build
        Push-Location
        Set-Location cpp/MFC
        & $msbuild17 id3DevicesSamples.sln /t:Build /p:Configuration=Release
        if (-not $?)
        {
            throw
        }
    }
    catch
    {
        Write-Output "Error in cpp x64"
        throw
    }
    finally
    {
        Pop-Location
    }
}

function BuildDotnet {
    try
    {
        Write-Output "==============================================================================="
        Write-Output "Build dotnet"
        Write-Output "==============================================================================="
        Copy-Item C:\ProgramData\id3\id3FingerToolkit_v3.lic id3Finger.lic
        Push-Location
        Set-Location dotnet
        #& "C:\\nuget\\nuget" restore id3.BioSeal.Samples.sln
        & $msbuild22 id3.Finger.Samples.sln /t:Build /p:Configuration=Release
        if (-not $?) 
        {
            throw
        }
        # execute cli samples
        Push-Location
        Set-Location .\id3.Finger.Samples.RecognitionCLI\bin\Release\
        .\id3.Finger.Samples.RecognitionCLI.exe
        if (-not $?) 
        {
            throw
        }
        Pop-Location
        Push-Location
        Set-Location .\id3.Finger.Samples.RecognitionNIST\bin\Release\
        .\id3.Finger.Samples.RecognitionNIST.exe
        Pop-Location
    }
    catch
    {
        Write-Output "Error in dotnet"
        throw
    }
    finally
    {
        Pop-Location
    }
}

function BuildJava {
    try
    {
        Write-Output "==============================================================================="
        Write-Output "Build java"
        Write-Output "==============================================================================="
        Copy-Item C:\ProgramData\id3\id3FingerToolkit_v3.lic id3Finger.lic
        Copy-Item sdk\bin\windows\x64\id3Finger.dll java\id3Finger.dll
        Push-Location
        # build
        Set-Location java
        java -cp ".;../sdk/java/eu.id3.finger.jar" RecognitionCLI.java
        if (-not $?) 
        {
            throw
        }
        java -cp ".;../sdk/java/eu.id3.finger.jar" RecognitionNIST.java
        if (-not $?) 
        {
            throw
        }
    }
    catch
    {
        Write-Output "Error in java"
        throw
    }
    finally
    {
        Pop-Location
    }
}

function BuildPython {
    try
    {
        Write-Output "==============================================================================="
        Write-Output "Build python"
        Write-Output "==============================================================================="
        Push-Location
        Set-Location python
        python -m venv sample-env
        sample-env\Scripts\activate
        python -m pip install opencv-python
        $pakname = Get-ChildItem -Name ..\sdk\python\id3finger-*-cp311-cp311-win_amd64.whl
        python -m pip install ..\sdk\python\$pakname
        # patch to replace numpy 2 by 1
        python -m pip install numpy<2
        python RecognitionCLI.py
        python RecognitionNIST.py
        deactivate
    }
    catch
    {
        Write-Output "Error in python"
        throw
    }
    finally
    {
        Pop-Location
    }
}

try
{
    switch  ($language)
    {
        "all" {
            # cpp
            BuildCppAll

            # dotnet
            BuildDotnet

            # java
            BuildJava

            # python
            BuildPython
        }
        "dotnet" {
            BuildDotnet
        }
        "java" {
            BuildJava
        }
        "python" {
            BuildPython
        }
        "cpp" {
            BuildCppAll
        }
        default { Write-Output "Unknown language $($language)" }
    }
    Pause
}
catch
{
}