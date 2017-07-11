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

#include "asThreadMethodOptimizerGeneticAlgorithms.h"

asThreadMethodOptimizerGeneticAlgorithms::asThreadMethodOptimizerGeneticAlgorithms(
        asMethodOptimizerGeneticAlgorithms *optimizer, asParametersOptimization *params, float *finalScoreCalib,
        vf *scoreClimatology)
        : asThread(),
          m_optimizer(optimizer),
          m_params(params),
          m_finalScoreCalib(finalScoreCalib),
          m_scoreClimatology(scoreClimatology)
{
    m_type = asThread::MethodOptimizerGeneticAlgorithms;
}

asThreadMethodOptimizerGeneticAlgorithms::~asThreadMethodOptimizerGeneticAlgorithms()
{
    //dtor
}

wxThread::ExitCode asThreadMethodOptimizerGeneticAlgorithms::Entry()
{
    // Create results objects. Needs to be in a critical section because of access to the config pointer.
    asResultsAnalogsDates anaDates;
    asResultsAnalogsDates anaDatesPrevious;
    asResultsAnalogsValues anaValues;
    asResultsAnalogsForecastScores anaScores;
    asResultsAnalogsForecastScoreFinal anaScoreFinal;

    *m_finalScoreCalib = NaNf;

    // Set the climatology score value
    if (m_scoreClimatology->size() != 0) {
        wxLogVerbose(_("Process score of the climatology"));
        m_optimizer->SetScoreClimatology(*m_scoreClimatology);
    }

    // Process every step one after the other
    int stepsNb = m_params->GetStepsNb();

    if (stepsNb == 0) {
        wxLogError(_("The number of processing steps is null in asThreadMethodOptimizerGeneticAlgorithms."));
        return NULL;
    }

    for (int iStep = 0; iStep < stepsNb; iStep++) {
        bool containsNaNs = false;
        if (iStep == 0) {
            if (!m_optimizer->GetAnalogsDates(anaDates, *m_params, iStep, containsNaNs)) {
                wxLogError(_("Failed processing the analogs dates"));
                return NULL;
            }
            anaDatesPrevious = anaDates;
        } else {
            if (!m_optimizer->GetAnalogsSubDates(anaDates, *m_params, anaDatesPrevious, iStep, containsNaNs)) {
                wxLogError(_("Failed processing the analogs sub dates"));
                return NULL;
            }
            anaDatesPrevious = anaDates;
        }
        if (containsNaNs) {
            wxLogError(_("The dates selection contains NaNs"));
            return NULL;
        }
        if (anaDates.GetTargetDates().size() == 0 || anaDates.GetAnalogsDates().size() == 0 ||
            anaDates.GetAnalogsCriteria().size() == 0) {
            wxLogError(_("The asResultsAnalogsDates object is empty in asThreadMethodOptimizerGeneticAlgorithms."));
            return NULL;
        }
    }
    if (!m_optimizer->GetAnalogsValues(anaValues, *m_params, anaDates, stepsNb - 1)) {
        wxLogError(_("Failed processing the analogs values"));
        return NULL;
    }
    if (!m_optimizer->GetAnalogsForecastScores(anaScores, *m_params, anaValues, stepsNb - 1)) {
        wxLogError(_("Failed processing the forecast scores"));
        return NULL;
    }
    if (!m_optimizer->GetAnalogsForecastScoreFinal(anaScoreFinal, *m_params, anaScores, stepsNb - 1)) {
        wxLogError(_("Failed processing the final score"));
        return NULL;
    }
    *m_finalScoreCalib = anaScoreFinal.GetForecastScore();

    if (m_scoreClimatology->size() == 0) {
        *m_scoreClimatology = m_optimizer->GetScoreClimatology();
    }

    return 0;
}
