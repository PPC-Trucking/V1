// This function is required only once to fix houses that were bought and sold again
// If you would sell a house, then restart the server, that house cannot be bought again (= bugged)
// The player doesn't own the house anymore, but the house still has the player as owner
// This function loops through all houses and checks if the owner (if any) still owns the house
// If the owner doesn't own the house, the house's ownership is cleared and the house-file will be re-saved
// After the fixing, you should remove the call to this function in OnGameModeInit, compile and restart the server
// as this function is only required once
stock FixHouses()
{
	// Setup local variables
	new file[100], File:PFile, Name[24], LineFromFile[100], ParameterName[50], ParameterValue[50];
	new bool:PlayerOwnedTheHouse = true, HousesFixed;

	// Loop through all houses
	for (new HouseID = 1; HouseID < MAX_HOUSES; HouseID++)
	{
		// Check if the house is owned
		if (AHouseData[HouseID][Owned] == true)
		{
			// Get the name of the owner
			format(Name, sizeof(Name), AHouseData[HouseID][Owner]);
			// Construct the complete filename for this player's account
			format(file, sizeof(file), PlayerFile, Name);

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

                    // If the parametername is correct ("House")
					if (strcmp(ParameterName, "House", false) == 0)
					{
					    // Check if the player owns this house
					    if (strval(ParameterValue) == HouseID)
                            PlayerOwnedTheHouse = true; // The player really owns the house
					}

                    // Read the next line of the file
					fread(PFile, LineFromFile);
				}

                // Close the player-file
				fclose(PFile);

				// Check if the player doesn't own the house
				if (PlayerOwnedTheHouse == false)
				{
					// Clear the ownership from the house
					AHouseData[HouseID][Owned] = false;
					AHouseData[HouseID][Owner] = 0;
					AHouseData[HouseID][HouseName] = 0;
					AHouseData[HouseID][HouseLevel] = 1;
					// Update the house-entrance
					House_UpdateEntrance(HouseID);
					// Save the house, hereby fixing the ownership-bug
					HouseFile_Save(HouseID);
					// Increase the number of houses that have been fixed
					HousesFixed++;
				}

				// Reset the variable that holds if the player owned the house, so the next house/owner can be processed
                PlayerOwnedTheHouse = false;
			}
		}
	}

	// All houses have been processed, now print a line to the server's console
	printf(TXT_HousesFixed, HousesFixed);
}



// Add all static vehicles and pickups when the server starts that are required
GameModeInit_VehiclesPickups()
{
	// Setup local variables
	new LastVehicleMsg[100], LastObjectMsg[100], LastHouseMsg[100], LastBusinessMsg[100], LastCameraMsg[100];

	// Add some trucks to all trucking depots
	GameModeInit_TruckDepots();
	// Add busses to all bus-depots
	GameModeInit_Busses();
	// Add busstops to the map
	GameModeInit_Busstops();
	// Add police vehicles to the Police Impounds
	GameModeInit_Police();
	// Add mafia vehicles to the mafia hideoue
	GameModeInit_Mafia();
	// Add planes and helicopters to the airports
	GameModeInit_Airports();
	// Add towtrucks for assistance class
	GameModeInit_Assistance();
	// Add burrito's and Faggio's for courier class
	GameModeInit_Courier();
	// Add dozers for roadworker class
	GameModeInit_RoadWorker();

	// Add some special pickups
	GameModeInit_PickupsIcons();

	// Add all objects required for toll-gates
	GameModeInit_Toll();

	// Load all houses and create pickups and 3dTexts for them
	Housing_LoadAll();
	// Load all speedcameras
	Camera_LoadAll();
	// Load all businesses and start the business timer (runs every hour)
    Business_LoadAll();
	SetTimer("Business_TransactionTimer", 1000 * 60 * 60, true);
	// Load the current intrest time and start the intrest timer (runs every hour)
	IntrestTime_Load();
	SetTimer("Bank_IntrestTimer", 1000 * 60 * 60, true);

	// Print the LastVehicleID to the server's console
	format(LastVehicleMsg, sizeof(LastVehicleMsg), TXT_LastVehicleID, LastVehicleID);
	format(LastObjectMsg, sizeof(LastObjectMsg), TXT_LastObjectID, LastObjectID);
	format(LastHouseMsg, sizeof(LastHouseMsg), TXT_TotalHouses, TotalHouses);
	format(LastBusinessMsg, sizeof(LastBusinessMsg), TXT_TotalBusiness, TotalBusiness);
	format(LastCameraMsg, sizeof(LastCameraMsg), TXT_TotalSpeedCameras, TotalCameras);
	print(TXT_DebugInfo);
	print("-------------------------------------");
	print(LastVehicleMsg);
	print(LastObjectMsg);
	print(LastHouseMsg);
	print(LastBusinessMsg);
	print(LastCameraMsg);
	print("-------------------------------------");
}



// Add trucks and trailers to the truck-stops
GameModeInit_TruckDepots()
{
	// Add trailers, trucks , Flatbeds and "DFT-30" to Fallen Tree Depot (Class 0)
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerCargo, -475.0, -523.0, 26.0, 90.0, random(126), random(126), 600); // Cargo trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerOre, -475.0, -518.0, 26.0, 90.0, random(126), random(126), 600); // Ore trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerFluids, -475.0, -513.0, 26.0, 90.0, random(126), random(126), 600); // Liquids trailer
	LastVehicleID = Vehicle_AddStatic(VehicleLineRunner, -475.0, -508.0, 26.0, 90.0, random(126), random(126), 600); // LineRunner
	LastVehicleID = Vehicle_AddStatic(VehicleTanker, -475.0, -503.0, 26.0, 90.0, random(126), random(126), 600); // Tanker
	LastVehicleID = Vehicle_AddStatic(VehicleRoadTrain, -475.0, -498.0, 26.0, 90.0, random(126), random(126), 600); // Roadtrain
	LastVehicleID = Vehicle_AddStatic(VehicleDFT30, -475.0, -493.0, 26.0, 90.0, random(126), random(126), 600); // DFT-30
	LastVehicleID = Vehicle_AddStatic(VehicleFlatbed, -475.0, -488.0, 26.0, 90.0, random(126), random(126), 600); // Flatbed
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerCargo, -475.0, -475.0, 26.0, 180.0, random(126), random(126), 600); // Cargo trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerOre, -480.0, -475.0, 26.0, 180.0, random(126), random(126), 600); // Ore trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerFluids, -485.0, -475.0, 26.0, 180.0, random(126), random(126), 600); // Liquids trailer
	LastVehicleID = Vehicle_AddStatic(VehicleLineRunner, -490.0, -475.0, 26.0, 180.0, random(126), random(126), 600); // LineRunner
	LastVehicleID = Vehicle_AddStatic(VehicleTanker, -495.0, -475.0, 26.0, 180.0, random(126), random(126), 600); // Tanker
	LastVehicleID = Vehicle_AddStatic(VehicleRoadTrain, -500.0, -475.0, 26.0, 180.0, random(126), random(126), 600); // Roadtrain
	LastVehicleID = Vehicle_AddStatic(VehicleDFT30, -505.0, -475.0, 26.0, 180.0, random(126), random(126), 600); // DFT-30
	LastVehicleID = Vehicle_AddStatic(VehicleFlatbed, -510.0, -475.0, 26.0, 180.0, random(126), random(126), 600); // Flatbed
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerCargo, -515.0, -475.0, 26.0, 180.0, random(126), random(126), 600); // Cargo trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerOre, -520.0, -475.0, 26.0, 180.0, random(126), random(126), 600); // Ore trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerFluids, -525.0, -475.0, 26.0, 180.0, random(126), random(126), 600); // Liquids trailer
	LastVehicleID = Vehicle_AddStatic(VehicleLineRunner, -530.0, -475.0, 26.0, 180.0, random(126), random(126), 600); // LineRunner
	LastVehicleID = Vehicle_AddStatic(VehicleTanker, -535.0, -475.0, 26.0, 180.0, random(126), random(126), 600); // Tanker
	LastVehicleID = Vehicle_AddStatic(VehicleRoadTrain, -540.0, -475.0, 26.0, 180.0, random(126), random(126), 600); // Roadtrain
	LastVehicleID = Vehicle_AddStatic(VehicleDFT30, -545.0, -475.0, 26.0, 180.0, random(126), random(126), 600); // DFT-30
	LastVehicleID = Vehicle_AddStatic(VehicleFlatbed, -550.0, -475.0, 26.0, 180.0, random(126), random(126), 600); // Flatbed

	// Add "Flatbed" and "DFT-30" trucks to Flint Trucking Depot (Class 1)
	LastVehicleID = Vehicle_AddStatic(VehicleDFT30, -77.0, -1109.0, 1.25, 160.0, random(126), random(126), 600); // DFT-30
	LastVehicleID = Vehicle_AddStatic(VehicleDFT30, -72.0, -1112.0, 1.25, 160.0, random(126), random(126), 600); // DFT-30
	LastVehicleID = Vehicle_AddStatic(VehicleDFT30, -67.0, -1114.0, 1.25, 160.0, random(126), random(126), 600); // DFT-30
	LastVehicleID = Vehicle_AddStatic(VehicleFlatbed, -30.0, -1128.0, 1.25, 160.0, random(126), random(126), 600); // Flatbed
	LastVehicleID = Vehicle_AddStatic(VehicleFlatbed, -41.0, -1152.0, 1.25, 335.0, random(126), random(126), 600); // Flatbed
	LastVehicleID = Vehicle_AddStatic(VehicleFlatbed, -46.0, -1150.0, 1.25, 335.0, random(126), random(126), 600); // Flatbed
	LastVehicleID = Vehicle_AddStatic(VehicleFlatbed, -51.0, -1148.0, 1.25, 335.0, random(126), random(126), 600); // Flatbed
	LastVehicleID = Vehicle_AddStatic(VehicleFlatbed, -56.0, -1146.0, 1.25, 335.0, random(126), random(126), 600); // Flatbed
	LastVehicleID = Vehicle_AddStatic(VehicleFlatbed, -61.0, -1144.0, 1.25, 335.0, random(126), random(126), 600); // Flatbed

	// Add trailers, trucks , Flatbeds and "DFT-30" to LVA Freight Depot (Class 2)
	LastVehicleID = Vehicle_AddStatic(VehicleDFT30, 1467.0, 975.0, 11.0, 0.0, random(126), random(126), 600); // DFT-30
	LastVehicleID = Vehicle_AddStatic(VehicleFlatbed, 1472.0, 975.0, 11.0, 0.0, random(126), random(126), 600); // Flatbed
	LastVehicleID = Vehicle_AddStatic(VehicleDFT30, 1445.0, 975.0, 11.0, 0.0, random(126), random(126), 600); // DFT-30
	LastVehicleID = Vehicle_AddStatic(VehicleFlatbed, 1440.0, 975.0, 11.0, 0.0, random(126), random(126), 600); // Flatbed
	LastVehicleID = Vehicle_AddStatic(VehicleDFT30, 1435.0, 975.0, 11.0, 0.0, random(126), random(126), 600); // DFT-30
	LastVehicleID = Vehicle_AddStatic(VehicleFlatbed, 1430.0, 975.0, 11.0, 0.0, random(126), random(126), 600); // Flatbed
	LastVehicleID = Vehicle_AddStatic(VehicleDFT30, 1420.0, 975.0, 11.0, 0.0, random(126), random(126), 600); // DFT-30
	LastVehicleID = Vehicle_AddStatic(VehicleFlatbed, 1415.0, 975.0, 11.0, 0.0, random(126), random(126), 600); // Flatbed
	LastVehicleID = Vehicle_AddStatic(VehicleDFT30, 1410.0, 975.0, 11.0, 0.0, random(126), random(126), 600); // DFT-30
	LastVehicleID = Vehicle_AddStatic(VehicleFlatbed, 1405.0, 975.0, 11.0, 0.0, random(126), random(126), 600); // Flatbed
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerCargo, 1490.0, 1015.0, 11.0, 90.0, random(126), random(126), 600); // Cargo trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerOre, 1490.0, 1020.0, 11.0, 90.0, random(126), random(126), 600); // Ore trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerFluids, 1490.0, 1025.0, 11.0, 90.0, random(126), random(126), 600); // Liquids trailer
	LastVehicleID = Vehicle_AddStatic(VehicleLineRunner, 1490.0, 1030.0, 11.0, 90.0, random(126), random(126), 600); // LineRunner
	LastVehicleID = Vehicle_AddStatic(VehicleTanker, 1490.0, 1035.0, 11.0, 90.0, random(126), random(126), 600); // Tanker
	LastVehicleID = Vehicle_AddStatic(VehicleRoadTrain, 1490.0, 1040.0, 11.0, 90.0, random(126), random(126), 600); // Roadtrain
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerCargo, 1490.0, 1045.0, 11.0, 90.0, random(126), random(126), 600); // Cargo trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerOre, 1490.0, 1050.0, 11.0, 90.0, random(126), random(126), 600); // Ore trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerFluids, 1490.0, 1055.0, 11.0, 90.0, random(126), random(126), 600); // Liquids trailer
	LastVehicleID = Vehicle_AddStatic(VehicleLineRunner, 1490.0, 1060.0, 11.0, 90.0, random(126), random(126), 600); // LineRunner
	LastVehicleID = Vehicle_AddStatic(VehicleTanker, 1490.0, 1065.0, 11.0, 90.0, random(126), random(126), 600); // Tanker
	LastVehicleID = Vehicle_AddStatic(VehicleRoadTrain, 1490.0, 1070.0, 11.0, 90.0, random(126), random(126), 600); // Roadtrain

	// Add trailers, trucks , Flatbeds and "DFT-30" to Doherty Depot (Class 3)
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerCargo, -2105.0, -200.0, 35.5, 90.0, random(126), random(126), 600); // Cargo trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerOre, -2105.0, -205.0, 35.5, 90.0, random(126), random(126), 600); // Ore trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerFluids, -2105.0, -210.0, 35.5, 90.0, random(126), random(126), 600); // Liquids trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerCargo, -2105.0, -215.0, 35.5, 90.0, random(126), random(126), 600); // Cargo trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerOre, -2105.0, -220.0, 35.5, 90.0, random(126), random(126), 600); // Ore trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerFluids, -2105.0, -225.0, 35.5, 90.0, random(126), random(126), 600); // Liquids trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerCargo, -2105.0, -230.0, 35.5, 90.0, random(126), random(126), 600); // Cargo trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerOre, -2105.0, -235.0, 35.5, 90.0, random(126), random(126), 600); // Ore trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerFluids, -2105.0, -240.0, 35.5, 90.0, random(126), random(126), 600); // Liquids trailer
	LastVehicleID = Vehicle_AddStatic(VehicleDFT30, -2105.0, -245.0, 35.5, 90.0, random(126), random(126), 600); // DFT-30
	LastVehicleID = Vehicle_AddStatic(VehicleFlatbed, -2105.0, -250.0, 35.5, 90.0, random(126), random(126), 600); // Flatbed
	LastVehicleID = Vehicle_AddStatic(VehicleDFT30, -2105.0, -255.0, 35.5, 90.0, random(126), random(126), 600); // DFT-30
	LastVehicleID = Vehicle_AddStatic(VehicleFlatbed, -2105.0, -260.0, 35.5, 90.0, random(126), random(126), 600); // Flatbed
	LastVehicleID = Vehicle_AddStatic(VehicleDFT30, -2105.0, -265.0, 35.5, 90.0, random(126), random(126), 600); // DFT-30
	LastVehicleID = Vehicle_AddStatic(VehicleFlatbed, -2105.0, -270.0, 35.5, 90.0, random(126), random(126), 600); // Flatbed
	LastVehicleID = Vehicle_AddStatic(VehicleLineRunner, -2144.0, -186.0, 35.5, 270.0, random(126), random(126), 600); // LineRunner
	LastVehicleID = Vehicle_AddStatic(VehicleTanker, -2144.0, -191.0, 35.5, 270.0, random(126), random(126), 600); // Tanker
	LastVehicleID = Vehicle_AddStatic(VehicleRoadTrain, -2144.0, -196.0, 35.5, 270.0, random(126), random(126), 600); // Roadtrain
	LastVehicleID = Vehicle_AddStatic(VehicleLineRunner, -2144.0, -201.0, 35.5, 270.0, random(126), random(126), 600); // LineRunner
	LastVehicleID = Vehicle_AddStatic(VehicleTanker, -2144.0, -206.0, 35.5, 270.0, random(126), random(126), 600); // Tanker
	LastVehicleID = Vehicle_AddStatic(VehicleRoadTrain, -2144.0, -211.0, 35.5, 270.0, random(126), random(126), 600); // Roadtrain

	// Add trailers, trucks , Flatbeds and "DFT-30" to El Corona Depot (Class 4)
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerCargo, 1805.0, -2025.0, 14.0, 90.0, random(126), random(126), 600); // Cargo trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerOre, 1805.0, -2030.0, 14.0, 90.0, random(126), random(126), 600); // Ore trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerFluids, 1805.0, -2035.0, 14.0, 90.0, random(126), random(126), 600); // Liquids trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerCargo, 1805.0, -2040.0, 14.0, 90.0, random(126), random(126), 600); // Cargo trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerOre, 1805.0, -2045.0, 14.0, 90.0, random(126), random(126), 600); // Ore trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerFluids, 1805.0, -2050.0, 14.0, 90.0, random(126), random(126), 600); // Liquids trailer
	LastVehicleID = Vehicle_AddStatic(VehicleDFT30, 1745.0, -2070.0, 14.0, 0.0, random(126), random(126), 600); // DFT-30
	LastVehicleID = Vehicle_AddStatic(VehicleFlatbed, 1750.0, -2070.0, 14.0, 0.0, random(126), random(126), 600); // Flatbed
	LastVehicleID = Vehicle_AddStatic(VehicleDFT30, 1755.0, -2070.0, 14.0, 0.0, random(126), random(126), 600); // DFT-30
	LastVehicleID = Vehicle_AddStatic(VehicleFlatbed, 1760.0, -2070.0, 14.0, 0.0, random(126), random(126), 600); // Flatbed
	LastVehicleID = Vehicle_AddStatic(VehicleLineRunner, 1765.0, -2070.0, 14.0, 0.0, random(126), random(126), 600); // LineRunner
	LastVehicleID = Vehicle_AddStatic(VehicleTanker, 1770.0, -2070.0, 14.0, 0.0, random(126), random(126), 600); // Tanker
	LastVehicleID = Vehicle_AddStatic(VehicleRoadTrain, 1775.0, -2070.0, 14.0, 0.0, random(126), random(126), 600); // Roadtrain

	// Add trailers, trucks , Flatbeds and "DFT-30" to Las Payasdas Depot (Class 5)
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerCargo, -535.0, 2635.0, 54.0, 270.0, random(126), random(126), 600); // Cargo trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerOre, -535.0, 2630.0, 54.0, 270.0, random(126), random(126), 600); // Ore trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerFluids, -535.0, 2625.0, 54.0, 270.0, random(126), random(126), 600); // Liquids trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerCargo, -535.0, 2620.0, 54.0, 270.0, random(126), random(126), 600); // Cargo trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerOre, -535.0, 2615.0, 54.0, 270.0, random(126), random(126), 600); // Ore trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerFluids, -535.0, 2610.0, 54.0, 270.0, random(126), random(126), 600); // Liquids trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerCargo, -535.0, 2605.0, 54.0, 270.0, random(126), random(126), 600); // Cargo trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerOre, -535.0, 2600.0, 54.0, 270.0, random(126), random(126), 600); // Ore trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerFluids, -535.0, 2595.0, 54.0, 270.0, random(126), random(126), 600); // Liquids trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerCargo, -535.0, 2590.0, 54.0, 270.0, random(126), random(126), 600); // Cargo trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerOre, -535.0, 2585.0, 54.0, 270.0, random(126), random(126), 600); // Ore trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerFluids, -535.0, 2580.0, 54.0, 270.0, random(126), random(126), 600); // Liquids trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerCargo, -535.0, 2575.0, 54.0, 270.0, random(126), random(126), 600); // Cargo trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerOre, -535.0, 2570.0, 54.0, 270.0, random(126), random(126), 600); // Ore trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerFluids, -535.0, 2565.0, 54.0, 270.0, random(126), random(126), 600); // Liquids trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerCargo, -535.0, 2560.0, 54.0, 270.0, random(126), random(126), 600); // Cargo trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerOre, -535.0, 2555.0, 54.0, 270.0, random(126), random(126), 600); // Ore trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerFluids, -535.0, 2550.0, 54.0, 270.0, random(126), random(126), 600); // Liquids trailer

	LastVehicleID = Vehicle_AddStatic(VehicleLineRunner, -512.0, 2635.0, 54.0, 90.0, random(126), random(126), 600); // LineRunner
	LastVehicleID = Vehicle_AddStatic(VehicleTanker, -512.0, 2630.0, 54.0, 90.0, random(126), random(126), 600); // Tanker
	LastVehicleID = Vehicle_AddStatic(VehicleRoadTrain, -512.0, 2625.0, 54.0, 90.0, random(126), random(126), 600); // Roadtrain
	LastVehicleID = Vehicle_AddStatic(VehicleLineRunner, -512.0, 2620.0, 54.0, 90.0, random(126), random(126), 600); // LineRunner
	LastVehicleID = Vehicle_AddStatic(VehicleTanker, -512.0, 2615.0, 54.0, 90.0, random(126), random(126), 600); // Tanker
	LastVehicleID = Vehicle_AddStatic(VehicleRoadTrain, -512.0, 2610.0, 54.0, 90.0, random(126), random(126), 600); // Roadtrain
	LastVehicleID = Vehicle_AddStatic(VehicleDFT30, -512.0, 2605.0, 54.0, 90.0, random(126), random(126), 600); // DFT-30
	LastVehicleID = Vehicle_AddStatic(VehicleFlatbed, -512.0, 2600.0, 54.0, 90.0, random(126), random(126), 600); // Flatbed
	LastVehicleID = Vehicle_AddStatic(VehicleDFT30, -512.0, 2585.0, 54.0, 90.0, random(126), random(126), 600); // DFT-30
	LastVehicleID = Vehicle_AddStatic(VehicleFlatbed, -512.0, 2580.0, 54.0, 90.0, random(126), random(126), 600); // Flatbed
	LastVehicleID = Vehicle_AddStatic(VehicleLineRunner, -512.0, 2575.0, 54.0, 90.0, random(126), random(126), 600); // LineRunner
	LastVehicleID = Vehicle_AddStatic(VehicleTanker, -512.0, 2570.0, 54.0, 90.0, random(126), random(126), 600); // Tanker
	LastVehicleID = Vehicle_AddStatic(VehicleRoadTrain, -512.0, 2565.0, 54.0, 90.0, random(126), random(126), 600); // Roadtrain
	LastVehicleID = Vehicle_AddStatic(VehicleLineRunner, -512.0, 2560.0, 54.0, 90.0, random(126), random(126), 600); // LineRunner
	LastVehicleID = Vehicle_AddStatic(VehicleTanker, -512.0, 2555.0, 54.0, 90.0, random(126), random(126), 600); // Tanker
	LastVehicleID = Vehicle_AddStatic(VehicleRoadTrain, -512.0, 2550.0, 54.0, 90.0, random(126), random(126), 600); // Roadtrain

	// Add cement-trucks at Quarry Top (Class 6)
	LastVehicleID = Vehicle_AddStatic(VehicleCementTruck, 340.0, 850.0, 21.0, 0, random(126), random(126), 600); // Cement truck
	LastVehicleID = Vehicle_AddStatic(VehicleCementTruck, 335.0, 860.0, 21.0, 0, random(126), random(126), 600); // Cement truck
	LastVehicleID = Vehicle_AddStatic(VehicleCementTruck, 330.0, 870.0, 21.0, 0, random(126), random(126), 600); // Cement truck
	LastVehicleID = Vehicle_AddStatic(VehicleCementTruck, 325.0, 880.0, 21.0, 0, random(126), random(126), 600); // Cement truck
	LastVehicleID = Vehicle_AddStatic(VehicleCementTruck, 320.0, 890.0, 21.0, 0, random(126), random(126), 600); // Cement truck

	// Add trailers and trucks to Shady Creek Depot (Class 7)
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerCargo, -1545.0, -2737.00, 49.0, 145.0, random(126), random(126), 600); // Cargo trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerOre, -1540.9, -2739.87, 49.0, 145.0, random(126), random(126), 600); // Ore trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerFluids, -1536.8, -2742.74, 49.0, 145.0, random(126), random(126), 600); // Liquids trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerCargo, -1532.7, -2745.61, 49.0, 145.0, random(126), random(126), 600); // Cargo trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerOre, -1528.6, -2748.48, 49.0, 145.0, random(126), random(126), 600); // Ore trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTrailerFluids, -1524.5, -2751.35, 49.0, 145.0, random(126), random(126), 600); // Liquids trailer
	LastVehicleID = Vehicle_AddStatic(VehicleLineRunner, -1557.75, -2744.80, 49.0, 145.0, random(126), random(126), 600); // LineRunner
	LastVehicleID = Vehicle_AddStatic(VehicleTanker, -1561.85, -2741.93, 49.0, 145.0, random(126), random(126), 600); // Tanker
	LastVehicleID = Vehicle_AddStatic(VehicleRoadTrain, -1565.95, -2739.06, 49.0, 145.0, random(126), random(126), 600); // Roadtrain
	LastVehicleID = Vehicle_AddStatic(VehicleLineRunner, -1570.05, -2736.19, 49.0, 145.0, random(126), random(126), 600); // LineRunner
	LastVehicleID = Vehicle_AddStatic(VehicleTanker, -1574.15, -2733.32, 49.0, 145.0, random(126), random(126), 600); // Tanker
	LastVehicleID = Vehicle_AddStatic(VehicleRoadTrain, -1578.25, -2730.45, 49.0, 145.0, random(126), random(126), 600); // Roadtrain
}

// Add busses to the bus-depots
GameModeInit_Busses()
{
	// Add all required busses to the busdepot in Los Santos
	LastVehicleID = Vehicle_AddStatic(VehicleCoach, 1782.0, -1888.0, 13.6, 270.0, random(126), random(126), 600); // Coach
	LastVehicleID = Vehicle_AddStatic(VehicleCoach, 1782.0, -1893.0, 13.6, 270.0, random(126), random(126), 600); // Coach
	LastVehicleID = Vehicle_AddStatic(VehicleCoach, 1782.0, -1898.0, 13.6, 270.0, random(126), random(126), 600); // Coach
	LastVehicleID = Vehicle_AddStatic(VehicleCoach, 1782.0, -1903.0, 13.6, 270.0, random(126), random(126), 600); // Coach
	LastVehicleID = Vehicle_AddStatic(VehicleCoach, 1782.0, -1908.0, 13.6, 270.0, random(126), random(126), 600); // Coach
	LastVehicleID = Vehicle_AddStatic(VehicleCoach, 1782.0, -1913.0, 13.6, 270.0, random(126), random(126), 600); // Coach
	LastVehicleID = Vehicle_AddStatic(VehicleCoach, 1782.0, -1918.0, 13.6, 270.0, random(126), random(126), 600); // Coach

	// Add all required busses to the busdepot in San Fierro
	LastVehicleID = Vehicle_AddStatic(VehicleCoach, -1974.0, 105.0, 27.9, 90.0, random(126), random(126), 600); // Coach
	LastVehicleID = Vehicle_AddStatic(VehicleCoach, -1974.0, 100.0, 27.9, 90.0, random(126), random(126), 600); // Coach
	LastVehicleID = Vehicle_AddStatic(VehicleCoach, -1974.0, 95.0, 27.9, 90.0, random(126), random(126), 600); // Coach
	LastVehicleID = Vehicle_AddStatic(VehicleCoach, -1974.0, 90.0, 27.9, 90.0, random(126), random(126), 600); // Coach
	LastVehicleID = Vehicle_AddStatic(VehicleCoach, -1974.0, 85.0, 27.9, 90.0, random(126), random(126), 600); // Coach
	LastVehicleID = Vehicle_AddStatic(VehicleCoach, -1974.0, 80.0, 27.9, 90.0, random(126), random(126), 600); // Coach

	// Add all required busses to the busdepot in Las Venturas
	LastVehicleID = Vehicle_AddStatic(VehicleCoach, 1085.0, 1230.0, 11.0, 90.0, random(126), random(126), 600); // Coach
	LastVehicleID = Vehicle_AddStatic(VehicleCoach, 1085.0, 1235.0, 11.0, 90.0, random(126), random(126), 600); // Coach
	LastVehicleID = Vehicle_AddStatic(VehicleCoach, 1085.0, 1240.0, 11.0, 90.0, random(126), random(126), 600); // Coach
	LastVehicleID = Vehicle_AddStatic(VehicleCoach, 1085.0, 1245.0, 11.0, 90.0, random(126), random(126), 600); // Coach
	LastVehicleID = Vehicle_AddStatic(VehicleCoach, 1085.0, 1250.0, 11.0, 90.0, random(126), random(126), 600); // Coach
	LastVehicleID = Vehicle_AddStatic(VehicleCoach, 1085.0, 1255.0, 11.0, 90.0, random(126), random(126), 600); // Coach
	LastVehicleID = Vehicle_AddStatic(VehicleCoach, 1085.0, 1260.0, 11.0, 90.0, random(126), random(126), 600); // Coach
	LastVehicleID = Vehicle_AddStatic(VehicleCoach, 1085.0, 1265.0, 11.0, 90.0, random(126), random(126), 600); // Coach
	LastVehicleID = Vehicle_AddStatic(VehicleCoach, 1085.0, 1270.0, 11.0, 90.0, random(126), random(126), 600); // Coach
	LastVehicleID = Vehicle_AddStatic(VehicleCoach, 1085.0, 1275.0, 11.0, 90.0, random(126), random(126), 600); // Coach
	LastVehicleID = Vehicle_AddStatic(VehicleCoach, 1085.0, 1280.0, 11.0, 90.0, random(126), random(126), 600); // Coach
}

// Add busstops to the map
GameModeInit_Busstops()
{
	LastObjectID = CreateDynamicObject(1257, -2571.25, 2316.0, 5.2, 0.0, 0.0, 180.0);
	LastObjectID = CreateDynamicObject(1257, -2333.25, 2371.5, 6.0, 0.0, 0.0, 320.0);
	LastObjectID = CreateDynamicObject(1257, -1467.0, 2679.0, 56.0, 0.0, 0.0, 90.0);
	LastObjectID = CreateDynamicObject(1257, -1528.5, 2543.0, 56.0, 0.0, 0.0, 270.0);
	LastObjectID = CreateDynamicObject(1257, -800.5, 1562.0, 27.4, 0.0, 0.0, 0.0);
	LastObjectID = CreateDynamicObject(1257, -853.5, 1529.5, 22.4, 0.0, 0.0, 180.0);
	LastObjectID = CreateDynamicObject(1257, -269.0, 2620.5, 63.2, 0.0, 0.0, 270.0);
	LastObjectID = CreateDynamicObject(1257, -45.5, 1206.5, 19.6, 0.0, 0.0, 90.0);
	LastObjectID = CreateDynamicObject(1257, -100.5, 1190.75, 20.0, 0.0, 0.0, 270.0);
	LastObjectID = CreateDynamicObject(1257, -249.5, 1010.75, 20.0, 0.0, 0.0, 270.0);
	LastObjectID = CreateDynamicObject(1257, -2172.5, -2309.5, 30.9, 0.0, 0.0, 230.0);
	LastObjectID = CreateDynamicObject(1257, -2094.25, -2503.0, 30.9, 0.0, 0.0, 322.0);
	LastObjectID = CreateDynamicObject(1257, 2263.75, -35.75, 26.7, 0.0, 0.0, 270.0);
	LastObjectID = CreateDynamicObject(1257, 2456.25, 1.0, 26.5, 0.0, 0.0, 180.0);
	LastObjectID = CreateDynamicObject(1257, 1290.25, 333.5, 19.8, 0.0, 0.0, 65.0);
	LastObjectID = CreateDynamicObject(1257, 1284.75, 319.5, 19.8, 0.0, 0.0, 245.0);
	LastObjectID = CreateDynamicObject(1257, 736.0, -522.5, 16.6, 0.0, 0.0, 90.0);
	LastObjectID = CreateDynamicObject(1257, 736.0, -537.75, 16.6, 0.0, 0.0, 270.0);
//	LastObjectID = CreateObject(1257, 0000, 0000, 000, 0.0, 0.0, 000.0);
}



// Add police vehicles to the Police Impounds
GameModeInit_Police()
{
	// Add vehicles to the Los Santos Police Impound
	LastVehicleID = Vehicle_AddStatic(VehiclePoliceLSPD, 1555.0, -1710.75, 6.0, 0, 0, 1, 600); // Police Car LSPD
	LastVehicleID = Vehicle_AddStatic(VehicleHPV1000, 1560.0, -1710.75, 6.0, 0, 0, 1, 600); // Police MotorCycle
	LastVehicleID = Vehicle_AddStatic(VehiclePoliceRanger, 1565.0, -1710.75, 6.0, 0, 0, 1, 600); // Police Ranger
	LastVehicleID = Vehicle_AddStatic(VehiclePoliceLSPD, 1570.0, -1710.75, 6.0, 0, 0, 1, 600); // Police Car LSPD
	LastVehicleID = Vehicle_AddStatic(VehicleHPV1000, 1575.0, -1710.75, 6.0, 0, 0, 1, 600); // Police MotorCycle
	LastVehicleID = Vehicle_AddStatic(VehiclePoliceRanger, 1580.0, -1710.75, 6.0, 0, 0, 1, 600); // Police Ranger
	LastVehicleID = Vehicle_AddStatic(VehiclePoliceLSPD, 1585.0, -1710.75, 6.0, 0, 0, 1, 600); // Police Car LSPD
	LastVehicleID = Vehicle_AddStatic(VehicleHPV1000, 1590.0, -1710.75, 6.0, 0, 0, 1, 600); // Police MotorCycle
	LastVehicleID = Vehicle_AddStatic(VehiclePoliceRanger, 1595.0, -1710.75, 6.0, 0, 0, 1, 600); // Police Ranger

	// Add vehicles to the San Fierro Police Impound
	LastVehicleID = Vehicle_AddStatic(VehiclePoliceSFPD, -1573.0, 701.0, -5.0, 90, 0, 1, 600); // Police Car SFPD
	LastVehicleID = Vehicle_AddStatic(VehicleHPV1000, -1573.0, 706.0, -5.0, 90, 0, 1, 600); // Police MotorCycle
	LastVehicleID = Vehicle_AddStatic(VehiclePoliceRanger, -1573.0, 711.0, -5.0, 90, 0, 1, 600); // Police Ranger
	LastVehicleID = Vehicle_AddStatic(VehiclePoliceSFPD, -1573.0, 716.0, -5.0, 90, 0, 1, 600); // Police Car SFPD
	LastVehicleID = Vehicle_AddStatic(VehicleHPV1000, -1573.0, 721.0, -5.0, 90, 0, 1, 600); // Police MotorCycle
	LastVehicleID = Vehicle_AddStatic(VehiclePoliceRanger, -1573.0, 726.0, -5.0, 90, 0, 1, 600); // Police Ranger
	LastVehicleID = Vehicle_AddStatic(VehiclePoliceSFPD, -1573.0, 731.0, -5.0, 90, 0, 1, 600); // Police Car SFPD
	LastVehicleID = Vehicle_AddStatic(VehicleHPV1000, -1573.0, 736.0, -5.0, 90, 0, 1, 600); // Police MotorCycle
	LastVehicleID = Vehicle_AddStatic(VehiclePoliceRanger, -1573.0, 741.0, -5.0, 90, 0, 1, 600); // Police Ranger

	// Add vehicles to the Las Venturas Police Impound
	LastVehicleID = Vehicle_AddStatic(VehiclePoliceLVPD, 2282.0, 2477.0, 11.0, 180, 0, 1, 600); // Police Car LVPD
	LastVehicleID = Vehicle_AddStatic(VehicleHPV1000, 2277.0, 2477.0, 11.0, 180, 0, 1, 600); // Police MotorCycle
	LastVehicleID = Vehicle_AddStatic(VehiclePoliceRanger, 2272.0, 2477.0, 11.0, 180, 0, 1, 600); // Police Ranger
	LastVehicleID = Vehicle_AddStatic(VehiclePoliceLVPD, 2262.0, 2477.0, 11.0, 180, 0, 1, 600); // Police Car LVPD
	LastVehicleID = Vehicle_AddStatic(VehicleHPV1000, 2257.0, 2477.0, 11.0, 180, 0, 1, 600); // Police MotorCycle
	LastVehicleID = Vehicle_AddStatic(VehiclePoliceRanger, 2252.0, 2477.0, 11.0, 180, 0, 1, 600); // Police Ranger

	LastVehicleID = Vehicle_AddStatic(VehiclePoliceLVPD, 2282.0, 2443.0, 11.0, 0, 0, 1, 600); // Police Car LVPD
	LastVehicleID = Vehicle_AddStatic(VehicleHPV1000, 2277.0, 2443.0, 11.0, 0, 0, 1, 600); // Police MotorCycle
	LastVehicleID = Vehicle_AddStatic(VehiclePoliceRanger, 2272.0, 2443.0, 11.0, 0, 0, 1, 600); // Police Ranger
	LastVehicleID = Vehicle_AddStatic(VehiclePoliceLVPD, 2262.0, 2443.0, 11.0, 0, 0, 1, 600); // Police Car LVPD
	LastVehicleID = Vehicle_AddStatic(VehicleHPV1000, 2257.0, 2443.0, 11.0, 0, 0, 1, 600); // Police MotorCycle
	LastVehicleID = Vehicle_AddStatic(VehiclePoliceRanger, 2252.0, 2443.0, 11.0, 0, 0, 1, 600); // Police Ranger
}

// Add mafia-vehicles to the mafia-hideout
GameModeInit_Mafia()
{
	LastVehicleID = Vehicle_AddStatic(VehicleSandKing, 2811.0, 900.0, 10.8, 0.0, 0, 0, 600); // Mafia Sandking
	LastVehicleID = Vehicle_AddStatic(VehicleSandKing, 2806.0, 900.0, 10.8, 0.0, 0, 0, 600); // Mafia Sandking
	LastVehicleID = Vehicle_AddStatic(VehicleSandKing, 2801.0, 900.0, 10.8, 0.0, 0, 0, 600); // Mafia Sandking
	LastVehicleID = Vehicle_AddStatic(VehicleSandKing, 2796.0, 900.0, 10.8, 0.0, 0, 0, 600); // Mafia Sandking

	LastVehicleID = Vehicle_AddStatic(VehicleMoonbeam, 2833.0, 900.0, 10.8, 0.0, 0, 0, 600); // Mafia Moonbeam
	LastVehicleID = Vehicle_AddStatic(VehicleMoonbeam, 2838.0, 900.0, 10.8, 0.0, 0, 0, 600); // Mafia Moonbeam
	LastVehicleID = Vehicle_AddStatic(VehicleMoonbeam, 2843.0, 900.0, 10.8, 0.0, 0, 0, 600); // Mafia Moonbeam
	LastVehicleID = Vehicle_AddStatic(VehicleMoonbeam, 2848.0, 900.0, 10.8, 0.0, 0, 0, 600); // Mafia Moonbeam

	LastVehicleID = Vehicle_AddStatic(VehicleRoadTrain, 2802, 966.0, 10.8, 180.0, 0, 0, 600); // Mafia RoadTrain
	LastVehicleID = Vehicle_AddStatic(VehicleRoadTrain, 2807, 966.0, 10.8, 180.0, 0, 0, 600); // Mafia RoadTrain
	LastVehicleID = Vehicle_AddStatic(VehicleRoadTrain, 2812, 966.0, 10.8, 180.0, 0, 0, 600); // Mafia RoadTrain
	LastVehicleID = Vehicle_AddStatic(VehicleRoadTrain, 2817, 966.0, 10.8, 180.0, 0, 0, 600); // Mafia RoadTrain
	LastVehicleID = Vehicle_AddStatic(VehicleRoadTrain, 2822, 966.0, 10.8, 180.0, 0, 0, 600); // Mafia RoadTrain
	LastVehicleID = Vehicle_AddStatic(VehicleRoadTrain, 2827, 966.0, 10.8, 180.0, 0, 0, 600); // Mafia RoadTrain
}

// Add planes and helicopters to the airports
GameModeInit_Airports()
{
	// Add planes and helicopters to Los Santos airport
	LastVehicleID = Vehicle_AddStatic(VehicleShamal, 1990.0, -2295.0, 14.5, 90.0, random(126), random(126), 300); // Plane: Shamal
	LastVehicleID = Vehicle_AddStatic(VehicleShamal, 1990.0, -2320.0, 14.5, 90.0, random(126), random(126), 300); // Plane: Shamal
	LastVehicleID = Vehicle_AddStatic(VehicleShamal, 1990.0, -2345.0, 14.5, 90.0, random(126), random(126), 300); // Plane: Shamal
	LastVehicleID = Vehicle_AddStatic(VehicleShamal, 1990.0, -2370.0, 14.5, 90.0, random(126), random(126), 300); // Plane: Shamal
	LastVehicleID = Vehicle_AddStatic(VehicleNevada, 1920.0, -2265.0, 14.5, 180.0, random(126), random(126), 300); // Plane: Nevada
	LastVehicleID = Vehicle_AddStatic(VehicleNevada, 1890.0, -2295.0, 14.5, 270.0, random(126), random(126), 300); // Plane: Nevada
	LastVehicleID = Vehicle_AddStatic(VehicleMaverick, 1940.0, -2265.0, 13.6, 180.0, random(126), random(126), 300); // Helicopter: Maverick
	LastVehicleID = Vehicle_AddStatic(VehicleMaverick, 1955.0, -2265.0, 13.6, 180.0, random(126), random(126), 300); // Helicopter: Maverick
	LastVehicleID = Vehicle_AddStatic(VehicleMaverick, 1970.0, -2265.0, 13.6, 180.0, random(126), random(126), 300); // Helicopter: Maverick
	LastVehicleID = Vehicle_AddStatic(VehicleMaverick, 1985.0, -2265.0, 13.6, 180.0, random(126), random(126), 300); // Helicopter: Maverick

	// Add planes and helicopters to San Fierro airport
	LastVehicleID = Vehicle_AddStatic(VehicleShamal, -1197.0, -153.0, 15.1, 135.0, random(126), random(126), 300); // Plane: Shamal
	LastVehicleID = Vehicle_AddStatic(VehicleShamal, -1214.0, -137.5, 15.1, 135.0, random(126), random(126), 300); // Plane: Shamal
	LastVehicleID = Vehicle_AddStatic(VehicleShamal, -1230.0, -120.0, 15.1, 135.0, random(126), random(126), 300); // Plane: Shamal
	LastVehicleID = Vehicle_AddStatic(VehicleShamal, -1246.0, -103.5, 15.1, 135.0, random(126), random(126), 300); // Plane: Shamal
	LastVehicleID = Vehicle_AddStatic(VehicleNevada, -1287.0, -52.0, 15.1, 135.0, random(126), random(126), 300); // Plane: Nevada
	LastVehicleID = Vehicle_AddStatic(VehicleNevada, -1312.0, -27.0, 15.1, 135.0, random(126), random(126), 300); // Plane: Nevada
	LastVehicleID = Vehicle_AddStatic(VehicleMaverick, -1262.0, -88.5, 14.2, 135.0, random(126), random(126), 300); // Helicopter: Maverick
	LastVehicleID = Vehicle_AddStatic(VehicleMaverick, -1277.0, -74.5, 14.2, 90.0, random(126), random(126), 300); // Helicopter: Maverick
	LastVehicleID = Vehicle_AddStatic(VehicleMaverick, -1264.0, -61.5, 14.2, 45.0, random(126), random(126), 300); // Helicopter: Maverick

	// Add planes and helicopters to Las Venturas airport
	LastVehicleID = Vehicle_AddStatic(VehicleShamal, 1615.0, 1630.0, 11.8, 180.0, random(126), random(126), 300); // Plane: Shamal
	LastVehicleID = Vehicle_AddStatic(VehicleShamal, 1590.0, 1630.0, 11.8, 180.0, random(126), random(126), 300); // Plane: Shamal
	LastVehicleID = Vehicle_AddStatic(VehicleShamal, 1565.0, 1630.0, 11.8, 180.0, random(126), random(126), 300); // Plane: Shamal
	LastVehicleID = Vehicle_AddStatic(VehicleNevada, 1545.0, 1640.0, 11.8, 90.0, random(126), random(126), 300); // Plane: Nevada
	LastVehicleID = Vehicle_AddStatic(VehicleNevada, 1545.0, 1675.0, 11.8, 90.0, random(126), random(126), 300); // Plane: Nevada
	LastVehicleID = Vehicle_AddStatic(VehicleNevada, 1545.0, 1710.0, 11.8, 90.0, random(126), random(126), 300); // Plane: Nevada
	LastVehicleID = Vehicle_AddStatic(VehicleMaverick, 1650.0, 1557, 10.9, 90.0, random(126), random(126), 300); // Helicopter: Maverick
	LastVehicleID = Vehicle_AddStatic(VehicleMaverick, 1650.0, 1542, 10.9, 90.0, random(126), random(126), 300); // Helicopter: Maverick
	LastVehicleID = Vehicle_AddStatic(VehicleMaverick, 1635.0, 1534, 10.9, 0.0, random(126), random(126), 300); // Helicopter: Maverick
}

// This function adds towtrucks used for the assistance class
GameModeInit_Assistance()
{
	LastVehicleID = Vehicle_AddStatic(VehicleTowTruck, 220.0, 34.0, 2.6, 270.0, random(126), random(126), 300); // Towtruck
	LastVehicleID = Vehicle_AddStatic(VehicleTowTruck, 220.0, 29.0, 2.6, 270.0, random(126), random(126), 300); // Towtruck
	LastVehicleID = Vehicle_AddStatic(VehicleTowTruck, 220.0, 24.0, 2.6, 270.0, random(126), random(126), 300); // Towtruck
	LastVehicleID = Vehicle_AddStatic(VehicleTowTruck, 220.0, 19.0, 2.6, 270.0, random(126), random(126), 300); // Towtruck
	LastVehicleID = Vehicle_AddStatic(VehicleTowTruck, 220.0, 14.0, 2.6, 270.0, random(126), random(126), 300); // Towtruck
	LastVehicleID = Vehicle_AddStatic(VehicleTowTruck, 220.0, 9.0, 2.6, 270.0, random(126), random(126), 300); // Towtruck
	LastVehicleID = Vehicle_AddStatic(VehicleTowTruck, 220.0, 4.0, 2.6, 270.0, random(126), random(126), 300); // Towtruck
	LastVehicleID = Vehicle_AddStatic(VehicleTowTruck, 220.0, -1.0, 2.6, 270.0, random(126), random(126), 300); // Towtruck
}

// This function adds all vehicles needed for courier class
GameModeInit_Courier()
{
	// Los Santos
	LastVehicleID = Vehicle_AddStatic(VehicleBurrito, 784.0, -610.0, 16.4, 0.0, random(126), random(126), 300); // Burrito
	LastVehicleID = Vehicle_AddStatic(VehicleBurrito, 794.0, -610.0, 16.4, 0.0, random(126), random(126), 300); // Burrito
	LastVehicleID = Vehicle_AddStatic(VehicleBurrito, 804.0, -610.0, 16.4, 0.0, random(126), random(126), 300); // Burrito
	LastVehicleID = Vehicle_AddStatic(VehicleBurrito, 814.0, -610.0, 16.4, 0.0, random(126), random(126), 300); // Burrito
	LastVehicleID = Vehicle_AddStatic(VehicleBurrito, 824.0, -610.0, 16.4, 0.0, random(126), random(126), 300); // Burrito
	LastVehicleID = Vehicle_AddStatic(VehicleFaggio, 789.0, -610.0, 16.4, 0.0, random(126), random(126), 300); // Faggio
	LastVehicleID = Vehicle_AddStatic(VehicleFaggio, 799.0, -610.0, 16.4, 0.0, random(126), random(126), 300); // Faggio
	LastVehicleID = Vehicle_AddStatic(VehicleFaggio, 809.0, -610.0, 16.4, 0.0, random(126), random(126), 300); // Faggio
	LastVehicleID = Vehicle_AddStatic(VehicleFaggio, 819.0, -610.0, 16.4, 0.0, random(126), random(126), 300); // Faggio
	LastVehicleID = Vehicle_AddStatic(VehicleFaggio, 829.0, -610.0, 16.4, 0.0, random(126), random(126), 300); // Faggio
	// San Fierro
	LastVehicleID = Vehicle_AddStatic(VehicleBurrito, -1851.0, -142.5, 12.0, 0.0, random(126), random(126), 300); // Burrito
	LastVehicleID = Vehicle_AddStatic(VehicleBurrito, -1856.0, -142.5, 12.0, 0.0, random(126), random(126), 300); // Burrito
	LastVehicleID = Vehicle_AddStatic(VehicleBurrito, -1861.0, -142.5, 12.0, 0.0, random(126), random(126), 300); // Burrito
	LastVehicleID = Vehicle_AddStatic(VehicleBurrito, -1866.0, -142.5, 12.0, 0.0, random(126), random(126), 300); // Burrito
	LastVehicleID = Vehicle_AddStatic(VehicleBurrito, -1871.0, -142.5, 12.0, 0.0, random(126), random(126), 300); // Burrito
	LastVehicleID = Vehicle_AddStatic(VehicleFaggio, -1851.0, -129.5, 12.0, 180.0, random(126), random(126), 300); // Faggio
	LastVehicleID = Vehicle_AddStatic(VehicleFaggio, -1856.0, -129.5, 12.0, 180.0, random(126), random(126), 300); // Faggio
	LastVehicleID = Vehicle_AddStatic(VehicleFaggio, -1861.0, -129.5, 12.0, 180.0, random(126), random(126), 300); // Faggio
	LastVehicleID = Vehicle_AddStatic(VehicleFaggio, -1866.0, -129.5, 12.0, 180.0, random(126), random(126), 300); // Faggio
	// Las Venturas
	LastVehicleID = Vehicle_AddStatic(VehicleBurrito, 1052.5, 1915.25, 10.9, 0.0, random(126), random(126), 300); // Burrito
	LastVehicleID = Vehicle_AddStatic(VehicleBurrito, 1062.5, 1915.25, 10.9, 0.0, random(126), random(126), 300); // Burrito
	LastVehicleID = Vehicle_AddStatic(VehicleBurrito, 1072.5, 1915.25, 10.9, 0.0, random(126), random(126), 300); // Burrito
	LastVehicleID = Vehicle_AddStatic(VehicleBurrito, 1082.5, 1915.25, 10.9, 0.0, random(126), random(126), 300); // Burrito
	LastVehicleID = Vehicle_AddStatic(VehicleBurrito, 1092.5, 1915.25, 10.9, 0.0, random(126), random(126), 300); // Burrito
	LastVehicleID = Vehicle_AddStatic(VehicleFaggio, 1057.5, 1915.25, 10.9, 0.0, random(126), random(126), 300); // Faggio
	LastVehicleID = Vehicle_AddStatic(VehicleFaggio, 1067.5, 1915.25, 10.9, 0.0, random(126), random(126), 300); // Faggio
	LastVehicleID = Vehicle_AddStatic(VehicleFaggio, 1077.5, 1915.25, 10.9, 0.0, random(126), random(126), 300); // Faggio
	LastVehicleID = Vehicle_AddStatic(VehicleFaggio, 1087.5, 1915.25, 10.9, 0.0, random(126), random(126), 300); // Faggio
}

// This function adds all vehicles needed for roadworker class
GameModeInit_RoadWorker()
{
	// Quarry top
	LastVehicleID = Vehicle_AddStatic(VehicleUtilityVan, -1895.0, -1705.0, 21.5, 180.0, random(126), random(126), 300); // Utility Van
	LastVehicleID = Vehicle_AddStatic(VehicleUtilityVan, -1900.0, -1705.0, 21.5, 180.0, random(126), random(126), 300); // Utility Van
	LastVehicleID = Vehicle_AddStatic(VehicleUtilityVan, -1905.0, -1705.0, 21.5, 180.0, random(126), random(126), 300); // Utility Van
	LastVehicleID = Vehicle_AddStatic(VehicleUtilityVan, -1910.0, -1705.0, 21.5, 180.0, random(126), random(126), 300); // Utility Van
	LastVehicleID = Vehicle_AddStatic(VehicleUtilityVan, -1915.0, -1705.0, 21.5, 180.0, random(126), random(126), 300); // Utility Van
	LastVehicleID = Vehicle_AddStatic(VehicleUtilityVan, -1920.0, -1705.0, 21.5, 180.0, random(126), random(126), 300); // Utility Van
	LastVehicleID = Vehicle_AddStatic(VehicleUtilityTrailer, -1895.0, -1700.0, 21.5, 180.0, random(126), random(126), 300); // Utility Trailer
	LastVehicleID = Vehicle_AddStatic(VehicleUtilityTrailer, -1900.0, -1700.0, 21.5, 180.0, random(126), random(126), 300); // Utility Trailer
	LastVehicleID = Vehicle_AddStatic(VehicleUtilityTrailer, -1905.0, -1700.0, 21.5, 180.0, random(126), random(126), 300); // Utility Trailer
	LastVehicleID = Vehicle_AddStatic(VehicleUtilityTrailer, -1910.0, -1700.0, 21.5, 180.0, random(126), random(126), 300); // Utility Trailer
	LastVehicleID = Vehicle_AddStatic(VehicleUtilityTrailer, -1915.0, -1700.0, 21.5, 180.0, random(126), random(126), 300); // Utility Trailer
	LastVehicleID = Vehicle_AddStatic(VehicleUtilityTrailer, -1920.0, -1700.0, 21.5, 180.0, random(126), random(126), 300); // Utility Trailer
	LastVehicleID = Vehicle_AddStatic(VehicleTowTruck, -1866.5, -1731.75, 21.7, 30.0, random(126), random(126), 300); // Towtruck
	LastVehicleID = Vehicle_AddStatic(VehicleTowTruck, -1870.5, -1734.25, 21.7, 30.0, random(126), random(126), 300); // Towtruck
	LastVehicleID = Vehicle_AddStatic(VehicleTowTruck, -1874.5, -1736.75, 21.7, 30.0, random(126), random(126), 300); // Towtruck
	LastVehicleID = Vehicle_AddStatic(VehicleTowTruck, -1878.5, -1739.25, 21.7, 30.0, random(126), random(126), 300); // Towtruck
	LastVehicleID = Vehicle_AddStatic(VehicleTowTruck, -1882.5, -1741.75, 21.7, 30.0, random(126), random(126), 300); // Towtruck
	LastVehicleID = Vehicle_AddStatic(VehicleTowTruck, -1886.5, -1744.25, 21.7, 30.0, random(126), random(126), 300); // Towtruck
}



// Add some special pickups to the map
GameModeInit_PickupsIcons()
{
	// Add a pickup so the player can get a trucker/busdriver license and position a 3DTextLabel above it
	Pickup_License = CreatePickup(1274, 2, -2033.0, -100.0, 35.2, 0);
	Create3DTextLabel(TXT_PickupText, COLOR_TEAL, -2033.0, -100.0, 36.0, 30.0, 0);

	// Add all refuel-pickups to the world (including their icon)
	AddRefuelPickup(-1471.5, 1863.75, 32.7);
	AddRefuelPickup(-1326.5, 2677.5, 50.1);
	AddRefuelPickup(611.5, 1694.5, 7.0);
	AddRefuelPickup(-2249.25, -2559.0, 32.0);
	AddRefuelPickup(-1606.5, -2714.0, 48.6);
	AddRefuelPickup(-93.5, -1175.0, 2.3);
	AddRefuelPickup(1377.5, 457.0, 19.9);
	AddRefuelPickup(651.5, -565.5, 16.4);
	AddRefuelPickup(-1675.75, 412.75, 7.2);
	AddRefuelPickup(-2405.50, 976.25, 45.3);
	AddRefuelPickup(-2023.25, 156.75, 28.9);
	AddRefuelPickup(-1131.75, -204.25, 14.2);
	AddRefuelPickup(66.50, 1220.50, 18.9);
	AddRefuelPickup(350.50, 2537.50, 16.8);
	AddRefuelPickup(2147.00, 2747.75, 10.9);
	AddRefuelPickup(2639.75, 1106.00, 10.9);
	AddRefuelPickup(2115.00, 920.00, 10.9);
	AddRefuelPickup(2202.00, 2475.00, 10.9);
	AddRefuelPickup(1596.50, 2199.75, 10.9);
	AddRefuelPickup(1584.25, 1448.25, 10.9);
	AddRefuelPickup(1004.25, -940.50, 42.2);
	AddRefuelPickup(1935.00, -1772.75, 13.4);

	// Add all pickups for the cardealers (including their icon)
	AddCarDealerPickup(-1972.5, 288.5, 35.2); // Wang cars in Doherty (San Fierro)

	// Add some icons to the map (modding garages), they will only appear when the player discovers them
	CreateDynamicMapIcon(1039, -1032, 32, 27, 0, 0, 0, -1, 150.0); // Transfender, Los Santos
	CreateDynamicMapIcon(-1936, 235, 34, 27, 0, 0, 0, -1, 150.0); // Transfender, San Fierro
	CreateDynamicMapIcon(2385, 1034, 11, 27, 0, 0, 0, -1, 150.0); // Transfender, Las Venturas
	CreateDynamicMapIcon(2646, -2025, 14, 27, 0, 0, 0, -1, 150.0); // Loco Low Co, Los Santos
	CreateDynamicMapIcon(-2712, 218, 4, 27, 0, 0, 0, -1, 150.0); // Wheel Arch Angels, San Fierro
}



// This function creates all objects required for the toll-gates
GameModeInit_Toll()
{
	// Add toll-houses to the world
	CreateObject(9623, 1787.00, 695.50, 17.40, -3.5, 0.0, 350.25); //object(toll_sfw) (1)

	// Add a toll-gate to the world
	AddTollGate(3578, 1801.00, 692.66, 10.00, 1801.00, 692.66, 15.00, 0.0, 0.0, 350.25, 30);
	AddTollGate(3578, 1791.33, 694.44, 10.00, 1791.33, 694.44, 15.00, 0.0, 0.0, 350.25, 30);
	AddTollGate(3578, 1781.66, 696.22, 10.00, 1781.66, 696.22, 15.00, 0.0, 0.0, 350.25, 30);
	AddTollGate(3578, 1772.00, 698.00, 10.00, 1772.00, 698.00, 15.00, 0.0, 0.0, 350.25, 30);

	AddTollGate(3578, -2668.25, 1274.50, 50.0, -2668.25, 1274.50, 55.0, 0.0, 0.0, 0.0, 30);
	AddTollGate(3578, -2677.75, 1274.50, 50.0, -2677.75, 1274.50, 55.0, 0.0, 0.0, 0.0, 30);
	AddTollGate(3578, -2686.25, 1274.50, 50.0, -2686.25, 1274.50, 55.0, 0.0, 0.0, 0.0, 30);
	AddTollGate(3578, -2694.75, 1274.50, 50.0, -2694.75, 1274.50, 55.0, 0.0, 0.0, 0.0, 30);
}

// Add character models to the class-selection and set their default spawn coordinates (without weapons)
GameModeInit_Classes()
{
	AddPlayerClass(133, 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0); // Class 0 = Truck driver
	AddPlayerClass(201, 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0); // Class 1 = Truck driver
	AddPlayerClass(202, 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0); // Class 2 = Truck driver
	AddPlayerClass(234, 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0); // Class 3 = Truck driver
	AddPlayerClass(258, 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0); // Class 4 = Truck driver
	AddPlayerClass(261, 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0); // Class 5 = Truck driver
	AddPlayerClass(206, 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0); // Class 6 = Truck driver
	AddPlayerClass(34, 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0); // Class 7 = Truck driver

	AddPlayerClass(255, 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0); // Class 8 = Bus driver
	AddPlayerClass(253, 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0); // Class 9 = Bus driver

	AddPlayerClass(61, 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0); // Class 10 = Pilot

	AddPlayerClass(280, 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0); // Class 11 = Police
	AddPlayerClass(282, 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0); // Class 12 = Police
	AddPlayerClass(283, 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0); // Class 13 = Police

	AddPlayerClass(111, 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0); // Class 14 = Mafia
	AddPlayerClass(112, 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0); // Class 15 = Mafia
	AddPlayerClass(113, 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0); // Class 16 = Mafia

	AddPlayerClass(250, 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0); // Class 17 = Courier
	AddPlayerClass(193, 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0); // Class 18 = Courier

	AddPlayerClass(50, 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0); // Class 19 = Assistance

	AddPlayerClass(16, 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0); // Class 20 = Roadworker
	AddPlayerClass(27, 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0); // Class 21 = Roadworker
	AddPlayerClass(260, 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0); // Class 22 = Roadworker
}



// This function is used to add refuelling pickups to the map
AddRefuelPickup(Float:x, Float:y, Float:z)
{
	// Add the pickup-id to the ARefuelPickups array
	for (new i; i < sizeof(ARefuelPickups); i++)
	{
		// If an empty array-index is found
		if (ARefuelPickups[i][PickupID] == 0)
		{
		    // Store the pickup-id in this empty slot
			ARefuelPickups[i][PickupID] = CreatePickup(1244, 1, x, y, z, 0); // Type 1, cannot be pickup up, exists all the time
			ARefuelPickups[i][pux] = x;
			ARefuelPickups[i][puy] = y;
			ARefuelPickups[i][puz] = z;

			// Add a 3DText message above the refuel-pickup
			Create3DTextLabel(TXT_PickupRefuel, COLOR_TEAL, x, y, z + 0.8, 30.0, 0);

			// Add an icon to the map for this refuel-spot
			CreateDynamicMapIcon(x, y, z, 56, 0, 0, 0, -1, 300.0);

			// Stop browsing through the array
			break;
		}
	}
}

// This function is used to add cardealer-pickups to the map
AddCarDealerPickup(Float:x, Float:y, Float:z)
{
	// Add the pickup-id to the ACarDealerPickups array
	for (new i; i < sizeof(ACarDealerPickups); i++)
	{
		// If an empty array-index is found
		if (ACarDealerPickups[i][PickupID] == 0)
		{
		    // Store the pickup-id in this empty slot
			ACarDealerPickups[i][PickupID] = CreatePickup(1274, 1, x, y, z, 0); // Type 1, cannot be pickup up, exists all the time
			ACarDealerPickups[i][pux] = x;
			ACarDealerPickups[i][puy] = y;
			ACarDealerPickups[i][puz] = z;

			// Add a 3DText message above the cardealer pickup
			Create3DTextLabel(TXT_PickupCarDealer, COLOR_TEAL, x, y, z + 0.8, 30.0, 0);

			// Add an icon to the map for this cardealer-spot
			CreateDynamicMapIcon(x, y, z, 55, 0, 0, 0, -1, 300.0);

			// Stop browsing through the array
			break;
		}
	}
}

// This function is used to add toll-gates to the map
AddTollGate(GateModel, Float:OX, Float:OY, Float:OZ, Float:CX, Float:CY, Float:CZ, Float:RX, Float:RY, Float:RZ, TollMoney)
{
	// Loop through all tollgates
	for (new TollGate; TollGate < MAX_TOLLGATES; TollGate++)
	{
		// Check if this is an empty entry
		if (ATollGates[TollGate][GateID] == 0)
		{
			// Create a new object for the toll-gate in it's closed status
			ATollGates[TollGate][GateID] = CreateObject(GateModel, CX, CY, CZ, RX, RY, RZ);
			// Set data
			ATollGates[TollGate][TollPrice] = TollMoney; // Set the price to pay for passing the toll-gate
			ATollGates[TollGate][GateStatus] = 0; // Set the status to CLOSED
			ATollGates[TollGate][OpenX] = OX; // Save the OpenX coordinates
			ATollGates[TollGate][OpenY] = OY; // Save the OpenY coordinates
			ATollGates[TollGate][OpenZ] = OZ; // Save the OpenZ coordinates
			ATollGates[TollGate][CloseX] = CX; // Save the CloseX coordinates
			ATollGates[TollGate][CloseY] = CY; // Save the CloseY coordinates
			ATollGates[TollGate][CloseZ] = CZ; // Save the CloseZ coordinates
			break; // Stop the for-loop
		}
	}
}

