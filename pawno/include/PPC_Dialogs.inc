// Process the Register-dialog
Dialog_Register(playerid, response, inputtext[])
{
	new file[100], Name[MAX_PLAYER_NAME]; // Setup local variables
	GetPlayerName(playerid, Name, sizeof(Name)); // Get the playername
	format(file, sizeof(file), PlayerFile, Name); // Construct the complete filename for this player's account

	switch (response) // Check which button was clicked
	{
		case 1: // Player clicked "Register"
		{
			// Check if the player entered a password
			if(strlen(inputtext) <= 0)
				return ShowPlayerDialog(playerid, DialogRegister, DIALOG_STYLE_PASSWORD, TXT_DialogRegisterTitle, TXT_DialogRegisterMsg, TXT_DialogRegisterButton1, TXT_DialogButtonCancel);

			// Store the password
			format(APlayerData[playerid][PlayerPassword], 50, "%s", inputtext);

			// Create the file and save default data to it, then reload it (so all data is put into the correct place)
			PlayerFile_Create(playerid);
			PlayerFile_Load(playerid);

			// Send a message to the client to inform him that his account has been registered
			SendClientMessage(playerid, COLOR_WHITE, TXT_AccountRegistered);
			APlayerData[playerid][LoggedIn] = true; // The player has logged in properly
		}
	    case 0: // Player clicked "Cancel"
		{
			// Show a message that the player must be registered to play on this server
			SendClientMessage(playerid, COLOR_WHITE, TXT_PlayerMustRegister);
		    // Kick the player
			SetTimerEx("TimedKick", (750 + GetPlayerPing(playerid)), false, "i", playerid);
		}
	}

	return 1;
}



// Process the login-dialog
Dialog_Login(playerid, response, inputtext[])
{
	new Msg[128];

	switch (response) // Check which button was clicked
	{
	    case 1: // Player clicked "Login"
		{
			// Player must provide a password
			if (strlen(inputtext) <= 0)
				return ShowPlayerDialog(playerid, DialogLogin, DIALOG_STYLE_PASSWORD, TXT_DialogLoginTitle, TXT_DialogLoginMsg, TXT_DialogLoginButton1, TXT_DialogButtonCancel);

			// Check if the entered password is the same as the password saved in the player's account
		    if (strcmp(APlayerData[playerid][PlayerPassword], inputtext, false) == 0)
			{
				APlayerData[playerid][LoggedIn] = true; // The player has logged in properly
                BankFile_Load(playerid); // Load your bank account (if it exists, the player will be notified about this)
				SendClientMessage(playerid, COLOR_WHITE, TXT_LoggedIn); // Send a message to the client to inform him that he logged in properly
			}
			else
			{
				SetPVarInt(playerid, "LoginAttempt", GetPVarInt(playerid, "LoginAttempt") + 1);

				if (GetPVarInt(playerid, "LoginAttempt") == MAX_LOGIN_ATTEMPTS) {
					SendClientMessage(playerid, COLOR_WHITE, TXT_FailedLoginProperly);
					SetTimerEx("TimedKick", (750 + GetPlayerPing(playerid)), false, "i", playerid);
				} else {
					format(Msg, sizeof(Msg), TXT_WrongPassword, (MAX_LOGIN_ATTEMPTS-GetPVarInt(playerid, "LoginAttempt")));
					SendClientMessage(playerid, COLOR_WHITE, Msg);

					ShowPlayerDialog(playerid, DialogLogin, DIALOG_STYLE_PASSWORD, TXT_DialogLoginTitle, TXT_DialogLoginMsg, TXT_DialogLoginButton1, TXT_DialogButtonCancel);
				}
			}
		}
	    case 0: // Player clicked "Cancel"
		{
			// Show a message that the player must be logged in to play on this server
			SendClientMessage(playerid, COLOR_WHITE, TXT_PlayerMustLogin);
		    // Kick the player
			SetTimerEx("TimedKick", (750 + GetPlayerPing(playerid)), false, "i", playerid);
		}
	}

	return 1;
}



// Process the rescue dialog
Dialog_Rescue(playerid, response, listitem)
{
	// Setup local variables
	new Message[128];

	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Check the class of the player
	switch (APlayerData[playerid][PlayerClass])
	{
		case ClassTruckDriver:
		{
			// Set the spawn coordinates based on the selection in the list
			switch (listitem)
			{
				case 0: SetSpawnInfo(playerid, 0, GetPlayerSkin(playerid), -525.0, -502.0, 26.0, 0.0, 0, 0, 0, 0, 0, 0);
				case 1: SetSpawnInfo(playerid, 0, GetPlayerSkin(playerid), -74.7, -1137.5, 4.5, 0.0, 0, 0, 0, 0, 0, 0);
				case 2: SetSpawnInfo(playerid, 0, GetPlayerSkin(playerid), 1457.0, 975.5, 11.0, 0.0, 0, 0, 0, 0, 0, 0);
				case 3: SetSpawnInfo(playerid, 0, GetPlayerSkin(playerid), -2136.0, -247.5, 36.5, 270.0, 0, 0, 0, 0, 0, 0);
				case 4: SetSpawnInfo(playerid, 0, GetPlayerSkin(playerid), 1766.5, -2040.7, 14.0, 270.0, 0, 0, 0, 0, 0, 0);
				case 5: SetSpawnInfo(playerid, 0, GetPlayerSkin(playerid), -546.0, 2594.0, 54.0, 270.0, 0, 0, 0, 0, 0, 0);
				case 6: SetSpawnInfo(playerid, 0, GetPlayerSkin(playerid), 332.0, 900.0, 25.0, 205.0, 0, 0, 0, 0, 0, 0);
				case 7: SetSpawnInfo(playerid, 0, GetPlayerSkin(playerid), -1575.0, -2724.0, 49.0, 146.0, 0, 0, 0, 0, 0, 0);
			}
		}
		case ClassBusDriver:
		{
			// Set the spawn coordinates based on the selection in the list
			switch (listitem)
			{
				case 0: SetSpawnInfo(playerid, 0, GetPlayerSkin(playerid), 1809.0, -1905.0, 13.6, 90.0, 0, 0, 0, 0, 0, 0);
				case 1: SetSpawnInfo(playerid, 0, GetPlayerSkin(playerid), -1983.0, 110.0, 27.7, 180.0, 0, 0, 0, 0, 0, 0);
				case 2: SetSpawnInfo(playerid, 0, GetPlayerSkin(playerid), 1060.0, 1260.0, 11.0, 270.0, 0, 0, 0, 0, 0, 0);
			}
		}
		case ClassPilot:
		{
			// Set the spawn coordinates based on the selection in the list
			switch (listitem)
			{
				case 0: SetSpawnInfo(playerid, 0, GetPlayerSkin(playerid), 2010.0, -2345.0, 13.6, 90.0, 0, 0, 0, 0, 0, 0);
				case 1: SetSpawnInfo(playerid, 0, GetPlayerSkin(playerid), -1211.0, -105.0, 14.2, 135.0, 0, 0, 0, 0, 0, 0);
				case 2: SetSpawnInfo(playerid, 0, GetPlayerSkin(playerid), 1630.0, 1615.0, 10.9, 90.0, 0, 0, 0, 0, 0, 0);
			}
		}
		case ClassPolice:
		{
			// Set the spawn coordinates based on the selection in the list
			switch (listitem)
			{
				case 0: SetSpawnInfo(playerid, 0, GetPlayerSkin(playerid), 1568.5, -1693.5, 6.0, 180.0, 0, 0, 0, 0, 0, 0);
				case 1: SetSpawnInfo(playerid, 0, GetPlayerSkin(playerid), -1590.0, 716.25, -5.0, 270.0, 0, 0, 0, 0, 0, 0);
				case 2: SetSpawnInfo(playerid, 0, GetPlayerSkin(playerid), 2275.0, 2460.0, 10.9, 90.0, 0, 0, 0, 0, 0, 0);
			}
		}
		case ClassCourier:
		{
			// Set the spawn coordinates based on the selection in the list
			switch (listitem)
			{
				case 0: SetSpawnInfo(playerid, 0, GetPlayerSkin(playerid), 798.0, -618.75, 16.4, 0.0, 0, 0, 0, 0, 0, 0);
				case 1: SetSpawnInfo(playerid, 0, GetPlayerSkin(playerid), -1849.25, -135.0, 12.0, 90.0, 0, 0, 0, 0, 0, 0);
				case 2: SetSpawnInfo(playerid, 0, GetPlayerSkin(playerid), 1050.5, 1931.0, 10.9, 270.0, 0, 0, 0, 0, 0, 0);
			}
		}
	}

	SpawnPlayer(playerid); // Force the player to spawn at the requested coordinates
	format(Message, sizeof(Message), TXT_PlayerRescued, PRICE_GOBASE);
	SendClientMessage(playerid, COLOR_GREEN, Message); // Send the player a message that using "/gobase"
	// Reduce the player's money
	RewardPlayer(playerid, -PRICE_GOBASE, 0);

	return 1;
}



// Process the BuyLicense dialog
Dialog_BuyLicenses(playerid, response, listitem)
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	switch (listitem)
	{
		case 0: // Player wants to buy a trucker license
		{
			// Check if the player is a trucker
			if (APlayerData[playerid][PlayerClass] == ClassTruckDriver)
			{
			    // Check if the player hasn't acquired his trucker's license yet
				if (APlayerData[playerid][TruckerLicense] == 0)
				{
					// Check if the player has enough money and enough score
					if ((APlayerData[playerid][PlayerMoney] >= 50000) && (APlayerData[playerid][PlayerScore] >= 100))
					{
						// Withdraw the money and score from the player's account
						RewardPlayer(playerid, -50000, -100);
						// Give the player the trucker's license
						APlayerData[playerid][TruckerLicense] = 1;
						// Let the player know he acquired his trucker's license
						SendClientMessage(playerid, COLOR_WHITE, TXT_TruckerLicenseBought);
					}
					else
					    SendClientMessage(playerid, COLOR_WHITE, TXT_TruckerLicenseNotEnoughMoney);
				}
				else
				    SendClientMessage(playerid, COLOR_WHITE, TXT_TruckerLicenseOwned);
			}
			else
				SendClientMessage(playerid, COLOR_WHITE, TXT_TruckerLicenseWrongClass);
		}
		case 1: // Player wants to buy a busdriver license
		{
			// Check if the player is a busdriver
			if (APlayerData[playerid][PlayerClass] == ClassBusDriver)
			{
			    // Check if the player hasn't acquired his busdriver's license yet
				if (APlayerData[playerid][BusLicense] == 0)
				{
					// Check if the player has enough money and enough score
					if ((APlayerData[playerid][PlayerMoney] >= 25000) && (APlayerData[playerid][PlayerScore] >= 50))
					{
						// Withdraw the money and score from the player's account
						RewardPlayer(playerid, -25000, -50);
						// Give the player the busdriver's license
						APlayerData[playerid][BusLicense] = 1;
						// Let the player know he acquired his busdriver's license
						SendClientMessage(playerid, COLOR_WHITE, TXT_BusLicenseBought);
					}
					else
					    SendClientMessage(playerid, COLOR_WHITE, TXT_BusLicenseNotEnoughMoney);
				}
				else
				    SendClientMessage(playerid, COLOR_WHITE, TXT_BusLicenseOwned);
			}
			else
				SendClientMessage(playerid, COLOR_WHITE, TXT_BusLicenseWrongClass);
		}
	}

	return 1;
}



// Process the work dialog for licensed truckers (show the list of loads after the player selected to choose his own load)
Dialog_TruckerSelectJobMethod(playerid, response, listitem)
{
	// Setup local variables
	new ProductList[50], NumProducts, TotalLoadList[1000];

	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Select what to do after the player chose "Select your own load" or "Auto assigned load"
	switch (listitem)
	{
		case 0: // Let the player choose his own load
		{
			switch (GetVehicleModel(GetPlayerVehicleID(playerid))) // Check the vehicle-model of the player to decide which loads the player can carry
			{
				case VehicleFlatbed, VehicleDFT30: // If the player's vehicle is a "Flatbed" or "DFT-30"
					ProductList = Product_GetList(PCV_TruckerNoTrailer, NumProducts); // Build a list of products defined for truckers without a trailer
				case VehicleCementTruck: // If the player's vehicle is a "CementTruck"
					ProductList = Product_GetList(PCV_TruckerCementTruck, NumProducts); // Build a list of products defined for truckers witha cement truck
				case VehicleLineRunner, VehicleTanker, VehicleRoadTrain: // If the player's vehicle is a "LineRunner", "Tanker" or "RoadTrain"
				{
					switch (GetVehicleModel(GetVehicleTrailer(GetPlayerVehicleID(playerid)))) // Select the loads based on the trailer model of the player
					{
						case VehicleTrailerCargo, VehicleTrailerCargo2: // A cargo-trailer is attached
							ProductList = Product_GetList(PCV_TruckerCargoTrailer, NumProducts); // Build a list of products defined for truckers with a cargo-trailer
						case VehicleTrailerOre: // An Ore-trailer is attached
							ProductList = Product_GetList(PCV_TruckerOreTrailer, NumProducts); // Build a list of products defined for truckers with an ore-trailer
						case VehicleTrailerFluids: // A fluids-trailer is attached
							ProductList = Product_GetList(PCV_TruckerFluidsTrailer, NumProducts); // Build a list of products defined for truckers with a fluids-trailer
					}
				}
			}

			// Build the List with product-names
			for (new i; i < NumProducts; i++)
				format(TotalLoadList, sizeof(TotalLoadList), "%s%s\n", TotalLoadList, ALoads[ProductList[i]][LoadName]);

			// Let the player choose a load
 			ShowPlayerDialog(playerid, DialogTruckerSelectLoad, DIALOG_STYLE_LIST, TXT_TruckerDialogSelectLoad, TotalLoadList, TXT_DialogButtonSelect, TXT_DialogButtonCancel);
		}

		case 1: Trucker_StartRandomJob(playerid); // Start a random trucker-job
	}

	return 1;
}



// Process the selected load and create the startlocation-dialog
Dialog_TruckerSelectLoad(playerid, response, listitem)
{
    // Setup local variables
	new TotalStartLocList[1000], ProductList[50], NumProducts, ProductID, LocID;

	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// First get the list of products again, so we can retrieve the selected load from it
	switch (GetVehicleModel(GetPlayerVehicleID(playerid))) // Check the vehicle-model of the player
	{
		case VehicleFlatbed, VehicleDFT30: // If the player's vehicle is a "Flatbed" or "DFT-30"
			ProductList = Product_GetList(PCV_TruckerNoTrailer, NumProducts); // Build a list of products defined for truckers without a trailer
		case VehicleCementTruck: // If the player's vehicle is a "CementTruck"
			ProductList = Product_GetList(PCV_TruckerCementTruck, NumProducts); // Build a list of products defined for truckers witha cement truck
		case VehicleLineRunner, VehicleTanker, VehicleRoadTrain: // If the player's vehicle is a "LineRunner", "Tanker" or "RoadTrain"
		{
			switch (GetVehicleModel(GetVehicleTrailer(GetPlayerVehicleID(playerid)))) // Select the loads based on the trailer model of the player
			{
				case VehicleTrailerCargo, VehicleTrailerCargo2: // A cargo-trailer is attached
					ProductList = Product_GetList(PCV_TruckerCargoTrailer, NumProducts); // Build a list of products defined for truckers with a cargo-trailer
				case VehicleTrailerOre: // An Ore-trailer is attached
					ProductList = Product_GetList(PCV_TruckerOreTrailer, NumProducts); // Build a list of products defined for truckers with an ore-trailer
				case VehicleTrailerFluids: // A fluids-trailer is attached
					ProductList = Product_GetList(PCV_TruckerFluidsTrailer, NumProducts); // Build a list of products defined for truckers with a fluids-trailer
			}
		}
	}

	// Store the selected LoadID in the player's account
	APlayerData[playerid][LoadID] = ProductList[listitem];
	ProductID = APlayerData[playerid][LoadID];

	// Build a list of start-locations for this product
	for (new i; i < 30; i++)
	{
	    // Get the location-id
	    LocID = ALoads[ProductID][FromLocations][i];
	    // Check if it a valid location-id (not 0)
	    if (LocID != 0)
			format(TotalStartLocList, sizeof(TotalStartLocList), "%s%s\n", TotalStartLocList, ALocations[LocID][LocationName]); // Add the location-name to the list
		else
		    break; // As soon as an invalid location-id has been found, stop adding entries to the location-list
	}

	// Ask the player to choose a start-location
	ShowPlayerDialog(playerid, DialogTruckerStartLoc, DIALOG_STYLE_LIST, TXT_TruckerDialogSelectStartLoc, TotalStartLocList, TXT_DialogButtonSelect, TXT_DialogButtonCancel); // Let the player choose a starting location

	return 1;
}



// Process the selected startlocation and create the endlocation-dialog
Dialog_TruckerSelectStartLoc(playerid, response, listitem)
{
    // Setup local variables
	new ProductID, LocID, TotalEndLocList[1000];

	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Get the LoadID that's stored in the player's account
	ProductID = APlayerData[playerid][LoadID];
	// Store the chosen start-location in the player's account
	APlayerData[playerid][JobLoc1] = ALoads[ProductID][FromLocations][listitem];

	// Build a list of end-locations for this product
	for (new i; i < 30; i++)
	{
	    // Get the location-id
	    LocID = ALoads[ProductID][ToLocations][i];
	    // Check if it a valid location-id (not 0)
	    if (LocID != 0)
			format(TotalEndLocList, sizeof(TotalEndLocList), "%s%s\n", TotalEndLocList, ALocations[LocID][LocationName]); // Add the location-name to the list
		else
		    break; // As soon as an invalid location-id has been found, stop adding entries to the location-list
	}

	// Ask the player to choose an end-location
	ShowPlayerDialog(playerid, DialogTruckerEndLoc, DIALOG_STYLE_LIST, TXT_TruckerDialogSelectEndLoc, TotalEndLocList, TXT_DialogButtonSelect, TXT_DialogButtonCancel); // Let the player choose a endlocation

	return 1;
}



// Process the selected endlocation and start the job
Dialog_TruckerSelectEndLoc(playerid, response, listitem)
{
    // Setup local variables
	new RouteText[128], loadName[50], startlocName[50], endlocName[50], LoadMsg[128], Float:x, Float:y, Float:z, ProductID;

	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Get the LoadID that's stored in the player's account
	ProductID = APlayerData[playerid][LoadID];
	// Store the chosen end-location in the player's account
	APlayerData[playerid][JobLoc2] = ALoads[ProductID][ToLocations][listitem];

    // Get the names for the load, startlocation and endlocation
	format(loadName, sizeof(loadName), "%s", ALoads[ProductID][LoadName]);
	format(startlocName, sizeof(startlocName), "%s", ALocations[APlayerData[playerid][JobLoc1]][LocationName]);
	format(endlocName, sizeof(endlocName), "%s", ALocations[APlayerData[playerid][JobLoc2]][LocationName]);

	// Job has started
	APlayerData[playerid][JobStarted] = true;
	// Store the vehicleID (required to be able to check if the player left his vehicle)
	APlayerData[playerid][VehicleID] = GetPlayerVehicleID(playerid);
	// Store the trailerID (required to be able to check if the player lost his trailer)
	APlayerData[playerid][TrailerID] = GetVehicleTrailer(GetPlayerVehicleID(playerid));
	// Set jobstep to 1 (going to load the goods)
	APlayerData[playerid][JobStep] = 1;
	// Combine all data into a string for the TextDraw (the player can see this all the time) to describe the mission
	format(RouteText, sizeof(RouteText), TXT_HaulingCargoFromToPickup, loadName, startlocName, endlocName);
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
	format(LoadMsg, sizeof(LoadMsg), TXT_PickupCargoAt, loadName, startlocName);
	SendClientMessage(playerid, COLOR_WHITE, LoadMsg);

	return 1;
}



// Process the work dialog for licensed busdrivers (show the list of busroutes after the player selected to choose his own route)
Dialog_BusSelectJobMethod(playerid, response, listitem)
{
	// Setup local variables
	new BusLine, Description[100], TotalRouteList[500];

	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

    // Check if the player already started a job
	if (APlayerData[playerid][JobStarted] == false)
	{
		// Select what to do after the player chose "Choose your own busroute" or "Auto assigned busroute"
		switch (listitem)
		{
			case 0: // Let the player choose his own busroute
			{
				// Construct the list of busroutes
				for (new i; i < sizeof(ABusRoutes); i++)
				{
					// Get the data about the route
					BusLine = ABusRoutes[i][LineNumber];
					format(Description, sizeof(Description), ABusRoutes[i][RouteDescription]);
					// Add the busline to the list
					format(TotalRouteList, sizeof(TotalRouteList), "%s%sLine %i (%s)", TotalRouteList, "\n", BusLine, Description);
				}

				// Let the player choose a busroute
 				ShowPlayerDialog(playerid, DialogBusSelectRoute, DIALOG_STYLE_LIST, TXT_SelectBusRoute, TotalRouteList, TXT_DialogButtonSelect, TXT_DialogButtonCancel);
			}

			case 1: BusDriver_StartJob(playerid, random(sizeof(ABusRoutes))); // Start a random job
		}
	}
	else // Send a message to let the player know he already has a job
	    SendClientMessage(playerid, COLOR_WHITE, TXT_BusdriverAlreadyDoingJob);

	return 1;
}

// Process the work dialog for licensed busdrivers (let the player start the job he selects from the busroute-list)
Dialog_BusSelectRoute(playerid, response, listitem)
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// The selected listitem is automatically the busroute, so start the job based on the item selected in the busroute-list
	BusDriver_StartJob(playerid, listitem);

	return 1;
}



// Process the chosen quantity for courier job
Dialog_CourierSelectQuant(playerid, response, listitem)
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Listitem 0 represent maximum 2 houses to deliver a package, listitem 1 is max 5 houses, item 2 is max 10 houses
	switch (listitem)
	{
		case 0: APlayerData[playerid][CourierMaxStep] = 2;
		case 1: APlayerData[playerid][CourierMaxStep] = 5;
		case 2: APlayerData[playerid][CourierMaxStep] = 10;
	}

	// Start the job
	Courier_BeginJob(playerid);

	return 1;
}



// Process the bike dialog
Dialog_Bike(playerid, response, listitem)
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Setup local variables
	new Float:x, Float:y, Float:z, Float:rot, CarID, msg[120], vID, pName[24];
	new engine,lights,alarm,doors,bonnet,boot,objective;

	// Get the player's position and name
	GetPlayerPos(playerid, x, y, z);
	GetPlayerFacingAngle(playerid, rot);
	GetPlayerName(playerid, pName, 24);

	// Spawn the motorcycle based on the selection in the list, and spawn it 5 units north of the player
	switch (listitem)
	{
		case 0: CarID = VehicleBike; // Bike
		case 1: CarID = VehicleBMX; // BMX
		case 2: CarID = VehicleMountainBike; // Mountain Bike
		case 3: CarID = VehicleFaggio; // Faggio
		case 4: CarID = VehiclePizzaBoy; // Pizzaboy
		case 5: CarID = VehicleBF400; // BF-400
		case 6: CarID = VehicleNRG500; // NRG-500
		case 7: CarID = VehiclePCJ600; // PCJ-600
		case 8: CarID = VehicleFCR900; // FCR-900
		case 9: CarID = VehicleFreeway; // Freeway
		case 10: CarID = VehicleWayfarer; // Wayfarer
		case 11: CarID = VehicleSanchez; // Sanchez
		case 12: CarID = VehicleQuad; // Quad
	}

	// Spawn the vehicle based on the selection in the list with a random color
	// Also set max-fuel and save the model for the vehicle
	vID = Vehicle_Create(CarID, x, y, z, rot, random(126), random(126), 600);
	// Put the player in the vehicle
	PutPlayerInVehicle(playerid, vID, 0);
	// Turn on the engine and lights
	GetVehicleParamsEx(vID, engine, lights, alarm, doors, bonnet, boot, objective);
	SetVehicleParamsEx(vID, 1, 1, alarm, doors, bonnet, boot, objective);
	// Store the player's current location and interior-id, otherwise anti-airbreak hack code could kick you
	GetPlayerPos(playerid, APlayerData[playerid][PreviousX], APlayerData[playerid][PreviousY], APlayerData[playerid][PreviousZ]);
	APlayerData[playerid][PreviousInt] = GetPlayerInterior(playerid);
	// Inform all players about it
	format(msg, sizeof(msg), TXT_PlayerSpawnedVehicle, pName, vID, CarID, x, y, z);
	SendClientMessageToAll(COLOR_WHITE, msg);

	return 1;
}



// Process the car dialog
Dialog_Car(playerid, response, listitem)
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response)
	{
		// Reset the FirstCar (so the list won't start at an unexpected location in the list, but back at the start of the list)
		APlayerData[playerid][DialogCarFirstCar] = 0;
		return 1; // Exit the function
	}

	// Process the selected listitem
	switch (listitem)
	{
		case 10: // Player chooses the empty line between the carlist and "Next...", so refresh the current list
		{
		    // Refresh the current list of cars and let the player choose one (show dialog)
		    CarList_Create(playerid);
			// Exit the function
			return 1;
		}
		case 11: // Player chooses "Next..."
		{
		    // Add 10 to the FirstCar, so the list will start further down the array (will show the next 10 cars)
		    APlayerData[playerid][DialogCarFirstCar] = APlayerData[playerid][DialogCarFirstCar] + 10;
		    // Create a new list of cars and show the dialog
		    CarList_Create(playerid);
		}
		default: // Player chooses a car
		{
		    // Setup local variables
	    	new Float:x, Float:y, Float:z, Float:rot, CarIndex, msg[128], vID, pName[24];
			new engine,lights,alarm,doors,bonnet,boot,objective;

			// FirstCar is the index of the first car in the list, so select the proper index when a car is chosen by the player
			CarIndex = APlayerData[playerid][DialogCarFirstCar] + listitem;

			// Get the player's position and name
			GetPlayerPos(playerid, x, y, z);
			GetPlayerFacingAngle(playerid, rot);
			GetPlayerName(playerid, pName, 24);

			// Spawn the vehicle based on the selection in the list with a random color
			// Also set max-fuel and save the model for the vehicle
			vID = Vehicle_Create(ACars[CarIndex][CarModel], x, y, z, rot, random(126), random(126), 600);
			// Put the player in the vehicle
			PutPlayerInVehicle(playerid, vID, 0);
			// Turn on the engine and lights
			GetVehicleParamsEx(vID, engine, lights, alarm, doors, bonnet, boot, objective);
			SetVehicleParamsEx(vID, 1, 1, alarm, doors, bonnet, boot, objective);
			// Store the player's current location and interior-id, otherwise anti-airbreak hack code could kick you
			GetPlayerPos(playerid, APlayerData[playerid][PreviousX], APlayerData[playerid][PreviousY], APlayerData[playerid][PreviousZ]);
			APlayerData[playerid][PreviousInt] = GetPlayerInterior(playerid);
			// Inform all players about it
			format(msg, sizeof(msg), TXT_PlayerSpawnedVehicle, pName, vID, ACars[CarIndex][CarModel], x, y, z);
			SendClientMessageToAll(COLOR_WHITE, msg);
			// Also, after spawning a vehicle, reset FirstCar (so next time, the list starts from the start)
			APlayerData[playerid][DialogCarFirstCar] = 0;
		}
	}

	return 1;
}



// Process the Plane dialog
Dialog_Plane(playerid, response, listitem)
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response)
	{
		// Reset the FirstPlane (so the list won't start at an unexpected location in the list, but back at the start of the list)
		APlayerData[playerid][DialogPlaneFirstPlane] = 0;
		return 1; // Exit the function
	}

	// Process the selected listitem
	switch (listitem)
	{
		case 10: // Player chooses the empty line between the Planelist and "Next...", so refresh the current list
		{
		    // Refresh the current list of Planes and let the player choose one (show dialog)
		    PlaneList_Create(playerid);
			// Exit the function
			return 1;
		}
		case 11: // Player chooses "Next..."
		{
		    // Add 10 to the FirstPlane, so the list will start further down the array (will show the next 10 Planes)
		    APlayerData[playerid][DialogPlaneFirstPlane] = APlayerData[playerid][DialogPlaneFirstPlane] + 10;
		    // Create a new list of Planes and show the dialog
		    PlaneList_Create(playerid);
		}
		default: // Player chooses a Plane
		{
		    // Setup local variables
	    	new Float:x, Float:y, Float:z, Float:rot, PlaneIndex, msg[128], vID, pName[24];
			new engine,lights,alarm,doors,bonnet,boot,objective;

			// FirstPlane is the index of the first Plane in the list, so select the proper index when a Plane is chosen by the player
			PlaneIndex = APlayerData[playerid][DialogPlaneFirstPlane] + listitem;

			// Get the player's position and name
			GetPlayerPos(playerid, x, y, z);
			GetPlayerFacingAngle(playerid, rot);
			GetPlayerName(playerid, pName, 24);

			// Spawn the vehicle based on the selection in the list with a random color
			// Also set max-fuel and save the model for the vehicle
			vID = Vehicle_Create(APlanes[PlaneIndex][PlaneModel], x, y, z, rot, random(126), random(126), 600);
			// Put the player in the vehicle
			PutPlayerInVehicle(playerid, vID, 0);
			// Turn on the engine and lights
			GetVehicleParamsEx(vID, engine, lights, alarm, doors, bonnet, boot, objective);
			SetVehicleParamsEx(vID, 1, 1, alarm, doors, bonnet, boot, objective);
			// Store the player's current location and interior-id, otherwise anti-airbreak hack code could kick you
			GetPlayerPos(playerid, APlayerData[playerid][PreviousX], APlayerData[playerid][PreviousY], APlayerData[playerid][PreviousZ]);
			APlayerData[playerid][PreviousInt] = GetPlayerInterior(playerid);
			// Inform the player about it
			format(msg, sizeof(msg), TXT_PlayerSpawnedVehicle, pName, vID, APlanes[PlaneIndex][PlaneModel], x, y, z);
			SendClientMessageToAll(COLOR_WHITE, msg);
			// Also, after spawning a vehicle, reset FirstPlane (so next time, the list starts from the start)
			APlayerData[playerid][DialogPlaneFirstPlane] = 0;
		}
	}

	return 1;
}



// Process the Trailer dialog
Dialog_Trailer(playerid, response, listitem)
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response)
	{
		// Reset the FirstTrailer (so the list won't start at an unexpected location in the list, but back at the start of the list)
		APlayerData[playerid][DialogTrailerFirstTrailer] = 0;
		return 1; // Exit the function
	}

	// Process the selected listitem
	switch (listitem)
	{
		case 10: // Player chooses the empty line between the Trailerlist and "Next...", so refresh the current list
		{
		    // Refresh the current list of Trailers and let the player choose one (show dialog)
		    TrailerList_Create(playerid);
			// Exit the function
			return 1;
		}
		case 11: // Player chooses "Next..."
		{
		    // Add 10 to the FirstTrailer, so the list will start further down the array (will show the next 10 Trailers)
		    APlayerData[playerid][DialogTrailerFirstTrailer] = APlayerData[playerid][DialogTrailerFirstTrailer] + 10;
		    // Create a new list of Trailers and show the dialog
		    TrailerList_Create(playerid);
		}
		default: // Player chooses a Trailer
		{
		    // Setup local variables
	    	new Float:x, Float:y, Float:z, TrailerIndex, msg[128], vID, pName[24];

			// FirstTrailer is the index of the first Trailer in the list, so select the proper index when a Trailer is chosen by the player
			TrailerIndex = APlayerData[playerid][DialogTrailerFirstTrailer] + listitem;

			// Get the player's position and name
			GetPlayerPos(playerid, x, y, z);
			GetPlayerName(playerid, pName, 24);

			// Spawn the vehicle based on the selection in the list with a random color, and spawn it 10 units north of the player
			// Also set max-fuel and save the model for the vehicle
			vID = Vehicle_Create(ATrailers[TrailerIndex][TrailerModel], x, y + 10.0, z + 5.0, 0.0, random(126), random(126), 600);
			// Inform the player about it
			format(msg, sizeof(msg), TXT_PlayerSpawnedVehicle, pName, vID, ATrailers[TrailerIndex][TrailerModel], x, y + 10.0, z + 5.0);
			SendClientMessageToAll(COLOR_WHITE, msg);
			// Also, after spawning a vehicle, reset FirstTrailer (so next time, the list starts from the start)
			APlayerData[playerid][DialogTrailerFirstTrailer] = 0;
		}
	}

	return 1;
}



// Process the boat dialog
Dialog_Boat(playerid, response, listitem)
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Setup local variables
	new Float:x, Float:y, Float:z, Float:rot, CarID, msg[120], vID, pName[24];
	new engine,lights,alarm,doors,bonnet,boot,objective;

	// Get the player's position and name
	GetPlayerPos(playerid, x, y, z);
	GetPlayerFacingAngle(playerid, rot);
	GetPlayerName(playerid, pName, 24);

	// Spawn the boat based on the selection in the list
	switch (listitem)
	{
		case 0: CarID = VehicleCoastguard; // Coastguard
		case 1: CarID = VehicleDinghy; // Dinghy
		case 2: CarID = VehicleJetmax; // Jetmax
		case 3: CarID = VehicleLaunch; // Launch
		case 4: CarID = VehicleMarquis; // Marquis
		case 5: CarID = VehiclePredator; // Predator
		case 6: CarID = VehicleReefer; // Reefer
		case 7: CarID = VehicleSpeeder; // Speeder
		case 8: CarID = VehicleSquallo; // Squallo
		case 9: CarID = VehicleTropic; // Tropic
	}

	// Spawn the vehicle based on the selection in the list with a random color
	// Also set max-fuel and save the model for the vehicle
	vID = Vehicle_Create(CarID, x, y, z, rot, random(126), random(126), 600);
	// Put the player in the vehicle
	PutPlayerInVehicle(playerid, vID, 0);
	// Turn on the engine and lights
	GetVehicleParamsEx(vID, engine, lights, alarm, doors, bonnet, boot, objective);
	SetVehicleParamsEx(vID, 1, 1, alarm, doors, bonnet, boot, objective);
	// Store the player's current location and interior-id, otherwise anti-airbreak hack code could kick you
	GetPlayerPos(playerid, APlayerData[playerid][PreviousX], APlayerData[playerid][PreviousY], APlayerData[playerid][PreviousZ]);
	APlayerData[playerid][PreviousInt] = GetPlayerInterior(playerid);
	// Inform all players about it
	format(msg, sizeof(msg), TXT_PlayerSpawnedVehicle, pName, vID, CarID, x, y, z);
	SendClientMessageToAll(COLOR_WHITE, msg);

	return 1;
}



// Process the neon dialog
Dialog_Neon(playerid, response, listitem)
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Setup local variables
	new vehicleid;

	// Get the player's vehicle
	vehicleid = GetPlayerVehicleID(playerid);

	// Check if the player chose anothet option than "Remove neons"
	if (listitem != 7)
	{
		// Check if the vehicle already has neon lights attached
	    if (AVehicleData[vehicleid][NeonObjectModel] != 0)
	    {
	        // Delete the current neon-lights and clear the variables
            DestroyObject(AVehicleData[vehicleid][NeonLeft]);
            DestroyObject(AVehicleData[vehicleid][NeonRight]);
            AVehicleData[vehicleid][NeonLeft] = 0;
            AVehicleData[vehicleid][NeonRight] = 0;
            AVehicleData[vehicleid][NeonObjectModel] = 0;
	    }
	}

	// Process the option chosen from the dialog
	if (listitem == 0) // Police Light
	{
	    AVehicleData[vehicleid][NeonLeft] = CreateObject(18646,0,0,0,0,0,0);
	    AVehicleData[vehicleid][NeonRight] = CreateObject(18646,0,0,0,0,0,0);
	    AVehicleData[vehicleid][NeonObjectModel] = 18646;
		AttachObjectToVehicle(AVehicleData[vehicleid][NeonLeft], vehicleid, -0.8, 0.0, -0.70, 0.0, 0.0, 0.0);
		AttachObjectToVehicle(AVehicleData[vehicleid][NeonRight], vehicleid, 0.8, 0.0, -0.70, 0.0, 0.0, 0.0);
		GameTextForPlayer(playerid, "~p~Police lights~w~ have been added to your vehicle", 3500, 5);
	}

	if (listitem == 1) // Red light
	{
	    AVehicleData[vehicleid][NeonLeft] = CreateObject(18647,0,0,0,0,0,0);
	    AVehicleData[vehicleid][NeonRight] = CreateObject(18647,0,0,0,0,0,0);
	    AVehicleData[vehicleid][NeonObjectModel] = 18647;
		AttachObjectToVehicle(AVehicleData[vehicleid][NeonLeft], vehicleid, -0.8, 0.0, -0.70, 0.0, 0.0, 0.0);
		AttachObjectToVehicle(AVehicleData[vehicleid][NeonRight], vehicleid, 0.8, 0.0, -0.70, 0.0, 0.0, 0.0);
		GameTextForPlayer(playerid, "~p~Red lights~w~ have been added to your vehicle", 3500, 5);
	}

	if (listitem == 2) // Blue light
	{
	    AVehicleData[vehicleid][NeonLeft] = CreateObject(18648,0,0,0,0,0,0);
	    AVehicleData[vehicleid][NeonRight] = CreateObject(18648,0,0,0,0,0,0);
	    AVehicleData[vehicleid][NeonObjectModel] = 18648;
		AttachObjectToVehicle(AVehicleData[vehicleid][NeonLeft], vehicleid, -0.8, 0.0, -0.70, 0.0, 0.0, 0.0);
		AttachObjectToVehicle(AVehicleData[vehicleid][NeonRight], vehicleid, 0.8, 0.0, -0.70, 0.0, 0.0, 0.0);
		GameTextForPlayer(playerid, "~p~Blue lights~w~ have been added to your vehicle", 3500, 5);
	}

	if (listitem == 3) // Green Light
	{
	    AVehicleData[vehicleid][NeonLeft] = CreateObject(18649,0,0,0,0,0,0);
	    AVehicleData[vehicleid][NeonRight] = CreateObject(18649,0,0,0,0,0,0);
	    AVehicleData[vehicleid][NeonObjectModel] = 18649;
		AttachObjectToVehicle(AVehicleData[vehicleid][NeonLeft], vehicleid, -0.8, 0.0, -0.70, 0.0, 0.0, 0.0);
		AttachObjectToVehicle(AVehicleData[vehicleid][NeonRight], vehicleid, 0.8, 0.0, -0.70, 0.0, 0.0, 0.0);
		GameTextForPlayer(playerid, "~p~Green lights~w~ have been added to your vehicle", 3500, 5);
	}

	if (listitem == 4) // Yellow Light
	{
	    AVehicleData[vehicleid][NeonLeft] = CreateObject(18650,0,0,0,0,0,0);
	    AVehicleData[vehicleid][NeonRight] = CreateObject(18650,0,0,0,0,0,0);
	    AVehicleData[vehicleid][NeonObjectModel] = 18650;
		AttachObjectToVehicle(AVehicleData[vehicleid][NeonLeft], vehicleid, -0.8, 0.0, -0.70, 0.0, 0.0, 0.0);
		AttachObjectToVehicle(AVehicleData[vehicleid][NeonRight], vehicleid, 0.8, 0.0, -0.70, 0.0, 0.0, 0.0);
		GameTextForPlayer(playerid, "~p~Yellow lights~w~ have been added to your vehicle", 3500, 5);
	}

	if (listitem == 5) // Pink Light
	{
	    AVehicleData[vehicleid][NeonLeft] = CreateObject(18651,0,0,0,0,0,0);
	    AVehicleData[vehicleid][NeonRight] = CreateObject(18651,0,0,0,0,0,0);
	    AVehicleData[vehicleid][NeonObjectModel] = 18651;
		AttachObjectToVehicle(AVehicleData[vehicleid][NeonLeft], vehicleid, -0.8, 0.0, -0.70, 0.0, 0.0, 0.0);
		AttachObjectToVehicle(AVehicleData[vehicleid][NeonRight], vehicleid, 0.8, 0.0, -0.70, 0.0, 0.0, 0.0);
		GameTextForPlayer(playerid, "~p~Pink lights~w~ have been added to your vehicle", 3500, 5);
	}

	if (listitem == 6) // White Light
	{
	    AVehicleData[vehicleid][NeonLeft] = CreateObject(18652,0,0,0,0,0,0);
	    AVehicleData[vehicleid][NeonRight] = CreateObject(18652,0,0,0,0,0,0);
	    AVehicleData[vehicleid][NeonObjectModel] = 18652;
		AttachObjectToVehicle(AVehicleData[vehicleid][NeonLeft], vehicleid, -0.8, 0.0, -0.70, 0.0, 0.0, 0.0);
		AttachObjectToVehicle(AVehicleData[vehicleid][NeonRight], vehicleid, 0.8, 0.0, -0.70, 0.0, 0.0, 0.0);
		GameTextForPlayer(playerid, "~p~White lights~w~ have been added to your vehicle", 3500, 5);
	}

	if (listitem == 7) // Remove neons
	{
        // Delete the current neon-lights and clear the variables
        DestroyObject(AVehicleData[vehicleid][NeonLeft]);
        DestroyObject(AVehicleData[vehicleid][NeonRight]);
        AVehicleData[vehicleid][NeonLeft] = 0;
        AVehicleData[vehicleid][NeonRight] = 0;
        AVehicleData[vehicleid][NeonObjectModel] = 0;
		GameTextForPlayer(playerid, "Neons have been removed from your vehicle", 3500, 5);
	}

	return 1;
}



// Process the chosen vehicleclass to show the actual vehicle-list of this vehicleclass
Dialog_RentProcessClass(playerid, response, listitem)
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Setup local variables
	new CarList[1000], DialogTitle[128];

	// Set the player's chosen vehicle-class based on the listitem he chose (add 1 as the vehicleclass starts at 1)
	APlayerData[playerid][DialogRentVClass] = listitem + 1;

	// Add all vehicles of the same class to the list
	for (new i; i < sizeof(ABuyableVehicles); i++)
	{
		// Check if the vehicle in the list has the same class as requested
		if (ABuyableVehicles[i][VehicleClass] == APlayerData[playerid][DialogRentVClass])
		{
			// Add the carname to the list and it's price
			format(CarList, sizeof(CarList), "%s%s%s ($%i)", CarList, "\n", ABuyableVehicles[i][CarName], ABuyableVehicles[i][VehiclePrice] / 10); // Add the name of the next car to the list on the next line
		}
	}

	// Check if the list is empty
	if (strlen(CarList) == 0)
	{
		// Send the player a message that all vehicles have been disabled of the chosen class (no vehicles in the array of this class)
		SendClientMessage(playerid, COLOR_WHITE, TXT_BuyRentVehicleEmptyList);
		// Exit the function (don't ask to choose a vehicle)
		return 1;
	}

	// Set a title for the dialog based on the requested vehicleclass
	switch (APlayerData[playerid][DialogRentVClass])
	{
		case VClassBike: format(DialogTitle, sizeof(DialogTitle), TXT_RentBike);
		case VClassBoat: format(DialogTitle, sizeof(DialogTitle), TXT_RentBoat);
		case VClassConvertible: format(DialogTitle, sizeof(DialogTitle), TXT_RentConvertible);
		case VClassHelicopter: format(DialogTitle, sizeof(DialogTitle), TXT_RentHelicopter);
		case VClassIndustrial: format(DialogTitle, sizeof(DialogTitle), TXT_RentIndustrial);
		case VClassLowRider: format(DialogTitle, sizeof(DialogTitle), TXT_RentLowRider);
		case VClassOffRoad: format(DialogTitle, sizeof(DialogTitle), TXT_RentOffRoad);
		case VClassPlane: format(DialogTitle, sizeof(DialogTitle), TXT_RentPlane);
		case VClassPublic: format(DialogTitle, sizeof(DialogTitle), TXT_RentPublicService);
		case VClassRCVehicle: format(DialogTitle, sizeof(DialogTitle), TXT_RentRC);
		case VClassSaloons: format(DialogTitle, sizeof(DialogTitle), TXT_RentSaloon);
		case VClassSportCar: format(DialogTitle, sizeof(DialogTitle), TXT_RentSport);
		case VClassStationCar: format(DialogTitle, sizeof(DialogTitle), TXT_RentStationWagon);
		case VClassTrailer: format(DialogTitle, sizeof(DialogTitle), TXT_RentTrailer);
		case VClassUnique: format(DialogTitle, sizeof(DialogTitle), TXT_RentUnique);
	}

	// Ask which car the player wants to have by showing the dialog
	ShowPlayerDialog(playerid, DialogRentVehicle, DIALOG_STYLE_LIST, DialogTitle, CarList, TXT_DialogButtonSelect, TXT_DialogButtonCancel);

	return 1;
}



// This dialog gives the chosen vehicle to the player (bought or rented)
Dialog_RentVehicle(playerid, response, listitem)
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Setup local variables
	new Counter, Name[24], Float:x, Float:y, Float:z, Msg[128];

	// Loop through the entire list of buyable vehicles
	for (new i; i < sizeof(ABuyableVehicles); i++)
	{
		// Check if the vehicle in the list has the same class as requested
		if (ABuyableVehicles[i][VehicleClass] == APlayerData[playerid][DialogRentVClass])
		{
		    // Check if this is the vehicle that the player chose
		    if (Counter == listitem)
			{
			    // Check if the player has enough money to afford renting this vehicle
			    if (APlayerData[playerid][PlayerMoney] >= (ABuyableVehicles[i][VehiclePrice] / 10))
			    {
					// Check if the player already has a rented vehicle
					if (APlayerData[playerid][RentedVehicleID] != 0)
					{
						// Clear the data for the already rented vehicle
						AVehicleData[APlayerData[playerid][RentedVehicleID]][Model] = 0;
						AVehicleData[APlayerData[playerid][RentedVehicleID]][Fuel] = 0;
						AVehicleData[APlayerData[playerid][RentedVehicleID]][Owned] = false;
						AVehicleData[APlayerData[playerid][RentedVehicleID]][Owner] = 0;
						AVehicleData[APlayerData[playerid][RentedVehicleID]][PaintJob] = 0;
						for (new j; j < 14; j++)
						{
							AVehicleData[APlayerData[playerid][RentedVehicleID]][Components][j] = 0;
						}
						// Destroy the vehicle
						DestroyVehicle(APlayerData[playerid][RentedVehicleID]);
						// Send the player a message that his previously rented vehicle has been replaced by the new one
						SendClientMessage(playerid, COLOR_WHITE, TXT_AlreadyRentedVehicle);
					}

					// Create the vehicle and link it to the player's account
					GetPlayerPos(playerid, x, y, z);
				    APlayerData[playerid][RentedVehicleID] = CreateVehicle(ABuyableVehicles[i][CarModel], x, y, z, 0.0, -1, -1, 600);
					// Also set owndership data
					AVehicleData[APlayerData[playerid][RentedVehicleID]][Model] = ABuyableVehicles[i][CarModel];
					AVehicleData[APlayerData[playerid][RentedVehicleID]][Fuel] = MaxFuel;
					AVehicleData[APlayerData[playerid][RentedVehicleID]][Owned] = true;
					GetPlayerName(playerid, Name, sizeof(Name));
					format(AVehicleData[APlayerData[playerid][RentedVehicleID]][Owner], 24, Name);
					// Put the player in the vehicle
					PutPlayerInVehicle(playerid, APlayerData[playerid][RentedVehicleID], 0);
					// Turn on the engine
					new engine,lights,alarm,doors,bonnet,boot,objective;
					GetVehicleParamsEx(APlayerData[playerid][RentedVehicleID], engine, lights, alarm, doors, bonnet, boot, objective);
					SetVehicleParamsEx(APlayerData[playerid][RentedVehicleID], 1, lights, alarm, doors, bonnet, boot, objective);
					// Store the player's current location and interior-id, otherwise anti-airbreak hack code could kick you
					GetPlayerPos(playerid, APlayerData[playerid][PreviousX], APlayerData[playerid][PreviousY], APlayerData[playerid][PreviousZ]);
					APlayerData[playerid][PreviousInt] = GetPlayerInterior(playerid);
					// Let the player pay for renting the vehicle
					APlayerData[playerid][PlayerMoney] = APlayerData[playerid][PlayerMoney] - (ABuyableVehicles[i][VehiclePrice] / 10);
					// Let the player know he rented a vehicle
					format(Msg, sizeof(Msg), TXT_PlayerRentsVehicle, ABuyableVehicles[i][CarName], (ABuyableVehicles[i][VehiclePrice] / 10));
					SendClientMessage(playerid, COLOR_WHITE, Msg);
				}
				else // The player has not enough money to rent this vehicle
				    SendClientMessage(playerid, COLOR_WHITE, TXT_RentingTooExpensive);

				// Stop looking for the vehicle, as it's found and given to the player already
				return 1;
			}
		    else // The player chose another vehicle from the same class, keep looking for another one
		        Counter++;
		}
	}

	return 1;
}



// Process the weather dialog
Dialog_Weather(playerid, response, listitem)
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Setup local variables
	new WeatherID;

	// Select the weather
	switch (listitem)
	{
		case 0: WeatherID = 0; // Normal
		case 1: WeatherID = 8; // Stormy
		case 2: WeatherID = 9; // Foggy
		case 3: WeatherID = 11; // Scorching hot
		case 4: WeatherID = 16; // Dull, cloudy, rainy
		case 5: WeatherID = 19; // Sandstorm
		case 6: WeatherID = 20; // Green Fog
		case 7: WeatherID = 33; // Dark, cloudy, brown
		case 8: WeatherID = 39; // Extremely bright
		case 9: WeatherID = 43; // Dark toxic clouds
		case 10: WeatherID = 44; // Black & white sky
	}

	// Set the weather
    SetWeather(WeatherID);
    // Inform the player about it
	SendClientMessage(playerid, COLOR_WHITE, TXT_ChangedWeather);

	return 1;
}

// Process the caroption dialog
Dialog_CarOption(playerid, response, listitem)
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

    // Setup local variables
	new vehicleid, engine, lights, bool:alarm, bool:doors, bool:bonnet, bool:boot, bool:objective;

	// Get the player's vehicle
	vehicleid = GetPlayerVehicleID(playerid);

	// Get the current status of the vehicle
	GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);

	// Select the weather
	switch (listitem)
	{
		case 0: SetVehicleParamsEx(vehicleid, 1 - engine, lights, alarm, doors, bonnet, boot, objective);
		case 1: SetVehicleParamsEx(vehicleid, engine, 1 - lights, alarm, doors, bonnet, boot, objective);
		case 2:
		{
			if (alarm == true)
				SetVehicleParamsEx(vehicleid, engine, lights, false, doors, bonnet, boot, objective);
			else
				SetVehicleParamsEx(vehicleid, engine, lights, true, doors, bonnet, boot, objective);
		}
		case 3:
		{
			if (doors == true)
				SetVehicleParamsEx(vehicleid, engine, lights, alarm, false, bonnet, boot, objective);
			else
				SetVehicleParamsEx(vehicleid, engine, lights, alarm, true, bonnet, boot, objective);
		}
		case 4:
		{
			if (bonnet == true)
				SetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, false, boot, objective);
			else
				SetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, true, boot, objective);
		}
		case 5:
		{
			if (boot == true)
				SetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, false, objective);
			else
				SetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, true, objective);
		}
		case 6:
		{
			if (objective == true)
				SetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, false);
			else
				SetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, true);
		}
	}

	return 1;
}



// This function processes the convoy dialog
Dialog_SelectConvoy(playerid, response, listitem)
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Setup local variables
	new Convoy;

	// The convoy-id is automatically the listitem
	Convoy = listitem;

	// Check the status of the selected convoy to determine what to do
	switch (AConvoys[Convoy][Status])
	{
	    case CONVOY_EMPTY: // Convoy isn't started yet, this is a free convoy
		{
			Convoy_Create(playerid, Convoy); // Try to create the convoy
		}
	    case CONVOY_OPEN: // Convoy has been created but still open for new members to join
		{
		    Convoy_Join(playerid, Convoy); // Try to join the convoy
		}
	    case CONVOY_FULL: // Convoy is full (10 members)
		{
			SendClientMessage(playerid, COLOR_WHITE, TXT_ConvoyFull);
		}
	    case CONVOY_CLOSED: // Convoy has started already (the leader already chose a job, causing the convoy to close)
		{
			SendClientMessage(playerid, COLOR_WHITE, TXT_ConvoyEnRoute);
		}
	}

	return 1;
}

// This function processes the housemenu dialog
Dialog_HouseMenu(playerid, response, listitem)
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Setup local variables
	new UpgradeList[2000], HouseID, DialogTitle[200], Counter, UpgradePrice, Msg[128], CarSlot, VehicleClassList[1000], VehicleList[500];

	// Get the HouseID of the house where the player is
	HouseID = APlayerData[playerid][CurrentHouse];

	// Select an option based on the selection in the list
	switch(listitem)
	{
	    case 0: // Change house name
	    {
			// Only the house-owner can use this option
			if (House_PlayerIsOwner(playerid, HouseID) != 0)
			{
		        format(DialogTitle, sizeof(DialogTitle), TXT_DialogOldHouseName, AHouseData[HouseID][HouseName]);
				ShowPlayerDialog(playerid, DialogHouseNameChange, DIALOG_STYLE_INPUT, DialogTitle, TXT_DialogEnterNewHouseName, TXT_DialogButtonSelect, TXT_DialogButtonCancel);
			}
			else
			    SendClientMessage(playerid, COLOR_RED, "Only the house-owner can use this option");
	    }
	    case 1: // Upgrade the house
	    {
			// Only the house-owner can use this option
			if (House_PlayerIsOwner(playerid, HouseID) != 0)
			{
		        // Check if it's possible to upgrade further
				if (AHouseData[HouseID][HouseLevel] < AHouseData[HouseID][HouseMaxLevel])
				{
					// Add only the upgrades above the current house-level to the upgradelist
					for (new i = AHouseData[HouseID][HouseLevel] + 1; i <= AHouseData[HouseID][HouseMaxLevel]; i++)
					{
					    Counter++; // Add 1 to the counter each time an upgrade is added to the upgradelist
					    UpgradePrice = ((AHouseData[HouseID][HousePrice] * Counter) / 100) * HouseUpgradePercent;
					    // Add the upgrade-info in green if the player can afford it, and in red if he cannot afford it
						if (APlayerData[playerid][PlayerMoney] >= UpgradePrice)
							format(UpgradeList, sizeof(UpgradeList), "%s{00FF00}%s (level %i)\t\t$%i\n", UpgradeList, AHouseInteriors[i][InteriorName], i, UpgradePrice);
						else
							format(UpgradeList, sizeof(UpgradeList), "%s{FF0000}%s (level %i)\t\t$%i\n", UpgradeList, AHouseInteriors[i][InteriorName], i, UpgradePrice);
					}

			        // Show another dialog to let the player select which upgrade he wants for his house
					ShowPlayerDialog(playerid, DialogUpgradeHouse, DIALOG_STYLE_LIST, TXT_DialogSelectHouseUpgrade, UpgradeList, TXT_DialogButtonSelect, TXT_DialogButtonCancel);
				}
				else
				    SendClientMessage(playerid, COLOR_WHITE, TXT_HouseReachedMaxLevel);
			}
			else
			    SendClientMessage(playerid, COLOR_RED, "Only the house-owner can use this option");
	    }
		case 2: // Buy house-car
		{
			// Only the house-owner can use this option
			if (House_PlayerIsOwner(playerid, HouseID) != 0)
			{
			    // Find a free carslot
			    CarSlot = House_GetFreeCarSlot(HouseID);
			    // Check if the carslot is valid
			    if (CarSlot != -1)
			    {
				    // Let the player choose a vehicle-class
					format(VehicleClassList, sizeof(VehicleClassList), "%s{00FF00}%s{FFFFFF}\n", VehicleClassList, "Bikes");
					format(VehicleClassList, sizeof(VehicleClassList), "%s{40FF00}%s{FFFFFF}\n", VehicleClassList, "Boats");
					format(VehicleClassList, sizeof(VehicleClassList), "%s{80FF00}%s{FFFFFF}\n", VehicleClassList, "Convertibles");
					format(VehicleClassList, sizeof(VehicleClassList), "%s{B0FF00}%s{FFFFFF}\n", VehicleClassList, "Helicopters");
					format(VehicleClassList, sizeof(VehicleClassList), "%s{FFFF00}%s{FFFFFF}\n", VehicleClassList, "Industrial vehicles");
					format(VehicleClassList, sizeof(VehicleClassList), "%s{B0FF40}%s{FFFFFF}\n", VehicleClassList, "Low-riders");
					format(VehicleClassList, sizeof(VehicleClassList), "%s{80FF80}%s{FFFFFF}\n", VehicleClassList, "Off-Road vehicles");
					format(VehicleClassList, sizeof(VehicleClassList), "%s{40FFB0}%s{FFFFFF}\n", VehicleClassList, "Planes");
					format(VehicleClassList, sizeof(VehicleClassList), "%s{00FFFF}%s{FFFFFF}\n", VehicleClassList, "Public Service vehicles");
					format(VehicleClassList, sizeof(VehicleClassList), "%s{00B0FF}%s{FFFFFF}\n", VehicleClassList, "RC vehicles");
					format(VehicleClassList, sizeof(VehicleClassList), "%s{0080FF}%s{FFFFFF}\n", VehicleClassList, "Saloon vehicles");
					format(VehicleClassList, sizeof(VehicleClassList), "%s{0040FF}%s{FFFFFF}\n", VehicleClassList, "Sport vehicles");
					format(VehicleClassList, sizeof(VehicleClassList), "%s{0000FF}%s{FFFFFF}\n", VehicleClassList, "Station wagons");
					format(VehicleClassList, sizeof(VehicleClassList), "%s{4000FF}%s{FFFFFF}\n", VehicleClassList, "Trailers");
					format(VehicleClassList, sizeof(VehicleClassList), "%s{8000FF}%s{FFFFFF}\n", VehicleClassList, "Unique vehicles");
					// Ask which vehicle class the player wants to see to buy a vehicle
					ShowPlayerDialog(playerid, DialogBuyCarClass, DIALOG_STYLE_LIST, TXT_DialogBuyCarSelectClass, VehicleClassList, TXT_DialogButtonSelect, TXT_DialogButtonCancel);
			    }
			    else
			        SendClientMessage(playerid, COLOR_WHITE, TXT_HouseHasMaxVehicles);
			}
			else
			    SendClientMessage(playerid, COLOR_RED, "Only the house-owner can use this option");
		}
		case 3: // Buy house-car insurance
		{
			// Only the house-owner can use this option
			if (House_PlayerIsOwner(playerid, HouseID) != 0)
			{
			    new MsgInsurance[128];
			    // Check if this house doesn't have insurance yet
			    if (AHouseData[HouseID][Insurance] == 0)
			    {
			        // Check if the player can afford the insurance
			        if (APlayerData[playerid][PlayerMoney] >= (AHouseData[HouseID][HousePrice] / 10))
			        {
					    // Construct the message for the dialog, this includes the price for the insurance
					    format(MsgInsurance, sizeof(MsgInsurance), TXT_DialogBuyInsurance, AHouseData[HouseID][HousePrice] / 10);
						// Ask the player if the wants to buy an insurance for this house's vehicles
						ShowPlayerDialog(playerid, DialogBuyInsurance, DIALOG_STYLE_MSGBOX, TXT_DialogTitleBuyInsurance, MsgInsurance, TXT_DialogButtonYes, TXT_DialogButtonNo);
					}
					else
					    SendClientMessage(playerid, COLOR_WHITE, TXT_CannotAffordInsurance);
				}
				else
				    SendClientMessage(playerid, COLOR_WHITE, TXT_HouseAlreadyHasInsurance);
			}
			else
			    SendClientMessage(playerid, COLOR_RED, "Only the house-owner can use this option");
		}
		case 4: // Sell house-car
		{
			// Only the house-owner can use this option
			if (House_PlayerIsOwner(playerid, HouseID) != 0)
			{
			    new BuyableCarIndex, bool:HouseHasCars = false;

				// Check if the house has any cars assigned to it
				for (CarSlot = 0; CarSlot < 10; CarSlot++)
				    if (AHouseData[HouseID][VehicleIDs][CarSlot] != 0)
				        HouseHasCars = true;

				// Check if the house has any cars assigned to it
				if (HouseHasCars == true)
				{
					// Add all vehicles to the list
					for (CarSlot = 0; CarSlot < 10; CarSlot++)
					{
						if (AHouseData[HouseID][VehicleIDs][CarSlot] != 0)
						{
						    // Get the index where the first vehicle is found in the ABuyableVehicles array
						    BuyableCarIndex = VehicleBuyable_GetIndex(GetVehicleModel(AHouseData[HouseID][VehicleIDs][CarSlot]));
						    // Add the name of the vehicle to the list
						    format(VehicleList, sizeof(VehicleList), "%s{00FF00}%s: $%i{FFFFFF}\n", VehicleList, ABuyableVehicles[BuyableCarIndex][CarName], ABuyableVehicles[BuyableCarIndex][VehiclePrice] / 2);
						}
						else
							format(VehicleList, sizeof(VehicleList), TXT_EmptyCarSlot, VehicleList);
					}

					// Ask which vehicle class the player wants to see to buy a vehicle
					ShowPlayerDialog(playerid, DialogSellCar, DIALOG_STYLE_LIST, TXT_SelectVehicleToSell, VehicleList, TXT_DialogButtonSelect, TXT_DialogButtonCancel);
				}
				else
				    SendClientMessage(playerid, COLOR_WHITE, TXT_NoHouseVehicles);
			}
			else
			    SendClientMessage(playerid, COLOR_RED, "Only the house-owner can use this option");
		}
	    case 5: // Open/Close the house to the public (everyone can enter it)
	    {
			// Only the house-owner can use this option
			if (House_PlayerIsOwner(playerid, HouseID) != 0)
			{
				// Check the if the house is closed to the public
				if (AHouseData[HouseID][HouseOpened] != true) {
					// Set the status to open
					AHouseData[HouseID][HouseOpened] = true;
					// ... and inform the owner about it
					SendClientMessage(playerid, COLOR_WHITE, TXT_HouseOpenedForEveryone);
				} else {
					// Otherwise set the status to closed
					AHouseData[HouseID][HouseOpened] = false;
					// ... and inform the owner about it
					SendClientMessage(playerid, COLOR_WHITE, TXT_HouseClosedForEveryone);
				}
			}
			else
			    SendClientMessage(playerid, COLOR_RED, "Only the house-owner can use this option");
	    }
		case 6: // Sell house
		{
			// Only the house-owner can use this option
			if (House_PlayerIsOwner(playerid, HouseID) != 0)
			{
			    // Setup local variables
			    new bool:HouseHasCars = false;

				// Check if the house has any cars assigned to it
				for (CarSlot = 0; CarSlot < 10; CarSlot++)
				    if (AHouseData[HouseID][VehicleIDs][CarSlot] != 0)
				        HouseHasCars = true;

				// Check if all house-cars have been sold (all slots are empty)
				if (HouseHasCars == false)
				{
				    format(Msg, sizeof(Msg), TXT_SureSellHouse, House_CalcSellPrice(HouseID));
					ShowPlayerDialog(playerid, DialogSellHouse, DIALOG_STYLE_MSGBOX, TXT_AreYouSure, Msg, TXT_DialogButtonYes, TXT_DialogButtonNo);
				}
				else
				    SendClientMessage(playerid, COLOR_WHITE, TXT_CannotSellHouseWithCars);
			}
			else
			    SendClientMessage(playerid, COLOR_RED, "Only the house-owner can use this option");
		}
	}

	return 1;
}

// This function processes the house-upgrade menu
Dialog_UpgradeHouse(playerid, response, listitem)
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Get the HouseID of the house where the player is
	new HouseID = APlayerData[playerid][CurrentHouse];
	// Calculate the new house-level based on the selection in the list (the first entry is 1 level higher than the current house-level)
	new hLevel = listitem + 1 + AHouseData[HouseID][HouseLevel];
	// Calculate the price for upgrading
	new Payment = ((AHouseData[HouseID][HousePrice] * (listitem + 1)) / 100) * HouseUpgradePercent;

	// Check if the player can afford it
	if (APlayerData[playerid][PlayerMoney] >= Payment)
	{
		// Upgrade the house
		AHouseData[HouseID][HouseLevel] = hLevel;
		// Set the worldid so other players cannot see him anymore
		SetPlayerVirtualWorld(playerid, 1000 + playerid);
		// Set the player inside the new interior of the house
		SetPlayerInterior(playerid, AHouseInteriors[hLevel][InteriorID]);
		// Set the position of the player at the spawn-location of the house's interior
		SetPlayerPos(playerid, AHouseInteriors[hLevel][IntX], AHouseInteriors[hLevel][IntY], AHouseInteriors[hLevel][IntZ]);

		// Let the player pay for the upgrade
		new Msg[128];
		RewardPlayer(playerid, -Payment, 0);
		format(Msg, sizeof(Msg), TXT_PlayerUpgradedHouse, AHouseData[HouseID][HouseLevel], Payment);
		SendClientMessage(playerid, COLOR_WHITE, Msg);

		// Also update the 3DText at the entrance of the house
		House_UpdateEntrance(APlayerData[playerid][CurrentHouse]);

		// Save the player-file (and his houses)
		PlayerFile_Save(playerid);
	}
	else
	    SendClientMessage(playerid, COLOR_WHITE, TXT_CannotAffordUpgrade);

	return 1;
}



// Check which house the player chose, and port him to it
Dialog_GoHome(playerid, response, inputtext[])
{
	// Setup local variables
	new HouseID, ListItemString[100];

	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	sscanf(inputtext, "s[100]i", ListItemString, HouseID);

	// Check if this is a valid house (HouseID != 0)
	if (HouseID != 0)
	{
		// Get the coordinates of the house's entrance
		SetPlayerPos(playerid, AHouseData[HouseID][HouseX], AHouseData[HouseID][HouseY], AHouseData[HouseID][HouseZ]);
	}
	else
	    SendClientMessage(playerid, COLOR_WHITE, TXT_NoHouseInSlot);

	return 1;
}

// Let the player change the name of his house
Dialog_ChangeHouseName(playerid, response, inputtext[])
{
	// Just close the dialog if the player clicked "Cancel" or if the player didn't input any text
	if ((!response) || (strlen(inputtext) == 0)) return 1;

	// Change the name of the house
	format(AHouseData[APlayerData[playerid][CurrentHouse]][HouseName], 100, inputtext);
	// Also update the 3DText at the entrance of the house
	House_UpdateEntrance(APlayerData[playerid][CurrentHouse]);
	// Let the player know that the name of his house has been changed
	SendClientMessage(playerid, COLOR_WHITE, TXT_ChangedHouseName);

	// Save the player-file (and his houses)
	PlayerFile_Save(playerid);

	return 1;
}

// Choose a vehicle class from which to buy a vehicle
Dialog_BuyCarClass(playerid, response, listitem)
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Setup local variables
	new CarList[1000], DialogTitle[128];

	// Set the player's chosen vehicle-class based on the listitem he chose (add 1 as the vehicleclass starts at 1)
	APlayerData[playerid][DialogBuyVClass] = listitem + 1;

	// Add all vehicles of the same class to the list
	for (new i; i < sizeof(ABuyableVehicles); i++)
	{
		// Check if the vehicle in the list has the same class as requested
		if (ABuyableVehicles[i][VehicleClass] == APlayerData[playerid][DialogBuyVClass])
		{
			// Add the carname to the list and it's price
			format(CarList, sizeof(CarList), "%s%s%s ($%i)", CarList, "\n", ABuyableVehicles[i][CarName], ABuyableVehicles[i][VehiclePrice]); // Add the name of the next car to the list on the next line
		}
	}

	// Check if the list is empty
	if (strlen(CarList) == 0)
	{
		// Send the player a message that all vehicles have been disabled of the chosen class (no vehicles in the array of this class)
		SendClientMessage(playerid, COLOR_WHITE, TXT_BuyRentVehicleEmptyList);
		// Exit the function (don't ask to choose a vehicle)
		return 1;
	}

	// Set a title for the dialog based on the requested vehicleclass
	switch (APlayerData[playerid][DialogBuyVClass])
	{
		case VClassBike: format(DialogTitle, sizeof(DialogTitle), TXT_BuyBike);
		case VClassBoat: format(DialogTitle, sizeof(DialogTitle), TXT_BuyBoat);
		case VClassConvertible: format(DialogTitle, sizeof(DialogTitle), TXT_BuyConvertible);
		case VClassHelicopter: format(DialogTitle, sizeof(DialogTitle), TXT_BuyHelicopter);
		case VClassIndustrial: format(DialogTitle, sizeof(DialogTitle), TXT_BuyIndustrial);
		case VClassLowRider: format(DialogTitle, sizeof(DialogTitle), TXT_BuyLowRider);
		case VClassOffRoad: format(DialogTitle, sizeof(DialogTitle), TXT_BuyOffRoad);
		case VClassPlane: format(DialogTitle, sizeof(DialogTitle), TXT_BuyPlane);
		case VClassPublic: format(DialogTitle, sizeof(DialogTitle), TXT_BuyPublicService);
		case VClassRCVehicle: format(DialogTitle, sizeof(DialogTitle), TXT_BuyRC);
		case VClassSaloons: format(DialogTitle, sizeof(DialogTitle), TXT_BuySaloon);
		case VClassSportCar: format(DialogTitle, sizeof(DialogTitle), TXT_BuySport);
		case VClassStationCar: format(DialogTitle, sizeof(DialogTitle), TXT_BuyStationWagon);
		case VClassTrailer: format(DialogTitle, sizeof(DialogTitle), TXT_BuyTrailer);
		case VClassUnique: format(DialogTitle, sizeof(DialogTitle), TXT_BuyUnique);
	}

	// Ask which car the player wants to have by showing the dialog
	ShowPlayerDialog(playerid, DialogBuyCar, DIALOG_STYLE_LIST, DialogTitle, CarList, TXT_DialogButtonSelect, TXT_DialogButtonCancel);

	return 1;
}

// Buy a vehicle and assign it to the house
Dialog_BuyCar(playerid, response, listitem)
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Setup local variables
	new Counter, Msg[128], cComponents[14];

	// Get the HouseID
	new HouseID = APlayerData[playerid][CurrentHouse];

	// Loop through the entire list of buyable vehicles
	for (new i; i < sizeof(ABuyableVehicles); i++)
	{
		// Check if the vehicle in the list has the same class as requested
		if (ABuyableVehicles[i][VehicleClass] == APlayerData[playerid][DialogBuyVClass])
		{
		    // Check if this is the vehicle that the player chose
		    if (Counter == listitem)
			{
			    // Check if the player has enough money to afford buying this vehicle
			    if (APlayerData[playerid][PlayerMoney] >= (ABuyableVehicles[i][VehiclePrice]))
			    {
					// Add the vehicle to the house (this sets ownership and other data that's needed)
					// The vehicles are by default created inside the hangar of KACC Military Fuels instead of near the EasterBoard Farm
					House_AddVehicle(HouseID, ABuyableVehicles[i][CarModel], 0, cComponents, 2585.0, 2829.0, 10.9, 0.0, -1, -1);

					// Let the player pay for buying the vehicle
					APlayerData[playerid][PlayerMoney] = APlayerData[playerid][PlayerMoney] - ABuyableVehicles[i][VehiclePrice];
					// Let the player know he bought a vehicle
					format(Msg, sizeof(Msg), TXT_PlayerBoughtVehicle, ABuyableVehicles[i][CarName], ABuyableVehicles[i][VehiclePrice]);
					SendClientMessage(playerid, COLOR_WHITE, Msg);
					SendClientMessage(playerid, COLOR_WHITE, TXT_PlayerMustUseGetVehicleAndPark);

					// Save the player-file (and his houses)
					PlayerFile_Save(playerid);
				}
				else // The player has not enough money to buy this vehicle
				    SendClientMessage(playerid, COLOR_WHITE, TXT_CannotAffordVehicle);

				// Stop looking for the vehicle, as it's found and given to the player already
				return 1;
			}
		    else // The player chose another vehicle from the same class, keep looking for another one
		        Counter++;
		}
	}

	return 1;
}

// Buy insurance for the house's vehicles
Dialog_BuyInsurance(playerid, response)
{
	// Just close the dialog if the player clicked "No"
	if(!response) return 1;

	// Setup local variables
	new Msg[128];

	// Get the HouseID where the player is right now
	new HouseID = APlayerData[playerid][CurrentHouse];

	// Buy an insurance for this house's vehicles (insurance costs 10% of the house's baseprice)
	AHouseData[HouseID][Insurance] = 1;
	RewardPlayer(playerid, -(AHouseData[HouseID][HousePrice] / 10), 0);

	// Let the player know he has bought a vehicle-insurance for this house
	format(Msg, sizeof(Msg), TXT_PlayerBoughtInsurance, AHouseData[HouseID][HousePrice] / 10);
	SendClientMessage(playerid, COLOR_WHITE, Msg);

	// Save the player-file (and his houses)
	PlayerFile_Save(playerid);
	// Also save the sold house, otherwise the old ownership-data is still there
	HouseFile_Save(HouseID);

	return 1;
}

// Sell the house
Dialog_SellHouse(playerid, response)
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Get the HouseID where the player is right now
	new HouseID = APlayerData[playerid][CurrentHouse];

	// Set the player in the normal world again
	SetPlayerVirtualWorld(playerid, 0);
	SetPlayerInterior(playerid, 0);
	// Set the position of the player at the entrance of his house
	SetPlayerPos(playerid, AHouseData[HouseID][HouseX], AHouseData[HouseID][HouseY], AHouseData[HouseID][HouseZ]);

	// Also clear the tracking-variable to track in which house the player is
	APlayerData[playerid][CurrentHouse] = 0;

	// Clear the owner of the house
	AHouseData[HouseID][Owned] = false;
	AHouseData[HouseID][Owner] = 0;
	// Clear the house-name and house-level
	AHouseData[HouseID][HouseName] = 0;
	AHouseData[HouseID][HouseLevel] = 1;

	// Refund the player 50% of the worth of the house
	RewardPlayer(playerid, House_CalcSellPrice(HouseID), 0);
	SendClientMessage(playerid, COLOR_WHITE, TXT_PlayerSoldHouse);

	// Clear the house-id from the player
	for (new i; i < MAX_HOUSESPERPLAYER; i++)
	{
		// If the house-slot if found where the house was added to the player
		if (APlayerData[playerid][Houses][i] == HouseID)
		{
		    // Clear the house-id
		    APlayerData[playerid][Houses][i] = 0;
		    // Stop searching
		    break;
		}
	}

	// Update the 3DText near the house's entrance to show other players that it's for sale again
	House_UpdateEntrance(HouseID);

	// Save the player-file (and his houses)
	PlayerFile_Save(playerid);
	// Also save the sold house, otherwise the old ownership-data is still there
	HouseFile_Save(HouseID);

	return 1;
}



// Sell the house
Dialog_SellCar(playerid, response, listitem)
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Setup local variables
	new HouseID = APlayerData[playerid][CurrentHouse];
	new vid = AHouseData[HouseID][VehicleIDs][listitem];

	// Check if there is a vehicle stored in this carslot
	if (vid != 0)
	{
		new BuyableCarIndex = VehicleBuyable_GetIndex(GetVehicleModel(vid));
		new Msg[128];

		// Destroy the vehicle and remove it from the house
		DestroyVehicle(vid);
		AHouseData[HouseID][VehicleIDs][listitem] = 0;
		// Also clear the data
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
		DestroyDynamic3DTextLabel(AVehicleData[vid][VehicleText]);

		// Refund the player with 50% of the vehicle's buying price
		APlayerData[playerid][PlayerMoney] = APlayerData[playerid][PlayerMoney] + (ABuyableVehicles[BuyableCarIndex][VehiclePrice] / 2);
		// Let the player know about it
		format(Msg, sizeof(Msg), TXT_PlayerSoldVehicle, ABuyableVehicles[BuyableCarIndex][CarName], ABuyableVehicles[BuyableCarIndex][VehiclePrice] / 2);
		SendClientMessage(playerid, COLOR_WHITE, Msg);

		// Save the player-file (and his houses)
		PlayerFile_Save(playerid);
	}
	else
	    SendClientMessage(playerid, COLOR_WHITE, TXT_NoVehicleInSlot);

	return 1;
}

// This dialog processes the selected house from which to get a vehicle using /getvehicle
Dialog_GetVehiclesSelectHouse(playerid, response, inputtext[])
{
	// Setup local variables
	new HouseID, ListItemString[100];

	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	sscanf(inputtext, "s[100]i", ListItemString, HouseID);

	// Check if it was a valid house
	if (HouseID != 0)
	{
		// Setup local variables
	    new BuyableCarIndex, VehicleList[500], bool:HouseHasCars = false, CarSlot;

		// Store the HouseID, otherwise the next dialog won't be able to get a car from the chosen house
		APlayerData[playerid][DialogGetVehiclesHouseID] = HouseID;

		// Check if the house has any cars assigned to it
		for (CarSlot = 0; CarSlot < 10; CarSlot++)
			if (AHouseData[HouseID][VehicleIDs][CarSlot] != 0)
			    HouseHasCars = true;

		// Check if the house has any cars assigned to it
		if (HouseHasCars == true)
		{
			// Add all vehicles to the list
			for (CarSlot = 0; CarSlot < 10; CarSlot++)
			{
				// Check if the carslot has a vehicle in it
				if (AHouseData[HouseID][VehicleIDs][CarSlot] != 0)
				{
				    // Get the index where the first vehicle is found in the ABuyableVehicles array
				    BuyableCarIndex = VehicleBuyable_GetIndex(GetVehicleModel(AHouseData[HouseID][VehicleIDs][CarSlot]));
				    // Add the name of the vehicle to the list
					if (AVehicleData[AHouseData[HouseID][VehicleIDs][CarSlot]][Clamped] == true)
					    format(VehicleList, sizeof(VehicleList), "%s{00FF00}%s (clamped)\n", VehicleList, ABuyableVehicles[BuyableCarIndex][CarName]);
					else
					    format(VehicleList, sizeof(VehicleList), "%s{00FF00}%s\n", VehicleList, ABuyableVehicles[BuyableCarIndex][CarName]);
				}
				else
					format(VehicleList, sizeof(VehicleList), TXT_EmptyCarSlot, VehicleList);
			}

			// Ask which vehicle the player wants to teleport to his location
			ShowPlayerDialog(playerid, DialogGetVehiclesSelectVehicle, DIALOG_STYLE_LIST, TXT_SelectVehicleToGet, VehicleList, TXT_DialogButtonSelect, TXT_DialogButtonCancel);
		}
		else
		    SendClientMessage(playerid, COLOR_WHITE, TXT_NoHouseVehicles);
	}
	else
	    SendClientMessage(playerid, COLOR_WHITE, TXT_NoHouseInSlot);

	return 1;
}

// This dialog processes the chosen car and spawns it at the player's location
Dialog_GetVehiclesSelectVehicle(playerid, response, listitem)
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Get the HouseID from which to get the car
	new HouseID = APlayerData[playerid][DialogGetVehiclesHouseID];

	// Get the vehicleid from the chosen listitem
	new vid = AHouseData[HouseID][VehicleIDs][listitem];

	// Check if it was a valid vehicleid
	if (vid != 0)
	{
		// Check if the vehicle is clamped
		if (AVehicleData[vid][Clamped] == false)
		{
			// Setup local variables
			new Float:x, Float:y, Float:z, Float:Angle;
			// Get the player's position
			GetPlayerPos(playerid, x, y, z);
			GetPlayerFacingAngle(playerid, Angle);
			// Port the vehicle to the player
			SetVehiclePos(vid, x, y, z);
			// Put the player inside the vehicle and rotate the vehicle to face where the player was facing
			PutPlayerInVehicle(playerid, vid, 0);
			SetVehicleZAngle(vid, Angle);
			// Turn on the engine and lights
			new engine,lights,alarm,doors,bonnet,boot,objective;
			GetVehicleParamsEx(vid, engine, lights, alarm, doors, bonnet, boot, objective);
			SetVehicleParamsEx(vid, 1, 1, alarm, doors, bonnet, boot, objective);
			// Store the player's current location and interior-id, otherwise anti-airbreak hack code could kick you
			GetPlayerPos(playerid, APlayerData[playerid][PreviousX], APlayerData[playerid][PreviousY], APlayerData[playerid][PreviousZ]);
			APlayerData[playerid][PreviousInt] = GetPlayerInterior(playerid);
			// Let the player know he should park the vehicle
			SendClientMessage(playerid, COLOR_WHITE, TXT_PlayerMustUsePark);
		}
		else // Inform the player that this vehicle is clamped and how he can un-clamp it
		{
		    SendClientMessage(playerid, COLOR_WHITE, TXT_SpeedometerClampedVehicle);
		    SendClientMessage(playerid, COLOR_WHITE, TXT_SpeedometerClampedVehicle2);
		}
	}
	else
	    SendClientMessage(playerid, COLOR_WHITE, TXT_NoVehicleInSlot);

	return 1;
}



// This dialog processes the chosen business-type and creates the business
Dialog_CreateBusSelType(playerid, response, listitem)
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

    // Setup some local variables
	new BusType, BusID, Float:x, Float:y, Float:z, Msg[128], bool:EmptySlotFound = false;

	// Get the player's position
	GetPlayerPos(playerid, x, y, z);

	// Get the business-type from the option the player chose
	BusType = listitem + 1;

	// Find a free business-id
	for (BusID = 1; BusID < MAX_BUSINESS; BusID++)
	{
		// Check if this business ID is free
		if (ABusinessData[BusID][BusinessType] == 0)
		{
			EmptySlotFound = true;
		    break; // Stop processing
		}
	}

	// Check if an empty slot has been found
	if (EmptySlotFound == false)
	{
		// If no empty slot was found, let the player know about it and exit the function
		SendClientMessage(playerid, COLOR_RED, "Maximum number of businesses reached");
		return 1;
	}

	// Set some default data at the index of NextFreeBusinessID (NextFreeBusinessID will point to the next free business-index)
	ABusinessData[BusID][BusinessX] = x;
	ABusinessData[BusID][BusinessY] = y;
	ABusinessData[BusID][BusinessZ] = z;
	ABusinessData[BusID][BusinessType] = BusType;
	ABusinessData[BusID][BusinessLevel] = 1;
	ABusinessData[BusID][Owned] = false;

	// Add the pickup and 3DText at the location of the business-entrance (where the player is standing when he creates the business)
	Business_CreateEntrance(BusID);

	// Save the business
	BusinessFile_Save(BusID);

	// Inform the player that he created a new house
	format(Msg, sizeof(Msg), "{00FF00}You've succesfully created business {FF00FF}%i{00FF00}", BusID);
	SendClientMessage(playerid, COLOR_WHITE, Msg);

	return 1;
}

// This function processes the businessmenu dialog
Dialog_BusinessMenu(playerid, response, listitem)
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Setup local variables
	new BusID, BusType, Msg[128], DialogTitle[200], UpgradePrice;

	// Get the HouseID of the house where the player is
	BusID = APlayerData[playerid][CurrentBusiness];
	BusType = ABusinessData[BusID][BusinessType];

	// Select an option based on the selection in the list
	switch(listitem)
	{
	    case 0: // Change business name
	    {
	        format(DialogTitle, sizeof(DialogTitle), TXT_DialogOldBusinessName, ABusinessData[BusID][BusinessName]);
			ShowPlayerDialog(playerid, DialogBusinessNameChange, DIALOG_STYLE_INPUT, DialogTitle, TXT_DialogEnterNewBusinessName, TXT_DialogButtonSelect, TXT_DialogButtonCancel);
	    }
	    case 1: // Upgrade the business
	    {
	        // Check if it's possible to upgrade further
			if (ABusinessData[BusID][BusinessLevel] < 5)
			{
			    // Get the upgrade-price
			    UpgradePrice = ABusinessInteriors[BusType][BusPrice];
			    // Check if the player can afford the upgrade
				if (APlayerData[playerid][PlayerMoney] >= UpgradePrice)
				{
				    // Give the current earnings of the business to the player and update the LastTransaction time
					Business_PayEarnings(playerid, BusID);
					// Upgrade the business 1 level
				    ABusinessData[BusID][BusinessLevel]++;
					// Let the player pay for the upgrade
					RewardPlayer(playerid, -UpgradePrice, 0);
					// Update the 3DText near the business's entrance to show what level the business is
					Business_UpdateEntrance(BusID);
					// Let the player know about it
					format(Msg, sizeof(Msg), "{00FF00}You have upgraded your business to level {FFFF00}%i", ABusinessData[BusID][BusinessLevel]);
					SendClientMessage(playerid, COLOR_WHITE, Msg);
				}
				else
					SendClientMessage(playerid, COLOR_RED, "You cannot afford the upgrade");
			}
			else
			    SendClientMessage(playerid, COLOR_WHITE, TXT_BusinessReachedMaxLevel);
	    }
		case 2: // Retrieve business earnings
		{
		    // Give the current earnings of the business to the player and update the LastTransaction time
			Business_PayEarnings(playerid, BusID);
		}
		case 3: // Sell business
		{
		    format(Msg, sizeof(Msg), TXT_SureSellBusiness, (ABusinessInteriors[BusType][BusPrice] * ABusinessData[BusID][BusinessLevel]) / 2);
			ShowPlayerDialog(playerid, DialogSellBusiness, DIALOG_STYLE_MSGBOX, TXT_AreYouSure, Msg, TXT_DialogButtonYes, TXT_DialogButtonNo);
		}
	}

	return 1;
}

// This function processes the /gobus dialog
Dialog_GoBusiness(playerid, response, listitem)
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Setup local variables
	new BusIndex, BusID;

	// The listitem directly indicates the business-index
	BusIndex = listitem;
	BusID = APlayerData[playerid][Business][BusIndex];

	// Check if this is a valid business (BusID != 0)
	if (BusID != 0)
	{
		// Get the coordinates of the business's entrance
		SetPlayerPos(playerid, ABusinessData[BusID][BusinessX], ABusinessData[BusID][BusinessY], ABusinessData[BusID][BusinessZ]);
	}
	else
	    SendClientMessage(playerid, COLOR_WHITE, TXT_NoBusinessInSlot);

	return 1;
}

// Let the player change the name of his business
Dialog_ChangeBusinessName(playerid, response, inputtext[])
{
	// Just close the dialog if the player clicked "Cancel" or if the player didn't input any text
	if ((!response) || (strlen(inputtext) == 0)) return 1;

	// Change the name of the business
	format(ABusinessData[APlayerData[playerid][CurrentBusiness]][BusinessName], 100, inputtext);
	// Also update the 3DText at the entrance of the business
	Business_UpdateEntrance(APlayerData[playerid][CurrentBusiness]);
	// Let the player know that the name of his business has been changed
	SendClientMessage(playerid, COLOR_WHITE, TXT_ChangedBusinessName);

	// Save the player-file (and his houses and businesses)
	PlayerFile_Save(playerid);

	return 1;
}

// Sell the business
Dialog_SellBusiness(playerid, response)
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Get the BusinessID where the player is right now and the business-type
	new BusID = APlayerData[playerid][CurrentBusiness];
	new BusType = ABusinessData[BusID][BusinessType];

	// Set the player in the normal world again
	SetPlayerVirtualWorld(playerid, 0);
	SetPlayerInterior(playerid, 0);
	// Set the position of the player at the entrance of his business
	SetPlayerPos(playerid, ABusinessData[BusID][BusinessX], ABusinessData[BusID][BusinessY], ABusinessData[BusID][BusinessZ]);

	// Also clear the tracking-variable to track in which business the player is
	APlayerData[playerid][CurrentBusiness] = 0;

	// Clear the owner of the business
	ABusinessData[BusID][Owned] = false;
	ABusinessData[BusID][Owner] = 0;
	// Clear the house-name and business-level
	ABusinessData[BusID][BusinessName] = 0;
	ABusinessData[BusID][BusinessLevel] = 1;

	// Refund the player 50% of the worth of the business
	RewardPlayer(playerid, (ABusinessInteriors[BusType][BusPrice] * ABusinessData[BusID][BusinessLevel]) / 2, 0);
	SendClientMessage(playerid, COLOR_WHITE, TXT_PlayerSoldBusiness);

	// Clear the house-id from the player
	for (new i; i < MAX_BUSINESSPERPLAYER; i++)
	{
		// If the business-slot if found where the business was added to the player
		if (APlayerData[playerid][Business][i] == BusID)
		{
		    // Clear the business-id
		    APlayerData[playerid][Business][i] = 0;
		    // Stop searching
		    break;
		}
	}

	// Update the 3DText near the business's entrance to show other players that it's for sale again
	Business_UpdateEntrance(BusID);

	// Save the player-file (and his houses and businesses)
	PlayerFile_Save(playerid);
	// Also save the sold business, otherwise the old ownership-data is still there
	BusinessFile_Save(BusID);

	return 1;
}

// Process the commands dialog
Dialog_PlayerCommands(playerid, response, listitem)
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response)
	{
		// Reset the FirstCommand (so the list won't start at an unexpected location in the list, but back at the start of the list)
		APlayerData[playerid][DialogFirstItem] = 0;
		return 1; // Exit the function
	}

	// Process the selected listitem
	switch (listitem)
	{
		case 11: // Player chooses "Next..."
		{
		    // Add 5 to the FirstCommand, so the list will start further down the array (will show the next 5 commands)
		    APlayerData[playerid][DialogFirstItem] = APlayerData[playerid][DialogFirstItem] + 5;
		    // Create a new list of commands and show the dialog
		    CommandList_Create(playerid);
		}
	}

	return 1;
}

// Process the primary car color dialog
Dialog_PrimaryCarColor(playerid, response, listitem)
{
	// Setup local variables
	new Message[128];

	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Setup local variables
	new vid = GetPlayerVehicleID(playerid);

	// Save the primary car-color and apply the colors
	AVehicleData[vid][Color1] = listitem;
	ChangeVehicleColor(vid, AVehicleData[vid][Color1], AVehicleData[vid][Color2]);
	// If the primary color is black, remove the paintjob
	if (listitem == 0)
		AVehicleData[vid][PaintJob] = 0;

	// Let the player pay for changing the color
	RewardPlayer(playerid, -PRICE_COLOR_CHANGE, 0);
	format(Message, sizeof(Message), TXT_VehColorChangePaid, PRICE_COLOR_CHANGE);
	SendClientMessage(playerid, COLOR_GREEN, Message);

	return 1;
}

// Process the secundary car color dialog
Dialog_SecondaryCarColor(playerid, response, listitem)
{
	// Setup local variables
	new Message[128];

	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Setup local variables
	new vid = GetPlayerVehicleID(playerid);

	// Save the primary car-color and apply the colors
	AVehicleData[vid][Color2] = listitem;
	ChangeVehicleColor(vid, AVehicleData[vid][Color1], AVehicleData[vid][Color2]);

	// Let the player pay for changing the color
	RewardPlayer(playerid, -PRICE_COLOR_CHANGE, 0);
	format(Message, sizeof(Message), TXT_VehColorChangePaid, PRICE_COLOR_CHANGE);
	SendClientMessage(playerid, COLOR_GREEN, Message);

	return 1;
}

// Process the unclamp dialog
Dialog_UnclampVehicles(playerid, response)
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Setup local variables
	new HouseID, vid, ClampedVehicles, UnclampPrice, Msg[128];

	// Loop through all vehicles owned by this player
	for (new HouseSlot; HouseSlot < MAX_HOUSESPERPLAYER; HouseSlot++)
	{
		// Get the HouseID that exists on this index
		HouseID = APlayerData[playerid][Houses][HouseSlot];

		// Check if this house-slot is used
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

	// Calculate the price to unclamp all the player's vehicles
	UnclampPrice = ClampedVehicles * UnclampPricePerVehicle;

	// Check if the player has enough cash to unclamp his vehicles
	if (APlayerData[playerid][PlayerMoney] >= UnclampPrice)
	{
		// Loop through all vehicles owned by this player
		for (new HouseSlot; HouseSlot < MAX_HOUSESPERPLAYER; HouseSlot++)
		{
			// Get the HouseID that exists on this index
			HouseID = APlayerData[playerid][Houses][HouseSlot];

			// Check if this house-slot is used
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
						    AVehicleData[vid][Clamped] = false; // Unclamp the vehicle
					}
				}
			}
		}

		// Remove the money from the player
		RewardPlayer(playerid, -UnclampPrice, 0);

		// Let the player know his vehicles have been unclamped
		format(Msg, sizeof(Msg), "{00FF00}Your vehicles have been unclamped for {FFFF00}$%i", UnclampPrice);
		SendClientMessage(playerid, COLOR_WHITE, Msg);

		// Save the player's profile
		PlayerFile_Save(playerid);
	}
	else
	    SendClientMessage(playerid, COLOR_RED, "You don't have enough money to unclamp your vehicles");

	return 1;
}

// Process the rules dialog
Dialog_Rules(playerid, response)
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Check if the player accepted the rules already
	if (APlayerData[playerid][RulesRead] == false)
	{
		// Reward the player with $5000 for accepting the rules
		RewardPlayer(playerid, 5000, 0);
		// Save that the player has accepted the rules
		APlayerData[playerid][RulesRead] = true;

		// Let the player know he earned some money for accepting the rules
		SendClientMessage(playerid, COLOR_GREEN, "You have earned {FFFF00}$5000{00FF00} for accepting the rules");

		// Save the player's profile
		PlayerFile_Save(playerid);
	}

	return 1;
}



// Process the dialog that displays statistics of another player (accessed through the scoreboard)
Dialog_StatsOtherPlayer(playerid, response, listitem)
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Setup local variables
	new Name[24], DialogTitle[128], HouseList[1000], BusinessList[1000], BusID, BusType, Earnings, OtherPlayer, HouseID;

	// Get the ID of the other player
	OtherPlayer = APlayerData[playerid][DialogOtherPlayer];

	// If the player chose the line "Houses: xxx" or "Businesses: xxx", open a new dialog that displays them
	switch (listitem)
	{
		case 20: // Player chose to display the other player's houses
		{
			// Construct the dialog-title
			GetPlayerName(OtherPlayer, Name, sizeof(Name));
			format(DialogTitle, sizeof(DialogTitle), "Houses of player: %s", Name);

			// Add all house-names of the other player to the list
			for (new i; i < MAX_HOUSESPERPLAYER; i++)
			{
			    // Get the HouseID
			    HouseID = APlayerData[OtherPlayer][Houses][i];

				// Check if this houseindex is occupied
				if (HouseID != 0)
					format(HouseList, sizeof(HouseList), "%s{00FF00}%s{FFFFFF}\n", HouseList, AHouseData[HouseID][HouseName]);
				else
					format(HouseList, sizeof(HouseList), "%s{FFFFFF}%s{FFFFFF}\n", HouseList, "Empty house-slot");
			}

			// Display the dialog (displays all statistics about the house, such as level, bought vehicles, insurance, ...)
			ShowPlayerDialog(playerid, DialogStatsHouse, DIALOG_STYLE_LIST, DialogTitle, HouseList, TXT_DialogButtonSelect, TXT_DialogButtonCancel);
		}
		case 21: // Player chose to display the other player's businesses
		{
			// Construct the dialog-title
			GetPlayerName(OtherPlayer, Name, sizeof(Name));
			format(DialogTitle, sizeof(DialogTitle), "Businesses of player: %s", Name);

			// Ask to which business the player wants to port
			for (new i; i < MAX_BUSINESSPERPLAYER; i++)
			{
				// Get the business-id
			    BusID = APlayerData[OtherPlayer][Business][i];

				// Check if this businessindex is occupied
				if (BusID != 0)
				{
					// Get the business-type
					BusType = ABusinessData[BusID][BusinessType];
					Earnings = (BusinessTransactionTime - ABusinessData[BusID][LastTransaction]) * ABusinessInteriors[BusType][BusEarnings] * ABusinessData[BusID][BusinessLevel];
					format(BusinessList, sizeof(BusinessList), "%s{00FF00}%s{FFFFFF} (earnings: $%i)\n", BusinessList, ABusinessData[BusID][BusinessName], Earnings);
				}
				else
					format(BusinessList, sizeof(BusinessList), "%s{FFFFFF}%s{FFFFFF}\n", BusinessList, "Empty business-slot");
			}

			// Display the dialog (and allow the player to port there)
			ShowPlayerDialog(playerid, DialogStatsGoBusiness, DIALOG_STYLE_LIST, DialogTitle, BusinessList, "Goto", TXT_DialogButtonCancel);
		}
	}

	return 1;
}

// Process the chosen house from another player (displays the statistics of the chosen house)
Dialog_StatsHouse(playerid, response, listitem)
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Setup local variables
	new OtherPlayer, HouseID, HouseData[2000], BuyableCarIndex, DialogTitle[128], Name[24];

	// Get the other player and chosen HouseID
	OtherPlayer = APlayerData[playerid][DialogOtherPlayer];
	HouseID = APlayerData[OtherPlayer][Houses][listitem];

	// Construct the dialog-title
	GetPlayerName(OtherPlayer, Name, sizeof(Name));
	format(DialogTitle, sizeof(DialogTitle), "House of player: %s", Name);

	// Check if this is a valid HouseID
	if (HouseID != 0)
	{
		// Add all data about the house to the dialog
		format(HouseData, sizeof(HouseData), "%s{FFFFFF}Name: {00FF00}%s\n", HouseData, AHouseData[HouseID][HouseName]);
		if (AHouseData[HouseID][Insurance] == 1)
			format(HouseData, sizeof(HouseData), "%s{FFFFFF}Insurance: {00FF00}Yes\n", HouseData);
		else
			format(HouseData, sizeof(HouseData), "%s{FFFFFF}Insurance: {00FF00}No\n", HouseData);

		format(HouseData, sizeof(HouseData), "%s{FFFFFF}House-level: {00FF00}%i/%i\n", HouseData, AHouseData[HouseID][HouseLevel], AHouseData[HouseID][HouseMaxLevel]);
		format(HouseData, sizeof(HouseData), "%s{FFFFFF}House-price: {00FF00}%i\n \n", HouseData, AHouseData[HouseID][HousePrice]);
		format(HouseData, sizeof(HouseData), "%s{FFFFFF}Vehicles:\n", HouseData);

		// Add all vehicles to the list
		for (new CarSlot; CarSlot < 10; CarSlot++)
		{
			// Check if the carslot has a vehicle in it
			if (AHouseData[HouseID][VehicleIDs][CarSlot] != 0)
			{
			    // Get the index where the first vehicle is found in the ABuyableVehicles array
			    BuyableCarIndex = VehicleBuyable_GetIndex(GetVehicleModel(AHouseData[HouseID][VehicleIDs][CarSlot]));
			    // Add the name of the vehicle to the list
				if (AVehicleData[AHouseData[HouseID][VehicleIDs][CarSlot]][Clamped] == true)
				    format(HouseData, sizeof(HouseData), "%s{00FF00}%s (clamped)\n", HouseData, ABuyableVehicles[BuyableCarIndex][CarName]);
				else
				    format(HouseData, sizeof(HouseData), "%s{00FF00}%s\n", HouseData, ABuyableVehicles[BuyableCarIndex][CarName]);
			}
			else
				format(HouseData, sizeof(HouseData), TXT_EmptyCarSlot, HouseData);
		}

		// Store the HouseID that was chosen of the other player so the other dialogs can display the statistics further
		APlayerData[playerid][DialogOtherPlayerHouse] = HouseID;

		// Display the dialog (and allow the player to port there)
		ShowPlayerDialog(playerid, DialogStatsGoHouse, DIALOG_STYLE_LIST, DialogTitle, HouseData, "Goto", TXT_DialogButtonCancel);
	}
	else
	    SendClientMessage(playerid, COLOR_RED, "There is no house in this house-slot");

	return 1;
}

// Port to the house that was chosen by the player when viewing stats about a house and choosing "Goto"
Dialog_StatsGoHouse(playerid, response, listitem)
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Setup local variables
	new HouseID, CarSlot, Float:x, Float:y, Float:z;

	// Get the HouseID
	HouseID = APlayerData[playerid][DialogOtherPlayerHouse];

	switch (listitem)
	{
	    case 6, 7, 8, 9, 10: // One of the cars in the list
	    {
			// Calculate the CarSlot
			CarSlot = listitem - 6;

			// Check if the carslot has a car in it
			if (AHouseData[HouseID][VehicleIDs][CarSlot] != 0)
			{
				// Get the position of the vehicle
				GetVehiclePos(AHouseData[HouseID][VehicleIDs][CarSlot], x, y, z);
				// Port the player to the vehicle
				SetPlayerPos(playerid, x, y, z);
			}
			else
			    SendClientMessage(playerid, COLOR_RED, "There is no vehicle in this car-slot");
	    }
	    default: // Any other line
		{
			// Port the player to the house entrance
			SetPlayerPos(playerid, AHouseData[HouseID][HouseX], AHouseData[HouseID][HouseY], AHouseData[HouseID][HouseZ]);
		}
	}

	return 1;
}

// Process the chosen business from another player to port to
Dialog_StatsGoBusiness(playerid, response, listitem)
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Setup local variables
	new BusIndex, BusID;

	// The listitem directly indicates the business-index
	BusIndex = listitem;
	BusID = APlayerData[APlayerData[playerid][DialogOtherPlayer]][Business][BusIndex];

	// Check if this is a valid business (BusID != 0)
	if (BusID != 0)
	{
		// Get the coordinates of the business's entrance
		SetPlayerPos(playerid, ABusinessData[BusID][BusinessX], ABusinessData[BusID][BusinessY], ABusinessData[BusID][BusinessZ]);
	}
	else
	    SendClientMessage(playerid, COLOR_RED, "There is no business in this business-slot");

	return 1;
}



// Process the password to register a bank account
Dialog_BankPasswordRegister(playerid, response, inputtext[])
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Check if the player entered a valid password
	if ((strlen(inputtext) > 0) && (strlen(inputtext) < 20))
	{
		// Store the password
		format(APlayerData[playerid][BankPassword], 24, inputtext);
		// Save the bank account
		BankFile_Save(playerid);
		// Inform the player that he has succesfully registered a bank account
		SendClientMessage(playerid, COLOR_GREEN, "You've succesfully registered a bank account");
	}
	else
		SendClientMessage(playerid, COLOR_RED, "You need to enter a valid password to register a bank account");

	return 1;
}

// Process the password to login to your bank account
Dialog_BankPasswordLogin(playerid, response, inputtext[])
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Check if the player entered a valid password
	if ((strlen(inputtext) > 0) && (strlen(inputtext) < 20))
	{
		// Check if the player entered the correct password to login to his bank account
	    if (strcmp(APlayerData[playerid][BankPassword], inputtext, false) == 0)
		{
			// The player has logged in to his bank account
	       	APlayerData[playerid][BankLoggedIn] = true;
			// Inform the player that he has succesfully registered a bank account
			SendClientMessage(playerid, COLOR_GREEN, "You've succesfully logged in to your bank account");
			// Show the bank dialog
			ShowBankMenu(playerid);
		}
		else // The player entered the wrong password
			SendClientMessage(playerid, COLOR_RED, "You've entered the wrong password for logging in to your bank account");
	}
	else
		SendClientMessage(playerid, COLOR_RED, "You need to enter the correct password to login to your bank account");

	return 1;
}

// Process the options in the bank dialog
Dialog_BankOptions(playerid, response, listitem)
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Setup local variables
	new DialogMsg[500];

	// Check what to do based on which option is chosen
	switch (listitem)
	{
	    case 0: // Deposit funds in your bank account
		{
		    format(DialogMsg, sizeof(DialogMsg), "{FFFFFF}Your money: {00FF00}$%i\n", APlayerData[playerid][PlayerMoney]);
		    format(DialogMsg, sizeof(DialogMsg), "%s{FFFFFF}Bank account funds: {00FF00}$%i\n \n", DialogMsg, APlayerData[playerid][BankMoney]);
		    format(DialogMsg, sizeof(DialogMsg), "%s{FFFFFF}Please enter the amount of money you want to deposit into your bank account:", DialogMsg);
			ShowPlayerDialog(playerid, DialogBankDeposit, DIALOG_STYLE_INPUT, "Enter amount of money", DialogMsg, TXT_DialogButtonSelect, TXT_DialogButtonCancel);
		}
	    case 1: // Withdraw funds from your bank account
		{
		    // Check if you have any money in your bank account to withdraw from it
		    if (APlayerData[playerid][BankMoney] > 0)
		    {
			    format(DialogMsg, sizeof(DialogMsg), "{FFFFFF}Your money: {00FF00}$%i\n", APlayerData[playerid][PlayerMoney]);
			    format(DialogMsg, sizeof(DialogMsg), "%s{FFFFFF}Bank account funds: {00FF00}$%i\n \n", DialogMsg, APlayerData[playerid][BankMoney]);
			    format(DialogMsg, sizeof(DialogMsg), "%s{FFFFFF}Please enter the amount of money you want to withdraw from your bank account:", DialogMsg);
				ShowPlayerDialog(playerid, DialogBankWithdraw, DIALOG_STYLE_INPUT, "Enter amount of money", DialogMsg, TXT_DialogButtonSelect, TXT_DialogButtonCancel);
			}
			else
				SendClientMessage(playerid, COLOR_RED, "You can't withdraw money from an empty bank account");
		}
	    case 2: // Transfer funds from your bank account to another player's bank account
		{
		    // Check if you have any money in your bank account to transfer from it
		    if (APlayerData[playerid][BankMoney] > 0)
		    {
			    format(DialogMsg, sizeof(DialogMsg), "{FFFFFF}Your money: {00FF00}$%i\n", APlayerData[playerid][PlayerMoney]);
			    format(DialogMsg, sizeof(DialogMsg), "%s{FFFFFF}Bank account funds: {00FF00}$%i\n \n", DialogMsg, APlayerData[playerid][BankMoney]);
			    format(DialogMsg, sizeof(DialogMsg), "%s{FFFFFF}Please enter the amount of money you want to transfer to another player's bank account:", DialogMsg);
				ShowPlayerDialog(playerid, DialogBankTransferMoney, DIALOG_STYLE_INPUT, "Enter amount of money", DialogMsg, TXT_DialogButtonSelect, TXT_DialogButtonCancel);
			}
			else
				SendClientMessage(playerid, COLOR_RED, "You can't transfer money from an empty bank account");
		}
		case 3: // Cancel your bank account
		{
			if (APlayerData[playerid][BankMoney] > 0)
			{
			    format(DialogMsg, sizeof(DialogMsg), "{FFFFFF}Your bank account has {00FF00}$%i{FFFFFF} in it\n", APlayerData[playerid][BankMoney]);
			    format(DialogMsg, sizeof(DialogMsg), "%s{FFFFFF}Your bank funds will be returned to you when you cancel your bank account\n", DialogMsg, APlayerData[playerid][BankMoney]);
			}
		    format(DialogMsg, sizeof(DialogMsg), "%s{FFFFFF}Are you sure you want to cancel your bank account?", DialogMsg);

			// Ask the player if he's sure he wants to cancel his bank account
			ShowPlayerDialog(playerid, DialogBankCancel, DIALOG_STYLE_MSGBOX, TXT_AreYouSure, DialogMsg, TXT_DialogButtonYes, TXT_DialogButtonNo);
		}
	}

	return 1;
}

// Process the entered amount of money that needs to be deposited into the player's bank account
Dialog_BankDeposit(playerid, response, inputtext[])
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Setup local variables
	new DepositMoney, Msg[128];

	// Convert the entered text into an integer
	DepositMoney = strval(inputtext);

	// Check if the player entered a positive amount of money
	if (DepositMoney > 0)
	{
		// Check if the player has enough money
		if (DepositMoney <= APlayerData[playerid][PlayerMoney])
		{
			// Add the money to your bank account
	        APlayerData[playerid][BankMoney] = APlayerData[playerid][BankMoney] + DepositMoney;
			// Take the money away from yourself
			APlayerData[playerid][PlayerMoney] = APlayerData[playerid][PlayerMoney] - DepositMoney;
			// Save the player's account
			PlayerFile_Save(playerid);
			// Save the bank account
			BankFile_Save(playerid);
			// Inform the player that his money has been deposited
			format(Msg, sizeof(Msg), "{00FF00}You have deposited {FFFF00}$%i{00FF00} into your bank account", DepositMoney);
			SendClientMessage(playerid, COLOR_WHITE, Msg);
			// Show the bank dialog
			ShowBankMenu(playerid);
		}
		else
		    SendClientMessage(playerid, COLOR_RED, "You don't have that amount of money to deposit into your bank account");
	}
	else
	    SendClientMessage(playerid, COLOR_RED, "You need to enter a positive amount of money to deposit into your bank account");

	return 1;
}

// Process the entered amount of money that needs to be withdrawn from the player's bank account
Dialog_BankWithdraw(playerid, response, inputtext[])
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Setup local variables
	new WithdrawMoney, Msg[128];

	// Convert the entered text into an integer
	WithdrawMoney = strval(inputtext);

	// Check if the player entered a positive amount of money
	if (WithdrawMoney > 0)
	{
		// Check if the player has enough money in his bank account
		if (WithdrawMoney <= APlayerData[playerid][BankMoney])
		{
			// Withdraw the money from your bank account
	        APlayerData[playerid][BankMoney] = APlayerData[playerid][BankMoney] - WithdrawMoney;
			// Give the money to yourself
			APlayerData[playerid][PlayerMoney] = APlayerData[playerid][PlayerMoney] + WithdrawMoney;
			// Save the player's account
			PlayerFile_Save(playerid);
			// Save the bank account
			BankFile_Save(playerid);
			// Inform the player that his money has been withdrawn
			format(Msg, sizeof(Msg), "{00FF00}You have withdrawn {FFFF00}$%i{00FF00} from your bank account", WithdrawMoney);
			SendClientMessage(playerid, COLOR_WHITE, Msg);
			// Show the bank dialog
			ShowBankMenu(playerid);
		}
		else
		    SendClientMessage(playerid, COLOR_RED, "You don't have that amount of money in your bank account to withdraw");
	}
	else
	    SendClientMessage(playerid, COLOR_RED, "You need to enter a positive amount of money to withdraw from your bank account");

	return 1;
}

// Process the transfer option (player entered the amount of money to transfer to another player)
Dialog_BankTransferMoney(playerid, response, inputtext[])
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Setup local variables
	new DialogMsg[500];

	// Check if there is a text entered
	if (strlen(inputtext) == 0)
	{
		// Let the player know he must enter a valid amount of money into the input field
	    SendClientMessage(playerid, COLOR_RED, "You need to enter a valid amount of money");
		return 1;
	}

	// Convert the entered text into an integer and store it
	APlayerData[playerid][UseMoney] = strval(inputtext);

	// Check if the player entered a valid amount of money to be transferred
	if (APlayerData[playerid][UseMoney] > 0)
	{
		// Check if the player has enough money in his bank account to transfer it to another player's account
		if (APlayerData[playerid][UseMoney] <= APlayerData[playerid][BankMoney])
		{
			// Ask the player to which account he wants to transfer the money
		    format(DialogMsg, sizeof(DialogMsg), "{FFFFFF}Your money: {00FF00}$%i\n", APlayerData[playerid][PlayerMoney]);
		    format(DialogMsg, sizeof(DialogMsg), "%s{FFFFFF}Bank account funds: {00FF00}$%i\n", DialogMsg, APlayerData[playerid][BankMoney]);
		    format(DialogMsg, sizeof(DialogMsg), "%s{FFFFFF}Requested transfer amount: {00FF00}$%i\n \n", DialogMsg, APlayerData[playerid][UseMoney]);
		    format(DialogMsg, sizeof(DialogMsg), "%s{FFFFFF}Please enter the name of the player who must receive your money-transfer:", DialogMsg);
			ShowPlayerDialog(playerid, DialogBankTransferName, DIALOG_STYLE_INPUT, "Enter player name", DialogMsg, TXT_DialogButtonSelect, TXT_DialogButtonCancel);
		}
		else
		    SendClientMessage(playerid, COLOR_RED, "You don't have that amount of money in your bank account to transfer to another player's account");
	}
	else
	    SendClientMessage(playerid, COLOR_RED, "You need to enter a positive amount of money to transfer money to another player's account");

	return 1;
}

// Process the transfer option (player entered the name of the other player who must receive the money)
Dialog_BankTransferName(playerid, response, inputtext[])
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Setup local variables
	new file[100], File:BFile, LineForFile[100], ReceiverName[24], SenderName[24], Msg[128];
	new OtherPlayer, Day, Month, Year, Hour, Minute, Second, sDay[3], sMonth[3], sYear[5], sHour[3], sMinute[3];

    // Get the playername of the sender (the player who executes the transfer)
	format(SenderName, sizeof(SenderName), APlayerData[playerid][PlayerName]);

	// Check if there is a text entered
	if (strlen(inputtext) == 0)
	{
		// Let the player know he must enter a playername or playerid into the input field
	    SendClientMessage(playerid, COLOR_RED, "You need to enter a playername OR id");
		return 1;
	}

	// Try to convert the given playername OR playerid to a playerid
	sscanf(inputtext, "u", OtherPlayer);

	// Check if an online player was found by sscanf
	if (OtherPlayer != INVALID_PLAYER_ID)
	{
		// Check if that other player is online (use online bank-transfer)
		if (APlayerData[OtherPlayer][LoggedIn] == true)
		{
			format(ReceiverName, sizeof(ReceiverName), APlayerData[OtherPlayer][PlayerName]); // Get the playername of the receiver
			format(file, sizeof(file), BankFile, ReceiverName); // Construct the complete filename for the receiver's bank-account

			// Check if the receiver has a bank account
			if (fexist(file))
			{
			    // Transfer the money from your bank account to the other player's bank account
			    APlayerData[OtherPlayer][BankMoney] = APlayerData[OtherPlayer][BankMoney] + APlayerData[playerid][UseMoney];
				// Take the money away from your own bank account
				APlayerData[playerid][BankMoney] = APlayerData[playerid][BankMoney] - APlayerData[playerid][UseMoney];
				// Save both bank accounts
				BankFile_Save(OtherPlayer);
				BankFile_Save(playerid);
				// Inform the receiver that money was transferred to his bank account
				format(Msg, sizeof(Msg), "{00FF00}Your bank account has received a transfer of {FFFF00}$%i{00FF00} from {FFFF00}%s", APlayerData[playerid][UseMoney], SenderName);
				SendClientMessage(OtherPlayer, COLOR_WHITE, Msg);
				// Inform the sender that money was transferred from his bank account to the receiver
				format(Msg, sizeof(Msg), "{00FF00}You have transferred {FFFF00}$%i{00FF00} to {FFFF00}%s{00FF00}'s bank account", APlayerData[playerid][UseMoney], ReceiverName);
				SendClientMessage(playerid, COLOR_WHITE, Msg);
				// Exit the function (bank transfer was completed), skipping offline transfer
				return 1;
			}
			else // The target player is online, but has no bank account
			{
				format(Msg, sizeof(Msg), "{FF0000}Player {FFFF00}%s{FF0000} is online, but has no bank account, transfer failed", inputtext);
			    SendClientMessage(playerid, COLOR_WHITE, Msg);
				// Exit the function (bank transfer failed as the target player has no bank account), skipping offline transfer
				return 1;
			}
		}
		else
		{
			// An online player was found here, but he hasn't logged in yet, so treat him as an offline player
			// The rest of the function will be executes to process offline bank transfers
		}
	}

	// The player might be offline or doesn't exist (no player account with that name), so try to use an offline bank-transfer
	// This part of the code is executed because sscanf failed to find an online player with the given name
	// or the player is connected to the server, but hasn't logged in yet (didn't type his password already), this is treated
	// as being offline

	// Construct the complete filename for the receiver's bank-account
	format(file, sizeof(file), BankFile, inputtext);

	// Check if the receiver has a bank account
	if (fexist(file))
	{
	    // Open the file
		BFile = fopen(file, io_append); // Open the receiver's bank account for appending data to it

		fwrite(BFile, "[Transfer]\r\n");
		format(LineForFile, sizeof(LineForFile), "TransferMoney %i\r\n", APlayerData[playerid][UseMoney]); // Construct the line: "TransferMoney <TransferMoney>"
		fwrite(BFile, LineForFile); // And save it to the file
		format(LineForFile, sizeof(LineForFile), "TransferName %s\r\n", SenderName); // Construct the line: "TransferName <SenderName>"
		fwrite(BFile, LineForFile); // And save it to the file
		// Get the current date and time
		getdate(Year, Month, Day);
		gettime(Hour, Minute, Second);

		// Make sure the Day is 2 characters long
		if (Day < 10) format(sDay, sizeof(sDay), "0%i", Day);
		else format(sDay, sizeof(sDay), "%i", Day);
		// Make sure the Month is 2 characters long
		if (Month < 10) format(sMonth, sizeof(sMonth), "0%i", Month);
		else format(sMonth, sizeof(sMonth), "%i", Month);
		// Convert the Year into a string
	    format(sYear, sizeof(sYear), "%i", Year);
		// Make sure the Hour is 2 characters long
		if (Hour < 10) format(sHour, sizeof(sHour), "0%i", Hour);
		else format(sHour, sizeof(sHour), "%i", Hour);
		// Make sure the Minute is 2 characters long
		if (Minute < 10) format(sMinute, sizeof(sMinute), "0%i", Minute);
		else format(sMinute, sizeof(sMinute), "%i", Minute);

		format(LineForFile, sizeof(LineForFile), "TransferTime %s/%s/%s %s:%s\r\n", sDay, sMonth, sYear, sHour, sMinute); // Construct the line: "TransferTime <TransferTime>"
		fwrite(BFile, LineForFile); // And save it to the file
		fwrite(BFile, "[/Transfer]\r\n"); // Save the transfer-terminator string to the file

		fclose(BFile); // Close the file

		// Take the money away from your own bank account
		APlayerData[playerid][BankMoney] = APlayerData[playerid][BankMoney] - APlayerData[playerid][UseMoney];
		// Save your bank account
		BankFile_Save(playerid);

		// Inform the sender that the transfer was completed
		format(Msg, sizeof(Msg), "{00FF00}Player {FFFF00}%s{00FF00} is offline and has a bank account, transfer of {FFFF00}$%i{00FF00} was succesfull", inputtext, APlayerData[playerid][UseMoney]);
		SendClientMessage(playerid, COLOR_WHITE, Msg);
	}
	else
	{
		format(Msg, sizeof(Msg), "{FF0000}Account-name {FFFF00}%s{FF0000} not found, transfer failed", inputtext);
	    SendClientMessage(playerid, COLOR_WHITE, Msg);
	}

	return 1;
}

// Process the Bank Cancel option
Dialog_BankCancel(playerid, response)
{
	// Just close the dialog if the player clicked "No"
	if(!response) return 1;

	// Setup local variables
	new file[100], Name[24];

    // Get the playername
	format(Name, sizeof(Name), APlayerData[playerid][PlayerName]);
	// Construct the complete filename for this player's bank-account
	format(file, sizeof(file), BankFile, Name);

	// Check if the player's ban account exists (to be sure it's still there)
	if (fexist(file))
		fremove(file); // Delete the bank account

	// If there is any money left in the account, give the funds back to the player
	RewardPlayer(playerid, APlayerData[playerid][BankMoney], 0);
	// Inform the player about the remaining funds of your bank account
	if (APlayerData[playerid][BankMoney] > 0)
	    SendClientMessage(playerid, COLOR_GREEN, "There was still some money in your bank account, it has been given to your character");

	// Save the player's account
	PlayerFile_Save(playerid);

	// Clear the data from the player's account
	APlayerData[playerid][BankPassword] = 0;
	APlayerData[playerid][BankLoggedIn] = false;
	APlayerData[playerid][BankMoney] = 0;

	// Inform the player about it
	SendClientMessage(playerid, COLOR_GREEN, "Your bank account has been deleted");

	return 1;
}

// Process the chosen help-item
Dialog_HelpItemChosen(playerid, response, listitem)
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Setup local variables
	new DialogTitle[128];

	// Clear the dialog-message
	format(DialogMsg5000, sizeof(DialogMsg5000), "");

	// Set the dialog-title for the help-topic to be shown
	format(DialogTitle, sizeof(DialogTitle), AHelpTopics[listitem]);

	// Select the help-item and create a dialog that displays all text about that item
	for (new i; i < sizeof(AHelpText); i++)
	{
		// Check if this textline belongs to the help-item
		if (AHelpText[i][HelpID] == listitem)
		{
		    // Add all help-textlines to the list
			format(DialogMsg5000, sizeof(DialogMsg5000), "%s%s\n", DialogMsg5000, AHelpText[i][HelpText]);
		}
	}

	// Show the help-text in a dialog
	ShowPlayerDialog(playerid, DialogHelpItem, DIALOG_STYLE_MSGBOX, DialogTitle, DialogMsg5000, "Back", TXT_DialogButtonCancel);

	return 1;
}

// Process the help-item (select "Back" or "Cancel"
Dialog_HelpItem(playerid, response)
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// If the player chose "Back", re-open the helplist
	HelpList_Create(playerid);

	return 1;
}



// Process the old password of the player
Dialog_OldPassword(playerid, response, inputtext[])
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Check if the entered password is the same as the password saved in the player's account
    if (strcmp(APlayerData[playerid][PlayerPassword], inputtext, false) == 0)
	{
		// Ask the player to enter a new password
		ShowPlayerDialog(playerid, DialogNewPassword, DIALOG_STYLE_PASSWORD, "Enter new password:", "Enter your new password here:", TXT_DialogButtonOk, TXT_DialogButtonCancel);
	}
	else
	    SendClientMessage(playerid, COLOR_RED, "You've entered the wrong password");

	return 1;
}

// Process the new password of the player
Dialog_NewPassword(playerid, response, inputtext[])
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Setup local variables
	new Msg[256];

	// Check if the entered password is valid (not empty)
    if (strlen(inputtext) > 0)
	{
		// Check if the player didn't enter the same password
	    if (strcmp(APlayerData[playerid][PlayerPassword], inputtext, false) != 0)
		{
			// Store the new password
			format(APlayerData[playerid][PlayerNewPassword], 50, inputtext);
			// Construct the content of the dialog, which displays the old and new password
			format(Msg, sizeof(Msg), "{FFFFFF}Your old password: \t{00FF00}%s{FFFFFF}\n", APlayerData[playerid][PlayerPassword]);
			format(Msg, sizeof(Msg), "%sYour new password: \t{00FF00}%s{FFFFFF}\n \n", Msg, APlayerData[playerid][PlayerNewPassword]);
			format(Msg, sizeof(Msg), "%sAre you sure you want to change your password?", Msg);

			// Ask the player if he really wants to change his password
			ShowPlayerDialog(playerid, DialogConfirmPassword, DIALOG_STYLE_MSGBOX, "Are you sure?", Msg, "Yes", "No");
		}
		else
		    SendClientMessage(playerid, COLOR_RED, "Your new password cannot be the same as the old password");
	}
	else
	    SendClientMessage(playerid, COLOR_RED, "Invalid password");

	return 1;
}

// Process the dialog where the player needs to confirm the password-change
Dialog_ConfirmPassword(playerid, response)
{
	// Just close the dialog if the player clicked "Cancel"
	if(!response) return 1;

	// Copy the new password to the old one, overwriting the old one
	format(APlayerData[playerid][PlayerPassword], 50, APlayerData[playerid][PlayerNewPassword]);
	// Clear the new password
	format(APlayerData[playerid][PlayerNewPassword], 50, "");

	// Let the player know his password has been changed
    SendClientMessage(playerid, COLOR_GREEN, "Your password has been changed");

	// Save the player's account
	PlayerFile_Save(playerid);


	return 1;
}

