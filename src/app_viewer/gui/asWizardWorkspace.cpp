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
 * Portions Copyright 2014 Pascal Horton, Terr@num.
 */
 
#include "asWizardWorkspace.h"

#include <wx/statline.h>
#include <wx/stdpaths.h>

wxDEFINE_EVENT(asEVT_ACTION_OPEN_WORKSPACE, wxCommandEvent);

asWizardWorkspace::asWizardWorkspace( wxWindow* parent, asWorkspace* workspace, wxWindowID id )
:
asWizardWorkspaceVirtual( parent, id )
{
    m_Workspace = workspace;
}

asWizardWorkspace::~asWizardWorkspace()
{

}

void asWizardWorkspace::OnWizardFinished( wxWizardEvent& event )
{
    wxString filePath = m_FilePickerWorkspaceFile->GetPath();
    m_Workspace->SetFilePath(filePath);
    wxString dirPath = m_DirPickerForecastResults->GetPath();
    m_Workspace->SetForecastsDirectory(dirPath);

    int baseMapSlct = m_ChoiceBaseMap->GetSelection();
    wxString baseMapPath = wxEmptyString;
    wxString wmsDir = wxStandardPaths::Get().GetDataDir();
    wmsDir = wmsDir + DS + "data" + DS + "wms" + DS + "basemaps" + DS;
    switch (baseMapSlct)
    {
        case 0: // Custom layers

            break;
        case 1: // Terrain from Google maps (recommended)
            baseMapPath = wmsDir + "GoogleMaps-mix.xml";
            break;
        case 2: // Map from Google maps
            baseMapPath = wmsDir + "GoogleMaps-map.xml";
            break;
        case 3: // Map from Openstreetmap
            baseMapPath = wmsDir + "OpenStreetMap.xml";
            break;
        case 4: // Map from ArcGIS Mapserver
            baseMapPath = wmsDir + "ArcgisMapserver.xml";
            break;
        case 5: // Satellite imagery from Google maps
            baseMapPath = wmsDir + "GoogleMaps-sat.xml";
            break;
        case 6: // Satellite imagery from VirtualEarth
            baseMapPath = wmsDir + "VirtualEarth.xml";
            break;
    }

    if (!baseMapPath.IsEmpty())
    {
        m_Workspace->AddLayer();
        m_Workspace->SetLayerPath(0, baseMapPath);
        m_Workspace->SetLayerTransparency(0, 0);
        m_Workspace->SetLayerType(0, "wms");
        m_Workspace->SetLayerVisibility(0, true);
    }

    m_Workspace->Save();
    
    if (!filePath.IsEmpty())
    {
        wxConfigBase *pConfig = wxFileConfig::Get();
        pConfig->Write("/Workspace/LastOpened", filePath);
    }
}

void asWizardWorkspace::OnLoadExistingWorkspace( wxCommandEvent& event )
{
    wxCommandEvent eventOpen (asEVT_ACTION_OPEN_WORKSPACE);
    GetParent()->ProcessWindowEvent(eventOpen);
    Close();
}