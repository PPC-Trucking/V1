// Define the time in days where the auto-evict system should auto-evict player's properties
#define AutoEvictTime   2

new FileContent[100][100];

// Setup a custom type that holds all data about the auto-evict system
enum TAutoEvict
{
	AEMinutes, // Holds the amount of minutes the server is online
	AEHours, // Holds the amount of hours the server is online
	AEDays // Holds the amount of days the server is online
}
// Setup a variable to hold all data about the auto-evict system
new AutoEvict[TAutoEvict];

// This global auto-evict timer runs every minute and increases the server-online time
forward AutoEvictTimer();
public AutoEvictTimer()
{
	// Increase the amount of minutes the server is online
	AutoEvict[AEMinutes]++;

	// Increase the amount of hours if the minutes reaches 60
	if (AutoEvict[AEMinutes] == 60)
	{
		AutoEvict[AEHours]++;
		AutoEvict[AEMinutes] = 0;
	}

	// Increase the days the server is online if the hours reaches 24
	if (AutoEvict[AEHours] == 24)
	{
	    AutoEvict[AEDays]++;
	    AutoEvict[AEHours] = 0;

		// Auto-evict all properties if the owner didn't login within the AutoEvictTime
		AutoEvictProperties();
	}

	// Save the auto-evict-time
	AutoEvict_Save();

	return 1;
}

// This function scans all houses and businesses and auto-evicts them if needed
AutoEvictProperties()
{
	// Setup local variables
	new File:LogFile, Msg[128];

	// Open the auto-evict-logfile for adding data
	LogFile = fopen("ServerData/AutoEvictLog.ini", io_append);
	// Log the eviction to the logfile
	format(Msg, sizeof(Msg), "Auto-eviction started for day: %i\r\n", AutoEvict[AEDays]);
	fwrite(LogFile, Msg); // And save it to the file
    // Close the logfile
	fclose(LogFile);

	// Loop through all player-owned houses
	for (new HouseID = 1; HouseID < MAX_HOUSES; HouseID++)
	{
		// Check if the house exists (it must have a valid pickup)
		if (IsValidDynamicPickup(AHouseData[HouseID][PickupID]))
		{
		    // Check if this house is owned
		    if (AHouseData[HouseID][Owned] == true)
		    {
				// Check if the AutoEvict-days are equal or higher than the value stored in this house
				if (AHouseData[HouseID][AutoEvictDays] < (AutoEvict[AEDays] - AutoEvictTime))
				{
					// Auto-Evict the house, inform the owner if he's online and remove the house from the owner's account
					House_Evict(HouseID);
					// Also log the auto-eviction to the logfile and inform all admins
					LogAutoEvict(1, AHouseData[HouseID][HouseName], AHouseData[HouseID][Owner]);
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
				// Check if the AutoEvict-days are equal or higher than the value stored in this business
				if (ABusinessData[BusID][AutoEvictDays] < (AutoEvict[AEDays] - AutoEvictTime))
				{
					// Auto-Evict the business, inform the owner if he's online and remove the business from the owner's account
					Business_Evict(BusID);
					// Also log the auto-eviction to the logfile and inform all admins
					LogAutoEvict(2, ABusinessData[BusID][BusinessName], ABusinessData[BusID][Owner]);
				}
			}
		}
	}

	// Open the auto-evict-logfile for adding data
	LogFile = fopen("ServerData/AutoEvictLog.ini", io_append);
	// Log the eviction to the logfile
	format(Msg, sizeof(Msg), "Auto-eviction ended for day: %i\r\n\r\n", AutoEvict[AEDays]);
	fwrite(LogFile, Msg); // And save it to the file
    // Close the logfile
	fclose(LogFile);
}

// This function informs all admins about the property that has been auto-evicted and also logs the auto-evict into a file
LogAutoEvict(PropertyType, PropertyName[], PropertyOwner[])
{
	// Setup local variables
	new Msg[128], File:LogFile;

	// Determine the type of the property
	if (PropertyType == 1)
		// Setup the text to send to all admins
		format(Msg, sizeof(Msg), "{AAAAAA}House auto-evicted: %s, owner: %s", PropertyName, PropertyOwner);
	else
		// Setup the text to send to all admins
		format(Msg, sizeof(Msg), "{AAAAAA}Business auto-evicted: %s, owner: %s", PropertyName, PropertyOwner);

	// Loop through all players
	for (new i; i < MAX_PLAYERS; i++)
	{
		// Check if the player is an admin
		if (APlayerData[i][PlayerLevel] > 0)
		{
			// Send the given text to the admin
			SendClientMessage(i, COLOR_WHITE, Msg);
		}
	}

	// Open the auto-evict-logfile for adding data
	LogFile = fopen("ServerData/AutoEvictLog.ini", io_append);

	// Determine the type of the property
	if (PropertyType == 1)
		// Setup the text to log to the logfile
		format(Msg, sizeof(Msg), "House auto-evicted: %s, owner: %s\r\n", PropertyName, PropertyOwner);
	else
		// Setup the text to log to the logfile
		format(Msg, sizeof(Msg), "Business auto-evicted: %s, owner: %s\r\n", PropertyName, PropertyOwner);

	// Log the eviction to the logfile
	fwrite(LogFile, Msg); // And save it to the file

    // Close the logfile
	fclose(LogFile);
}

// This function loads the file that holds the current auto-evict-time
AutoEvict_Load()
{
	// Setup local variables
	new File:AEFile, LineFromFile[100], ParameterName[50], ParameterValue[50];

	// Try to load the auto-evict file
	if (fexist("ServerData/AutoEvict.ini"))
	{
		// Open the auto-evict-file for reading
		AEFile = fopen("ServerData/AutoEvict.ini", io_read);

        // Read the first line of the file
		fread(AEFile, LineFromFile);

		// Keep reading until the end of the file is found (no more data)
		while (strlen(LineFromFile) > 0)
		{
			StripNewLine(LineFromFile); // Strip any newline characters from the LineFromFile
			sscanf(LineFromFile, "s[50]s[50]", ParameterName, ParameterValue); // Extract parametername and parametervalue

			// Store the proper value in the proper place
			if (strcmp(ParameterName, "Minutes", false) == 0) // If the parametername is correct ("Minutes")
				AutoEvict[AEMinutes] = strval(ParameterValue); // Store the Minutes
			if (strcmp(ParameterName, "Hours", false) == 0) // If the parametername is correct ("Hours")
				AutoEvict[AEHours] = strval(ParameterValue); // Store the Hours
			if (strcmp(ParameterName, "Days", false) == 0) // If the parametername is correct ("Days")
				AutoEvict[AEDays] = strval(ParameterValue); // Store the Days

            // Read the next line of the file
			fread(AEFile, LineFromFile);
		}

        // Close the file
		fclose(AEFile);

        // Return if the file was read correctly
		return 1;
	}
	else
	    return 0; // Return 0 if the file couldn't be read (doesn't exist)
}

// This function saves the file that holds the current auto-evict-time
AutoEvict_Save()
{
	new File:AEFile, LineForFile[100];

	AEFile = fopen("ServerData/AutoEvict.ini", io_write); // Open the auto-evict-file for writing

	format(LineForFile, sizeof(LineForFile), "Minutes %i\r\n", AutoEvict[AEMinutes]); // Construct the line: "Minutes <Minutes>"
	fwrite(AEFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "Hours %i\r\n", AutoEvict[AEHours]); // Construct the line: "Hours <Hours>"
	fwrite(AEFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "Days %i\r\n", AutoEvict[AEDays]); // Construct the line: "Days <Days>"
	fwrite(AEFile, LineForFile); // And save it to the file

	fclose(AEFile); // Close the file

	return 1;
}

// This function will delete the given house from the server and from the owner's account (if it has an owner)
stock House_Delete(HouseID)
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
					format(Msg, sizeof(Msg), "{FF0000}Your house {FFFF00}\"%s\"{FF0000} is being deleted", AHouseData[HouseID][HouseName]);
					SendClientMessage(pid, COLOR_WHITE, Msg);

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
					format(Msg, sizeof(Msg), "{FF0000}Your house {FFFF00}\"%s\"{FF0000} is being evicted", AHouseData[HouseID][HouseName]);
					SendClientMessage(pid, COLOR_WHITE, Msg);

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
stock Business_Delete(BusID)
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
					format(Msg, sizeof(Msg), "{FF0000}Your business {FFFF00}\"%s\"{FF0000} is being deleted", ABusinessData[BusID][BusinessName]);
					SendClientMessage(pid, COLOR_WHITE, Msg);

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
					format(Msg, sizeof(Msg), "{FF0000}Your business {FFFF00}\"%s\"{FF0000} is being evicted", ABusinessData[BusID][BusinessName]);
					SendClientMessage(pid, COLOR_WHITE, Msg);

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
				format(FileContent[Index], sizeof(LineFromFile), LineFromFile);
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
			format(SearchString, sizeof(SearchString), "House %i", HouseID);
			// Check if this line holds the string we're looking for
			if (strcmp(FileContent[Index], SearchString, false) == 0)
			    // Clear the array-index
			    FileContent[Index][0] = 0;

			// Build the string to search for the BusinessID
			format(SearchString, sizeof(SearchString), "Business %i", BusID);
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
				format(LineFromFile, sizeof(LineFromFile), "%s\r\n", FileContent[Index]);
		        // And save it to the file
				fwrite(PFile, LineFromFile);
			}
		}

        // Close the file
		fclose(PFile);
	}
}