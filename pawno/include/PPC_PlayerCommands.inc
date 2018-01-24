// This is just a joke command
COMMAND:me(playerid,params[])
{
	// Setup local variables
	new Msg[128], Message[128], Name[24];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/me", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		if (sscanf(params, "s[128]", Message)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/me <Action>\"");
		else
		{
		    // Check if the player isn't muted
		    if (APlayerData[playerid][Muted] == false)
		    {
				GetPlayerName(playerid, Name, sizeof(Name));
				format(Msg, sizeof(Msg), "* %s %s", Name, Message);
				SendClientMessageToAll(COLOR_YELLOW, Msg);
			}
			else
			    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}You are still muted");
		}
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// This commands allows the player to send a private message to another player
COMMAND:pm(playerid, params[])
{
	// Setup local variables
	new OtherPlayer, Message[128], Msg1[128], Msg2[128], YourName[24], OtherPlayerName[24];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/pm", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		if (sscanf(params, "us[128]", OtherPlayer, Message)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/pm <Player> <Message>\"");
		else
		{
			if (OtherPlayer == playerid)
				return SendClientMessage(playerid, COLOR_WHITE, "{FF0000}You can't send a private message to yourself");

			// Check if that other player is online
			if (IsPlayerConnected(OtherPlayer))
			{
			    // Check if the player isn't muted
			    if (APlayerData[playerid][Muted] == false)
			    {
				    // Get both names
					GetPlayerName(playerid, YourName, sizeof(YourName));
					GetPlayerName(OtherPlayer, OtherPlayerName, sizeof(OtherPlayerName));
				    // Construct the message that is sent to yourself
					format(Msg1, 128, "{808080}PM to %s{FFFFFF}: %s", OtherPlayerName, Message);
					// Construct the message that is sent to the other player
					format(Msg2, 128, "{A0A0A0}PM by %s{FFFFFF}: %s", YourName, Message);
					// Send the messages
					SendClientMessage(playerid, COLOR_WHITE, Msg1);
					SendClientMessage(OtherPlayer, COLOR_WHITE, Msg2);
				}
				else
				    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}You are still muted");
			}
			else
			    SendClientMessage(playerid, COLOR_RED, "Player is not online");
		}
	}
	else
	 return 0;

	// Let the server know that this was a valid command
	return 1;
}


// Lets the player add new houses
COMMAND:createhouse(playerid, params[])
{
	// Setup local variables
	new HPrice, MaxLevel, HouseID;

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/createhouse", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 5
		if (APlayerData[playerid][PlayerLevel] >= 5)
		{
			// Check if the player isn't inside a vehicle
			if (GetPlayerVehicleSeat(playerid) == -1)
			{
				if (sscanf(params, "ii", HPrice, MaxLevel)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/createhouse <Price> <Max Level (1-10)>\"");
				else
				{
					// Check if the player entered a proper maxlevel
					if ((MaxLevel >= 1) && (MaxLevel <= 10))
					{
						// Find the first free HouseID
						for (HouseID = 1; HouseID < MAX_HOUSES; HouseID++)
							if (AHouseData[HouseID][PickupID] == 0) // Check if an empty house-index has been found (PickupID is 0)
							    break; // Stop searching, the first free HouseID has been found now

					    // Check if the house-limit hasn't been reached yet
						if (HouseID < MAX_HOUSES)
						{
						    // Setup some local variables
							new Float:x, Float:y, Float:z, Msg[128];
							// Get the player's position
							GetPlayerPos(playerid, x, y, z);
							// Set some default data
							AHouseData[HouseID][HouseX] = x;
							AHouseData[HouseID][HouseY] = y;
							AHouseData[HouseID][HouseZ] = z;
							AHouseData[HouseID][HouseLevel] = 1;
							AHouseData[HouseID][HouseMaxLevel] = MaxLevel;
							AHouseData[HouseID][HousePrice] = HPrice;
							AHouseData[HouseID][Owned] = false;

							// Add the pickup and 3DText at the location of the house-entrance (where the player is standing when he creates the house)
							House_CreateEntrance(HouseID);

							// Save the house
							HouseFile_Save(HouseID);

							// Inform the player that he created a new house
							format(Msg, 128, "{00FF00}You've succesfully created house {FF00FF}%i{00FF00}", HouseID);
							SendClientMessage(playerid, COLOR_WHITE, Msg);
						}
						else
						    SendClientMessage(playerid, COLOR_RED, "The maximum amount of houses has been reached");
					}
					else
					    SendClientMessage(playerid, COLOR_RED, "You have to use a max-level from 1 to 10");
				}
			}
			else
			    SendClientMessage(playerid, COLOR_RED, "You can't be inside a vehicle to create a house");
		}
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// This command lets the player delete a house
COMMAND:delhouse(playerid, params[])
{
	// Setup local variables
	new file[100], Msg[128];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/delhouse", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 5
		if (APlayerData[playerid][PlayerLevel] >= 5)
		{
			// Make sure the player isn't inside a vehicle
			if (GetPlayerVehicleID(playerid) == 0)
			{
				// Loop through all player-owned houses
				for (new HouseID = 1; HouseID < MAX_HOUSES; HouseID++)
				{
					// Check if the house exists
					if (AHouseData[HouseID][PickupID] != 0)
					{
						// Check if the house has no owner
						if (AHouseData[HouseID][Owned] == false)
						{
							// Check if the player is in range of the house-pickup
							if (IsPlayerInRangeOfPoint(playerid, 2.5, AHouseData[HouseID][HouseX], AHouseData[HouseID][HouseY], AHouseData[HouseID][HouseZ]))
							{
								// Clear all data of the house
	                            AHouseData[HouseID][HouseName] = 0;
	                            AHouseData[HouseID][Insurance] = 0;
	                            AHouseData[HouseID][HouseX] = 0.0;
	                            AHouseData[HouseID][HouseY] = 0.0;
	                            AHouseData[HouseID][HouseZ] = 0.0;
	                            AHouseData[HouseID][HouseLevel] = 0;
	                            AHouseData[HouseID][HouseMaxLevel] = 0;
	                            AHouseData[HouseID][HousePrice] = 0;
	                            AHouseData[HouseID][Owned] = false;
	                            AHouseData[HouseID][Owner] = 0;
	                            AHouseData[HouseID][HouseName] = 0;
	                            AHouseData[HouseID][HouseName] = 0;
	                            AHouseData[HouseID][HouseName] = 0;
								// Destroy the mapicon, 3DText and pickup for the house
								DestroyDynamicPickup(AHouseData[HouseID][PickupID]);
								DestroyDynamicMapIcon(AHouseData[HouseID][MapIconID]);
								DestroyDynamic3DTextLabel(AHouseData[HouseID][DoorText]);
	                            AHouseData[HouseID][PickupID] = 0;
	                            AHouseData[HouseID][MapIconID] = 0;

								// Delete the House-file
								format(file, sizeof(file), HouseFile, HouseID); // Construct the complete filename for this house-file
								if (fexist(file)) // Make sure the file exists
									fremove(file); // Delete the file

								// Also let the player know he deleted the house
								format(Msg, 128, "{00FF00}You have deleted the house with ID: {FFFF00}%i", HouseID);
								SendClientMessage(playerid, COLOR_WHITE, Msg);

								// Exit the function
								return 1;
							}
						}
					}
				}

				// There was no house in range, so let the player know about it
				SendClientMessage(playerid, COLOR_WHITE, "{FF0000}No house in range to delete");
			}
			else
			    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}You can't be inside a vehicle to delete a house");
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// This command lets the player buy a house when he's standing in range of a house that isn't owned yet
COMMAND:buyhouse(playerid, params[])
{
	// Setup local variables
	new Msg[128];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/buyhouse", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Make sure the player isn't inside a vehicle
		if (GetPlayerVehicleID(playerid) == 0)
		{
			// Check if the player is near a house-pickup
			for (new i = 1; i < sizeof(AHouseData); i++)
			{
				// Check if this house is created (it would have a valid pickup in front of the door)
				if (AHouseData[i][PickupID] != 0)
				{
					// Check if the player is in range of the house-pickup
					if (IsPlayerInRangeOfPoint(playerid, 2.5, AHouseData[i][HouseX], AHouseData[i][HouseY], AHouseData[i][HouseZ]))
					{
					    // Check if the house isn't owned yet
					    if (AHouseData[i][Owned] == false)
					    {
					        // Check if the player can afford this house
					        if (APlayerData[playerid][PlayerMoney] >= AHouseData[i][HousePrice])
					            House_SetOwner(playerid, i); // Give ownership of the house to the player
					        else
					            SendClientMessage(playerid, COLOR_RED, "You cannot afford this house"); // The player cannot afford this house
					    }
					    else
					    {
					        // Let the player know that this house is already owned by a player
							format(Msg, 128, "{FF0000}This house is already owned by {00FF00}%s{FFFFFF}", AHouseData[i][Owner]);
							SendClientMessage(playerid, COLOR_WHITE, Msg);
					    }

						// The player was in range of a house-pickup, so stop searching for the other house pickups
					    return 1;
					}
				}
			}

			// All houses have been processed, but the player wasn't in range of any house-pickup, let him know about it
			SendClientMessage(playerid, COLOR_RED, "To buy a house, you have to be near a house-pickup");
		}
		else
		    SendClientMessage(playerid, COLOR_RED, "You can't buy a house when you're inside a vehicle");
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// This command lets the player enter the house/business if he's the owner
COMMAND:enter(playerid, params[])
{
	// Setup local variables
	new HouseID, hLevel, BusID, BusType;

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/enter", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Make sure the player isn't inside a vehicle
		if (GetPlayerVehicleID(playerid) == 0)
		{
			// Loop through all houses
			for (HouseID = 1; HouseID < MAX_HOUSES; HouseID++)
			{
				// Check if this house exists
				if (AHouseData[HouseID][PickupID] != 0)
				{
					// Check if the player is in range of the house-pickup
					if (IsPlayerInRangeOfPoint(playerid, 2.5, AHouseData[HouseID][HouseX], AHouseData[HouseID][HouseY], AHouseData[HouseID][HouseZ]))
					{
						// Check if the house is closed to the public
						if (AHouseData[HouseID][HouseOpened] == false)
						{
							// The house isn't open to the public, so keep anyone out who isn't the owner of the house
							if (House_PlayerIsOwner(playerid, HouseID) == 0)
							{
							    // Let the player know that this house isn't open to the public and he can't enter it
								SendClientMessage(playerid, COLOR_WHITE, "{FF0000}This house isn't open to the public, you can't enter it");
							    return 1;
							}
						}

						// The house is open to the public, or the player trying to enter is the owner, so let the player inside the house

					    // Get the level of the house
						hLevel = AHouseData[HouseID][HouseLevel];
						// Set the worldid so other players cannot see him anymore
						SetPlayerVirtualWorld(playerid, 5000 + HouseID);
						// Set the player inside the interior of the house
						SetPlayerInterior(playerid, AHouseInteriors[hLevel][InteriorID]);
						// Set the position of the player at the spawn-location of the house's interior
						SetPlayerPos(playerid, AHouseInteriors[hLevel][IntX], AHouseInteriors[hLevel][IntY], AHouseInteriors[hLevel][IntZ]);
						// Also set a tracking-variable to enable /housemenu to track in which house the player is
						APlayerData[playerid][CurrentHouse] = HouseID;
						// Also let the player know he can use /housemenu to upgrade/exit his house
						SendClientMessage(playerid, COLOR_WHITE, "{00FF00}Use {FFFF00}/housemenu{00FF00} to change options for your house");

						// Exit the function
						return 1;
					}
				}
			}

			// Loop through all player-owned businesses
			for (new i; i < MAX_BUSINESSPERPLAYER; i++)
			{
			    // Get the business-id at the selected slot from the player
			    BusID = APlayerData[playerid][Business][i];

				// Check if the player has owned a business in this slot
				if (BusID != 0)
				{
					// Check if the player is in range of the business-pickup
					if (IsPlayerInRangeOfPoint(playerid, 2.5, ABusinessData[BusID][BusinessX], ABusinessData[BusID][BusinessY], ABusinessData[BusID][BusinessZ]))
					{
					    // Get the business-type
					    BusType = ABusinessData[BusID][BusinessType];
						// Set the worldid so other players cannot see him anymore
						SetPlayerVirtualWorld(playerid, 1000 + playerid);
						// Set the player inside the interior of the business
						SetPlayerInterior(playerid, ABusinessInteriors[BusType][InteriorID]);

						// Set the position of the player at the spawn-location of the business's interior
						SetPlayerPos(playerid, ABusinessInteriors[BusType][IntX], ABusinessInteriors[BusType][IntY], ABusinessInteriors[BusType][IntZ]);
						// Also set a tracking-variable to enable /busmenu to track in which business the player is
						APlayerData[playerid][CurrentBusiness] = BusID;
						// Also let the player know he can use /busmenu to upgrade/exit his business
						SendClientMessage(playerid, COLOR_WHITE, "{00FF00}Use {FFFF00}/busmenu{00FF00} to change options for your business");

						// Exit the function
						return 1;
					}
				}
			}
		}
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// This command opens a menu when you're inside your house to allow to access the options of your house
COMMAND:housemenu(playerid, params[])
{
	// Setup local variables
	new OptionsList[200], DialogTitle[200];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/housemenu", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player is inside a house
		if (APlayerData[playerid][CurrentHouse] != 0)
		{
			format(DialogTitle, sizeof(DialogTitle), "Select option for %s", AHouseData[APlayerData[playerid][CurrentHouse]][HouseName]);

			format(OptionsList, sizeof(OptionsList), "%sChange house-name\n", OptionsList);
			format(OptionsList, sizeof(OptionsList), "%sUpgrade house\n", OptionsList);
			format(OptionsList, sizeof(OptionsList), "%sBuy house-car\n", OptionsList);
			format(OptionsList, sizeof(OptionsList), "%sBuy house-car insurance\n", OptionsList);
			format(OptionsList, sizeof(OptionsList), "%sSell house-car\n", OptionsList);
			format(OptionsList, sizeof(OptionsList), "%sSell house\n", OptionsList);
			format(OptionsList, sizeof(OptionsList), "%sOpen house to the public\n", OptionsList);
			format(OptionsList, sizeof(OptionsList), "%sClose house to the public\n", OptionsList);
			format(OptionsList, sizeof(OptionsList), "%sExit house\n", OptionsList);
			// Show the housemenu
			ShowPlayerDialog(playerid, DialogHouseMenu, DIALOG_STYLE_LIST, DialogTitle, OptionsList, "Select", "Cancel");
		}
		else
		    SendClientMessage(playerid, COLOR_RED, "You're not inside a house");
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// This command allows you to port a vehicle from your house to your location
COMMAND:getcar(playerid, params[])
{
	// Setup local variables
	new HouseList[1000];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/getcar", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player is not jailed
		if (APlayerData[playerid][PlayerJailed] == 0)
		{
			// Check if the player is not inside a vehicle
			if (GetPlayerVehicleID(playerid) == 0)
			{
				// Ask to which house the player wants to add his vehicle
				for (new i; i < MAX_HOUSESPERPLAYER; i++)
				{
					// Check if this houseindex is occupied
					if (APlayerData[playerid][Houses][i] != 0)
						format(HouseList, 1000, "%s{00FF00}%s{FFFFFF}\n", HouseList, AHouseData[APlayerData[playerid][Houses][i]][HouseName]);
					else
						format(HouseList, 1000, "%s{FFFFFF}%s{FFFFFF}\n", HouseList, "Empty house-slot");
				}
				ShowPlayerDialog(playerid, DialogGetCarSelectHouse, DIALOG_STYLE_LIST, "Choose the house to get a car from:", HouseList, "Select", "Cancel");
			}
			else
				SendClientMessage(playerid, COLOR_RED, "You need to be on-foot to port a vehicle to your location");
		}
		else
		    SendClientMessage(playerid, COLOR_RED, "You cannot use /getcar when you're in jail");
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// This command checks if the player is inside a vehicle that he owns and if he's in range of the house where the vehicle is assigned to
COMMAND:park(playerid, params[])
{
	// Setup local variables
	new Float:x, Float:y, Float:z, Float:rot, vid, HouseID, Msg[128];
	new engine,lights,alarm,doors,bonnet,boot,objective;

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/park", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player is inside a vehicle (he must be the driver)
		if (GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
		{
			// Get the vehicle-id
			vid = GetPlayerVehicleID(playerid);
			// Get the HouseID to which this vehicle belongs
			HouseID = AVehicleData[vid][BelongsToHouse];

			// Check if the vehicle is owned (owner-check is not really required, as another player would get kicked out very fast)
			// AND it must belong to a house that the player owns
			if ((AVehicleData[vid][Owned] == true) && (HouseID != 0))
			{
				// Check if the vehicle is in range of the house-entrance (you cannot park a vehicle further away from your house than 150m)
				if (IsPlayerInRangeOfPoint(playerid, ParkRange, AHouseData[HouseID][HouseX], AHouseData[HouseID][HouseY], AHouseData[HouseID][HouseZ]))
				{
					// Get the player's position and angle
					GetVehiclePos(vid, x, y, z);
					GetVehicleZAngle(vid, rot);
					// Save those values for the vehicle
					AVehicleData[vid][SpawnX] = x;
					AVehicleData[vid][SpawnY] = y;
					AVehicleData[vid][SpawnZ] = z;
					AVehicleData[vid][SpawnRot] = rot;

					// Find the vehicle in the player's houses
					for (new i; i < MAX_HOUSESPERPLAYER; i++)
					{
						// Get the HouseID of the current house
					    HouseID = APlayerData[playerid][Houses][i];

						// Loop through all carslots of this house to find the vehicle-id
						for (new CarSlot; CarSlot < 10; CarSlot++)
						{
							// Check if this carslot holds the same vehicle-id
						    if (AHouseData[HouseID][VehicleIDs][CarSlot] == vid)
						    {
			                    House_ReplaceVehicle(HouseID, CarSlot); // Re-create the vehicle at the same spot the player wants to park his vehicle
			                    PutPlayerInVehicle(playerid, AHouseData[HouseID][VehicleIDs][CarSlot], 0);
								// Turn on the engine
								GetVehicleParamsEx(AHouseData[HouseID][VehicleIDs][CarSlot], engine, lights, alarm, doors, bonnet, boot, objective);
								SetVehicleParamsEx(AHouseData[HouseID][VehicleIDs][CarSlot], 1, lights, alarm, doors, bonnet, boot, objective);
			                    break; // Stop the for-loop
							}
						}
					}

					// Let the player know he parked his vehicle
					SendClientMessage(playerid, COLOR_GREEN, "You've parked your vehicle");

					// Save the player-file (and his houses)
					PlayerFile_Save(playerid);
				}
				else
				{
				    format(Msg, 128, "{FF0000}You need to be within %.0fm of your house to park this vehicle", ParkRange);
				    SendClientMessage(playerid, COLOR_WHITE, Msg);
				}
			}
			else
			    SendClientMessage(playerid, COLOR_RED, "You cannot park a vehicle that's not owned by you");
		}
		else
		    SendClientMessage(playerid, COLOR_RED, "You must be driving a vehicle you own to park it");
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}



// Lets the player rent a vehicle
COMMAND:rentcar(playerid, params[])
{
	// Setup local variables
	new VehicleClassList[1000];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/rentcar", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Make sure the player isn't inside a vehicle
		if (GetPlayerVehicleID(playerid) == 0)
		{
			// Check if the player is near a cardealerpickup
			for (new i; i < sizeof(ACarDealerPickups); i++)
			{
				// Check if a valid cardealerpickup is found
				if (ACarDealerPickups[i][PickupID] != 0)
				{
					// Check if the player is in range of the cardealerpickup
					if(IsPlayerInRangeOfPoint(playerid, 2.5, ACarDealerPickups[i][pux], ACarDealerPickups[i][puy], ACarDealerPickups[i][puz]))
					{
					    // Let the player choose a vehicle-class
						format(VehicleClassList, 1000, "%s{00FF00}%s{FFFFFF}\n", VehicleClassList, "Bikes");
						format(VehicleClassList, 1000, "%s{40FF00}%s{FFFFFF}\n", VehicleClassList, "Boats");
						format(VehicleClassList, 1000, "%s{80FF00}%s{FFFFFF}\n", VehicleClassList, "Convertibles");
						format(VehicleClassList, 1000, "%s{B0FF00}%s{FFFFFF}\n", VehicleClassList, "Helicopters");
						format(VehicleClassList, 1000, "%s{FFFF00}%s{FFFFFF}\n", VehicleClassList, "Industrial vehicles");
						format(VehicleClassList, 1000, "%s{B0FF40}%s{FFFFFF}\n", VehicleClassList, "Low-riders");
						format(VehicleClassList, 1000, "%s{80FF80}%s{FFFFFF}\n", VehicleClassList, "Off-Road vehicles");
						format(VehicleClassList, 1000, "%s{40FFB0}%s{FFFFFF}\n", VehicleClassList, "Planes");
						format(VehicleClassList, 1000, "%s{00FFFF}%s{FFFFFF}\n", VehicleClassList, "Public Service vehicles");
						format(VehicleClassList, 1000, "%s{00B0FF}%s{FFFFFF}\n", VehicleClassList, "RC vehicles");
						format(VehicleClassList, 1000, "%s{0080FF}%s{FFFFFF}\n", VehicleClassList, "Saloon vehicles");
						format(VehicleClassList, 1000, "%s{0040FF}%s{FFFFFF}\n", VehicleClassList, "Sport vehicles");
						format(VehicleClassList, 1000, "%s{0000FF}%s{FFFFFF}\n", VehicleClassList, "Station wagons");
						format(VehicleClassList, 1000, "%s{4000FF}%s{FFFFFF}\n", VehicleClassList, "Trailers");
						format(VehicleClassList, 1000, "%s{8000FF}%s{FFFFFF}\n", VehicleClassList, "Unique vehicles");
						// Ask which vehicle class the player wants to see to buy a vehicle
						ShowPlayerDialog(playerid, DialogRentCarClass, DIALOG_STYLE_LIST, "Select vehicle class:", VehicleClassList, "Select", "Cancel");
						// Exit the function
						return 1;
					}
				}
			}

			// The player wasn't in range of a cardealer
			SendClientMessage(playerid, COLOR_RED, "You cannot rent a vehicle when you're not close to a cardealer");
		}
		else
		    SendClientMessage(playerid, COLOR_RED, "You cannot rent a vehicle when you're inside a car");
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}



// This command teleports you to your selected house
COMMAND:gohome(playerid, params[])
{
	// Setup local variables
	new HouseList[1000];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/gohome", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player has a wanted level of less than 3
		if (GetPlayerWantedLevel(playerid) < 3)
		{
			// Check if the player is not jailed
			if (APlayerData[playerid][PlayerJailed] == 0)
			{
				// Check if the player is not inside a vehicle
				if (GetPlayerVehicleID(playerid) == 0)
				{
					// Ask to which house the player wants to add his vehicle
					for (new i; i < MAX_HOUSESPERPLAYER; i++)
					{
						// Check if this houseindex is occupied
						if (APlayerData[playerid][Houses][i] != 0)
							format(HouseList, 1000, "%s{00FF00}%s{FFFFFF}\n", HouseList, AHouseData[APlayerData[playerid][Houses][i]][HouseName]);
						else
							format(HouseList, 1000, "%s{FFFFFF}%s{FFFFFF}\n", HouseList, "Empty house-slot");
					}
					ShowPlayerDialog(playerid, DialogGoHome, DIALOG_STYLE_LIST, "Choose the house to go to:", HouseList, "Select", "Cancel");
				}
				else
					SendClientMessage(playerid, COLOR_RED, "You need to be on-foot to port to your house");
			}
			else
			    SendClientMessage(playerid, COLOR_RED, "You cannot use /gohome when you're in jail");
		}
		else
			SendClientMessage(playerid, COLOR_WHITE, "{FF0000}You cannot use /gohome when you're wanted");
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Detaches the trailer from the vehicle
COMMAND:detach(playerid, params[])
{
	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/detach", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Detach the trailer from the vehicle
		DetachTrailerFromVehicle(GetPlayerVehicleID(playerid));
		// Send the player a message that the trailer has been detached
		SendClientMessage(playerid, COLOR_BLUE, "Your trailer is detached");
	}
	else
		return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Puts the player's vehicle back onto his wheels
COMMAND:flip(playerid, params[])
{
	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/flip", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		if (GetPlayerState(playerid) == PLAYER_STATE_DRIVER) {
			// Setup some local variables
			new Float:x = 0.0, Float:y = 0.0, Float:z = 0.0;
			// Check if the player is inside a vehicle
			if(IsPlayerInAnyVehicle(playerid))
				SetCameraBehindPlayer(playerid);
			// Get the player's position
			GetPlayerPos(playerid, x, y, z);
			// Set the vehicle on the player's coordinates
			SetVehiclePos(GetPlayerVehicleID(playerid), x, y, z);
			// Let the vehicle point north
			SetVehicleZAngle(GetPlayerVehicleID(playerid), 0.0);
		}
		else
			return SendClientMessage(playerid, COLOR_RED, "You must be the driver of a vehicle to use this command");
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Repairs the player's vehicle
COMMAND:repair(playerid, params[])
{
	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/repair", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 1
		if (APlayerData[playerid][PlayerLevel] >= 1)
		{
		    // Check if the player is inside a vehicle
			if(!IsPlayerInAnyVehicle(playerid))
				return SendClientMessage(playerid, COLOR_RED, "You are not in a vehicle!"); // Let the player know he's not inside a vehicle and let the server know that this was a valid command
			// Fully repair the vehicle (damage value and bodywork)
			RepairVehicle(GetPlayerVehicleID(playerid));
			// Send the player a message to inform him that his vehicle has been repaired
			SendClientMessage(playerid, COLOR_GREEN, "Your vehicle has been successfully repaired!");
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Repairs all vehicles
COMMAND:repairall(playerid, params[])
{
	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/repairall", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 3
		if (APlayerData[playerid][PlayerLevel] >= 3)
		{
			// Loop through all vehicles
			for (new i; i < 2000; i++)
				RepairVehicle(i); // Fully repair the vehicle (damage value and bodywork)

			// Send all players a message to inform them that all vehicles have been repaired
			SendClientMessageToAll(COLOR_GREEN, "All vehicles have been successfully repaired!");
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Heals all players
COMMAND:healall(playerid, params[])
{
	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/healall", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 3
		if (APlayerData[playerid][PlayerLevel] >= 3)
		{
			// Loop through all players
			for (new i; i < MAX_PLAYERS; i++)
				if (IsPlayerConnected(i)) // Check if the player is connected
				    if (IsPlayerInAnyVehicle(i) == 0) // Check if the player isn't inside a vehicle
				        SetPlayerHealth(i, 100.0); // Heal the player

			// Send all players a message to inform them that all players have been healed
			SendClientMessageToAll(COLOR_GREEN, "All players have been healed!");
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Lets the player choose another class
COMMAND:reclass(playerid, params[])
{
	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/reclass", params);
	
	if (GetPlayerState(playerid) == PLAYER_STATE_WASTED || GetPlayerState(playerid) == PLAYER_STATE_NONE)
        return 0;
	
	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player has a wanted level of less than 3
		if (GetPlayerWantedLevel(playerid) < 3)
		{
			// Check if the player isn't in jail
			if (APlayerData[playerid][PlayerJailed] == 0)
			{
			    // Force the player back into class-selection
				ForceClassSelection(playerid);
				// Kill the player (required after ForceClassSelection)
				SetPlayerHealth(playerid, 0.0);
			}
			else
			    SendClientMessage(playerid, COLOR_RED, "You cannot use /reclass when you're in jail");
		}
		else
		    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}You cannot use /reclass when you're wanted");
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Starts a job
COMMAND:work(playerid, params[])
{
	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/work", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// First check if the player already has a job
		if (APlayerData[playerid][JobStarted] == false)
		{
		    // Check the player's class
			switch (APlayerData[playerid][PlayerClass])
			{
				case ClassTruckDriver:
				{
					// Get the id of the convoy (if the player is in a convoy)
					new Convoy = APlayerData[playerid][ConvoyID];

					// Check if the player is part of a convoy AND is not the leader
					if ((APlayerData[playerid][InConvoy] == true) && (AConvoys[Convoy][Members][0] != playerid))
					{
						// Let the player know he's not the leader of his convoy and cannot start a job
						SendClientMessage(playerid, COLOR_RED, "You're not the leader of your convoy, you cannot start a job");
						// Exit the function
						return 1;
					}

					// A convoy-leader proceeds here, and also a normal player (no convoy-member)

					// Check if the player is the driver of a vehicle
					if (GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
					{
						// Check if the player is inside a valid trucking vehicle
						switch (GetVehicleModel(GetPlayerVehicleID(playerid)))
						{
							case VehicleFlatbed, VehicleDFT30, VehicleCementTruck: // Flatbed, DFT-30, CementTruck
								if (APlayerData[playerid][TruckerLicense] == 1) // Check if the player has acquired a truckers license
									ShowPlayerDialog(playerid, DialogTruckerJobMethod, DIALOG_STYLE_LIST, "Select method:", "Setup your own load and route\r\nAuto assigned load", "Select", "Cancel");
								else
								    Trucker_StartRandomJob(playerid); // Start a random job

							case VehicleLineRunner, VehicleTanker, VehicleRoadTrain: // Player is driving a truck which needs a trailer
								if(IsTrailerAttachedToVehicle(GetPlayerVehicleID(playerid))) // Check if there is a trailer attached
									if (APlayerData[playerid][TruckerLicense] == 1) // Check if the player has acquired a truckers license
										ShowPlayerDialog(playerid, DialogTruckerJobMethod, DIALOG_STYLE_LIST, "Select method:", "Setup your own load and route\r\nAuto assigned load", "Select", "Cancel");
									else
									    Trucker_StartRandomJob(playerid); // Start a random job
								else
								    SendClientMessage(playerid, COLOR_RED, "You need a trailer to start a job");

							default: SendClientMessage(playerid, COLOR_RED, "You need to be the driver of a trucking vehicle to start a job");
						}
					}
					else
		                SendClientMessage(playerid, COLOR_RED, "You need to be the driver of a trucking vehicle to start a job");
				}
				case ClassBusDriver:
				{
					// Check if the player is the driver of a vehicle
					if (GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
						if (GetVehicleModel(GetPlayerVehicleID(playerid)) == VehicleCoach) // Check if the player is inside a valid busdriver vehicle
							if (APlayerData[playerid][BusLicense] == 1) // Check if the player has acquired a busdriver license
								ShowPlayerDialog(playerid, DialogBusJobMethod, DIALOG_STYLE_LIST, "Select method:", "Choose your own busroute\r\nAuto assigned busroute", "Select", "Cancel");
							else
								BusDriver_StartJob(playerid, random(sizeof(ABusRoutes))); // Start a random job
						else
			                SendClientMessage(playerid, COLOR_RED, "You need to be the driver of a bus to start a job");
					else
		                SendClientMessage(playerid, COLOR_RED, "You need to be the driver of a bus to start a job");
				}
				case ClassPilot:
				{
					// Check if the player is the driver of a vehicle
					if (GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
					{
						// Check if the player is inside a valid piloting vehicle
						switch (GetVehicleModel(GetPlayerVehicleID(playerid)))
						{
							case VehicleShamal, VehicleNevada, VehicleMaverick, VehicleCargobob: // Plane (Shamal), Plane (Nevada), helicopter (Maverick)
								Pilot_StartRandomJob(playerid); // Start a random piloting job
							default: SendClientMessage(playerid, COLOR_RED, "You need to be the driver of a piloting vehicle to start a job");
						}
					}
					else
		                SendClientMessage(playerid, COLOR_RED, "You need to be the driver of a piloting vehicle to start a job");
				}
				case ClassMafia:
				{
					// Check if the player is the driver of a vehicle
					if (GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
					{
						// Check if the player is inside a valid piloting vehicle
						switch (GetVehicleModel(GetPlayerVehicleID(playerid)))
						{
							case VehicleSandKing, VehicleMoonbeam: // Sangking, Moonbeam
								Mafia_StartRandomJob(playerid); // Start a random mafia job
							default: SendClientMessage(playerid, COLOR_RED, "You need to be the driver of a mafia vehicle to start a job");
						}
					}
					else
		                SendClientMessage(playerid, COLOR_RED, "You need to be the driver of a mafia vehicle to start a job");
				}
				case ClassCourier:
				{
					// Check if the player is the driver of a vehicle
					if (GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
					{
						// Check if the player is inside a valid courier vehicle
						switch (GetVehicleModel(GetPlayerVehicleID(playerid)))
						{
							case VehicleBurrito, VehicleFaggio, VehicleBenson: // Van (Burrito), bike (Faggio)
								Courier_StartJob(playerid); // Start a random courier job
							default: SendClientMessage(playerid, COLOR_RED, "You need to be the driver of a courier vehicle to start a job");
						}
					}
					else
		                SendClientMessage(playerid, COLOR_RED, "You need to be the driver of a courier vehicle to start a job");
				}
				case ClassRoadWorker:
				{
					// Check if the player is the driver of a vehicle
					if (GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
					{
						// Check if the player is inside a valid courier vehicle
						switch (GetVehicleModel(GetPlayerVehicleID(playerid)))
						{
							case VehicleUtilityVan, VehicleTowTruck: // Utility Van, Towtruck
								Roadworker_StartRandomJob(playerid); // Start a random roadworker job
							default: SendClientMessage(playerid, COLOR_RED, "You need to be the driver of a roadworker vehicle to start a job");
						}
					}
					else
		                SendClientMessage(playerid, COLOR_RED, "You need to be the driver of a roadworker vehicle to start a job");
				}
				default: SendClientMessage(playerid, COLOR_RED, "Your class cannot do any jobs");
			}
		}
		else // Send a message to let the player know he already has a job
		    SendClientMessage(playerid, COLOR_RED, "You're already doing a job");
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Stops the current job
COMMAND:stopwork(playerid, params[])
{
	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/stopwork", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
	    // Check if the player has a job started
		if (APlayerData[playerid][JobStarted] == true)
		{
		    // Check the class of the player
			switch (APlayerData[playerid][PlayerClass])
			{
				case ClassTruckDriver: Trucker_EndJob(playerid); // Stop any trucker job
				case ClassBusDriver: BusDriver_EndJob(playerid); // Stop any busdriver job
				case ClassPilot: Pilot_EndJob(playerid); // Stop any pilot job
				case ClassMafia: Mafia_EndJob(playerid); // Stop any mafia job
				case ClassCourier: Courier_EndJob(playerid);
				case ClassRoadWorker: Roadworker_EndJob(playerid);
				default: SendClientMessage(playerid, COLOR_RED, "Your class has no job to end");
			}

			// Inform the player that he failed the mission
			GameTextForPlayer(playerid, "~w~You ~r~failed~w~ your mission. You lost ~y~$1000~w~ to cover expenses.", 5000, 4);
			// Reduce the player's cash by 1000
			RewardPlayer(playerid, -1000, 0);
		}
		else
			SendClientMessage(playerid, COLOR_RED, "You are not doing any job");
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Displays the player's coordinates on the map
COMMAND:loc(playerid, params[])
{
	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/loc", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 3
		if (APlayerData[playerid][PlayerLevel] >= 3)
		{
		    // Setup some local variables
			new Message[200], Float:x, Float:y, Float:z, Float:rot, Interior, World;
			// Get the player's position
			GetPlayerPos(playerid, x, y, z);
			// Check if the player is on foot or in a vehicle
			if (GetPlayerVehicleSeat(playerid) == -1)
				GetPlayerFacingAngle(playerid, rot); // Get the player's angle
			else
			    GetVehicleZAngle(GetPlayerVehicleID(playerid), rot);

			// Get the interior where the player is located
			Interior = GetPlayerInterior(playerid);
			// Get the virtual world of the player
			World = GetPlayerVirtualWorld(playerid);
			// combine the position and angle into a proper message
		    format(Message, sizeof(Message), "Location: X=%4.2f, Y=%4.2f, Z=%4.2f, rotation=%4.2f, interior=%i, world=%i", x, y, z, rot, Interior, World);
		    // Send the message with the coordinates and the angle of the player
			SendClientMessage(playerid, COLOR_RED, Message);
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Lets the player choose where he wants to respawn (costs $200)
COMMAND:gobase(playerid, params[])
{
	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/gobase", params);
	
	if (GetPlayerState(playerid) == PLAYER_STATE_WASTED || GetPlayerState(playerid) == PLAYER_STATE_NONE)
        return 0;
	
	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player has a wanted level of less than 3
		if (GetPlayerWantedLevel(playerid) < 3)
		{
			// Check if the player isn't in jail
			if (APlayerData[playerid][PlayerJailed] == 0)
			{
				if (APlayerData[playerid][JobStarted] == false)
				{
					// Make sure you can't use "/gobase" when you're inside a vehicle (doesn't respawn you at the requested coords
					// and puts a random item (bottle, sigarette, ...) in the player's hands
					if (GetPlayerVehicleID(playerid) == 0)
					{
					    // Create a dialog based on the player's class
						switch (APlayerData[playerid][PlayerClass])
						{
							case ClassTruckDriver: // Ask where the trucker player wants to respawn
								ShowPlayerDialog(playerid, DialogRescue, DIALOG_STYLE_LIST, "Choose trucker spawn point:", "Fallen Tree Depot\r\nFlint Trucking Depot\r\nLVA Freight Depot\r\nDoherty Depot\r\nEl Corona Depot\r\nLas Payasdas Depot\r\nQuarry Top\r\nShady Creek Depot", "Spawn", "Cancel");
							case ClassBusDriver: // Ask where the busdriver wants to respawn
								ShowPlayerDialog(playerid, DialogRescue, DIALOG_STYLE_LIST, "Choose busdriver spawn point:", "Los Santos\r\nSan Fierro\r\nLas Venturas", "Spawn", "Cancel");
							case ClassPilot: // Ask where the pilot wants to respawn
								ShowPlayerDialog(playerid, DialogRescue, DIALOG_STYLE_LIST, "Choose pilot spawn point:", "Los Santos\r\nSan Fierro\r\nLas Venturas", "Spawn", "Cancel");
							case ClassPolice: // Ask where the police player wants to respawn
								ShowPlayerDialog(playerid, DialogRescue, DIALOG_STYLE_LIST, "Choose police spawn point:", "Los Santos\r\nSan Fierro\r\nLas Venturas", "Spawn", "Cancel");
							case ClassCourier: // Ask where the courier player wants to respawn
								ShowPlayerDialog(playerid, DialogRescue, DIALOG_STYLE_LIST, "Choose courier spawn point:", "Los Santos\r\nSan Fierro\r\nLas Venturas", "Spawn", "Cancel");
						}
					}
					else
						SendClientMessage(playerid, COLOR_RED, "You cannot use '/gobase' when you're inside a vehicle"); // "/gobase" doesn't work inside a vehicle
				}
				else
				    SendClientMessage(playerid, COLOR_RED, "You cannot use gobase when doing a job"); // "/gobase" doesn't work during a job
			}
			else
			    SendClientMessage(playerid, COLOR_RED, "You cannot use /gobase when you're in jail");
		}
		else
		    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}You cannot use /gobase when you're wanted");
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Sets the player's wanted level
COMMAND:setwanted(playerid, params[])
{
	new OtherPlayer, Stars;

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/setwanted", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 2
		if (APlayerData[playerid][PlayerLevel] >= 2)
		{
			if (sscanf(params, "ui", OtherPlayer, Stars)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/setwanted <Player> <Stars (0-6)>\"");
			else
				if (IsPlayerConnected(OtherPlayer)) // If the player is a valid playerid (he's connected)
				{
					if ((Stars >= 0) && (Stars <= 6))
					{
						SetPlayerWantedLevel(OtherPlayer, Stars);
						SendClientMessage(OtherPlayer, COLOR_BLUE, "Your wanted level has been set by an admin");
					}
					else
						SendClientMessage(playerid, COLOR_RED, "Invalid wanted level, you can only use 0-6");
				}
				else
				    SendClientMessage(playerid, COLOR_RED, "That player isn't online");
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}



// Lets the player choose a motorcycle to spawn
COMMAND:bike(playerid, params[])
{
	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/bike", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 1
		if (APlayerData[playerid][PlayerLevel] >= 1)
		{
			// Make sure the player isn't inside a vehicle
			if (GetPlayerVehicleID(playerid) == 0)
			{
				// Ask which motorcycle the player wants to have
				ShowPlayerDialog(playerid, DialogBike, DIALOG_STYLE_LIST, "Choose a motorcycle:", "Bike\r\nBMX\r\nMountain Bike\r\nFaggio\r\nPizzaboy\r\nBF-400\r\nNRG-500\r\nPCJ-600\r\nFCR-900\r\nFreeway\r\nWayfarer\r\nSanchez\r\nQuad", "Spawn", "Cancel");
				// Let the server know that this was a valid command
				return 1;
			}
			else
				SendClientMessage(playerid, COLOR_RED, "You must be on foot to use this command");
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}



// Lets the player choose a car to spawn (in a split list which shows only 10 cars at a time)
COMMAND:car(playerid, params[])
{
	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/car", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 1
		if (APlayerData[playerid][PlayerLevel] >= 1)
		{
			// Make sure the player isn't inside a vehicle
			if (GetPlayerVehicleID(playerid) == 0)
				CarList_Create(playerid); // Create a list of cars (only the first 10 cars) and show the dialog so the player can choose a car
			else
				SendClientMessage(playerid, COLOR_RED, "You must be on foot to use this command");
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}



// Lets the player choose a plane to spawn (in a split list which shows only 10 planes at a time)
COMMAND:plane(playerid, params[])
{
	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/plane", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 1
		if (APlayerData[playerid][PlayerLevel] >= 1)
		{
			// Make sure the player isn't inside a vehicle
			if (GetPlayerVehicleID(playerid) == 0)
				PlaneList_Create(playerid); // Create a list of planes (only the first 10 planes) and show the dialog so the player can choose a plane
			else
				SendClientMessage(playerid, COLOR_RED, "You must be on foot to use this command");
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}



// Lets the player choose a trailer to spawn (in a split list which shows only 10 trailers at a time)
COMMAND:trailer(playerid, params[])
{
	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/trailer", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 1
		if (APlayerData[playerid][PlayerLevel] >= 1)
		{
			// Make sure the player isn't inside a vehicle
			if (GetPlayerVehicleID(playerid) == 0)
				TrailerList_Create(playerid); // Create a list of trailers (only the first 10 trailers) and show the dialog so the player can choose a trailer
			else
				SendClientMessage(playerid, COLOR_RED, "You must be on foot to use this command");
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Lets the player choose a boat to spawn
COMMAND:boat(playerid, params[])
{
	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/boat", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 1
		if (APlayerData[playerid][PlayerLevel] >= 1)
		{
			// Make sure the player isn't inside a vehicle
			if (GetPlayerVehicleID(playerid) == 0)
			{
				// Ask which motorcycle the player wants to have
				ShowPlayerDialog(playerid, DialogBoat, DIALOG_STYLE_LIST, "Choose a boat:", "Coastguard\nDinghy\nJetmax\nLaunch\nMarquis\nPredator\nReefer\nSpeeder\nSquallo\nTropic", "Spawn", "Cancel");
				// Let the server know that this was a valid command
				return 1;
			}
			else
				SendClientMessage(playerid, COLOR_RED, "You must be on foot to use this command");
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}



// Jail a player
COMMAND:jail(playerid, params[])
{
	new PlayerToJail, JailTime, Reason[128], Msg[128], Name[24], AdminName[24];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/jail", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 1
		if (APlayerData[playerid][PlayerLevel] >= 1)
		{
			if (sscanf(params, "uis[128]", PlayerToJail, JailTime, Reason)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/jail <Player> <Duration (in Seconds)> <Reason>\"");
			else
				if (IsPlayerConnected(PlayerToJail)) // If the player is a valid playerid (he's connected)
				{
				    // Jail the player
					Police_JailPlayer(PlayerToJail, JailTime);
					// Get the name of the player who jailed the player
					GetPlayerName(playerid, AdminName, sizeof(AdminName));
					// Get the name of the player who's being sent to jail
					GetPlayerName(PlayerToJail, Name, sizeof(Name));
					// Send the jailed player a message who jailed him, why he's been jailed and how long
					format(Msg, 128, "You have been jailed by %s %s for %i seconds", AdminLevelName[APlayerData[playerid][PlayerLevel]], AdminName, JailTime);
					SendClientMessage(PlayerToJail, COLOR_RED, Msg);
					format(Msg, 128, "Reason: %s", Reason);
					SendClientMessage(PlayerToJail, COLOR_RED, Msg);
					format(Msg, 128, "{00FF00}You have jailed {FFFF00}%s{00FF00} for {FFFF00}%i{00FF00} seconds", Name, JailTime);
					SendClientMessage(playerid, COLOR_WHITE, Msg);
				}
				else
				    SendClientMessage(playerid, COLOR_RED, "That player isn't online");
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Warn a player
COMMAND:warn(playerid, params[])
{
	new PlayerToWarn, Reason[128], ReasonMsg[128], Name[24];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/warn", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 1
		if (APlayerData[playerid][PlayerLevel] >= 1)
		{
			if (sscanf(params, "us[128]", PlayerToWarn, Reason)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/warn <PlayerToWarn> <Reason>\"");
			else
				if (IsPlayerConnected(PlayerToWarn)) // If the player is a valid playerid (he's connected)
				{
					// Increase the number of warnings
					APlayerData[PlayerToWarn][Warnings]++;

					// Get the name of the player who warned the player
					GetPlayerName(playerid, Name, sizeof(Name));
					// Send the warned player a message who warned him and why he's been warned
					format(ReasonMsg, 128, "You have been warned by %s %s", AdminLevelName[APlayerData[playerid][PlayerLevel]], Name);
					SendClientMessage(PlayerToWarn, COLOR_RED, ReasonMsg);
					format(ReasonMsg, 128, "Reason: %s", Reason);
					SendClientMessage(PlayerToWarn, COLOR_RED, ReasonMsg);
					format(ReasonMsg, 128, "~w~Warning %i/%i: ~r~%s~w~", APlayerData[PlayerToWarn][Warnings], AutoKickWarnings, Reason);
					GameTextForPlayer(PlayerToWarn, ReasonMsg, 5000, 4);

					// Get the name of the warned player
					GetPlayerName(PlayerToWarn, Name, sizeof(Name));
					// Let the admin know who has been warned and why
					format(ReasonMsg, 128, "You have warned %s (warnings: %i/%i)", Name, APlayerData[PlayerToWarn][Warnings], AutoKickWarnings);
					SendClientMessage(playerid, COLOR_GREEN, ReasonMsg);
					format(ReasonMsg, 128, "Reason: %s", Reason);
					SendClientMessage(playerid, COLOR_RED, ReasonMsg);

					// Automatically kick the player if he got 3 warnings (if autokick is enabled) and inform the player about that
					if ((APlayerData[PlayerToWarn][Warnings] == AutoKickWarnings) && (AutoKickAfterWarn == 1))
					{
						format(ReasonMsg, 128, "You have been kicked. (warnings: %i/%i)", APlayerData[PlayerToWarn][Warnings], AutoKickWarnings);
						SendClientMessage(PlayerToWarn, COLOR_RED, ReasonMsg);

						SetTimerEx("TimedKick", (750 + GetPlayerPing(PlayerToWarn)), false, "i", PlayerToWarn);
					}
				}
				else
				    SendClientMessage(playerid, COLOR_RED, "That player isn't online");
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Kicks a player with a reason
COMMAND:kick(playerid, params[])
{
	new PlayerToKick, Reason[128], ReasonMsg[128], Name[24];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/kick", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 1
		if (APlayerData[playerid][PlayerLevel] >= 1)
		{
			if (sscanf(params, "us[128]", PlayerToKick, Reason)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/kick <Player> <Reason>\"");
			else
				if (IsPlayerConnected(PlayerToKick)) // If the player is a valid playerid (he's connected)
				{
					// Get the name of the player who warned the player
					GetPlayerName(playerid, Name, sizeof(Name));
					// Send the warned player a message who kicked him and why he's been kicked
					format(ReasonMsg, 128, "You have been kicked by %s %s", AdminLevelName[APlayerData[playerid][PlayerLevel]], Name);
					SendClientMessage(PlayerToKick, COLOR_RED, ReasonMsg);
					format(ReasonMsg, 128, "Reason: %s", Reason);
					SendClientMessage(PlayerToKick, COLOR_RED, ReasonMsg);
					// Kick the player
					SetTimerEx("TimedKick", (750 + GetPlayerPing(PlayerToKick)), false, "i", PlayerToKick);
				}
				else
				    SendClientMessage(playerid, COLOR_RED, "That player isn't online");
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Bans a player for (days, hours, minutes, seconds)
COMMAND:ban(playerid, params[])
{
	// Setup local variables
	new PlayerToBan, Days, Hours, Reason[128], TotalBanTime, Msg[128], Name[24], AdminName[24];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/ban", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 3
		if (APlayerData[playerid][PlayerLevel] >= 3)
		{
			if (sscanf(params, "uiis[128]", PlayerToBan, Days, Hours, Reason))
				SendClientMessage(playerid, COLOR_RED, "Usage: \"/ban <Player> <Days> <Hours> <Reason>\"");
			else
			{
				if (IsPlayerConnected(PlayerToBan))
				{
					// Get the names of the player and the admin who executed the ban
					GetPlayerName(playerid, AdminName, sizeof(AdminName));
					GetPlayerName(PlayerToBan, Name, sizeof(Name));

					// Increase the number of bans
					APlayerData[PlayerToBan][Bans]++;
					// Calculate the total bantime (when the player can login again)
					TotalBanTime = (Days * 86400) + (Hours * 3600) + gettime();
					// Check if this is the player's 5th ban
					if (APlayerData[PlayerToBan][Bans] == 5)
						APlayerData[PlayerToBan][BanTime] = 2147483640; // Make the ban permanent (as high as it can go)
					else
						APlayerData[PlayerToBan][BanTime] = TotalBanTime; // Store this value for the player

					// Inform the player about his ban
					// Check if this is the player's 5th ban
					if (APlayerData[PlayerToBan][Bans] == 5)
					{
						format(Msg, 128, "You have been banned permanently by %s, this was your 5th ban", AdminName);
						SendClientMessage(PlayerToBan, COLOR_GRAY, Msg);
					}
					else
					{
						format(Msg, 128, "You have been banned by %s for %i days and %i hours", AdminName, Days, Hours);
						SendClientMessage(PlayerToBan, COLOR_GRAY, Msg);
						format(Msg, 128, "Reason: %s", Reason);
						SendClientMessage(PlayerToBan, COLOR_GRAY, Msg);
						format(Msg, 128, "You've been banned %i times now, 5th time is permament", APlayerData[PlayerToBan][Bans]);
						SendClientMessage(PlayerToBan, COLOR_GRAY, Msg);
					}

					// Kick the player (his data will be saved)
					SetTimerEx("TimedKick", (750 + GetPlayerPing(PlayerToBan)), false, "i", PlayerToBan);

					// Inform everybody else which player was banned and for how long
					format(Msg, 128, "%s %s has banned %s for %i days and %i hours", AdminLevelName[APlayerData[playerid][PlayerLevel]], AdminName, Name, Days, Hours);
					SendClientMessageToAll(COLOR_GRAY, Msg);
				}
				else
					SendClientMessage(playerid, COLOR_RED, "Player isn't connected");
			}
		}
		else
		    return 0;
	}
	else
	    return 0;

	return 1;
}

// Unbans a player (cleares the ban-time)
COMMAND:unban(playerid, params[])
{
	// Setup local variables
	new PlayerToUnban[24], Msg[128], Name[24];
	new file[100], File:PFile, LineForFile[100];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/unban", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 3
		if (APlayerData[playerid][PlayerLevel] >= 3)
		{
			if (sscanf(params, "s[128]", PlayerToUnban))
				SendClientMessage(playerid, COLOR_RED, "Usage: \"/unban <PlayerToUnban>\"");
			else
			{
			    // Get the name of the admin
				GetPlayerName(playerid, Name, sizeof(Name));

				// Construct the complete filename for this player's account
				format(file, sizeof(file), PlayerFile, PlayerToUnban);

				// Check if the file exists
				if (fexist(file))
				{
					PFile = fopen(file, io_append); // Open the playerfile for appending (this command only appends a new line to overwrite the bantime)

					format(LineForFile, 100, "BanTime 0\r\n"); // Construct the line: "BanTime <0>"
					fwrite(PFile, LineForFile); // And save it to the file

					fclose(PFile); // Close the file

					// Inform everybody else which player was unbanned
					format(Msg, 128, "%s %s has un-banned %s", AdminLevelName[APlayerData[playerid][PlayerLevel]], Name, PlayerToUnban);
					SendClientMessageToAll(COLOR_GRAY, Msg);
				}
				else
				    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}There is no account with that playername");
			}
		}
		else
		    return 0;
	}
	else
	    return 0;

	return 1;
}

// Spawns the object at the location given by the player
COMMAND:createobject(playerid, params[])
{
    // Setup local variables
	new ObjectModel, ObjID, Msg[128], Float:x, Float:y, Float:z, Float:Angle;

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/createobject", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 5
		if (APlayerData[playerid][PlayerLevel] >= 5)
		{
			if (sscanf(params, "iffff", ObjectModel, x, y, z, Angle)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/createobject <Object Model ID> <x> <y> <z> <Angle>\"");
			else
			{
				// Spawn the object 5 units north of the player
				ObjID = CreateObject(ObjectModel, x, y, z, 0.0, 0.0, Angle, 250.0);
				// Inform the player about it
				format(Msg, 128, "You spawned object-id %i (model-id = %i) at coords: x=%4.2f, y=%4.2f, z=%4.2f", ObjID, ObjectModel, x, y, z);
				SendClientMessage(playerid, COLOR_GREEN, Msg);
			}
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Spawns the pickup at the location given by the player
COMMAND:createpickup(playerid, params[])
{
    // Setup local variables
	new PickupModel, PickID, Msg[128], Float:x, Float:y, Float:z, PickupType;

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/createpickup", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 5
		if (APlayerData[playerid][PlayerLevel] >= 5)
		{
			if (sscanf(params, "ifffi", PickupModel, x, y, z, PickupType)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/createpickup <Pickup Model ID> <x> <y> <z> <Type>\"");
			else
			{
				// Spawn the pickup
				PickID = CreatePickup(PickupModel, PickupType, x, y, z, -1);
				// Inform the player about it
				format(Msg, 128, "You spawned pickup-id %i (model-id = %i) at coords: x=%4.2f, y=%4.2f, z=%4.2f", PickID, PickupModel, x, y, z);
				SendClientMessage(playerid, COLOR_GREEN, Msg);
			}
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Deletes the given vehicle from the game
COMMAND:delobject(playerid, params[])
{
    // Setup local variables
	new oID, Msg[128];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/delobject", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 5
		if (APlayerData[playerid][PlayerLevel] >= 5)
		{
			if (sscanf(params, "i", oID)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/delobject <ObjectID>\"");
			else
			{
				// Destroy the given vehicle
				DestroyObject(oID);
				// Inform the player about it
				format(Msg, 128, "You deleted object-id %i", oID);
				SendClientMessage(playerid, COLOR_GREEN, Msg);
			}
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Ports the player to the given coordinates
COMMAND:portloc(playerid, params[])
{
	// Setup local variables
	new Float:x, Float:y, Float:z, PortMsg[128];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/portloc", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 1
		if (APlayerData[playerid][PlayerLevel] >= 1)
		{
			// Check if the player has a wanted level of less than 3
			if (GetPlayerWantedLevel(playerid) < 3)
			{
				// Check if the player is not jailed
				if (APlayerData[playerid][PlayerJailed] == 0)
				{
					if (sscanf(params, "fff", x, y, z)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/portloc <x> <y> <z>\"");
					else
					{
						// Port the player to the given location
						SetPlayerPos(playerid, x, y, z);
						// Let the player know about it
						format(PortMsg, 128, "You have been ported to location: %4.2f, %4.2f, %4.2f", x, y, z);
						SendClientMessage(playerid, COLOR_GREEN, PortMsg);
					}
				}
				else
				    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}You cannot use /portloc when you're in jail");
			}
			else
			    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}You cannot use /portloc when you're wanted");
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Ports the player to the given player
COMMAND:port(playerid, params[])
{
	// Setup local variables
	new OtherPlayer, Float:x, Float:y, Float:z, PortMsg[128], IntID, WorldID;

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/port", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 1
		if (APlayerData[playerid][PlayerLevel] >= 1)
		{
			// Check if the player has a wanted level of less than 3
			if (GetPlayerWantedLevel(playerid) < 3)
			{
				// Check if the player is not jailed
				if (APlayerData[playerid][PlayerJailed] == 0)
				{
					if (sscanf(params, "u", OtherPlayer)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/port <Player>\"");
					else
					{
						// Check if that other player is online
						if (IsPlayerConnected(OtherPlayer))
						{
							// Get the location of the other player
							GetPlayerPos(OtherPlayer, x, y, z);
							IntID = GetPlayerInterior(OtherPlayer);
							WorldID = GetPlayerVirtualWorld(OtherPlayer);
							// Port the player to the given location
							SetPlayerVirtualWorld(playerid, WorldID);
							SetPlayerInterior(playerid, IntID);
							SetPlayerPos(playerid, x, y, z + 3.0);
							// Let the player know about it
							format(PortMsg, 128, "You have been ported to location: %4.2f, %4.2f, %4.2f", x, y, z + 3.0);
						}
						else
						    SendClientMessage(playerid, COLOR_RED, "That player isn't online");
					}
				}
				else
				    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}You cannot use /port when you're in jail");
			}
			else
			    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}You cannot use /port when you're wanted");
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Ports one player to another player
COMMAND:tele(playerid, params[])
{
	// Setup local variables
	new Player1, Player2, Float:x, Float:y, Float:z, PortMsg[128], IntID, WorldID, Name[24], AdminName[24];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/tele", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 1
		if (APlayerData[playerid][PlayerLevel] >= 1)
		{
			if (sscanf(params, "uu", Player1, Player2)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/tele <Player 1> <Player 2>\"");
			else
			{
				if (Player1 != Player2) {
					// Check if player1 is online
					if (APlayerData[Player1][LoggedIn] == true)
					{
						// Check if player2 is online
						if (APlayerData[Player2][LoggedIn] == true)
						{
						    // Get the name of the admin and the second player
						    GetPlayerName(playerid, AdminName, sizeof(AdminName));
						    GetPlayerName(Player2, Name, sizeof(Name));
							// Get the location of the second player
							GetPlayerPos(Player2, x, y, z);
							IntID = GetPlayerInterior(Player2);
							WorldID = GetPlayerVirtualWorld(Player2);
							// Port the first player to player2's location
							SetPlayerVirtualWorld(Player1, WorldID);
							SetPlayerInterior(Player1, IntID);
							SetPlayerPos(Player1, x, y, z + 3.0);
							// Let the first player know he's been ported
							format(PortMsg, 128, "{00FF00}You have been ported to player {FFFF00}%s{00FF00} by {FFFF00}%s", Name, AdminName);
							SendClientMessage(Player1, COLOR_WHITE, PortMsg);
						}
						else
						    SendClientMessage(playerid, COLOR_RED, "Player 2 isn't online");
					}
					else
					    SendClientMessage(playerid, COLOR_RED, "Player 1 isn't online");
				}
				else
					SendClientMessage(playerid, COLOR_RED, "Players must be different");
			}
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Ports the player to the given vehicle
COMMAND:portvehicle(playerid, params[])
{
	// Setup local variables
	new Car, Float:x, Float:y, Float:z, PortMsg[128];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/portvehicle", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 1
		if (APlayerData[playerid][PlayerLevel] >= 1)
		{
			// Check if the player has a wanted level of less than 3
			if (GetPlayerWantedLevel(playerid) < 3)
			{
				// Check if the player is not jailed
				if (APlayerData[playerid][PlayerJailed] == 0)
				{
					if (sscanf(params, "i", Car)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/portvehicle <Vehicle ID>\"");
					else
					{
						//check if the car exists
						if(AVehicleData[Car][Model] != 0)
						{
							// Get the location of the car
							GetVehiclePos(Car, x, y, z);
							// Port the player to the given location
							SetPlayerPos(playerid, x, y, z + 3.0);
							// Let the player know about it
							format(PortMsg, 128, "You have been ported to location: %4.2f, %4.2f, %4.2f", x, y, z + 3.0);
							SendClientMessage(playerid, COLOR_GREEN, PortMsg);
						}
						else
							SendClientMessage(playerid, COLOR_RED, "The vehicle id its not valid");
					}
				}
				else
				    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}You cannot use /portvehicle when you're in jail");
			}
			else
			    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}You cannot use /portvehicle when you're wanted");
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Sets the admin-level of another player
COMMAND:setlevel(playerid, params[])
{
	// Setup local variables
	new OtherPlayer, Level, Msg[128], Name[24], AdminName[24], OldLevel;

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/setlevel", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 5
		if (APlayerData[playerid][PlayerLevel] >= 5)
		{
			if (sscanf(params, "ui", OtherPlayer, Level)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/setlevel <Player> <AdminLevel>\"");
			else
			{
				// Check if that other player is online
				if (IsPlayerConnected(OtherPlayer))
				{
					// Check if the level is in the range
					if(Level < 0 || Level > 5)
						return SendClientMessage(playerid, COLOR_RED, "The level its out of range (0-5)");
				    // Get the old level of the other player
				    OldLevel = APlayerData[OtherPlayer][PlayerLevel];
					// Get the playername of the admin
					GetPlayerName(playerid, AdminName, sizeof(AdminName));
					// Also get the name of the player that has been promoted
					GetPlayerName(OtherPlayer, Name, sizeof(Name));
					// Store the level of the player
					APlayerData[OtherPlayer][PlayerLevel] = Level;
					// Let all players know about it
					if (OldLevel != Level)
					{
						// Check if the player has been promoted or demoted
						if (OldLevel < Level)
							format(Msg, 128, "Player %s has been promoted to %s by %s", Name, AdminLevelName[Level], AdminName);
						if (OldLevel > Level)
							format(Msg, 128, "Player %s has been demoted to %s by %s", Name, AdminLevelName[Level], AdminName);
						SendClientMessageToAll(COLOR_GREEN, Msg);
					}
					else
						SendClientMessage(playerid, COLOR_RED, "Other player's level hasn't been changed");
				}
				else
				    SendClientMessage(playerid, COLOR_RED, "That player isn't online");
			}
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Refuel the player's vehicle
COMMAND:fuel(playerid, params[])
{
	// Setup local variables
	new vID;

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/fuel", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 1
		if (APlayerData[playerid][PlayerLevel] >= 1)
		{
			// Check if the player is inside a vehicle
			if (IsPlayerInAnyVehicle(playerid))
			{
			    // Get the vehicleid
			    vID = GetPlayerVehicleID(playerid);
			    // Refuel the vehicle
			    AVehicleData[vID][Fuel] = MaxFuel;
				// Let the player know about it
				SendClientMessage(playerid, COLOR_GREEN, "Your vehicle is refuelled");
			}
			else
				SendClientMessage(playerid, COLOR_RED, "You're not driving a vehicle");
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Let the player use a jetpack
COMMAND:fly(playerid, params[])
{
	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/fly", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 3
		if (APlayerData[playerid][PlayerLevel] >= 3)
		{
			// Equip the player with a jetpack
			SetPlayerSpecialAction(playerid, 2);
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Paints a car with a paintjob
COMMAND:paint(playerid, params[])
{
    // Setup local variables
	new PaintjobID, vid;

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/paint", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player is the driver of the vehicle
		if (GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
		{
			if (sscanf(params, "i", PaintjobID)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/paint <Paintjob ID (0-2)>\"");
			else
			{
			    // Check if the player entered a correct paintjob id
			    if ((PaintjobID >= 0) && (PaintjobID <= 2))
				{
					// Get the vehicle-id
					vid = GetPlayerVehicleID(playerid);
					// Check if the player is the owner of the vehicle
					if (AVehicleData[vid][Owned] == true)
					{
						// Change the paintjob of the player's vehicle
						ChangeVehiclePaintjob(vid, PaintjobID);
						// Also save the paintjob for the vehicle
						AVehicleData[vid][PaintJob] = PaintjobID + 1;

						// Also set the vehiclecolor to 1 (white), so the paintjob has it's full color
						ChangeVehicleColor(vid, 1, 1);
						// Also save the colors for the vehicle
						AVehicleData[vid][Color1] = 1;
						AVehicleData[vid][Color2] = 1;

						// Let the player pay for the paintjob
						RewardPlayer(playerid, -500, 0);
						// Inform the player about it
						SendClientMessage(playerid, COLOR_GREEN, "You repainted your vehicle for $500");
						// Save the player-file (along with the houses and businesses)
						PlayerFile_Save(playerid);
					}
					else
					    SendClientMessage(playerid, COLOR_RED, "You're not the owner of the vehicle");
				}
				else
				    SendClientMessage(playerid, COLOR_RED, "You need to enter a paintjob id from 0 to 2");
			}
		}
		else
			SendClientMessage(playerid, COLOR_RED, "You must be inside a vehicle you own to apply a paintjob");
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Re-colors the given vehicle (lets you choose the first color)
COMMAND:color1(playerid, params[])
{
	// Setup local variables
	new ColorList[1000];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/color1", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player is the driver of the vehicle
		if (GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
		{
			// Check if the player is the owner of the vehicle
			if (AVehicleData[GetPlayerVehicleID(playerid)][Owned] == true || AVehicleData[GetPlayerVehicleID(playerid)][Owned] == false && APlayerData[playerid][PlayerLevel] >= 1)
			{
				// Construct the colorlist
				format(ColorList, sizeof(ColorList), "%s{FFFFFF}%s\n", ColorList, "Black (removes paintjob)"); // Color 0
				format(ColorList, sizeof(ColorList), "%s{FFFFFF}%s\n", ColorList, "White"); // Color 1

				for (new i = 2; i < sizeof(AVehicleColors); i++)
					format(ColorList, sizeof(ColorList), "%s%s%s\n", ColorList, AVehicleColors[i], "CarColor");

				// Show the dialog with all the colors
				ShowPlayerDialog(playerid, DialogPrimaryCarColor, DIALOG_STYLE_LIST, "Choose a primary color:", ColorList, "Select", "Cancel");
			}
			else
			    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}You're not the owner of the vehicle");
		}
		else
			SendClientMessage(playerid, COLOR_RED, "You must be inside a vehicle to apply a color");
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Re-colors the given vehicle (lets you choose the first color)
COMMAND:color2(playerid, params[])
{
	// Setup local variables
	new ColorList[1000];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/color2", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player is the driver of the vehicle
		if (GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
		{
			// Check if the player is the owner of the vehicle
			if (AVehicleData[GetPlayerVehicleID(playerid)][Owned] == true || AVehicleData[GetPlayerVehicleID(playerid)][Owned] == false && APlayerData[playerid][PlayerLevel] >= 1)
			{
				// Construct the colorlist
				format(ColorList, sizeof(ColorList), "%s{FFFFFF}%s\n", ColorList, "Black"); // Color 0
				format(ColorList, sizeof(ColorList), "%s{FFFFFF}%s\n", ColorList, "White"); // Color 1

				for (new i = 2; i < sizeof(AVehicleColors); i++)
					format(ColorList, sizeof(ColorList), "%s%s%s\n", ColorList, AVehicleColors[i], "CarColor");

				// Show the dialog with all the colors
				ShowPlayerDialog(playerid, DialogSedundaryCarColor, DIALOG_STYLE_LIST, "Choose a secundary color:", ColorList, "Select", "Cancel");
			}
			else
			    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}You're not the owner of the vehicle");
		}
		else
			SendClientMessage(playerid, COLOR_RED, "You must be inside a vehicle to apply a color");
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Starts or stops the engine of your vehicle
COMMAND:engine(playerid, params[])
{
    // Setup local variables
	new vehicleid, engine,lights,alarm,doors,bonnet,boot,objective;

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/engine", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		//check if the player is the driver of the car, don't let the passenger to control the engine
		if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
		{
			// Get the player's vehicle
			vehicleid = GetPlayerVehicleID(playerid);

			// Check if the player is inside a vehicle
			if (vehicleid != 0)
			{
				// Get the current status of the vehicle
				GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);

				SetVehicleParamsEx(vehicleid, (1 - engine), lights, alarm, doors, bonnet, boot, objective);

				//Check if the engine is running or not
				if(engine)
					//Inform player about that
					SendClientMessage(playerid, COLOR_GREEN, "You have turned off the engine");
				else
					//Inform player about that
					SendClientMessage(playerid, COLOR_GREEN, "You have turned on the engine");
				
			}
			else
				SendClientMessage(playerid, COLOR_RED, "You must be inside a vehicle to toggle your engine");
		}
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Asks which vehicle parameter you wanna toggle
COMMAND:caroption(playerid, params[])
{
	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/caroption", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 3
		if (APlayerData[playerid][PlayerLevel] >= 3)
		{
			// Let the player select a weather-type
			ShowPlayerDialog(playerid, DialogCarOption, DIALOG_STYLE_LIST, "Select option for your vehicle:", "Engine\nLights\nAlarm\nDoors\nBonnet\nBoot\nObjective", "Toggle", "Cancel");
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Sets the global weather
COMMAND:weather(playerid, params[])
{
	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/weather", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 4
		if (APlayerData[playerid][PlayerLevel] >= 4)
		{
			// Let the player select a weather-type
			ShowPlayerDialog(playerid, DialogWeather, DIALOG_STYLE_LIST, "Select weather type:", "Normal\nStormy\nFoggy\nScorching Hot\nDull, cloudy, rainy\nSandstorm\nGreen Fog\nDark, cloudy, brown\nExtremely bright\nDark toxic clouds\nBlack & white sky", "Select", "Cancel");
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}



// This command lists all online admins
COMMAND:admins(playerid, params[])
{
	// Setup local variables
	new AdminList[500], Name[24];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/admins", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Scan through all players
		for (new i; i < MAX_PLAYERS; i++)
		{
			// Check if this player is connected
			if (IsPlayerConnected(i))
			{
				// Get the name of the player
				GetPlayerName(i, Name, sizeof(Name));

				// Check if this player is an RCON admin
				if (IsPlayerAdmin(i))
				{
					// Add all admin players to the list
					format(AdminList, 500, "%s%s: %s (id: %i), admin-level: %i (RCON admin)\n", AdminList, AdminLevelName[APlayerData[i][PlayerLevel]], Name, i, APlayerData[i][PlayerLevel]); // Add the name of the admin-player to the list
					// Re-start the for loop (skipping the remaining code for this iteration)
					continue;
				}

				//Check if that player is an admin (using the PlayerLevel)
				if (APlayerData[i][PlayerLevel] > 0)
				{
					// Add all admin players to the list
					format(AdminList, 500, "%s%s: %s (id: %i), admin-level: %i\n", AdminList, AdminLevelName[APlayerData[i][PlayerLevel]], Name, i, APlayerData[i][PlayerLevel]); // Add the name of the admin-player to the list
				}
			}
		}

		// Check if there were admin-names added to the list
		if (strlen(AdminList) > 0)
		    ShowPlayerDialog(playerid, DialogNoResponse, DIALOG_STYLE_LIST, "Online admins:", AdminList, "OK", "Cancel");
		else
			SendClientMessage(playerid, COLOR_RED, "No admin online"); // No admins are online
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// This command lists all commands for normal players (admin-level 0)
COMMAND:cmds(playerid, params[])
{
	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/cmds", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		CommandList_Create(playerid); // Create a list of commands (only the first 4 commands) and show the dialog
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

COMMAND:convoy(playerid, params[])
{
	// Setup local variables
	new ConvoyList[750], Name[24], NumMembers, ConvoyStatus[10];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/convoy", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Create the list of convoys with all their data
		for (new i; i < MAX_CONVOYS; i++)
		{
		    // Check if this is an empty convoy (not created yet by a player)
			if (AConvoys[i][Status] == CONVOY_EMPTY)
			{
				// Setup data for an empty convoy (one which hasn't been chosen yet)
				NumMembers = 0;
				format(Name, 24, "None");
				format(ConvoyStatus, 10, "Empty");
			}
			else
			{
				// Get the name of the convoy-leader
				GetPlayerName(AConvoys[i][Members][0], Name, sizeof(Name));
				// Calculate the members in the convoy
				NumMembers = Convoy_CountMembers(i);
				// Set the status of the convoy
				switch (AConvoys[i][Status])
				{
					case CONVOY_OPEN: format(ConvoyStatus, 10, "Open");
					case CONVOY_FULL: format(ConvoyStatus, 10, "Full");
					case CONVOY_CLOSED: format(ConvoyStatus, 10, "Closed");
				}
			}

			// Put all data together to form the content of the entire dialog
			format(ConvoyList, sizeof(ConvoyList), "%sLeader: {00FF00}%s{FFFFFF}, members: {FF0000}%i{FFFFFF}, Status: {00FF00}%s{FFFFFF}\n", ConvoyList, Name, NumMembers, ConvoyStatus);
		}

		// Show the dialog
		ShowPlayerDialog(playerid, DialogSelectConvoy, DIALOG_STYLE_LIST, "Select convoy:", ConvoyList, "Select", "Cancel");
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Allows the leader to kick a member from the convoy
COMMAND:convoykick(playerid, params[])
{
	// Setup local variables
	new Convoy, LeaderID, OtherPlayer, LeaderName[24], MemberName[24], Msg[128];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/convoykick", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		Convoy = APlayerData[playerid][ConvoyID]; // Get the convoy of the player
		LeaderID = AConvoys[Convoy][Members][0]; // Get the leader of his convoy

		// Check if this player is part of a convoy
		if (APlayerData[playerid][InConvoy] == true)
		{
			// Check if this player is the leader of the convoy
			if (LeaderID == playerid)
			{
				if (sscanf(params, "u", OtherPlayer)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/convoykick <Player>\"");
				else
				{
					if (playerid != OtherPlayer) {
						// Check if that other player is online
						if (IsPlayerConnected(OtherPlayer)) {
							// Check if that other player is part of the convoy
							if ((APlayerData[OtherPlayer][InConvoy] == true) && (APlayerData[OtherPlayer][ConvoyID] == Convoy))
							{
							    // Kick the other player from the convoy
								Convoy_Leave(OtherPlayer);
								// Get the names of the leader and member
								GetPlayerName(playerid, LeaderName, sizeof(LeaderName));
								GetPlayerName(OtherPlayer, MemberName, sizeof(MemberName));
								// Inform the leader that he has kicked the other player
								format(Msg, 128, "You've kicked {0000FF}%s{00FF00} from the convoy", MemberName);
								SendClientMessage(playerid, COLOR_GREEN, Msg);
								// Inform the leader that he has kicked the other player
								format(Msg, 128, "Leader {0000FF}%s{00FF00} kicked you from the convoy", LeaderName);
								SendClientMessage(OtherPlayer, COLOR_GREEN, Msg);
							}
							else
							    SendClientMessage(playerid, COLOR_RED, "You cannot kick a player that's not part of your convoy");
						}
						else
							SendClientMessage(playerid, COLOR_RED, "Player isn't connected");
					}
					else
						SendClientMessage(playerid, COLOR_RED, "You can not kick yourself from convoy. Use /convoyleave instead.");
				}
			}
			else
			    SendClientMessage(playerid, COLOR_RED, "You need to be the leader of a convoy to kick another convoy-member");
		}
		else
		    SendClientMessage(playerid, COLOR_RED, "You need to be the leader of a convoy to kick another convoy-member");
	}
	else
		return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Allows a convoy-member to leave the convoy
COMMAND:convoyleave(playerid, params[])
{
	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/convoyleave", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if this player is part of a convoy
		if (APlayerData[playerid][InConvoy] == true)
		{
		    // Kick the other player from the convoy
			Convoy_Leave(playerid);
			// Inform the player that he left the convoy
			SendClientMessage(playerid, COLOR_GREEN, "You left the convoy");
		}
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Allows the leader to cancel a convoy
COMMAND:convoycancel(playerid, params[])
{
	// Setup local variables
	new Convoy, LeaderID;

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/convoycancel", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		Convoy = APlayerData[playerid][ConvoyID]; // Get the convoy of the player
		LeaderID = AConvoys[Convoy][Members][0]; // Get the leader of his convoy

		// Check if this player is part of a convoy
		if (APlayerData[playerid][InConvoy] == true)
		{
			// Check if this player is the leader of the convoy
			if (LeaderID == playerid)
				Convoy_Cancel(Convoy); // Cancel the convoy, kicking every other member from it
			else
			    SendClientMessage(playerid, COLOR_RED, "You need to be the leader of a convoy to cancel it");
		}
		else
		    SendClientMessage(playerid, COLOR_RED, "You need to be the leader of a convoy to cancel it");
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Displays all members in the convoy
COMMAND:convoymembers(playerid, params[])
{
	// Setup local variables
	new Name[24], MemberList[1000], Convoy;

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/convoymembers", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the member is in a convoy
		if (APlayerData[playerid][InConvoy] == true)
		{
			// Get the convoyID of the member
			Convoy = APlayerData[playerid][ConvoyID];

			// Loop through all members
			for (new i; i < CONVOY_MAX_MEMBERS; i++)
			{
			    if (AConvoys[Convoy][Members][i] != -1) // Check if this member-spot is occupied
				{
					// Get the name of the member
					GetPlayerName(AConvoys[Convoy][Members][i], Name, sizeof(Name));
					// Add the membernames to the list
					format(MemberList, 1000, "%s%s\n", MemberList, Name);
				}
			}

			// Show the dialog
			ShowPlayerDialog(playerid, DialogConvoyMembers, DIALOG_STYLE_LIST, "Convoy-members", MemberList, "OK", "Cancel");
		}
		else
		    SendClientMessage(playerid, COLOR_RED, "You're not a member of a convoy");
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Displays the statictics of the player
COMMAND:stats(playerid, params[])
{
	// Setup local variables
	new StatsMsg[1000], TitleMsg[128], StatsPlayer;

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/stats", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		if(sscanf(params, "U(-1)", StatsPlayer)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/stats <StatsPlayer (optional)>\"");
		else
		{
			if (-1 == StatsPlayer)
				StatsPlayer = playerid;

			// Check if stats player is online
			if (IsPlayerConnected(StatsPlayer)) {
				if (playerid == StatsPlayer) {
					format(TitleMsg, sizeof(TitleMsg), "Your Statistics");
				} else {
					new Name[MAX_PLAYER_NAME];
					// Get the player's name
					GetPlayerName(StatsPlayer, Name, sizeof(Name));
					// Construct the title for the dialog
					format(TitleMsg, sizeof(TitleMsg), "Statistics for %s:", Name);
				}

				// Construct the stats
				format(StatsMsg, sizeof(StatsMsg), "Distance driven: {00FF00}%.0f meters (%.2f km)\n", APlayerData[StatsPlayer][StatsMetersDriven], (APlayerData[StatsPlayer][StatsMetersDriven] / 1000));
				format(StatsMsg, sizeof(StatsMsg), "%sFinished trucker jobs: {00FF00}%i{FFFFFF}\n", StatsMsg, APlayerData[StatsPlayer][StatsTruckerJobs]);
				format(StatsMsg, sizeof(StatsMsg), "%sFinished convoy jobs: {00FF00}%i{FFFFFF}\n", StatsMsg, APlayerData[StatsPlayer][StatsConvoyJobs]);
				format(StatsMsg, sizeof(StatsMsg), "%sFinished busdriver routes: {00FF00}%i{FFFFFF}\n", StatsMsg, APlayerData[StatsPlayer][StatsBusDriverJobs]);
				format(StatsMsg, sizeof(StatsMsg), "%sFinished pilot jobs: {00FF00}%i{FFFFFF}\n", StatsMsg, APlayerData[StatsPlayer][StatsPilotJobs]);
				format(StatsMsg, sizeof(StatsMsg), "%sFinished courier jobs: {00FF00}%i{FFFFFF}\n", StatsMsg, APlayerData[StatsPlayer][StatsCourierJobs]);
				format(StatsMsg, sizeof(StatsMsg), "%sFinished roadworker jobs: {00FF00}%i{FFFFFF}\n", StatsMsg, APlayerData[StatsPlayer][StatsRoadworkerJobs]);
				format(StatsMsg, sizeof(StatsMsg), "%sFixed vehicles for assistance: {00FF00}%i{FFFFFF}\n", StatsMsg, APlayerData[StatsPlayer][StatsAssistance]);
				format(StatsMsg, sizeof(StatsMsg), "%sFinished mafia jobs: {00FF00}%i{FFFFFF}\n", StatsMsg, APlayerData[StatsPlayer][StatsMafiaJobs]);
				format(StatsMsg, sizeof(StatsMsg), "%sDelivered stolen mafia-loads: {00FF00}%i{FFFFFF}\n", StatsMsg, APlayerData[StatsPlayer][StatsMafiaStolen]);
				format(StatsMsg, sizeof(StatsMsg), "%sFined players: {00FF00}%i{FFFFFF}\n", StatsMsg, APlayerData[StatsPlayer][StatsPoliceFined]);
				format(StatsMsg, sizeof(StatsMsg), "%sJailed players: {00FF00}%i{FFFFFF}\n", StatsMsg, APlayerData[StatsPlayer][StatsPoliceJailed]);

				// Show the dialog
				ShowPlayerDialog(playerid, DialogStats, DIALOG_STYLE_LIST, TitleMsg, StatsMsg, TXT_DialogButtonOk, TXT_DialogButtonCancel);
			}
			else
				SendClientMessage(playerid, COLOR_RED, "Player isn't connected");
		}
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// This command allows you to create a speedcamera
COMMAND:createcamera(playerid, params[])
{
	// Setup local variables
	new Float:x, Float:y, Float:z, Float:Angle, MaxSpeed, file[100], File:PFile, LineForFile[100], Msg[128];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/createcamera", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 5
		if (APlayerData[playerid][PlayerLevel] >= 5)
		{
			if (sscanf(params, "i", MaxSpeed)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/createcamera <Max Speed>\"");
			else
			{
				// Get player's position and facing angle
				GetPlayerPos(playerid, x, y, z);
				GetPlayerFacingAngle(playerid, Angle);
				z = z - 1.0; // Adjust camera Z-coordinate 1m lower than normal (otherwise the camera floats in the air)

				// Move the player a bit, otherwise he could get stuck inside the camera-object
				SetPlayerPos(playerid, x, y + 1.0, z + 1.0);

				// Save the camera to a file
				for (new CamID; CamID < MAX_CAMERAS; CamID++)
				{
					// Check if this index is free
					if (ACameras[CamID][CamSpeed] == 0)
					{
					    // Setup this camera (create the objects and store the data)
                        SetupSpeedCamera(CamID, x, y, z, Angle, MaxSpeed);

					    // Save the file
						format(file, sizeof(file), CameraFile, CamID); // Construct the complete filename for this camera-file

						PFile = fopen(file, io_write); // Open the camera-file for writing

						format(LineForFile, 100, "CamX %f\r\n", x);
						fwrite(PFile, LineForFile); // And save it to the file
						format(LineForFile, 100, "CamY %f\r\n", y);
						fwrite(PFile, LineForFile); // And save it to the file
						format(LineForFile, 100, "CamZ %f\r\n", z);
						fwrite(PFile, LineForFile); // And save it to the file
						format(LineForFile, 100, "CamAngle %f\r\n", Angle);
						fwrite(PFile, LineForFile); // And save it to the file
						format(LineForFile, 100, "CamSpeed %i\r\n", MaxSpeed);
						fwrite(PFile, LineForFile); // And save it to the file

						fclose(PFile); // Close the file

						// Let the player know he created a new camera
						format(Msg, 128, "You've created a speed-camera with ID: %i", CamID);
						SendClientMessage(playerid, COLOR_GREEN, Msg);

						// Exit the function
						return 1;
					}
				}

				// In case all camera-slots are occupied (100 camera's have been created already), let the player know about it
				format(Msg, 128, "{FF0000}You cannot create more than %i speedcamera's", MAX_CAMERAS);
				SendClientMessage(playerid, COLOR_WHITE, Msg);
			}
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// This command allows you to delete a speedcamera
COMMAND:delcamera(playerid, params[])
{
	// Setup local variables
	new file[100], Msg[128];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/delcamera", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 5
		if (APlayerData[playerid][PlayerLevel] >= 5)
		{
				// Loop through all camera's
			for (new CamID; CamID < MAX_CAMERAS; CamID++)
			{
				// Check if this index is used
				if (ACameras[CamID][CamSpeed] != 0)
				{
					// Check if the player is in range of the camera
					if (IsPlayerInRangeOfPoint(playerid, 5.0, ACameras[CamID][CamX], ACameras[CamID][CamY], ACameras[CamID][CamZ]))
					{
					    // Delete the file
						format(file, sizeof(file), CameraFile, CamID); // Construct the complete filename for this camera-file
						if (fexist(file)) // Make sure the file exists
							fremove(file); // Delete the file

						// Delete both camera objects
					    DestroyObject(ACameras[CamID][CamObj1]);
					    DestroyObject(ACameras[CamID][CamObj2]);
						// Also clear the data from memory
					    ACameras[CamID][CamX] = 0.0;
					    ACameras[CamID][CamY] = 0.0;
					    ACameras[CamID][CamZ] = 0.0;
					    ACameras[CamID][CamAngle] = 0.0;
					    ACameras[CamID][CamSpeed] = 0;
                        ACameras[CamID][CamObj1] = 0;
                        ACameras[CamID][CamObj2] = 0;

						// Let the player know he deleted a camera
						format(Msg, 128, "You've deleted speed-camera %i", CamID);
						SendClientMessage(playerid, COLOR_GREEN, Msg);

						// Exit the function
						return 1;
					}
				}
			}

			// In case the player wasn't near a speedcamera, inform him about it
			SendClientMessage(playerid, COLOR_WHITE, "{FF0000}You must be near a speedcamera to delete it");
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}



// This command ports a player to the player who executed the command
COMMAND:get(playerid, params[])
{
	// Setup local variables
	new OtherPlayer, Float:x, Float:y, Float:z, PortMsg[128], IntID, WorldID, Name[24];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/get", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 1
		if (APlayerData[playerid][PlayerLevel] >= 1)
		{
			if (sscanf(params, "u", OtherPlayer)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/get <Player>\"");
			else
			{
				// Check if that other player is online
				if (IsPlayerConnected(OtherPlayer))
				{
					// Get the name of the other player
					GetPlayerName(OtherPlayer, Name, sizeof(Name));
					// Get the location of the player
					GetPlayerPos(playerid, x, y, z);
					IntID = GetPlayerInterior(playerid);
					WorldID = GetPlayerVirtualWorld(playerid);
					// Port the other player to this player
					SetPlayerVirtualWorld(OtherPlayer, WorldID);
					SetPlayerInterior(OtherPlayer, IntID);
					SetPlayerPos(OtherPlayer, x, y, z + 3.0);
					// Let the player know about it
					format(PortMsg, 128, "{00FF00}You have ported {FFFF00}%s{00FF00} to your location", Name);
					SendClientMessage(playerid, COLOR_WHITE, PortMsg);
				}
				else
				    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}That player isn't online");
			}
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// This command deleted all vehicles that are spawned using /car, /plane, /bike, /trailer
COMMAND:cleanupallvehicles(playerid, params[])
{
	// Setup local variables
	new CarsDeleted, Msg[128];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/cleanupallvehicles", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 3
		if (APlayerData[playerid][PlayerLevel] >= 3)
		{
		    // Loop through all vehicles
		    for (new vid; vid < 2000; vid++)
		    {
				// Check if this vehicle exists (check the model), otherwise all empty slots are processed as well
				if (AVehicleData[vid][Model] != 0)
				{
					// Check if the vehicle is NOT a static vehicle
					if (AVehicleData[vid][StaticVehicle] == false)
					{
						// Check if the vehicle is NOT owned by anybody (this leaves all vehicles spawned with /car, /trailer, /plane)
						if (AVehicleData[vid][Owned] == false)
						{
							// Count the cars that have been deleted
							CarsDeleted++;
							// Eject everyone from the vehicle
							RemoveAllPlayersFromVehicle(vid);
							// Delete the vehicle and clear the data
							SetTimerEx("TimedDestroyVehicle", 1000, false, "i", vid);
							AVehicleData[vid][Owned] = false;
							AVehicleData[vid][Owner] = 0;
							AVehicleData[vid][Model] = 0;
							AVehicleData[vid][PaintJob] = 0;
							for (new i; i < 14; i++)
							    AVehicleData[vid][Components][i] = 0;
							AVehicleData[vid][SpawnX] = 0.0;
							AVehicleData[vid][SpawnY] = 0.0;
							AVehicleData[vid][SpawnZ] = 0.0;
							AVehicleData[vid][SpawnRot] = 0.0;
							AVehicleData[vid][BelongsToHouse] = 0;
						}
					}
				}
		    }

			// Let the player know how many vehicles have been cleaned up
			format(Msg, 128, "{00FF00}Total number of vehicles cleaned up: {FFFF00}%i", CarsDeleted);
			SendClientMessage(playerid, COLOR_WHITE, Msg);
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Deletes a specific vehicle that has been spawned by an admin
COMMAND:cleanupvehicle(playerid, params[])
{
    // Setup local variables
	new vID, Msg[128];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/cleanupvehicle", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 3
		if (APlayerData[playerid][PlayerLevel] >= 3)
		{
			if (sscanf(params, "i", vID)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/cleanupvehicle <Vehicle ID>\"");
			else
			{
				// Check if this vehicle exists (check the model), otherwise all empty slots are processed as well
				if (AVehicleData[vID][Model] != 0)
				{
					// Check if the vehicle is NOT a static vehicle
					if (AVehicleData[vID][StaticVehicle] == false)
					{
						// Check if the vehicle is NOT owned by anybody (this leaves all vehicles spawned with /car, /trailer, /plane)
						if (AVehicleData[vID][Owned] == false)
						{
							// Eject everyone from the vehicle
							RemoveAllPlayersFromVehicle(vID);
							// Destroy the given vehicle
							SetTimerEx("TimedDestroyVehicle", 1000, false, "i", vID);
							AVehicleData[vID][Owned] = false;
							AVehicleData[vID][Owner] = 0;
							AVehicleData[vID][Model] = 0;
							AVehicleData[vID][PaintJob] = 0;
							for (new i; i < 14; i++)
							    AVehicleData[vID][Components][i] = 0;
							AVehicleData[vID][SpawnX] = 0.0;
							AVehicleData[vID][SpawnY] = 0.0;
							AVehicleData[vID][SpawnZ] = 0.0;
							AVehicleData[vID][SpawnRot] = 0.0;
							AVehicleData[vID][BelongsToHouse] = 0;
							// Inform the player about it
							format(Msg, 128, "You deleted vehicle-id %i", vID);
							SendClientMessage(playerid, COLOR_GREEN, Msg);
						}
						else
							SendClientMessage(playerid, COLOR_WHITE, "{FF0000}That vehicle belongs to a player, you can use /clamp");
					}
					else
						SendClientMessage(playerid, COLOR_WHITE, "{FF0000}That vehicle is a static vehicle, delete it in the script");
				}
				else
					SendClientMessage(playerid, COLOR_WHITE, "{FF0000}That vehicle doesn't exist");
			}
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// This command allows the player to call for assistance
COMMAND:assist(playerid, params[])
{
	// Setup local variables
	new bool:AssistOnline = false, Msg[128], Name[24];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/assist", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Get the player's name
		GetPlayerName(playerid, Name, sizeof(Name));
		// Preset the message that needs to be sent to assistance players
		format(Msg, 128, "{00FF00}Player {FFFF00}%s{00FF00} needs assistance, go help him", Name);

		// Check if there is at least one assistance player online
		for (new i; i < MAX_PLAYERS; i++)
		{
			// Check if this player is connected
			if (IsPlayerConnected(i))
			{
				// Check if this player is assistance class
				if (APlayerData[i][PlayerClass] == ClassAssistance)
				{
					// Set the flag to indicate that at least one assistance player is online
				    AssistOnline = true;
				    // Send the assistance player a message to inform him who needs assistance
				    SendClientMessage(i, COLOR_WHITE, Msg);
				}
			}
		}

		// Check if there is at least one assistance player online
		if (AssistOnline == true)
		{
			// Set yourself as "AssistanceNeeded"
			APlayerData[playerid][AssistanceNeeded] = true;
			// Let the player know he called for assistance
			SendClientMessage(playerid, COLOR_WHITE, "{00FF00}You've called for assistance");
		}
		else // No assistance is online
		{
			// Check if the player is the driver of a vehicle
			if (GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
			{
				// Fully repair the vehicle (damage value and bodywork)
				RepairVehicle(GetPlayerVehicleID(playerid));
				// Also re-fuel the vehicle
				AVehicleData[GetPlayerVehicleID(playerid)][Fuel] = MaxFuel;
				// Let the player pay for the repairs and refuel (default $2000)
				RewardPlayer(playerid, -2000, 0);
				// Let the player know he spent $2000 for auto-repair because there were no assistance players online
			    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}Your vehicle has been auto-repaired and refuelled for $2000");
			    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}because there is no assistance player online");
			}
		}
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// This command adds nitro to the player's vehicle
COMMAND:nos(playerid, params[])
{
	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/nos", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 1
		if (APlayerData[playerid][PlayerLevel] >= 1)
		{
			// Check if the player is the driver of a vehicle
			if (GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
			{
				// Check if the vehicle isn't owned (owned vehicle's need to buy nitro at mod garages)
				if (AVehicleData[GetPlayerVehicleID(playerid)][Owned] == false)
					AddVehicleComponent(GetPlayerVehicleID(playerid), 1010); // Add nitro to the player's vehicle
				
				//Inform player the nos has been added
				SendClientMessage(playerid, COLOR_GREEN, "Nos has been added");
			}
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// This command let's an admin spectate another player
COMMAND:spec(playerid, params[])
{
	// Setup local variables
	new OtherPlayer, Name[24], Msg[128], Float:x, Float:y, Float:z, Float:angle;

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/spec", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 1
		if (APlayerData[playerid][PlayerLevel] >= 1)
		{
			if (sscanf(params, "u", OtherPlayer)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/spec <Player>\"");
			else
			{
				if (OtherPlayer == playerid) {
					format(Msg, 128, TXT_PlayerSelfSpectate, Name);
					return SendClientMessage(playerid, COLOR_WHITE, Msg);
				}
				
				// Check if that other player is online
				if (IsPlayerConnected(OtherPlayer))
				{
					// Get the player's name
					GetPlayerName(OtherPlayer, Name, sizeof(Name));

					// Get current coordinates
					GetPlayerPos(playerid, x, y, z);
					GetPlayerFacingAngle(playerid, angle);

					if (GetPlayerState(OtherPlayer) == PLAYER_STATE_SPECTATING) {
						format(Msg, 128, TXT_PlayerSpectating, Name);
						return SendClientMessage(playerid, COLOR_WHITE, Msg);
					}

					if (GetPlayerState(OtherPlayer) == PLAYER_STATE_WASTED || GetPlayerState(OtherPlayer) == PLAYER_STATE_NONE)
					{
						format(Msg, 128, TXT_PlayerNotSpawned);
						return SendClientMessage(playerid, COLOR_WHITE, Msg);
					}

					// Only save the current positon at the first time
					if (GetPlayerState(playerid) != PLAYER_STATE_SPECTATING) {
						APlayerData[playerid][SpectateX] = x;
						APlayerData[playerid][SpectateY] = y;
						APlayerData[playerid][SpectateZ] = z;
						APlayerData[playerid][SpectateA] = angle;
						APlayerData[playerid][Spectating] = true;
					}

				    // Turn spectating on
				    TogglePlayerSpectating(playerid, 1);

					// Check if the other player is driving a vehicle
					if (GetPlayerVehicleSeat(OtherPlayer) == -1)
					{
						// The other player is on foot, so spectate him
						PlayerSpectatePlayer(playerid, OtherPlayer);
						SetPlayerInterior(playerid, GetPlayerInterior(OtherPlayer));
						APlayerData[playerid][SpectateID] = OtherPlayer;
						APlayerData[playerid][SpectateType] = ADMIN_SPEC_TYPE_PLAYER;
					}
					else
					{
						// The other player is in a vehicle, so spectate the vehicle
						PlayerSpectateVehicle(playerid, GetPlayerVehicleID(OtherPlayer));
						APlayerData[playerid][SpectateID] = OtherPlayer;
						APlayerData[playerid][SpectateVehicle] = GetPlayerVehicleID(OtherPlayer);
						APlayerData[playerid][SpectateType] = ADMIN_SPEC_TYPE_VEHICLE;
					}

					format(Msg, 128, "{00FF00}You're spectating {FFFF00}%s", Name);
					SendClientMessage(playerid, COLOR_WHITE, Msg);
				}
				else
				    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}That player isn't online");
			}
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// This command ends the spectate mode
COMMAND:endspec(playerid, params[])
{
	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/endspec", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 1
		if (APlayerData[playerid][PlayerLevel] >= 1)
		{
			// Check if the player is spectating
			if (GetPlayerState(playerid) == PLAYER_STATE_SPECTATING)
			{
			    TogglePlayerSpectating(playerid, 0);
				APlayerData[playerid][SpectateID] = -1;
				APlayerData[playerid][SpectateVehicle] = -1;
				APlayerData[playerid][SpectateType] = ADMIN_SPEC_TYPE_NONE;
			}
			else
			    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}You are not spectating");
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}



// Lets the player add new businesses
COMMAND:createbusiness(playerid, params[])
{
	// Setup local variables
	new BusinessList[2000];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/createbusiness", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 5
		if (APlayerData[playerid][PlayerLevel] >= 5)
		{
			// Check if the player isn't inside a vehicle
			if (GetPlayerVehicleSeat(playerid) == -1)
			{
				// Construct the list of businesses
				for (new BusType = 1; BusType < sizeof(ABusinessInteriors); BusType++)
				{
				    format(BusinessList, sizeof(BusinessList), "%s%s\n", BusinessList, ABusinessInteriors[BusType][InteriorName]);
				}

				// Let the player choose a business-type via a dialog
				ShowPlayerDialog(playerid, DialogCreateBusSelType, DIALOG_STYLE_LIST, "Choose business-type:", BusinessList, "Select", "Cancel");
			}
			else
			    SendClientMessage(playerid, COLOR_RED, "You can't be inside a vehicle to create a business");
		}
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// This command lets the player delete a business
COMMAND:delbusiness(playerid, params[])
{
	// Setup local variables
	new file[100], Msg[128];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/delbusiness", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 5
		if (APlayerData[playerid][PlayerLevel] >= 5)
		{
			// Make sure the player isn't inside a vehicle
			if (GetPlayerVehicleID(playerid) == 0)
			{
				// Loop through all player-owned businesses
				for (new BusID = 1; BusID < MAX_BUSINESS; BusID++)
				{
					// Check if the business exists
					if (ABusinessData[BusID][PickupID] != 0)
					{
						// Check if the business has no owner
						if (ABusinessData[BusID][Owned] == false)
						{
							// Check if the player is in range of the business-pickup
							if (IsPlayerInRangeOfPoint(playerid, 2.5, ABusinessData[BusID][BusinessX], ABusinessData[BusID][BusinessY], ABusinessData[BusID][BusinessZ]))
							{
								// Clear all data of the business
	                            ABusinessData[BusID][BusinessName] = 0;
	                            ABusinessData[BusID][BusinessX] = 0.0;
	                            ABusinessData[BusID][BusinessY] = 0.0;
	                            ABusinessData[BusID][BusinessZ] = 0.0;
	                            ABusinessData[BusID][BusinessType] = 0;
	                            ABusinessData[BusID][BusinessLevel] = 0;
	                            ABusinessData[BusID][LastTransaction] = 0;
	                            ABusinessData[BusID][Owned] = false;
	                            ABusinessData[BusID][Owner] = 0;
								// Destroy the mapicon, 3DText and pickup for the house
								DestroyDynamicPickup(ABusinessData[BusID][PickupID]);
								DestroyDynamicMapIcon(ABusinessData[BusID][MapIconID]);
								DestroyDynamic3DTextLabel(ABusinessData[BusID][DoorText]);
	                            ABusinessData[BusID][PickupID] = 0;
	                            ABusinessData[BusID][MapIconID] = 0;

								// Delete the business-file
								format(file, sizeof(file), BusinessFile, BusID); // Construct the complete filename for this business-file
								if (fexist(file)) // Make sure the file exists
									fremove(file); // Delete the file

								// Also let the player know he deleted the business
								format(Msg, 128, "{00FF00}You have deleted the business with ID: {FFFF00}%i", BusID);
								SendClientMessage(playerid, COLOR_WHITE, Msg);

								// Exit the function
								return 1;
							}
						}
					}
				}

				// There was no house in range, so let the player know about it
				SendClientMessage(playerid, COLOR_WHITE, "{FF0000}No business in range to delete");
			}
			else
			    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}You can't be inside a vehicle to delete a business");
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// This command lets the player buy a business when he's standing in range of a business that isn't owned yet
COMMAND:buybus(playerid, params[])
{
	// Setup local variables
	new Msg[128], BusType;

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/buybus", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Make sure the player isn't inside a vehicle
		if (GetPlayerVehicleID(playerid) == 0)
		{
			// Check if the player is near a business-pickup
			for (new i = 1; i < sizeof(ABusinessData); i++)
			{
				// Check if this business is created (it would have a valid pickup in front of the door)
				if (ABusinessData[i][PickupID] != 0)
				{
					// Check if the player is in range of the business-pickup
					if (IsPlayerInRangeOfPoint(playerid, 2.5, ABusinessData[i][BusinessX], ABusinessData[i][BusinessY], ABusinessData[i][BusinessZ]))
					{
					    // Check if the business isn't owned yet
					    if (ABusinessData[i][Owned] == false)
					    {
							// Get the type of business
							BusType = ABusinessData[i][BusinessType];
					        // Check if the player can afford this type of business business
					        if (APlayerData[playerid][PlayerMoney] >= ABusinessInteriors[BusType][BusPrice])
					            Business_SetOwner(playerid, i); // Give ownership of the business to the player
					        else
					            SendClientMessage(playerid, COLOR_RED, "You cannot afford this business"); // The player cannot afford this business
					    }
					    else
					    {
					        // Let the player know that this business is already owned by a player
							format(Msg, 128, "{FF0000}This business is already owned by {00FF00}%s{FFFFFF}", ABusinessData[i][Owner]);
							SendClientMessage(playerid, COLOR_WHITE, Msg);
					    }

						// The player was in range of a business-pickup, so stop searching for the other business pickups
					    return 1;
					}
				}
				else
				    return 1; // If there are no more houses (no more pickup-id's found), stop searching
			}

			// All businesses have been processed, but the player wasn't in range of any business-pickup, let him know about it
			SendClientMessage(playerid, COLOR_RED, "To buy a business, you have to be near a business-pickup");
		}
		else
		    SendClientMessage(playerid, COLOR_RED, "You can't buy a business when you're inside a vehicle");
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// This command opens a menu when you're inside your business to allow to access the options of your business
COMMAND:busmenu(playerid, params[])
{
	// Setup local variables
	new OptionsList[200], DialogTitle[200];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/busmenu", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player is inside a business
		if (APlayerData[playerid][CurrentBusiness] != 0)
		{
			format(DialogTitle, sizeof(DialogTitle), "Select option for %s", ABusinessData[APlayerData[playerid][CurrentBusiness]][BusinessName]);

			format(OptionsList, sizeof(OptionsList), "%sChange business-name\n", OptionsList);
			format(OptionsList, sizeof(OptionsList), "%sUpgrade business\n", OptionsList);
			format(OptionsList, sizeof(OptionsList), "%sRetrieve business earnings\n", OptionsList);
			format(OptionsList, sizeof(OptionsList), "%sSell business\n", OptionsList);
			format(OptionsList, sizeof(OptionsList), "%sExit business\n", OptionsList);
			// Show the businessmenu
			ShowPlayerDialog(playerid, DialogBusinessMenu, DIALOG_STYLE_LIST, DialogTitle, OptionsList, "Select", "Cancel");
		}
		else
		    SendClientMessage(playerid, COLOR_RED, "You're not inside a business");
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// This command teleports you to your selected business
COMMAND:gobus(playerid, params[])
{
	// Setup local variables
	new BusinessList[1000], BusID, BusType, Earnings;

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/gobus", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player is not jailed
		if (APlayerData[playerid][PlayerJailed] == 0)
		{
			// Check if the player has a wanted level of less than 3
			if (GetPlayerWantedLevel(playerid) < 3)
			{
				// Check if the player is not inside a vehicle
				if (GetPlayerVehicleID(playerid) == 0)
				{
					// Ask to which business the player wants to port
					for (new i; i < MAX_BUSINESSPERPLAYER; i++)
					{
						// Get the business-id
					    BusID = APlayerData[playerid][Business][i];

						// Check if this businessindex is occupied
						if (BusID != 0)
						{
							// Get the business-type
							BusType = ABusinessData[BusID][BusinessType];
							Earnings = (BusinessTransactionTime - ABusinessData[BusID][LastTransaction]) * ABusinessInteriors[BusType][BusEarnings] * ABusinessData[BusID][BusinessLevel];
							format(BusinessList, 1000, "%s{00FF00}%s{FFFFFF} (earnings: $%i)\n", BusinessList, ABusinessData[BusID][BusinessName], Earnings);
						}
						else
							format(BusinessList, 1000, "%s{FFFFFF}%s{FFFFFF}\n", BusinessList, "Empty business-slot");
					}
					ShowPlayerDialog(playerid, DialogGoBusiness, DIALOG_STYLE_LIST, "Choose the business to go to:", BusinessList, "Select", "Cancel");
				}
				else
					SendClientMessage(playerid, COLOR_RED, "You need to be on-foot to port to your business");
			}
			else
			    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}You cannot use /gobus when you're wanted");
		}
		else
		    SendClientMessage(playerid, COLOR_RED, "You cannot use /gobus when you're in jail");
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Mutes a player (he cannot talk anymore)
COMMAND:mute(playerid, params[])
{
	// Setup local variables
	new Msg[128], Name[24], AdminName[24], Reason[128], OtherPlayer;

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/mute", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 1
		if (APlayerData[playerid][PlayerLevel] >= 1)
		{
			if (sscanf(params, "us[128]", OtherPlayer, Reason)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/mute <Player> <Reason>\"");
			else
			{
				// Check if the otherplayer is online
				if (IsPlayerConnected(OtherPlayer))
				{
					//Check if another player is not muted
					if(APlayerData[OtherPlayer][Muted] == false)
					{
						// Get the player-names
						GetPlayerName(playerid, AdminName, sizeof(AdminName));
						GetPlayerName(OtherPlayer, Name, sizeof(Name));
						// Mute the other player
						APlayerData[OtherPlayer][Muted] = true;
						// Let the other player know that he has been muted
						format(Msg, 128, "{FF0000}You have been muted by {FFFF00}%s {FF0000}for {FFFF00}%s", AdminName, Reason);
						SendClientMessage(OtherPlayer, 0xFFFFFFFF, Msg);
						// Let the admin know who he has muted
						format(Msg, 128, "{00FF00}You have muted {FFFF00}%s", Name);
						SendClientMessage(playerid, 0xFFFFFFFF, Msg);
						// Save the player-stats
						PlayerFile_Save(OtherPlayer);
					}
					else
						SendClientMessage(playerid, 0xFFFFFFFF, "{FF0000}That player is already muted");
				}
				else
				    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}That player isn't online");
			}
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Un-mutes a player (he cannot talk anymore)
COMMAND:unmute(playerid, params[])
{
	// Setup local variables
	new Msg[128], Name[24], AdminName[24], OtherPlayer;

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/unmute", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 1
		if (APlayerData[playerid][PlayerLevel] >= 1)
		{
			if (sscanf(params, "u", OtherPlayer)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/unmute <Player>\"");
			else
			{
				// Check if the otherplayer is online
				if (IsPlayerConnected(OtherPlayer))
				{
					//Check if the another player is muted
					if(APlayerData[OtherPlayer][Muted] == true)
					{
						// Get the player-names
						GetPlayerName(playerid, AdminName, sizeof(AdminName));
						GetPlayerName(OtherPlayer, Name, sizeof(Name));
						// Un-mute the other player
						APlayerData[OtherPlayer][Muted] = false;
						// Let the other player know that he has been un-muted
						format(Msg, 128, "{FF0000}You have been un-muted by {FFFF00}%s", AdminName);
						SendClientMessage(OtherPlayer, 0xFFFFFFFF, Msg);
						// Let the admin know who he has un-muted
						format(Msg, 128, "{00FF00}You have un-muted {FFFF00}%s", Name);
						SendClientMessage(playerid, 0xFFFFFFFF, Msg);
						// Save the player-stats
						PlayerFile_Save(OtherPlayer);
					}
					else
						SendClientMessage(playerid, 0xFFFFFFFF, "{FF0000}That player is not muted");
				}
				else
				    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}That player isn't online");
			}
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Let's the admins see which players are still muted
COMMAND:muted(playerid, params[])
{
	// Setup local variables
	new PlayerList[1000], Name[24];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/muted", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 1
		if (APlayerData[playerid][PlayerLevel] >= 1)
		{
			// Loop through all players
			for (new i; i < MAX_PLAYERS; i++)
			{
				// Check if that player is online
				if (IsPlayerConnected(i))
				{
				    if (APlayerData[i][Muted] == true)
					{
						// Get the player's name
						GetPlayerName(i, Name, sizeof(Name));
						// Add his name and ID to the list
						format(PlayerList, sizeof(PlayerList), "%s%s (ID: %i)\n", PlayerList, Name, i);
					}
				}
			}

			// Show the list of muted players to the admin
			if (strlen(PlayerList) != 0)
				ShowPlayerDialog(playerid, DialogNoResponse, DIALOG_STYLE_LIST, "Muted players:", PlayerList, "OK", "Cancel");
			else
			    SendClientMessage(playerid, COLOR_WHITE, "{00FF00}There are no muted players");
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Freeze a player (he cannot move anymore)
COMMAND:freeze(playerid, params[])
{
	// Setup local variables
	new Msg[128], Name[24], AdminName[24], Reason[128], OtherPlayer, Duration;

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/freeze", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 1
		if (APlayerData[playerid][PlayerLevel] >= 1)
		{
			if (sscanf(params, "uis[128]", OtherPlayer, Duration, Reason)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/freeze <Player> <Duration (in Seconds)> <Reason>\"");
			else
			{
				// Check if the otherplayer is online
				if (IsPlayerConnected(OtherPlayer))
				{
					// Get the player-names
					GetPlayerName(playerid, AdminName, sizeof(AdminName));
					GetPlayerName(OtherPlayer, Name, sizeof(Name));
					// Store the duration for the freeze, freeze him and start the frozentimer
					APlayerData[OtherPlayer][PlayerFrozen] = Duration;
					TogglePlayerControllable(OtherPlayer, 0);
					FreezeTimer[OtherPlayer] = SetTimerEx("Player_FreezeTimer", 1000, true, "i", OtherPlayer);
					// Let the other player know that he has been muted
					format(Msg, 128, "{FF0000}You have been frozen by {FFFF00}%s {FF0000}for {FFFF00}%s", AdminName, Reason);
					SendClientMessage(OtherPlayer, COLOR_WHITE, Msg);
					// Let the admin know who he has muted
					format(Msg, 128, "{00FF00}You have frozen {FFFF00}%s", Name);
					SendClientMessage(playerid, COLOR_WHITE, Msg);
					// Save the player-stats
					PlayerFile_Save(OtherPlayer);
				}
				else
				    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}That player isn't online");
			}
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Unfreeze a player (he can move again)
COMMAND:unfreeze(playerid, params[])
{
	// Setup local variables
	new Msg[128], Name[24], AdminName[24], OtherPlayer;

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/unfreeze", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 1
		if (APlayerData[playerid][PlayerLevel] >= 1)
		{
			if (sscanf(params, "u", OtherPlayer)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/unfreeze <Player>\"");
			else
			{
				// Check if the otherplayer is online
				if (IsPlayerConnected(OtherPlayer))
				{
					// Check if the player is frozen
					if (APlayerData[playerid][PlayerFrozen] >= 1) {
						// Just set the frozen timer to zero
						// the freeze timer will do the rest
						APlayerData[playerid][PlayerFrozen] = 0;

						// Get the player names
						GetPlayerName(playerid, AdminName, sizeof(AdminName));
						GetPlayerName(OtherPlayer, Name, sizeof(Name));
						// Let the other player know that he has been un-frozen
						format(Msg, 128, "{FF0000}You have been un-frozen by {FFFF00}%s {FF0000}for {FFFF00}%s", AdminName);
						SendClientMessage(OtherPlayer, COLOR_WHITE, Msg);
						// Let the admin know who he has un-frozen
						format(Msg, 128, "{00FF00}You have un-frozen {FFFF00}%s", Name);
						SendClientMessage(playerid, COLOR_WHITE, Msg);
					}
					else
						SendClientMessage(playerid, COLOR_RED, "That player has not been frozen");
				}
				else
				    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}That player isn't online");
			}
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Transfer money to another player
COMMAND:givecash(playerid, params[])
{
	// Setup local variables
	new Msg[128], Name[24], OtherName[24], OtherPlayer, Money;

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/givecash", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		if (sscanf(params, "ui", OtherPlayer, Money)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/givecash <Player> <Money>\"");
		else
		{
			if (OtherPlayer == playerid)
				return SendClientMessage(playerid, COLOR_WHITE, "{FF0000}You can't give money to yourself");

			// Check if the otherplayer is online
			if (IsPlayerConnected(OtherPlayer))
			{
				// Get the player-names
				GetPlayerName(playerid, Name, sizeof(Name));
				GetPlayerName(OtherPlayer, OtherName, sizeof(OtherName));

				// Check if the money has a positive value (to prevent stealing money using negative values)
				if (Money > 0)
				{
					// Check if the player has enough money
					if (APlayerData[playerid][PlayerMoney] >= Money)
					{
						// Transfer the money
						RewardPlayer(playerid, -Money, 0);
						RewardPlayer(OtherPlayer, Money, 0);
						// Let the other player know that he has received money
						format(Msg, 128, "{00FF00}You have received {FFFF00}$%i{00FF00} from {FFFF00}%s", Money, Name);
						SendClientMessage(OtherPlayer, COLOR_WHITE, Msg);
						// Let the player know he gave money to somebody else
						format(Msg, 128, "{00FF00}You gave {FFFF00}$%i{00FF00} to {FFFF00}%s", Money, OtherName);
						SendClientMessage(playerid, COLOR_WHITE, Msg);
						// Save the player-stats
						PlayerFile_Save(playerid);
						PlayerFile_Save(OtherPlayer);
					}
					else
					    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}You don't have enough money");
				}
				else
				    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}You must use values higher than 0");
			}
			else
			    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}That player isn't online");
		}
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Respawns the given vehicle
COMMAND:respawnvehicle(playerid, params[])
{
	// Setup local variables
	new vid, Msg[128];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/respawnvehicle", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 1
		if (APlayerData[playerid][PlayerLevel] >= 1)
		{
			if (sscanf(params, "i", vid)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/respawnvehicle <Vehicle-id>\"");
			else
			{
				// Check if the vehicle-id is valid
				if ((vid > 0) && (vid < 2000))
				{
					// Check if the vehicle exists
					if (AVehicleData[vid][Model] != 0)
					{
					    // Force the vehicle to respawn
						SetVehicleToRespawn(vid);
						// Let the player know he respawned the vehicle
						format(Msg, 128, "{00FF00}You've respawned vehicle {FFFF00}%i", vid);
						SendClientMessage(playerid, COLOR_WHITE, Msg);
					}
				}
				else
				    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}You have to use a vehicle-id between 1 and 1999");
			}
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Creates a spikestrip (can only be used by police players)
COMMAND:createspike(playerid, params[])
{
	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/createspike", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player if a police player
		if (APlayerData[playerid][PlayerClass] == ClassPolice)
		{
			// Check if the player is on foot
			if (GetPlayerVehicleSeat(playerid) == -1)
			{
				// Try to create a spikestrip
				SpikeStrip_Create(playerid);
			}
			else
			    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}You must be on foot to place a spikestrip");
		}
		else
			return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// This command ejects a player from his vehicle
COMMAND:eject(playerid, params[])
{
	// Setup local variables
	new DriverName[24], Msg[128], OtherPlayer, OtherPlayerName[24];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/eject", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		if (sscanf(params, "u", OtherPlayer)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/eject <Player>\"");
		else
		{
			// Check if the player trys to eject himself
			if (playerid == OtherPlayer) {
			    // Check if the player is logged in
				if (APlayerData[OtherPlayer][LoggedIn] == true)
				{
					// Check if the player is driving a vehicle
					if (GetPlayerVehicleSeat(playerid) != 0)
					{
						// Check if the other player is in the same vehicle
						if (GetPlayerVehicleID(OtherPlayer) != GetPlayerVehicleID(playerid)) {
							// Get the names of the players
							GetPlayerName(playerid, DriverName, sizeof(DriverName));
							GetPlayerName(OtherPlayer, OtherPlayerName, sizeof(OtherPlayerName));
							// Eject the player from the vehicle
							RemovePlayerFromVehicle(OtherPlayer);
							// Let the player know he's been ejected from his vehicle
							format(Msg, 128, "{FF0000}You've been ejected from the vehicle by {FFFF00}%s", DriverName);
							SendClientMessage(OtherPlayer, COLOR_WHITE, Msg);
							// Inform the driver that he ejected the player
							format(Msg, 128, "{00FF00}You've ejected {FFFF00}%s{00FF00} from your vehicle", OtherPlayerName);
							SendClientMessage(playerid, COLOR_WHITE, Msg);
						}
						else
							SendClientMessage(playerid, COLOR_WHITE, "{FF0000}That player is not in your vehicle");
					}
					else
					    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}You're not the driver of a vehicle");
				}
				else
				    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}That player isn't connected");
			}
			else
				SendClientMessage(playerid, COLOR_WHITE, "{FF0000}You can not eject yourself from the vehicle");
		}
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// This command lets you talk only to players of the same class
COMMAND:radio(playerid, params[])
{
	// Setup local variables
	new Name[24], Msg[128], Message[128], ClassName[24];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/radio", params);

	// Get the player's name
	GetPlayerName(playerid, Name, sizeof(Name));

	// Get the player's class name
	switch (APlayerData[playerid][PlayerClass])
	{
		case ClassTruckDriver: // Truckdriver class
			ClassName = "Trucker";
		case ClassBusDriver: // BusDriver class
			ClassName = "Bus Driver";
		case ClassPilot: // Pilot class
			ClassName = "Pilot";
		case ClassMafia: // Mafia class
			ClassName = "Mafia";
		case ClassCourier: // Courier class
			ClassName = "Courier";
		case ClassRoadWorker: // Roadworker class
			ClassName = "Roadworker";
	}

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		if (sscanf(params, "s[128]", Message)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/radio <Message>\"");
		else
		{
			// Loop through all players
			for (new i; i < MAX_PLAYERS; i++)
			{
			    // Check if the player is logged in
				if (APlayerData[i][LoggedIn] == true)
				{
				    // Check if the other player has the same class
				    if (APlayerData[i][PlayerClass] == APlayerData[playerid][PlayerClass])
				    {
						format(Msg, 128, "{A0A0A0}(%s Chat) {D0D0D0}%s: {FFFFFF}%s", ClassName, Name, Message);
						SendClientMessage(i, COLOR_WHITE, Msg);
				    }
				}
			}
		}
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// This command lets you talk only to admin players
COMMAND:achat(playerid, params[])
{
	// Setup local variables
	new Name[24], Msg[128], Message[128];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/achat", params);

	// Get the player's name
	GetPlayerName(playerid, Name, sizeof(Name));

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player is an admin
		if (APlayerData[playerid][PlayerLevel] > 0)
		{
			if (sscanf(params, "s[128]", Message)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/achat <Message>\"");
			else
			{
				// Loop through all players
				for (new i; i < MAX_PLAYERS; i++)
				{
				    // Check if the player is logged in
					if (APlayerData[i][LoggedIn] == true)
					{
					    // Check if the other player is also an admin
					    if (APlayerData[i][PlayerLevel] > 0)
					    {
							format(Msg, 128, "{A0A0A0}(Admin Chat) {D0D0D0}%s: {FFFFFF}%s", Name, Message);
							SendClientMessage(i, COLOR_WHITE, Msg);
					    }
					}
				}
			}
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Clamp an owned vehicle (remove it completely)
COMMAND:clamp(playerid, params[])
{
	// Setup local variables
	new vid, Reason[128], HouseID, HouseOwner, Name[24], AdminName[24], Msg[128];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/clamp", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 5
		if (APlayerData[playerid][PlayerLevel] >= 5)
		{
			if (sscanf(params, "is[128]", vid, Reason)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/clamp <Vehicle ID> <Reason>\"");
			else
			{
			    // Get the HouseID to which this vehicle belongs
				HouseID = AVehicleData[vid][BelongsToHouse];

				// Check if the vehicle belongs to a house
				if (HouseID != 0)
				{
					//Also set the new location of the vehicle inside the KACC Military fuels hangar
					AVehicleData[vid][SpawnX] = 2585.0;
					AVehicleData[vid][SpawnY] = 2829.0;
					AVehicleData[vid][SpawnZ] = 10.9;
					AVehicleData[vid][SpawnRot] = 0.0;

					// Search all the players to see who owns the house
					for (new i; i < MAX_PLAYERS; i++)
					{
						// Loop through all the houses this player owns
						for(new j; j < MAX_HOUSESPERPLAYER; j++)
						{
						    // Check if the player owns this house
							if (APlayerData[i][Houses][j] == HouseID)
							{
							    // Store the houseowner
							    HouseOwner = i;

								// Find the CarSlot where this vehicle exists
								for (new CarSlot; CarSlot < 10; CarSlot++)
								{
									// Check if the vehicle is stored in this carslot
									if (AHouseData[HouseID][VehicleIDs][CarSlot] == vid)
									{
										vid = House_ReplaceVehicle(HouseID, CarSlot); // Re-create the vehicle at the KACC Military fuels hangar
										// Set the vehicle as clamped (clamping needs to be done after re-creating the vehice, otherwise the wrong id can be clamped)
										AVehicleData[vid][Clamped] = true;
									}
								}

								break;
							}
						}
					}

					// Get the name of the admin and owner of the house
					GetPlayerName(playerid, AdminName, sizeof(AdminName));
					GetPlayerName(HouseOwner, Name, sizeof(Name));

					// Let the admin know who's vehicle he deleted
					format(Msg, 128, "{00FF00}You've clamped a vehicle that was owned by {FFFF00}%s", Name);
					SendClientMessage(playerid, COLOR_WHITE, Msg);
					// Let the player know who deleted his vehicle
					format(Msg, 128, "{FF0000}A vehicle you owned was clamped by admin {FFFF00}%s", AdminName);
					SendClientMessage(HouseOwner, COLOR_WHITE, Msg);
					format(Msg, 128, "{FF0000}It belongs to house: {FFFF00}%s", AHouseData[HouseID][HouseName]);
					SendClientMessage(HouseOwner, COLOR_WHITE, Msg);
					format(Msg, 128, "{FF0000}Reason: {FFFF00}%s", Reason);
					SendClientMessage(HouseOwner, COLOR_WHITE, Msg);
					// Save the player's datafile (and his houses/businesses/vehicles)
					PlayerFile_Save(HouseOwner);
				}
				else
				    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}That vehicle doesn't belong to a player, you cannot clamp it");
			}
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// This command can be used to unclamp all your vehicles at once
COMMAND:unclamp(playerid, params[])
{
	// Setup local variables
	new HouseID, vid, ClampedVehicles, UnclampPrice, Msg[128];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/unclamp", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Loop through all vehicles owned by this player
		for (new HouseSlot; HouseSlot < MAX_HOUSESPERPLAYER; HouseSlot++)
		{
			// Get the HouseID that exists on this index
			HouseID = APlayerData[playerid][Houses][HouseSlot];

			// check if this house-slot is used
			if (HouseID != 0)
			{
				// Loop through all vehicles assigned to this house
				for (new CarSlot; CarSlot < 10; CarSlot++)
				{
					// Get the vehicle-id of the vehicle in this carslot
					vid = AHouseData[HouseID][VehicleIDs][CarSlot];

					// Check if this carslot is used
					if (vid != 0)
					{
						// Check if this vehicle is clamped
						if (AVehicleData[vid][Clamped] == true)
						    ClampedVehicles++; // Count the clamped vehicles
					}
				}
			}
		}

		// Check if there were any clamped vehicles
		if (ClampedVehicles > 0)
		{
			// Calculate the price to unclamp all the player's vehicles
			UnclampPrice = ClampedVehicles * UnclampPricePerVehicle;

			// Construct the message to inform the player how many vehicles have been clamped and how much it costs to un-clamp them
			format(Msg, 128, "You have %i clamped vehicles, it will cost you $%i to unclamp them", ClampedVehicles, UnclampPrice);
		    // Show a dialog that informs the player how many vehicles have been clamped and how much it costs to un-clamp them
			ShowPlayerDialog(playerid, DialogUnclampVehicles, DIALOG_STYLE_MSGBOX, TXT_AreYouSure, Msg, "Unclamp", TXT_DialogButtonCancel);
		}
		else
		    SendClientMessage(playerid, COLOR_RED, "You don't have any clamped vehicles");
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// This command displays the rules of the server
COMMAND:rules(playerid, params[])
{
	// Setup local variables
	new Msg[2000];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/rules", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Construct the rules
		format(Msg, 2000, "%s1. Always drive on the right side of the road\n", Msg);
		format(Msg, 2000, "%s2. No flaming or disrespecting other players\n", Msg);
		format(Msg, 2000, "%s3. Main language is english\n", Msg);
		format(Msg, 2000, "%s4. Don't use any hacks, you'll be banned\n", Msg);
		format(Msg, 2000, "%s5. No spamming on the chat\n", Msg);
		format(Msg, 2000, "%s6. No car-jacking allowed\n", Msg);
	    // Show a dialog that shows the rules
		ShowPlayerDialog(playerid, DialogRules, DIALOG_STYLE_MSGBOX, "Rules of the server:", Msg, "Accept", TXT_DialogButtonCancel);
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Report a player for breaking the rules
COMMAND:report(playerid, params[])
{
	// Setup local variables
	new OtherPlayer, Name[24], Reason[128], Msg[128];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/report", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		if (sscanf(params, "us[128]", OtherPlayer, Reason)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/report <Player> <Message>\"");
		else
		{
			if (OtherPlayer == playerid)
				return SendClientMessage(playerid, COLOR_WHITE, "{FF0000}You can't report yourself");

		    // Check if that other player is logged in
		    if (APlayerData[OtherPlayer][LoggedIn] == true)
		    {
				// Send the report to all admins and add the report to the report-list so admins can review it
				SendReportToAdmins(OtherPlayer, Reason);

				// Get the name of the offender
				GetPlayerName(OtherPlayer, Name, sizeof(Name));
				// Let the player know he reported the other player
				format(Msg, 128, "{00FF00}You've reported {FFFF00}%s", Name);
				SendClientMessage(playerid, COLOR_WHITE, Msg);
			}
			else
			    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}That player isn't connected");
		}
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}


// Lets the admins see the list of reports
COMMAND:rep(playerid, params[])
{
	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/rep", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 1
		if (APlayerData[playerid][PlayerLevel] >= 1)
		{
			// Add the first report to the list (if it exists)
			if (AReports[0][ReportUsed] == true)
			    format(ReportList, 5000, "%s: %s\n", AReports[0][ReportName], AReports[0][ReportReason]);
			else
				return SendClientMessage(playerid, COLOR_RED, "There are no reports");

			// Construct the report-dialog
			for (new i = 1; i < 50; i++)
			{
				// Check if the ReportID has been used already
				if (AReports[i][ReportUsed] == true)
				{
				    format(ReportList, 5000, "%s%s: %s\n", ReportList, AReports[i][ReportName], AReports[i][ReportReason]);
				}
			}

			// Show all the reports
			ShowPlayerDialog(playerid, DialogReports, DIALOG_STYLE_LIST, "Reports list:", ReportList, "OK", "Cancel");
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Enables the trucker to overload himself
COMMAND:overload(playerid, params[])
{
	// Setup local variables
	new vModel, bool:ValidOverLoad = false, Float:x, Float:y, Float:z, Name[24], Msg[128];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/overload", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the playeris a trucker
		if (APlayerData[playerid][PlayerClass] == ClassTruckDriver)
		{
			// Check if he has already started a job
			if (APlayerData[playerid][JobStarted] == true)
			{
				// Check if the player has already loaded his load
				if (APlayerData[playerid][JobStep] == 2)
				{
					// Check if the player isn't overloaded already
					if (APlayerData[playerid][Overloaded] == false)
					{
						// Get the coordinates of the loading point
						x = ALocations[APlayerData[playerid][JobLoc1]][LocX];
						y = ALocations[APlayerData[playerid][JobLoc1]][LocY];
						z = ALocations[APlayerData[playerid][JobLoc1]][LocZ];

					    // Check if the player is still near the loading point
					    if (IsPlayerInRangeOfPoint(playerid, 25.0, x, y, z))
					    {
							// Get the vehicle-model of the player's vehicle
							vModel = GetVehicleModel(APlayerData[playerid][VehicleID]);

							// Check if the trucker is driving a trucking vehicle that can be overloaded
							switch (vModel)
							{
								case VehicleFlatbed, VehicleDFT30: ValidOverLoad = true; // Flatbed and DFT-30 can be overloaded
								case VehicleLineRunner, VehicleTanker, VehicleRoadTrain:
								{
									switch (GetVehicleModel(APlayerData[playerid][TrailerID]))
									{
										case VehicleTrailerCargo, VehicleTrailerCargo2, VehicleTrailerOre: ValidOverLoad = true; // Cargo and ore trailer can be overloaded
									}
								}
							}

							// Check if the vehicle is valid for overloading
							if (ValidOverLoad == true)
							{
								// Set overloaded for this player to True
							    APlayerData[playerid][Overloaded] = true;
							    // Add 2 to the player's wanted level
							    SetPlayerWantedLevel(playerid, GetPlayerWantedLevel(playerid) + 2);
							    // Let the player know he has been overloaded now
							    SendClientMessage(playerid, COLOR_WHITE, "{00FF00}You have overloaded your truck, watch out for the police");
								// Inform the police this trucker is overloaded
								GetPlayerName(playerid, Name, sizeof(Name));
								format(Msg, 128, "{00FF00}Trucker {FFFF00}%s{00FF00} is overloaded, pursue and fine him", Name);
								Police_SendMessage(Msg);
							}
							else
							    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}Your vehicle cannot be overloaded");
						}
						else
						    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}You must be near the loading point to overload your truck");
					}
					else
					    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}You are overloaded already");
				}
				else
				    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}You must load your truck first");
			}
			else
			    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}You haven't started a job yet");
		}
		else
			return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// This command resets a player's money, score and stats to 0 (if chosen)
COMMAND:resetplayer(playerid, params[])
{
	// Setup local variables
	new Name[24], AdminName[24], Reason[128], Msg[128], OtherPlayer, ClearMoney, ClearScore, ClearStats;

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/resetplayer", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 5
		if (APlayerData[playerid][PlayerLevel] >= 5)
		{
			if (sscanf(params, "uiiis[128]", OtherPlayer, ClearMoney, ClearScore, ClearStats, Reason)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/resetplayer <Player> <Money (0/1)> <Score (0/1)> <Stats (0/1)> <Reason>\"");
			else
			{
			    // Check if the other player is logged in
				if (APlayerData[OtherPlayer][LoggedIn] == true)
				{
					// Check if there is at least one parameter given to be cleared, otherwise exit the command
					if ((ClearMoney + ClearScore + ClearStats) == 0)
						return 1;

					// Get the names of both players
					GetPlayerName(playerid, AdminName, sizeof(AdminName));
					GetPlayerName(OtherPlayer, Name, sizeof(Name));

					// Reset the other player's money to 0
					if (ClearMoney == 1)
					{
						APlayerData[OtherPlayer][PlayerMoney] = 0;
						format(Msg, 128, "{FF0000}Your money has been reset by {FFFF00}%s", AdminName);
						SendClientMessage(OtherPlayer, COLOR_WHITE, Msg);
						format(Msg, 128, "{00FF00}You've reset the money of player {FFFF00}%s", Name);
						SendClientMessage(playerid, COLOR_WHITE, Msg);
					}

					// Reset the other player's score to 0
					if (ClearScore == 1)
					{
						APlayerData[OtherPlayer][PlayerScore] = 0;
						format(Msg, 128, "{FF0000}Your score has been reset by {FFFF00}%s", AdminName);
						SendClientMessage(OtherPlayer, COLOR_WHITE, Msg);
						format(Msg, 128, "{00FF00}You've reset the score of player {FFFF00}%s", Name);
						SendClientMessage(playerid, COLOR_WHITE, Msg);
					}

					// Reset the other player's stats to 0
					if (ClearStats == 1)
					{
						APlayerData[OtherPlayer][StatsTruckerJobs] = 0;
						APlayerData[OtherPlayer][StatsConvoyJobs] = 0;
						APlayerData[OtherPlayer][StatsBusDriverJobs] = 0;
						APlayerData[OtherPlayer][StatsPilotJobs] = 0;
						APlayerData[OtherPlayer][StatsMafiaJobs] = 0;
						APlayerData[OtherPlayer][StatsMafiaStolen] = 0;
						APlayerData[OtherPlayer][StatsPoliceFined] = 0;
						APlayerData[OtherPlayer][StatsPoliceJailed] = 0;
						APlayerData[OtherPlayer][StatsCourierJobs] = 0;
						APlayerData[OtherPlayer][StatsAssistance] = 0;
						APlayerData[OtherPlayer][StatsRoadworkerJobs] = 0;
						APlayerData[OtherPlayer][StatsMetersDriven] = 0.0;
						format(Msg, 128, "{FF0000}Your stats have been reset by {FFFF00}%s", AdminName);
						SendClientMessage(OtherPlayer, COLOR_WHITE, Msg);
						format(Msg, 128, "{00FF00}You've reset the stats of player {FFFF00}%s", Name);
						SendClientMessage(playerid, COLOR_WHITE, Msg);
					}

					// Let the other player know the reason too
					format(Msg, 128, "{FF0000}Reason: {FFFF00}%s", Reason);
					SendClientMessage(OtherPlayer, COLOR_WHITE, Msg);

					// Save the other player's account
					PlayerFile_Save(OtherPlayer);
				}
				else
				    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}That player isn't connected");
			}
		}
		else
			return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// This command searches every house and business that the player owns and restores the data for it in the player's account
COMMAND:fixplayer(playerid, params[])
{
	// Setup local variables
	new Name[24], AdminName[24], Msg[128], OtherPlayer;

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/fixplayer", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 5
		if (APlayerData[playerid][PlayerLevel] >= 5)
		{
			if (sscanf(params, "u", OtherPlayer)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/fixplayer <Player>\"");
			else
			{
			    // Check if the other player is logged in
				if (APlayerData[OtherPlayer][LoggedIn] == true)
				{
					// Get the names of both players
					GetPlayerName(playerid, AdminName, sizeof(AdminName));
					GetPlayerName(OtherPlayer, Name, sizeof(Name));

					// Clear the houses and businesses that the player owns right now
					for (new i; i < MAX_HOUSESPERPLAYER; i++)
					    APlayerData[OtherPlayer][Houses][i] = 0;

					for (new i; i < MAX_BUSINESSPERPLAYER; i++)
					    APlayerData[OtherPlayer][Business][i] = 0;

					// Now search through all houses and re-add them to the player
					for (new HouseID = 1; HouseID < MAX_HOUSES; HouseID++)
						if (AHouseData[HouseID][Owned] == true) // Check if the house is owned by somebody
							if (strcmp(AHouseData[HouseID][Owner], Name, false) == 0) // Check if the player is the owner of the house
							{
								for (new i; i < MAX_HOUSESPERPLAYER; i++) // Loop through all houses the player owns
								    if (APlayerData[OtherPlayer][Houses][i] == 0) // Check if the houseslot is free
								    {
										APlayerData[OtherPlayer][Houses][i] = HouseID; // Store the HouseID
										break; // Stop searching for more free slots
								    }
							}

					// Now search through all businesses and re-add them to the player
					for (new BusID = 1; BusID < MAX_BUSINESS; BusID++)
						if (ABusinessData[BusID][Owned] == true) // Check if the business is owner by someone
							if (strcmp(ABusinessData[BusID][Owner], Name, false) == 0) // Check if the player is the owner of the business
							{
								for (new i; i < MAX_BUSINESSPERPLAYER; i++) // Loop through all businesses the player owns
								    if (APlayerData[OtherPlayer][Business][i] == 0) // Check if the businessslot is free
								    {
										APlayerData[OtherPlayer][Business][i] = BusID; // Store the BusID
										break; // Stop searching for more free slots
								    }
							}

					// Let the other player know his property has been restored
					format(Msg, 128, "{00FF00}Your property has been restored by: {FFFF00}%s", AdminName);
					SendClientMessage(OtherPlayer, COLOR_WHITE, Msg);

					// Save the other player's account
					PlayerFile_Save(OtherPlayer);
				}
				else
				    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}That player isn't connected");
			}
		}
		else
			return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// This command sets a skin for a player
COMMAND:setskin(playerid, params[])
{
	// Setup local variables
	new Msg[128], AdminName[24], OtherName[24], OtherPlayer, Skin;

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/setskin", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 4
		if (APlayerData[playerid][PlayerLevel] >= 4)
		{
			if (sscanf(params, "ui", OtherPlayer, Skin)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/setskin <Player> <Skin ID (0-299)>\"");
			else
			{
				// Check if the otherplayer is online
				if (IsPlayerConnected(OtherPlayer))
				{
					// Check if the player entered a valid skin-id
					if ((Skin >= 0) && (Skin <= 299))
					{
					    // Set the skin for the player
					    SetPlayerSkin(OtherPlayer, Skin);

						// Get the name of the admin and the other player
						GetPlayerName(playerid, AdminName, sizeof(AdminName));
						GetPlayerName(OtherPlayer, OtherName, sizeof(OtherName));

						// Let the admin know who he changed the skin for
						format(Msg, 128, "You've changed the skin for {FFFF00}%s {00FF00}to {FFFF00}%i", OtherName, Skin);
						SendClientMessage(playerid, COLOR_GREEN, Msg);

						// Let the other player know about his new outfit
						format(Msg, 128, "Your skin has been changed by admin {FFFF00}%s {00FF00}to {FFFF00}%i", AdminName, Skin);
						SendClientMessage(OtherPlayer, COLOR_GREEN, Msg);
					}
					else
						SendClientMessage(playerid, COLOR_WHITE, "{FF0000}Invalid skin-id, you can only use skins 0-299");
				}
				else
				    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}That player isn't online");
			}
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Bans a player by his ip
COMMAND:ipban(playerid, params[])
{
	// Setup local variables
	new PlayerToBan, Reason[128], Msg[128], Name[24], AdminName[24];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/ipban", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 3
		if (APlayerData[playerid][PlayerLevel] >= 3)
		{
			if (sscanf(params, "us[128]", PlayerToBan, Reason))
				SendClientMessage(playerid, COLOR_RED, "Usage: \"/ipban <Player> <Reason>\"");
			else
			{
				if (IsPlayerConnected(PlayerToBan))
				{
					// Get the names of the player and the admin who executed the ban
					GetPlayerName(playerid, AdminName, sizeof(AdminName));
					GetPlayerName(PlayerToBan, Name, sizeof(Name));

					// Inform the player about his ban
					format(Msg, 128, "{FF0000}You have been ip-banned permanently by {FFFF00}%s", AdminName);
					SendClientMessage(PlayerToBan, COLOR_WHITE, Msg);
					format(Msg, 128, "{FF0000}Reason: {FFFF00}%s", Reason);
					SendClientMessage(PlayerToBan, COLOR_WHITE, Msg);

					// Ban the player with a reason
					SetTimerEx("TimedBanEx", (750 + GetPlayerPing(PlayerToBan)), false, "is", PlayerToBan, Reason);

					// Inform everybody else which player was ip-banned
					format(Msg, 128, "{808080}%s %s has ip-banned {FFFF00}%s", AdminLevelName[APlayerData[playerid][PlayerLevel]], AdminName, Name);
					SendClientMessageToAll(COLOR_WHITE, Msg);
				}
				else
					SendClientMessage(playerid, COLOR_RED, "That player isn't connected");
			}
		}
		else
		    return 0;
	}
	else
	    return 0;

	return 1;
}

// Bans a player's entire range of IP addresses (the last part of the IP-address will be from 0 to 255)
COMMAND:rangeban(playerid, params[])
{
	// Setup local variables
	new PlayerToBan, PlayerIP[16], FirstPartsOfIP[16], BanCmd[24], Reason[128], Msg[128], Name[24], AdminName[24];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/rangeban", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 3
		if (APlayerData[playerid][PlayerLevel] >= 3)
		{
			if (sscanf(params, "us[128]", PlayerToBan, Reason))
				SendClientMessage(playerid, COLOR_RED, "Usage: \"/rangeban <Player> <Reason>\"");
			else
			{
				if (IsPlayerConnected(PlayerToBan))
				{
					// Get the names of the player and the admin who executed the ban
					GetPlayerName(playerid, AdminName, sizeof(AdminName));
					GetPlayerName(PlayerToBan, Name, sizeof(Name));

					// Get the player's IP-address
					GetPlayerIp(PlayerToBan, PlayerIP, 16);

					// Inform the player about his ban
					format(Msg, 128, "{FF0000}You have been ip-range-banned permanently by {FFFF00}%s", AdminName);
					SendClientMessage(PlayerToBan, COLOR_WHITE, Msg);
					format(Msg, 128, "{FF0000}Reason: {FFFF00}%s", Reason);
					SendClientMessage(PlayerToBan, COLOR_WHITE, Msg);

					// Get the first three digits from the player's ip, so the fourth part can be added from 0 to 255
					FirstPartsOfIP = GetFirstThreeDigitsFromIP(PlayerIP);
					// Ban the entire range of IP-addresses of the player
					for (new i; i < 256; i++)
					{
					    format(BanCmd, 24, "banip %s%i", FirstPartsOfIP, i); // Construct the RCon command to ban every IP
                        SendRconCommand(BanCmd); // Execute the command
					}
					// Finally kick the player (the RCon command doesn't kick you out automatically)
					SetTimerEx("TimedKick", (750 + GetPlayerPing(PlayerToBan)), false, "i", PlayerToBan);

					// Inform everybody else which player was ip-range-banned
					format(Msg, 128, "{808080}%s %s has ip-range-banned {FFFF00}%s", AdminLevelName[APlayerData[playerid][PlayerLevel]], AdminName, Name);
					SendClientMessageToAll(COLOR_WHITE, Msg);
				}
				else
					SendClientMessage(playerid, 0xFFFFFFFF, "{FF0000}That player isn't online");
			}
		}
		else
		    return 0;
	}
	else
	    return 0;

	return 1;
}

// Sets the score of another player
COMMAND:setscore(playerid, params[])
{
	// Setup local variables
	new Msg[128], Name[24], OtherName[24], OtherPlayer, pScore;

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/setscore", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 3
		if (APlayerData[playerid][PlayerLevel] >= 3)
		{
			if (sscanf(params, "ui", OtherPlayer, pScore)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/setscore <Player> <Amount>\"");
			else
			{
				// Check if the otherplayer is online
				if (IsPlayerConnected(OtherPlayer))
				{
					// Get the player-names
					GetPlayerName(playerid, Name, sizeof(Name));
					GetPlayerName(OtherPlayer, OtherName, sizeof(OtherName));

					// Set the other player's score
					APlayerData[OtherPlayer][PlayerScore] = pScore;
					// Let the other player know that his score has been changed
					format(Msg, 128, "{00FF00}Your score has been set to {FFFF00}%i{00FF00} by {FFFF00}%s", pScore, Name);
					SendClientMessage(OtherPlayer, COLOR_WHITE, Msg);
					// Let the player know he has set the score of the other player
					format(Msg, 128, "{00FF00}You've set the score of {FFFF00}%s{00FF00} to {FFFF00}%i", OtherName, pScore);
					SendClientMessage(playerid, COLOR_WHITE, Msg);
					// Save the other player's account
					PlayerFile_Save(OtherPlayer);
				}
				else
				    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}That player isn't online");
			}
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Sets the money of another player
COMMAND:setcash(playerid, params[])
{
	// Setup local variables
	new Msg[128], Name[24], OtherName[24], OtherPlayer, pMoney;

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/setcash", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 3
		if (APlayerData[playerid][PlayerLevel] >= 3)
		{
			if (sscanf(params, "ui", OtherPlayer, pMoney)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/setcash <Player> <Amount>\"");
			else
			{
				// Check if the otherplayer is online
				if (IsPlayerConnected(OtherPlayer))
				{
					// Get the player-names
					GetPlayerName(playerid, Name, sizeof(Name));
					GetPlayerName(OtherPlayer, OtherName, sizeof(OtherName));

					// Set the other player's money
					APlayerData[OtherPlayer][PlayerMoney] = pMoney;
					// Let the other player know that his money has been changed
					format(Msg, 128, "{00FF00}Your cash has been set to {FFFF00}%i{00FF00} by {FFFF00}%s", pMoney, Name);
					SendClientMessage(OtherPlayer, COLOR_WHITE, Msg);
					// Let the player know he has set the money of the other player
					format(Msg, 128, "{00FF00}You've set the cash of {FFFF00}%s{00FF00} to {FFFF00}%i", OtherName, pMoney);
					SendClientMessage(playerid, COLOR_WHITE, Msg);
					// Save the other player's account
					PlayerFile_Save(OtherPlayer);
				}
				else
				    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}That player isn't online");
			}
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// Allows the player to setup a bank account, login to his bank account, or use his bank account after he logged in to his bank account
COMMAND:bank(playerid, params[])
{
	// Setup local variables
	new file[100], Name[24];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/bank", params);

    // Get the playername
	format(Name, sizeof(Name), APlayerData[playerid][PlayerName]);
	// Construct the complete filename for this player's bank-account
	format(file, sizeof(file), BankFile, Name);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player doesn't have a bank account
		if (!fexist(file))
		{
			// Ask for a password to setup his bank account
			ShowPlayerDialog(playerid, DialogBankPasswordRegister, DIALOG_STYLE_PASSWORD, "Enter password", "Please enter a password to register your bank account:", TXT_DialogButtonSelect, TXT_DialogButtonCancel);
		}
		else // The player has a bank account
		{
			// If the player hasn't logged in to his bank account yet
			if (APlayerData[playerid][BankLoggedIn] == false)
			{
				// Ask for the password to login to his bank account
				ShowPlayerDialog(playerid, DialogBankPasswordLogin, DIALOG_STYLE_PASSWORD, "Enter password", "Please enter your password to login to your bank account:", TXT_DialogButtonSelect, TXT_DialogButtonCancel);
			}
			else // The player has logged in to his bank account already
			{
			    // Show the main bank menu dialog
				ShowBankMenu(playerid);
			}
		}
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// This command lists all help-items for which the player can get information about it
COMMAND:help(playerid, params[])
{
	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/help", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Create the dialog that lists all help-items
        HelpList_Create(playerid);
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// This command allows you to change your password for logging in
COMMAND:changepassword(playerid, params[])
{
	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/changepassword", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Show the dialog where the player must enter his old password
		ShowPlayerDialog(playerid, DialogOldPassword, DIALOG_STYLE_PASSWORD, "Enter old password:", "Enter your old password here:", "OK", "Cancel");
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// This command opens a menu where you can choose neons lights for your vehicle
COMMAND:neon(playerid, params[])
{
	// Setup local variables
	new OptionsList[200], DialogTitle[200];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/neon", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player is the driver of a vehicle
		if (GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
		{
			format(DialogTitle, sizeof(DialogTitle), "Select neon lights:");

			format(OptionsList, sizeof(OptionsList), "%sPolice lights\n", OptionsList);
			format(OptionsList, sizeof(OptionsList), "%sRed lights\n", OptionsList);
			format(OptionsList, sizeof(OptionsList), "%sBlue lights\n", OptionsList);
			format(OptionsList, sizeof(OptionsList), "%sGreen lights\n", OptionsList);
			format(OptionsList, sizeof(OptionsList), "%sYellow lights\n", OptionsList);
			format(OptionsList, sizeof(OptionsList), "%sPink lights\n", OptionsList);
			format(OptionsList, sizeof(OptionsList), "%sWhite lights\n", OptionsList);
			format(OptionsList, sizeof(OptionsList), "%sRemove neons\n", OptionsList);
			// Show the neon menu
			ShowPlayerDialog(playerid, DialogNeon, DIALOG_STYLE_LIST, DialogTitle, OptionsList, "Select", "Cancel");
		}
		else
		    SendClientMessage(playerid, COLOR_RED, "You're not the driver of a vehicle");
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}

// This command shows the current bonus mission
COMMAND:bonus(playerid, params[])
{
	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/bonus", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player is a trucker
		if (APlayerData[playerid][PlayerClass] == ClassTruckDriver)
			ShowRandomBonusMission(); // Show the bonus mission again for all truckers (and perhaps generate a new one)
		else
		    return SendClientMessage(playerid, COLOR_RED, "You are not a truck driver!");
	}
	else
	    return SendClientMessage(playerid, COLOR_RED, "You must login before using this command");

	// Let the server know that this was a valid command
	return 1;
}

// This command gives a truckers license to a player
COMMAND:givelicense(playerid,params[])
{
	// Setup local variables
	new OtherPlayer, Msg[128], Name[24], OtherName[24];

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/givelicense", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 3
		if (APlayerData[playerid][PlayerLevel] >= 3)
		{
			if (sscanf(params, "u", OtherPlayer)) SendClientMessage(playerid, COLOR_RED, "Usage: \"/givelicense <Player>\"");
			else
			{
			    // Check if the other player is logged in
			    if (APlayerData[OtherPlayer][LoggedIn] == true)
			    {
			    	if (1 == APlayerData[OtherPlayer][TruckerLicense])
			    		return SendClientMessage(playerid, COLOR_WHITE, "{FF0000}That player has already a trucker license");

					// Get the admin's name
					GetPlayerName(playerid, Name, sizeof(Name));
					// Get the other player's name
					GetPlayerName(OtherPlayer, OtherName, sizeof(OtherName));
					// Give the other player a truckers license
					APlayerData[OtherPlayer][TruckerLicense] = 1;

					// Save the player-file (and his houses)
					PlayerFile_Save(OtherPlayer);

					// Inform the admin that you've given a player a truckers license
					format(Msg, sizeof(Msg), "{00FF00}You've given {FFFF00}%s{00FF00} a trucker's license", OtherName);
					SendClientMessage(playerid, COLOR_WHITE, Msg);
					// Inform the player that he received a free truckers license from an admin
					format(Msg, sizeof(Msg), "{00FF00}You've been given a free trucker's license by {FFFF00}%s", Name);
					SendClientMessage(OtherPlayer, COLOR_WHITE, Msg);
				}
				else
				    SendClientMessage(playerid, COLOR_WHITE, "{FF0000}That player isn't logged in");
			}
		}
		else
		    return 0;
	}
	else
	    return 0;

	// Let the server know that this was a valid command
	return 1;
}
//This command let the admins to send an announce message
COMMAND:announce(playerid, params[])
{
	// Setup local variabiles
	new Message[64], Style, DisplayTime;

	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/announce", params);

	// Check if the player has logged in
	if (APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 2
		if(APlayerData[playerid][PlayerLevel] >= 2)
		{
			if(sscanf(params, "iis[64]", Style, DisplayTime, Message)) SendClientMessage(playerid, 0xFF0000AA, "Usage: \"/announce <Style (0-6)> <Time in Seconds 1-5> <Message>\"");
			else
			{
				if ((Style >= 0) && (Style <= 6) && 2 != Style)
				{
					if ((DisplayTime >= 1) && (DisplayTime <= 5))
					{
						GameTextForAll(Message, (DisplayTime * 1000), Style);
					}
					else
						SendClientMessage(playerid, COLOR_RED, "Invalid time, you can only use 1-5 seconds");
				}
				else
    				SendClientMessage(playerid, COLOR_RED, "Invalid style, you can only use styles 0-6 except 2");
			}
		}
	}
	return 1;
}

// Let admins to refuel all player vehicles
COMMAND:fuelall(playerid, params[])
{
	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/fuelall", params);

	// Check if the player has logged in
	if(APlayerData[playerid][LoggedIn] == true)
	{
		// Check if the player's admin-level is at least 3
		if(APlayerData[playerid][PlayerLevel] >= 3)
		{
			// Loop through all vehicles
			for(new vid = 0; vid < MAX_VEHICLES; vid++)
			{
				// Check if the vehicle exists
				if (AVehicleData[vid][Model] != 0) {
  					AVehicleData[vid][Fuel] = MaxFuel;
				}
			}

			// Let all players know about it
			SendClientMessageToAll(COLOR_GREEN, "All vehicles have been refueled");
		}
	}
	// Let the server know that this was a valid command
	return 1;
}
