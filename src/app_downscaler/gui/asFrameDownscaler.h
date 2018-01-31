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
 * The Original Software is AtmoSwing.
 * The Original Software was developed at the University of Lausanne.
 * All Rights Reserved.
 * 
 */

/*
 * Portions Copyright 2017 Pascal Horton, University of Bern.
 */

#ifndef __asFrameDownscaler__
#define __asFrameDownscaler__

#include "AtmoswingDownscalerGui.h"
#include <asIncludes.h>
#include "asMethodDownscaler.h"
#include "asLogWindow.h"

class asFrameDownscaler
        : public asFrameDownscalerVirtual
{
public:
    asFrameDownscaler(wxWindow *parent);

    ~asFrameDownscaler();

    void OnInit();

protected:
    asLogWindow *m_logWindow;
    asMethodDownscaler *m_methodDownscaler;

    void Update();

    void OnSaveDefault(wxCommandEvent &event);

    void Launch(wxCommandEvent &event);

    void LoadOptions();

    void SaveOptions() const;

    void OpenFramePreferences(wxCommandEvent &event);

    void OpenFrameAbout(wxCommandEvent &event);

    void OnShowLog(wxCommandEvent &event);

    void OnLogLevel1(wxCommandEvent &event);

    void OnLogLevel2(wxCommandEvent &event);

    void OnLogLevel3(wxCommandEvent &event);

    void DisplayLogLevelMenu();

    void Cancel(wxCommandEvent &event);

};

#endif // __asFrameDownscaler__