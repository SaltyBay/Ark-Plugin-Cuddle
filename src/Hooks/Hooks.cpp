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
* \brief Implementation of all hooks
*
*/


/* ================================================[includes]================================================ */
#include "Hooks.h"
#include "Util.h"
#include <math.h>


/* ============================================ [type definitions] ============================================ */

/* =============================================== [local data] =============================================== */

/* ===================================== [prototype of local functions] ======================================= */
static void  Hook_APrimalDinoCharacter_UpdateBabyCuddling(APrimalDinoCharacter* _this, long double NewBabyNextCuddleTime, char NewBabyCuddleType, TSubclassOf<UPrimalItem> NewBabyCuddleFood);
static void  Hook_APrimalDinoCharacter_UpdateImprintingQuality(APrimalDinoCharacter* _this, float NewImprintingQuality);
static void  Hook_APrimalDinoCharacter_AddedImprintingQuality(APrimalDinoCharacter* _this, float Amount);

/**
* \brief Hook of APrimalDinoCharacter_UpdateBabyCuddling
*
* This function modifies the function parameters NewBabyNextCuddleTime,  NewBabyCuddleType and NewBabyCuddleFood depending on the dinosaur and the configuration
*
* \param[in] _this the dino that gets updated
* \param[in] NewBabyNextCuddleTime the cuddle time (server time in seconds)
* \param[in] NewBabyCuddleType the cuddletype (cuddle/food/walking) that the dinos requests
* \param[in] NewBabyCuddleFood the food that the dino requests (if cuddletype is food)
*
* \return void
*/
static void  Hook_APrimalDinoCharacter_UpdateBabyCuddling(APrimalDinoCharacter* _this, long double NewBabyNextCuddleTime, char NewBabyCuddleType, TSubclassOf<UPrimalItem> NewBabyCuddleFood)
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
                    babyNextCuddleTime = CurrentServerTime + cuddledata.CuddleIntervalTime;
                }
            }


            if ((EBabyCuddleType::FOOD == NewBabyCuddleType) ||
                ((EBabyCuddleType::WALK == NewBabyCuddleType) && (false == Util::IsCuddleTypeWalkActive())))
            {
                std::wstring foodblueprint;

                /* overwride to EBabyCuddleType::PET as fallback. in case that EBabyCuddleType::WALK should be overwritten but no foodlist is configured */
                if (EBabyCuddleType::WALK == NewBabyCuddleType)
                {
                    cuddleType = EBabyCuddleType::PET;
                }

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


/**
* \brief Hook of Hook_APrimalDinoCharacter_UpdateImprintingQuality
*
* This function modifies the function parameters NewImprintingQuality depending on the dinosaur and the configuration
*
* \param[in] _this the dino that gets updated
* \param[in] NewImprintingQuality the new imprint quality (server time in seconds)
*
* \return void
*/
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


/**
* \brief Hook of Hook_APrimalDinoCharacter_AddedImprintingQuality
*
* This function recalculates the added increase per cuddle
*
* \param[in] _this the dino that gets updated
* \param[in] Amount the cuddle increase
*
* \return void
*/
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


/* ===================================== [definition of global functions] ===================================== */


/**
* \brief Initialisation of needed Hooks
*
* This function initialises all needed Hooks.
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

/* =================================================[end of file]================================================= */
