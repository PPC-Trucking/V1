// This file holds all common functions that don't fit in any other file

// This functions gives the player the given amount of money and scorepoints
RewardPlayer(playerid, Money, Points)
{
	// Add the given amount of money to the player's account
	APlayerData[playerid][PlayerMoney] = APlayerData[playerid][PlayerMoney] + Money;
	// Add the given amount of scorepoints to the player's account
	APlayerData[playerid][PlayerScore] = APlayerData[playerid][PlayerScore] + Points;
}

// This function creates the vehicle and saves the vehicle-lodel in the AVehicleData-array (can only be used during GameModeInit)
// It also sets the fuel to maximum
Vehicle_AddStatic(vModel, Float:vX, Float:vY, Float:vZ, Float:vRotation, vC1, vC2, vSpawnDelay)
{
	// Create a new static vehicle during GameModeInit
	new vid = AddStaticVehicleEx(vModel, vX, vY, vZ, vRotation, vC1, vC2, vSpawnDelay);
	// Save the vehicle-model
    AVehicleData[vid][Model] = vModel;
	// Set the fuel to maximum so the vehicle can be used
    AVehicleData[vid][Fuel] = MaxFuel;
	// Save the colors
    AVehicleData[vid][Color1] = vC1;
    AVehicleData[vid][Color2] = vC2;
	// Set this vehicle as a static vehicle
	AVehicleData[vid][StaticVehicle] = true;

	return vid;
}

// This function is the same as Vehicle_AddStatic, but uses CreateVehicle instead of AddStaticVehicleEx, so can be used everywhere
Vehicle_Create(vModel, Float:vX, Float:vY, Float:vZ, Float:vRotation, vC1, vC2, vSpawnDelay)
{
	// Create a new static vehicle during GameModeInit
	new vid = CreateVehicle(vModel, vX, vY, vZ, vRotation, vC1, vC2, vSpawnDelay);
	// Save the vehicle-model
    AVehicleData[vid][Model] = vModel;
	// Set the fuel to maximum so the vehicle can be used
    AVehicleData[vid][Fuel] = MaxFuel;
	// Save the colors
    AVehicleData[vid][Color1] = vC1;
    AVehicleData[vid][Color2] = vC2;

	return vid;
}

// This function deletes the vehicle and clears all the data (is only used for player-owned vehicles)
Vehicle_Delete(vid)
{
	// Setup local variables
	new HouseID, CarSlot;

	// Get the HouseID and CarSlot where the vehicle is linked to
	HouseID = AVehicleData[vid][BelongsToHouse];

	// Check if this was a valid HouseID
	if (HouseID != 0)
	{
		// Loop through all carslots of this house to find the vehicle-id
		for (new i; i < 10; i++)
		{
			// Check if this carslot holds the same vehicle-id
		    if (AHouseData[HouseID][VehicleIDs][i] == vid)
		    {
		        CarSlot = i; // The carslot has been found where the vehicle is stored, remember it
				break; // Stop the for-loop
			}
		}

		// Remove the vehicle from the house
		AHouseData[HouseID][VehicleIDs][CarSlot] = 0;
	}

	// Delete the vehicle
	DestroyVehicle(vid);
	// Clear the data
	AVehicleData[vid][Owned] = false;
	AVehicleData[vid][Owner] = 0;
	AVehicleData[vid][Model] = 0;
	AVehicleData[vid][PaintJob] = 0;
	for (new i; i < 14; i++)
	    AVehicleData[vid][Components][i] = 0;
	AVehicleData[vid][Color1] = 0;
	AVehicleData[vid][Color2] = 0;
	AVehicleData[vid][SpawnX] = 0.0;
	AVehicleData[vid][SpawnY] = 0.0;
	AVehicleData[vid][SpawnZ] = 0.0;
	AVehicleData[vid][SpawnRot] = 0.0;
	AVehicleData[vid][BelongsToHouse] = 0;
	DestroyDynamic3DTextLabel(AVehicleData[vid][VehicleText]);
}

// This function returns "1" if both locations are not closeby and returns "0" if both locations are close to eachother
Locations_CheckDistance(LocationA, LocationB, Float:Range)
{
	// Setup local variables
	new Float:Xa, Float:Ya, Float:Xb, Float:Yb, Float:X, Float:Y;

	// Get the coordinates of LocationA
	Xa = ALocations[LocationA][LocX];
	Ya = ALocations[LocationA][LocY];
	// Get the coordinates of LocationB
	Xb = ALocations[LocationB][LocX];
	Yb = ALocations[LocationB][LocY];

	// Calculate the distances between both locations
	Y = Yb - Ya;
	X = Xb - Xa;

	// Check if both locations are further apart then the range indicates
	if (((X * X) + (Y * Y)) > (Range * Range))
		return 1; // Location B is further away from Location A than Range indicates
	else
	    return 0; // both locations are closer to eachother than Range indicates
}

// This function ports all non-admin players out of the given area to the location specified
Player_PortOutAdminZone(playerid, Float:x1, Float:y1, Float:z1, Float:x2, Float:y2, Float:z2, Float:x3, Float:y3, Float:z3)
{
	// Setup local variables
	new Float:x, Float:y, Float:z;

	// Get the player's coordinates
	GetPlayerPos(playerid, x, y, z);

	// Check if the player is not an admin
	if (APlayerData[playerid][PlayerLevel] == 0)
		if ((x1 < x) && (x < x2))
		    if ((y1 < y) && (y < y2))
		        if ((z1 < z) && (z < z2))
				{
					SendClientMessage(playerid, COLOR_RED, "You're not allowed inside an admin-area");
					SetPlayerPos(playerid, x3, y3, z3); // Port the player out of the area
				}
}

// This function sends the given text to all admins
SendAdminText(playerid, command[], text[])
{
	// Setup local variables
	new Name[24], Msg[128];

	// Get the player's name
	GetPlayerName(playerid, Name, sizeof(Name));
	
	// Loop through all players
	for (new i; i < MAX_PLAYERS; i++)
	{
		// Check if the player is an admin
		if (APlayerData[i][PlayerLevel] > 0)
		{
			// Send the given text to the admin
			format(Msg, sizeof(Msg), "{00FFCC}%s used: %s %s", Name, command, text);
			SendClientMessage(i, COLOR_WHITE, Msg);
		}
	}

	// Also log all used commands in the server.log file
	format(Msg, sizeof(Msg), "%s used: %s %s", Name, command, text);
	print(Msg);
}

// This timer informs the player how long he will be frozen
forward Player_FreezeTimer(playerid);
public Player_FreezeTimer(playerid)
{
	// Setup local variables
	new Msg[128];

	// Decrease the remaining time this player will be frozen
	APlayerData[playerid][PlayerFrozen]--;

	// Check if the player is allowed to move again
	if (APlayerData[playerid][PlayerFrozen] >= 1)
	{
		// Construct the message to inform the player how long he stays frozen
		if (APlayerData[playerid][PlayerFrozen] >= 60)
			format(Msg, sizeof(Msg), "Frozen for %i minutes", APlayerData[playerid][PlayerFrozen] / 60);
		else
			format(Msg, sizeof(Msg), "Frozen for %i seconds", APlayerData[playerid][PlayerFrozen]);

		// Display the message to inform the player how long he stays frozen
		GameTextForPlayer(playerid, Msg, 1000, 4);
	}
	else { // The timer has run out, so allow his to move again
		TogglePlayerControllable(playerid, 1);
		KillTimer(FreezeTimer[playerid]);
	}
}

// This function creates a list of commands, starting from the FirstCommand and automatically shows the dialog
CommandList_Create(playerid)
{
	// Setup local variables
	new Counter, CommandList[1000], DialogTitle[128];

	// Only add 4 commands to the list, starting from the FirstItem
	for (new i = APlayerData[playerid][DialogFirstItem]; i < sizeof(ACommands); i++)
	{
		// Increase a counter (which holds the number of commands that have been added to the list
		Counter++;

		// Check if the maximum hasn't been reached yet
		if (Counter <= 5)
		{
		    // Check if this command is valid for this player
			if (APlayerData[playerid][PlayerLevel] >= ACommands[i][CommandLevel])
			{
				// Check if the command is used for admins or not
				if (ACommands[i][CommandLevel] == 0)
				{
				    // Add the commandname and description to the list
					format(CommandList, sizeof(CommandList), "%s%s{00FF00}%s", CommandList, "\n", ACommands[i][CommandStructure]); // Add the name of the next command to the list on the next line
					format(CommandList, sizeof(CommandList), "%s%s\t\t{FFFF00}%s", CommandList, "\n", ACommands[i][CommandDescription]); // Add the description of the next command to the list on the next line
				}
				else
				{
				    // Add the commandname and description to the list
					format(CommandList, sizeof(CommandList), "%s%sLvl %i: {00FF00}%s", CommandList, "\n", ACommands[i][CommandLevel], ACommands[i][CommandStructure]); // Add the name of the next command to the list on the next line
					format(CommandList, sizeof(CommandList), "%s%s\t\t{FFFF00}%s", CommandList, "\n", ACommands[i][CommandDescription]); // Add the description of the next command to the list on the next line
				}
			}
			else
				break;
		}
		else // 5 commands have been added to the list (now Counter = 6)
		{
		    // Add an empty line and "Next..." to the list to let the player know there are more commands
			format(CommandList, sizeof(CommandList), "%s%s%s", CommandList, "\n \n", TXT_DialogEntryNext);
			// Also stop the For-loop
			break;
		}
	}

	// Construct the title for the dialog (to include a page number)
	format(DialogTitle, sizeof(DialogTitle), TXT_DialogCommandTitle, (APlayerData[playerid][DialogFirstItem] / 5) + 1);
	// Show the commands in a dialog
	ShowPlayerDialog(playerid, DialogPlayerCommands, DIALOG_STYLE_LIST, DialogTitle, CommandList, TXT_DialogButtonSelect, TXT_DialogButtonCancel);

	return 1;
}

// This function returns "1" if the given vehicle-id is a plane or helicopter
IsVehicleAirVehicle(vid)
{
	switch (GetVehicleModel(vid))
	{
		case 592, 577, 511, 512, 593, 520, 553, 476, 519, 460, 513, 548, 425, 417, 487, 488, 497, 563, 447, 469: return 1;
		default: return 0;
	}

	return 0;
}

// This function creates a speedcamera (store data and create the objects)
SetupSpeedCamera(CamID, Float:x, Float:y, Float:z, Float:rot, MaxSpeed)
{
	// Store all the given values
    ACameras[CamID][CamX] = x;
    ACameras[CamID][CamY] = y;
    ACameras[CamID][CamZ] = z;
    ACameras[CamID][CamAngle] = rot;
    ACameras[CamID][CamSpeed] = MaxSpeed;
	// Create both camera objects and store their reference
    ACameras[CamID][CamObj1] = CreateObject(18880, x, y, z, 0.0, 0.0, rot);
    ACameras[CamID][CamObj2] = CreateObject(18880, x, y, z, 0.0, 0.0, rot + 180.0);
}



// This function sends the report to all admins
SendReportToAdmins(OffenderID, Reason[], bool:AutoReport = false)
{
	// Setup local variables
	new Name[24], Msg[128], TxtMsg[128], TotalReason[128];

	// Get the name of the offender
	GetPlayerName(OffenderID, Name, sizeof(Name));

	// Construct the report message for all admins
	if (AutoReport == false)
	{
	    // Construct the report messages for a normal report
		format(Msg, sizeof(Msg), "{00FFFF}*** Report: %s (%i): %s", Name, OffenderID, Reason);
		format(TxtMsg, sizeof(TxtMsg), "Report:~n~~g~%s (%i)~n~~r~%s", Name, OffenderID, Reason);
		format(TotalReason, sizeof(TotalReason), Reason);
	}
	else
	{
	    // Construct the report messages for an automated report (sent by the AntiHack function)
		format(Msg, sizeof(Msg), "{00FFFF}*** Auto-Report: %s (%i): %s", Name, OffenderID, Reason);
		format(TxtMsg, sizeof(TxtMsg), "Auto-Report:~n~~g~%s (%i)~n~~r~%s", Name, OffenderID, Reason);
		format(TotalReason, sizeof(TotalReason), "%s (by AntiHack)", Reason);
		// For automated reports, preset the time to 60 seconds to stop the Anti-Hack system reporting the player every half a second
		APlayerData[OffenderID][AutoReportTime] = 120; // The time must be doubled, as the speedometer runs twice every second
	}

	// Loop through all the players to find all online admins
	for (new playerid; playerid < MAX_PLAYERS; playerid++)
	{
	    // Check if this player is an admin
	    if (APlayerData[playerid][PlayerLevel] > 0)
	    {
			// Send a message to the admin to inform him about the report
			SendClientMessage(playerid, COLOR_WHITE, Msg);
			// Also send the admin a GameText-message so he can see it more clearly
			GameTextForPlayer(playerid, TxtMsg, 10000, 4);
	    }
	}

	// Add the report to the AReports-array so admins can review it in a dialog (maximum 50 reports can be stored)
	AddReport(OffenderID, TotalReason);
}

// This function adds the report to the AReports array automatically
AddReport(OffenderID, Reason[])
{
	// Setup local variables
	new ReportID = -1, Name[24];

	// Check if there is a free spot in the AReports array
	for (new i; i < 50; i++)
	{
		// Check if this report-spot is empty
		if (AReports[i][ReportUsed] == false)
		{
		    ReportID = i;
		    break; // If this spot hasn't been used yet, stop the for-loop
		}
	}

	// If no spot is free (ReportID is still -1)
	if (ReportID == -1)
	{
		// Drop the report on ID 0, and move them all downwards
		for (new i = 1; i < 50; i++)
		{
			AReports[i - 1][ReportUsed] = AReports[i][ReportUsed];
			format(AReports[i - 1][ReportName], 24, "%s", AReports[i][ReportName]);
			format(AReports[i - 1][ReportReason], 128, "%s", AReports[i][ReportReason]);
		}

		// ReportID 49 is available now, so use this to add the new report
		ReportID = 49;
	}

	// Get the name of the offender
	GetPlayerName(OffenderID, Name, sizeof(Name));

	// Store the data into the array
	AReports[ReportID][ReportUsed] = true;
	format(AReports[ReportID][ReportName], 24, "%s", Name);
	format(AReports[ReportID][ReportReason], 128, "%s", Reason);
}



// This function converts an ip-address into 4 separate integer parts and returns a string that only holds the first three parts
GetFirstThreeDigitsFromIP(PlayerIP[])
{
	// Setup local variables
	new Part1, Part2, Part3, DotLoc, RetIP[16];

	// Get the first part of the IP-address
	Part1 = strval(PlayerIP[0]);

	// Find the first dot
	DotLoc = strfind(PlayerIP, ".", false, 0);
	// Get the second part of the IP-address (the part that follows the first dot)
	Part2 = strval(PlayerIP[DotLoc+1]);

	// Find the second dot
	DotLoc = strfind(PlayerIP, ".", false, DotLoc+1);
	// Get the third part of the IP-address (the part that follows the second dot)
	Part3 = strval(PlayerIP[DotLoc+1]);

	// Combine them all into an IP that only holds the first three digits, followed by a dot
	format(RetIP, sizeof(RetIP), "%i.%i.%i.", Part1, Part2, Part3);
	// Return it to the calling function
	return RetIP;
}



// This timer is executed every 2 minutes and sends all timedmessages one at a time
forward Timer_TimedMessages();
public Timer_TimedMessages()
{
	// Send the message
	SendClientMessageToAll(COLOR_WHITE, ATimedMessages[LastTimedMessage]);

	// Select the next message
    LastTimedMessage++;

	// Check if the next chosen message exists (the messagenumber is the same as the size of the array of messages)
	if (LastTimedMessage == sizeof(ATimedMessages))
	    LastTimedMessage = 0; // Select the first message again

	return 1;
}

// This timer is executed every 5 minutes and sends the data about a random bonus mission to every trucker
forward ShowRandomBonusMission();
public ShowRandomBonusMission()
{
	// Setup local variables
	new bool:MissionSet = false, Msg1[128], Msg2[128], Msg3[128], lName[50], sName[50], eName[50], tName[100];

	// Check if there is no mission defined yet (only happens when server is restarted), or when the mission has been completed by a player
	if ((RandomBonusMission[RandomLoad] == 0) || (RandomBonusMission[MissionFinished] == true))
	{
		// Keep checking until a valid mission has been set
		while (MissionSet == false)
		{
			// Choose a random LoadID
	        RandomBonusMission[RandomLoad] = random(sizeof(ALoads));

			switch (RandomBonusMission[RandomLoad])
			{
				case 0: MissionSet = false; // If the dummy load has been chosen, allow the while loop to run again to search for another valid load
				default: // If another load has been chosen, check if it is a trucker load
				{
					switch(ALoads[RandomBonusMission[RandomLoad]][PCV_Required]) // Check the class & vehicle variable
					{
					    case PCV_TruckerOreTrailer, PCV_TruckerFluidsTrailer, PCV_TruckerCargoTrailer, PCV_TruckerCementTruck, PCV_TruckerNoTrailer:
					    {
					        // Now only the loads for truckers are checked
							MissionSet = true; // The load has been determined now as it's a valid load for truckers only
					    }
					    default: MissionSet = false; // If another class & vehicle has been chosen (mafia or pilot load), search again
					}
				}
			}
		}

		// Find a random startlocation and endlocation from the chosen load
	    RandomBonusMission[RandomStartLoc] = Product_GetRandomStartLoc(RandomBonusMission[RandomLoad]);
	    RandomBonusMission[RandomEndLoc] = Product_GetRandomEndLoc(RandomBonusMission[RandomLoad]);
		// Set the new random mission as not-completed yet
		RandomBonusMission[MissionFinished] = false;
	}

	// Get the names of the load, start-location and end-location
	format(lName, sizeof(lName), ALoads[RandomBonusMission[RandomLoad]][LoadName]);
	format(sName, sizeof(sName), ALocations[RandomBonusMission[RandomStartLoc]][LocationName]);
	format(eName, sizeof(eName), ALocations[RandomBonusMission[RandomEndLoc]][LocationName]);
	// Setup the name of the vehicle you'll need for this mission
	switch(ALoads[RandomBonusMission[RandomLoad]][PCV_Required])
	{
		case PCV_TruckerOreTrailer: format(tName, sizeof(tName), "truck with ore trailer");
		case PCV_TruckerFluidsTrailer: format(tName, sizeof(tName), "truck with fluids trailer");
		case PCV_TruckerCargoTrailer: format(tName, sizeof(tName), "truck with cargo trailer");
		case PCV_TruckerCementTruck: format(tName, sizeof(tName), "cement truck");
		case PCV_TruckerNoTrailer: format(tName, sizeof(tName), "Flatbed or DFT-30");
	}

	// Construct the messages that describe the mission
	format(Msg1, sizeof(Msg1), "{00BBFF}Bonus mission: transport {FFBB00}%s", lName);
	format(Msg2, sizeof(Msg2), "{00BBFF}from {FFBB00}%s{00BBFF} to {FFBB00}%s", sName, eName);
	format(Msg3, sizeof(Msg3), "{00BBFF}You'll need a {FFBB00}%s{00BBFF} to complete this mission", tName);

	// Now send the data about the random mission to all truckers
	for (new playerid; playerid < MAX_PLAYERS; playerid++)
	{
		// Check if this player is online
		if (APlayerData[playerid][LoggedIn] == true)
		{
			// Check if this player is a trucker
			if (APlayerData[playerid][PlayerClass] == ClassTruckDriver)
			{
				SendClientMessage(playerid, COLOR_WHITE, Msg1);
				SendClientMessage(playerid, COLOR_WHITE, Msg2);
				SendClientMessage(playerid, COLOR_WHITE, Msg3);
			}
		}
	}

	return 1;
}



// This function displays the main bank menu dialog
ShowBankMenu(playerid)
{
	// Setup local variables
	new BankOptions[256], BankTitle[128];

    // Construct the title of the bank dialog
	format(BankTitle, sizeof(BankTitle), "{FFFFFF}Your bank funds: {00FF00}$%i", APlayerData[playerid][BankMoney]);

    // Construct the options of the bank dialog
	format(BankOptions, sizeof(BankOptions), "{00FF00}Deposit money\n");
	format(BankOptions, sizeof(BankOptions), "%s{00FF00}Withdraw money\n", BankOptions);
	format(BankOptions, sizeof(BankOptions), "%s{00FF00}Transfer money\n", BankOptions);
	format(BankOptions, sizeof(BankOptions), "%s{00FF00}Cancel bank account\n", BankOptions);

	// Show the bank dialog
	ShowPlayerDialog(playerid, DialogBankOptions, DIALOG_STYLE_LIST, BankTitle, BankOptions, TXT_DialogButtonSelect, TXT_DialogButtonCancel);
}

// This timer increases the variable "CurrentIntrestTime" every hour
forward Bank_IntrestTimer();
public Bank_IntrestTimer()
{
	// Setup local variables
	new IntrestAmount, Msg[128];

	// Increase the variable by one
    CurrentIntrestTime++;

	// And save it to the file
	IntrestTime_Save();

	// Process intrest for all online player with a bank account if intrest is enabled
	if (IntrestEnabled == true)
	{
		// Loop through all players
		for (new playerid; playerid < MAX_PLAYERS; playerid++)
		{
			// Check if this player is logged in
			if (APlayerData[playerid][LoggedIn] == true)
			{
				// Check if this player has a bank account (just check if there is money in the bank, it's useless to process the intrest for
				// a bank account which doesn't have any money in it, and having BankMoney also means the player has a bank account)
				if (APlayerData[playerid][BankMoney] > 0)
				{
					// Calculate the intrest
					IntrestAmount = floatround(floatmul(APlayerData[playerid][BankMoney], BankIntrest), floatround_floor);
					// Add intrest to this player's bank account
					APlayerData[playerid][BankMoney] = APlayerData[playerid][BankMoney] + IntrestAmount;
					// Save the CurrentIntrestTime for this bank account
					APlayerData[playerid][LastIntrestTime] = CurrentIntrestTime;
					// Save the player's bank account
					BankFile_Save(playerid);

					// Inform the player that he received intrest on his bank account and how much
					format(Msg, sizeof(Msg), "{00BBFF}You've received {FFBB00}$%i{00BBFF} intrest on your bank account", IntrestAmount);
					SendClientMessage(playerid, COLOR_WHITE, Msg);
				}
			}
		}
	}
}



// This function creates a list of help-items, for which the player can get information about it
HelpList_Create(playerid)
{
	// Setup local variables
	new HelpList[1000], DialogTitle[128];

	// Construct the title for the dialog (to include a page number)
	format(DialogTitle, sizeof(DialogTitle), "Here you can get help about any of these items:");

    // Add all help-topics to the list
	for (new i; i < sizeof(AHelpTopics); i++)
	{
		format(HelpList, sizeof(HelpList), "%s%s\n", HelpList, AHelpTopics[i]);
	}

	// Show the commands in a dialog
	ShowPlayerDialog(playerid, DialogHelpItemChosen, DIALOG_STYLE_LIST, DialogTitle, HelpList, TXT_DialogButtonSelect, TXT_DialogButtonCancel);

	return 1;
}

// Timed Kick/Ban to make sure the player gets messages
forward TimedKick(playerid);
public TimedKick(playerid)
{
    Kick(playerid);
    return 1;
}

forward TimedBanEx(playerid, reason[]);
public TimedBanEx(playerid, reason[])
{
    BanEx(playerid, reason);
    return 1;
}

forward TimedDestroyVehicle(vid);
public TimedDestroyVehicle(vid)
{
	new engine, lights, alarm, doors, bonnet, boot, objective;
    GetVehicleParamsEx(vid, engine, lights, alarm, doors, bonnet, boot, objective);
	if(doors == VEHICLE_PARAMS_ON)
	{
		SetVehicleParamsEx(vid, engine, lights, alarm, VEHICLE_PARAMS_OFF, bonnet, boot, objective);
	}
    DestroyVehicle(vid);
    return 1;
}

// This function ejects all player from a specific vehicle
RemoveAllPlayersFromVehicle(vid)
{
	for (new i=0;i<MAX_PLAYERS;i++)
	{
		if ((IsPlayerConnected(i)) && (IsPlayerInVehicle(i,vid)))
		{
			RemovePlayerFromVehicle(i);
		}
	}
}

