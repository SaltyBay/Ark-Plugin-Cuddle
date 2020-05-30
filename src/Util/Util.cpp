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
* \file Util.cpp
* \author Matthias Birnthaler
* \date 19 March 2019
* \brief Implementation of utility functions
*
*/



#include "Util.h"

#include "Tools.h"
#include "Requests.h"
#include "ICommands.h"
#include "IHooks.h"
#include "API/ARK/Ark.h"

#include <fstream>
#include "json.hpp"




namespace Util {


    static std::vector<std::wstring> Util_Foodlist;
    static std::map<std::string, CuddleData_t> Util_Dinolist;


    static nlohmann::json ReadConfig();
    static std::vector<std::string> split(std::string stringToBeSplitted, std::string delimeter);

    /**
    * \brief ReadConfig
    *
    * This function reads out the json config file
    *
    * \return  nlohmann::json
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
    * \brief GetFoodList
    *
    * This function determines the FoodList from the json config
    *
    * \return std::vector<std::wstring>
    */
    std::vector<std::wstring> GetFoodList(nlohmann::json config)
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


    bool CompFloat(float A, float B, float epsilon)
    {
        return (fabs(A - B) < epsilon);
    }



    std::map<std::string, CuddleData_t> GetDinoList(nlohmann::json config)
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



    extern bool GetDinoCuddleData(std::string DinoTag, CuddleData_t * CuddleData)
    {
        bool result = false;

        if (nullptr != CuddleData)
        {
            auto it = Util_Dinolist.find(DinoTag);


            if (Util_Dinolist.end() != it)
            {
                (*CuddleData).CuddleIncreasePercent = it->second.CuddleIncreasePercent;
                (*CuddleData).CuddleIntervalTime = it->second.CuddleIntervalTime;

                result = true;
            }

        }

        return result;
    }


    std::vector<std::wstring> GetFoodlist()
    {
        return Util_Foodlist;
    }

    
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


    bool ReloadConfig(void)
    {
        bool result = false;

        try
        {
            nlohmann::json config = ReadConfig();
            
            Util_Foodlist.clear();
            Util_Dinolist.clear();
            
            Util_Foodlist = GetFoodList(config);
            Util_Dinolist = GetDinoList(config);

            result = true;
        }
        catch (const std::exception& error)
        {
            Log::GetLog()->error(error.what());
        }

        return result;
    }



    void Util_Init(void)
    {
        try
        {
            nlohmann::json config = ReadConfig();

            Util_Foodlist = GetFoodList(config);
            Util_Dinolist = GetDinoList(config);
        }
        catch (const std::exception& error)
        {
            Log::GetLog()->error(error.what());
        }
    }
}
