// Forward the function to Load or Unload goods during missions (used by a timer)
forward Trucker_LoadUnload(playerid);



// This function is called when a truckdriver enters a checkpoint
Trucker_OnPlayerEnterCheckpoint(playerid)
{
	// Check if the player is inside his vehicle while entering a checkpoint
	if (GetPlayerVehicleID(playerid) == APlayerData[playerid][VehicleID])
	{
		// Also check if the player still has his trailer attached
		if (APlayerData[playerid][TrailerID] == GetVehicleTrailer(GetPlayerVehicleID(playerid)))
		{
		    // Check the jobstep
		    switch (APlayerData[playerid][JobStep])
		    {
				// JobStep is 1 (truckdriver is loading his goods at the checkpoint)
		 		case 1: GameTextForPlayer(playerid, TXT_TruckerLoadingGoods, 5000, 4);
				// JobStep is 2 (truckdriver is unloading his goods at the checkpoint) or 3 (unloading for convoys)
				case 2, 3: GameTextForPlayer(playerid, TXT_TruckerUnloadingGoods, 5000, 4);
		    }

			// Disable the player's actions (he cannot move anymore)
			TogglePlayerControllable(playerid, 0);
			// Start a timer (Public function "LoadUnload(playerid)" gets called when the timer runs out)
			APlayerData[playerid][LoadingTimer] = SetTimerEx("Trucker_LoadUnload", 5000, false, "d" , playerid);
		}
		else
		    SendClientMessage(playerid, COLOR_WHITE, TXT_NeedTrailerToProceed);
	}
	else
	    SendClientMessage(playerid, COLOR_WHITE, TXT_NeedVehicleToProceed);

	return 1;
}

// After a truckdriver entered a checkpoint, a timer is created. This function is called when the timer runs out
public Trucker_LoadUnload(playerid)
{
	// Check if the player is inside a convoy
	if (APlayerData[playerid][InConvoy] == true)
	{
		// If the player just loaded his goods at the loading-point
	    if (APlayerData[playerid][JobStep] == 1)
		{
	        APlayerData[playerid][JobStep] = 2; // Set the next step of the convoy-job (wait until all members have loaded their cargo)
			TextDrawSetString(APlayerData[playerid][MissionText], TXT_WaitingMembersToLoadCargo);
		}

		// If the player just delivered his goods at the unloading-point
		if (APlayerData[playerid][JobStep] == 3)
		{
	        APlayerData[playerid][JobStep] = 4; // Set the next step of the convoy-job (wait until all members have unloaded their cargo)
	    	TextDrawSetString(APlayerData[playerid][MissionText], TXT_WaitingMembersToUnLoadCargo);
		}

		DisablePlayerCheckpoint(playerid); // Delete the loading/unloading-checkpoint
		TogglePlayerControllable(playerid, 1); // Enable the player again (he can move again)

		return 1; // Don't allow the rest of the function to be executed
	}

	// If the player isn't inside a convoy, this part is executed

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

			// Randomly set the load as overloaded (15% chance the load is overloaded)
			Trucker_SetRandomOverloaded(playerid);

			// Pre-format the missiontext (there may be some parts appended when overloaded/mafiaload
			format(RouteText, sizeof(RouteText), TXT_HaulingCargoFromToDeliver, Load, StartLoc, EndLoc);
			// Check if the player is overloaded
			if (APlayerData[playerid][Overloaded] == true)
			{
			    // Add "(OL)" to the missiontext to let the player know he's been overloaded
				format(RouteText, sizeof(RouteText), "%s%s", RouteText, " ~r~(OL)~w~");
				// Send a message to the player to let him know he's been overloaded
				SendClientMessage(playerid, COLOR_WHITE, TXT_TruckerOverloaded);
			}
			// Check if the player is carrying a mafia-load
			if (ALoads[APlayerData[playerid][LoadID]][Mafia] == true)
			{
			    // Add "(ML)" to the missiontext to let the player know his load is wanted by the mafia
				format(RouteText, sizeof(RouteText), "%s%s", RouteText, " ~r~(ML)~w~");
			    // If the player is carrying a mafia-load, inform him about it
				GameTextForPlayer(playerid, TXT_TruckerMafiaInterested, 5000, 4);
				// Also set the data for the player to indicate he's carrying a mafiaload
				APlayerData[playerid][MafiaLoad] = true;
				// Also set the player's trailer ID (or the vehicle itself) as Mafia-load in the array "AVehicleMafiaLoad"
				if (APlayerData[playerid][TrailerID] == 0)
				    AVehicleData[APlayerData[playerid][VehicleID]][MafiaLoad] = true; // The player has no trailer, so set his main vehicle as wanted by the mafia
				else
                    AVehicleData[APlayerData[playerid][TrailerID]][MafiaLoad] = true; // The player has a trailer, so set his trailer as wanted by the mafia
			}

			// Set the TextDraw so the player can see it
			TextDrawSetString(APlayerData[playerid][MissionText], RouteText);

			// Grab the x, y, z positions for the second location (to unload the goods)
			x = ALocations[APlayerData[playerid][JobLoc2]][LocX];
			y = ALocations[APlayerData[playerid][JobLoc2]][LocY];
			z = ALocations[APlayerData[playerid][JobLoc2]][LocZ];
			// Create a checkpoint where the player should unload the goods
			SetPlayerCheckpoint(playerid, x, y, z, 7);
			// Inform the player that he must unload his goods
			format(UnloadMsg, sizeof(UnloadMsg), TXT_DeliverCargoTo, Load, EndLoc);
			SendClientMessage(playerid, COLOR_WHITE, UnloadMsg);
		}
		case 2: // Player is delivering his goods
		{
		    // Setup local variables
			new StartLoc[50], EndLoc[50], Load[50], Msg1[128], Msg2[128], Name[24], BonusMsg[128];

			// Get the player name
			GetPlayerName(playerid, Name, sizeof(Name));
			// Get the startlocation, endlocation and the load texts
			format(StartLoc, sizeof(StartLoc), ALocations[APlayerData[playerid][JobLoc1]][LocationName]);
			format(EndLoc, sizeof(EndLoc), ALocations[APlayerData[playerid][JobLoc2]][LocationName]);
			format(Load, sizeof(Load), ALoads[APlayerData[playerid][LoadID]][LoadName]);

			// Construct the message sent to all players that this player completed a trucking mission
			format(Msg1, sizeof(Msg1), TXT_PlayerCompletedTruckJob, Name, Load);
			format(Msg2, sizeof(Msg2), TXT_PlayerCompletedTruckJobInfo, StartLoc, EndLoc);
			SendClientMessageToAll(COLOR_WHITE, Msg1);
			SendClientMessageToAll(COLOR_WHITE, Msg2);



		    // Setup local variables
			new Float:x1, Float:y1, Float:x2, Float:y2, Float:Distance, Message[128], Payment, Bonus;
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

			// Check if the player has done the bonus mission
			if (RandomBonusMission[MissionFinished] == false)
			{
				// Check all paramters (load, startlocation and end-location)
				if (RandomBonusMission[RandomLoad] == APlayerData[playerid][LoadID])
					if (RandomBonusMission[RandomStartLoc] == APlayerData[playerid][JobLoc1])
						if (RandomBonusMission[RandomEndLoc] == APlayerData[playerid][JobLoc2])
						{
						    Payment = Payment * 2; // Double the payment if the player was the first to do the bonus mission
                            RandomBonusMission[MissionFinished] = true; // Only one player can do the bonus mission, a new one is chosen next
							format(BonusMsg, sizeof(BonusMsg), "{00BBFF}Player {FFBB00}%s{00BBFF} has finished the bonus mission", Name);
							SendClientMessageToAll(COLOR_WHITE, BonusMsg);
						}
			}

			// Pay the player based on the distance between the loading-point and unloading-point
			RewardPlayer(playerid, Payment, 0);
			// Send a message to let the player know he finished his mission and got paid
			format(Message, sizeof(Message), TXT_RewardJob, Payment);
			SendClientMessage(playerid, COLOR_WHITE, Message);

			// Add 25% bonus if the player has been overloaded
			if (APlayerData[playerid][Overloaded] == true)
			{
			    // Calculate the bonus
			    Bonus = (Payment * 25) / 100;
				// Pay the bonus to the player
				RewardPlayer(playerid, Bonus, 0);
				// Send a message to let the player know he was overloaded and got paid
				format(Message, sizeof(Message), TXT_TruckerBonusOverloaded, Bonus);
				SendClientMessage(playerid, COLOR_WHITE, Message);
			}

			// Add 50% bonus if the player has delivered a mafia load (mafia couldn't steal his load)
			if (APlayerData[playerid][MafiaLoad] == true)
			{
			    // Calculate the bonus
			    Bonus = (Payment * 50) / 100;
				// Pay the bonus to the player
				RewardPlayer(playerid, Bonus, 0);
				// Send a message to let the player know he was overloaded and got paid
				format(Message, sizeof(Message), TXT_TruckerBonusMafiaLoad, Bonus);
				SendClientMessage(playerid, COLOR_WHITE, Message);
			}

			// Add 10% bonus if the player has delivered the load with his own truck
			if (AVehicleData[APlayerData[playerid][VehicleID]][Owned] == true)
			{
			    // Calculate the bonus
			    Bonus = (Payment * 10) / 100;
				// Pay the bonus to the player
				RewardPlayer(playerid, Bonus, 0);
				// Send a message to let the player know he was overloaded and got paid
				format(Message, sizeof(Message), TXT_TruckerBonusOwnVehicle, Bonus);
				SendClientMessage(playerid, COLOR_WHITE, Message);
			}

			// Also add score-points to the score of the player based on the distance between the loading and unloading points
			if (Distance > 3000.0)
				RewardPlayer(playerid, 0, 2); // Distance is larger than 3000 units, so add 2 points
			else
				RewardPlayer(playerid, 0, 1); // Distance is less than 3000 units, so add 1 point

			// Increase the stats for completing a trucking job
			APlayerData[playerid][StatsTruckerJobs]++;
			// Also save the data (in case the server crashes, progress would be lost)
			PlayerFile_Save(playerid);

			// End the current trucker job (clear mission-data)
			Trucker_EndJob(playerid);
		}
	}

    // Enable the player again (he can move again)
	TogglePlayerControllable(playerid, 1);

	return 1;
}

// This function randomly determines if the load is overloaded and adds 2 to the playerwanted-level
Trucker_SetRandomOverloaded(playerid)
{
	// Setup local variables
	new Name[24], Msg[128];

	// Check the vehicle model that the player is driving
	switch (GetVehicleModel(GetPlayerVehicleID(playerid)))
	{
		case VehicleCementTruck: return 0; // A cementtruck cannot be overloaded
		case VehicleLineRunner, VehicleTanker, VehicleRoadTrain: // When driving a LineRunner, Tanker or RoadTrain
		{
			// A Fluids-trailer cannot be overloaded
			if (GetVehicleModel(GetVehicleTrailer(GetPlayerVehicleID(playerid))) == VehicleTrailerFluids) return 0;
		}
	}

	// The player wasn't driving one of the above vehicle models, so this one can be overloaded
	// There is a 15% chance that your load will be overloaded
	if (random(100) <= 15)
	{
		// Set overloaded for this player to True
	    APlayerData[playerid][Overloaded] = true;
	    // Add 2 to the player's wanted level
	    SetPlayerWantedLevel(playerid, GetPlayerWantedLevel(playerid) + 2);
		// Inform the police this trucker is overloaded
		GetPlayerName(playerid, Name, sizeof(Name));
		format(Msg, sizeof(Msg), "{00FF00}Trucker {FFFF00}%s{00FF00} is overloaded, pursue and fine him", Name);
		Police_SendMessage(Msg);
	}

	return 1;
}

// This function is called when a truckdriver wants to start a job by entering "/work" and has no truckers license
Trucker_StartRandomJob(playerid)
{
	// Check if a job could be set correctly (player must be driving a valid trucking vehicle)
	if (Trucker_SetRandomJob(playerid) != 0)
	{
	    // Setup local variables
		new StartLoc[50], EndLoc[50], Load[50], RouteText[255], Float:x, Float:y, Float:z, LoadMsg[128];

		// Job has started
		APlayerData[playerid][JobStarted] = true;

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
		// Set the job-fail-time for the global vehicle-timer
		APlayerData[playerid][VehicleTimerTime] = Job_TimeToFailMission;
		// Inform the player that he must load his goods
		format(LoadMsg, sizeof(LoadMsg), TXT_PickupCargoAt, Load, StartLoc);
		SendClientMessage(playerid, COLOR_WHITE, LoadMsg);
	}

	return 1;
}

// This function sets a random job based on the player's vehicle and returns 1 if a job has been set
// The function returns 0 if a job couldn't be set (if the player is driving an invalid vehicle to start trucking-jobs)
Trucker_SetRandomJob(playerid)
{
	// If the player is the driver of the vehicle (GetPlayerVehicleSeat returns -1 if the player is not in a vehicle)
	if (GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
	{
		// Check the vehicle-model of the player to decide which job the player can get
		switch (GetVehicleModel(GetPlayerVehicleID(playerid)))
		{
			case VehicleFlatbed, VehicleDFT30: // Select a random job from the routes that don't require a trailer and store the data for the player (Flatbed or DFT-30)
				return Trucker_SetRandomJobData(playerid, PCV_TruckerNoTrailer);
			case VehicleCementTruck: // Select a random job from the routes for cementtrucks and store the data for the player
				return Trucker_SetRandomJobData(playerid, PCV_TruckerCementTruck);
			case VehicleLineRunner, VehicleTanker, VehicleRoadTrain: // If the player's vehicle is a "LineRunner", "Tanker" or "RoadTrain"
			{
				// Select a job based on the trailer model of the player
				switch (GetVehicleModel(GetVehicleTrailer(GetPlayerVehicleID(playerid))))
				{
					case VehicleTrailerCargo, VehicleTrailerCargo2: // Select a random job from the routes that require a cargo-trailer and store the data for the player
						return Trucker_SetRandomJobData(playerid, PCV_TruckerCargoTrailer);
					case VehicleTrailerOre: // Select a random job from the routes that require a ore-trailer and store the data for the player
						return Trucker_SetRandomJobData(playerid, PCV_TruckerOreTrailer);
					case VehicleTrailerFluids: // Select a random job from the routes that require a fluids-trailer and store the data for the player
						return Trucker_SetRandomJobData(playerid, PCV_TruckerFluidsTrailer);
				}
			}
		}
	}

	// If no job could be set correctly, return 0
	return 0;
}

// This function chooses a random product for the trucker with a given vehicle-type and also the start-location and end-location
Trucker_SetRandomJobData(playerid, PCV_Needed)
{
	// Get a random Load from the loads that are defined for truckers with the given vehicle-type
    APlayerData[playerid][LoadID] = Product_GetRandom(PCV_Needed);
	// Get a random start-location and end-location for this load
    APlayerData[playerid][JobLoc1] = Product_GetRandomStartLoc(APlayerData[playerid][LoadID]);
    APlayerData[playerid][JobLoc2] = Product_GetRandomEndLoc(APlayerData[playerid][LoadID]);

	// Store the vehicleID (required to be able to check if the player left his vehicle)
	APlayerData[playerid][VehicleID] = GetPlayerVehicleID(playerid);
	// Store the trailerID (required to be able to check if the player lost his trailer)
	APlayerData[playerid][TrailerID] = GetVehicleTrailer(GetPlayerVehicleID(playerid));

	// Return 1 to indicate that a job has been set correctly
	return 1;
}



// This function is used to cleanup the current job
Trucker_EndJob(playerid)
{
	if (APlayerData[playerid][JobStarted] == true)
	{
		// Clear the Mafia-wanted status of the vehicle (or trailer) in the array "AVehicleData"
		if (APlayerData[playerid][TrailerID] == 0)
		    AVehicleData[APlayerData[playerid][VehicleID]][MafiaLoad] = false; // The player has no trailer, so clear his main vehicle as wanted by the mafia
		else
			AVehicleData[APlayerData[playerid][TrailerID]][MafiaLoad] = false; // The player has a trailer, so clear his trailer as wanted by the mafia

		// Clear all data about the job from the player, so he can start a new one
		APlayerData[playerid][JobStarted] = false;
		APlayerData[playerid][JobStep] = 0;
		APlayerData[playerid][JobID] = 0;
		APlayerData[playerid][VehicleTimerTime] = 0;
		APlayerData[playerid][VehicleID] = 0;
		APlayerData[playerid][TrailerID] = 0;
		APlayerData[playerid][LoadID] = 0;
		APlayerData[playerid][JobLoc1] = 0;
		APlayerData[playerid][JobLoc2] = 0;
		APlayerData[playerid][MafiaLoad] = false;

		// Delete the checkpoint
		DisablePlayerCheckpoint(playerid);
		// Reset the missiontext
		TextDrawSetString(APlayerData[playerid][MissionText], Trucker_NoJobText);
		// Kill the LoadingTimer
		KillTimer(APlayerData[playerid][LoadingTimer]);

		// Check if the player has been overloaded
		if (APlayerData[playerid][Overloaded] == true)
		{
			// Clear the overloaded status of the player
		    APlayerData[playerid][Overloaded] = false;

		    // Check if the player has a wanted level of 2 or higher
			if (GetPlayerWantedLevel(playerid) >= 2)
				SetPlayerWantedLevel(playerid, GetPlayerWantedLevel(playerid) - 2); // Reduce the wanted level by 2
			else
				SetPlayerWantedLevel(playerid, 0); // If the player has a wanted level of less than 2, reset the wanted level to 0
		}
	}

	return 1;
}

