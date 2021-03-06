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
* \file Util.cpp
* \author Matthias Birnthaler Matthias-Birnthaler@outlook.com
* \date 26 July 2020
* \brief Implementation of all utility functions
*
*/

/* ================================================ [includes] ================================================ */
#include "Util.h"

#include "Tools.h"
#include "Requests.h"
#include "ICommands.h"
#include "IHooks.h"
#include "API/ARK/Ark.h"

#include <fstream>
#include "json.hpp"
#include <vector>
#include <map>



namespace Util {

    /* ============================================ [type definitions] ============================================ */
    /** \brief factor to convert minutes to secounds */
    #define FACTOR_MINUTES_TO_SECONDS   60
    /* =============================================== [local data] =============================================== */


    /** \brief Configured list with food for the cuddly type food */
    static std::vector<std::wstring> Util_Foodlist;

    /** \brief Configured dino list */
    static std::map<std::string, CuddleData_t> Util_Dinolist;

    /** \brief Configuration (on/off) for the cuddle type walk*/
    static bool Util_IsCuddleTypeWalkActive;


    /* ===================================== [prototype of local functions] ======================================= */

    static nlohmann::json ReadConfig();
    static std::vector<std::string> split(std::string stringToBeSplitted, std::string delimeter);
    static std::vector<std::wstring> DetermineFoodList(nlohmann::json config);
    static std::map<std::string, CuddleData_t> DetermineDinoList(nlohmann::json config);


    /**
    * \brief ReadConfig
    *
    * This function reads out the json configuration file
    *
    * \return nlohmann::json configuration of the plugin
    */
    static nlohmann::json ReadConfig(void)
    {
        nlohmann::json config;

        const std::string config_path = ArkApi::Tools::GetCurrentDir() + "/ArkApi/Plugins/CuddlePlugin/config.json";
        std::ifstream file{ config_path };
        if (!file.is_open())
            throw std::runtime_error("Can't open config.json");

        file >> config;

        file.close();

        return config;
    }


    /**
    * \brief split
    *
    * This function splits a given string \p stringToBeSplitted by the given \p delimeter
    *
    * \param[in] stringToBeSplitted the string that need to be splitted
    * \param[in] delimeter the delimiter that splits the given string stringToBeSplitted
    * \return std::vector<std::string>
    */
    static std::vector<std::string> split(std::string stringToBeSplitted, std::string delimeter)
    {
        std::vector<std::string> splittedString;
        size_t startIndex = 0;
        size_t  endIndex = 0;
        while ((endIndex = stringToBeSplitted.find(delimeter, startIndex)) < stringToBeSplitted.size())
        {
            std::string val = stringToBeSplitted.substr(startIndex, endIndex - startIndex);
            splittedString.push_back(val);
            startIndex = endIndex + delimeter.size();
        }
        if (startIndex < stringToBeSplitted.size())
        {
            std::string val = stringToBeSplitted.substr(startIndex);
            splittedString.push_back(val);
        }
        return splittedString;
    }


    /**
    * \brief DetermineFoodList
    *
    * This function determines the configured food list from the json configuration
    *
    * \param[in] nlohmann::json configuration of the plugin
    * \return vector<std::wstring> vector with the food configured list
    */
    std::vector<std::wstring> DetermineFoodList(nlohmann::json config)
    {
        std::vector<std::wstring> list;

        auto items_map = config.value("Footlist", nlohmann::json::array());

        for (const auto& item : items_map)
        {
            /* "Blueprint'/Game/PrimalEarth/CoreBlueprints/Items/Consumables/PrimalItemConsumable_Kibble_Base_Large.PrimalItemConsumable_Kibble_Base_Large'" */
            /*           ^   <--split                                                                                                       split   -->   ^  */
            std::vector<std::string> bpsplit = split(item, "'");
            std::string blueprint = bpsplit[1] + "_C";

            std::wstring wblueprint = std::wstring(blueprint.begin(), blueprint.end());

            list.push_back(wblueprint);

        }
        return list;
    }


    /**
    * \brief DetermineDinoList
    *
    * This function determines the configured dino list from the json configuration
    * \return nlohmann::json configuration of the plugin
    * \return map<std::string, CuddleData_t> map with the dino configurations 
    */
    std::map<std::string, CuddleData_t> DetermineDinoList(nlohmann::json config)
    {

        std::map<std::string, CuddleData_t> dinolist;


        for (auto it = config["Dinolist"].begin(); it != config["Dinolist"].end(); ++it)
        {
            CuddleData_t CuddleData;
            CuddleData.CuddleIntervalTime = (float)it.value()["CuddleIntervalTime"];
            CuddleData.CuddleIncreasePercent = (float)it.value()["CuddleIncreasePercent"];

            dinolist.insert(std::pair<std::string, CuddleData_t>((std::string)it.key(), CuddleData));
        }

        return dinolist;
    }


    /* ===================================== [definition of global functions] ===================================== */


    /**
    * \brief CompFloat
    *
    * This function implements the float equals operation
    *
    * \param[in] A Operand A
    * \param[in] B Operand B 
    * \param[in] epsilon almost equal floating point accuracy
    * \return bool: true if equal floating point numbers, otherwise false
    */
    bool CompFloat(float A, float B, float epsilon)
    {
        return (fabs(A - B) < epsilon);
    }


    /**
    * \brief GetDinoCuddleData
    *
    * This function returns the cuddle data for a given dino tag
    *
    * \param[in] DinoTag the dino tag to be determine 
    * \param[out] CuddleData the cuddle data
    * \return bool: true if if the dino tag could be determined, otherwise false 
    */
    bool GetDinoCuddleData(std::string DinoTag, CuddleData_t * CuddleData)
    {
        bool result = false;

        if (nullptr != CuddleData)
        {
            auto it = Util_Dinolist.find(DinoTag);


            if (Util_Dinolist.end() != it)
            {
                (*CuddleData).CuddleIncreasePercent = it->second.CuddleIncreasePercent;
                (*CuddleData).CuddleIntervalTime = it->second.CuddleIntervalTime * FACTOR_MINUTES_TO_SECONDS;

                result = true;
            }

        }

        return result;
    }


    /**
    * \brief IsCuddleTypeWalkActive
    *
    * This function returns if cuddle type walk is active 
    *
    * \return bool, returns the active configuration for cuddle type walk
    */
    bool IsCuddleTypeWalkActive()
    {
        return Util_IsCuddleTypeWalkActive;
    }

    
    /**
    * \brief GetRandomFood
    *
    * This function returns a random selected food of the Util_Foodlist
    *
    * \param[out] food the selected food 
    * \return bool: true if if the a food could be determined, otherwise false
    */
    bool GetRandomFood(std::wstring * food)
    {
        bool result = false;

        if (nullptr != food)
        {
            if (false == Util_Foodlist.empty())
            {
                auto randNumber = rand() % Util_Foodlist.size();

                *food = Util_Foodlist[randNumber];
                result = true;
            }
        }
        return result;
    }


    /**
    * \brief ReloadConfig
    *
    * This function reads out the json configuration file
    *
    * \return bool: true if the configuration could be reloaded, otherwise false
    */
    bool ReloadConfig(void)
    {
        bool result = false;

        try
        {
            nlohmann::json config = ReadConfig();
            
            Util_Foodlist.clear();
            Util_Dinolist.clear();
            
            Util_Foodlist = DetermineFoodList(config);
            Util_Dinolist = DetermineDinoList(config);

            if (true == (bool)config["General"].value("ReplaceCuddleTypeWalk", true))
            {
                Util_IsCuddleTypeWalkActive = false;
            }
            else
            {
                Util_IsCuddleTypeWalkActive = true; 
            }

            result = true;
        }
        catch (const std::exception& error)
        {
            Log::GetLog()->error(error.what());
        }

        return result;
    }

}


/* =================================================[end of file]================================================= */
