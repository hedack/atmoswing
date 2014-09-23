/*
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can read the License at http://opensource.org/licenses/CDDL-1.0
 * See the License for the specific language governing permissions
 * and limitations under the License.
 * 
 * When distributing Covered Code, include this CDDL Header Notice in 
 * each file and include the License file (licence.txt). If applicable, 
 * add the following below this CDDL Header, with the fields enclosed
 * by brackets [] replaced by your own identifying information:
 * "Portions Copyright [year] [name of copyright owner]"
 * 
 * The Original Software is AtmoSwing. The Initial Developer of the 
 * Original Software is Pascal Horton of the University of Lausanne. 
 * All Rights Reserved.
 * 
 */

/*
 * Portions Copyright 2008-2013 University of Lausanne.
 */

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include "AtmoswingMainForecaster.h"

#include "asGeo.h"
#include "asGeoArea.h"
#include "asGeoAreaCompositeGrid.h"
#include "asGeoPoint.h"
#include "asTime.h"
#include "asTimeArray.h"
#include "asDataPredictand.h"
#include "asDataPredictandPrecipitation.h"
#include "asDataPredictandTemperature.h"
#include "asDataPredictorArchive.h"
#include "asResults.h"
#include "asResultsAnalogsDates.h"
#include "asResultsAnalogsValues.h"
#include "asFileNetcdf.h"
#include "asFileXml.h"
#include "asFileAscii.h"
#include "asFileDat.h"
#include "asPredictorCriteria.h"
#include "asProcessor.h"
#include "asConfig.h"



////////////CLEAN//////////////
//helper functions


AtmoswingFrameForecaster::AtmoswingFrameForecaster(wxFrame *frame)
    : asFrameMain(frame)
{
#if wxUSE_STATUSBAR
    wxLogStatus(_("Welcome to AtmoSwing %s."), asVersion::GetFullString().c_str());
#endif

    // Config file
    wxConfigBase *pConfig = wxFileConfig::Get();

    // Set default options
    SetDefaultOptions();

    // Create log window and file
    bool displayLogWindow;
    pConfig->Read("/General/DisplayLogWindow", &displayLogWindow, true);
    m_LogWindow = new asLogWindow(this, _("AtmoSwing log window"), displayLogWindow);
    Log().CreateFile("AtmoSwingForecaster.log");

    // Restore frame position and size
    int minHeight = 600, minWidth = 500;
    int x = pConfig->Read("/MainFrame/x", 50),
        y = pConfig->Read("/MainFrame/y", 50),
        w = pConfig->Read("/MainFrame/w", minWidth),
        h = pConfig->Read("/MainFrame/h", minHeight);
    wxRect screen = wxGetClientDisplayRect();
    if (x<screen.x-10) x = screen.x;
    if (x>screen.width) x = screen.x;
    if (y<screen.y-10) y = screen.y;
    if (y>screen.height) y = screen.y;
    if (w+x>screen.width) w = screen.width-x;
    if (w<minWidth) w = minWidth;
    if (w+x>screen.width) x = screen.width-w;
    if (h+y>screen.height) h = screen.height-y;
    if (h<minHeight) h = minHeight;
    if (h+y>screen.height) y = screen.height-h;
    Move(x, y);
    SetClientSize(w, h);
    Fit();

    // Get the GUI mode -> silent or not
    long guiOptions = pConfig->Read("/General/GuiOptions", 0l);
    if (guiOptions==0l)
    {
        g_SilentMode = true;
    }
    else
    {
        g_SilentMode = false;
        g_VerboseMode = false;
        if (guiOptions==2l)
        {
            g_VerboseMode = true;
        }
    }

}

void AtmoswingFrameForecaster::SetDefaultOptions()
{
    wxConfigBase *pConfig = wxFileConfig::Get();
    wxString pathSep = wxFileName::GetPathSeparator();

    // General
    long guiOptions = pConfig->Read("/General/GuiOptions", 1l);
    pConfig->Write("/General/GuiOptions", guiOptions);
    bool responsive;
    pConfig->Read("/General/Responsive", &responsive, true);
    pConfig->Write("/General/Responsive", responsive);
    long defaultLogLevel = 1; // = selection +1
    long logLevel = pConfig->Read("/General/LogLevel", defaultLogLevel);
    pConfig->Write("/General/LogLevel", logLevel);
    bool displayLogWindow;
    pConfig->Read("/General/DisplayLogWindow", &displayLogWindow, false);
    pConfig->Write("/General/DisplayLogWindow", displayLogWindow);

    // Internet
    int maxPrevStepsNb = 5;
    wxString maxPrevStepsNbStr = wxString::Format("%d", maxPrevStepsNb);
    wxString InternetMaxPrevStepsNb = pConfig->Read("/Internet/MaxPreviousStepsNb", maxPrevStepsNbStr);
    pConfig->Write("/Internet/MaxPreviousStepsNb", InternetMaxPrevStepsNb);
    int maxParallelRequests = 1;
    wxString maxParallelRequestsStr = wxString::Format("%d", maxParallelRequests);
    wxString InternetParallelRequestsNb = pConfig->Read("/Internet/ParallelRequestsNb", maxParallelRequestsStr);
    pConfig->Write("/Internet/ParallelRequestsNb", InternetParallelRequestsNb);
    bool restrictDownloads;
    pConfig->Read("/Internet/RestrictDownloads", &restrictDownloads, true);
    pConfig->Write("/Internet/RestrictDownloads", restrictDownloads);
    bool checkBoxProxy;
    pConfig->Read("/Internet/UsesProxy", &checkBoxProxy, false);
    pConfig->Write("/Internet/UsesProxy", checkBoxProxy);

    // Processing
    bool allowMultithreading;
    pConfig->Read("/Processing/AllowMultithreading", &allowMultithreading, true);
    pConfig->Write("/Processing/AllowMultithreading", allowMultithreading);
    int maxThreads = wxThread::GetCPUCount();
    if (maxThreads==-1) maxThreads = 2;
    wxString maxThreadsStr = wxString::Format("%d", maxThreads);
    wxString ProcessingMaxThreadNb = pConfig->Read("/Processing/MaxThreadNb", maxThreadsStr);
    pConfig->Write("/Processing/MaxThreadNb", ProcessingMaxThreadNb);
    long defaultMethod = (long)asMULTITHREADS;
    long ProcessingMethod = pConfig->Read("/Processing/Method", defaultMethod);
    if (!allowMultithreading)
    {
        ProcessingMethod = (long)asMULTITHREADS;
    }
    pConfig->Write("/Processing/Method", ProcessingMethod);
    long defaultLinAlgebra = (long)asLIN_ALGEBRA_NOVAR;
    long ProcessingLinAlgebra = pConfig->Read("/Processing/LinAlgebra", defaultLinAlgebra);
    pConfig->Write("/Processing/LinAlgebra", ProcessingLinAlgebra);

    pConfig->Flush();
}

AtmoswingFrameForecaster::~AtmoswingFrameForecaster()
{
    // Config file
    wxConfigBase *pConfig = wxFileConfig::Get();
    if ( pConfig == NULL )
        return;

    // Save the frame position
    int x, y, w, h;
    GetClientSize(&w, &h);
    GetPosition(&x, &y);
    pConfig->Write("/MainFrame/x", (long) x);
    pConfig->Write("/MainFrame/y", (long) y);
    pConfig->Write("/MainFrame/w", (long) w);
    pConfig->Write("/MainFrame/h", (long) h);

    //wxDELETE(m_LogWindow);
}

void AtmoswingFrameForecaster::OnClose(wxCloseEvent &event)
{
    Close(true);
}

void AtmoswingFrameForecaster::OnQuit(wxCommandEvent &event)
{
    Close(true);
}
