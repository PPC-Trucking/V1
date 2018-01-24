// Forward the function to timer to check players every second to see if they're wanted
forward Police_CheckWantedPlayers(playerid);
forward UnjailPlayer(playerid);



// This timer is created every time a player changes his class to police
public Police_CheckWantedPlayers(playerid)
{
	// Scan through all players
	for (new PlayerToCheck; PlayerToCheck < MAX_PLAYERS; PlayerToCheck++)
	{
		// check if this player is connected
		if (IsPlayerConnected(PlayerToCheck))
		{
			//Check if that player is wanted
			if (GetPlayerWantedLevel(PlayerToCheck) > 0)
			    SetPlayerMarkerForPlayer(playerid, PlayerToCheck, COLOR_RED); // Make that player red to the police-player
			else
			{
				// Reset the playercolor based on the player's class
			    switch (APlayerData[PlayerToCheck][PlayerClass])
			    {
			        case ClassTruckDriver: SetPlayerMarkerForPlayer(playerid, PlayerToCheck, ColorClassTruckDriver);
			        case ClassBusDriver: SetPlayerMarkerForPlayer(playerid, PlayerToCheck, ColorClassBusDriver);
			        case ClassPilot: SetPlayerMarkerForPlayer(playerid, PlayerToCheck, ColorClassPilot);
			        case ClassPolice: SetPlayerMarkerForPlayer(playerid, PlayerToCheck, ColorClassPolice);
			        case ClassMafia: SetPlayerMarkerForPlayer(playerid, PlayerToCheck, ColorClassMafia);
			        case ClassCourier: SetPlayerMarkerForPlayer(playerid, PlayerToCheck, ColorClassCourier);
			        case ClassAssistance: SetPlayerMarkerForPlayer(playerid, PlayerToCheck, ColorClassAssistance);
			    }
			}
		}
	}
}

// This function gets called when a police player presses the SECUNDAIRY FIRE button (to warn nearby wanted players)
Police_WarnNearbyPlayers(playerid)
{
	// Setup local variables
	new Float:x, Float:y, Float:z, Name[24], Msg[128];

	// Scan through all players
	for (new PlayerToCheck; PlayerToCheck < MAX_PLAYERS; PlayerToCheck++)
	{
		// check if this player is connected
		if (IsPlayerConnected(PlayerToCheck))
		{
			// Check if the current player is wanted and NOT a police officer
			if (GetPlayerWantedLevel(PlayerToCheck) > 0 && PlayerToCheck != playerid)
			{
			    // Get the position of this player
			    GetPlayerPos(PlayerToCheck, x, y, z);
			    // Check if the police-player is in range of the player
				if (IsPlayerInRangeOfPoint(playerid, 50.0, x, y, z))
				{
					GameTextForPlayer(PlayerToCheck, TXT_PoliceWarnPlayer, 3000, 4); // Warn the player
					// Also start a timer which gives the player a chance to stop and get a fine
					// If he doesn't stop, the player will be sent to jail when he gets fined
					if (APlayerData[PlayerToCheck][PoliceWarnedMe] == false)
					{
					    APlayerData[PlayerToCheck][PoliceWarnedMe] = true;
						APlayerData[PlayerToCheck][Value_PoliceCanJailMe] = 60;
						APlayerData[PlayerToCheck][Timer_PoliceCanJailMe] = SetTimerEx("Timer_PoliceCanJailPlayer", 5000, true, "i", PlayerToCheck);
					}

					// Let the police player know that he warned the player
					GetPlayerName(PlayerToCheck, Name, sizeof(Name));
					format(Msg, sizeof(Msg), "{00FF00}You've warned {FFFF00}%s", Name);
					SendClientMessage(playerid, COLOR_WHITE, Msg);
				}
			}
		}
	}

	return 1;
}

// This function gets called when a police player presses the FIRE key (to fine nearby wanted players) when he's on foot
Police_FineNearbyPlayers(playerid)
{
	// Setup local variables
	new Float:x, Float:y, Float:z;

	// Scan through all players
	for (new PlayerToCheck; PlayerToCheck < MAX_PLAYERS; PlayerToCheck++)
	{
		// check if this player is connected
		if (IsPlayerConnected(PlayerToCheck))
		{
			// Check if the other player isn't the same police player
			if (PlayerToCheck != playerid)
			{
				// Check if the current player is wanted and the wanted player is driving slowly (below 30 kph)
				if ((GetPlayerWantedLevel(PlayerToCheck) > 0) && (APlayerData[PlayerToCheck][PlayerSpeed] < 30))
				{
				    // Get the position of this player
				    GetPlayerPos(PlayerToCheck, x, y, z);

				    // Check if the police-player is in range of the player (police player and wanted player must be within 10 meters of eachother)
					if (IsPlayerInRangeOfPoint(playerid, 10.0, x, y, z))
					{
					    // Fine the player
						Police_PayFine(playerid, PlayerToCheck);

						// Exit the function
						return 1;
					}

					// Check if the police-player is in range of the player (he can be inside his vehicle or on foot)
					if (IsPlayerInRangeOfPoint(playerid, 50.0, x, y, z))
					{
						GameTextForPlayer(PlayerToCheck, TXT_PoliceWarnPlayer, 3000, 4); // Warn the player
						// Also start a timer which gives the player a chance to stop and get a fine
						// If he doesn't stop, the player will be sent to jail when he gets fined
						if (APlayerData[PlayerToCheck][PoliceWarnedMe] == false)
						{
						    APlayerData[PlayerToCheck][PoliceWarnedMe] = true;
							APlayerData[PlayerToCheck][Value_PoliceCanJailMe] = DefaultWarnTimeBeforeJail;
							APlayerData[PlayerToCheck][Timer_PoliceCanJailMe] = SetTimerEx("Timer_PoliceCanJailPlayer", 5000, true, "i", PlayerToCheck);
						}
					}
				}
			}
		}
	}

	return 1;
}

// Pay the police player and fine the player
Police_PayFine(playerid, PlayerToFine)
{
	// Setup local variables
	new PoliceName[24], FinedPlayerName[24], PoliceMsg[200], PlayerMsg[200], JailTime, Fine;

	// Get the names of the police player and the wanted player
	GetPlayerName(playerid, PoliceName, 24);
	GetPlayerName(PlayerToFine, FinedPlayerName, 24);

	// Check if the wanted player's timer hasn't ran out yet
	if (APlayerData[PlayerToFine][PoliceCanJailMe] == false)
	{
		// Calculate the fine
		Fine = GetPlayerWantedLevel(PlayerToFine) * DefaultFinePerStar;
		// Reward the police player (give cash and points)
		RewardPlayer(playerid, Fine, GetPlayerWantedLevel(PlayerToFine));
		// Let the police player know that he fined the player
		format(PoliceMsg, sizeof(PoliceMsg), TXT_PoliceFinedPlayerReward, FinedPlayerName, Fine);
		SendClientMessage(playerid, COLOR_WHITE, PoliceMsg);
		// Let the wanted player pay the fine
		RewardPlayer(PlayerToFine, -Fine, 0);
		format(PlayerMsg, sizeof(PlayerMsg), TXT_PlayerGotFinedByPolice, PoliceName, Fine);
		SendClientMessage(PlayerToFine, COLOR_WHITE, PlayerMsg);
		// Let the other players know that the police player has fined the wanted player
		format(PoliceMsg, sizeof(PoliceMsg), TXT_PoliceFinedPlayer, PoliceName, FinedPlayerName);
		SendClientMessageToAll(COLOR_WHITE, PoliceMsg);
		// Increase the stats for fining a player
		APlayerData[playerid][StatsPoliceFined]++;
	}
	else // The wanted player didn't pull over, now the police player has the right to send him to jail and double the fine
	{
	    // Set jailtime
	    JailTime = DefaultJailTime;
		// Calculate the fine (double the normal fine)
		Fine = GetPlayerWantedLevel(PlayerToFine) * DefaultFinePerStar * 2;
		// Reward the police player (give cash and points)
		RewardPlayer(playerid, Fine, GetPlayerWantedLevel(PlayerToFine));
		// Let the police player know that he jailed the wanted player
		format(PoliceMsg, sizeof(PoliceMsg), TXT_PoliceJailedPlayerReward, FinedPlayerName, Fine);
		SendClientMessage(playerid, COLOR_WHITE, PoliceMsg);
		// Let the wanted player pay a double fine
		RewardPlayer(PlayerToFine, -Fine, 0);
		// Let the player know he's been jailed and for how long
		format(PlayerMsg, sizeof(PlayerMsg), TXT_PlayerGotJailedByPolice, PoliceName, (JailTime / 60));
		SendClientMessage(PlayerToFine, COLOR_WHITE, PlayerMsg);
		// Let the other players know that the police player has jailed the wanted player
		format(PoliceMsg, sizeof(PoliceMsg), TXT_PoliceJailedPlayer, PoliceName, FinedPlayerName, (JailTime / 60));
		SendClientMessageToAll(COLOR_WHITE, PoliceMsg);
		// Teleport the player to jail
		Police_JailPlayer(PlayerToFine, JailTime);
		// Increase the stats for jailing a player
		APlayerData[playerid][StatsPoliceJailed]++;
	}

	// Clear the wanted player's wanted status (the speedometer will automatically clear all data and kill the timer)
	SetPlayerWantedLevel(PlayerToFine, 0);

	// Also save the data (in case the server crashes, progress would be lost)
	PlayerFile_Save(playerid);
	PlayerFile_Save(PlayerToFine);

	return 1;
}

// This function ports the player inside the jail and sets a timer to get him back out
Police_JailPlayer(playerid, JailTime)
{
	// First remove the player from his vehicle
	RemovePlayerFromVehicle(playerid);
	// Set the player in the virtual world of the jail (so other players cannot see the jailed players on their radar)
	SetPlayerVirtualWorld(playerid, WORLD_JAIL);
	// Set player interior to the police station in San Fierro
	SetPlayerInterior(playerid, 10);
	// Put the player inside the jail
	SetPlayerPos(playerid, 220.0, 110.0, 999.1);

	// Store the jailtime for this player
	APlayerData[playerid][PlayerJailed] = JailTime;
	// Start the jailtimer, which checks every second if the player is allowed to get out
	KillTimer(APlayerData[playerid][PlayerJailedTimer]);
	APlayerData[playerid][PlayerJailedTimer] = SetTimerEx("UnjailPlayer", 1000, true, "i", playerid);

	// If the player started a job, let it fail
	if (APlayerData[playerid][JobStarted] == true)
	{
		// Stop any job that may have started
		switch (APlayerData[playerid][PlayerClass])
		{
			case ClassTruckDriver: Trucker_EndJob(playerid);
			case ClassBusDriver: BusDriver_EndJob(playerid);
			case ClassPilot: Pilot_EndJob(playerid);
			case ClassPolice: Police_EndJob(playerid);
			case ClassMafia: Mafia_EndJob(playerid);
			case ClassCourier: Courier_EndJob(playerid);
			case ClassAssistance: Assistance_EndJob(playerid);
		}

		// Inform the player that he failed the mission
		GameTextForPlayer(playerid, TXT_FailedMission, 5000, 4);
		// Reduce the player's cash by 1000
		RewardPlayer(playerid, -1000, 0);
	}

	return 1;
}

// This is the timer that runs for every player who's in jail
public UnjailPlayer(playerid)
{
	new JailMsg[20];

	// Check if the player is allowed to leave yet
	if (APlayerData[playerid][PlayerJailed] == 0)
	{
		// Set the player in the normal world
		SetPlayerVirtualWorld(playerid, 0);
		// Set player interior to the outside
		SetPlayerInterior(playerid, 0);
		// Put the player outside the jail (he should spawn at the location where he spawned after login or after choosing a rescue-point)
		SpawnPlayer(playerid);
		// Also, kill the jailtimer
		KillTimer(APlayerData[playerid][PlayerJailedTimer]);
	}
	else
	{
		// Show the remaining jailtime (only if the remaining time is below 60 seconds)
		if (APlayerData[playerid][PlayerJailed] < 60)
		{
			format(JailMsg, sizeof(JailMsg), TXT_JailTimer, APlayerData[playerid][PlayerJailed]);
			GameTextForPlayer(playerid, JailMsg, 750, 4);
		}
		// Decrease the jailtime by 1 second
        APlayerData[playerid][PlayerJailed] = APlayerData[playerid][PlayerJailed] - 1;
	}
}

// This function gets called when the police player dies (or changes class)
Police_EndJob(playerid)
{
	// Kill the PlayerCheckTimer
	KillTimer(APlayerData[playerid][PlayerCheckTimer]);

	// Scan through all players (to reset them to their default colors for the police-player)
	for (new PlayerToCheck; PlayerToCheck < MAX_PLAYERS; PlayerToCheck++)
	{
		// check if this player is connected
		if (IsPlayerConnected(PlayerToCheck))
		{
			// Reset the playercolor based on the player's class
		    switch (APlayerData[PlayerToCheck][PlayerClass])
		    {
		        case ClassTruckDriver: SetPlayerMarkerForPlayer(playerid, PlayerToCheck, ColorClassTruckDriver);
		        case ClassBusDriver: SetPlayerMarkerForPlayer(playerid, PlayerToCheck, ColorClassBusDriver);
		        case ClassPilot: SetPlayerMarkerForPlayer(playerid, PlayerToCheck, ColorClassPilot);
		        case ClassPolice: SetPlayerMarkerForPlayer(playerid, PlayerToCheck, ColorClassPolice);
		        case ClassMafia: SetPlayerMarkerForPlayer(playerid, PlayerToCheck, ColorClassMafia);
		        case ClassCourier: SetPlayerMarkerForPlayer(playerid, PlayerToCheck, ColorClassCourier);
		        case ClassAssistance: SetPlayerMarkerForPlayer(playerid, PlayerToCheck, ColorClassAssistance);
		    }
		}
	}

	return 1;
}

// This timer is started when a wanted player was warned by a police player
forward Timer_PoliceCanJailPlayer(playerid);
public Timer_PoliceCanJailPlayer(playerid)
{
	// Setup local variables
	new Msg[128];

	// Let the player know how much time he has left to pull over
	format(Msg, sizeof(Msg), "{FF0000}You have %i seconds to pull over and stop", APlayerData[playerid][Value_PoliceCanJailMe]);
	SendClientMessage(playerid, COLOR_WHITE, Msg);

	// Check if the timer has ran out
	if (APlayerData[playerid][Value_PoliceCanJailMe] == 0)
	{
	    // Set a switch that indicates that this player didn't stop when he got the warning from the police player
	    // When the police can catch him now, he'll be sent to jail and the fine is doubled
	    APlayerData[playerid][PoliceCanJailMe] = true;
		// Also kill the timer, as it's not needed anymore
		KillTimer(APlayerData[playerid][Timer_PoliceCanJailMe]);
		// Let the player know what consequences it will have by not stopping
		format(Msg, sizeof(Msg), "{FF0000}You didn't pull-over and stop, now the police can send you to jail immediately");
		SendClientMessage(playerid, COLOR_WHITE, Msg);
		format(Msg, sizeof(Msg), "{FF0000}Your fines will be doubled as well");
		SendClientMessage(playerid, COLOR_WHITE, Msg);
	}

	// Reduce the remaining time by 5 seconds
	APlayerData[playerid][Value_PoliceCanJailMe] = APlayerData[playerid][Value_PoliceCanJailMe] - 5;

	return 1;
}

// This function creates a spikestrip when the player is standing
SpikeStrip_Create(playerid)
{
	// Setup local variables
	new StripIndex = -1, Float:x, Float:y, Float:z, Float:rot;

	// Check if a spikestrip can be created
	for (new i; i < MAX_SPIKESTRIPS; i++)
	{
		if (ASpikeStrips[i][SpikeTime] == 0)
		{
			StripIndex = i;
			break;
		}
	}

	// Check if a free index has been found
	if (StripIndex != -1)
	{
		// Get the position of the player
		GetPlayerPos(playerid, x, y, z);
		GetPlayerFacingAngle(playerid, rot);

		// Create a new spike-strip object at the location of the player and store the coordinates of the spikestrip
		ASpikeStrips[StripIndex][SpikeObject] = CreateObject(2892, x, y, z - 1.0, 0.0, 0.0, rot + 90.0);
		ASpikeStrips[StripIndex][SpikeX] = x;
		ASpikeStrips[StripIndex][SpikeY] = y;
		ASpikeStrips[StripIndex][SpikeZ] = z;
		// Set the time for this timer to 1 minute
		ASpikeStrips[StripIndex][SpikeTime] = 600;
		// Create a timer that checks all players in range to see if they run over the spikestrip
		ASpikeStrips[StripIndex][SpikeTimer] = SetTimerEx("CheckSpikeStrip", 100, true, "i", StripIndex);
		// Let the player know he created a spikestrip
		SendClientMessage(playerid, COLOR_WHITE, "{00FF00}You've placed a spikestrip");
	}
	else
	    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}The maximum number of spikestrips have been created, try again later");
}

// This timer checks the spikestrips that have been planted (they automatically disappear after one minute)
forward CheckSpikeStrip(StripIndex);
public CheckSpikeStrip(StripIndex)
{
	// Setup local variables
	new vid, panels, doors, lights, tires;

	// Decrease the time left for the spikestrip
	ASpikeStrips[StripIndex][SpikeTime]--;

	// Check if the spikestrip is still allowed to exist
	if (ASpikeStrips[StripIndex][SpikeTime] > 0)
	{
		// Loop through all players
		for (new playerid; playerid < MAX_PLAYERS; playerid++)
		{
			// Check if the player is connected
			if (APlayerData[playerid][LoggedIn] == true)
			{
				// Check if the player is the driver of a vehicle
				if (GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
				{
					// Get the vehicleid
					vid = GetPlayerVehicleID(playerid);
					// Check if the player is near the spikestrip object
					if (IsPlayerInRangeOfPoint(playerid, 7.0, ASpikeStrips[StripIndex][SpikeX], ASpikeStrips[StripIndex][SpikeY], ASpikeStrips[StripIndex][SpikeZ]))
					{
						// Pop all the tires of the player's vehicle
						GetVehicleDamageStatus(vid, panels, doors, lights, tires);
						UpdateVehicleDamageStatus(vid, panels, doors, lights, 15);
					}
				}
			}
		}
	}
	else
	{
		// Kill the timer and destroy the spikestrip object
		DestroyObject(ASpikeStrips[StripIndex][SpikeObject]);
		KillTimer(ASpikeStrips[StripIndex][SpikeTimer]);
	}

	return 1;
}



// This function sends the given message to all police players
Police_SendMessage(PoliceMessage[])
{
	for (new PoliceID; PoliceID < MAX_PLAYERS; PoliceID++) // Loop through all players
		if (APlayerData[PoliceID][LoggedIn] == true) // Check if this player has logged in
		    if (APlayerData[PoliceID][PlayerClass] == ClassPolice) // Check if this player is a police player
				SendClientMessage(PoliceID, COLOR_WHITE, PoliceMessage); // Send the message to the police player
}

