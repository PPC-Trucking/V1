// Forward the function to timer to check players every second to see if they're carrying a mafia-load
forward Mafia_CheckMafiaLoads(playerid);
// Forward the public function used as a timer to load/unload your vehicle
forward Mafia_LoadUnload(playerid);



// This function is called when a mafia wants to start a job by entering "/work"
Mafia_StartRandomJob(playerid)
{
	if (APlayerData[playerid][JobStarted] == false)
	{
	    // Setup local variables
		new StartLoc[50], EndLoc[50], Load[50], RouteText[255], Float:x, Float:y, Float:z, LoadMsg[128];

		// Job has started
		APlayerData[playerid][JobStarted] = true;

		// Get a random LoadID from the mafia-products
		APlayerData[playerid][LoadID] = Product_GetRandom(PCV_MafiaVan);
		// Also get a random start-location and end-location
		APlayerData[playerid][JobLoc1] = Product_GetRandomStartLoc(APlayerData[playerid][LoadID]);
		APlayerData[playerid][JobLoc2] = Product_GetRandomEndLoc(APlayerData[playerid][LoadID]);

		// Set jobstep to 1 (going to load the goods)
		APlayerData[playerid][JobStep] = 1;
		// Get the startlocation, endlocation and the load texts
		format(StartLoc, sizeof(StartLoc), ALocations[APlayerData[playerid][JobLoc1]][LocationName]);
		format(EndLoc, sizeof(EndLoc), ALocations[APlayerData[playerid][JobLoc2]][LocationName]);
		format(Load, sizeof(Load), ALoads[APlayerData[playerid][LoadID]][LoadName]);
		// Combine it all into a string for the TextDraw (the player can see this all the time) to describe the mission
		format(RouteText, sizeof(RouteText), TXT_HaulingCargoFromToPickup, Load, StartLoc, EndLoc);
		// Set the TextDraw so the player can see it
		TextDrawSetString(APlayerData[playerid][MissionText], RouteText);
		// Grab the x, y, z positions for the first location
		x = ALocations[APlayerData[playerid][JobLoc1]][LocX];
		y = ALocations[APlayerData[playerid][JobLoc1]][LocY];
		z = ALocations[APlayerData[playerid][JobLoc1]][LocZ];
		// Create a checkpoint where the player should load the goods
		SetPlayerCheckpoint(playerid, x, y, z, 7);
		// Store the vehicleID (required to be able to check if the player left his vehicle)
		APlayerData[playerid][VehicleID] = GetPlayerVehicleID(playerid);
		// Set the job-fail-time for the global vehicle-timer
		APlayerData[playerid][VehicleTimerTime] = Job_TimeToFailMission;
		// Inform the player that he must load his goods
		format(LoadMsg, sizeof(LoadMsg), TXT_PickupCargoAt, Load, StartLoc);
		SendClientMessage(playerid, COLOR_WHITE, LoadMsg);
	}

	return 1;
}



// This timer is created every time a player changes his class to mafia and is called every second
public Mafia_CheckMafiaLoads(playerid)
{
	// Scan through all players
	for (new PlayerToCheck; PlayerToCheck < MAX_PLAYERS; PlayerToCheck++)
	{
		// check if this player is connected
		if (IsPlayerConnected(PlayerToCheck))
		{
			//Check if that player is carrying a mafia-load
			if (APlayerData[PlayerToCheck][MafiaLoad] == true)
			    SetPlayerMarkerForPlayer(playerid, PlayerToCheck, COLOR_RED); // Make that player red to the mafia-player
			else
			    if (APlayerData[PlayerToCheck][PlayerClass] == ClassTruckDriver) // Reset the playercolor for truckers (only truckers can carry mafia-loads)
			        SetPlayerMarkerForPlayer(playerid, PlayerToCheck, ColorClassTruckDriver);
		}
	}

	// If the mafia-player hasn't started a job, check if he has stolen a mafiaload from a trucker
	if (APlayerData[playerid][JobStarted] == false)
	{
		new vehicle = GetPlayerVehicleID(playerid);
		new trailer = GetVehicleTrailer(vehicle);

		// Check if the mafia-player has hijacked a mafia-load already
		if (APlayerData[playerid][MafiaLoadHijacked] == false)
		{
			// If the mafia-player hasn't hijacked a mafia-load yet, check if he did now (check vehicle and trailer)
			if ((AVehicleData[vehicle][MafiaLoad] == true) || (AVehicleData[trailer][MafiaLoad] == true))
			{
				// Store the vehicleid and trailerid to be able to check if the mafia-player lost his load afterwards
				APlayerData[playerid][VehicleID] = vehicle;
				APlayerData[playerid][TrailerID] = trailer;
			    // Now the mafia-player has hijacked a mafia-load
			    APlayerData[playerid][MafiaLoadHijacked] = true;
			    // Set the checkpoint where the mafia-player must bring the load
			    SetPlayerCheckpoint(playerid, 2867, 939, 10.8, 7.0);
				// Update the missiontext
				TextDrawSetString(APlayerData[playerid][MissionText], TXT_MafiaDeliverStolenLoad);
			}
		}

		// Check if the mafia-player already hijacked a mafia-load
		if (APlayerData[playerid][MafiaLoadHijacked] == true)
		{
			// If the mafia-player has hijacked a mafia-load, check if he still has the load (check vehicle and trailer)
			if ((APlayerData[playerid][VehicleID] == vehicle) && (APlayerData[playerid][TrailerID] == trailer))
				// Do nothing if vehicle and trailer are the same
				return 1;
			else
			{
				// Clear the vehicleid and trailerid
				APlayerData[playerid][VehicleID] = 0;
				APlayerData[playerid][TrailerID] = 0;
			    // Now the mafia-player lost his stolen load
			    APlayerData[playerid][MafiaLoadHijacked] = false;
			    // Disable the checkpoint where he must bring his load
				DisablePlayerCheckpoint(playerid);
				// Reset the missiontext
				TextDrawSetString(APlayerData[playerid][MissionText], Mafia_NoJobText);
			}
		}
	}

	return 1;
}



// This function is called whenever a mafia player enters a checkpoint
Mafia_OnPlayerEnterCheckpoint(playerid)
{
	// First check if the mafia player started a job or not
	if (APlayerData[playerid][JobStarted] == false) // Mafia player delivered a mafia-load
	{
	    // This code handles stolen mafia-loads
		new vehicle = GetPlayerVehicleID(playerid);
		new trailer = GetVehicleTrailer(vehicle);

		// If the mafia player has a trailer, so detach and respawn the trailer
		if (trailer > 0)
		{
			DetachTrailerFromVehicle(vehicle);
			SetVehicleToRespawn(trailer);
		}
		else // The mafia player has no trailer, so the load is inside the truck -> remove player from vehicle and respawn the vehicle
		{
		    RemovePlayerFromVehicle(playerid);
			SetVehicleToRespawn(vehicle);
		}

		// Reward the mafia player (give cash and points)
		RewardPlayer(playerid, 5000, 2);
		// Let the player know he succesfully delivered a stolen load to the mafia hideout
		SendClientMessage(playerid, COLOR_WHITE, TXT_MafiaDeliveredStolenLoad);

		// Increase the stats for completing a delivery of a stolen mafiaload
		APlayerData[playerid][StatsMafiaStolen]++;
		// Also save the data (in case the server crashes, progress would be lost)
		PlayerFile_Save(playerid);

		// Cleanup
		APlayerData[playerid][VehicleID] = 0;
		APlayerData[playerid][TrailerID] = 0;
	    // Disable the checkpoint where he must bring his load
		DisablePlayerCheckpoint(playerid);
		// Reset the missiontext
		TextDrawSetString(APlayerData[playerid][MissionText], Mafia_NoJobText);
	}
	else // The mafia-player is doing a job
	{
		// Check if the player is inside his vehicle while entering a checkpoint
		if (GetPlayerVehicleID(playerid) == APlayerData[playerid][VehicleID])
		{
			new LoadMsg[128];

		    // Check the jobstep
		    switch (APlayerData[playerid][JobStep])
		    {
				// JobStep is 1 (mafia is loading his goods at the checkpoint)
		 		case 1: format(LoadMsg, sizeof(LoadMsg), TXT_LoadingGoods, ALoads[APlayerData[playerid][LoadID]][LoadName]);
				// JobStep is 2 (mafia is unloading his goods at the checkpoint)
				case 2: format(LoadMsg, sizeof(LoadMsg), TXT_UnloadingGoods, ALoads[APlayerData[playerid][LoadID]][LoadName]);
		    }

			// Disable the player's actions (he cannot move anymore)
			TogglePlayerControllable(playerid, 0);

			// Show the message to inform him what he's doing (loading/unloading)
			GameTextForPlayer(playerid, LoadMsg, 5000, 5);
			// Start a timer (Public function "LoadUnload(playerid)" gets called when the timer runs out)
			APlayerData[playerid][LoadingTimer] = SetTimerEx("Mafia_LoadUnload", 5000, false, "d" , playerid);
		}
	}

	return 1;
}



// After a mafia entered a checkpoint, a timer is created. This function is called when the timer runs out
public Mafia_LoadUnload(playerid)
{
	// Setup local variables
	new Name[24], Msg[128];

	// Get the player name
	GetPlayerName(playerid, Name, sizeof(Name));

	// Check the JobStep
	switch (APlayerData[playerid][JobStep])
	{
		case 1: // Player must load his goods
		{
		    // Setup local variables
			new StartLoc[50], EndLoc[50], Load[50], RouteText[255], Float:x, Float:y, Float:z, UnloadMsg[100];
			// Set JobStep to 2 (unloading goods)
			APlayerData[playerid][JobStep] = 2;
			// Delete the loading-checkpoint
			DisablePlayerCheckpoint(playerid);
			// Get the startlocation, endlocation and the load texts
			format(StartLoc, sizeof(StartLoc), ALocations[APlayerData[playerid][JobLoc1]][LocationName]);
			format(EndLoc, sizeof(EndLoc), ALocations[APlayerData[playerid][JobLoc2]][LocationName]);
			format(Load, sizeof(Load), ALoads[APlayerData[playerid][LoadID]][LoadName]);

			// Update the missiontext
			format(RouteText, sizeof(RouteText), TXT_HaulingCargoFromToDeliver, Load, StartLoc, EndLoc);
			// Set the TextDraw so the player can see it
			TextDrawSetString(APlayerData[playerid][MissionText], RouteText);

			// Grab the x, y, z positions for the second location (to unload the goods)
			x = ALocations[APlayerData[playerid][JobLoc2]][LocX];
			y = ALocations[APlayerData[playerid][JobLoc2]][LocY];
			z = ALocations[APlayerData[playerid][JobLoc2]][LocZ];
			// Create a checkpoint where the player should unload the goods
			SetPlayerCheckpoint(playerid, x, y, z, 7);
		    // Add 4 to the player's wanted level
		    SetPlayerWantedLevel(playerid, GetPlayerWantedLevel(playerid) + 4);
			// Inform the police this mafia player is wanted
			format(Msg, sizeof(Msg), "{00FF00}Mafia {FFFF00}%s{00FF00} is transporting illegal goods, pursue and fine him", Name);
			Police_SendMessage(Msg);
			// Inform the player that he must unload his goods
			format(UnloadMsg, sizeof(UnloadMsg), TXT_DeliverCargoTo, Load, EndLoc);
			SendClientMessage(playerid, COLOR_WHITE, UnloadMsg);
		}
		case 2: // Player is delivering his goods
		{
		    // Setup local variables
			new StartLoc[50], EndLoc[50], Load[50], Msg1[128], Msg2[128];

			// Get the startlocation, endlocation and the load texts
			format(StartLoc, sizeof(StartLoc), ALocations[APlayerData[playerid][JobLoc1]][LocationName]);
			format(EndLoc, sizeof(EndLoc), ALocations[APlayerData[playerid][JobLoc2]][LocationName]);
			format(Load, sizeof(Load), ALoads[APlayerData[playerid][LoadID]][LoadName]);

			// Construct the message sent to all players that this player completed a mafia mission
			format(Msg1, sizeof(Msg1), TXT_PlayerCompletedMafiaJob, Name, Load);
			format(Msg2, sizeof(Msg2), TXT_PlayerCompletedMafiaJobInfo, StartLoc, EndLoc);
			SendClientMessageToAll(COLOR_WHITE, Msg1);
			SendClientMessageToAll(COLOR_WHITE, Msg2);



		    // Setup local variables
			new Float:x1, Float:y1, Float:x2, Float:y2, Float:Distance, Message[128], Payment;
			// Grab the x, y, z positions for the first location (to load the goods)
			x1 = ALocations[APlayerData[playerid][JobLoc1]][LocX];
			y1 = ALocations[APlayerData[playerid][JobLoc1]][LocY];
			// Grab the x, y, z positions for the second location (to unload the goods)
			x2 = ALocations[APlayerData[playerid][JobLoc2]][LocX];
			y2 = ALocations[APlayerData[playerid][JobLoc2]][LocY];
			// Calculate the distance between both points
			Distance = floatsqroot(((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1)));

			// Calculate the payment for the player
			Payment = floatround((Distance * ALoads[APlayerData[playerid][LoadID]][PayPerUnit]), floatround_floor);
			// Reward the player (give cash and points)
			RewardPlayer(playerid, Payment, 2);

			// Send a message to let the player know he finished his mission and got paid
			format(Message, sizeof(Message), TXT_RewardJob, Payment);
			SendClientMessage(playerid, COLOR_WHITE, Message);

			// Increase the stats for completing a mafia job
			APlayerData[playerid][StatsMafiaJobs]++;
			// Also save the data (in case the server crashes, progress would be lost)
			PlayerFile_Save(playerid);

			// End the current mafia job (clear mission-data)
			Mafia_EndJob(playerid);
		}
	}

    // Enable the player again (he can move again)
	TogglePlayerControllable(playerid, 1);

	return 1;
}



// This function gets called when the mafia player dies (or changes class)
Mafia_EndJob(playerid)
{
	// Kill the PlayerCheckTimer
	KillTimer(APlayerData[playerid][PlayerCheckTimer]);

	// Scan through all players (to reset them to their default colors for the mafia-player)
	for (new PlayerToCheck; PlayerToCheck < MAX_PLAYERS; PlayerToCheck++)
	{
		// Check if this player is connected
		if (IsPlayerConnected(PlayerToCheck))
		    if (APlayerData[PlayerToCheck][PlayerClass] == ClassTruckDriver) // Reset the playercolor for truckers (only truckers can carry mafia-loads)
		        SetPlayerMarkerForPlayer(playerid, PlayerToCheck, ColorClassTruckDriver);
	}

	// Check if a job has started
	if (APlayerData[playerid][JobStarted] == true)
	{
		// Clear all data about the job from the player, so he can start a new one
		APlayerData[playerid][JobStarted] = false;
		APlayerData[playerid][JobID] = 0;
		APlayerData[playerid][JobStep] = 0;
		APlayerData[playerid][LoadID] = 0;
		APlayerData[playerid][JobLoc1] = 0;
		APlayerData[playerid][JobLoc2] = 0;
		// Reset the missiontext
		TextDrawSetString(APlayerData[playerid][MissionText], Mafia_NoJobText);
		// Kill the LoadingTimer
		KillTimer(APlayerData[playerid][LoadingTimer]);
	    // Check if the player has a wanted level of 4 or higher
		if (GetPlayerWantedLevel(playerid) >= 4)
			SetPlayerWantedLevel(playerid, GetPlayerWantedLevel(playerid) - 4); // Reduce the wanted level by 4
		else
			SetPlayerWantedLevel(playerid, 0); // If the player has a wanted level of less than 4, reset the wanted level to 0
	}

	// Also cleanup the vehicle and trailer id's
	APlayerData[playerid][VehicleID] = 0;
	APlayerData[playerid][TrailerID] = 0;
    // Disable the checkpoint where he must bring his load
	DisablePlayerCheckpoint(playerid);

	return 1;
}

