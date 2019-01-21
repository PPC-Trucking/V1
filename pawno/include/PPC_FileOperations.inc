// This function will create the player-datafile for the given player (only used when a new player joins the server)
PlayerFile_Create(playerid)
{
	new file[100], File:PFile, Name[24], LineForFile[100];
	format(Name, sizeof(Name), APlayerData[playerid][PlayerName]); // Get the playername
	format(file, sizeof(file), PlayerFile, Name); // Construct the complete filename for this player's account


	PFile = fopen(file, io_write); // Open the playerfile for writing

	format(LineForFile, sizeof(LineForFile), "Password %s\r\n", APlayerData[playerid][PlayerPassword]); // Construct the line: "Password <playerpassword>"
	fwrite(PFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "Level %i\r\n", 0); // Construct the line: "Level <playerlevel>"
	fwrite(PFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "Jailed %i\r\n", 0); // Construct the line: "Jailed <playerjailed>"
	fwrite(PFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "Wanted %i\r\n", 0); // Construct the line: "Wanted <wantedlevel>"
	fwrite(PFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "Bans %i\r\n", 0); // Construct the line: "Bans <NumberOfBans>"
	fwrite(PFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "BanTime %i\r\n", 0); // Construct the line: "BanTime <TimeToUnban>"
	fwrite(PFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "TruckerLicense %i\r\n", 0); // Construct the line: "TruckerLicense <playertruckerlicense>"
	fwrite(PFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "BusLicense %i\r\n", 0); // Construct the line: "BusLicense <playerbuslicense>"
	fwrite(PFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "StatsMetersDriven %f\r\n", 0.0); // Construct the line: "StatsMetersDriven <StatsMetersDriven>"
	fwrite(PFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "StatsTruckerJobs %i\r\n", 0); // Construct the line: "StatsTruckerJobs <StatsTruckerJobs>"
	fwrite(PFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "StatsConvoyJobs %i\r\n", 0); // Construct the line: "StatsConvoyJobs <StatsConvoyJobs>"
	fwrite(PFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "StatsBusDriverJobs %i\r\n", 0); // Construct the line: "StatsBusDriverJobs <StatsBusDriverJobs>"
	fwrite(PFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "StatsPilotJobs %i\r\n", 0); // Construct the line: "StatsPilotJobs <StatsPilotJobs>"
	fwrite(PFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "StatsMafiaJobs %i\r\n", 0); // Construct the line: "StatsMafiaJobs <StatsMafiaJobs>"
	fwrite(PFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "StatsMafiaStolen %i\r\n", 0); // Construct the line: "StatsMafiaStolen <StatsMafiaStolen>"
	fwrite(PFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "StatsPoliceFined %i\r\n", 0); // Construct the line: "StatsPoliceFined <StatsPoliceFined>"
	fwrite(PFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "StatsPoliceJailed %i\r\n", 0); // Construct the line: "StatsPoliceJailed <StatsPoliceJailed>"
	fwrite(PFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "Money %i\r\n", 0); // Construct the line: "Money <playermoney>"
	fwrite(PFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "Score %i\r\n", 0); // Construct the line: "Score <playerscore>"
	fwrite(PFile, LineForFile); // And save it to the file

	fclose(PFile); // Close the file

	return 1;
}

// This function will load the player's datafile (used when a player connects to the server)
PlayerFile_Load(playerid)
{
	// Setup local variables
	new file[100], File:PFile, Name[24], LineFromFile[100], ParameterName[50], ParameterValue[50], HouseIndex, BusIndex;

	format(Name, sizeof(Name), APlayerData[playerid][PlayerName]); // Get the playername
	format(file, sizeof(file), PlayerFile, Name); // Construct the complete filename for this player's account

	// Check if the player's datafile exists
	if (fexist(file))
	{
		PFile = fopen(file, io_read); // Open the playerfile for reading

		fread(PFile, LineFromFile); // Read the first line of the file

		// Keep reading until the end of the file is found (no more data)
		while (strlen(LineFromFile) > 0)
		{
			StripNewLine(LineFromFile); // Strip any newline characters from the LineFromFile
			sscanf(LineFromFile, "s[50]s[50]", ParameterName, ParameterValue); // Extract parametername and parametervalue

			// Store the proper value in the proper place
			if (strcmp(ParameterName, "Password", false) == 0) // If the parametername is correct ("Password")
			    format(APlayerData[playerid][PlayerPassword], 50, ParameterValue); // Store the password
			if (strcmp(ParameterName, "Level", false) == 0) // If the parametername is correct ("Level")
			    APlayerData[playerid][PlayerLevel] = strval(ParameterValue); // Store the playerlevel
			if (strcmp(ParameterName, "Jailed", false) == 0) // If the parametername is correct ("Jailed")
			    APlayerData[playerid][PlayerJailed] = strval(ParameterValue); // Store the jailed-status
			if (strcmp(ParameterName, "Wanted", false) == 0) // If the parametername is correct ("Wanted")
			    SetPlayerWantedLevel(playerid, strval(ParameterValue)); // Set the wanted-status
			if (strcmp(ParameterName, "Bans", false) == 0) // If the parametername is correct ("Bans")
			    APlayerData[playerid][Bans] = strval(ParameterValue); // Store the bans
			if (strcmp(ParameterName, "BanTime", false) == 0) // If the parametername is correct ("BanTime")
			    APlayerData[playerid][BanTime] = strval(ParameterValue); // Store the bantime
			if (strcmp(ParameterName, "TruckerLicense", false) == 0) // If the parametername is correct ("TruckerLicense")
			    APlayerData[playerid][TruckerLicense] = strval(ParameterValue); // Store the TruckerLicense
			if (strcmp(ParameterName, "BusLicense", false) == 0) // If the parametername is correct ("BusLicense")
			    APlayerData[playerid][BusLicense] = strval(ParameterValue); // Store the BusLicense
			if (strcmp(ParameterName, "Muted", false) == 0) // If the parametername is correct ("Muted")
			    APlayerData[playerid][Muted] = strval(ParameterValue); // Store the muted time

			if (strcmp(ParameterName, "RulesRead", false) == 0) // If the parametername is correct ("RulesRead")
			{
			    if (strcmp(ParameterValue, "Yes", false) == 0) // If the value "Yes" was read
					APlayerData[playerid][RulesRead] = true; // Player has accepted the rules
				else
					APlayerData[playerid][RulesRead] = false; // Player hasn't accepted the rules yet
			}

			if (strcmp(ParameterName, "StatsMetersDriven", false) == 0) // If the parametername is correct ("StatsMetersDriven")
			    APlayerData[playerid][StatsMetersDriven] = floatstr(ParameterValue); // Store the StatsMetersDriven
			if (strcmp(ParameterName, "StatsTruckerJobs", false) == 0) // If the parametername is correct ("StatsTruckerJobs")
			    APlayerData[playerid][StatsTruckerJobs] = strval(ParameterValue); // Store the StatsTruckerJobs
			if (strcmp(ParameterName, "StatsConvoyJobs", false) == 0) // If the parametername is correct ("StatsConvoyJobs")
			    APlayerData[playerid][StatsConvoyJobs] = strval(ParameterValue); // Store the StatsConvoyJobs
			if (strcmp(ParameterName, "StatsBusDriverJobs", false) == 0) // If the parametername is correct ("StatsBusDriverJobs")
			    APlayerData[playerid][StatsBusDriverJobs] = strval(ParameterValue); // Store the StatsBusDriverJobs
			if (strcmp(ParameterName, "StatsPilotJobs", false) == 0) // If the parametername is correct ("StatsPilotJobs")
			    APlayerData[playerid][StatsPilotJobs] = strval(ParameterValue); // Store the StatsPilotJobs
			if (strcmp(ParameterName, "StatsMafiaJobs", false) == 0) // If the parametername is correct ("StatsMafiaJobs")
			    APlayerData[playerid][StatsMafiaJobs] = strval(ParameterValue); // Store the StatsMafiaJobs
			if (strcmp(ParameterName, "StatsMafiaStolen", false) == 0) // If the parametername is correct ("StatsMafiaStolen")
			    APlayerData[playerid][StatsMafiaStolen] = strval(ParameterValue); // Store the StatsMafiaStolen
			if (strcmp(ParameterName, "StatsPoliceFined", false) == 0) // If the parametername is correct ("StatsPoliceFined")
			    APlayerData[playerid][StatsPoliceFined] = strval(ParameterValue); // Store the StatsPoliceFined
			if (strcmp(ParameterName, "StatsPoliceJailed", false) == 0) // If the parametername is correct ("StatsPoliceJailed")
			    APlayerData[playerid][StatsPoliceJailed] = strval(ParameterValue); // Store the StatsPoliceJailed
			if (strcmp(ParameterName, "StatsAssistance", false) == 0) // If the parametername is correct ("StatsAssistance")
			    APlayerData[playerid][StatsAssistance] = strval(ParameterValue); // Store the StatsAssistance
			if (strcmp(ParameterName, "StatsCourierJobs", false) == 0) // If the parametername is correct ("StatsCourierJobs")
			    APlayerData[playerid][StatsCourierJobs] = strval(ParameterValue); // Store the StatsCourierJobs
			if (strcmp(ParameterName, "StatsRoadworkerJobs", false) == 0) // If the parametername is correct ("StatsRoadworkerJobs")
			    APlayerData[playerid][StatsRoadworkerJobs] = strval(ParameterValue); // Store the StatsRoadworkerJobs


			if (strcmp(ParameterName, "House", false) == 0) // If the parametername is correct ("House")
			{
			    APlayerData[playerid][Houses][HouseIndex] = strval(ParameterValue); // Store the HouseID at the selected slot
			    HouseIndex++; // Select the next house-slot in case another house-id is found
			}
			if (strcmp(ParameterName, "Business", false) == 0) // If the parametername is correct ("Business")
			{
				if (strcmp(ABusinessData[strval(ParameterValue)][Owner], Name, false) == 0) // Check if the player is the true owner of the business
				{
				    APlayerData[playerid][Business][BusIndex] = strval(ParameterValue); // Store the BusinessID at the selected slot
				    BusIndex++; // Select the next business-slot in case another business-id is found
				}
			}

			if (strcmp(ParameterName, "Money", false) == 0) // If the parametername is correct ("Money")
				RewardPlayer(playerid, strval(ParameterValue), 0); // Store the money
			if (strcmp(ParameterName, "Score", false) == 0) // If the parametername is correct ("Score")
				RewardPlayer(playerid, 0, strval(ParameterValue)); // Store the score

			fread(PFile, LineFromFile); // Read the next line of the file
		}

		fclose(PFile); // Close the file

		return 1; // Return if the file was read correctly
	}
	else
	    return 0; // Return 0 if the file couldn't be read (doesn't exist)
}

// This function will save all player-data (used when the player disconnects from the server)
PlayerFile_Save(playerid)
{
	new file[100], File:PFile, Name[24], LineForFile[100];
	format(Name, sizeof(Name), APlayerData[playerid][PlayerName]); // Get the playername
	format(file, sizeof(file), PlayerFile, Name); // Construct the complete filename for this player's account

	PFile = fopen(file, io_write); // Open the playerfile for writing

	format(LineForFile, sizeof(LineForFile), "Password %s\r\n", APlayerData[playerid][PlayerPassword]); // Construct the line: "Password <playerpassword>"
	fwrite(PFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "Level %i\r\n", APlayerData[playerid][PlayerLevel]); // Construct the line: "Level <playerlevel>"
	fwrite(PFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "Jailed %i\r\n", APlayerData[playerid][PlayerJailed]); // Construct the line: "Jailed <playerjailed>"
	fwrite(PFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "Wanted %i\r\n", GetPlayerWantedLevel(playerid)); // Construct the line: "Wanted <wantedlevel>"
	fwrite(PFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "Bans %i\r\n", APlayerData[playerid][Bans]); // Construct the line: "Bans <NumberOfBans>"
	fwrite(PFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "BanTime %i\r\n", APlayerData[playerid][BanTime]); // Construct the line: "BanTime <TimeToUnban>"
	fwrite(PFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "TruckerLicense %i\r\n", APlayerData[playerid][TruckerLicense]); // Construct the line: "TruckerLicense <playertruckerlicense>"
	fwrite(PFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "BusLicense %i\r\n", APlayerData[playerid][BusLicense]); // Construct the line: "BusLicense <playerbuslicense>"
	fwrite(PFile, LineForFile); // And save it to the file

	format(LineForFile, sizeof(LineForFile), "Muted %i\r\n", APlayerData[playerid][Muted]); // Construct the line: "Muted <Time>"
	fwrite(PFile, LineForFile); // And save it to the file

	if (APlayerData[playerid][RulesRead] == true) // Check if the player has accepted the rules
		format(LineForFile, sizeof(LineForFile), "RulesRead Yes\r\n"); // Construct the line: "RulesRead <Yes>"
	else
		format(LineForFile, sizeof(LineForFile), "RulesRead No\r\n"); // Construct the line: "RulesRead <No>"
	fwrite(PFile, LineForFile); // And save it to the file

	format(LineForFile, sizeof(LineForFile), "StatsMetersDriven %f\r\n", APlayerData[playerid][StatsMetersDriven]); // Construct the line: "StatsMetersDriven <StatsMetersDriven>"
	fwrite(PFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "StatsTruckerJobs %i\r\n", APlayerData[playerid][StatsTruckerJobs]); // Construct the line: "StatsTruckerJobs <StatsTruckerJobs>"
	fwrite(PFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "StatsConvoyJobs %i\r\n", APlayerData[playerid][StatsConvoyJobs]); // Construct the line: "StatsConvoyJobs <StatsConvoyJobs>"
	fwrite(PFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "StatsBusDriverJobs %i\r\n", APlayerData[playerid][StatsBusDriverJobs]); // Construct the line: "StatsBusDriverJobs <StatsBusDriverJobs>"
	fwrite(PFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "StatsPilotJobs %i\r\n", APlayerData[playerid][StatsPilotJobs]); // Construct the line: "StatsPilotJobs <StatsPilotJobs>"
	fwrite(PFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "StatsMafiaJobs %i\r\n", APlayerData[playerid][StatsMafiaJobs]); // Construct the line: "StatsMafiaJobs <StatsMafiaJobs>"
	fwrite(PFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "StatsMafiaStolen %i\r\n", APlayerData[playerid][StatsMafiaStolen]); // Construct the line: "StatsMafiaStolen <StatsMafiaStolen>"
	fwrite(PFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "StatsPoliceFined %i\r\n", APlayerData[playerid][StatsPoliceFined]); // Construct the line: "StatsPoliceFined <StatsPoliceFined>"
	fwrite(PFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "StatsPoliceJailed %i\r\n", APlayerData[playerid][StatsPoliceJailed]); // Construct the line: "StatsPoliceJailed <StatsPoliceJailed>"
	fwrite(PFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "StatsAssistance %i\r\n", APlayerData[playerid][StatsAssistance]); // Construct the line: "StatsAssistance <StatsAssistance>"
	fwrite(PFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "StatsCourierJobs %i\r\n", APlayerData[playerid][StatsCourierJobs]); // Construct the line: "StatsCourierJobs <StatsCourierJobs>"
	fwrite(PFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "StatsRoadworkerJobs %i\r\n", APlayerData[playerid][StatsRoadworkerJobs]); // Construct the line: "StatsRoadworkerJobs <StatsRoadworkerJobs>"
	fwrite(PFile, LineForFile); // And save it to the file

	// Save all the valid house-id's
	for (new i; i < MAX_HOUSESPERPLAYER; i++)
	{
		// Check if there is a valid house-id in this slot
		if (APlayerData[playerid][Houses][i] != 0)
		{
			format(LineForFile, sizeof(LineForFile), "House %i\r\n", APlayerData[playerid][Houses][i]); // Construct the line: "House <HouseID>"
			fwrite(PFile, LineForFile); // And save it to the file
		}
	}
	// Save all the valid business-id's
	for (new i; i < MAX_BUSINESSPERPLAYER; i++)
	{
		// Check if there is a valid business-id in this slot
		if (APlayerData[playerid][Business][i] != 0)
		{
			format(LineForFile, sizeof(LineForFile), "Business %i\r\n", APlayerData[playerid][Business][i]); // Construct the line: "Business <BusinessID>"
			fwrite(PFile, LineForFile); // And save it to the file
		}
	}

	format(LineForFile, sizeof(LineForFile), "Money %i\r\n", APlayerData[playerid][PlayerMoney]); // Construct the line: "Money <playermoney>"
	fwrite(PFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "Score %i\r\n", APlayerData[playerid][PlayerScore]); // Construct the line: "Score <playerscore>"
	fwrite(PFile, LineForFile); // And save it to the file

	fclose(PFile); // Close the file

	// Also save the houses that this player owns
	for (new i; i < MAX_HOUSESPERPLAYER; i++)
	{
		// Check if there is a valid house-id in this slot
		if (APlayerData[playerid][Houses][i] != 0)
		{
		    // Save the house (and linked vehicles)
		    HouseFile_Save(APlayerData[playerid][Houses][i]);
		}
	}
	// Save all the valid business-id's
	for (new i; i < MAX_BUSINESSPERPLAYER; i++)
	{
		// Check if there is a valid business-id in this slot
		if (APlayerData[playerid][Business][i] != 0)
		{
		    // Save the business
		    BusinessFile_Save(APlayerData[playerid][Business][i]);
		}
	}

	return 1;
}








// This function loads all houses
Housing_LoadAll()
{
	// Loop through all houses and try to load them
	for (new HouseID = 1; HouseID < MAX_HOUSES; HouseID++)
	{
	    // Try to load the file and check if it was succesfully loaded (file exists)
		if (HouseFile_Load(HouseID) == 1)
		{
			// Add a pickup and 3DText for this house
			House_CreateEntrance(HouseID);
			// Count the amount of houses that are loaded
		    TotalHouses++;
		}
	}

	return 1;
}

// This function will load the house's datafile (used when the server is started to load all houses)
HouseFile_Load(HouseID, bool:OnlyLoadVehicles = false)
{
	new file[100], File:HFile, LineFromFile[100], ParameterName[50], ParameterValue[50];
	// Setup local variables
	new cModel, cPaint, components[14], Float:cx, Float:cy, Float:cz, Float:crot, Col1, Col2, vid, bool:VehicleClamped, cFuel = -1, Neons;

	format(file, sizeof(file), HouseFile, HouseID); // Construct the complete filename for this house-file

	if (fexist(file))
	{
		HFile = fopen(file, io_read); // Open the housefile for reading

		fread(HFile, LineFromFile); // Read the first line of the file

		// Set the house so it cannot be entered by anyone, except the owner (close the house)
		AHouseData[HouseID][HouseOpened] = false;

		// Keep reading until the end of the file is found (no more data)
		while (strlen(LineFromFile) > 0)
		{
			StripNewLine(LineFromFile); // Strip any newline characters from the LineFromFile
			sscanf(LineFromFile, "s[50]s[50]", ParameterName, ParameterValue); // Extract parametername and parametervalue

			// If OnlyLoadVehicles is "false", only load the house-data
			// If OnlyLoadVehicles is "true", only load the vehicle-data
			if (OnlyLoadVehicles == false)
			{
				// Store the proper value in the proper place
				if (strcmp(ParameterName, "HouseName", false) == 0) // If the parametername is correct ("HouseName")
				    format(AHouseData[HouseID][HouseName], 24, ParameterValue); // Store the HouseName
				if (strcmp(ParameterName, "HouseX", false) == 0) // If the parametername is correct ("HouseX")
					AHouseData[HouseID][HouseX] = floatstr(ParameterValue); // Store the HouseX
				if (strcmp(ParameterName, "HouseY", false) == 0) // If the parametername is correct ("HouseY")
					AHouseData[HouseID][HouseY] = floatstr(ParameterValue); // Store the HouseY
				if (strcmp(ParameterName, "HouseZ", false) == 0) // If the parametername is correct ("HouseZ")
					AHouseData[HouseID][HouseZ] = floatstr(ParameterValue); // Store the HouseZ
				if (strcmp(ParameterName, "HouseLevel", false) == 0) // If the parametername is correct ("HouseLevel")
					AHouseData[HouseID][HouseLevel] = strval(ParameterValue); // Store the HouseLevel
				if (strcmp(ParameterName, "HouseMaxLevel", false) == 0) // If the parametername is correct ("HouseMaxLevel")
					AHouseData[HouseID][HouseMaxLevel] = strval(ParameterValue); // Store the HouseMaxLevel
				if (strcmp(ParameterName, "HousePrice", false) == 0) // If the parametername is correct ("HousePrice")
					AHouseData[HouseID][HousePrice] = strval(ParameterValue); // Store the HousePrice
				if (strcmp(ParameterName, "Owned", false) == 0) // If the parametername is correct ("Owned")
				{
				    if (strcmp(ParameterValue, "Yes", false) == 0) // If the value "Yes" was read
						AHouseData[HouseID][Owned] = true; // House is owned
					else
						AHouseData[HouseID][Owned] = false; // House is not owned
				}
				if (strcmp(ParameterName, "Owner", false) == 0) // If the parametername is correct ("Owner")
					// Store the Owner (Owner will hold "1" if there is no owner (empty string), done by "sscanf" I guess)
					// But this doesn't matter, as the owner will never be displayed when the house is not owned by someone
				    format(AHouseData[HouseID][Owner], 24, ParameterValue);
				if (strcmp(ParameterName, "Insurance", false) == 0) // If the parametername is correct ("Insurance")
					AHouseData[HouseID][Insurance] = strval(ParameterValue); // Store the Insurance
				if (strcmp(ParameterName, "AutoEvictDays", false) == 0) // If the parametername is correct ("AutoEvictDays")
					AHouseData[HouseID][AutoEvictDays] = strval(ParameterValue); // Store the AutoEvictDays
			}
			else // OnlyLoadVehicles is "true", so only read the vehicle-data
			{
				if (strcmp(ParameterName, "[Vehicle]", false) == 0) // If the parametername is correct ("[Vehicle]")
				{
				    // Clear all data to start a new vehicle
				    cModel = 0;
				    cPaint = 0;
				    cFuel = -1;
				    Neons = 0;
				    for (new i; i < 14; i++)
				        components[i] = 0;
				}
				if (strcmp(ParameterName, "VehicleModel", false) == 0) // If the parametername is correct ("VehicleModel")
					cModel = strval(ParameterValue); // Store the VehicleModel
				if (strcmp(ParameterName, "Fuel", false) == 0) // If the parametername is correct ("Fuel")
					cFuel = strval(ParameterValue); // Store the Fuel
				if (strcmp(ParameterName, "VehiclePaintJob", false) == 0) // If the parametername is correct ("VehiclePaintJob")
					cPaint = strval(ParameterValue); // Store the VehiclePaintJob
				if (strcmp(ParameterName, "VehicleSpoiler", false) == 0) // If the parametername is correct ("VehicleSpoiler")
					components[0] = strval(ParameterValue); // Store the VehicleSpoiler
				if (strcmp(ParameterName, "VehicleHood", false) == 0) // If the parametername is correct ("VehicleHood")
					components[1] = strval(ParameterValue); // Store the VehicleHood
				if (strcmp(ParameterName, "VehicleRoof", false) == 0) // If the parametername is correct ("VehicleRoof")
					components[2] = strval(ParameterValue); // Store the VehicleRoof
				if (strcmp(ParameterName, "VehicleSideSkirt", false) == 0) // If the parametername is correct ("VehicleSideSkirt")
					components[3] = strval(ParameterValue); // Store the VehicleSideSkirt
				if (strcmp(ParameterName, "VehicleLamps", false) == 0) // If the parametername is correct ("VehicleLamps")
					components[4] = strval(ParameterValue); // Store the VehicleLamps
				if (strcmp(ParameterName, "VehicleNitro", false) == 0) // If the parametername is correct ("VehicleNitro")
					components[5] = strval(ParameterValue); // Store the VehicleNitro
				if (strcmp(ParameterName, "VehicleExhaust", false) == 0) // If the parametername is correct ("VehicleExhaust")
					components[6] = strval(ParameterValue); // Store the VehicleExhaust
				if (strcmp(ParameterName, "VehicleWheels", false) == 0) // If the parametername is correct ("VehicleWheels")
					components[7] = strval(ParameterValue); // Store the VehicleWheels
				if (strcmp(ParameterName, "VehicleStereo", false) == 0) // If the parametername is correct ("VehicleStereo")
					components[8] = strval(ParameterValue); // Store the VehicleStereo
				if (strcmp(ParameterName, "VehicleHydraulics", false) == 0) // If the parametername is correct ("VehicleHydraulics")
					components[9] = strval(ParameterValue); // Store the VehicleHydraulics
				if (strcmp(ParameterName, "VehicleFrontBumper", false) == 0) // If the parametername is correct ("VehicleFrontBumper")
					components[10] = strval(ParameterValue); // Store the VehicleFrontBumper
				if (strcmp(ParameterName, "VehicleRearBumper", false) == 0) // If the parametername is correct ("VehicleRearBumper")
					components[11] = strval(ParameterValue); // Store the VehicleRearBumper
				if (strcmp(ParameterName, "VehicleVentRight", false) == 0) // If the parametername is correct ("VehicleVentRight")
					components[12] = strval(ParameterValue); // Store the VehicleVentRight
				if (strcmp(ParameterName, "VehicleVentLeft", false) == 0) // If the parametername is correct ("VehicleVentLeft")
					components[13] = strval(ParameterValue); // Store the VehicleVentLeft

				if (strcmp(ParameterName, "Color1", false) == 0) // If the parametername is correct ("Color1")
					Col1 = strval(ParameterValue); // Store the Color1
				if (strcmp(ParameterName, "Color2", false) == 0) // If the parametername is correct ("Color2")
					Col2 = strval(ParameterValue); // Store the Color2

				if (strcmp(ParameterName, "VehicleX", false) == 0) // If the parametername is correct ("VehicleX")
					cx = floatstr(ParameterValue); // Store the VehicleX
				if (strcmp(ParameterName, "VehicleY", false) == 0) // If the parametername is correct ("VehicleY")
					cy = floatstr(ParameterValue); // Store the VehicleY
				if (strcmp(ParameterName, "VehicleZ", false) == 0) // If the parametername is correct ("VehicleZ")
					cz = floatstr(ParameterValue); // Store the VehicleZ
				if (strcmp(ParameterName, "VehicleAngle", false) == 0) // If the parametername is correct ("VehicleAngle")
					crot = floatstr(ParameterValue); // Store the VehicleAngle
				if (strcmp(ParameterName, "NeonsApplied", false) == 0) // If the parametername is correct ("NeonsApplied")
					Neons = strval(ParameterValue); // Store the NeonsApplied

				if (strcmp(ParameterName, "Clamped", false) == 0) // If the parametername is correct ("Clamped")
				{
				    if (strcmp(ParameterValue, "Yes", false) == 0) // If the value "Yes" was read
						VehicleClamped = true; // Vehicle is clamped
					else
						VehicleClamped = false; // Vehicle is not clamped
				}

				if (strcmp(ParameterName, "[/Vehicle]", false) == 0) // If the parametername is correct ("[/Vehicle]")
				{
					// Set both colors to 1 if they are 0 AND if there is a paintjob applied
					if ((Col1 == 0) && (cPaint != 0))
					    Col1 = 1;
					if ((Col2 == 0) && (cPaint != 0))
					    Col2 = 1;

					// The "[/Vehicle]" is found, this means that all data about this vehicle is now stored in the variables
					// Now add the vehicle to the house and set it's data
					vid = House_AddVehicle(HouseID, cModel, cPaint, components, cx, cy, cz, crot, Col1, Col2);
					AVehicleData[vid][Clamped] = VehicleClamped;
					// Apply the neons if they have been loaded
					if (Neons != 0)
					{
						AVehicleData[vid][NeonObjectModel] = Neons;
					    AVehicleData[vid][NeonLeft] = CreateObject(AVehicleData[vid][NeonObjectModel],0,0,0,0,0,0);
					    AVehicleData[vid][NeonRight] = CreateObject(AVehicleData[vid][NeonObjectModel],0,0,0,0,0,0);
						AttachObjectToVehicle(AVehicleData[vid][NeonLeft], vid, -0.8, 0.0, -0.70, 0.0, 0.0, 0.0);
						AttachObjectToVehicle(AVehicleData[vid][NeonRight], vid, 0.8, 0.0, -0.70, 0.0, 0.0, 0.0);
					}
					// Also set the fuel (set it to maximum when the fuel parameter wasn't inside the file)
					if (cFuel == -1)
						AVehicleData[vid][Fuel] = MaxFuel;
					else // If the parameter was there, store it
						AVehicleData[vid][Fuel] = cFuel;
				}
			}

            // Read the next line of the file
			fread(HFile, LineFromFile);
		}

		// Check if the maximum house-level isn't 0 (when updating to the new version)
		if (AHouseData[HouseID][HouseMaxLevel] == 0)
		    AHouseData[HouseID][HouseMaxLevel] = 10; // Set the maximum level to 10

        // Close the file
		fclose(HFile);

        // Return if the file was read correctly
		return 1;
	}
	else
	    return 0; // Return 0 if the file couldn't be read (doesn't exist)
}

// This function will save the given house
HouseFile_Save(HouseID)
{
	new file[100], File:HFile, LineForFile[100], vid;

    // Construct the complete filename for this house
	format(file, sizeof(file), HouseFile, HouseID);

	HFile = fopen(file, io_write); // Open the playerfile for writing

	format(LineForFile, sizeof(LineForFile), "HouseName %s\r\n", AHouseData[HouseID][HouseName]); // Construct the line: "HouseName <HouseName>"
	fwrite(HFile, LineForFile); // And save it to the file

	format(LineForFile, sizeof(LineForFile), "HouseX %f\r\n", AHouseData[HouseID][HouseX]); // Construct the line: "HouseX <HouseX>"
	fwrite(HFile, LineForFile); // And save it to the file

	format(LineForFile, sizeof(LineForFile), "HouseY %f\r\n", AHouseData[HouseID][HouseY]); // Construct the line: "HouseY <HouseY>"
	fwrite(HFile, LineForFile); // And save it to the file

	format(LineForFile, sizeof(LineForFile), "HouseZ %f\r\n", AHouseData[HouseID][HouseZ]); // Construct the line: "HouseZ <HouseZ>"
	fwrite(HFile, LineForFile); // And save it to the file

	format(LineForFile, sizeof(LineForFile), "HouseLevel %i\r\n", AHouseData[HouseID][HouseLevel]); // Construct the line: "HouseLevel <HouseLevel>"
	fwrite(HFile, LineForFile); // And save it to the file

	format(LineForFile, sizeof(LineForFile), "HouseMaxLevel %i\r\n", AHouseData[HouseID][HouseMaxLevel]); // Construct the line: "HouseMaxLevel <HouseMaxLevel>"
	fwrite(HFile, LineForFile); // And save it to the file

	format(LineForFile, sizeof(LineForFile), "HousePrice %i\r\n", AHouseData[HouseID][HousePrice]); // Construct the line: "HousePrice <HousePrice>"
	fwrite(HFile, LineForFile); // And save it to the file

	if (AHouseData[HouseID][Owned] == true) // Check if the house is owned
	{
		format(LineForFile, sizeof(LineForFile), "Owned Yes\r\n"); // Construct the line: "Owned Yes"
		fwrite(HFile, LineForFile); // And save it to the file
	}
	else
	{
		format(LineForFile, sizeof(LineForFile), "Owned No\r\n"); // Construct the line: "Owned No"
		fwrite(HFile, LineForFile); // And save it to the file
	}

	format(LineForFile, sizeof(LineForFile), "Owner %s\r\n", AHouseData[HouseID][Owner]); // Construct the line: "Owner <Owner>"
	fwrite(HFile, LineForFile); // And save it to the file

	format(LineForFile, sizeof(LineForFile), "Insurance %i\r\n", AHouseData[HouseID][Insurance]); // Construct the line: "Insurance <Insurance>"
	fwrite(HFile, LineForFile); // And save it to the file

	AHouseData[HouseID][AutoEvictDays] = AutoEvict[AEDays]; // Update the AutoEvictDays for this house when the player logs out
	format(LineForFile, sizeof(LineForFile), "AutoEvictDays %i\r\n", AutoEvict[AEDays]); // Construct the line: "AutoEvictDays <AutoEvictDays>"
	fwrite(HFile, LineForFile); // And save it to the file



	// Save the vehicle-data for every vehicle added to the house
	for (new CarSlot; CarSlot < 10; CarSlot++)
	{
	    // If a valid vehicle-id has been found
		if (AHouseData[HouseID][VehicleIDs][CarSlot] != 0)
		{
			// Get the vehicle id
			vid = AHouseData[HouseID][VehicleIDs][CarSlot];

		    format(LineForFile, sizeof(LineForFile), "[Vehicle]\r\n"); // Construct the line: "[Vehicle]"
			fwrite(HFile, LineForFile); // And save it to the file

		    format(LineForFile, sizeof(LineForFile), "VehicleModel %i\r\n", AVehicleData[vid][Model]); // Construct the line: "VehicleModel <VehicleModel>"
			fwrite(HFile, LineForFile); // And save it to the file
		    format(LineForFile, sizeof(LineForFile), "Fuel %i\r\n", AVehicleData[vid][Fuel]); // Construct the line: "Fuel <Fuel>"
			fwrite(HFile, LineForFile); // And save it to the file
		    format(LineForFile, sizeof(LineForFile), "VehiclePaintJob %i\r\n", AVehicleData[vid][PaintJob]); // Construct the line: "VehiclePaintJob <VehiclePaintJob>"
			fwrite(HFile, LineForFile); // And save it to the file

		    format(LineForFile, sizeof(LineForFile), "VehicleSpoiler %i\r\n", AVehicleData[vid][Components][0]); // Construct the line: "VehicleSpoiler <VehicleSpoiler>"
			fwrite(HFile, LineForFile); // And save it to the file
		    format(LineForFile, sizeof(LineForFile), "VehicleHood %i\r\n", AVehicleData[vid][Components][1]); // Construct the line: "VehicleHood <VehicleHood>"
			fwrite(HFile, LineForFile); // And save it to the file
		    format(LineForFile, sizeof(LineForFile), "VehicleRoof %i\r\n", AVehicleData[vid][Components][2]); // Construct the line: "VehicleRoof <VehicleRoof>"
			fwrite(HFile, LineForFile); // And save it to the file
		    format(LineForFile, sizeof(LineForFile), "VehicleSideSkirt %i\r\n", AVehicleData[vid][Components][3]); // Construct the line: "VehicleSideSkirt <VehicleSideSkirt>"
			fwrite(HFile, LineForFile); // And save it to the file
		    format(LineForFile, sizeof(LineForFile), "VehicleLamps %i\r\n", AVehicleData[vid][Components][4]); // Construct the line: "VehicleLamps <VehicleLamps>"
			fwrite(HFile, LineForFile); // And save it to the file
		    format(LineForFile, sizeof(LineForFile), "VehicleNitro %i\r\n", AVehicleData[vid][Components][5]); // Construct the line: "VehicleNitro <VehicleNitro>"
			fwrite(HFile, LineForFile); // And save it to the file
		    format(LineForFile, sizeof(LineForFile), "VehicleExhaust %i\r\n", AVehicleData[vid][Components][6]); // Construct the line: "VehicleSpoiler <VehicleSpoiler>"
			fwrite(HFile, LineForFile); // And save it to the file
		    format(LineForFile, sizeof(LineForFile), "VehicleWheels %i\r\n", AVehicleData[vid][Components][7]); // Construct the line: "VehicleWheels <VehicleWheels>"
			fwrite(HFile, LineForFile); // And save it to the file
		    format(LineForFile, sizeof(LineForFile), "VehicleStereo %i\r\n", AVehicleData[vid][Components][8]); // Construct the line: "VehicleStereo <VehicleStereo>"
			fwrite(HFile, LineForFile); // And save it to the file
		    format(LineForFile, sizeof(LineForFile), "VehicleHydraulics %i\r\n", AVehicleData[vid][Components][9]); // Construct the line: "VehicleHydraulics <VehicleHydraulics>"
			fwrite(HFile, LineForFile); // And save it to the file
		    format(LineForFile, sizeof(LineForFile), "VehicleFrontBumper %i\r\n", AVehicleData[vid][Components][10]); // Construct the line: "VehicleFrontBumper <VehicleFrontBumper>"
			fwrite(HFile, LineForFile); // And save it to the file
		    format(LineForFile, sizeof(LineForFile), "VehicleRearBumper %i\r\n", AVehicleData[vid][Components][11]); // Construct the line: "VehicleRearBumper <VehicleRearBumper>"
			fwrite(HFile, LineForFile); // And save it to the file
		    format(LineForFile, sizeof(LineForFile), "VehicleVentRight %i\r\n", AVehicleData[vid][Components][12]); // Construct the line: "VehicleVentRight <VehicleVentRight>"
			fwrite(HFile, LineForFile); // And save it to the file
		    format(LineForFile, sizeof(LineForFile), "VehicleVentLeft %i\r\n", AVehicleData[vid][Components][13]); // Construct the line: "VehicleVentLeft <VehicleVentLeft>"
			fwrite(HFile, LineForFile); // And save it to the file

		    format(LineForFile, sizeof(LineForFile), "Color1 %i\r\n", AVehicleData[vid][Color1]); // Construct the line: "Color1 <Color1>"
			fwrite(HFile, LineForFile); // And save it to the file
		    format(LineForFile, sizeof(LineForFile), "Color2 %i\r\n", AVehicleData[vid][Color2]); // Construct the line: "Color2 <Color2>"
			fwrite(HFile, LineForFile); // And save it to the file

		    format(LineForFile, sizeof(LineForFile), "VehicleX %f\r\n", AVehicleData[vid][SpawnX]); // Construct the line: "VehicleVentLeft <VehicleVentLeft>"
			fwrite(HFile, LineForFile); // And save it to the file
		    format(LineForFile, sizeof(LineForFile), "VehicleY %f\r\n", AVehicleData[vid][SpawnY]); // Construct the line: "VehicleVentLeft <VehicleVentLeft>"
			fwrite(HFile, LineForFile); // And save it to the file
		    format(LineForFile, sizeof(LineForFile), "VehicleZ %f\r\n", AVehicleData[vid][SpawnZ]); // Construct the line: "VehicleVentLeft <VehicleVentLeft>"
			fwrite(HFile, LineForFile); // And save it to the file
		    format(LineForFile, sizeof(LineForFile), "VehicleAngle %f\r\n", AVehicleData[vid][SpawnRot]); // Construct the line: "VehicleVentLeft <VehicleVentLeft>"
			fwrite(HFile, LineForFile); // And save it to the file
			format(LineForFile, sizeof(LineForFile), "NeonsApplied %i\r\n", AVehicleData[vid][NeonObjectModel]); // Construct the line: "NeonsApplied <NeonsApplied>"
			fwrite(HFile, LineForFile); // And save it to the file
			
			if (AVehicleData[vid][Clamped] == true)
			    format(LineForFile, sizeof(LineForFile), "Clamped Yes\r\n"); // Construct the line: "Clamped <Yes>"
			else
			    format(LineForFile, sizeof(LineForFile), "Clamped No\r\n"); // Construct the line: "Clamped <No>"

			fwrite(HFile, LineForFile); // And save it to the file

		    format(LineForFile, sizeof(LineForFile), "[/Vehicle]\r\n"); // Construct the line: "[/Vehicle]"
			fwrite(HFile, LineForFile); // And save it to the file
		}
	}

	fclose(HFile); // Close the file

	return 1;
}



// This function loads all speedcameras
Camera_LoadAll()
{
	// Loop through all cameras and try to load them
	for (new CamID; CamID < MAX_CAMERAS; CamID++)
	{
	    // Try to load the file and check if it was succesfully loaded (file exists)
		if (CameraFile_Load(CamID) == 1)
		{
			// Save the number of camera's that have been loaded, so it can be displayed
		    TotalCameras++;
		}
	}

	return 1;
}



// This function will load the speedcamera's datafile (used when the server is started to load all cameras)
CameraFile_Load(CamID)
{
	// Setup local variables
	new file[100], File:CFile, LineFromFile[100], ParameterName[50], ParameterValue[50];
	new Float:x, Float:y, Float:z, Float:rot, MaxSpeed;

	format(file, sizeof(file), CameraFile, CamID); // Construct the complete filename for this camera-file

	if (fexist(file))
	{
		CFile = fopen(file, io_read); // Open the camerafile for reading

		fread(CFile, LineFromFile); // Read the first line of the file

		// Keep reading until the end of the file is found (no more data)
		while (strlen(LineFromFile) > 0)
		{
			StripNewLine(LineFromFile); // Strip any newline characters from the LineFromFile
			sscanf(LineFromFile, "s[50]s[50]", ParameterName, ParameterValue); // Extract parametername and parametervalue

			// Store the proper value in the proper place
			if (strcmp(ParameterName, "CamX", false) == 0) // If the parametername is correct ("CamX")
			    x = floatstr(ParameterValue); // Store the CamX
			if (strcmp(ParameterName, "CamY", false) == 0) // If the parametername is correct ("CamY")
			    y = floatstr(ParameterValue); // Store the CamY
			if (strcmp(ParameterName, "CamZ", false) == 0) // If the parametername is correct ("CamZ")
			    z = floatstr(ParameterValue); // Store the CamZ
			if (strcmp(ParameterName, "CamAngle", false) == 0) // If the parametername is correct ("CamAngle")
			    rot = floatstr(ParameterValue); // Store the CamAngle
			if (strcmp(ParameterName, "CamSpeed", false) == 0) // If the parametername is correct ("CamSpeed")
			    MaxSpeed = strval(ParameterValue); // Store the CamSpeed

			fread(CFile, LineFromFile); // Read the next line of the file
		}

		fclose(CFile); // Close the file

		// All data has been read about the camera, now setup the camera
		SetupSpeedCamera(CamID, x, y, z, rot, MaxSpeed);

		return 1; // Return that the file was read correctly
	}
	else
	    return 0; // Return 0 if the file couldn't be read (doesn't exist)
}



// This function loads all businesses
Business_LoadAll()
{
	// First load the business-time file, this one holds the current business-time which is increased every minute the server if online
	BusinessTime_Load();

	// Loop through all businesses and try to load them
	for (new BusID = 1; BusID < MAX_BUSINESS; BusID++)
	{
	    // Try to load the file and check if it was succesfully loaded (file exists)
		if (BusinessFile_Load(BusID) == 1)
		{
			// Add a pickup and 3DText for this business
			Business_CreateEntrance(BusID);
			// Count the loaded businesses
		    TotalBusiness++;
		}
	}

	return 1;
}

// This function loads the file that holds the current business-time
BusinessTime_Load()
{
	// Setup local variables
	new File:BFile, LineFromFile[100], ParameterName[50], ParameterValue[50];

	// Try to load the businesstime file
	if (fexist("ServerData/Business/BusinessTime.ini"))
	{
		BFile = fopen("ServerData/Business/BusinessTime.ini", io_read); // Open the businesstime-file for reading

		fread(BFile, LineFromFile); // Read the first line of the file

		// Keep reading until the end of the file is found (no more data)
		while (strlen(LineFromFile) > 0)
		{
			StripNewLine(LineFromFile); // Strip any newline characters from the LineFromFile
			sscanf(LineFromFile, "s[50]s[50]", ParameterName, ParameterValue); // Extract parametername and parametervalue

			// Store the proper value in the proper place
			if (strcmp(ParameterName, "BusinessTime", false) == 0) // If the parametername is correct ("BusinessTime")
				BusinessTransactionTime = strval(ParameterValue); // Store the BusinessTime

            // Read the next line of the file
			fread(BFile, LineFromFile);
		}

        // Close the file
		fclose(BFile);

        // Return if the file was read correctly
		return 1;
	}
	else
	    return 0; // Return 0 if the file couldn't be read (doesn't exist)
}

// This function saves the file that holds the current business-time
BusinessTime_Save()
{
	new File:BFile, LineForFile[100];

	BFile = fopen("ServerData/Business/BusinessTime.ini", io_write); // Open the businesstime-file for writing

	format(LineForFile, sizeof(LineForFile), "BusinessTime %i\r\n", BusinessTransactionTime); // Construct the line: "BusinessTime <BusinessTransactionTime>"
	fwrite(BFile, LineForFile); // And save it to the file

	fclose(BFile); // Close the file

	return 1;
}



// This function will load the business's datafile (used when the server is started to load all businesses)
BusinessFile_Load(BusID)
{
	// Setup local variables
	new file[100], File:BFile, LineFromFile[100], ParameterName[50], ParameterValue[50];

	format(file, sizeof(file), BusinessFile, BusID); // Construct the complete filename for this business-file

	if (fexist(file))
	{
		BFile = fopen(file, io_read); // Open the housefile for reading

		fread(BFile, LineFromFile); // Read the first line of the file

		// Keep reading until the end of the file is found (no more data)
		while (strlen(LineFromFile) > 0)
		{
			StripNewLine(LineFromFile); // Strip any newline characters from the LineFromFile
			sscanf(LineFromFile, "s[50]s[50]", ParameterName, ParameterValue); // Extract parametername and parametervalue

			// Store the proper value in the proper place
			if (strcmp(ParameterName, "BusinessName", false) == 0) // If the parametername is correct ("BusinessName")
			    format(ABusinessData[BusID][BusinessName], 24, ParameterValue); // Store the BusinessName
			if (strcmp(ParameterName, "BusinessX", false) == 0) // If the parametername is correct ("BusinessX")
				ABusinessData[BusID][BusinessX] = floatstr(ParameterValue); // Store the BusinessX
			if (strcmp(ParameterName, "BusinessY", false) == 0) // If the parametername is correct ("BusinessY")
				ABusinessData[BusID][BusinessY] = floatstr(ParameterValue); // Store the BusinessY
			if (strcmp(ParameterName, "BusinessZ", false) == 0) // If the parametername is correct ("BusinessZ")
				ABusinessData[BusID][BusinessZ] = floatstr(ParameterValue); // Store the BusinessZ
			if (strcmp(ParameterName, "BusinessType", false) == 0) // If the parametername is correct ("BusinessType")
				ABusinessData[BusID][BusinessType] = strval(ParameterValue); // Store the BusinessType
			if (strcmp(ParameterName, "BusinessLevel", false) == 0) // If the parametername is correct ("BusinessLevel")
				ABusinessData[BusID][BusinessLevel] = strval(ParameterValue); // Store the BusinessLevel
			if (strcmp(ParameterName, "LastTransaction", false) == 0) // If the parametername is correct ("LastTransaction")
				ABusinessData[BusID][LastTransaction] = strval(ParameterValue); // Store the LastTransaction
			if (strcmp(ParameterName, "Owned", false) == 0) // If the parametername is correct ("Owned")
			{
			    if (strcmp(ParameterValue, "Yes", false) == 0) // If the value "Yes" was read
					ABusinessData[BusID][Owned] = true; // House is owned
				else
					ABusinessData[BusID][Owned] = false; // House is not owned
			}
			if (strcmp(ParameterName, "Owner", false) == 0) // If the parametername is correct ("Owner")
			    format(ABusinessData[BusID][Owner], 24, ParameterValue);
			
			if (strcmp(ParameterName, "AutoEvictDays", false) == 0) // If the parametername is correct ("AutoEvictDays")
				ABusinessData[BusID][AutoEvictDays] = strval(ParameterValue); // Store the AutoEvictDays

            // Read the next line of the file
			fread(BFile, LineFromFile);
		}

        // Close the file
		fclose(BFile);

        // Return if the file was read correctly
		return 1;
	}
	else
	    return 0; // Return 0 if the file couldn't be read (doesn't exist)
}

// This function will save the given business
BusinessFile_Save(BusID)
{
	new file[100], File:BFile, LineForFile[100];

    // Construct the complete filename for this business
	format(file, sizeof(file), BusinessFile, BusID);

	BFile = fopen(file, io_write); // Open the business-file for writing

	format(LineForFile, sizeof(LineForFile), "BusinessName %s\r\n", ABusinessData[BusID][BusinessName]); // Construct the line: "BusinessName <BusinessName>"
	fwrite(BFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "BusinessX %f\r\n", ABusinessData[BusID][BusinessX]); // Construct the line: "BusinessX <BusinessX>"
	fwrite(BFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "BusinessY %f\r\n", ABusinessData[BusID][BusinessY]); // Construct the line: "BusinessY <BusinessY>"
	fwrite(BFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "BusinessZ %f\r\n", ABusinessData[BusID][BusinessZ]); // Construct the line: "BusinessZ <BusinessZ>"
	fwrite(BFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "BusinessType %i\r\n", ABusinessData[BusID][BusinessType]); // Construct the line: "BusinessType <BusinessType>"
	fwrite(BFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "BusinessLevel %i\r\n", ABusinessData[BusID][BusinessLevel]); // Construct the line: "BusinessLevel <BusinessLevel>"
	fwrite(BFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "LastTransaction %i\r\n", ABusinessData[BusID][LastTransaction]); // Construct the line: "LastTransaction <LastTransaction>"
	fwrite(BFile, LineForFile); // And save it to the file

	ABusinessData[BusID][AutoEvictDays] = AutoEvict[AEDays]; // Update the AutoEvictDays for this business when the player logs out
	format(LineForFile, sizeof(LineForFile), "AutoEvictDays %i\r\n", AutoEvict[AEDays]); // Construct the line: "AutoEvictDays <AutoEvictDays>"
	fwrite(BFile, LineForFile); // And save it to the file

	if (ABusinessData[BusID][Owned] == true) // Check if the house is owned
	{
		format(LineForFile, sizeof(LineForFile), "Owned Yes\r\n"); // Construct the line: "Owned Yes"
		fwrite(BFile, LineForFile); // And save it to the file
	}
	else
	{
		format(LineForFile, sizeof(LineForFile), "Owned No\r\n"); // Construct the line: "Owned No"
		fwrite(BFile, LineForFile); // And save it to the file
	}

	format(LineForFile, sizeof(LineForFile), "Owner %s\r\n", ABusinessData[BusID][Owner]); // Construct the line: "Owner <Owner>"
	fwrite(BFile, LineForFile); // And save it to the file

	fclose(BFile); // Close the file

	return 1;
}



// This function will load the player's bank account and process transfers that were made to his account when he was offline
BankFile_Load(playerid)
{
	// Setup local variables
	new file[100], File:BFile, Name[24], LineFromFile[100], ParameterName[50], ParameterValue[50];
	new TransferMoney, TransferName[24], TransferTime[24], Msg[128];
	new IntrestAmount, OldBankMoney, NewBankMoney;

	format(Name, sizeof(Name), APlayerData[playerid][PlayerName]); // Get the playername
	format(file, sizeof(file), BankFile, Name); // Construct the complete filename for this player's bank-account

	if (fexist(file))
	{
		// Send a message to the player to inform him that he has a bank account
	    SendClientMessage(playerid, COLOR_DODGERBLUE, "You have a bank-account. Use {FFBB00}/bank{00BBFF} to login");

		BFile = fopen(file, io_read); // Open the bankfile for reading

		fread(BFile, LineFromFile); // Read the first line of the file

		// Keep reading until the end of the file is found (no more data)
		while (strlen(LineFromFile) > 0)
		{
			StripNewLine(LineFromFile); // Strip any newline characters from the LineFromFile
			sscanf(LineFromFile, "s[50]s[50]", ParameterName, ParameterValue); // Extract parametername and parametervalue

			// Store the proper value in the proper place
			if (strcmp(ParameterName, "Password", false) == 0) // If the parametername is correct ("Password")
			    format(APlayerData[playerid][BankPassword], 24, ParameterValue); // Store the Password
			if (strcmp(ParameterName, "Money", false) == 0) // If the parametername is correct ("Money")
				APlayerData[playerid][BankMoney] = strval(ParameterValue); // Store the Money
			if (strcmp(ParameterName, "LastIntrestTime", false) == 0) // If the parametername is correct ("LastIntrestTime")
				APlayerData[playerid][LastIntrestTime] = strval(ParameterValue); // Store the LastIntrestTime

			// Process transfer-data
			if (strcmp(ParameterName, "[Transfer]", false) == 0) // If the parametername is correct ("[Transfer]")
			{
				TransferMoney = 0; // Clear the money variable
			    for (new i; i < sizeof(TransferName); i++) // Clear the name variable
			        TransferName[i] = 0;
			    for (new i; i < sizeof(TransferTime); i++) // Clear the time variable
			        TransferTime[i] = 0;
			}
			if (strcmp(ParameterName, "TransferMoney", false) == 0) // If the parametername is correct ("TransferMoney")
				TransferMoney = strval(ParameterValue); // Store the TransferMoney
			if (strcmp(ParameterName, "TransferName", false) == 0) // If the parametername is correct ("TransferName")
			    format(TransferName, sizeof(TransferName), ParameterValue); // Store the TransferName
			if (strcmp(ParameterName, "TransferTime", false) == 0) // If the parametername is correct ("TransferTime")
			    format(TransferTime, sizeof(TransferTime), ParameterValue); // Store the TransferTime
			if (strcmp(ParameterName, "[/Transfer]", false) == 0) // If the parametername is correct ("[/Transfer]")
			{
				// Add the money to your bank-account that was transferred by another player
				APlayerData[playerid][BankMoney] = APlayerData[playerid][BankMoney] + TransferMoney;
				// Inform the player that a player has transferred some money to your bank account
				format(Msg, sizeof(Msg), "{00BBFF}Player {FFBB00}%s{00BBFF} has transferred {FFBB00}$%i{00BBFF}", TransferName, TransferMoney);
				SendClientMessage(playerid, COLOR_WHITE, Msg);
				format(Msg, sizeof(Msg), "{00BBFF}to your bank account on {FFBB00}%s", TransferTime);
				SendClientMessage(playerid, COLOR_WHITE, Msg);
			}

            // Read the next line of the file
			fread(BFile, LineFromFile);
		}

        // Close the file
		fclose(BFile);

		// Process intrest if intrest is enabled for the time the player was offline
		if (IntrestEnabled == true)
		{
		    // Check if the player has any money in his bank account (adding intrest to $0 is useless)
			if (APlayerData[playerid][BankMoney] > 0)
			{
				// Also process intrest for this bank account for every hour the player was offline
				if (APlayerData[playerid][LastIntrestTime] < CurrentIntrestTime)
				{
				    // Get the old amount of money
				    OldBankMoney = APlayerData[playerid][BankMoney];
					// Count the hours the player was offline and didn't receive intrest yet
					for (new i; i < (CurrentIntrestTime - APlayerData[playerid][LastIntrestTime]); i++)
					{
						// Calculate the intrest
						IntrestAmount = floatround(floatmul(APlayerData[playerid][BankMoney], BankIntrest), floatround_floor);
						// Add the intrest to this player's bank account
						APlayerData[playerid][BankMoney] = APlayerData[playerid][BankMoney] + IntrestAmount;
					}
				    // Get the new amount of money
				    NewBankMoney = APlayerData[playerid][BankMoney];
					// Save the CurrentIntrestTime for this bank account
					APlayerData[playerid][LastIntrestTime] = CurrentIntrestTime;

					// Inform the player that he received intrest on his bank account and how much
					format(Msg, sizeof(Msg), "{00BBFF}You've received {FFBB00}$%i{00BBFF} intrest on your bank account", NewBankMoney - OldBankMoney);
					SendClientMessage(playerid, COLOR_WHITE, Msg);
				}
			}
		}

		// Re-save the bank account (in case intrest and transfers have been processed)
		BankFile_Save(playerid);

        // Return if the file was read correctly
		return 1;
	}
	else
	{
		// Send a message if the player doesn't have a bank account
	    SendClientMessage(playerid, COLOR_DODGERBLUE, "You don't have a bank-account. Use {FFBB00}/bank{00BBFF} to set it up");

	    return 0; // Return 0 if the file couldn't be read (doesn't exist)
	}
}

// This function will save the player's bank account
BankFile_Save(playerid)
{
	// Setup local variables
	new file[100], File:BFile, LineForFile[100], Name[24];

	format(Name, sizeof(Name), APlayerData[playerid][PlayerName]); // Get the playername
	format(file, sizeof(file), BankFile, Name); // Construct the complete filename for this player's bank-account

	BFile = fopen(file, io_write); // Open the business-file for writing

	format(LineForFile, sizeof(LineForFile), "Password %s\r\n", APlayerData[playerid][BankPassword]); // Construct the line: "Password <BankPassword>"
	fwrite(BFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "Money %i\r\n", APlayerData[playerid][BankMoney]); // Construct the line: "Money <BankMoney>"
	fwrite(BFile, LineForFile); // And save it to the file
	format(LineForFile, sizeof(LineForFile), "LastIntrestTime %i\r\n", APlayerData[playerid][LastIntrestTime]); // Construct the line: "LastIntrestTime <LastIntrestTime>"
	fwrite(BFile, LineForFile); // And save it to the file

	fclose(BFile); // Close the file

	return 1;
}



// This function loads the file that holds the current intrest-time
IntrestTime_Load()
{
	// Setup local variables
	new File:BFile, LineFromFile[100], ParameterName[50], ParameterValue[50];

	// Try to load the businesstime file
	if (fexist("ServerData/Bank/IntrestTime.ini"))
	{
		BFile = fopen("ServerData/Bank/IntrestTime.ini", io_read); // Open the intrest-time-file for reading

		fread(BFile, LineFromFile); // Read the first line of the file

		// Keep reading until the end of the file is found (no more data)
		while (strlen(LineFromFile) > 0)
		{
			StripNewLine(LineFromFile); // Strip any newline characters from the LineFromFile
			sscanf(LineFromFile, "s[50]s[50]", ParameterName, ParameterValue); // Extract parametername and parametervalue

			// Store the proper value in the proper place
			if (strcmp(ParameterName, "CurrentIntrestTime", false) == 0) // If the parametername is correct ("CurrentIntrestTime")
				CurrentIntrestTime = strval(ParameterValue); // Store the CurrentIntrestTime

            // Read the next line of the file
			fread(BFile, LineFromFile);
		}

        // Close the file
		fclose(BFile);

        // Return if the file was read correctly
		return 1;
	}
	else
	    return 0; // Return 0 if the file couldn't be read (doesn't exist)
}

// This function saves the file that holds the current intrest-time
IntrestTime_Save()
{
	new File:BFile, LineForFile[100];

	BFile = fopen("ServerData/Bank/IntrestTime.ini", io_write); // Open the intresttime-file for writing

	format(LineForFile, sizeof(LineForFile), "CurrentIntrestTime %i\r\n", CurrentIntrestTime); // Construct the line: "CurrentIntrestTime <CurrentIntrestTime>"
	fwrite(BFile, LineForFile); // And save it to the file

	fclose(BFile); // Close the file

	return 1;
}

