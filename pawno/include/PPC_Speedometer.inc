// Forward the function needed to update the speedometer (used by a timer)
forward Speedometer_Update(playerid);
forward RefuelVehicle(playerid);

// This function sets up the speedometer for the given player
Speedometer_Setup(playerid)
{
	// Setup the speedometer for the player
	APlayerData[playerid][VehicleName] = TextDrawCreate(500.0, 380.0, " ");
	APlayerData[playerid][SpeedometerText] = TextDrawCreate(500.0, 395.0, " ");
	APlayerData[playerid][FuelGauge] = TextDrawCreate(500.0, 410.0, " ");
	// Enable the TextDraw for this player
	TextDrawShowForPlayer(playerid, APlayerData[playerid][VehicleName]);
	TextDrawShowForPlayer(playerid, APlayerData[playerid][SpeedometerText]);
	TextDrawShowForPlayer(playerid, APlayerData[playerid][FuelGauge]);

	// Start the speedometer timer
	APlayerData[playerid][SpeedometerTimer] = SetTimerEx("Speedometer_Update", 500, true, "i", playerid);

	return 1;
}

// This function cleans up the speedometer for the given player
Speedometer_Cleanup(playerid)
{
	// Destroy the speedometer textdraw
	TextDrawDestroy(APlayerData[playerid][VehicleName]);
	TextDrawDestroy(APlayerData[playerid][SpeedometerText]);
	TextDrawDestroy(APlayerData[playerid][FuelGauge]);
	// Kill the speedometer timer
	KillTimer(APlayerData[playerid][SpeedometerTimer]);
	// Set player speed to 0
	APlayerData[playerid][PlayerSpeed] = 0;

	return 1;
}

// This function gets called by a timer which runs every 500ms to display and update the speedometer
public Speedometer_Update(playerid)
{
	// Setup local variables
	new vehicleid, vehiclemodel, Float:speed_x, Float:speed_y, Float:speed_z, Float:final_speed, speed_string[50], final_speed_int, Float:vehiclehealth;
	new FuelString[50], VehName[30], FuelStatus[20];
	new Msg[128], Name[24];

	// Get the ID of the player's vehicle
	vehicleid = GetPlayerVehicleID(playerid);
	// Get the model id of the vehicle
	vehiclemodel = GetVehicleModel(vehicleid);

	GetPVarString(playerid, "VehicleName", VehName, sizeof(VehName));

	//******************************************************************************************************************************
	// Anti-hack stuff
	//******************************************************************************************************************************

	// exclude NPC from AntiHack
	if (IsPlayerNPC(playerid) == 0)
		AntiHack(playerid);

	//******************************************************************************************************************************
	// End of anti-hack stuff
	//******************************************************************************************************************************

	// Check and toggle spectate-mode when needed (when target player entered or exited his vehicle)
	if (GetPlayerState(playerid) == PLAYER_STATE_SPECTATING)
	{
	    // Get the target player's ID and name
	    new OtherPlayer = APlayerData[playerid][SpectateID];
		GetPlayerName(OtherPlayer, Name, sizeof(Name));

		// Use the same worldid and interiorid as the OtherPlayer
		SetPlayerVirtualWorld(playerid, GetPlayerVirtualWorld(OtherPlayer));
		SetPlayerInterior(playerid, GetPlayerInterior(OtherPlayer));

		// Check if the player is spectating a player
		if (APlayerData[playerid][SpectateType] == ADMIN_SPEC_TYPE_PLAYER)
		{
		    // Check if the target player has entered a vehicle
		    if (GetPlayerVehicleSeat(OtherPlayer) != -1)
		    {
				// Change spectate mode to vehicle
				PlayerSpectateVehicle(playerid, GetPlayerVehicleID(OtherPlayer));
				APlayerData[playerid][SpectateID] = OtherPlayer;
				APlayerData[playerid][SpectateVehicle] = GetPlayerVehicleID(OtherPlayer);
				APlayerData[playerid][SpectateType] = ADMIN_SPEC_TYPE_VEHICLE;
				format(Msg, sizeof(Msg), "{00FF00}Player {FFFF00}%s{00FF00} has entered a vehicle, changing spectate mode to match", Name);
				SendClientMessage(playerid, COLOR_WHITE, Msg);
		    }
		}
		else // The player is spectating a vehicle
		{
		    // Check if the target player has exited a vehicle
		    if (GetPlayerVehicleSeat(OtherPlayer) == -1)
		    {
				// Change spectate mode to player
				PlayerSpectatePlayer(playerid, OtherPlayer);
				SetPlayerInterior(playerid, GetPlayerInterior(OtherPlayer));
				APlayerData[playerid][SpectateID] = OtherPlayer;
				APlayerData[playerid][SpectateType] = ADMIN_SPEC_TYPE_PLAYER;
				format(Msg, sizeof(Msg), "{00FF00}Player {FFFF00}%s{00FF00} has exited a vehicle, changing spectate mode to match", Name);
				SendClientMessage(playerid, COLOR_WHITE, Msg);
		    }
		}
	}

	// When the player got a wanted level and a police player warned him to stop, a timer is started and a variable is set to "true"
	// Check if this variable has been set
	if (APlayerData[playerid][PoliceWarnedMe] == true)
	{
	    // Check if the player has no wanted level anymore (after finishing a overloaded mission, player got fined, ...)
		if (GetPlayerWantedLevel(playerid) == 0)
		{
			APlayerData[playerid][PoliceCanJailMe] = false; // Clear the variable
			APlayerData[playerid][PoliceWarnedMe] = false; // Clear the variable
			APlayerData[playerid][Value_PoliceCanJailMe] = 0; // Clear the remaining time for the timer
			KillTimer(APlayerData[playerid][Timer_PoliceCanJailMe]); // Kill the timer
		}
	}

	// If the player is inside a vehicle
	if(vehicleid != 0)
	{
		// Get the vehicles velocity
		GetVehicleVelocity(vehicleid, speed_x, speed_y, speed_z);
		// Calculate the speed (in kph)
		final_speed = floatsqroot(((speed_x * speed_x) + (speed_y * speed_y)) + (speed_z * speed_z)) * 158.179;
		// Convert the float value to an int value
		final_speed_int = floatround(final_speed, floatround_round);
		// Also save the speed for the player
		APlayerData[playerid][PlayerSpeed] = final_speed_int;
		// Setup the string to display for the player and display it
		format(speed_string, sizeof(speed_string), TXT_SpeedometerSpeed, final_speed_int);
		TextDrawSetString(APlayerData[playerid][SpeedometerText], speed_string);

		// Add the speed to the stats (this will be the meters driven in total)
		APlayerData[playerid][StatsMetersDriven] = APlayerData[playerid][StatsMetersDriven] + (final_speed / 7.2);

		// Also display the vehicle's health through the player-health bar
		GetVehicleHealth(vehicleid, vehiclehealth);
		SetPlayerHealth(playerid, vehiclehealth / 10.0);

		if (isnull(VehName)) {
			for (new i = 0; i < sizeof(ABuyableVehicles); i++)
			{
				// Assign the name of the vehicle based on it's model id
				if (ABuyableVehicles[i][CarModel] == vehiclemodel) {
					SetPVarString(playerid,"VehicleName",ABuyableVehicles[i][CarName]);
				}
			}
		}

		TextDrawSetString(APlayerData[playerid][VehicleName], VehName);

		// Check if the speed is above 10kph and the fuel of the vehicle isn't empty yet
		if ((final_speed_int > 10) && (AVehicleData[vehicleid][Fuel] > 0))
			AVehicleData[vehicleid][Fuel] = AVehicleData[vehicleid][Fuel] - 1; // Decrease the fuel for this vehicle every time the timer is run

		// Construct the fuelgauge
		if ((AVehicleData[vehicleid][Fuel] > 0) && (AVehicleData[vehicleid][Fuel] < 100000))
			format(FuelStatus, sizeof(FuelStatus), "~g~%s~r~%s", "I", "IIIIIIIII"); // Fuel is between 0% and 10% full

		if ((AVehicleData[vehicleid][Fuel] >= ((MaxFuel / 10) * 1)) && (AVehicleData[vehicleid][Fuel] < ((MaxFuel / 10) * 2)))
			format(FuelStatus, sizeof(FuelStatus), "~g~%s~r~%s", "II", "IIIIIIII"); // Fuel is between 10% and 20% full

		if ((AVehicleData[vehicleid][Fuel] >= ((MaxFuel / 10) * 2)) && (AVehicleData[vehicleid][Fuel] < ((MaxFuel / 10) * 3)))
			format(FuelStatus, sizeof(FuelStatus), "~g~%s~r~%s", "III", "IIIIIII"); // Fuel is between 20% and 30% full

		if ((AVehicleData[vehicleid][Fuel] >= ((MaxFuel / 10) * 3)) && (AVehicleData[vehicleid][Fuel] < ((MaxFuel / 10) * 4)))
			format(FuelStatus, sizeof(FuelStatus), "~g~%s~r~%s", "IIII", "IIIIII"); // Fuel is between 30% and 40% full

		if ((AVehicleData[vehicleid][Fuel] >= ((MaxFuel / 10) * 4)) && (AVehicleData[vehicleid][Fuel] < ((MaxFuel / 10) * 5)))
			format(FuelStatus, sizeof(FuelStatus), "~g~%s~r~%s", "IIIII", "IIIII"); // Fuel is between 40% and 50% full

		if ((AVehicleData[vehicleid][Fuel] >= ((MaxFuel / 10) * 5)) && (AVehicleData[vehicleid][Fuel] < ((MaxFuel / 10) * 6)))
			format(FuelStatus, sizeof(FuelStatus), "~g~%s~r~%s", "IIIIII", "IIII"); // Fuel is between 50% and 60% full

		if ((AVehicleData[vehicleid][Fuel] >= ((MaxFuel / 10) * 6)) && (AVehicleData[vehicleid][Fuel] < ((MaxFuel / 10) * 7)))
			format(FuelStatus, sizeof(FuelStatus), "~g~%s~r~%s", "IIIIIII", "III"); // Fuel is between 60% and 70% full

		if ((AVehicleData[vehicleid][Fuel] >= ((MaxFuel / 10) * 7)) && (AVehicleData[vehicleid][Fuel] < ((MaxFuel / 10) * 8)))
			format(FuelStatus, sizeof(FuelStatus), "~g~%s~r~%s", "IIIIIIII", "II"); // Fuel is between 70% and 80% full

		if ((AVehicleData[vehicleid][Fuel] >= ((MaxFuel / 10) * 8)) && (AVehicleData[vehicleid][Fuel] < ((MaxFuel / 10) * 9)))
			format(FuelStatus, sizeof(FuelStatus), "~g~%s~r~%s", "IIIIIIIII", "I"); // Fuel is between 80% and 90% full

		if ((AVehicleData[vehicleid][Fuel] >= ((MaxFuel / 10) * 9)) && (AVehicleData[vehicleid][Fuel] <= MaxFuel))
			format(FuelStatus, sizeof(FuelStatus), "~g~%s", "IIIIIIIIII"); // Fuel is between 90% and 100% full (all bars are green)

		if (AVehicleData[vehicleid][Fuel] == 0)
			format(FuelStatus, sizeof(FuelStatus), "~r~%s", "IIIIIIIIII"); // Fuel is empty (all bars are red)

		// Format the final fuel-gauge readout
		format(FuelString, sizeof(FuelString), TXT_SpeedometerFuel, FuelStatus);
		// Display the fuel-gauge
		TextDrawSetString(APlayerData[playerid][FuelGauge], FuelString);

		// Check if the vehicle is out of fuel
		if (AVehicleData[vehicleid][Fuel] == 0)
		{
			// Stop the engine and turn off the lights
			new engine,lights,alarm,doors,bonnet,boot,objective;
			GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
			SetVehicleParamsEx(vehicleid, 0, 0, alarm, doors, bonnet, boot, objective);
		}

		// Check if the player is not in any plane or helicopter (those cannot be caught by speedcamera's)
		if (IsVehicleAirVehicle(vehicleid) == 0)
			if (APlayerData[playerid][PlayerClass] != ClassPolice) // Check if the player isn't speeding (cops won't get caught)
				CheckPlayerSpeeding(playerid);
	}
	else
	{
		// If the player is not inside a vehicle, display an empty string (looks like the speedometer is gone)
		TextDrawSetString(APlayerData[playerid][VehicleName], " ");
		TextDrawSetString(APlayerData[playerid][SpeedometerText], " ");
		TextDrawSetString(APlayerData[playerid][FuelGauge], " ");
		// Set the speed of the player to 0
		APlayerData[playerid][PlayerSpeed] = 0;
		DeletePVar(playerid, "VehicleName");
	}
}



// This timer-function is called when a player picks up a refuelpickup
public RefuelVehicle(playerid)
{
	new RefuelMsg[128];
	// Get the vehicle-id of the player's vehicle
	new vID = GetPlayerVehicleID(playerid);
	// Calculate the amount of fuel that needs to be refuelled
	new Amount = MaxFuel - AVehicleData[vID][Fuel];
	// Calculate the price to refuel
	new RefuelPrice = (Amount * RefuelMaxPrice) / MaxFuel;

	// Check if the player has enough cash
	if (APlayerData[playerid][PlayerMoney] >= RefuelPrice)
	{
		// Refuel the vehicle
		AVehicleData[vID][Fuel] = MaxFuel;
		// Withdraw the money from the player
		RewardPlayer(playerid, -RefuelPrice, 0);
		// Let the player know he refuelled his vehicle
		format(RefuelMsg, sizeof(RefuelMsg), TXT_RefuelledVehicle, RefuelPrice);
		SendClientMessage(playerid, COLOR_WHITE, RefuelMsg);
	}
	else
	    SendClientMessage(playerid, COLOR_WHITE, TXT_CannotRefuelVehicle);

	// Allow the player to move again
	TogglePlayerControllable(playerid, 1);

	return 1;
}



// This function checks if the player is speeding near a speedcamera
CheckPlayerSpeeding(playerid)
{
	// Setup local variables
	new Name[24], Msg[128];

	// Check if the player hasn't been caught speeding recently
	if (APlayerData[playerid][PlayerCaughtSpeeding] == 0)
	{
		// Loop through all speedcameras
		for (new CamID; CamID < MAX_CAMERAS; CamID++)
		{
		    // Check if this camera has been created
		    if (ACameras[CamID][CamSpeed] != 0)
		    {
				// Check if the player is the driver of the vehicle
				if (GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
				{
					// Check if the player's speed is greater than the speed allowed by this camera (no need to process a distance-check if not speeding)
					if (APlayerData[playerid][PlayerSpeed] > ACameras[CamID][CamSpeed])
					{
						// Check if the player is near the camera
						if (IsPlayerInRangeOfPoint(playerid, 50.0, ACameras[CamID][CamX], ACameras[CamID][CamY], ACameras[CamID][CamZ]))
						{
						    // Prevent the player being caught multiple times by the same speed-camera
						    APlayerData[playerid][PlayerCaughtSpeeding] = 20;
						    // Increase the wanted-level of this player by 1 star
						    SetPlayerWantedLevel(playerid, GetPlayerWantedLevel(playerid) + 1);
						    // Let the player know he's been caught speeding
						    SendClientMessage(playerid, COLOR_WHITE, TXT_PlayerCaughtSpeeding);

							// Get the name of the player
							GetPlayerName(playerid, Name, sizeof(Name));
							// Also inform all police players that this player is caught speeding
							format(Msg, sizeof(Msg), "{00FF00}Player {FFFF00}%s{00FF00} is caught speeding, pursue and fine him", Name);
							Police_SendMessage(Msg);
						}
					}
				}
		    }
		}
	}
	else // If the player has been caught before, reduce the value until it's 0 again, then he can be caught again
	    APlayerData[playerid][PlayerCaughtSpeeding]--;
}



// This function processes anti-hack stuff
stock AntiHack(playerid)
{
	// Setup local variables
	new Float:Armour;

	// Skip checking for hacks used by the player if he was reported by the Anti-Hack system already
	if (APlayerData[playerid][AutoReportTime] > 0)
	{
	    // Reduce the time so the player can be reported again soon if he doesn't stop using hacks
	    APlayerData[playerid][AutoReportTime]--;
		// Exit the function, this skips the hack-checks until the AutoReportTime has reached 0
		// Otherwise the player is reported every half a second until he stops using hacks
	    return 1;
	}



	// Check if a filterscript gave some money (or took it) to the player
	if (GetPVarInt(playerid, "PVarMoney") != 0)
	{
		// Add the money to the players account
		APlayerData[playerid][PlayerMoney] = APlayerData[playerid][PlayerMoney] + GetPVarInt(playerid, "PVarMoney");
		// Clear the PVar
		SetPVarInt(playerid, "PVarMoney", 0);
	}
	if (GetPVarInt(playerid, "PVarScore") != 0)
	{
		// Add the money to the players account
		APlayerData[playerid][PlayerScore] = APlayerData[playerid][PlayerScore] + GetPVarInt(playerid, "PVarScore");
		// Clear the PVar
		SetPVarInt(playerid, "PVarScore", 0);
	}

	// Reset the player's money and set it to the stored value in the player's account (do the same for scorepoints)
	ResetPlayerMoney(playerid);
	GivePlayerMoney(playerid, APlayerData[playerid][PlayerMoney]);
	SetPlayerScore(playerid, APlayerData[playerid][PlayerScore]);

	// Limit the cash that the player can have
	if (APlayerData[playerid][PlayerMoney] > 999000000)
	    APlayerData[playerid][PlayerMoney] = 999000000;

	// Limit the cash that the player can have below 0
	if (APlayerData[playerid][PlayerMoney] < -1000000)
	    APlayerData[playerid][PlayerMoney] = -1000000;

	// Port anyone out of the area who is not an admin and inside the area 69
	Player_PortOutAdminZone(playerid, 106.0, 1805.0, -50.0, 285.0, 1940.0, 40.0, 15.0, 1732.0, 25.0);

	// Weapon hacks are also neutralized here, except for police players (if they are allowed to have weapons)
	if ((PoliceGetsWeapons == true) && (APlayerData[playerid][PlayerClass] == ClassPolice))
	{
	    // Do nothing
	}
	else // Extended check to allow parachute
	{
		if (GetPlayerWeapon(playerid) != WEAPON_PARACHUTE) // Check if the current weapons isn't a parachute (ID: 46 (WEAPON_PARACHUTE))
			ResetPlayerWeapons(playerid); // If it's not, remove weapon
	}

	// Check if the player got any armour (= health-hack)
	GetPlayerArmour(playerid, Armour);
	// Send an automated report to the admins so they're informed about it and can take action
	if (Armour > 1.0)
        SendReportToAdmins(playerid, "Health-hack", true);

	// Check if the speed is higher than 300 (kick player if it is)
	// Send an automated report to the admins so they're informed about it and can take action
	if (APlayerData[playerid][PlayerSpeed] > 300)
        SendReportToAdmins(playerid, "Speed-hack", true);

	// Check if the player is not allowed to have a jetpack (admins lvl 3 and higher can use /fly, so they will be excluded)
	if (APlayerData[playerid][PlayerLevel] < 3)
	{
		// Check if the player is using a jetpack
		// Send an automated report to the admins so they're informed about it and can take action
		if (GetPlayerSpecialAction(playerid) == 2)
	        SendReportToAdmins(playerid, "Jetpack-hack", true);
	}

	// Check if the player has an admin level 1 and higher, if so ignore him for airbreak hack
	// this airbreack hack gets called sometimes if you really drive fast on a mountain
	// even this can happen to players resulting in a false-positive report, we must stick to
	// this check as it gives extra security
	if (APlayerData[playerid][PlayerLevel] < 1)
	{
		// Detect airbreak hack
		if (GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
		{
			// Check if the player is nearly standing still
			if (APlayerData[playerid][PlayerSpeed] < 10)
			{
				// Check if the player switched interior-id's
				if (GetPlayerInterior(playerid) != APlayerData[playerid][PreviousInt])
				{
					// Check if the new interior is the normal world or any mod-shop
				    switch (GetPlayerInterior(playerid))
					{
						case 0, 1, 2, 3: // Check interiors 0, 1, 2 and 3 (normal world and all mod-shops)
						{
							// Store the player's current location and interior-id for the next iteration
							GetPlayerPos(playerid, APlayerData[playerid][PreviousX], APlayerData[playerid][PreviousY], APlayerData[playerid][PreviousZ]);
							APlayerData[playerid][PreviousInt] = GetPlayerInterior(playerid);
							// Exit the function
							return 1;
						}
					}
				}

			    // Check if the player is still near the same place he was half a second ago
				if (IsPlayerInRangeOfPoint(playerid, 7.5, APlayerData[playerid][PreviousX], APlayerData[playerid][PreviousY], APlayerData[playerid][PreviousZ]))
				{
				}
				else // Send an automated report to the admins so they're informed about it and can take action
			        SendReportToAdmins(playerid, "Airbreak-hack", true);
			}
		}
	}

	// Store the player's current location and interior-id for the next iteration
	GetPlayerPos(playerid, APlayerData[playerid][PreviousX], APlayerData[playerid][PreviousY], APlayerData[playerid][PreviousZ]);
	APlayerData[playerid][PreviousInt] = GetPlayerInterior(playerid);

	return 1;
}

