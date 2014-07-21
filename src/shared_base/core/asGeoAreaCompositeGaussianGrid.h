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
 
#ifndef asGeoAreaCompositeGaussianGrid_H
#define asGeoAreaCompositeGaussianGrid_H

#include <asIncludes.h>
#include <asGeoAreaCompositeGrid.h>
#include <asGeoAreaGaussianGrid.h>

class asGeoAreaCompositeGaussianGrid: public asGeoAreaCompositeGrid
{
public:

    /** Default constructor
     * \param coosys The coordinate system
     * \param CornerUL The coordinates of the upper left corner
     * \param CornerUR The coordinates of the upper right corner
     * \param CornerLL The coordinates of the lower left corner
     * \param CornerLR The coordinates of the lower right corner
     * \param Ustep The step according to the U axis
     * \param Vstep The step according to the v axis
     * \param Level The height in hPa
     * \param Height The height in m
     * \param flatAllowed Allows the area to have a dimension that is null
     */
    asGeoAreaCompositeGaussianGrid(CoordSys coosys, const Coo &CornerUL, const Coo &CornerUR, const Coo &CornerLL, const Coo &CornerLR, double Ustep, double Vstep, asGeoAreaGaussianGrid::GaussianGridType type=asGeoAreaGaussianGrid::T62, float Level = asNONE, float Height = asNONE, int flatAllowed = asFLAT_FORBIDDEN);

    /** Alternative constructor
     * \param coosys The coordinate system
     * \param Umin The left border
     * \param Uptsnb The size on U axis
     * \param Ustep The step according to the U axis
     * \param Vmin The left border
     * \param Vptsnb The size on V axis
     * \param Vstep The step according to the v axis
     * \param Level The height in hPa
     * \param Height The height in m
     * \param flatAllowed Allows the area to have a dimension that is null
     */
    asGeoAreaCompositeGaussianGrid(CoordSys coosys, double Umin, int Uptsnb, double Vmin, int Vptsnb, asGeoAreaGaussianGrid::GaussianGridType type=asGeoAreaGaussianGrid::T62, float Level = asNONE, float Height = asNONE, int flatAllowed = asFLAT_FORBIDDEN);

    /** Default destructor */
    virtual ~asGeoAreaCompositeGaussianGrid();


    bool GridsOverlay(asGeoAreaCompositeGrid *otherarea);

    asGeoAreaGaussianGrid::GaussianGridType GetGaussianGridType()
    {
        return m_GaussianGridType;
    }

    /** Access m_Ustep
     * \return The current value of m_Ustep
     */
    double GetUstep()
    {
        return 0.0;
    }

    /** Access m_Vstep
     * \return The current value of m_Vstep
     */
    double GetVstep()
    {
        return 0.0;
    }

    /** Get the U axis for the given composite
     * \param compositeNb The desired composite
     * \return The axis built on the boundaries and the step
     */
    Array1DDouble GetUaxisComposite(int compositeNb);

    /** Get the V axis for the given composite
     * \param compositeNb The desired composite
     * \return The axis built on the boundaries and the step
     */
    Array1DDouble GetVaxisComposite(int compositeNb);

    /** Get the size of the U axis for the given composite
     * \param compositeNb The desired composite
     * \return The size of the axis
     */
    int GetUaxisCompositePtsnb(int compositeNb);

    /** Get the size of the V axis for the given composite
     * \param compositeNb The desired composite
     * \return The size of the axis
     */
    int GetVaxisCompositePtsnb(int compositeNb);

    /** Get the width of the U axis for the given composite
     * \param compositeNb The desired composite
     * \return The width of the axis
     */
    double GetUaxisCompositeWidth(int compositeNb);

    /** Get the width of the V axis for the given composite
     * \param compositeNb The desired composite
     * \return The width of the axis
     */
    double GetVaxisCompositeWidth(int compositeNb);

    /** Get the start value of the U axis for the given composite
     * \param compositeNb The desired composite
     * \return The start value of the axis
     */
    double GetUaxisCompositeStart(int compositeNb);

    /** Get the start value of the V axis for the given composite
     * \param compositeNb The desired composite
     * \return The start value of the axis
     */
    double GetVaxisCompositeStart(int compositeNb);

    /** Get the last value of the U axis for the given composite
     * \param compositeNb The desired composite
     * \return The last value of the axis
     */
    double GetUaxisCompositeEnd(int compositeNb);

    /** Get the last value of the V axis for the given composite
     * \param compositeNb The desired composite
     * \return The last value of the axis
     */
    double GetVaxisCompositeEnd(int compositeNb);


protected:

private:
    asGeoAreaGaussianGrid::GaussianGridType m_GaussianGridType;
    Array1DDouble m_FullAxisU;
    Array1DDouble m_FullAxisV;

    bool IsOnGrid(const Coo &point);
    bool IsOnGrid(double Ucoord, double Vcoord);
};

#endif // asGeoAreaCompositeGaussianGrid_H
