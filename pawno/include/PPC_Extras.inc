
// Setup a global array to be used for loading player-file data
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
	if (GetPlayerState(playerid) != PLAYER_STATE_ONFOOT) return SendClientMessage(playerid, COLOR_RED, "You cannot use /porthouse when you're inside a vehicle");

	// Setup local variables
	new Msg[128], HouseID;

	// Get the HouseID from the parameters
	if (sscanf(params, "i", HouseID)) return SendClientMessage(playerid, COLOR_RED, "Usage: \"/porthouse <HouseID>\"");

	// Check if the player entered a valid HouseID
	if ((HouseID < 0) || (HouseID >= MAX_HOUSES)) return SendClientMessage(playerid, COLOR_RED, "You need to enter a valid HouseID");

	// Check if the house exists (it must have a valid pickup)
	if (IsValidDynamicPickup(AHouseData[HouseID][PickupID]))
	{
		// Port the player to the given location
		SetPlayerPos(playerid, AHouseData[HouseID][HouseX], AHouseData[HouseID][HouseY], AHouseData[HouseID][HouseZ]);

		// Let the player know he ported to the given house
		format(Msg, sizeof(Msg), "{00FF00}You ported to house with ID: {FFFF00}%i", HouseID);
		SendClientMessage(playerid, COLOR_WHITE, Msg);
	}
	else
		SendClientMessage(playerid, COLOR_RED, "That house doesn't exist");

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
	if (GetPlayerState(playerid) != PLAYER_STATE_ONFOOT) return SendClientMessage(playerid, COLOR_RED, "You cannot use /portbus when you're inside a vehicle");

	// Setup local variables
	new Msg[128], BusID;

	// Get the BusID from the parameters
	if (sscanf(params, "i", BusID)) return SendClientMessage(playerid, COLOR_RED, "Usage: \"/portbus <BusID>\"");

	// Check if the player entered a valid BusID
	if ((BusID < 0) || (BusID >= MAX_BUSINESS)) return SendClientMessage(playerid, COLOR_RED, "You need to enter a valid BusID");

	// Check if the business exists (it must have a valid pickup)
	if (IsValidDynamicPickup(ABusinessData[BusID][PickupID]))
	{
		// Port the player to the given location
		SetPlayerPos(playerid, ABusinessData[BusID][BusinessX], ABusinessData[BusID][BusinessY], ABusinessData[BusID][BusinessZ]);

		// Let the player know he ported to the given business
		format(Msg, sizeof(Msg), "{00FF00}You ported to business with ID: {FFFF00}%i", BusID);
		SendClientMessage(playerid, COLOR_WHITE, Msg);
	}
	else
		SendClientMessage(playerid, COLOR_RED, "That business doesn't exist");

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
	if (GetPlayerState(playerid) != PLAYER_STATE_ONFOOT) return SendClientMessage(playerid, COLOR_RED, "You cannot use /properties when you're inside a vehicle");

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
					format(Name, sizeof(Name), AHouseData[HouseID][Owner]);

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
						format(Name, sizeof(Name), ABusinessData[BusID][Owner]);

						// End the search
						break;
					}
				}
			}
		}
	}

	// Check if a name has been found
	if (Name[0] == 0) return SendClientMessage(playerid, COLOR_RED, "No owned house or business found in range");

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
	format(DialogTitle, sizeof(DialogTitle), "Properties of \"%s\"", Name);

	// List all properties in a dialog
	ShowPlayerDialog(playerid, DialogNoResponse, DIALOG_STYLE_LIST, DialogTitle, PropertyList, TXT_DialogButtonOk, TXT_DialogButtonCancel);

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
	if (GetPlayerState(playerid) != PLAYER_STATE_ONFOOT) return SendClientMessage(playerid, COLOR_RED, "You cannot use /propertyid when you're inside a vehicle");

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
				format(Msg, sizeof(Msg), "{00FF00}This house has ID: {FFFF00}%i", HouseID);
				SendClientMessage(playerid, COLOR_WHITE, Msg);

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
				format(Msg, sizeof(Msg), "{00FF00}This business has ID: {FFFF00}%i", BusID);
				SendClientMessage(playerid, COLOR_WHITE, Msg);

				// Exit the function
				return 1;
			}
		}
	}

	// There was no house or business in range, so let the player know about it
	SendClientMessage(playerid, COLOR_RED, "No house or business in range to ID");

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
	if (GetPlayerState(playerid) != PLAYER_STATE_ONFOOT) return SendClientMessage(playerid, COLOR_RED, "You cannot use /delproperty when you're inside a vehicle");

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
				format(Msg, sizeof(Msg), "{00FF00}You have deleted the house with ID: {FFFF00}%i", HouseID);
				SendClientMessage(playerid, COLOR_WHITE, Msg);

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
				format(Msg, sizeof(Msg), "{00FF00}You have deleted the business with ID: {FFFF00}%i", BusID);
				SendClientMessage(playerid, COLOR_WHITE, Msg);

				// Exit the function
				return 1;
			}
		}
	}

	// There was no house or business in range, so let the player know about it
	SendClientMessage(playerid, COLOR_RED, "No house or business in range to delete");

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
	if (GetPlayerState(playerid) != PLAYER_STATE_ONFOOT) return SendClientMessage(playerid, COLOR_RED, "You cannot use /evict when you're inside a vehicle");

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
					format(Msg, sizeof(Msg), "{00FF00}You have evicted the house with ID: {FFFF00}%i", HouseID);
					SendClientMessage(playerid, COLOR_WHITE, Msg);

					// Exit the function
					return 1;
				}
				else
					return SendClientMessage(playerid, COLOR_RED, "This house has no owner and cannot be evicted");
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
					format(Msg, sizeof(Msg), "{00FF00}You have evicted the business with ID: {FFFF00}%i", BusID);
					SendClientMessage(playerid, COLOR_WHITE, Msg);

					// Exit the function
					return 1;
				}
				else
					return SendClientMessage(playerid, COLOR_RED, "This business has no owner and cannot be evicted");
			}
		}
	}

	// There was no house or business in range, so let the player know about it
	SendClientMessage(playerid, COLOR_RED, "No house or business in range to evict");

	// Let the server know that this was a valid command
	return 1;
}
