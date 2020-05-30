/****************************************************************************
* Copyright (C) 2019 by Matthias Birnthaler                                 *
*                                                                           *
* This file is part of the OverwriteCuddleWalking Plugin for Ark Server API *
*                                                                           *
*   The OverwriteCuddleWalking Plugin is free software: you can             *
*	redistribute it and/or modify it under the terms of the GNU Lesser      *
*	General Public License as published by the Free Software Foundation,    *
*	either version 3 of the License, or (at your option) any later version. *
*                                                                           *
*   The OverwriteCuddleWalking Plugin is distributed in the hope that it    *
*   will be useful, but WITHOUT ANY WARRANTY; without even the implied      *
* 	warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.        *
* 	See the GNU Lesser General Public License for more details.             *
*                                                                           *
****************************************************************************/

/**
* \file Util.h
* \author Matthias Birnthaler
* \date 19 March 2019
* \brief header file of utility functions
*
*/


#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <map>

namespace Util {

    struct CuddleData_t {
        float CuddleIntervalTime;
        float CuddleIncreasePercent;
    };


    /**
    * \brief GetFoodList
    *
    * This function determines the FoodList from the json config
    *
    * \return std::vector<std::wstring>
    */
    extern bool ReloadConfig(void);

    extern bool GetRandomFood(std::wstring * food);

    extern bool GetDinoCuddleData(std::string DinoTag, CuddleData_t * CuddleData);

    extern bool CompFloat(float A, float B, float epsilon = 0.0000001f);

    extern void Util_Init(void);

}

 

#endif /* UTIL_H */