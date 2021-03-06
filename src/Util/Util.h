/*****************************************************************************************************************
* Copyright (C) 2019 by Matthias Birnthaler                                                                      *
*                                                                                                                *
* This file is part of the Cuddle Plugin for Ark Server API                                                      *
*                                                                                                                *
*    This program is free software : you can redistribute it and/or modify                                       *
*    it under the terms of the GNU General Public License as published by                                        *
*    the Free Software Foundation, either version 3 of the License, or                                           *
*    (at your option) any later version.                                                                         *
*                                                                                                                *
*    This program is distributed in the hope that it will be useful,                                             *
*    but WITHOUT ANY WARRANTY; without even the implied warranty of                                              *
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the                                                 *
*    GNU General Public License for more details.                                                                *
*                                                                                                                *
*    You should have received a copy of the GNU General Public License                                           *
*    along with this program.If not, see <https://www.gnu.org/licenses/>.                                        *
*                                                                                                                *
*****************************************************************************************************************/

/**
* \file Util.h
* \author Matthias Birnthaler Matthias-Birnthaler@outlook.com
* \date 26 July 2020
* \brief Implementation of all utility functions
*
*/


#ifndef UTIL_H
#define UTIL_H

/* ================================================[includes]================================================ */

#include <string>


namespace Util {

    /* ============================================ [type definitions] ============================================ */

    struct CuddleData_t {
        /** \brief the cuddle interval time in secounds*/
        float CuddleIntervalTime;
        /** \brief the increase per cuddle in floating point representation*/
        float CuddleIncreasePercent;
    };

    /* ================================================[declaration of public functions]========================= */

    extern bool ReloadConfig(void);
    extern bool GetRandomFood(std::wstring * food);
    extern bool GetDinoCuddleData(std::string DinoTag, CuddleData_t * CuddleData);
    extern bool CompFloat(float A, float B, float epsilon = 0.0000001f);
    extern bool IsCuddleTypeWalkActive(void);
}


#endif /* UTIL_H */

/* =================================================[end of file]================================================= */
