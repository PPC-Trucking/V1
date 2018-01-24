// Setup a custom type that holds all data about a bus route
enum TBusRoute
{
	HomeDepot, // The ID of the home-depot
	LineNumber, // The number of the busroute
	Score, // Determines the score the player gets when he reaches the end of the busroute
	RouteDescription[100], // The description of the busroute
	Locations[30] // The Location-IDs where the player must load/unload passengers (up to 30 busstops per route)
}

// Setup an array that holds all busroute data
new ABusRoutes[][TBusRoute] =
{
	// HomeDepot, LineNumber, array of BusLocations (location "-1" states the end of the route -> start over from the first location)
	{64, 101, 2, "SomeStart1 - SomeStop1", {65, 67, 94, 99, 69, 75, 76, 65, -1}},
	{64, 102, 1, "SomeStart2 - SomeStop2", {94, 96, 99, 100, 71, 94, -1}},
	{64, 201, 3, "SomeStart3 - SomeStop3", {92, 93, 91, 89, 88, 86, 84, 85, 79, 73, 72, 92, -1}},
	{64, 301, 2, "Las Barrancas - Bayside", {108, 107, 105, 103, 104, 108, -1}},
	{64, 302, 2, "Bayside - Las Payasdas", {103, 104, 106, 109, 105, 103, -1}},
	{64, 303, 2, "Fort Carson - El Quebrados", {112, 108, 105, 106, 108, 111, 112, -1}},
	{64, 401, 2, "Palomino Creek - Dillimore", {116, 117, 120, 118, 115, 116, -1}}
};



// Forward the function to Load or Unload passengers during missions (used by a timer)
forward BusDriver_LoadUnload(playerid, PassengersOnBusStop);



// This function is called when a busdriver enters a checkpoint
Bus_EnterRaceCheckpoint(playerid)
{
	// Check if the player is inside his vehicle while entering a checkpoint
	if (GetPlayerVehicleID(playerid) == APlayerData[playerid][VehicleID])
	{
		// Setup local variables
		new LineNr, Description[100], PassengersOnBus, PassengersOnBusStop, Job, RouteText[255];

		// Show that the player is loading and unloading passengers
	//	GameTextForPlayer(playerid, "Loading/unloading passengers... Please wait", 5000, 4);

		// Disable the player's actions (he cannot move anymore)
		TogglePlayerControllable(playerid, 0);

		// Get the JobID
		Job = APlayerData[playerid][JobID];
		// Get the data to construct the textdraw
		LineNr = ABusRoutes[Job][LineNumber];
		format(Description, sizeof(Description), "%s", ABusRoutes[Job][RouteDescription]);
		PassengersOnBus = APlayerData[playerid][Passengers];
		// Determine a random number of passengers on the busstop (between 10 and 30 passengers can be waiting at the busstop)
		PassengersOnBusStop = random(20) + 10;
		// Limit the number of passengers to 100
	//	if ((APlayerData[playerid][Passengers] + PassengersOnBusStop) > 100)
	//	    PassengersOnBusStop = 100 - APlayerData[playerid][Passengers];

		// Combine it all into a string for the TextDraw (the player can see this all the time) to describe the mission
		format(RouteText, sizeof(RouteText), TXT_BusDriverBusStopInfo, LineNr, Description, PassengersOnBus, PassengersOnBusStop);
		// Set the TextDraw so the player can see it
		TextDrawSetString(APlayerData[playerid][MissionText], RouteText);

		// Start a timer (Public function "BusDriver_LoadUnload(playerid)" gets called when the timer runs out)
		APlayerData[playerid][LoadingTimer] = SetTimerEx("BusDriver_LoadUnload", 5000, false, "di" , playerid, PassengersOnBusStop);
	}

	return 1;
}

// After a truckdriver entered a checkpoint, a timer is created. This function is called when the timer runs out
public BusDriver_LoadUnload(playerid, PassengersOnBusStop)
{
    // Setup local variables
	new RouteText[255], Float:x, Float:y, Float:z, Job, NextLoc, NextStep, Depot, Msg[128];
	new PassengersOnBus, PassengersGettingOff, LineNr, Description[100], Payment;
	new Float:xn, Float: yn, Float:zn;

	// Also delete any race-checkpoint (at the location of the homedepot)
	DisablePlayerCheckpoint(playerid);
	// Delete the checkpoint where the player just loaded/unloaded passengers
	DisablePlayerRaceCheckpoint(playerid);

	// Get the JobID
	Job = APlayerData[playerid][JobID];
	// Select the next location (jobstep)
	APlayerData[playerid][JobStep]++;
	NextStep = APlayerData[playerid][JobStep];
	// Determine a random number of passengers leaving the bus and let them leave (if there are any)
	if (APlayerData[playerid][Passengers] > 0)
		PassengersGettingOff = random(APlayerData[playerid][Passengers]);
	APlayerData[playerid][Passengers] = APlayerData[playerid][Passengers] - PassengersGettingOff;
	// Also let the passengers, that are waiting at the busstop, get on the bus
	APlayerData[playerid][Passengers] = APlayerData[playerid][Passengers] + PassengersOnBusStop;

	// Grab the next locationID
	NextLoc = ABusRoutes[Job][Locations][NextStep];
	// If the end of the route is reached, restart it from location 0 (also add a racecheckpoint at the busdepot to end the mission)
	if (NextLoc == -1)
	{
	    // Setup local variables
		new MissionMsg[128], Name[24];

		// Get the player name
		GetPlayerName(playerid, Name, sizeof(Name));
		// Construct the message sent to all players that this player completed a mafia mission
		format(MissionMsg, sizeof(MissionMsg), TXT_PlayerCompletedBusLine, Name, ABusRoutes[Job][LineNumber]);
		SendClientMessageToAll(COLOR_WHITE, MissionMsg);



	    // Start the route again from the first location
	    NextStep = 1;
	    APlayerData[playerid][JobStep] = NextStep;
		// Grab the locationID of this busstop
		NextLoc = ABusRoutes[Job][Locations][NextStep];

		// Get the homedepot locationID
		Depot = ABusRoutes[Job][HomeDepot];
		// Grab the coordinates of the homedepot
		x = ALocations[Depot][LocX];
		y = ALocations[Depot][LocY];
		z = ALocations[Depot][LocZ];
		// Create a checkpoint at the coordinates of the homedepot
		SetPlayerCheckpoint(playerid, x, y, z, 7);
		// Reward the player with a score for completing a busroute
		RewardPlayer(playerid, 0, ABusRoutes[Job][Score]);

		// Increase the stats for completing a bus-route
		APlayerData[playerid][StatsBusDriverJobs]++;
		// Also save the data (in case the server crashes, progress would be lost)
		PlayerFile_Save(playerid);
	}

	// Get the data to construct the textdraw
	LineNr = ABusRoutes[Job][LineNumber];
	format(Description, sizeof(Description), "%s", ABusRoutes[Job][RouteDescription]);
	PassengersOnBus = APlayerData[playerid][Passengers];
	// Combine it all into a string for the TextDraw (the player can see this all the time) to describe the mission
	format(RouteText, sizeof(RouteText), TXT_BusDriverJobInfo, LineNr, Description, PassengersOnBus);
	// Set the TextDraw so the player can see it
	TextDrawSetString(APlayerData[playerid][MissionText], RouteText);

	// Grab the coordinates of the next location
	x = ALocations[NextLoc][LocX];
	y = ALocations[NextLoc][LocY];
	z = ALocations[NextLoc][LocZ];
	// Create a new checkpoint where the player should load/unload the passengers
	SetPlayerRaceCheckpoint(playerid, 2, x, y, z, xn, yn, zn, 7);

    // Enable the player again (he can move again)
	TogglePlayerControllable(playerid, 1);

	// Reward the player (every passengers that left the bus pays $9) and let him know about it
	if (PassengersGettingOff != 0)
	{
		// Reward the player (give cash and points)
		Payment = PassengersGettingOff * 9;
		RewardPlayer(playerid, Payment, 0);
		format(Msg, sizeof(Msg), TXT_BusDriverReward, Payment);
		GameTextForPlayer(playerid, Msg, 3000, 4);
	}

	return 1;
}



// This function starts the busdriver job
BusDriver_StartJob(playerid, Job)
{
	new Vehicle = GetPlayerVehicleID(playerid), StartLoc;
	new Float:xn, Float: yn, Float:zn;

    // Setup local variables
	new RouteText[255], Float:x, Float:y, Float:z, LineNr, Description[100], PassengersOnBus;

	// Job has started
	APlayerData[playerid][JobStarted] = true;
	// Store VehicleID
	APlayerData[playerid][VehicleID] = Vehicle;
	// Store the busroute busroute
	APlayerData[playerid][JobID] = Job;
	// Set jobstep to 0 (going to the first busstop)
	APlayerData[playerid][JobStep] = 0;

	// Get the data to construct the textdraw
	LineNr = ABusRoutes[Job][LineNumber];
	format(Description, sizeof(Description), "%s", ABusRoutes[Job][RouteDescription]);
	PassengersOnBus = 0;
	APlayerData[playerid][Passengers] = PassengersOnBus;
	// Combine it all into a string for the TextDraw (the player can see this all the time) to describe the mission
	format(RouteText, sizeof(RouteText), TXT_BusDriverJobInfo, LineNr, Description, PassengersOnBus);
	// Set the TextDraw so the player can see it
	TextDrawSetString(APlayerData[playerid][MissionText], RouteText);
	// Grab the x, y, z positions for the first location
	StartLoc = ABusRoutes[Job][Locations][0];
	x = ALocations[StartLoc][LocX];
	y = ALocations[StartLoc][LocY];
	z = ALocations[StartLoc][LocZ];
	// Create a race-checkpoint where the player should load/unload passengers
	SetPlayerRaceCheckpoint(playerid, 2, x, y, z, xn, yn, zn, 7);
	// Set the job-fail-time for the global vehicle-timer
	APlayerData[playerid][VehicleTimerTime] = Job_TimeToFailMission;

	return 1;
}

// This function is used to cleanup the current job
BusDriver_EndJob(playerid)
{
	if (APlayerData[playerid][JobStarted] == true)
	{
		// Clear all data about the job from the player, so he can start a new one
		APlayerData[playerid][JobStarted] = false;
		APlayerData[playerid][JobStep] = 0;
		APlayerData[playerid][JobID] = 0;
		APlayerData[playerid][Passengers] = 0;
		APlayerData[playerid][VehicleTimerTime] = 0;
		APlayerData[playerid][VehicleID] = 0;

		// Delete the checkpoint
		DisablePlayerCheckpoint(playerid);
		// Also delete any race-checkpoint (at the location of the homedepot)
		DisablePlayerRaceCheckpoint(playerid);
		// Reset the missiontext
		TextDrawSetString(APlayerData[playerid][MissionText], BusDriver_NoJobText);
	}

	return 1;
}

