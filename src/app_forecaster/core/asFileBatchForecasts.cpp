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
 * Portions Copyright 2014 Pascal Horton, Terr@num.
 */
 
#include "asFileBatchForecasts.h"

asFileBatchForecasts::asFileBatchForecasts(const wxString &FileName, const ListFileMode &FileMode)
:
asFileXml(FileName, FileMode)
{
    // FindAndOpen() processed by asFileXml
}

asFileBatchForecasts::~asFileBatchForecasts()
{
    //dtor
}

bool asFileBatchForecasts::InsertRootElement()
{
    if(!GoToFirstNodeWithPath("AtmoSwingFile")) return false;
    if(!InsertElement(wxEmptyString, "BatchForecasts")) return false;
    if(!GoToFirstNodeWithPath("BatchForecasts")) return false;
    return true;
}

bool asFileBatchForecasts::GoToRootElement()
{
    if(!GoToFirstNodeWithPath("AtmoSwingFile.BatchForecasts"))
    {
        asLogError(wxString::Format(_("The file %s is not an AtmoSwing batch file."), m_FileName.GetFullName()));
        return false;
    }
    return true;
}