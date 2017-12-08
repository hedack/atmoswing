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

#ifndef ASDATAPREDICTAND_H
#define ASDATAPREDICTAND_H

#include <asIncludes.h>

class asCatalogPredictands;

class asFileNetcdf;


class asPredictand
        : public wxObject
{
public:
    enum Parameter
    {
        Precipitation,
        AirTemperature,
        Lightnings,
        Wind
    };

    enum Unit
    {
        nb, mm, m, in, percent, degC, degK
    };

    enum TemporalResolution
    {
        Daily, SixHourly, Hourly, OneHourlyMTW, ThreeHourlyMTW, SixHourlyMTW, TwelveHourlyMTW, TwoDays, ThreeDays, Weekly
    };

    enum SpatialAggregation
    {
        Station, Groupment, Catchment
    };

    asPredictand(Parameter parameter, TemporalResolution temporalResolution,
                     SpatialAggregation spatialAggregation);

    virtual ~asPredictand();

    static Parameter StringToParameterEnum(const wxString &parameterStr);

    static wxString ParameterEnumToString(Parameter parameter);

    static Unit StringToUnitEnum(const wxString &unitStr);

    static TemporalResolution StringToTemporalResolutionEnum(const wxString &temporalResolution);

    static wxString TemporalResolutionEnumToString(TemporalResolution temporalResolution);

    static SpatialAggregation StringToSpatialAggregationEnum(const wxString &spatialAggregation);

    static wxString SpatialAggregationEnumToString(SpatialAggregation spatialAggregation);

    static asPredictand *GetInstance(const wxString &parameterStr, const wxString &temporalResolutionStr,
                                     const wxString &spatialAggregationStr);

    static asPredictand *GetInstance(Parameter parameter, TemporalResolution temporalResolution,
                                         SpatialAggregation spatialAggregation);

    static asPredictand *GetInstance(const wxString &filePath);

    virtual bool Load(const wxString &AlternateFilePath = wxEmptyString) = 0;

    virtual bool Save(const wxString &AlternateFilePath = wxEmptyString) const = 0;

    virtual bool BuildPredictandDB(const wxString &catalogFilePath, const wxString &AlternateDataDir = wxEmptyString,
                                   const wxString &AlternatePatternDir = wxEmptyString,
                                   const wxString &AlternateDestinationDir = wxEmptyString) = 0;

    virtual a1f GetReferenceAxis() const
    {
        a1f nodata(1);
        nodata << NaNf;
        return nodata;
    }

    virtual float GetReferenceValue(int iStat, double duration, float reference) const
    {
        return NaNf;
    }

    virtual a2f GetReferenceValuesArray() const
    {
        a1f nodata(1);
        nodata << NaNf;
        return nodata;
    }

    wxString GetDatasetId() const
    {
        return m_datasetId;
    }

    Parameter GetDataParameter() const
    {
        return m_parameter;
    }

    void SetDataParameter(Parameter val)
    {
        m_parameter = val;
    }

    TemporalResolution GetDataTemporalResolution() const
    {
        return m_temporalResolution;
    }

    void SetDataTemporalResolution(TemporalResolution val)
    {
        m_temporalResolution = val;
    }

    SpatialAggregation GetDataSpatialAggregation() const
    {
        return m_spatialAggregation;
    }

    void SetDataSpatialAggregation(SpatialAggregation val)
    {
        m_spatialAggregation = val;
    }

    bool HasNormalizedData() const
    {
        return m_hasNormalizedData;
    }

    void SetHasReferenceValues(bool val)
    {
        m_hasReferenceValues = val;
        m_hasNormalizedData = val;
    }

    bool HasReferenceValues() const
    {
        return m_hasReferenceValues;
    }

    double GetTimeStepDays() const
    {
        return m_timeStepDays;
    }

    double GetTimeStepHours() const
    {
        return m_timeStepDays * 24;
    }

    int GetStationsNb() const
    {
        return m_stationsNb;
    }

    int GetTimeLength() const
    {
        return m_timeLength;
    }

    vwxs &GetStationNamesArray()
    {
        return m_stationNames;
    }

    vwxs &GetStationOfficialIdsArray()
    {
        return m_stationOfficialIds;
    }

    a1f &GetStationHeightsArray()
    {
        return m_stationHeights;
    }

    a1i &GetStationsIdArray()
    {
        return m_stationIds;
    }

    a1d &GetStationXCoordsArray()
    {
        return m_stationXCoords;
    }

    a1d &GetStationYCoordsArray()
    {
        return m_stationYCoords;
    }

    a2f &GetDataGross()
    {
        return m_dataGross;
    }

    a1f GetDataGrossStation(int predictandStationId) const
    {
        int indexStation = GetStationIndex(predictandStationId);
        return m_dataGross.col(indexStation);
    }

    a2f &GetDataNormalized()
    {
        if (m_hasNormalizedData) {
            return m_dataNormalized;
        } else {
            return m_dataGross;
        }
    }

    a1f GetDataNormalizedStation(int predictandStationId) const
    {
        int indexStation = GetStationIndex(predictandStationId);
        if (m_hasNormalizedData) {
            return m_dataNormalized.col(indexStation);
        } else {
            return m_dataGross.col(indexStation);
        }
    }

    a1d &GetTime()
    {
        return m_time;
    }

    int GetStationIndex(int stationId) const;

protected:
    // Single value
    float m_fileVersion;
    Parameter m_parameter;
    TemporalResolution m_temporalResolution;
    SpatialAggregation m_spatialAggregation;
    wxString m_datasetId;
    double m_timeStepDays;
    int m_timeLength;
    int m_stationsNb;
    double m_dateProcessed;
    double m_dateStart;
    double m_dateEnd;
    bool m_hasNormalizedData;
    bool m_hasReferenceValues;
    // Matrix data
    a2f m_dataGross;
    a2f m_dataNormalized;
    // Vector (dim = time)
    a1d m_time;
    // Vector (dim = stations)
    vwxs m_stationNames;
    vwxs m_stationOfficialIds;
    a1i m_stationIds;
    a1f m_stationHeights;
    a1d m_stationXCoords;
    a1d m_stationYCoords;
    a1d m_stationStarts;
    a1d m_stationEnds;

    wxString GetDBFilePathSaving(const wxString &destinationDir) const;

    bool InitMembers(const wxString &catalogFilePath);

    bool InitBaseContainers();

    bool LoadCommonData(asFileNetcdf &ncFile);

    void SetCommonDefinitions(asFileNetcdf &ncFile) const;

    bool SaveCommonData(asFileNetcdf &ncFile) const;

    bool ParseData(const wxString &catalogFilePath, const wxString &AlternateDataDir = wxEmptyString,
                   const wxString &AlternatePatternDir = wxEmptyString);

    a2f GetAnnualMax(double timeStepDays = 1, int nansNbMax = 10) const;

    bool SetStationProperties(asCatalogPredictands &currentData, size_t stationIndex);

    bool GetFileContent(asCatalogPredictands &currentData, size_t stationIndex,
                        const wxString &AlternateDataDir = wxEmptyString,
                        const wxString &AlternatePatternDir = wxEmptyString);

private:

    float ParseAndCheckDataValue(asCatalogPredictands &currentData, wxString &dataStr) const;
};

#endif // ASDATAPREDICTAND_H