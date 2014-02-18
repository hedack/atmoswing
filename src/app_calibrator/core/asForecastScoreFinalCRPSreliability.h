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
 * Portions Copyright 2014 Renaud Marty, DREAL.
 */
 
#ifndef ASFORECASTSCOREFINALCRPSRELIABILITY_H
#define ASFORECASTSCOREFINALCRPSRELIABILITY_H

#include <asIncludes.h>
#include <asForecastScoreFinal.h>

class asForecastScoreFinalCRPSreliability: public asForecastScoreFinal
{
public:
    asForecastScoreFinalCRPSreliability(Period period);

    asForecastScoreFinalCRPSreliability(const wxString& periodString);

    virtual ~asForecastScoreFinalCRPSreliability();

    float Assess(Array1DFloat &targetDates, Array1DFloat &forecastScores, asTimeArray &timeArray);

    float Assess(Array1DFloat &targetDates, Array2DFloat &forecastScores, asTimeArray &timeArray);

protected:
private:

};

#endif // ASFORECASTSCOREFINALCRPSRELIABILITY_H