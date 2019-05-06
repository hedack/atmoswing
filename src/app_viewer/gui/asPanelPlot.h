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
 * Portions Copyright 2008-2013 Pascal Horton, University of Lausanne.
 */

#ifndef AS_PANEL_PLOT_H
#define AS_PANEL_PLOT_H

#include "asIncludes.h"
#include "AtmoswingViewerGui.h"
#include <wx/plotctrl/plotctrl.h>

class asPanelPlot
        : public wxPanel
{
public:
    explicit asPanelPlot(wxWindow *parent, wxWindowID id = wxID_ANY, const wxPoint &pos = wxDefaultPosition,
                         const wxSize &size = wxSize(-1, -1), long style = wxTAB_TRAVERSAL);

    void Print();

    void PrintPreview();

    void ExportSVG();

    wxPlotCtrl *GetPlotCtrl() const
    {
        wxASSERT(m_plotCtrl);
        return m_plotCtrl;
    }

protected:
    wxPlotCtrl *m_plotCtrl;

private:
    void OnPlotCtrl(wxPlotCtrlEvent &event);

DECLARE_EVENT_TABLE()

};

#endif
