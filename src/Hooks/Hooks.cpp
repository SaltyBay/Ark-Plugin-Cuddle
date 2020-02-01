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


#define CUDDLE_TYPE_CUDDLE		0  /**< \brief value for dino imprint type cuddle */
#define CUDDLE_TYPE_FOOD		1  /**< \brief value for dino imprint type food */
#define CUDDLE_TYPE_WALKING		2  /**< \brief value for dino imprint type walking */


/**
* \var FoodList
* \brief list given food given by json config
*/
static std::vector<std::wstring> FoodList;  


/**
* \brief Initialisation of needed Hooks 
*
* This function initialise all needed Hooks and read the relevant information from the json config file.
*
* \return void
*/
void InitHooks(void)
{
	ArkApi::GetHooks().SetHook("APrimalDinoCharacter.UpdateBabyCuddling", &Hook_APrimalDinoCharacter_UpdateBabyCuddling, &APrimalDinoCharacter_UpdateBabyCuddling_original);

	FoodList = GetFoodList();
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
	ArkApi::GetHooks().DisableHook("APrimalDinoCharacter.UpdateBabyCuddling", &Hook_APrimalDinoCharacter_UpdateBabyCuddling);
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

	if ((CUDDLE_TYPE_WALKING == NewBabyCuddleType || 
		(CUDDLE_TYPE_FOOD == NewBabyCuddleType)))
	{
		if (FALSE == FoodList.empty())
		{

			int RandNumber = rand() % FoodList.size();

			const wchar_t* foodblueprint = FoodList[RandNumber].c_str();

			UObject* object = Globals::StaticLoadObject(UObject::GetPrivateStaticClass(), nullptr, foodblueprint, nullptr, 0, 0, true);

			if (nullptr != object)
			{
				TSubclassOf<UPrimalItem> item;
				item.uClass = reinterpret_cast<UClass*>(object);

				APrimalDinoCharacter_UpdateBabyCuddling_original(_this, NewBabyNextCuddleTime, CUDDLE_TYPE_FOOD, item);
			}
			else /* food object can not be created; call of original UpdateBabyCuddling function without any modified parameters */
			{
				APrimalDinoCharacter_UpdateBabyCuddling_original(_this, NewBabyNextCuddleTime, NewBabyCuddleType, NewBabyCuddleFood);
				Log::GetLog()->error("can not create object blueprint {}", FString(foodblueprint).ToString());
			}
		}
		else /* food object list is empty; call of original UpdateBabyCuddling function without any modified parameters */
		{
			APrimalDinoCharacter_UpdateBabyCuddling_original(_this, NewBabyNextCuddleTime, NewBabyCuddleType, NewBabyCuddleFood);
			Log::GetLog()->error("no foodlist given, check json config ");
		}
		
	}
	else
	{
		APrimalDinoCharacter_UpdateBabyCuddling_original(_this, NewBabyNextCuddleTime, NewBabyCuddleType, NewBabyCuddleFood);
	}
}
