// Define the time in days where the auto-evict system should auto-evict player's properties
#define AutoEvictTime   2

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
	format(Msg, 128, "Auto-eviction started for day: %i\r\n", AutoEvict[AEDays]);
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
	format(Msg, 128, "Auto-eviction ended for day: %i\r\n\r\n", AutoEvict[AEDays]);
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
		format(Msg, 128, "{AAAAAA}House auto-evicted: %s, owner: %s", PropertyName, PropertyOwner);
	else
		// Setup the text to send to all admins
		format(Msg, 128, "{AAAAAA}Business auto-evicted: %s, owner: %s", PropertyName, PropertyOwner);

	// Loop through all players
	for (new i; i < MAX_PLAYERS; i++)
	{
		// Check if the player is an admin
		if (APlayerData[i][PlayerLevel] > 0)
		{
			// Send the given text to the admin
			SendClientMessage(i, 0xFFFFFFFF, Msg);
		}
	}

	// Open the auto-evict-logfile for adding data
	LogFile = fopen("ServerData/AutoEvictLog.ini", io_append);

	// Determine the type of the property
	if (PropertyType == 1)
		// Setup the text to log to the logfile
		format(Msg, 128, "House auto-evicted: %s, owner: %s\r\n", PropertyName, PropertyOwner);
	else
		// Setup the text to log to the logfile
		format(Msg, 128, "Business auto-evicted: %s, owner: %s\r\n", PropertyName, PropertyOwner);

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

	format(LineForFile, 100, "Minutes %i\r\n", AutoEvict[AEMinutes]); // Construct the line: "Minutes <Minutes>"
	fwrite(AEFile, LineForFile); // And save it to the file
	format(LineForFile, 100, "Hours %i\r\n", AutoEvict[AEHours]); // Construct the line: "Hours <Hours>"
	fwrite(AEFile, LineForFile); // And save it to the file
	format(LineForFile, 100, "Days %i\r\n", AutoEvict[AEDays]); // Construct the line: "Days <Days>"
	fwrite(AEFile, LineForFile); // And save it to the file

	fclose(AEFile); // Close the file

	return 1;
}