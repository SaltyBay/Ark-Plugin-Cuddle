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
* \file Hooks.cpp
* \author Matthias Birnthaler 
* \date 19 March 2019
* \brief File containing the implementation for all needed Hooks  
*
*/

#include "Hooks.h"
#include "Util.h"
#include <math.h>



/**
* \brief Initialisation of needed Hooks 
*
* This function initialise all needed Hooks and read the relevant information from the json config file.
*
* \return void
*/
void InitHooks(void)
{
   ArkApi::GetHooks().SetHook("APrimalDinoCharacter.UpdateBabyCuddling", 
       &Hook_APrimalDinoCharacter_UpdateBabyCuddling, 
       &APrimalDinoCharacter_UpdateBabyCuddling_original);

   ArkApi::GetHooks().SetHook("APrimalDinoCharacter.UpdateImprintingQuality", 
       &Hook_APrimalDinoCharacter_UpdateImprintingQuality, 
       &APrimalDinoCharacter_UpdateImprintingQuality_original);

   ArkApi::GetHooks().SetHook("APrimalDinoCharacter.AddedImprintingQuality",
       &Hook_APrimalDinoCharacter_AddedImprintingQuality,
       &APrimalDinoCharacter_AddedImprintingQuality_original);


}


/**
* \brief Cancellation of needed Hooks
*
* This function removes all needed Hooks.
*
* \return void
*/
void RemoveHooks(void)
{
	ArkApi::GetHooks().DisableHook("APrimalDinoCharacter.UpdateBabyCuddling", 
        &Hook_APrimalDinoCharacter_UpdateBabyCuddling);

    ArkApi::GetHooks().DisableHook("APrimalDinoCharacter.UpdateImprintingQuality", 
        &Hook_APrimalDinoCharacter_UpdateImprintingQuality);

    ArkApi::GetHooks().DisableHook("APrimalDinoCharacter.AddedImprintingQuality", 
        &Hook_APrimalDinoCharacter_AddedImprintingQuality);
}


/**
* \brief Hook of APrimalDinoCharacter_UpdateBabyCuddling
*
* This function validates if the \p NewBabyCuddleType is walking or food and calls the original UpdateBabyCuddling function with modified parameters.
* The parameter \p NewBabyCuddleType will modify to food and the \p NewBabyCuddleFood will be filled by one random selected food of the FoodList.
* In case the \p NewBabyCuddleType is not walking this function calls the original UpdateBabyCuddling function without any modified parameters.
* In case of an error the original UpdateBabyCuddling function without any modified parameters.
*
* \param[in] _this the dino that gets updated  
* \param[in] NewBabyNextCuddleTime the cuddle time (server time in seconds)
* \param[in] NewBabyCuddleType the cuddletype (cuddle/food/walking) that the dinos requests 
* \param[in] NewBabyCuddleFood the food that the dino requests (if cuddletype is food) 
* \warning global variable used 
* \return void
*/
void  Hook_APrimalDinoCharacter_UpdateBabyCuddling(APrimalDinoCharacter* _this, long double NewBabyNextCuddleTime, char NewBabyCuddleType, TSubclassOf<UPrimalItem> NewBabyCuddleFood)
{
    Util::CuddleData_t cuddledata;
    long double babyNextCuddleTime = NewBabyNextCuddleTime;
    TSubclassOf<UPrimalItem> foodItem = NewBabyCuddleFood;
    char cuddleType = NewBabyCuddleType;


    if (nullptr != _this)
    {
        FString dinoNameTag;
        _this->DinoNameTagField().ToString(&dinoNameTag);;


        if (true == Util::GetDinoCuddleData(dinoNameTag.ToString(), &cuddledata))
        {
            auto  CurrentServerTime = ArkApi::GetApiUtils().GetWorld()->TimeSecondsField();

            if ((nullptr != _this->MyCharacterStatusComponentField()))
            {
                if (true == (Util::CompFloat(_this->MyCharacterStatusComponentField()->DinoImprintingQualityField(), 1.0f)))
                {
                    babyNextCuddleTime = CurrentServerTime + _this->BabyImprintingQualityTotalMaturationTimeField();
                }
                else
                {
                    babyNextCuddleTime = CurrentServerTime + cuddledata.CuddleIntervalTime * 60;
                }
            }


            if ((EBabyCuddleType::FOOD == NewBabyCuddleType) || (EBabyCuddleType::WALK == NewBabyCuddleType))
            {
                std::wstring foodblueprint;

                if (true == Util::GetRandomFood(&foodblueprint))
                {
                    UObject* object = Globals::StaticLoadObject(UObject::StaticClass(), nullptr, foodblueprint.c_str(), nullptr, 0, 0, true);
                
                    if (nullptr != object)
                    {
                        foodItem.uClass = reinterpret_cast<UClass*>(object);
                        cuddleType = EBabyCuddleType::FOOD;
                    }
                    else
                    {
                        Log::GetLog()->error("can not create object blueprint {}", FString(foodblueprint).ToString());
                    }
                }
            }
        }
    }

    APrimalDinoCharacter_UpdateBabyCuddling_original(_this, babyNextCuddleTime, cuddleType, foodItem);
}


void  Hook_APrimalDinoCharacter_UpdateImprintingQuality(APrimalDinoCharacter* _this, float NewImprintingQuality)
{
    Util::CuddleData_t cuddledata;
    float imprintingQuality = NewImprintingQuality;

    if ((nullptr != _this) && (nullptr != _this->MyCharacterStatusComponentField()))
    {
        FString dinoNameTag;
        _this->DinoNameTagField().ToString(&dinoNameTag);;


        if (true == Util::GetDinoCuddleData(dinoNameTag.ToString(), &cuddledata))
        {
            if (_this->MyCharacterStatusComponentField()->DinoImprintingQualityField() + cuddledata.CuddleIncreasePercent < 1.0f)
            {
                imprintingQuality = _this->MyCharacterStatusComponentField()->DinoImprintingQualityField() + cuddledata.CuddleIncreasePercent;
            }
            else
            {
                imprintingQuality = 1.0f;
            }

        }
    }

    APrimalDinoCharacter_UpdateImprintingQuality_original(_this, imprintingQuality);
}



void  Hook_APrimalDinoCharacter_AddedImprintingQuality(APrimalDinoCharacter* _this, float Amount)
{
    Util::CuddleData_t cuddledata;
    float amount = Amount;

    if ((nullptr != _this) && (nullptr != _this->MyCharacterStatusComponentField()))
    {
        FString dinoNameTag;
        _this->DinoNameTagField().ToString(&dinoNameTag);;


        if (true == Util::GetDinoCuddleData(dinoNameTag.ToString(), &cuddledata))
        {
            if (_this->MyCharacterStatusComponentField()->DinoImprintingQualityField() < 1.0f)
            {
                if ((1.0f - _this->MyCharacterStatusComponentField()->DinoImprintingQualityField()) >= cuddledata.CuddleIncreasePercent)
                {
                    amount = cuddledata.CuddleIncreasePercent;
                }
                else
                {
                    amount = 1.0f - _this->MyCharacterStatusComponentField()->DinoImprintingQualityField();
                }
            }
            else
            {
                amount = 0.0f;
            }
        }
    }

    APrimalDinoCharacter_AddedImprintingQuality_original(_this, amount);
}
