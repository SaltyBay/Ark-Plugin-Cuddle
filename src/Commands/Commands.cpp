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
* \file Commands.cpp
* \author Matthias Birnthaler Matthias-Birnthaler@outlook.com
* \date 26 July 2020
* \brief Implementation of all Commands 
*
*/

/* ================================================ [includes] ================================================ */

#include "Commands.h"
#include <sstream>
#include <string>
#include "Util.h"




namespace Commands
{
    /* ============================================ [type definitions] ============================================ */

    /* =============================================== [local data] =============================================== */

    /* ===================================== [prototype of local functions] ======================================= */

    static void InitRconCommands(void);
    static void RemoveRconCommands(void);
    static void InitConsoleCommands(void);
    static void RemoveConsoleCommands(void);
    static void InitChatCommands(void);
    static void RemoveChatCommands(void);

    /* ===================================== [definition of local functions] ====================================== */


    /**
    * \brief Console command: ConsoleReloadConfig
    *
    * This command is to reload the json config
    *
    * \param[in] player caller of the command
    * \param[in] cmd the commands data
    * \param[in] boolean, variable not used
    * \return void
    */
    static void ConsoleReloadConfig(APlayerController* player, FString* cmd, bool boolean)
    {
        if ((nullptr != player) && (nullptr != cmd))
        {
            if (true == player->bIsAdmin().Get())
            {
                const auto shooter_controller = static_cast<AShooterPlayerController*>(player);
                if (true == Util::ReloadConfig())
                {
                    ArkApi::GetApiUtils().SendServerMessage(shooter_controller, FColorList::White, "Successfully reloaded!");
                }
                else
                {
                    ArkApi::GetApiUtils().SendServerMessage(shooter_controller, FColorList::White, "Couldn't reloaded!");
                }
            }
        }
    }


    /**
    * \brief Rcon command: RconReloadConfig
    *
    * This command is to reload the json config
    *
    * \param[in] rcon_connection caller of the command
    * \param[in] rcon_packet rcon data
    * \param[in] UWorld, variable not used
    * \return void
    */
    static void RconReloadConfig(RCONClientConnection* rcon_connection, RCONPacket* rcon_packet, UWorld*)
    {
        if ((nullptr != rcon_connection) && (nullptr != rcon_packet))
        {
            if (true == Util::ReloadConfig())
            {
                FString reply = "Successfully reloaded!\n";
                rcon_connection->SendMessageW(rcon_packet->Id, 0, &reply);
            }
            else
            {
                FString reply = "Couldn't reloaded!\n";
                rcon_connection->SendMessageW(rcon_packet->Id, 0, &reply);
            }
        }
    }



    /**
    * \brief Initialisation of rcon commands
    *
    * This function initializes all rcon commands
    *
    * \return void
    */
    static void InitRconCommands(void)
    {
        ArkApi::GetCommands().AddRconCommand("CuddlePlugin.reload", &RconReloadConfig);
    }


    /**
    * \brief Remove of rcon commands
    *
    * This function removes all rcon commands
    *
    * \return void
    */
    static void RemoveRconCommands(void)
    {
        ArkApi::GetCommands().RemoveRconCommand("CuddlePlugin.reload");
    }


    /**
    * \brief Initialisation of console commands
    *
    * This function initializes all console commands
    *
    * \return void
    */
    static void InitConsoleCommands(void)
    {
        ArkApi::GetCommands().AddConsoleCommand("CuddlePlugin.reload", &ConsoleReloadConfig);
    }


    /**
    * \brief Remove of console commands
    *
    * This function removes all console commands
    *
    * \return void
    */
    static void RemoveConsoleCommands(void)
    {
        ArkApi::GetCommands().RemoveConsoleCommand("CuddlePlugin.reload");
    }


    /**
    * \brief Initialisation of chat commands
    *
    * This function initializes all chat commands
    *
    * \return void
    */
    static void InitChatCommands(void)
    {

    }


    /**
    * \brief Remove of chat commands
    *
    * This function removes all chat commands
    *
    * \return void
    */
    static void RemoveChatCommands(void)
    {

    }


    /* ===================================== [definition of global functions] ===================================== */

    /**
    * \brief Initialisation of commands
    *
    * This function initializes all commands
    *
    * \return void
    */
    void InitCommands(void)
    {
        InitChatCommands();
        InitConsoleCommands();
        InitRconCommands();
    }


    /**
    * \brief Remove of commands
    *
    * This function removes all commands
    *
    * \return void
    */
    void RemoveComands(void)
    {
        RemoveChatCommands();
        RemoveConsoleCommands();
        RemoveRconCommands();
    }
}

/* =================================================[end of file]================================================= */
