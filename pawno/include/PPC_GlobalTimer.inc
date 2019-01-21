// This global timer runs every second and checks if a player is about to fail his mission (by getting out of his vehicle during a job)
forward GlobalTimer();
public GlobalTimer()
{
	// Setup local variables
	new OldVehicleID, NewVehicleID, OldTrailerID, NewTrailerID;

	// Loop through all players
	for (new playerid; playerid < MAX_PLAYERS; playerid++)
	{
		// Check if this player is logged in
		if (APlayerData[playerid][LoggedIn] == true)
		{
			// Get the vehicle-id's from this player
			OldVehicleID = APlayerData[playerid][VehicleID];
			NewVehicleID = GetPlayerVehicleID(playerid);
			OldTrailerID = APlayerData[playerid][TrailerID];
			NewTrailerID = GetVehicleTrailer(GetPlayerVehicleID(playerid));

			// Check the class of the player
			switch (APlayerData[playerid][PlayerClass])
			{
				case ClassTruckDriver:
				{
				    // Check if the trucker has a job
				    if (APlayerData[playerid][JobStarted] == true)
				    {
						// Check if the vehicletimer didn't run out yet
						if (APlayerData[playerid][VehicleTimerTime] != 0)
						{
							// If VehicleID and TrailerID are still the same as when the player accepted the job
							if ((OldVehicleID == NewVehicleID) && (OldTrailerID == NewTrailerID))
								APlayerData[playerid][VehicleTimerTime] = Job_TimeToFailMission; // Reset the time before the mission fails
							else // One (or both) aren't still the same (player lost his trailer or vehicle)
								PlayerLeftVehicle(playerid); // Inform the player that he left his vehicle and that he must re-enter it
						}
						else // Time left has reached 0
							FailJob(playerid);
				    }
				}
				case ClassBusDriver:
				{
				    // Check if the busdriver has a job
				    if (APlayerData[playerid][JobStarted] == true)
				    {
						if (APlayerData[playerid][VehicleTimerTime] != 0)
						{
							// If VehicleID is still the same as when the player accepted the job
							if (OldVehicleID == NewVehicleID)
								APlayerData[playerid][VehicleTimerTime] = Job_TimeToFailMission; // Reset the time before the mission fails
							else // Player got out of his bus
								PlayerLeftVehicle(playerid); // Inform the player that he left his vehicle and that he must re-enter it
						}
						else // Time left has reached 0
							FailJob(playerid);
					}
				}
				case ClassMafia:
				{
				    // Check if the mafia has a job
				    if (APlayerData[playerid][JobStarted] == true)
				    {
						if (APlayerData[playerid][VehicleTimerTime] != 0)
						{
							// If VehicleID is still the same as when the player accepted the job
							if (OldVehicleID == NewVehicleID)
								APlayerData[playerid][VehicleTimerTime] = Job_TimeToFailMission; // Reset the time before the mission fails
							else // Player left his vehicle
								PlayerLeftVehicle(playerid); // Inform the player that he left his vehicle and that he must re-enter it
						}
						else // Time left has reached 0
							FailJob(playerid);
					}
				}
				case ClassCourier:
				{
				    // Check if the courier has a job
				    if (APlayerData[playerid][JobStarted] == true)
				    {
						if (APlayerData[playerid][VehicleTimerTime] != 0)
						{
							// If VehicleID and TrailerID are still the same as when the player accepted the job
							if (OldVehicleID == NewVehicleID)
								APlayerData[playerid][VehicleTimerTime] = Job_TimeToFailMission; // Reset the time before the mission fails
							else // Player stepped out of his vehicle
								PlayerLeftVehicle(playerid); // Inform the player that he left his vehicle and that he must re-enter it
						}
						else // Time left has reached 0
							FailJob(playerid);
					}
				}
				case ClassRoadWorker:
				{
				    // Check if the roadworker has a job
				    if (APlayerData[playerid][JobStarted] == true)
				    {
						// Check if the vehicletimer didn't run out yet
						if (APlayerData[playerid][VehicleTimerTime] != 0)
						{
							// If VehicleID and TrailerID are still the same as when the player accepted the job
							// In case of the "tow broken vehicle" jobtype, the mission starts without a trailer (so it's 0),
							// but gets updated when the player enters the checkpoint to set the broken vehicle as trailer
							if ((OldVehicleID == NewVehicleID) && (OldTrailerID == NewTrailerID))
								APlayerData[playerid][VehicleTimerTime] = Job_TimeToFailMission; // Reset the time before the mission fails
							else // VehicleID isn't still the same (player lost his vehicle or trailer)
								PlayerLeftVehicle(playerid); // Inform the player that he left his vehicle and that he must re-enter it
						}
						else // Time left has reached 0
							FailJob(playerid);
					}
				}
			}
		}
	}

	return 1;
}

// This function is called by the global vehicletimer to fail a job
FailJob(playerid)
{
	// Setup local variables
	new Message[128];

	// Check the class of the player
	switch (APlayerData[playerid][PlayerClass])
	{
		case ClassTruckDriver:
		{
		    // End the player's job
			Trucker_EndJob(playerid);
			// If the player is part of a convoy, kick him from it (as he failed his mission, the rest of the convoy would be stuck)
			Convoy_Leave(playerid);
		}
		case ClassBusDriver:
		{
		    // End the player's job
			BusDriver_EndJob(playerid);
		}
		case ClassMafia:
		{
		    // End the player's job
			Mafia_EndJob(playerid);
		}
		case ClassCourier:
		{
		    // End the player's job
			Courier_EndJob(playerid);
		}
		case ClassRoadWorker:
		{
		    // End the player's job
			Roadworker_EndJob(playerid);
		}
	}

	format(Message, sizeof(Message), TXT_FailedMission, PRICE_FAILED_JOB);
	// Inform the player that he failed the mission
	GameTextForPlayer(playerid, Message, 5000, 4);
	// Reduce the player's cash
	RewardPlayer(playerid, -PRICE_FAILED_JOB, 0);
}

// This function is used by the global vehicletimer and informs the player that he left his vehicle and must re-enter it
PlayerLeftVehicle(playerid)
{
	// Setup local variables
	new TimeLeft[5];

	// Reduce the time left by 1
	APlayerData[playerid][VehicleTimerTime] = APlayerData[playerid][VehicleTimerTime] - 1;
	// Convert the time left to a string for displaying
	valstr(TimeLeft, APlayerData[playerid][VehicleTimerTime]);
	// Display the time left
	GameTextForPlayer(playerid, TimeLeft, 1000, 4);
	// Send only one message to inform the player what he must do
	if (APlayerData[playerid][VehicleTimerTime] == (Job_TimeToFailMission - 1))
	{
		// Check the class of the player and inform them what to do
		switch (APlayerData[playerid][PlayerClass])
		{
			case ClassTruckDriver:
				SendClientMessage(playerid, COLOR_WHITE, TXT_TruckerMustEnterVehicle);
			case ClassBusDriver:
				SendClientMessage(playerid, COLOR_WHITE, TXT_BusDriverMustEnterBus);
			case ClassMafia:
			    SendClientMessage(playerid, COLOR_WHITE, TXT_MafiaMustEnterVehicle);
			case ClassCourier:
			    SendClientMessage(playerid, COLOR_WHITE, TXT_CourierMustEnterVehicle);
			case ClassRoadWorker:
			    SendClientMessage(playerid, COLOR_WHITE, TXT_RoadworkerMustEnterVehicle);
		}
	}
}

