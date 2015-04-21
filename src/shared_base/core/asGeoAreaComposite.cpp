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
 */
 
#include "asGeoAreaComposite.h"

asGeoAreaComposite::asGeoAreaComposite(const Coo &CornerUL, const Coo &CornerUR, const Coo &CornerLL, const Coo &CornerLR, float Level, float Height, int flatAllowed)
:
asGeo()
{
    // Set the members
    m_cornerUL = CornerUL;
    m_cornerUR = CornerUR;
    m_cornerLL = CornerLL;
    m_cornerLR = CornerLR;
    m_level = Level;
    m_height = Height;
    m_flatAllowed = flatAllowed;
    m_absoluteXmin = m_cornerUL.x;
    m_absoluteXmax = m_cornerUR.x;
    m_absoluteYmin = m_cornerLL.y;
    m_absoluteYmax = m_cornerUL.y;

    // Initialization and check points
    Init();
    CreateComposites();

    wxLogVerbose(_("The composite area was successfully created."));
}

asGeoAreaComposite::asGeoAreaComposite(double Xmin, double Xwidth, double Ymin, double Ywidth, float Level, float Height, int flatAllowed)
:
asGeo()
{
    // Set the members
    m_cornerUL.x = Xmin;
    m_cornerUL.y = Ymin+Ywidth;
    m_cornerUR.x = Xmin+Xwidth;
    m_cornerUR.y = Ymin+Ywidth;
    m_cornerLL.x = Xmin;
    m_cornerLL.y = Ymin;
    m_cornerLR.x = Xmin+Xwidth;
    m_cornerLR.y = Ymin;
    m_level = Level;
    m_height = Height;
    m_flatAllowed = flatAllowed;
    m_absoluteXmin = m_cornerUL.x;
    m_absoluteXmax = m_cornerUR.x;
    m_absoluteYmin = m_cornerLL.y;
    m_absoluteYmax = m_cornerUL.y;

    // Initialization and check points
    Init();
    CreateComposites();

    wxLogVerbose(_("The composite area was successfully created."));
}

asGeoAreaComposite::asGeoAreaComposite(float Level, float Height)
:
asGeo()
{
    // Set the members
    m_level = Level;
    m_height = Height;
    m_nbComposites = 0;
    m_cornerUL.x = 0;
    m_cornerUL.y = 0;
    m_cornerUR.x = 0;
    m_cornerUR.y = 0;
    m_cornerLL.x = 0;
    m_cornerLL.y = 0;
    m_cornerLR.x = 0;
    m_cornerLR.y = 0;
    m_flatAllowed = asFLAT_ALLOWED;
    m_absoluteXmin = 0;
    m_absoluteXmax = 0;
    m_absoluteYmin = 0;
    m_absoluteYmax = 0;
}

asGeoAreaComposite::~asGeoAreaComposite()
{
    //dtor
}

void asGeoAreaComposite::Generate(double Xmin, double Xwidth, double Ymin, double Ywidth, int flatAllowed)
{
    // Set the members
    m_cornerUL.x = Xmin;
    m_cornerUL.y = Ymin+Ywidth;
    m_cornerUR.x = Xmin+Xwidth;
    m_cornerUR.y = Ymin+Ywidth;
    m_cornerLL.x = Xmin;
    m_cornerLL.y = Ymin;
    m_cornerLR.x = Xmin+Xwidth;
    m_cornerLR.y = Ymin;
    m_flatAllowed = flatAllowed;
    m_absoluteXmin = m_cornerUL.x;
    m_absoluteXmax = m_cornerUR.x;
    m_absoluteYmin = m_cornerLL.y;
    m_absoluteYmax = m_cornerUL.y;

    // Initialization and check points
    Init();
    CreateComposites();

    wxLogVerbose(_("The composite area was successfully generated."));
}

void asGeoAreaComposite::Init()
{
    InitBounds();
    DoCheckPoints();
    if (!CheckConsistency()) asThrowException(_("Unable to build a consistent area with the given coordinates."));
}

bool asGeoAreaComposite::DoCheckPoints()
{
    // Check the points and proceed to changes if necessary
    CheckPoint(m_cornerUL, asEDIT_ALLOWED);
    CheckPoint(m_cornerUR, asEDIT_ALLOWED);
    CheckPoint(m_cornerLL, asEDIT_ALLOWED);
    CheckPoint(m_cornerLR, asEDIT_ALLOWED);
    return true;
}

bool asGeoAreaComposite::CheckConsistency()
{
    // Area is a single point
    if (m_flatAllowed == asFLAT_FORBIDDEN)
    {
        if ((m_cornerUL.x == m_cornerUR.x) || (m_cornerLL.x == m_cornerLR.x) || (m_cornerLL.y == m_cornerUL.y) || (m_cornerLR.y == m_cornerUR.y))
        {
            return false;
        }
    }

    // Lon min is on the edge and should be corrected
    if ((m_cornerUL.x > m_cornerUR.x) && (m_cornerUL.x == m_axisXmax))
    {
        m_cornerUL.x -= m_axisXmax;
    }
    if ((m_cornerLL.x > m_cornerLR.x) && (m_cornerLL.x == m_axisXmax))
    {
        m_cornerLL.x -= m_axisXmax;
    }

    // Coordinates order vary
    if ((m_cornerUL.x > m_cornerUR.x) || (m_cornerLL.x > m_cornerLR.x) || (m_cornerLL.y > m_cornerUL.y) || (m_cornerLR.y > m_cornerUR.y))
    {
        // Do not proceed to change
        wxLogVerbose(_("The given coordinates are not increasing. This is a normal behavior if the area is on the coordinates edge."));
    }

    return true;
}

double asGeoAreaComposite::GetXmin()
{
    double RealXmin = InfDouble;
    for (int i_area = 0; i_area<m_nbComposites; i_area++)
    {
        RealXmin = wxMin(RealXmin, m_composites[i_area].GetXmin());
    }
    return RealXmin;
}

double asGeoAreaComposite::GetXmax()
{
    double RealXmax = -InfDouble;
    for (int i_area = 0; i_area<m_nbComposites; i_area++)
    {
        RealXmax = wxMax(RealXmax, m_composites[i_area].GetXmax());
    }
    return RealXmax;
}

double asGeoAreaComposite::GetYmin()
{
    double RealYmin = InfDouble;
    for (int i_area = 0; i_area<m_nbComposites; i_area++)
    {
        RealYmin = wxMin(RealYmin, m_composites[i_area].GetYmin());
    }
    return RealYmin;
}

double asGeoAreaComposite::GetYmax()
{
    double RealYmax = -InfDouble;
    for (int i_area = 0; i_area<m_nbComposites; i_area++)
    {
        RealYmax = wxMax(RealYmax, m_composites[i_area].GetYmax());
    }
    return RealYmax;
}

Coo asGeoAreaComposite::GetCenter()
{
    Coo center;

    if((m_cornerUL.x<m_cornerUR.x) & (m_cornerLL.x<m_cornerLR.x) & (m_cornerLL.y<m_cornerUL.y) & (m_cornerLR.y<m_cornerUR.y))
    {
        center = m_composites[0].GetCenter();
    }
    else if((m_cornerUL.x>m_cornerUR.x) & (m_cornerLL.x>m_cornerLR.x) & (m_cornerLL.y<m_cornerUL.y) & (m_cornerLR.y<m_cornerUR.y))
    {
        double CornerUR = 360+m_cornerUR.x;
        double CornerLR = 360+m_cornerLR.x;
        double Xmin = wxMin(m_cornerUL.x, m_cornerLL.x);
        double Xmax = wxMin(CornerUR, CornerLR);
        center.x = Xmin + (Xmax-Xmin)/2;
        center.y = GetYmin() + (GetYmax()-GetYmin())/2;
        return center;
    }
    else
    {
// TODO (phorton#1#): Implement me !
        asThrowException(_("This case is not managed yet."));
    }


    center.x = GetXmin() + (GetXmax()-GetXmin())/2;
    center.y = GetYmin() + (GetYmax()-GetYmin())/2;
    return center;
}

bool asGeoAreaComposite::IsRectangle()
{
    // Check that the area is a square
    if ((m_cornerUL.x != m_cornerLL.x) | (m_cornerUL.y != m_cornerUR.y) | (m_cornerUR.x != m_cornerLR.x) | (m_cornerLL.y != m_cornerLR.y))
    {
        return false;
    }
    return true;
}

void asGeoAreaComposite::CreateComposites()
{
    m_composites.clear();
    m_nbComposites = 0;

    if((m_cornerUL.x<=m_cornerUR.x) & (m_cornerLL.x<=m_cornerLR.x) & (m_cornerLL.y<=m_cornerUL.y) & (m_cornerLR.y<=m_cornerUR.y))
    {
        asGeoArea area(m_cornerUL, m_cornerUR, m_cornerLL, m_cornerLR, m_level, m_height, m_flatAllowed);
        m_composites.push_back(area);
        m_nbComposites = 1;
    }
    else if((m_cornerUL.x>=m_cornerUR.x) & (m_cornerLL.x>=m_cornerLR.x) & (m_cornerLL.y<=m_cornerUL.y) & (m_cornerLR.y<=m_cornerUR.y) & (m_cornerLR.x==m_axisXmin) & (m_cornerUR.x==m_axisXmin))
    {
        m_cornerLR.x = m_axisXmax;
        m_cornerUR.x = m_axisXmax;
        asGeoArea area(m_cornerUL, m_cornerUR, m_cornerLL, m_cornerLR, m_level, m_height, m_flatAllowed);
        m_composites.push_back(area);
        m_nbComposites = 1;
    }
    else if((m_cornerUL.x>=m_cornerUR.x) & (m_cornerLL.x>=m_cornerLR.x) & (m_cornerLL.y<=m_cornerUL.y) & (m_cornerLR.y<=m_cornerUR.y) & (m_cornerLR.x!=m_axisXmin) & (m_cornerUR.x!=m_axisXmin))
    {
        Coo a1UL = m_cornerUL, a1UR = m_cornerUR, a1LL = m_cornerLL, a1LR = m_cornerLR;
        Coo a2UL = m_cornerUL, a2UR = m_cornerUR, a2LL = m_cornerLL, a2LR = m_cornerLR;
        a1UL.x = m_axisXmin;
        a1LL.x = m_axisXmin;
        a2UR.x = m_axisXmax;
        a2LR.x = m_axisXmax;
        asGeoArea area1(a1UL, a1UR, a1LL, a1LR, m_level, m_height, m_flatAllowed);
        asGeoArea area2(a2UL, a2UR, a2LL, a2LR, m_level, m_height, m_flatAllowed);
        m_composites.push_back(area1);
        m_composites.push_back(area2);
        m_nbComposites = 2;
    }
    else
    {
// TODO (phorton#1#): Implement me and check the other functions (GetCenter(), ...)!
        wxString error = "This case is not managed yet (asGeoAreaComposite::CreateComposites):\n ";
        error.Append(wxString::Format( "m_cornerUL.x = %g\n", m_cornerUL.x ));
        error.Append(wxString::Format( "m_cornerUR.x = %g\n", m_cornerUR.x ));
        error.Append(wxString::Format( "m_cornerLL.x = %g\n", m_cornerLL.x ));
        error.Append(wxString::Format( "m_cornerLR.x = %g\n", m_cornerLR.x ));
        error.Append(wxString::Format( "m_cornerLL.y = %g\n", m_cornerLL.y ));
        error.Append(wxString::Format( "m_cornerUL.y = %g\n", m_cornerUL.y ));
        error.Append(wxString::Format( "m_cornerLR.y = %g\n", m_cornerLR.y ));
        error.Append(wxString::Format( "m_cornerUR.y = %g\n", m_cornerUR.y ));
        asLogError(error);
        asThrowException(_("This case is not managed yet."));
    }
}
