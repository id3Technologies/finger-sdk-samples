
// id3FingerCaptureSamples.h : fichier d'en-tête principal de l'application id3FingerCaptureSamples
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // symboles principaux


// id3CaptureSamplesApp :
// Consultez id3FingerCaptureSamples.cpp pour l'implémentation de cette classe
//

class id3CaptureSamplesApp : public CWinApp
{
public:
	id3CaptureSamplesApp() noexcept;


// Substitutions
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implémentation

public:
	afx_msg void OnAppAbout();
    afx_msg void OnFileOpen();
	DECLARE_MESSAGE_MAP()
};

extern id3CaptureSamplesApp theApp;
