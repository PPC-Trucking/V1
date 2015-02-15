
// Setup a global array to be used for loading player-file data
new FileContent[100][100];
new PropertyList[5000];


// This command lets the player teleport to a house
COMMAND:porthouse(playerid, params[])
{
	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/porthouse", params);

	// Exit the command if the player hasn't logged in using his password
	if (APlayerData[playerid][LoggedIn] == false) return 0;
	// Exit the command if the player has an admin-level lower than 5
	if (APlayerData[playerid][PlayerLevel] < 5) return 0;
	// Exit the command if the player is inside a vehicle
	if (GetPlayerVehicleSeat(playerid) != -1) return SendClientMessage(playerid, 0xFFFFFFFF, "{FF0000}You cannot use /porthouse when you're inside a vehicle");

	// Setup local variables
	new Msg[128], HouseID;

	// Get the HouseID from the parameters
	if (sscanf(params, "i", HouseID)) return SendClientMessage(playerid, 0xFF0000AA, "Usage: \"/porthouse <HouseID>\"");

	// Check if the player entered a valid HouseID
	if ((HouseID < 0) || (HouseID >= MAX_HOUSES)) return SendClientMessage(playerid, 0xFF0000AA, "You need to enter a valid HouseID");

	// Check if the house exists (it must have a valid pickup)
	if (IsValidDynamicPickup(AHouseData[HouseID][PickupID]))
	{
		// Port the player to the given location
		SetPlayerPos(playerid, AHouseData[HouseID][HouseX], AHouseData[HouseID][HouseY], AHouseData[HouseID][HouseZ]);

		// Let the player know he ported to the given house
		format(Msg, 128, "{00FF00}You ported to house with ID: {FFFF00}%i", HouseID);
		SendClientMessage(playerid, 0xFFFFFFFF, Msg);
	}
	else
		SendClientMessage(playerid, 0xFF0000AA, "That house doesn't exist");

	// Let the server know that this was a valid command
	return 1;
}

// This command lets the player teleport to a business
COMMAND:portbus(playerid, params[])
{
	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/portbus", params);

	// Exit the command if the player hasn't logged in using his password
	if (APlayerData[playerid][LoggedIn] == false) return 0;
	// Exit the command if the player has an admin-level lower than 5
	if (APlayerData[playerid][PlayerLevel] < 5) return 0;
	// Exit the command if the player is inside a vehicle
	if (GetPlayerVehicleSeat(playerid) != -1) return SendClientMessage(playerid, 0xFFFFFFFF, "{FF0000}You cannot use /portbus when you're inside a vehicle");

	// Setup local variables
	new Msg[128], BusID;

	// Get the BusID from the parameters
	if (sscanf(params, "i", BusID)) return SendClientMessage(playerid, 0xFF0000AA, "Usage: \"/portbus <BusID>\"");

	// Check if the player entered a valid BusID
	if ((BusID < 0) || (BusID >= MAX_BUSINESS)) return SendClientMessage(playerid, 0xFF0000AA, "You need to enter a valid BusID");

	// Check if the business exists (it must have a valid pickup)
	if (IsValidDynamicPickup(ABusinessData[BusID][PickupID]))
	{
		// Port the player to the given location
		SetPlayerPos(playerid, ABusinessData[BusID][BusinessX], ABusinessData[BusID][BusinessY], ABusinessData[BusID][BusinessZ]);

		// Let the player know he ported to the given business
		format(Msg, 128, "{00FF00}You ported to business with ID: {FFFF00}%i", BusID);
		SendClientMessage(playerid, 0xFFFFFFFF, Msg);
	}
	else
		SendClientMessage(playerid, 0xFF0000AA, "That business doesn't exist");

	// Let the server know that this was a valid command
	return 1;
}



// This command lets the player see all other properties of the same player
COMMAND:properties(playerid, params[])
{
	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/properties", params);

	// Exit the command if the player hasn't logged in using his password
	if (APlayerData[playerid][LoggedIn] == false) return 0;
	// Exit the command if the player has an admin-level lower than 5
	if (APlayerData[playerid][PlayerLevel] < 5) return 0;
	// Exit the command if the player is inside a vehicle
	if (GetPlayerVehicleSeat(playerid) != -1) return SendClientMessage(playerid, 0xFFFFFFFF, "{FF0000}You cannot use /properties when you're inside a vehicle");

	// Setup local variables
	new Name[24], DialogTitle[128];

	// Loop through all player-owned houses
	for (new HouseID = 1; HouseID < MAX_HOUSES; HouseID++)
	{
		// Check if the house exists (it must have a valid pickup)
		if (IsValidDynamicPickup(AHouseData[HouseID][PickupID]))
		{
			// Check if the player is in range of the house-pickup
			if (IsPlayerInRangeOfPoint(playerid, 2.5, AHouseData[HouseID][HouseX], AHouseData[HouseID][HouseY], AHouseData[HouseID][HouseZ]))
			{
			    // Check if this house is owned
			    if (AHouseData[HouseID][Owned] == true)
			    {
					// Store the ownername
					format(Name, 24, AHouseData[HouseID][Owner]);

					// End the search
					break;
				}
			}
		}
	}

	// In case no house was found in range (Name is empty), try to find a business
	if (Name[0] == 0)
	{
		// Loop through all businesses
		for (new BusID = 1; BusID < MAX_BUSINESS; BusID++)
		{
			// Check if the business exists (it must have a valid pickup)
			if (IsValidDynamicPickup(ABusinessData[BusID][PickupID]))
			{
				// Check if the player is in range of the house-pickup
				if (IsPlayerInRangeOfPoint(playerid, 2.5, ABusinessData[BusID][BusinessX], ABusinessData[BusID][BusinessY], ABusinessData[BusID][BusinessZ]))
				{
				    // Check if this business is owned
				    if (ABusinessData[BusID][Owned] == true)
					{
						// Store the ownername
						format(Name, 24, ABusinessData[BusID][Owner]);

						// End the search
						break;
					}
				}
			}
		}
	}

	// Check if a name has been found
	if (Name[0] == 0) return SendClientMessage(playerid, 0xFFFFFFFF, "{FF0000}No owned house or business found in range");

	// Clear the array
	for (new i; i < sizeof(PropertyList); i++)
	    PropertyList[i] = 0;

	// Loop through all player-owned houses
	for (new HouseID = 1; HouseID < MAX_HOUSES; HouseID++)
	{
		// Check if the house exists (it must have a valid pickup)
		if (IsValidDynamicPickup(AHouseData[HouseID][PickupID]))
		{
		    // Check if this house is owned
		    if (AHouseData[HouseID][Owned] == true)
		    {
				// Check if the owner-name is the same
				if (strcmp(AHouseData[HouseID][Owner], Name, false) == 0)
				{
					// Add the property to the list
					format(PropertyList, sizeof(PropertyList), "%sHouse %i: %s\n", PropertyList, HouseID, AHouseData[HouseID][HouseName]);
				}
			}
		}
	}
	// Loop through all businesses
	for (new BusID = 1; BusID < MAX_BUSINESS; BusID++)
	{
		// Check if the business exists (it must have a valid pickup)
		if (IsValidDynamicPickup(ABusinessData[BusID][PickupID]))
		{
		    // Check if this business is owned
		    if (ABusinessData[BusID][Owned] == true)
			{
				// Check if the owner-name is the same
				if (strcmp(ABusinessData[BusID][Owner], Name, false) == 0)
				{
					// Add the property to the list
					format(PropertyList, sizeof(PropertyList), "%sBusiness %i: %s\n", PropertyList, BusID, ABusinessData[BusID][BusinessName]);
				}
			}
		}
	}

	// Setup the dialog title
	format(DialogTitle, 128, "Properties of \"%s\"", Name);

	// List all properties in a dialog
	ShowPlayerDialog(playerid, DialogNoResponse, DIALOG_STYLE_LIST, DialogTitle, PropertyList, "OK", "Cancel");

	// Let the server know that this was a valid command
	return 1;
}



// This command lets the player ID a house or business
COMMAND:propertyid(playerid, params[])
{
	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/propertyid", params);

	// Exit the command if the player hasn't logged in using his password
	if (APlayerData[playerid][LoggedIn] == false) return 0;
	// Exit the command if the player has an admin-level lower than 5
	if (APlayerData[playerid][PlayerLevel] < 5) return 0;
	// Exit the command if the player is inside a vehicle
	if (GetPlayerVehicleSeat(playerid) != -1) return SendClientMessage(playerid, 0xFFFFFFFF, "{FF0000}You cannot use /propertyid when you're inside a vehicle");

	// Setup local variables
	new Msg[128];

	// Loop through all player-owned houses
	for (new HouseID = 1; HouseID < MAX_HOUSES; HouseID++)
	{
		// Check if the house exists (it must have a valid pickup)
		if (IsValidDynamicPickup(AHouseData[HouseID][PickupID]))
		{
			// Check if the player is in range of the house-pickup
			if (IsPlayerInRangeOfPoint(playerid, 2.5, AHouseData[HouseID][HouseX], AHouseData[HouseID][HouseY], AHouseData[HouseID][HouseZ]))
			{
				// Let the player know the HouseID of this house
				format(Msg, 128, "{00FF00}This house has ID: {FFFF00}%i", HouseID);
				SendClientMessage(playerid, 0xFFFFFFFF, Msg);

				// Exit the function
				return 1;
			}
		}
	}

	// In case no house was found in range, try to find a business

	// Loop through all businesses
	for (new BusID = 1; BusID < MAX_BUSINESS; BusID++)
	{
		// Check if the business exists (it must have a valid pickup)
		if (IsValidDynamicPickup(ABusinessData[BusID][PickupID]))
		{
			// Check if the player is in range of the house-pickup
			if (IsPlayerInRangeOfPoint(playerid, 2.5, ABusinessData[BusID][BusinessX], ABusinessData[BusID][BusinessY], ABusinessData[BusID][BusinessZ]))
			{
				// Let the player know the BusinessID of this business
				format(Msg, 128, "{00FF00}This business has ID: {FFFF00}%i", BusID);
				SendClientMessage(playerid, 0xFFFFFFFF, Msg);

				// Exit the function
				return 1;
			}
		}
	}

	// There was no house or business in range, so let the player know about it
	SendClientMessage(playerid, 0xFFFFFFFF, "{FF0000}No house or business in range to ID");

	// Let the server know that this was a valid command
	return 1;
}



// This command lets the player delete a house or business
COMMAND:delproperty(playerid, params[])
{
	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/delproperty", params);

	// Exit the command if the player hasn't logged in using his password
	if (APlayerData[playerid][LoggedIn] == false) return 0;
	// Exit the command if the player has an admin-level lower than 5
	if (APlayerData[playerid][PlayerLevel] < 5) return 0;
	// Exit the command if the player is inside a vehicle
	if (GetPlayerVehicleSeat(playerid) != -1) return SendClientMessage(playerid, 0xFFFFFFFF, "{FF0000}You cannot use /delproperty when you're inside a vehicle");

	// Setup local variables
	new Msg[128];

	// Loop through all player-owned houses
	for (new HouseID = 1; HouseID < MAX_HOUSES; HouseID++)
	{
		// Check if the house exists (it must have a valid pickup)
		if (IsValidDynamicPickup(AHouseData[HouseID][PickupID]))
		{
			// Check if the player is in range of the house-pickup
			if (IsPlayerInRangeOfPoint(playerid, 2.5, AHouseData[HouseID][HouseX], AHouseData[HouseID][HouseY], AHouseData[HouseID][HouseZ]))
			{
				// Delete the house, inform the owner if he's online and remove the house from the owner's account
				House_Delete(HouseID);

				// Also let the player know he deleted the house
				format(Msg, 128, "{00FF00}You have deleted the house with ID: {FFFF00}%i", HouseID);
				SendClientMessage(playerid, 0xFFFFFFFF, Msg);

				// Exit the function
				return 1;
			}
		}
	}

	// In case no house was found in range, try to find a business to delete

	// Loop through all businesses
	for (new BusID = 1; BusID < MAX_BUSINESS; BusID++)
	{
		// Check if the business exists (it must have a valid pickup)
		if (IsValidDynamicPickup(ABusinessData[BusID][PickupID]))
		{
			// Check if the player is in range of the house-pickup
			if (IsPlayerInRangeOfPoint(playerid, 2.5, ABusinessData[BusID][BusinessX], ABusinessData[BusID][BusinessY], ABusinessData[BusID][BusinessZ]))
			{
				// Delete the business, inform the owner if he's online and remove the business from the owner's account
				Business_Delete(BusID);

				// Also let the player know he deleted the business
				format(Msg, 128, "{00FF00}You have deleted the business with ID: {FFFF00}%i", BusID);
				SendClientMessage(playerid, 0xFFFFFFFF, Msg);

				// Exit the function
				return 1;
			}
		}
	}

	// There was no house or business in range, so let the player know about it
	SendClientMessage(playerid, 0xFFFFFFFF, "{FF0000}No house or business in range to delete");

	// Let the server know that this was a valid command
	return 1;
}

// This command lets the player evict a house or business (remove ownership of the house or business to make it available for sale again)
COMMAND:evict(playerid, params[])
{
	// Send the command to all admins so they can see it
	SendAdminText(playerid, "/evict", params);

	// Exit the command if the player hasn't logged in using his password
	if (APlayerData[playerid][LoggedIn] == false) return 0;
	// Exit the command if the player has an admin-level lower than 5
	if (APlayerData[playerid][PlayerLevel] < 5) return 0;
	// Exit the command if the player is inside a vehicle
	if (GetPlayerVehicleSeat(playerid) != -1) return SendClientMessage(playerid, 0xFFFFFFFF, "{FF0000}You cannot use /evict when you're inside a vehicle");

	// Setup local variables
	new Msg[128];

	// Loop through all player-owned houses
	for (new HouseID = 1; HouseID < MAX_HOUSES; HouseID++)
	{
		// Check if the house exists (it must have a valid pickup)
		if (IsValidDynamicPickup(AHouseData[HouseID][PickupID]))
		{
			// Check if the player is in range of the house-pickup
			if (IsPlayerInRangeOfPoint(playerid, 2.5, AHouseData[HouseID][HouseX], AHouseData[HouseID][HouseY], AHouseData[HouseID][HouseZ]))
			{
				// Check if the house is owned
				if (AHouseData[HouseID][Owned] == true)
				{
					// Evict the house, inform the owner if he's online and remove the house from the owner's account
					House_Evict(HouseID);

					// Also let the player know he evicted the house
					format(Msg, 128, "{00FF00}You have evicted the house with ID: {FFFF00}%i", HouseID);
					SendClientMessage(playerid, 0xFFFFFFFF, Msg);

					// Exit the function
					return 1;
				}
				else
					return SendClientMessage(playerid, 0xFFFFFFFF, "{FF0000}This house has no owner and cannot be evicted");
			}
		}
	}

	// In case no house was found in range, try to find a business to delete

	// Loop through all businesses
	for (new BusID = 1; BusID < MAX_BUSINESS; BusID++)
	{
		// Check if the business exists (it must have a valid pickup)
		if (IsValidDynamicPickup(ABusinessData[BusID][PickupID]))
		{
			// Check if the player is in range of the house-pickup
			if (IsPlayerInRangeOfPoint(playerid, 2.5, ABusinessData[BusID][BusinessX], ABusinessData[BusID][BusinessY], ABusinessData[BusID][BusinessZ]))
			{
				// Check if the business is owned
				if (ABusinessData[BusID][Owned] == true)
				{
					// Evict the business, inform the owner if he's online and remove the business from the owner's account
					Business_Evict(BusID);

					// Also let the player know he evicted the business
					format(Msg, 128, "{00FF00}You have evicted the business with ID: {FFFF00}%i", BusID);
					SendClientMessage(playerid, 0xFFFFFFFF, Msg);

					// Exit the function
					return 1;
				}
				else
					return SendClientMessage(playerid, 0xFFFFFFFF, "{FF0000}This business has no owner and cannot be evicted");
			}
		}
	}

	// There was no house or business in range, so let the player know about it
	SendClientMessage(playerid, 0xFFFFFFFF, "{FF0000}No house or business in range to evict");

	// Let the server know that this was a valid command
	return 1;
}



// This function will delete the given house from the server and from the owner's account (if it has an owner)
House_Delete(HouseID)
{
	// Setup local variables
	new file[100], Msg[128], Name[24], bool:PlayerOnline = false;

	// Check if the house is owned
	if (AHouseData[HouseID][Owned] == true)
	{
		// Loop through all players to find the owner (if he's online)
		for (new pid; pid < MAX_PLAYERS; pid++)
		{
			// Check if this player is online
		    if (IsPlayerConnected(pid))
		    {
		        // Get that player's name
		        GetPlayerName(pid, Name, sizeof(Name));
		        // Compare if this player has the same name as the owner of the house
				if (strcmp(AHouseData[HouseID][Owner], Name, false) == 0)
				{
				    // Set PlayerOnline to "true"
				    PlayerOnline = true;

					// Inform the player that his house is being deleted
					format(Msg, 128, "{FF0000}Your house {FFFF00}\"%s\"{FF0000} is being deleted", AHouseData[HouseID][HouseName]);
					SendClientMessage(pid, 0xFFFFFFFF, Msg);

					// Also remove the HouseID from his list of houses
					for (new HouseSlot; HouseSlot < MAX_HOUSESPERPLAYER; HouseSlot++)
					    // Check if the house is stored in this houseslot
					    if (APlayerData[pid][Houses][HouseSlot] == HouseID)
					        // If the house is stored in this slot, clear the slot
					        APlayerData[pid][Houses][HouseSlot] = 0;

					// Save the player's account again
					PlayerFile_Save(pid);

					// Stop this For-loop, as the owner was already found and informed
					break;
				}
		    }
		}

		// Check if the player was not online
		if (PlayerOnline == false)
			// Remove the house from the player's account (open the account, remove the data and re-save it)
			PlayerFile_RemoveProperty(AHouseData[HouseID][Owner], HouseID, 0);
	}

	// First remove all vehicles from the house
	House_RemoveVehicles(HouseID);

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
}

// This function will evict the given house from the server and from the owner's account (if it has an owner)
House_Evict(HouseID)
{
	// Setup local variables
	new Msg[128], Name[24], bool:PlayerOnline = false;

	// Check if the house is owned
	if (AHouseData[HouseID][Owned] == true)
	{
		// Loop through all players to find the owner (if he's online)
		for (new pid; pid < MAX_PLAYERS; pid++)
		{
			// Check if this player is online
		    if (IsPlayerConnected(pid))
		    {
		        // Get that player's name
		        GetPlayerName(pid, Name, sizeof(Name));
		        // Compare if this player has the same name as the owner of the house
				if (strcmp(AHouseData[HouseID][Owner], Name, false) == 0)
				{
				    // Set PlayerOnline to "true"
				    PlayerOnline = true;

					// Inform the player that his house is being deleted
					format(Msg, 128, "{FF0000}Your house {FFFF00}\"%s\"{FF0000} is being evicted", AHouseData[HouseID][HouseName]);
					SendClientMessage(pid, 0xFFFFFFFF, Msg);

					// Also remove the HouseID from his list of houses
					for (new HouseSlot; HouseSlot < MAX_HOUSESPERPLAYER; HouseSlot++)
					    // Check if the house is stored in this houseslot
					    if (APlayerData[pid][Houses][HouseSlot] == HouseID)
					        // If the house is stored in this slot, clear the slot
					        APlayerData[pid][Houses][HouseSlot] = 0;

					// Save the player's account again
					PlayerFile_Save(pid);

					// Stop this For-loop, as the owner was already found and informed
					break;
				}
		    }
		}

		// Check if the player was not online
		if (PlayerOnline == false)
			// Remove the house from the player's account (open the account, remove the data and re-save it)
			PlayerFile_RemoveProperty(AHouseData[HouseID][Owner], HouseID, 0);
	}

	// First remove all vehicles from the house
	House_RemoveVehicles(HouseID);

	// Clear ownership of the house
	AHouseData[HouseID][Owned] = false;
	AHouseData[HouseID][Owner] = 0;

	// Update the entrance of the house
	House_UpdateEntrance(HouseID);

	// Save the housefile
	HouseFile_Save(HouseID);
}

// This function will delete the given business from the server and from the owner's account (if it has an owner)
Business_Delete(BusID)
{
	// Setup local variables
	new file[100], Msg[128], Name[24], bool:PlayerOnline = false;

	// Check if the business is owned
	if (ABusinessData[BusID][Owned] == true)
	{
		// Loop through all players to find the owner (if he's online)
		for (new pid; pid < MAX_PLAYERS; pid++)
		{
			// Check if this player is online
		    if (IsPlayerConnected(pid))
		    {
		        // Get that player's name
		        GetPlayerName(pid, Name, sizeof(Name));
		        // Compare if this player has the same name as the owner of the business
				if (strcmp(ABusinessData[BusID][Owner], Name, false) == 0)
				{
				    // Set PlayerOnline to "true"
				    PlayerOnline = true;

					// Inform the player that his business is being deleted
					format(Msg, 128, "{FF0000}Your business {FFFF00}\"%s\"{FF0000} is being deleted", ABusinessData[BusID][BusinessName]);
					SendClientMessage(pid, 0xFFFFFFFF, Msg);

					// Also remove the BusID from his list of businesses
					for (new BusSlot; BusSlot < MAX_BUSINESSPERPLAYER; BusSlot++)
					    // Check if the business is stored in this business-slot
					    if (APlayerData[pid][Business][BusSlot] == BusID)
					        // If the business is stored in this slot, clear the slot
					        APlayerData[pid][Business][BusSlot] = 0;

					// Save the player's account again
					PlayerFile_Save(pid);

					// Stop this For-loop, as the owner was already found and informed
					break;
				}
		    }
		}

		// Check if the player was not online
		if (PlayerOnline == false)
			// Remove the house from the player's account (open the account, remove the data and re-save it)
			PlayerFile_RemoveProperty(ABusinessData[BusID][Owner], 0, BusID);
	}

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
	// Destroy the mapicon, 3DText and pickup for the business
	DestroyDynamicPickup(ABusinessData[BusID][PickupID]);
	DestroyDynamicMapIcon(ABusinessData[BusID][MapIconID]);
	DestroyDynamic3DTextLabel(ABusinessData[BusID][DoorText]);
	ABusinessData[BusID][PickupID] = 0;
	ABusinessData[BusID][MapIconID] = 0;

	// Delete the business-file
	format(file, sizeof(file), BusinessFile, BusID); // Construct the complete filename for this business-file
	if (fexist(file)) // Make sure the file exists
		fremove(file); // Delete the file
}

// This function will evict the given business from the server and from the owner's account (if it has an owner)
Business_Evict(BusID)
{
	// Setup local variables
	new Msg[128], Name[24], bool:PlayerOnline = false;

	// Check if the business is owned
	if (ABusinessData[BusID][Owned] == true)
	{
		// Loop through all players to find the owner (if he's online)
		for (new pid; pid < MAX_PLAYERS; pid++)
		{
			// Check if this player is online
		    if (IsPlayerConnected(pid))
		    {
		        // Get that player's name
		        GetPlayerName(pid, Name, sizeof(Name));
		        // Compare if this player has the same name as the owner of the business
				if (strcmp(ABusinessData[BusID][Owner], Name, false) == 0)
				{
				    // Set PlayerOnline to "true"
				    PlayerOnline = true;

					// Inform the player that his business is being deleted
					format(Msg, 128, "{FF0000}Your business {FFFF00}\"%s\"{FF0000} is being evicted", ABusinessData[BusID][BusinessName]);
					SendClientMessage(pid, 0xFFFFFFFF, Msg);

					// Also remove the BusID from his list of businesses
					for (new BusSlot; BusSlot < MAX_BUSINESSPERPLAYER; BusSlot++)
					    // Check if the business is stored in this business-slot
					    if (APlayerData[pid][Business][BusSlot] == BusID)
					        // If the business is stored in this slot, clear the slot
					        APlayerData[pid][Business][BusSlot] = 0;

					// Save the player's account again
					PlayerFile_Save(pid);

					// Stop this For-loop, as the owner was already found and informed
					break;
				}
		    }
		}

		// Check if the player was not online
		if (PlayerOnline == false)
			// Remove the house from the player's account (open the account, remove the data and re-save it)
			PlayerFile_RemoveProperty(ABusinessData[BusID][Owner], 0, BusID);
	}

	// Clear ownership of the business
	ABusinessData[BusID][Owned] = false;
	ABusinessData[BusID][Owner] = 0;

	// Update the entrance of the business
	Business_UpdateEntrance(BusID);

	// Save the businessfile
	BusinessFile_Save(BusID);
}



// This function loads the playerfile for the given player, removes the lines that contain the HouseID or BusinessID and resaves the file
PlayerFile_RemoveProperty(Name[], HouseID, BusID)
{
	// Setup local variables
	new file[100], File:PFile, LineFromFile[100], Index, SearchString[100];

	// Construct the complete filename for this player's account
	format(file, sizeof(file), PlayerFile, Name);

	// Check if the player's datafile exists
	if (fexist(file))
	{
		// First clear the array before using it to make sure it's empty
		for (Index = 0; Index < 100; Index++)
            FileContent[Index][0] = 0;

		// Open the playerfile for reading
		PFile = fopen(file, io_read);

		// Read up to 100 lines from the file (no player-account should have more than this)
		for (Index = 0; Index < 100; Index++)
		{
			// Read a line from the file
			fread(PFile, LineFromFile);
			// Strip any newline characters from the LineFromFile
			StripNewLine(LineFromFile);

			// Check if this line holds data
			if (strlen(LineFromFile) > 0)
				// Store this line into the array
				format(FileContent[Index], 100, LineFromFile);
			else // If the read line doesn't contain any more data (empty string), the end of the file has been reached
			    break;
		}

        // Close the file
		fclose(PFile);



		// Now the file has been read and stored into the array, now scan the array to search for the matching HouseID and/or BusinessID

		// Loop through the entire array
		for (Index = 0; Index < 100; Index++)
		{
			// Build the string to search for the HouseID
			format(SearchString, 100, "House %i", HouseID);
			// Check if this line holds the string we're looking for
			if (strcmp(FileContent[Index], SearchString, false) == 0)
			    // Clear the array-index
			    FileContent[Index][0] = 0;

			// Build the string to search for the BusinessID
			format(SearchString, 100, "Business %i", BusID);
			// Check if this line holds the string we're looking for
			if (strcmp(FileContent[Index], SearchString, false) == 0)
			    // Clear the array-index
			    FileContent[Index][0] = 0;
		}


		// Now all matching HouseID's and BusinessID's should be removed from the account, now re-save the playerfile

        // Open the playerfile for writing
		PFile = fopen(file, io_write);

		// Loop through the entire array
		for (Index = 0; Index < 100; Index++)
		{
			// Check if there is data in this array-slot
			if (FileContent[Index][0] != 0)
			{
			    // Construct the line for saving
				format(LineFromFile, 100, "%s\r\n", FileContent[Index]);
		        // And save it to the file
				fwrite(PFile, LineFromFile);
			}
		}

        // Close the file
		fclose(PFile);
	}
}