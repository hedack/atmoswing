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
 * Portions Copyright 2018 Pascal Horton, University of Bern.
 */

#include "asAreaCompGenGrid.h"

asAreaCompGenGrid::asAreaCompGenGrid(const Coo &cornerUL, const Coo &cornerUR, const Coo &cornerLL, const Coo &cornerLR,
                                     float level, float height, int flatAllowed)
        : asAreaCompGrid(cornerUL, cornerUR, cornerLL, cornerLR, level, height, flatAllowed),
          m_xPtsNb(0),
          m_yPtsNb(0)
{
    m_gridType = Generic;
    m_axesInitialized = false;
}

asAreaCompGenGrid::asAreaCompGenGrid(double xMin, double xWidth, double yMin, double yWidth, float level, float height,
                                     int flatAllowed)
        : asAreaCompGrid(xMin, xWidth, yMin, yWidth, level, height, flatAllowed),
          m_xPtsNb(0),
          m_yPtsNb(0)
{
    m_gridType = Generic;
    m_axesInitialized = false;
}

asAreaCompGenGrid::asAreaCompGenGrid(double xMin, int xPtsNb, double yMin, int yPtsNb, float level, float height,
                                     int flatAllowed)
        : asAreaCompGrid(xMin, 0, yMin, 0, level, height, flatAllowed),
          m_xPtsNb(xPtsNb),
          m_yPtsNb(yPtsNb)
{
    m_gridType = Generic;
    m_axesInitialized = false;
}

bool asAreaCompGenGrid::GridsOverlay(asAreaCompGrid *otherarea) const
{
    return false;
}

a1d asAreaCompGenGrid::GetXaxisComposite(int compositeNb)
{
    double xMin = GetComposite(compositeNb).GetXmin();
    double xMax = GetComposite(compositeNb).GetXmax();

    int xMinIndex = asFind(&m_fullAxisX[0], &m_fullAxisX[m_fullAxisX.size() - 1], xMin, 0.01);
    int xMaxIndex = asFind(&m_fullAxisX[0], &m_fullAxisX[m_fullAxisX.size() - 1], xMax, 0.01);

    wxASSERT(xMinIndex >= 0);
    wxASSERT(xMaxIndex >= 0);
    wxASSERT(xMaxIndex >= xMinIndex);

    return m_fullAxisX.segment(xMinIndex, xMaxIndex - xMinIndex + 1);
}

a1d asAreaCompGenGrid::GetYaxisComposite(int compositeNb)
{
    double yMin = GetComposite(compositeNb).GetYmin();
    double yMax = GetComposite(compositeNb).GetYmax();

    int yMinIndex = asFind(&m_fullAxisY[0], &m_fullAxisY[m_fullAxisY.size() - 1], yMin, 0.01);
    int yMaxIndex = asFind(&m_fullAxisY[0], &m_fullAxisY[m_fullAxisY.size() - 1], yMax, 0.01);

    wxASSERT(yMinIndex >= 0);
    wxASSERT(yMaxIndex >= 0);
    wxASSERT(yMaxIndex >= yMinIndex);

    return m_fullAxisY.segment(yMinIndex, yMaxIndex - yMinIndex + 1);
}

int asAreaCompGenGrid::GetXaxisCompositePtsnb(int compositeNb)
{
    double xMin = GetComposite(compositeNb).GetXmin();
    double xMax = GetComposite(compositeNb).GetXmax();

    int xMinIndex = asFind(&m_fullAxisX[0], &m_fullAxisX[m_fullAxisX.size() - 1], xMin, 0.01);
    int xMaxIndex = asFind(&m_fullAxisX[0], &m_fullAxisX[m_fullAxisX.size() - 1], xMax, 0.01);

    wxASSERT(xMinIndex >= 0);
    wxASSERT(xMaxIndex >= 0);

    int ptsnb = xMaxIndex - xMinIndex;

    if (compositeNb == 0) // from 0
    {
        ptsnb += 1;
    } else if (compositeNb == 1) // to 360
    {
        ptsnb += 1;
    } else {
        asThrowException(_("The latitude split is not implemented yet."));
    }

    return ptsnb;
}

int asAreaCompGenGrid::GetYaxisCompositePtsnb(int compositeNb)
{
    double yMin = GetComposite(compositeNb).GetYmin();
    double yMax = GetComposite(compositeNb).GetYmax();

    int yMinIndex = asFind(&m_fullAxisY[0], &m_fullAxisY[m_fullAxisY.size() - 1], yMin, 0.01);
    int yMaxIndex = asFind(&m_fullAxisY[0], &m_fullAxisY[m_fullAxisY.size() - 1], yMax, 0.01);

    wxASSERT(yMinIndex >= 0);
    wxASSERT(yMaxIndex >= 0);

    int ptsnb = yMaxIndex - yMinIndex;
    ptsnb += 1;

    return ptsnb;
}

double asAreaCompGenGrid::GetXaxisCompositeWidth(int compositeNb) const
{
    return std::abs(GetComposite(compositeNb).GetXmax() - GetComposite(compositeNb).GetXmin());
}

double asAreaCompGenGrid::GetYaxisCompositeWidth(int compositeNb) const
{
    return std::abs(GetComposite(compositeNb).GetYmax() - GetComposite(compositeNb).GetYmin());
}

double asAreaCompGenGrid::GetXaxisCompositeStart(int compositeNb) const
{
    // If only one composite
    if (GetNbComposites() == 1) {
        return GetComposite(compositeNb).GetXmin();
    }

    // If multiple composites
    if (compositeNb == 0) // from 0
    {
        return GetComposite(compositeNb).GetXmin();
    } else if (compositeNb == 1) // to 360
    {
        return GetComposite(compositeNb).GetXmin();
    } else {
        asThrowException(_("The latitude split is not implemented yet."));
    }
}

double asAreaCompGenGrid::GetYaxisCompositeStart(int compositeNb) const
{
    // If only one composite
    if (GetNbComposites() == 1) {
        return GetComposite(compositeNb).GetYmin();
    }

    // If multiple composites
    if (compositeNb == 0) // from 0
    {
        return GetComposite(compositeNb).GetYmin();
    } else if (compositeNb == 1) // to 360
    {
        return GetComposite(compositeNb).GetYmin();
    } else {
        asThrowException(_("The latitude split is not implemented yet."));
    }
}

double asAreaCompGenGrid::GetXaxisCompositeEnd(int compositeNb) const
{
    // If only one composite
    if (GetNbComposites() == 1) {
        return GetComposite(compositeNb).GetXmax();
    }

    // If multiple composites
    if (compositeNb == 1) // to 360
    {
        return GetComposite(compositeNb).GetXmax();
    } else if (compositeNb == 0) // from 0
    {
        return GetComposite(compositeNb).GetXmax();
    } else {
        asThrowException(_("The latitude split is not implemented yet."));
    }
}

double asAreaCompGenGrid::GetYaxisCompositeEnd(int compositeNb) const
{
    // If only one composite
    if (GetNbComposites() == 1) {
        return GetComposite(compositeNb).GetYmax();
    }

    // If multiple composites
    if (compositeNb == 1) // to 360
    {
        return GetComposite(compositeNb).GetYmax();
    } else if (compositeNb == 0) // from 0
    {
        return GetComposite(compositeNb).GetYmax();
    } else {
        asThrowException(_("The latitude split is not implemented yet."));
    }
}

bool asAreaCompGenGrid::IsOnGrid(const Coo &point) const
{
    if (!IsRectangle())
        return false;

    int foundX = asFind(&m_fullAxisX[0], &m_fullAxisX[m_fullAxisX.size() - 1], point.x, 0.01);
    if ((foundX == asNOT_FOUND) || (foundX == asOUT_OF_RANGE))
        return false;

    int foundY = asFind(&m_fullAxisY[0], &m_fullAxisY[m_fullAxisY.size() - 1], point.y, 0.01);
    if ((foundY == asNOT_FOUND) || (foundY == asOUT_OF_RANGE))
        return false;

    return true;
}

bool asAreaCompGenGrid::IsOnGrid(double xCoord, double yCoord) const
{
    int foundX = asFind(&m_fullAxisX[0], &m_fullAxisX[m_fullAxisX.size() - 1], xCoord, 0.01);
    if ((foundX == asNOT_FOUND) || (foundX == asOUT_OF_RANGE))
        return false;

    int foundY = asFind(&m_fullAxisY[0], &m_fullAxisY[m_fullAxisY.size() - 1], yCoord, 0.01);
    if ((foundY == asNOT_FOUND) || (foundY == asOUT_OF_RANGE))
        return false;

    return true;
}