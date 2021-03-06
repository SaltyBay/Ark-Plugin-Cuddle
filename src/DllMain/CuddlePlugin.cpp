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
* \file CuddlePlugin.cpp
* \author Matthias Birnthaler Matthias-Birnthaler@outlook.com
* \date 26 July 2020
* \brief Implementation of DLLMain
*/


/* ================================================ [includes] ================================================ */
#include "CuddlePlugin.h"
#include "Hooks.h"
#include "Util.h"
#include "Commands.h"


/* ============================================ [type definitions] ============================================ */

/* =============================================== [local data] =============================================== */

/* ===================================== [prototype of local functions] ======================================= */
static void Load(void);
static void Unload(void);

/**
* \brief Load of OverwriteCuddleWalking Plugin
*
* This function loads the Plugin and calls the initialisation
*
* \return void
*/
static void Load(void)
{
	Log::Get().Init("CuddlePlugin");

	InitHooks();

    Util::ReloadConfig();

    Commands::InitCommands();
}


/**
* \brief Unload of OverwriteCuddleWalking Plugin
*
* This function unloads the Plugin 
*
* \return void
*/
static void Unload(void)
{
	RemoveHooks();
}


/* ===================================== [definition of global functions] ===================================== */


/**
* \brief DLLMain of the OverwriteCuddleWalking Plugin
*
* \param[in] hModule
* \param[in] ul_reason_for_call
* \param[in] lpReserved
* \return BOOL APIENTRY
*/
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		Load();
		break;
	case DLL_PROCESS_DETACH:
		Unload();
		break;
	}
	return TRUE;
}

/* =================================================[end of file]================================================= */
