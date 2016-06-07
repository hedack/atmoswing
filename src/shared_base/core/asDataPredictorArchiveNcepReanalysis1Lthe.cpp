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
 * Portions Copyright 2014-2015 Pascal Horton, Terranum.
 */

#include "asDataPredictorArchiveNcepReanalysis1Lthe.h"

#include <asTimeArray.h>
#include <asGeoAreaCompositeGrid.h>
#include <asFileNetcdf.h>


asDataPredictorArchiveNcepReanalysis1Lthe::asDataPredictorArchiveNcepReanalysis1Lthe(const wxString &dataId)
        : asDataPredictorArchiveNcepReanalysis1Subset(dataId)
{
    // Set the basic properties.
    m_initialized = false;
    m_datasetId = "NCEP_Reanalysis_v1_lthe";
    m_originalProvider = "NCEP/NCAR";
    m_transformedBy = "LTHE";
    m_datasetName = "Reanalysis 1 subset from LTHE";
    m_originalProviderStart = asTime::GetMJD(1948, 1, 1);
    m_originalProviderEnd = NaNDouble;
    m_timeZoneHours = 0;
    m_timeStepHours = 24;
    m_firstTimeStepHours = 0;
    m_nanValues.push_back(32767);
    m_nanValues.push_back(936 * std::pow(10.f, 34.f));
    m_xAxisShift = 0;
    m_yAxisShift = 0;
    m_xAxisStep = 2.5;
    m_yAxisStep = 2.5;
    m_subFolder = wxEmptyString;
    m_fileAxisLatName = "lat";
    m_fileAxisLonName = "lon";
    m_fileAxisTimeName = "time";
    m_fileAxisLevelName = "level";

    // Identify data ID and set the corresponding properties.
    if (m_dataId.IsSameAs("hgt_500hPa", false)) {
        m_parameter = GeopotentialHeight;
        m_parameterName = "Geopotential Height";
        m_fileNamePattern = "NCEP_Reanalysis_v1_lthe_hgt_500hPa.nc";
        m_fileVariableName = "hgt";
        m_unit = m;
        m_firstTimeStepHours = 0;
        m_timeStepHours = 24;
    } else if (m_dataId.IsSameAs("hgt_1000hPa", false)) {
        m_parameter = GeopotentialHeight;
        m_parameterName = "Geopotential Height";
        m_fileNamePattern = "NCEP_Reanalysis_v1_lthe_hgt_1000hPa.nc";
        m_fileVariableName = "hgt";
        m_unit = m;
        m_firstTimeStepHours = 12;
        m_timeStepHours = 24;
    } else if (m_dataId.IsSameAs("prwtr", false)) {
        m_parameter = PrecipitableWater;
        m_parameterName = "Precipitable water";
        m_fileNamePattern = "NCEP_Reanalysis_v1_lthe_prwtr.nc";
        m_fileVariableName = "pwa";
        m_unit = mm;
        m_firstTimeStepHours = 0;
        m_timeStepHours = 12;
    } else if (m_dataId.IsSameAs("rhum", false)) {
        m_parameter = RelativeHumidity;
        m_parameterName = "Relative Humidity";
        m_fileNamePattern = "NCEP_Reanalysis_v1_lthe_rhum.nc";
        m_fileVariableName = "rhum";
        m_unit = percent;
        m_firstTimeStepHours = 0;
        m_timeStepHours = 12;
    } else {
        asThrowException(wxString::Format(_("No '%s' parameter identified for the provided level type (%s)."),
                                          m_dataId, LevelEnumToString(m_levelType)));
    }
}

asDataPredictorArchiveNcepReanalysis1Lthe::~asDataPredictorArchiveNcepReanalysis1Lthe()
{

}

bool asDataPredictorArchiveNcepReanalysis1Lthe::Init()
{
    // Check data ID
    if (m_fileNamePattern.IsEmpty() || m_fileVariableName.IsEmpty()) {
        asLogError(
                wxString::Format(_("The provided data ID (%s) does not match any possible option in the dataset %s."),
                                 m_dataId, m_datasetName));
        return false;
    }

    // Check directory is set
    if (GetDirectoryPath().IsEmpty()) {
        asLogError(
                wxString::Format(_("The path to the directory has not been set for the data %s from the dataset %s."),
                                 m_dataId, m_datasetName));
        return false;
    }

    // Set to initialized
    m_initialized = true;

    return true;
}