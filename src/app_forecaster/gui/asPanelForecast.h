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
 * Portions Copyright 2013-2015 Pascal Horton, Terranum.
 */

#ifndef AS_PANEL_FORECAST
#define AS_PANEL_FORECAST

#include <wx/awx/led.h>

#include "AtmoswingForecasterGui.h"
#include "images.h"

class asPanelsManagerForecasts;

class asPanelForecast : public asPanelForecastVirtual {
 public:
  explicit asPanelForecast(wxWindow *parent);

  bool Layout() override;

  awxLed *GetLed() const {
    return m_led;
  }

  void SetPanelsManager(asPanelsManagerForecasts *panelManager) {
    m_panelsManager = panelManager;
  }

  wxString GetParametersFileName() const {
    return m_textCtrlParametersFileName->GetValue();
  }

  void SetParametersFileName(const wxString &val) {
    m_textCtrlParametersFileName->SetValue(val);
  }

 protected:
  wxWindow *m_parentFrame;
  awxLed *m_led;

  void ClosePanel(wxCommandEvent &event) override;

  void ChangeForecastName(wxCommandEvent &event);

 private:
  asPanelsManagerForecasts *m_panelsManager;
};

#endif
