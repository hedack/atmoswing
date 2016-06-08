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

#include "asMethodCalibrator.h"
#include "asThreadPreloadData.h"

#ifndef UNIT_TESTING

#include "AtmoswingAppOptimizer.h"

#endif

asMethodCalibrator::asMethodCalibrator()
        : asMethodStandard()
{
    m_predictorDataDir = wxEmptyString;
    m_paramsFilePath = wxEmptyString;
    m_preloaded = false;
    m_validationMode = false;
    m_scoreValid = NaNFloat;

    // Seeds the random generator
    asTools::InitRandom();
}

asMethodCalibrator::~asMethodCalibrator()
{
    DeletePreloadedData();
}

bool asMethodCalibrator::Manager()
{
    // Set unresponsive to speedup
    g_responsive = false;

    // Reset the score of the climatology
    m_scoreClimatology.clear();

    // Seeds the random generator
    asTools::InitRandom();

    // Load parameters
    asParametersCalibration params;
    if (!params.LoadFromFile(m_paramsFilePath)) {
        return false;
    }
    params.InitValues();
    m_originalParams = params;

    // Load the Predictand DB
    if (!LoadPredictandDB(m_predictandDBFilePath)) {
        return false;
    }

    // Get the score order
    LoadScoreOrder(params);

    // Watch
    wxStopWatch sw;

    // Calibrate
    if (Calibrate(params)) {
        // Display processing time
        asLogMessageImportant(wxString::Format(_("The whole processing took %ldms to execute"), sw.Time()));
        asLogState(_("Calibration over."));
    } else {
        asLogError(_("The parameters could not be calibrated"));
    }

    // Delete preloaded data and cleanup
    DeletePreloadedData();

    return true;
}

void asMethodCalibrator::LoadScoreOrder(asParametersCalibration &params)
{
    asForecastScore *forecastScore = asForecastScore::GetInstance(params.GetForecastScoreName());
    Order scoreOrder = forecastScore->GetOrder();
    SetScoreOrder(scoreOrder);
    wxDELETE(forecastScore);
}

void asMethodCalibrator::ClearAll()
{
    m_parametersTemp.clear();
    m_scoresCalibTemp.clear();
    m_parameters.clear();
    m_scoresCalib.clear();
    m_scoreValid = NaNFloat;
}

void asMethodCalibrator::ClearTemp()
{
    m_parametersTemp.clear();
    m_scoresCalibTemp.clear();
}

void asMethodCalibrator::PushBackBestTemp()
{
    SortScoresAndParametersTemp();
    PushBackFirstTemp();
}

void asMethodCalibrator::RemoveNaNsInTemp()
{
    wxASSERT(m_parametersTemp.size() == m_scoresCalibTemp.size());

    std::vector<asParametersCalibration> copyParametersTemp;
    VectorFloat copyScoresCalibTemp;

    for (unsigned int i = 0; i < m_scoresCalibTemp.size(); i++) {
        if (!asTools::IsNaN(m_scoresCalibTemp[i])) {
            copyScoresCalibTemp.push_back(m_scoresCalibTemp[i]);
            copyParametersTemp.push_back(m_parametersTemp[i]);
        }
    }

    m_scoresCalibTemp = copyScoresCalibTemp;
    m_parametersTemp = copyParametersTemp;

    wxASSERT(m_parametersTemp.size() == m_scoresCalibTemp.size());
    wxASSERT(m_parametersTemp.size() > 0);
}

void asMethodCalibrator::KeepBestTemp()
{
    SortScoresAndParametersTemp();
    KeepFirstTemp();
}

void asMethodCalibrator::PushBackFirstTemp()
{
    m_parameters.push_back(m_parametersTemp[0]);
    m_scoresCalib.push_back(m_scoresCalibTemp[0]);
}

void asMethodCalibrator::KeepFirstTemp()
{
    wxASSERT(m_parameters.size() > 0);
    wxASSERT(m_parametersTemp.size() > 0);
    wxASSERT(m_scoresCalibTemp.size() > 0);
    m_parameters[0] = m_parametersTemp[0];
    if (m_scoresCalib.size() == 0) {
        m_scoresCalib.push_back(m_scoresCalibTemp[0]);
    } else {
        m_scoresCalib[0] = m_scoresCalibTemp[0];
    }
}

void asMethodCalibrator::SortScoresAndParametersTemp()
{
    wxASSERT(m_scoresCalibTemp.size() == m_parametersTemp.size());
    wxASSERT(m_scoresCalibTemp.size() > 0);
    wxASSERT(m_parametersTemp.size() > 0);

    if (m_parametersTemp.size() == 1)
        return;

    // Sort according to the score
    Array1DFloat vIndices = Array1DFloat::LinSpaced(Eigen::Sequential, m_scoresCalibTemp.size(), 0,
                                                    m_scoresCalibTemp.size() - 1);
    asTools::SortArrays(&m_scoresCalibTemp[0], &m_scoresCalibTemp[m_scoresCalibTemp.size() - 1], &vIndices[0],
                        &vIndices[m_scoresCalibTemp.size() - 1], m_scoreOrder);

    // Sort the parameters sets as the scores
    std::vector<asParametersCalibration> copyParameters;
    for (unsigned int i = 0; i < m_scoresCalibTemp.size(); i++) {
        copyParameters.push_back(m_parametersTemp[i]);
    }
    for (unsigned int i = 0; i < m_scoresCalibTemp.size(); i++) {
        int index = (int) vIndices(i);
        m_parametersTemp[i] = copyParameters[index];
    }
}

bool asMethodCalibrator::PushBackInTempIfBetter(asParametersCalibration &params,
                                                asResultsAnalogsForecastScoreFinal &scoreFinal)
{
    float thisScore = scoreFinal.GetForecastScore();

    switch (m_scoreOrder) {
        case Asc:
            if (thisScore < m_scoresCalib[0]) {
                m_parametersTemp.push_back(params);
                m_scoresCalibTemp.push_back(thisScore);
                return true;
            }
            break;

        case Desc:
            if (thisScore > m_scoresCalib[0]) {
                m_parametersTemp.push_back(params);
                m_scoresCalibTemp.push_back(thisScore);
                return true;
            }
            break;

        default:
            asThrowException(_("The score order is not correctly defined."));
    }

    return false;
}

bool asMethodCalibrator::KeepIfBetter(asParametersCalibration &params, asResultsAnalogsForecastScoreFinal &scoreFinal)
{
    float thisScore = scoreFinal.GetForecastScore();

    switch (m_scoreOrder) {
        case Asc:
            if (thisScore < m_scoresCalib[0]) {
                wxASSERT(m_parameters.size() > 0);
                wxASSERT(m_scoresCalib.size() > 0);
                m_parameters[0] = params;
                m_scoresCalib[0] = thisScore;
                return true;
            }
            break;

        case Desc:
            if (thisScore > m_scoresCalib[0]) {
                wxASSERT(m_parameters.size() > 0);
                wxASSERT(m_scoresCalib.size() > 0);
                m_parameters[0] = params;
                m_scoresCalib[0] = thisScore;
                return true;
            }
            break;

        default:
            asThrowException(_("The score order is not correctly defined."));
    }

    return false;
}

bool asMethodCalibrator::SetSelectedParameters(asResultsParametersArray &results)
{
    // Extract selected parameters & best parameters
    for (unsigned int i = 0; i < m_parameters.size(); i++) {
        results.Add(m_parameters[i], m_scoresCalib[i], m_scoreValid);
    }

    return true;
}

bool asMethodCalibrator::SetBestParameters(asResultsParametersArray &results)
{
    wxASSERT(m_parameters.size() > 0);
    wxASSERT(m_scoresCalib.size() > 0);

    // Extract selected parameters & best parameters
    float bestScore = m_scoresCalib[0];
    int bestScoreRow = 0;

    for (unsigned int i = 0; i < m_parameters.size(); i++) {
        if (m_scoreOrder == Asc) {
            if (m_scoresCalib[i] < bestScore) {
                bestScore = m_scoresCalib[i];
                bestScoreRow = i;
            }
        } else {
            if (m_scoresCalib[i] > bestScore) {
                bestScore = m_scoresCalib[i];
                bestScoreRow = i;
            }
        }
    }

    if (bestScoreRow != 0) {
        // Re-validate
        Validate(bestScoreRow);
    }

    results.Add(m_parameters[bestScoreRow], m_scoresCalib[bestScoreRow], m_scoreValid);

    return true;
}

wxString asMethodCalibrator::GetPredictandStationIdsList(VectorInt &stationIds) const
{
    wxString id;

    if (stationIds.size() == 1) {
        id << stationIds[0];
    } else {
        for (int i = 0; i < (int) stationIds.size(); i++) {
            id << stationIds[i];
            if (i < (int) stationIds.size() - 1) {
                id << ",";
            }
        }
    }

    return id;
}

bool asMethodCalibrator::PreloadData(asParametersScoring &params)
{
    bool parallelDataLoad = false;
    ThreadsManager().CritSectionConfig().Enter();
    wxFileConfig::Get()->Read("/General/ParallelDataLoad", &parallelDataLoad, true);
    ThreadsManager().CritSectionConfig().Leave();

    // Load data once.
    if (!m_preloaded) {
        // Set preload to true here, so cleanup is made in case of exceptions.
        m_preloaded = true;

        if (parallelDataLoad) {
            asLogMessage(_("Preloading data with threads."));
        }

        // Resize container
        InitializePreloadedDataContainer(params);

        for (int i_step = 0; i_step < params.GetStepsNb(); i_step++) {
            for (int i_ptor = 0; i_ptor < params.GetPredictorsNb(i_step); i_ptor++) {
                if (params.NeedsPreloading(i_step, i_ptor)) {

                    if (params.NeedsPreprocessing(i_step, i_ptor)) {
                        if (PointersShared(params, i_step, i_ptor, 0)) {
                            continue;
                        }
                        if (!PreloadDataWithPreprocessing(params, i_step, i_ptor)) {
                            return false;
                        }
                    } else {
                        for (int i_dat = 0; i_dat < params.GetPredictorDataIdVector(i_step, i_ptor).size(); i_dat++) {
                            if (PointersShared(params, i_step, i_ptor, i_dat)) {
                                continue;
                            }
                            if (parallelDataLoad) {
                                asThreadPreloadData *thread = new asThreadPreloadData(this, params, i_step, i_ptor,
                                                                                      i_dat);
                                if (!ThreadsManager().HasFreeThread(thread->GetType())) {
                                    ThreadsManager().WaitForFreeThread(thread->GetType());
                                }
                                ThreadsManager().AddThread(thread);
                            } else {
                                if (!PreloadDataWithoutPreprocessing(params, i_step, i_ptor, i_dat)) {
                                    return false;
                                }
                            }
                        }
                    }
                }

                if (parallelDataLoad) {
                    // Wait until all done in order to have non null pointers to copy.
                    ThreadsManager().Wait(asThread::PreloadData);
                }
            }
        }

        if (parallelDataLoad) {
            // Wait until all done
            ThreadsManager().Wait(asThread::PreloadData);
            asLogMessage(_("Data preloaded with threads."));
        }

        // Check if data were preloaded.
        for (int i_step = 0; i_step < params.GetStepsNb(); i_step++) {
            for (int i_ptor = 0; i_ptor < params.GetPredictorsNb(i_step); i_ptor++) {
                if (params.NeedsPreloading(i_step, i_ptor)) {
                    if (!params.NeedsPreprocessing(i_step, i_ptor)) {
                        for (int i_dat = 0; i_dat < params.GetPredictorDataIdVector(i_step, i_ptor).size(); i_dat++) {
                            if (!HasPreloadedData(i_step, i_ptor, i_dat)) {
                                asLogError(wxString::Format(
                                        _("No data was preloaded for step %d and level %d and variable %d"), i_step,
                                        i_ptor, i_dat));
                                return false;
                            }
                        }
                    }
                    if (!HasPreloadedData(i_step, i_ptor)) {
                        asLogError(
                                wxString::Format(_("No data was preloaded for step %d and level %d"), i_step, i_ptor));
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

bool asMethodCalibrator::HasPreloadedData(int i_step, int i_ptor) const
{
    for (int i_dat = 0; i_dat < m_preloadedArchive[i_step][i_ptor].size(); i_dat++) {
        for (int i_level = 0; i_level < m_preloadedArchive[i_step][i_ptor][i_dat].size(); i_level++) {
            for (int i_hour = 0; i_hour < m_preloadedArchive[i_step][i_ptor][i_dat][i_level].size(); i_hour++) {
                if (m_preloadedArchive[i_step][i_ptor][i_dat][i_level][i_hour] != NULL) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool asMethodCalibrator::HasPreloadedData(int i_step, int i_ptor, int i_dat) const
{
    for (int i_level = 0; i_level < m_preloadedArchive[i_step][i_ptor][i_dat].size(); i_level++) {
        for (int i_hour = 0; i_hour < m_preloadedArchive[i_step][i_ptor][i_dat][i_level].size(); i_hour++) {
            if (m_preloadedArchive[i_step][i_ptor][i_dat][i_level][i_hour] != NULL) {
                return true;
            }
        }
    }
    return false;
}

void asMethodCalibrator::InitializePreloadedDataContainer(asParametersScoring &params)
{
    if (m_preloadedArchive.size() == 0) {
        m_preloadedArchive.resize((unsigned long) params.GetStepsNb());
        m_preloadedArchivePointerCopy.resize((unsigned long) params.GetStepsNb());
        for (int i_step = 0; i_step < params.GetStepsNb(); i_step++) {
            m_preloadedArchive[i_step].resize((unsigned long) params.GetPredictorsNb(i_step));
            m_preloadedArchivePointerCopy[i_step].resize((unsigned long) params.GetPredictorsNb(i_step));

            for (int i_ptor = 0; i_ptor < params.GetPredictorsNb(i_step); i_ptor++) {

                VectorString preloadDataIds = params.GetPreloadDataIds(i_step, i_ptor);
                VectorFloat preloadLevels = params.GetPreloadLevels(i_step, i_ptor);
                VectorDouble preloadTimeHours = params.GetPreloadTimeHours(i_step, i_ptor);

                unsigned long preloadDataIdsSize = wxMax(preloadDataIds.size(), 1);
                unsigned long preloadLevelsSize = wxMax(preloadLevels.size(), 1);
                unsigned long preloadTimeHoursSize = wxMax(preloadTimeHours.size(), 1);

                m_preloadedArchivePointerCopy[i_step][i_ptor].resize(preloadDataIdsSize);
                m_preloadedArchive[i_step][i_ptor].resize(preloadDataIdsSize);

                for (int i_dat = 0; i_dat < preloadDataIdsSize; i_dat++) {
                    m_preloadedArchivePointerCopy[i_step][i_ptor][i_dat] = false;
                    m_preloadedArchive[i_step][i_ptor][i_dat].resize(preloadLevelsSize);

                    // Load data for every level and every hour
                    for (int i_level = 0; i_level < preloadLevelsSize; i_level++) {
                        m_preloadedArchive[i_step][i_ptor][i_dat][i_level].resize(preloadTimeHoursSize);
                        for (int i_hour = 0; i_hour < preloadTimeHoursSize; i_hour++) {
                            m_preloadedArchive[i_step][i_ptor][i_dat][i_level][i_hour] = NULL;
                        }
                    }
                }
            }
        }
    }
}

double asMethodCalibrator::GetTimeStartCalibration(asParametersScoring &params) const
{
    double timeStartCalibration = params.GetCalibrationStart();
    timeStartCalibration += std::abs(params.GetTimeShiftDays());

    return timeStartCalibration;
}

double asMethodCalibrator::GetTimeStartArchive(asParametersScoring &params) const
{
    double timeStartArchive = params.GetArchiveStart();
    timeStartArchive += std::abs(params.GetTimeShiftDays());

    return timeStartArchive;
}

double asMethodCalibrator::GetTimeEndCalibration(asParametersScoring &params) const
{
    double timeEndCalibration = params.GetCalibrationEnd();
    timeEndCalibration = wxMin(timeEndCalibration, timeEndCalibration - params.GetTimeSpanDays());

    return timeEndCalibration;
}

double asMethodCalibrator::GetTimeEndArchive(asParametersScoring &params) const
{
    double timeEndArchive = params.GetArchiveEnd();
    timeEndArchive = wxMin(timeEndArchive, timeEndArchive - params.GetTimeSpanDays());

    return timeEndArchive;
}

bool asMethodCalibrator::PointersShared(asParametersScoring &params, int i_step, int i_ptor, int i_dat)
{
    if (i_step == 0 && i_ptor == 0) {
        return false;
    }

    int prev_step = 0, prev_ptor = 0, prev_dat = 0;
    bool share = false;

    for (prev_step = 0; prev_step <= i_step; prev_step++) {

        int ptor_max = params.GetPredictorsNb(prev_step);
        if (prev_step == i_step) {
            ptor_max = i_ptor;
        }

        for (prev_ptor = 0; prev_ptor < ptor_max; prev_ptor++) {
            share = true;

            if (!params.NeedsPreprocessing(i_step, i_ptor)) {
                if (!params.GetPredictorDatasetId(i_step, i_ptor).IsSameAs(
                        params.GetPredictorDatasetId(prev_step, prev_ptor), false))
                    share = false;
                if (!params.GetPredictorGridType(i_step, i_ptor).IsSameAs(
                        params.GetPredictorGridType(prev_step, prev_ptor), false))
                    share = false;
            } else {
                if (!params.GetPreprocessMethod(i_step, i_ptor).IsSameAs(params.GetPreprocessMethod(prev_step, prev_ptor),
                                                                         false))
                    share = false;
                if (params.GetPreprocessSize(i_step, i_ptor) != params.GetPreprocessSize(prev_step, prev_ptor)) {
                    share = false;
                } else {
                    int preprocessSize = params.GetPreprocessSize(i_step, i_ptor);

                    for (int i_prep = 0; i_prep < preprocessSize; i_prep++) {
                        if (!params.GetPreprocessDatasetId(i_step, i_ptor, i_prep).IsSameAs(
                                params.GetPreprocessDatasetId(prev_step, prev_ptor, i_prep), false))
                            share = false;
                        if (!params.GetPreprocessDataId(i_step, i_ptor, i_prep).IsSameAs(
                                params.GetPreprocessDataId(prev_step, prev_ptor, i_prep), false))
                            share = false;
                    }
                }
            }

            if (params.GetPreloadXmin(i_step, i_ptor) != params.GetPreloadXmin(prev_step, prev_ptor))
                share = false;
            if (params.GetPreloadXptsnb(i_step, i_ptor) != params.GetPreloadXptsnb(prev_step, prev_ptor))
                share = false;
            if (params.GetPredictorXstep(i_step, i_ptor) != params.GetPredictorXstep(prev_step, prev_ptor))
                share = false;
            if (params.GetPreloadYmin(i_step, i_ptor) != params.GetPreloadYmin(prev_step, prev_ptor))
                share = false;
            if (params.GetPreloadYptsnb(i_step, i_ptor) != params.GetPreloadYptsnb(prev_step, prev_ptor))
                share = false;
            if (params.GetPredictorYstep(i_step, i_ptor) != params.GetPredictorYstep(prev_step, prev_ptor))
                share = false;
            if (params.GetPredictorFlatAllowed(i_step, i_ptor) != params.GetPredictorFlatAllowed(prev_step, prev_ptor))
                share = false;

            VectorFloat levels1 = params.GetPreloadLevels(i_step, i_ptor);
            VectorFloat levels2 = params.GetPreloadLevels(prev_step, prev_ptor);
            if (levels1.size() != levels2.size()) {
                share = false;
            } else {
                for (unsigned int i = 0; i < levels1.size(); i++) {
                    if (levels1[i] != levels2[i])
                        share = false;
                }
            }

            VectorDouble hours1 = params.GetPreloadTimeHours(i_step, i_ptor);
            VectorDouble hours2 = params.GetPreloadTimeHours(prev_step, prev_ptor);
            if (hours1.size() != hours2.size()) {
                share = false;
            } else {
                for (unsigned int i = 0; i < hours1.size(); i++) {
                    if (hours1[i] != hours2[i])
                        share = false;
                }
            }

            bool dataIdFound = false;
            VectorString preloadDataIds = params.GetPreloadDataIds(i_step, i_ptor);
            VectorString preloadDataIdsPrev = params.GetPreloadDataIds(prev_step, prev_ptor);
            for (unsigned int i = 0; i < preloadDataIdsPrev.size(); i++) {
                // Vector can be empty in case of preprocessing
                if (preloadDataIds.size() > i_dat && preloadDataIdsPrev.size() > i) {
                    wxASSERT(!preloadDataIds[i_dat].IsEmpty());
                    wxASSERT(!preloadDataIdsPrev[i].IsEmpty());
                    if (preloadDataIds[i_dat].IsSameAs(preloadDataIdsPrev[i])) {
                        prev_dat = i;
                        dataIdFound = true;
                    }
                }
            }
            if (!dataIdFound) {
                share = false;
            }

            if (share)
                break;
        }

        if (share)
            break;
    }

    if (share) {
        asLogMessage(_("Share data pointer"));

        VectorFloat preloadLevels = params.GetPreloadLevels(i_step, i_ptor);
        VectorDouble preloadTimeHours = params.GetPreloadTimeHours(i_step, i_ptor);
        wxASSERT(preloadLevels.size() > 0);
        wxASSERT(preloadTimeHours.size() > 0);

        m_preloadedArchivePointerCopy[i_step][i_ptor][i_dat] = true;

        wxASSERT(m_preloadedArchive[prev_step].size() > (unsigned) prev_ptor);
        wxASSERT(m_preloadedArchive[prev_step][prev_ptor].size() > (unsigned) prev_dat);
        wxASSERT(m_preloadedArchive[prev_step][prev_ptor][prev_dat].size() == preloadLevels.size());

        // Load data for every level and every hour
        for (unsigned int i_level = 0; i_level < preloadLevels.size(); i_level++) {
            wxASSERT(m_preloadedArchive[prev_step][prev_ptor][prev_dat][i_level].size() == preloadTimeHours.size());
            for (unsigned int i_hour = 0; i_hour < preloadTimeHours.size(); i_hour++) {
                // Copy pointer
                m_preloadedArchive[i_step][i_ptor][i_dat][i_level][i_hour] = m_preloadedArchive[prev_step][prev_ptor][prev_dat][i_level][i_hour];
            }
        }

        params.SetPreloadYptsnb(i_step, i_ptor, params.GetPreloadYptsnb(prev_step, prev_ptor));

        return true;
    }

    return false;
}

bool asMethodCalibrator::PreloadDataWithoutPreprocessing(asParametersScoring &params, int i_step, int i_ptor, int i_dat)
{
    asLogMessage(wxString::Format(_("Preloading data for predictor %d of step %d."), i_ptor, i_step));

    double timeStartData = wxMin(GetTimeStartCalibration(params), GetTimeStartArchive(params));
    double timeEndData = wxMax(GetTimeEndCalibration(params), GetTimeEndArchive(params));

    VectorString preloadDataIds = params.GetPreloadDataIds(i_step, i_ptor);
    VectorFloat preloadLevels = params.GetPreloadLevels(i_step, i_ptor);
    VectorDouble preloadTimeHours = params.GetPreloadTimeHours(i_step, i_ptor);
    wxASSERT(preloadDataIds.size() > i_dat);
    wxASSERT(preloadLevels.size() > 0);
    wxASSERT(preloadTimeHours.size() > 0);

    // Load data for every level and every hour
    for (unsigned int i_level = 0; i_level < preloadLevels.size(); i_level++) {
        for (unsigned int i_hour = 0; i_hour < preloadTimeHours.size(); i_hour++) {
            // Loading the dataset information
            asDataPredictorArchive *predictor = asDataPredictorArchive::GetInstance(
                    params.GetPredictorDatasetId(i_step, i_ptor), preloadDataIds[i_dat], m_predictorDataDir);
            if (!predictor) {
                return false;
            }

            // Date array object instantiation for the data loading.
            // The array has the same length than timeArrayArchive, and the predictor dates are aligned with the
            // target dates, but the dates are not the same.
            double ptorStart = timeStartData - double(params.GetTimeShiftDays()) + preloadTimeHours[i_hour] / 24.0;

            // For debugging:
            // wxLogMessage("%f - %f + %f = %f", timeStartData, double(params.GetTimeShiftDays()),
            // preloadTimeHours[i_hour]/24.0, ptorStart);
            // wxLogMessage("ptorStart = %s", asTime::GetStringTime(ptorStart));
            // wxLogMessage("timeStartData = %s", asTime::GetStringTime(timeStartData));
            // wxLogMessage("params.GetTimeShiftDays() = %f", double(params.GetTimeShiftDays()));
            // wxLogMessage("preloadTimeHours[i_hour]/24.0 = %f", preloadTimeHours[i_hour]/24.0);

            double ptorEnd = timeEndData - double(params.GetTimeShiftDays()) + preloadTimeHours[i_hour] / 24.0;

            asTimeArray timeArray(ptorStart, ptorEnd, params.GetTimeArrayAnalogsTimeStepHours(), asTimeArray::Simple);
            timeArray.Init();

            asGeo geo;
            double Ymax = params.GetPreloadYmin(i_step, i_ptor) + params.GetPredictorYstep(i_step, i_ptor) *
                                                                  (double) (params.GetPreloadYptsnb(i_step, i_ptor) -
                                                                            1);
            if (Ymax > geo.GetAxisYmax()) {
                double diff = Ymax - geo.GetAxisYmax();
                int removePts = (int) asTools::Round(diff / params.GetPredictorYstep(i_step, i_ptor));
                params.SetPreloadYptsnb(i_step, i_ptor, params.GetPreloadYptsnb(i_step, i_ptor) - removePts);
                asLogMessage(
                        wxString::Format(_("Adapt Y axis extent according to the maximum allowed (from %.3f to %.3f)."),
                                         Ymax, Ymax - diff));
                asLogMessage(
                        wxString::Format(_("Remove %d points (%.3f-%.3f)/%.3f."), removePts, Ymax, geo.GetAxisYmax(),
                                         params.GetPredictorYstep(i_step, i_ptor)));
            }

            wxASSERT(params.GetPreloadXptsnb(i_step, i_ptor) > 0);
            wxASSERT(params.GetPreloadYptsnb(i_step, i_ptor) > 0);

            // Area object instantiation
            asGeoAreaCompositeGrid *area = asGeoAreaCompositeGrid::GetInstance(
                    params.GetPredictorGridType(i_step, i_ptor), params.GetPreloadXmin(i_step, i_ptor),
                    params.GetPreloadXptsnb(i_step, i_ptor), params.GetPredictorXstep(i_step, i_ptor),
                    params.GetPreloadYmin(i_step, i_ptor), params.GetPreloadYptsnb(i_step, i_ptor),
                    params.GetPredictorYstep(i_step, i_ptor), preloadLevels[i_level], asNONE,
                    params.GetPredictorFlatAllowed(i_step, i_ptor));
            wxASSERT(area);

            // Check the starting dates coherence
            if (predictor->GetOriginalProviderStart() > ptorStart) {
                asLogError(wxString::Format(
                        _("The first year defined in the parameters (%s) is prior to the start date of the data (%s) (in asMethodCalibrator::PreloadData)."),
                        asTime::GetStringTime(ptorStart),
                        asTime::GetStringTime(predictor->GetOriginalProviderStart())));
                wxDELETE(area);
                wxDELETE(predictor);
                return false;
            }

            // Data loading
            asLogMessage(wxString::Format(_("Loading %s data for level %d, %d h."), preloadDataIds[i_dat],
                                          (int) preloadLevels[i_level], (int) preloadTimeHours[i_hour]));
            try {
                if (!predictor->Load(area, timeArray)) {
                    asLogWarning(wxString::Format(_("The data (%s for level %d, at %d h) could not be loaded."),
                                                  preloadDataIds[i_dat], (int) preloadLevels[i_level],
                                                  (int) preloadTimeHours[i_hour]));
                    wxDELETE(area);
                    wxDELETE(predictor);
                    continue; // The requested data can be missing (e.g. level not available).
                }
            } catch (std::bad_alloc &ba) {
                wxString msg(ba.what(), wxConvUTF8);
                asLogError(wxString::Format(_("Bad allocation in the data preloading: %s"), msg));
                wxDELETE(area);
                wxDELETE(predictor);
                return false;
            } catch (std::exception &e) {
                wxString msg(e.what(), wxConvUTF8);
                asLogError(wxString::Format(_("Exception in the data preloading: %s"), msg));
                wxDELETE(area);
                wxDELETE(predictor);
                return false;
            }
            asLogMessage(_("Data loaded."));
            wxDELETE(area);

            m_preloadedArchive[i_step][i_ptor][i_dat][i_level][i_hour] = predictor;
        }
    }

    return true;
}

bool asMethodCalibrator::PreloadDataWithPreprocessing(asParametersScoring &params, int i_step, int i_ptor)
{
    asLogMessage(wxString::Format(_("Preloading data for predictor preprocessed %d of step %d."), i_ptor, i_step));

    double timeStartData = wxMin(GetTimeStartCalibration(params), GetTimeStartArchive(params));
    double timeEndData = wxMax(GetTimeEndCalibration(params), GetTimeEndArchive(params));

    // Check the preprocessing method
    wxString method = params.GetPreprocessMethod(i_step, i_ptor);

    // Get the number of sub predictors
    int preprocessSize = params.GetPreprocessSize(i_step, i_ptor);

    // Levels and time arrays
    VectorFloat preloadLevels = params.GetPreloadLevels(i_step, i_ptor);
    VectorDouble preloadTimeHours = params.GetPreloadTimeHours(i_step, i_ptor);

    // Check on which variable to loop
    unsigned long preloadLevelsSize = preloadLevels.size();
    unsigned long preloadTimeHoursSize = preloadTimeHours.size();
    bool loopOnLevels = true;
    bool loopOnTimeHours = true;


    if (preloadLevelsSize == 0) {
        loopOnLevels = false;
        preloadLevelsSize = 1;
    }

    if (preloadTimeHoursSize == 0) {
        loopOnTimeHours = false;
        preloadTimeHoursSize = 1;
    }

    asLogMessage(wxString::Format(_("Preprocessing data (%d predictor(s)) while loading."), preprocessSize));

    // Load data for every level and every hour
    for (unsigned int i_level = 0; i_level < preloadLevelsSize; i_level++) {
        for (unsigned int i_hour = 0; i_hour < preloadTimeHoursSize; i_hour++) {
            std::vector<asDataPredictorArchive *> predictorsPreprocess;

            for (int i_prepro = 0; i_prepro < preprocessSize; i_prepro++) {
                asLogMessage(wxString::Format(_("Preloading data for predictor %d (preprocess %d) of step %d."), i_ptor,
                                              i_prepro, i_step));

                // Get level
                float level;
                if (loopOnLevels) {
                    level = preloadLevels[i_level];
                } else {
                    level = params.GetPreprocessLevel(i_step, i_ptor, i_prepro);
                }

                // Get time
                double timeHours;
                if (loopOnTimeHours) {
                    timeHours = preloadTimeHours[i_hour];
                } else {
                    timeHours = params.GetPreprocessTimeHours(i_step, i_ptor, i_prepro);
                }

                // Correct according to the method
                if (method.IsSameAs("Gradients")) {
                    // Nothing to change
                } else if (method.IsSameAs("HumidityIndex")) {
                    if (i_prepro == 1)
                        level = 0; // pr_wtr
                } else if (method.IsSameAs("HumidityFlux")) {
                    if (i_prepro == 2)
                        level = 0; // pr_wtr
                } else if (method.IsSameAs("FormerHumidityIndex")) {
                    if (i_prepro == 2)
                        level = 0; // pr_wtr
                    if (i_prepro == 3)
                        level = 0; // pr_wtr
                    if (i_prepro == 0)
                        timeHours = preloadTimeHours[0];
                    if (i_prepro == 1)
                        timeHours = preloadTimeHours[1];
                    if (i_prepro == 2)
                        timeHours = preloadTimeHours[0];
                    if (i_prepro == 3)
                        timeHours = preloadTimeHours[1];
                }

                // Date array object instantiation for the data loading.
                // The array has the same length than timeArrayArchive, and the predictor dates are aligned
                // with the target dates, but the dates are not the same.
                double ptorStart = timeStartData - double(params.GetTimeShiftDays()) + timeHours / 24.0;
                double ptorEnd = timeEndData - double(params.GetTimeShiftDays()) + timeHours / 24.0;
                asTimeArray timeArray(ptorStart, ptorEnd, params.GetTimeArrayAnalogsTimeStepHours(),
                                      asTimeArray::Simple);
                timeArray.Init();

                // Loading the datasets information
                asDataPredictorArchive *predictorPreprocess = asDataPredictorArchive::GetInstance(
                        params.GetPreprocessDatasetId(i_step, i_ptor, i_prepro),
                        params.GetPreprocessDataId(i_step, i_ptor, i_prepro), m_predictorDataDir);
                if (!predictorPreprocess) {
                    Cleanup(predictorsPreprocess);
                    return false;
                }

                asGeo geo;
                double Ymax = params.GetPreloadYmin(i_step, i_ptor) + params.GetPredictorYstep(i_step, i_ptor) *
                                                                      double(params.GetPreloadYptsnb(i_step, i_ptor) - 1);
                if (Ymax > geo.GetAxisYmax()) {
                    double diff = Ymax - geo.GetAxisYmax();
                    int removePts = (int) asTools::Round(diff / params.GetPredictorYstep(i_step, i_ptor));
                    params.SetPreloadYptsnb(i_step, i_ptor, params.GetPreloadYptsnb(i_step, i_ptor) - removePts);
                    asLogMessage(wxString::Format(
                            _("Adapt Y axis extent according to the maximum allowed (from %.2f to %.2f)."), Ymax,
                            Ymax - diff));
                }

                // Area object instantiation
                asGeoAreaCompositeGrid *area = asGeoAreaCompositeGrid::GetInstance(
                        params.GetPredictorGridType(i_step, i_ptor), params.GetPreloadXmin(i_step, i_ptor),
                        params.GetPreloadXptsnb(i_step, i_ptor), params.GetPredictorXstep(i_step, i_ptor),
                        params.GetPreloadYmin(i_step, i_ptor), params.GetPreloadYptsnb(i_step, i_ptor),
                        params.GetPredictorYstep(i_step, i_ptor), level, asNONE,
                        params.GetPredictorFlatAllowed(i_step, i_ptor));
                wxASSERT(area);

                // Check the starting dates coherence
                if (predictorPreprocess->GetOriginalProviderStart() > ptorStart) {
                    asLogError(wxString::Format(
                            _("The first year defined in the parameters (%s) is prior to the start date of the data (%s) (in asMethodCalibrator::PreloadData, preprocessing)."),
                            asTime::GetStringTime(ptorStart),
                            asTime::GetStringTime(predictorPreprocess->GetOriginalProviderStart())));
                    wxDELETE(area);
                    wxDELETE(predictorPreprocess);
                    Cleanup(predictorsPreprocess);
                    return false;
                }

                // Data loading
                asLogMessage(wxString::Format(_("Loading %s data for level %d, %d h."),
                                              params.GetPreprocessDataId(i_step, i_ptor, i_prepro), (int) level,
                                              (int) timeHours));
                if (!predictorPreprocess->Load(area, timeArray)) {
                    asLogError(_("The data could not be loaded."));
                    wxDELETE(area);
                    wxDELETE(predictorPreprocess);
                    return false;
                }
                wxDELETE(area);
                predictorsPreprocess.push_back(predictorPreprocess);
            }

            asLogMessage(_("Preprocessing data."));
            asDataPredictorArchive *predictor = new asDataPredictorArchive(*predictorsPreprocess[0]);
            try {
                if (!asPreprocessor::Preprocess(predictorsPreprocess, params.GetPreprocessMethod(i_step, i_ptor),
                                                predictor)) {
                    asLogError(_("Data preprocessing failed."));
                    wxDELETE(predictor);
                    Cleanup(predictorsPreprocess);
                    return false;
                }
                m_preloadedArchive[i_step][i_ptor][0][i_level][i_hour] = predictor;
            } catch (std::bad_alloc &ba) {
                wxString msg(ba.what(), wxConvUTF8);
                asLogError(wxString::Format(_("Bad allocation caught in the data preprocessing: %s"), msg));
                wxDELETE(predictor);
                Cleanup(predictorsPreprocess);
                return false;
            } catch (std::exception &e) {
                wxString msg(e.what(), wxConvUTF8);
                asLogError(wxString::Format(_("Exception in the data preprocessing: %s"), msg));
                wxDELETE(predictor);
                Cleanup(predictorsPreprocess);
                return false;
            }
            Cleanup(predictorsPreprocess);
            asLogMessage(_("Preprocessing over."));
        }
    }

    // Fix the criteria if S1
    if (method.IsSameAs("Gradients") && params.GetPredictorCriteria(i_step, i_ptor).IsSameAs("S1")) {
        params.SetPredictorCriteria(i_step, i_ptor, "S1grads");
    } else if (method.IsSameAs("Gradients") && params.GetPredictorCriteria(i_step, i_ptor).IsSameAs("NS1")) {
        params.SetPredictorCriteria(i_step, i_ptor, "NS1grads");
    }

    return true;
}

bool asMethodCalibrator::LoadData(std::vector<asDataPredictor *> &predictors, asParametersScoring &params, int i_step,
                                  double timeStartData, double timeEndData)
{
    try {
        // Loop through every predictor
        for (int i_ptor = 0; i_ptor < params.GetPredictorsNb(i_step); i_ptor++) {
            if (!PreloadData(params)) {
                asLogError(_("Could not preload the data."));
                return false;
            }

            if (params.NeedsPreloading(i_step, i_ptor)) {
                if (!ExtractPreloadedData(predictors, params, i_step, i_ptor)) {
                    return false;
                }
            } else {
                asLogMessage(_("Loading data."));

                if (!params.NeedsPreprocessing(i_step, i_ptor)) {
                    if (!ExtractDataWithoutPreprocessing(predictors, params, i_step, i_ptor, timeStartData, timeEndData)) {
                        return false;
                    }
                } else {
                    if (!ExtractDataWithPreprocessing(predictors, params, i_step, i_ptor, timeStartData, timeEndData)) {
                        return false;
                    }
                }

                asLogMessage(_("Data loaded"));
            }
        }
    } catch (std::bad_alloc &ba) {
        wxString msg(ba.what(), wxConvUTF8);
        asLogError(wxString::Format(_("Bad allocation in the data loading: %s"), msg));
        return false;
    } catch (asException &e) {
        wxString fullMessage = e.GetFullMessage();
        if (!fullMessage.IsEmpty()) {
            asLogError(fullMessage);
        }
        asLogError(_("Failed to load data."));
        return false;
    } catch (std::exception &e) {
        wxString msg(e.what(), wxConvUTF8);
        asLogError(wxString::Format(_("Exception in the data loading: %s"), msg));
        return false;
    }

    return true;
}

bool asMethodCalibrator::ExtractPreloadedData(std::vector<asDataPredictor *> &predictors, asParametersScoring &params,
                                              int i_step, int i_ptor)
{
    asLogMessage(_("Using preloaded data."));

    bool doPreprocessGradients = false;

    // Get preload arrays
    VectorFloat preloadLevels = params.GetPreloadLevels(i_step, i_ptor);
    VectorDouble preloadTimeHours = params.GetPreloadTimeHours(i_step, i_ptor);
    float level;
    double time;
    int i_level = 0, i_hour = 0, i_dat = 0;

    // Get data ID
    VectorString preloadDataIds = params.GetPreloadDataIds(i_step, i_ptor);
    for (int i = 0; i < preloadDataIds.size(); i++) {
        if (preloadDataIds[i].IsSameAs(params.GetPredictorDataId(i_step, i_ptor))) {
            i_dat = i;
        }
    }

    if (!params.NeedsPreprocessing(i_step, i_ptor)) {
        wxASSERT(preloadLevels.size() > 0);
        wxASSERT(preloadTimeHours.size() > 0);

        level = params.GetPredictorLevel(i_step, i_ptor);
        time = params.GetPredictorTimeHours(i_step, i_ptor);

        // Get level and hour indices
        i_level = asTools::SortedArraySearch(&preloadLevels[0], &preloadLevels[preloadLevels.size() - 1], level);
        i_hour = asTools::SortedArraySearch(&preloadTimeHours[0], &preloadTimeHours[preloadTimeHours.size() - 1], time);

        // Force gradients preprocessing anyway.
        if (params.GetPredictorCriteria(i_step, i_ptor).IsSameAs("S1")) {
            doPreprocessGradients = true;
            params.SetPredictorCriteria(i_step, i_ptor, "S1grads");
        } else if (params.GetPredictorCriteria(i_step, i_ptor).IsSameAs("S1grads")) {
            doPreprocessGradients = true;
        } else if (params.GetPredictorCriteria(i_step, i_ptor).IsSameAs("NS1")) {
            doPreprocessGradients = true;
            params.SetPredictorCriteria(i_step, i_ptor, "NS1grads");
        } else if (params.GetPredictorCriteria(i_step, i_ptor).IsSameAs("NS1grads")) {
            doPreprocessGradients = true;
        }
    } else {
        // Correct according to the method
        if (params.GetPreprocessMethod(i_step, i_ptor).IsSameAs("Gradients")) {
            level = params.GetPreprocessLevel(i_step, i_ptor, 0);
            time = params.GetPreprocessTimeHours(i_step, i_ptor, 0);
        } else if (params.GetPreprocessMethod(i_step, i_ptor).IsSameAs("HumidityIndex")) {
            level = params.GetPreprocessLevel(i_step, i_ptor, 0);
            time = params.GetPreprocessTimeHours(i_step, i_ptor, 0);
        } else if (params.GetPreprocessMethod(i_step, i_ptor).IsSameAs("HumidityFlux")) {
            level = params.GetPreprocessLevel(i_step, i_ptor, 0);
            time = params.GetPreprocessTimeHours(i_step, i_ptor, 0);
        } else if (params.GetPreprocessMethod(i_step, i_ptor).IsSameAs("FormerHumidityIndex")) {
            level = params.GetPreprocessLevel(i_step, i_ptor, 0);
            time = params.GetPreprocessTimeHours(i_step, i_ptor, 0);
        } else {
            level = params.GetPreprocessLevel(i_step, i_ptor, 0);
            time = params.GetPreprocessTimeHours(i_step, i_ptor, 0);
        }

        // Get level and hour indices
        if (preloadLevels.size() > 0) {
            i_level = asTools::SortedArraySearch(&preloadLevels[0], &preloadLevels[preloadLevels.size() - 1], level);
        }
        if (preloadTimeHours.size() > 0) {
            i_hour = asTools::SortedArraySearch(&preloadTimeHours[0], &preloadTimeHours[preloadTimeHours.size() - 1],
                                                time);
        }
    }

    // Check indices
    if (i_level == asNOT_FOUND || i_level == asOUT_OF_RANGE) {
        asLogError(wxString::Format(_("The level (%f) could not be found in the preloaded data."), level));
        return false;
    }
    if (i_hour == asNOT_FOUND || i_hour == asOUT_OF_RANGE) {
        asLogError(wxString::Format(_("The hour (%d) could not be found in the preloaded data."), (int) time));
        return false;
    }

    // Get data on the desired domain
    wxASSERT((unsigned) i_step < m_preloadedArchive.size());
    wxASSERT((unsigned) i_ptor < m_preloadedArchive[i_step].size());
    wxASSERT((unsigned) i_dat < m_preloadedArchive[i_step][i_ptor].size());
    wxASSERT((unsigned) i_level < m_preloadedArchive[i_step][i_ptor][i_dat].size());
    wxASSERT((unsigned) i_hour < m_preloadedArchive[i_step][i_ptor][i_dat][i_level].size());
    if (m_preloadedArchive[i_step][i_ptor][i_dat][i_level][i_hour] == NULL) {
        if (!GetRandomValidData(params, i_step, i_ptor, i_dat)) {
            asLogError(_("The pointer to preloaded data is null."));
            return false;
        }

        level = params.GetPredictorLevel(i_step, i_ptor);
        time = params.GetPredictorTimeHours(i_step, i_ptor);
        i_level = asTools::SortedArraySearch(&preloadLevels[0], &preloadLevels[preloadLevels.size() - 1], level);
        i_hour = asTools::SortedArraySearch(&preloadTimeHours[0], &preloadTimeHours[preloadTimeHours.size() - 1], time);
    }
    // Copy the data
    wxASSERT(m_preloadedArchive[i_step][i_ptor][i_dat][i_level][i_hour]);
    asDataPredictorArchive *desiredPredictor = new asDataPredictorArchive(
            *m_preloadedArchive[i_step][i_ptor][i_dat][i_level][i_hour]);

    // Area object instantiation
    asGeoAreaCompositeGrid *desiredArea = asGeoAreaCompositeGrid::GetInstance(
            params.GetPredictorGridType(i_step, i_ptor), params.GetPredictorXmin(i_step, i_ptor),
            params.GetPredictorXptsnb(i_step, i_ptor), params.GetPredictorXstep(i_step, i_ptor),
            params.GetPredictorYmin(i_step, i_ptor), params.GetPredictorYptsnb(i_step, i_ptor),
            params.GetPredictorYstep(i_step, i_ptor), params.GetPredictorLevel(i_step, i_ptor), asNONE,
            params.GetPredictorFlatAllowed(i_step, i_ptor));

    wxASSERT(desiredArea);

    if (!desiredPredictor->ClipToArea(desiredArea)) {
        asLogError(wxString::Format(
                _("The data could not be extracted (i_step = %d, i_ptor = %d, i_dat = %d, i_level = %d, i_hour = %d)."),
                i_step, i_ptor, i_dat, i_level, i_hour));
        wxDELETE(desiredArea);
        wxDELETE(desiredPredictor);
        return false;
    }
    wxDELETE(desiredArea);

    if (doPreprocessGradients) {
        std::vector<asDataPredictorArchive *> predictorsPreprocess;
        predictorsPreprocess.push_back(desiredPredictor);

        asDataPredictorArchive *newPredictor = new asDataPredictorArchive(*predictorsPreprocess[0]);
        if (!asPreprocessor::Preprocess(predictorsPreprocess, "Gradients", newPredictor)) {
            asLogError(_("Data preprocessing failed."));
            Cleanup(predictorsPreprocess);
            return false;
        }

        Cleanup(predictorsPreprocess);

        wxASSERT(newPredictor->GetTimeSize() > 0);
        predictors.push_back(newPredictor);
    } else {
        wxASSERT(desiredPredictor->GetTimeSize() > 0);
        predictors.push_back(desiredPredictor);
    }

    return true;
}

bool asMethodCalibrator::ExtractDataWithoutPreprocessing(std::vector<asDataPredictor *> &predictors,
                                                         asParametersScoring &params, int i_step, int i_ptor,
                                                         double timeStartData, double timeEndData)
{
    // Date array object instantiation for the data loading. The array has the same length than timeArrayArchive, and the predictor dates are aligned with the target dates, but the dates are not the same.
    double ptorStart = timeStartData - params.GetTimeShiftDays() + params.GetPredictorTimeHours(i_step, i_ptor) / 24.0;
    double ptorEnd = timeEndData - params.GetTimeShiftDays() + params.GetPredictorTimeHours(i_step, i_ptor) / 24.0;
    asTimeArray timeArray(ptorStart, ptorEnd, params.GetTimeArrayAnalogsTimeStepHours(), asTimeArray::Simple);
    timeArray.Init();

    // Loading the datasets information
    asDataPredictorArchive *predictor = asDataPredictorArchive::GetInstance(
            params.GetPredictorDatasetId(i_step, i_ptor), params.GetPredictorDataId(i_step, i_ptor),
            m_predictorDataDir);
    if (!predictor) {
        return false;
    }

    // Area object instantiation
    asGeoAreaCompositeGrid *area = asGeoAreaCompositeGrid::GetInstance(params.GetPredictorGridType(i_step, i_ptor),
                                                                       params.GetPredictorXmin(i_step, i_ptor),
                                                                       params.GetPredictorXptsnb(i_step, i_ptor),
                                                                       params.GetPredictorXstep(i_step, i_ptor),
                                                                       params.GetPredictorYmin(i_step, i_ptor),
                                                                       params.GetPredictorYptsnb(i_step, i_ptor),
                                                                       params.GetPredictorYstep(i_step, i_ptor),
                                                                       params.GetPredictorLevel(i_step, i_ptor), asNONE,
                                                                       params.GetPredictorFlatAllowed(i_step, i_ptor));
    wxASSERT(area);

    // Check the starting dates coherence
    if (predictor->GetOriginalProviderStart() > ptorStart) {
        asLogError(wxString::Format(
                _("The first year defined in the parameters (%s) is prior to the start date of the data (%s) (in asMethodCalibrator::GetAnalogsDates, no preprocessing)."),
                asTime::GetStringTime(ptorStart), asTime::GetStringTime(predictor->GetOriginalProviderStart())));
        wxDELETE(area);
        wxDELETE(predictor);
        return false;
    }

    // Data loading
    if (!predictor->Load(area, timeArray)) {
        asLogError(_("The data could not be loaded."));
        wxDELETE(area);
        wxDELETE(predictor);
        return false;
    }
    wxDELETE(area);
    predictors.push_back(predictor);

    return true;
}

bool asMethodCalibrator::ExtractDataWithPreprocessing(std::vector<asDataPredictor *> &predictors,
                                                      asParametersScoring &params, int i_step, int i_ptor,
                                                      double timeStartData, double timeEndData)
{
    std::vector<asDataPredictorArchive *> predictorsPreprocess;

    int preprocessSize = params.GetPreprocessSize(i_step, i_ptor);

    asLogMessage(wxString::Format(_("Preprocessing data (%d predictor(s)) while loading."), preprocessSize));

    for (int i_prep = 0; i_prep < preprocessSize; i_prep++) {
        // Date array object instantiation for the data loading. The array has the same length than timeArrayArchive, and the predictor dates are aligned with the target dates, but the dates are not the same.
        double ptorStart = timeStartData - double(params.GetTimeShiftDays()) +
                           params.GetPreprocessTimeHours(i_step, i_ptor, i_prep) / 24.0;
        double ptorEnd = timeEndData - double(params.GetTimeShiftDays()) +
                         params.GetPreprocessTimeHours(i_step, i_ptor, i_prep) / 24.0;
        asTimeArray timeArray(ptorStart, ptorEnd, params.GetTimeArrayAnalogsTimeStepHours(), asTimeArray::Simple);
        timeArray.Init();

        // Loading the dataset information
        asDataPredictorArchive *predictorPreprocess = asDataPredictorArchive::GetInstance(
                params.GetPreprocessDatasetId(i_step, i_ptor, i_prep),
                params.GetPreprocessDataId(i_step, i_ptor, i_prep), m_predictorDataDir);
        if (!predictorPreprocess) {
            Cleanup(predictorsPreprocess);
            return false;
        }

        // Area object instantiation
        asGeoAreaCompositeGrid *area = asGeoAreaCompositeGrid::GetInstance(params.GetPredictorGridType(i_step, i_ptor),
                                                                           params.GetPredictorXmin(i_step, i_ptor),
                                                                           params.GetPredictorXptsnb(i_step, i_ptor),
                                                                           params.GetPredictorXstep(i_step, i_ptor),
                                                                           params.GetPredictorYmin(i_step, i_ptor),
                                                                           params.GetPredictorYptsnb(i_step, i_ptor),
                                                                           params.GetPredictorYstep(i_step, i_ptor),
                                                                           params.GetPreprocessLevel(i_step, i_ptor,
                                                                                                     i_prep), asNONE,
                                                                           params.GetPredictorFlatAllowed(i_step,
                                                                                                          i_ptor));
        wxASSERT(area);

        // Check the starting dates coherence
        if (predictorPreprocess->GetOriginalProviderStart() > ptorStart) {
            asLogError(wxString::Format(
                    _("The first year defined in the parameters (%s) is prior to the start date of the data (%s) (in asMethodCalibrator::GetAnalogsDates, preprocessing)."),
                    asTime::GetStringTime(ptorStart),
                    asTime::GetStringTime(predictorPreprocess->GetOriginalProviderStart())));
            wxDELETE(area);
            wxDELETE(predictorPreprocess);
            Cleanup(predictorsPreprocess);
            return false;
        }

        // Data loading
        if (!predictorPreprocess->Load(area, timeArray)) {
            asLogError(_("The data could not be loaded."));
            wxDELETE(area);
            wxDELETE(predictorPreprocess);
            Cleanup(predictorsPreprocess);
            return false;
        }
        wxDELETE(area);
        predictorsPreprocess.push_back(predictorPreprocess);
    }

    // Fix the criteria if S1
    if (params.GetPreprocessMethod(i_step, i_ptor).IsSameAs("Gradients") &&
        params.GetPredictorCriteria(i_step, i_ptor).IsSameAs("S1")) {
        params.SetPredictorCriteria(i_step, i_ptor, "S1grads");
    } else if (params.GetPreprocessMethod(i_step, i_ptor).IsSameAs("Gradients") &&
               params.GetPredictorCriteria(i_step, i_ptor).IsSameAs("NS1")) {
        params.SetPredictorCriteria(i_step, i_ptor, "NS1grads");
    }

    asDataPredictorArchive *predictor = new asDataPredictorArchive(*predictorsPreprocess[0]);
    if (!asPreprocessor::Preprocess(predictorsPreprocess, params.GetPreprocessMethod(i_step, i_ptor), predictor)) {
        asLogError(_("Data preprocessing failed."));
        Cleanup(predictorsPreprocess);
        return false;
    }

    Cleanup(predictorsPreprocess);
    predictors.push_back(predictor);

    return true;
}

VArray1DFloat asMethodCalibrator::GetClimatologyData(asParametersScoring &params)
{
    VectorInt stationIds = params.GetPredictandStationIds();

    // Get start and end dates
    Array1DDouble predictandTime = m_predictandDB->GetTime();
    float predictandTimeDays = float(params.GetPredictandTimeHours() / 24.0);
    double timeStart, timeEnd;
    timeStart = wxMax(predictandTime[0], params.GetArchiveStart());
    timeStart = floor(timeStart) + predictandTimeDays;
    timeEnd = wxMin(predictandTime[predictandTime.size() - 1], params.GetArchiveEnd());
    timeEnd = floor(timeEnd) + predictandTimeDays;

    // Check if data are effectively available for this period
    int indexPredictandTimeStart = asTools::SortedArraySearchCeil(&predictandTime[0],
                                                                  &predictandTime[predictandTime.size() - 1],
                                                                  timeStart);
    int indexPredictandTimeEnd = asTools::SortedArraySearchFloor(&predictandTime[0],
                                                                 &predictandTime[predictandTime.size() - 1], timeEnd);

    for (int i_st = 0; i_st < (int) stationIds.size(); i_st++) {
        Array1DFloat predictandDataNorm = m_predictandDB->GetDataNormalizedStation(stationIds[i_st]);

        while (asTools::IsNaN(predictandDataNorm(indexPredictandTimeStart))) {
            indexPredictandTimeStart++;
        }
        while (asTools::IsNaN(predictandDataNorm(indexPredictandTimeEnd))) {
            indexPredictandTimeEnd--;
        }
    }

    timeStart = predictandTime[indexPredictandTimeStart];
    timeStart = floor(timeStart) + predictandTimeDays;
    timeEnd = predictandTime[indexPredictandTimeEnd];
    timeEnd = floor(timeEnd) + predictandTimeDays;
    indexPredictandTimeStart = asTools::SortedArraySearchCeil(&predictandTime[0],
                                                              &predictandTime[predictandTime.size() - 1], timeStart);
    indexPredictandTimeEnd = asTools::SortedArraySearchFloor(&predictandTime[0],
                                                             &predictandTime[predictandTime.size() - 1], timeEnd);

    // Get index step
    double predictandTimeStep = predictandTime[1] - predictandTime[0];
    double targetTimeStep = params.GetTimeArrayTargetTimeStepHours() / 24.0;
    int indexStep = int(targetTimeStep / predictandTimeStep);

    // Get vector length
    int dataLength = (indexPredictandTimeEnd - indexPredictandTimeStart) / indexStep + 1;

    // Process the climatology score
    VArray1DFloat climatologyData(stationIds.size(), Array1DFloat(dataLength));
    for (int i_st = 0; i_st < (int) stationIds.size(); i_st++) {
        Array1DFloat predictandDataNorm = m_predictandDB->GetDataNormalizedStation(stationIds[i_st]);

        // Set data
        int counter = 0;
        for (int i = indexPredictandTimeStart; i <= indexPredictandTimeEnd; i += indexStep) {
            climatologyData[i_st][counter] = predictandDataNorm[i];
            counter++;
        }
        wxASSERT(dataLength == counter);
    }

    return climatologyData;
}

void asMethodCalibrator::Cleanup(std::vector<asDataPredictorArchive *> predictorsPreprocess)
{
    if (predictorsPreprocess.size() > 0) {
        for (unsigned int i = 0; i < predictorsPreprocess.size(); i++) {
            wxDELETE(predictorsPreprocess[i]);
        }
        predictorsPreprocess.resize(0);
    }
}

void asMethodCalibrator::Cleanup(std::vector<asDataPredictor *> predictors)
{
    if (predictors.size() > 0) {
        for (unsigned int i = 0; i < predictors.size(); i++) {
            wxDELETE(predictors[i]);
        }
        predictors.resize(0);
    }
}

void asMethodCalibrator::Cleanup(std::vector<asPredictorCriteria *> criteria)
{
    if (criteria.size() > 0) {
        for (unsigned int i = 0; i < criteria.size(); i++) {
            wxDELETE(criteria[i]);
        }
        criteria.resize(0);
    }
}

void asMethodCalibrator::DeletePreloadedData()
{
    if (!m_preloaded)
        return;

    for (unsigned int i = 0; i < m_preloadedArchive.size(); i++) {
        for (unsigned int j = 0; j < m_preloadedArchive[i].size(); j++) {
            for (unsigned int k = 0; k < m_preloadedArchive[i][j].size(); k++) {
                if (!m_preloadedArchivePointerCopy[i][j][k]) {
                    for (unsigned int l = 0; l < m_preloadedArchive[i][j][k].size(); l++) {
                        for (unsigned int m = 0; m < m_preloadedArchive[i][j][k][l].size(); m++) {
                            wxDELETE(m_preloadedArchive[i][j][k][l][m]);
                        }
                    }
                }
            }
        }
    }

    m_preloaded = false;
}

bool asMethodCalibrator::GetAnalogsDates(asResultsAnalogsDates &results, asParametersScoring &params, int i_step,
                                         bool &containsNaNs)
{
    // Get the linear algebra method
    ThreadsManager().CritSectionConfig().Enter();
    int linAlgebraMethod = (int) (wxFileConfig::Get()->Read("/Processing/LinAlgebra", (long) asLIN_ALGEBRA_NOVAR));
    ThreadsManager().CritSectionConfig().Leave();

    // Initialize the result object
    results.SetCurrentStep(i_step);
    results.Init(params);

    // If result file already exists, load it
    if (results.Load()) {
        return true;
    }

    // Archive date array
    asTimeArray timeArrayArchive(GetTimeStartArchive(params), GetTimeEndArchive(params),
                                 params.GetTimeArrayAnalogsTimeStepHours(), asTimeArray::Simple);
    if (params.HasValidationPeriod()) // remove validation years
    {
        timeArrayArchive.SetForbiddenYears(params.GetValidationYearsVector());
    }
    timeArrayArchive.Init();

    // Target date array
    asTimeArray timeArrayTarget(GetTimeStartCalibration(params), GetTimeEndCalibration(params),
                                params.GetTimeArrayTargetTimeStepHours(), params.GetTimeArrayTargetMode());

    if (!m_validationMode) {
        if (params.HasValidationPeriod()) // remove validation years
        {
            timeArrayTarget.SetForbiddenYears(params.GetValidationYearsVector());
        }
    }

    if (params.GetTimeArrayTargetMode().CmpNoCase("predictand_thresholds") == 0 ||
        params.GetTimeArrayTargetMode().CmpNoCase("PredictandThresholds") == 0) {
        VectorInt stations = params.GetPredictandStationIds();
        if (stations.size() > 1) {
            asLogError(_("You cannot use predictand thresholds with the multivariate approach."));
            return false;
        }

        if (!timeArrayTarget.Init(*m_predictandDB, params.GetTimeArrayTargetPredictandSerieName(), stations[0],
                                  params.GetTimeArrayTargetPredictandMinThreshold(),
                                  params.GetTimeArrayTargetPredictandMaxThreshold())) {
            asLogError(_("The time array mode for the target dates is not correctly defined."));
            return false;
        }
    } else {
        if (!timeArrayTarget.Init()) {
            asLogError(_("The time array mode for the target dates is not correctly defined."));
            return false;
        }
    }

    // If in validation mode, only keep validation years
    if (m_validationMode) {
        timeArrayTarget.KeepOnlyYears(params.GetValidationYearsVector());
    }

    // Data date array
    double timeStartData = wxMin(GetTimeStartCalibration(params), GetTimeStartArchive(params)); // Always Jan 1st
    double timeEndData = wxMax(GetTimeEndCalibration(params), GetTimeEndArchive(params));
    asTimeArray timeArrayData(timeStartData, timeEndData, params.GetTimeArrayAnalogsTimeStepHours(),
                              asTimeArray::Simple);
    timeArrayData.Init();

    // Check on the archive length
    if (timeArrayArchive.GetSize() < 100) {
        asLogError(
                wxString::Format(_("The time array is not consistent in asMethodCalibrator::GetAnalogsDates: size=%d."),
                                 timeArrayArchive.GetSize()));
        return false;
    }
    asLogMessage(_("Date arrays created."));
    /*
        // Calculate needed memory
        wxLongLong neededMem = 0;
        for(int i_ptor=0; i_ptor<params.GetPredictorsNb(i_step); i_ptor++)
        {
            neededMem += (params.GetPredictorXptsnb(i_step, i_ptor))
                         * (params.GetPredictorYptsnb(i_step, i_ptor));
        }
        neededMem *= timeArrayArchive.GetSize(); // time dimension
        neededMem *= 4; // to bytes (for floats)
        double neededMemMb = neededMem.ToDouble();
        neededMemMb /= 1048576.0; // to Mb

        // Get available memory
        wxMemorySize freeMemSize = wxGetFreeMemory();
        wxLongLong freeMem = freeMemSize;
        double freeMemMb = freeMem.ToDouble();
        freeMemMb /= 1048576.0; // To Mb

        if(freeMemSize==-1)
        {
            asLogMessage(wxString::Format(_("Needed memory for data: %.2f Mb (cannot evaluate available memory)"),
                                          neededMemMb));
        }
        else
        {
            asLogMessage(wxString::Format(_("Needed memory for data: %.2f Mb (%.2f Mb available)"),
                                          neededMemMb, freeMemMb));
            if(neededMemMb>freeMemMb)
            {
                asLogError(_("Data cannot fit into available memory."));
                return false;
            }
        }
    */
    // Load the predictor data
    std::vector<asDataPredictor *> predictors;
    if (!LoadData(predictors, params, i_step, timeStartData, timeEndData)) {
        asLogError(_("Failed loading predictor data."));
        Cleanup(predictors);
        return false;
    }

    // Create the criterion
    std::vector<asPredictorCriteria *> criteria;
    for (int i_ptor = 0; i_ptor < params.GetPredictorsNb(i_step); i_ptor++) {
        // Instantiate a score object
        asPredictorCriteria *criterion = asPredictorCriteria::GetInstance(params.GetPredictorCriteria(i_step, i_ptor),
                                                                          linAlgebraMethod);
        if(criterion->NeedsDataRange()) {
            wxASSERT(predictors.size()>i_ptor);
            wxASSERT(predictors[i_ptor]);
            criterion->SetDataRange(predictors[i_ptor]);
        }
        criteria.push_back(criterion);
    }

    // Check time sizes
#ifdef _DEBUG
    int prevTimeSize = 0;

    for (unsigned int i = 0; i < predictors.size(); i++) {
        if (i > 0) {
            wxASSERT(predictors[i]->GetTimeSize() == prevTimeSize);
        }
        prevTimeSize = predictors[i]->GetTimeSize();
    }
#endif // _DEBUG

    // Inline the data when possible
    for (int i_ptor = 0; i_ptor < (int) predictors.size(); i_ptor++) {
        if (criteria[i_ptor]->CanUseInline()) {
            predictors[i_ptor]->Inline();
        }
    }

    // Send data and criteria to processor
    asLogMessage(_("Start processing the comparison."));

    if (!asProcessor::GetAnalogsDates(predictors, predictors, timeArrayData, timeArrayArchive, timeArrayData,
                                      timeArrayTarget, criteria, params, i_step, results, containsNaNs)) {
        asLogError(_("Failed processing the analogs dates."));
        Cleanup(predictors);
        Cleanup(criteria);
        return false;
    }
    asLogMessage(_("The processing is over."));

    // Saving intermediate results
    results.Save();

    Cleanup(predictors);
    Cleanup(criteria);

    return true;
}

bool asMethodCalibrator::GetAnalogsSubDates(asResultsAnalogsDates &results, asParametersScoring &params,
                                            asResultsAnalogsDates &anaDates, int i_step, bool &containsNaNs)
{
    // Get the linear algebra method
    ThreadsManager().CritSectionConfig().Enter();
    int linAlgebraMethod = (int) (wxFileConfig::Get()->Read("/Processing/LinAlgebra", (long) asLIN_ALGEBRA_NOVAR));
    ThreadsManager().CritSectionConfig().Leave();

    // Initialize the result object
    results.SetCurrentStep(i_step);
    results.Init(params);

    // If result file already exists, load it
    if (results.Load())
        return true;

    // Date array object instantiation for the processor
    asLogMessage(_("Creating a date arrays for the processor."));
    double timeStart = params.GetArchiveStart();
    double timeEnd = params.GetArchiveEnd();
    timeEnd = wxMin(timeEnd,
                    timeEnd - params.GetTimeSpanDays()); // Adjust so the predictors search won't overtake the array
    asTimeArray timeArrayArchive(timeStart, timeEnd, params.GetTimeArrayAnalogsTimeStepHours(), asTimeArray::Simple);
    timeArrayArchive.Init();
    asLogMessage(_("Date arrays created."));

    // Load the predictor data
    std::vector<asDataPredictor *> predictors;
    if (!LoadData(predictors, params, i_step, timeStart, timeEnd)) {
        asLogError(_("Failed loading predictor data."));
        Cleanup(predictors);
        return false;
    }

    // Create the score objects
    std::vector<asPredictorCriteria *> criteria;
    for (int i_ptor = 0; i_ptor < params.GetPredictorsNb(i_step); i_ptor++) {
        asLogMessage(_("Creating a criterion object."));
        asPredictorCriteria *criterion = asPredictorCriteria::GetInstance(params.GetPredictorCriteria(i_step, i_ptor),
                                                                          linAlgebraMethod);
        if(criterion->NeedsDataRange()) {
            wxASSERT(predictors.size()>i_ptor);
            wxASSERT(predictors[i_ptor]);
            criterion->SetDataRange(predictors[i_ptor]);
        }
        criteria.push_back(criterion);
        asLogMessage(_("Criterion object created."));
    }

    // Inline the data when possible
    for (int i_ptor = 0; i_ptor < (int) predictors.size(); i_ptor++) {
        if (criteria[i_ptor]->CanUseInline()) {
            predictors[i_ptor]->Inline();
        }
    }

    // Send data and criteria to processor
    asLogMessage(_("Start processing the comparison."));
    if (!asProcessor::GetAnalogsSubDates(predictors, predictors, timeArrayArchive, timeArrayArchive, anaDates, criteria,
                                         params, i_step, results, containsNaNs)) {
        asLogError(_("Failed processing the analogs dates."));
        Cleanup(predictors);
        Cleanup(criteria);
        return false;
    }
    asLogMessage(_("The processing is over."));

    // Saving intermediate results
    results.Save();

    Cleanup(predictors);
    Cleanup(criteria);

    return true;
}

bool asMethodCalibrator::GetAnalogsValues(asResultsAnalogsValues &results, asParametersScoring &params,
                                          asResultsAnalogsDates &anaDates, int i_step)
{
    // Initialize the result object
    results.SetCurrentStep(i_step);
    results.Init(params);

    // If result file already exists, load it
    if (results.Load())
        return true;

    // Set the predictand values to the corresponding analog dates
    wxASSERT(m_predictandDB);
    asLogMessage(_("Start setting the predictand values to the corresponding analog dates."));
    if (!asProcessor::GetAnalogsValues(*m_predictandDB, anaDates, params, results)) {
        asLogError(_("Failed setting the predictand values to the corresponding analog dates."));
        return false;
    }
    asLogMessage(_("Predictand association over."));

    // Saving intermediate results
    results.Save();

    return true;
}

bool asMethodCalibrator::GetAnalogsForecastScores(asResultsAnalogsForecastScores &results, asParametersScoring &params,
                                                  asResultsAnalogsValues &anaValues, int i_step)
{
    // Initialize the result object
    results.SetCurrentStep(i_step);
    results.Init(params);

    // If result file already exists, load it
    if (results.Load())
        return true;

    // Instantiate a forecast score object
    asLogMessage(_("Instantiating a forecast score object"));
    asForecastScore *forecastScore = asForecastScore::GetInstance(params.GetForecastScoreName());
    forecastScore->SetQuantile(params.GetForecastScoreQuantile());
    forecastScore->SetThreshold(params.GetForecastScoreThreshold());

    if (forecastScore->UsesClimatology() && m_scoreClimatology.size() == 0) {
        asLogMessage(_("Processing the score of the climatology."));

        VArray1DFloat climatologyData = GetClimatologyData(params);
        VectorInt stationIds = params.GetPredictandStationIds();
        m_scoreClimatology.resize(stationIds.size());

        for (int i_st = 0; i_st < (int) stationIds.size(); i_st++) {
            forecastScore->ProcessScoreClimatology(anaValues.GetTargetValues()[i_st], climatologyData[i_st]);
            m_scoreClimatology[i_st] = forecastScore->GetScoreClimatology();
        }
    }

    // Pass data and score to processor
    asLogMessage(_("Start processing the forecast score."));

    if (!asProcessorForecastScore::GetAnalogsForecastScores(anaValues, forecastScore, params, results,
                                                            m_scoreClimatology)) {
        asLogError(_("Failed processing the forecast score."));
        wxDELETE(forecastScore);
        return false;
    }

    // Saving intermediate results
    results.Save();

    wxDELETE(forecastScore);

    return true;
}

bool asMethodCalibrator::GetAnalogsForecastScoreFinal(asResultsAnalogsForecastScoreFinal &results,
                                                      asParametersScoring &params,
                                                      asResultsAnalogsForecastScores &anaScores, int i_step)
{
    // Initialize the result object
    results.SetCurrentStep(i_step);
    results.Init(params);

    // If result file already exists, load it
    if (results.Load())
        return true;

    // Date array object instantiation for the final score
    asLogMessage(_("Creating a date array for the final score."));
    double timeStart = params.GetCalibrationStart();
    double timeEnd = params.GetCalibrationEnd() + 1;
    while (timeEnd > params.GetCalibrationEnd() + 0.999) {
        timeEnd -= params.GetTimeArrayTargetTimeStepHours() / 24.0;
    }
    asTimeArray timeArray(timeStart, timeEnd, params.GetTimeArrayTargetTimeStepHours(),
                          params.GetForecastScoreTimeArrayMode());

    // TODO (phorton#1#): Fix me: add every options for the Init function (generic version)
    //    timeArray.Init(params.GetForecastScoreTimeArrayDate(), params.GetForecastScoreTimeArrayIntervalDays());
    timeArray.Init();
    asLogMessage(_("Date array created."));

    // Pass data and score to processor
    asLogMessage(_("Start processing the final score."));
    if (!asProcessorForecastScore::GetAnalogsForecastScoreFinal(anaScores, timeArray, params, results)) {
        asLogError(_("Failed to process the final score."));
        return false;
    }
    asLogMessage(_("Processing over."));

    // Saving intermediate results
    results.Save();

    return true;
}

bool asMethodCalibrator::SubProcessAnalogsNumber(asParametersCalibration &params,
                                                 asResultsAnalogsDates &anaDatesPrevious, int i_step)
{
    VectorInt analogsNbVect = params.GetAnalogsNumberVector(i_step);

    // Cannot be superior to previous analogs nb
    int rowEnd = int(analogsNbVect.size() - 1);
    if (i_step > 0) {
        int prevAnalogsNb = params.GetAnalogsNumber(i_step - 1);
        if (prevAnalogsNb < analogsNbVect[analogsNbVect.size() - 1]) {
            rowEnd = asTools::SortedArraySearchFloor(&analogsNbVect[0], &analogsNbVect[analogsNbVect.size() - 1],
                                                     prevAnalogsNb);
        }
    }

    asResultsAnalogsDates anaDates;
    asResultsAnalogsValues anaValues;

    // If at the end of the chain
    if (i_step == params.GetStepsNb() - 1) {
        // Set the maximum and let play with the analogs nb on the forecast score (faster)
        params.SetAnalogsNumber(i_step, analogsNbVect[rowEnd]);

        // Process first the dates and the values
        bool containsNaNs = false;
        if (i_step == 0) {
            if (!GetAnalogsDates(anaDates, params, i_step, containsNaNs))
                return false;
        } else {
            if (!GetAnalogsSubDates(anaDates, params, anaDatesPrevious, i_step, containsNaNs))
                return false;
        }
        if (containsNaNs) {
            asLogError(_("The dates selection contains NaNs"));
            return false;
        }
        if (!GetAnalogsValues(anaValues, params, anaDates, i_step))
            return false;

        asResultsAnalogsForecastScores anaScores;
        asResultsAnalogsForecastScoreFinal anaScoreFinal;
        asResultsAnalogsDates anaDatesTmp(anaDates);
        Array2DFloat dates = anaDates.GetAnalogsDates();

        for (int i_anb = 0; i_anb <= rowEnd; i_anb++) {
            params.SetAnalogsNumber(i_step, analogsNbVect[i_anb]);

            // Fixes and checks
            params.FixAnalogsNb();

            // Extract analogs dates from former results
            Array2DFloat subDates = dates.leftCols(params.GetAnalogsNumber(i_step));
            anaDatesTmp.SetAnalogsDates(subDates);

            if (!GetAnalogsForecastScores(anaScores, params, anaValues, i_step))
                return false;
            if (!GetAnalogsForecastScoreFinal(anaScoreFinal, params, anaScores, i_step))
                return false;

            m_parametersTemp.push_back(params);
            m_scoresCalibTemp.push_back(anaScoreFinal.GetForecastScore());
        }

    } else {
        int nextStep = i_step + 1;

        for (int i_anb = 0; i_anb <= rowEnd; i_anb++) {
            params.SetAnalogsNumber(i_step, analogsNbVect[i_anb]);

            // Fixes and checks
            params.FixAnalogsNb();

            // Process the dates and the values
            bool containsNaNs = false;
            if (i_step == 0) {
                if (!GetAnalogsDates(anaDates, params, i_step, containsNaNs))
                    return false;
            } else {
                if (!GetAnalogsSubDates(anaDates, params, anaDatesPrevious, i_step, containsNaNs))
                    return false;
            }
            if (containsNaNs) {
                asLogError(_("The dates selection contains NaNs"));
                return false;
            }

            // Continue
            if (!SubProcessAnalogsNumber(params, anaDates, nextStep))
                return false;
        }
    }

    return true;
}

bool asMethodCalibrator::Validate(const int bestScoreRow)
{
    bool skipValidation = false;
    wxFileConfig::Get()->Read("/Optimizer/SkipValidation", &skipValidation, false);

    if (skipValidation) {
        return true;
    }

    if (m_parameters.size() == 0) {
        asLogError("The parameters array is empty in the validation procedure.");
        return false;
    } else if (m_parameters.size() < unsigned(bestScoreRow + 1)) {
        asLogError("Trying to access parameters outside the array in the validation procedure.");
        return false;
    }

    if (!m_parameters[bestScoreRow].HasValidationPeriod()) {
        asLogWarning("The parameters have no validation period !");
        return false;
    }

    m_validationMode = true;

    asResultsAnalogsDates anaDatesPrevious;
    asResultsAnalogsDates anaDates;
    asResultsAnalogsValues anaValues;
    asResultsAnalogsForecastScores anaScores;
    asResultsAnalogsForecastScoreFinal anaScoreFinal;

    // Process every step one after the other
    int stepsNb = m_parameters[bestScoreRow].GetStepsNb();
    for (int i_step = 0; i_step < stepsNb; i_step++) {
        bool containsNaNs = false;
        if (i_step == 0) {
            if (!GetAnalogsDates(anaDates, m_parameters[bestScoreRow], i_step, containsNaNs))
                return false;
        } else {
            anaDatesPrevious = anaDates;
            if (!GetAnalogsSubDates(anaDates, m_parameters[bestScoreRow], anaDatesPrevious, i_step, containsNaNs))
                return false;
        }
        if (containsNaNs) {
            asLogError(_("The dates selection contains NaNs"));
            return false;
        }
    }
    if (!GetAnalogsValues(anaValues, m_parameters[bestScoreRow], anaDates, stepsNb - 1))
        return false;
    if (!GetAnalogsForecastScores(anaScores, m_parameters[bestScoreRow], anaValues, stepsNb - 1))
        return false;
    if (!GetAnalogsForecastScoreFinal(anaScoreFinal, m_parameters[bestScoreRow], anaScores, stepsNb - 1))
        return false;

    m_scoreValid = anaScoreFinal.GetForecastScore();

    m_validationMode = false;

    return true;
}

bool asMethodCalibrator::GetRandomValidData(asParametersScoring &params, int i_step, int i_ptor, int i_dat)
{
    VectorInt levels, hours;

    for (int i_level = 0; i_level < m_preloadedArchive[i_step][i_ptor][i_dat].size(); i_level++) {
        for (int i_hour = 0; i_hour < m_preloadedArchive[i_step][i_ptor][i_dat][i_level].size(); i_hour++) {
            if (m_preloadedArchive[i_step][i_ptor][i_dat][i_level][i_hour] != NULL) {
                levels.push_back(i_level);
                hours.push_back(i_hour);
            }
        }
    }

    wxASSERT(levels.size() == hours.size());

    int randomIndex = asTools::Random(0, levels.size() - 1, 1);
    float newLevel = params.GetPreloadLevels(i_step, i_ptor)[levels[randomIndex]];
    double newHour = params.GetPreloadTimeHours(i_step, i_ptor)[hours[randomIndex]];

    params.SetPredictorLevel(i_step, i_ptor, newLevel);
    params.SetPredictorTimeHours(i_step, i_ptor, newHour);

    return true;
}