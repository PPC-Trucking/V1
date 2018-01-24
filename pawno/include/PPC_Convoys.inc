// This include file holds all functions for doing convoys
forward Convoy_Timer(Convoy);

// This function is called only once and is used to setup the textdraws and default data for convoys
Convoys_Init()
{
	for (new i; i < MAX_CONVOYS; i++)
	{
		AConvoys[i][ConvoyTextLeader] = TextDrawCreate(320.0, 1.0, " "); // Create the textdraw for the leader
		TextDrawSetShadow(AConvoys[i][ConvoyTextLeader], 1); // Reduce the shadow to 1
		TextDrawAlignment(AConvoys[i][ConvoyTextLeader], 2); // Align the convoy-infobar to the center for the leader
		TextDrawUseBox(AConvoys[i][ConvoyTextLeader], 1); // Set the missiontext to display inside a box
		TextDrawBoxColor(AConvoys[i][ConvoyTextLeader] ,0x00000066); // Set the box color of the missiontext

		AConvoys[i][ConvoyTextMember] = TextDrawCreate(320.0, 1.0, " "); // Create the textdraw for the members
		TextDrawSetShadow(AConvoys[i][ConvoyTextLeader], 1); // Reduce the shadow to 1
		TextDrawAlignment(AConvoys[i][ConvoyTextMember], 2); // Align the convoy-infobar to the center for the members
		TextDrawUseBox(AConvoys[i][ConvoyTextMember], 1); // Set the missiontext to display inside a box
		TextDrawBoxColor(AConvoys[i][ConvoyTextMember] ,0x00000066); // Set the box color of the missiontext
	}
}

// This function is used when a player selected an empty convoy-slot (the player will start the convoy and become the leader)
Convoy_Create(playerid, Convoy)
{
	// Setup local variables
	new Name[24], Msg[128];

	// Get the name of the player
	GetPlayerName(playerid, Name, sizeof(Name));

	// Check if the player is allowed to create a convoy (he must be a trucker without a job and not part of a convoy yet)
	if (Convoy_PlayerAllowed(playerid))
	{
		// Set status of the convoy to "open"
		AConvoys[Convoy][Status] = CONVOY_OPEN;
		// Set the player as leader of the convoy
		AConvoys[Convoy][Members][0] = playerid;
		// Set the player as a member of a convoy
		APlayerData[playerid][InConvoy] = true;
		APlayerData[playerid][ConvoyID] = Convoy;

		// Set all other member-indices to "-1" (no player yet)
		for (new i = 1; i < CONVOY_MAX_MEMBERS; i++)
		    AConvoys[Convoy][Members][i] = -1;
		// Start the convoy-timer (this timer updates and checks everything for the whole convoy), it runs every second
		AConvoys[Convoy][ConvoyTimer] = SetTimerEx("Convoy_Timer", 1000, true, "i", Convoy);

		// Let all players know that this player wants to start a convoy
		format(Msg, sizeof(Msg), TXT_PlayerStartsConvoy, Name);
		SendClientMessageToAll(COLOR_WHITE, Msg);
	}
}



// This function is used to let another player join a convoy
Convoy_Join(playerid, Convoy)
{
	// Setup local variables
	new Name[24], Msg[128];

	// Get the name of the player
	GetPlayerName(playerid, Name, sizeof(Name));

	// Check if the player is allowed to join the convoy (he must be a trucker without a job and not part of a convoy yet)
	if (Convoy_PlayerAllowed(playerid))
	{
	    // Check if the convoy isn't full already
		if (Convoy_CountMembers(Convoy) < CONVOY_MAX_MEMBERS)
		{
			// Inform all the members of the convoy that this player joined the convoy
			format(Msg, sizeof(Msg), TXT_PlayerJoinedConvoy, Name);
			Convoy_SendMessage(Convoy, Msg);
			// Inform the player that he joined the convoy
			SendClientMessage(playerid, COLOR_WHITE, TXT_YouJoinedConvoy);

			// Set the player as member of the convoy (find a free spot for this player)
			for (new i; i < CONVOY_MAX_MEMBERS; i++)
			{
			    if (AConvoys[Convoy][Members][i] == -1) // Check if this member-spot is empty
				{
					AConvoys[Convoy][Members][i] = playerid; // Put the player in this member-spot
					break; // Stop the for-loop
				}
			}
			// Set the player as a member of a convoy
			APlayerData[playerid][InConvoy] = true;
			APlayerData[playerid][ConvoyID] = Convoy;

			// Set the convoystatus as "Full" if all member-spots are occupied
			if (Convoy_CountMembers(Convoy) == CONVOY_MAX_MEMBERS)
			    AConvoys[Convoy][Status] = CONVOY_FULL;

			// Also update the player's missiontext to inform the player that he must wait for the leader to start a job
			TextDrawSetString(APlayerData[playerid][MissionText], TXT_WaitingLeaderJob);
		}
		else
		    SendClientMessage(playerid, COLOR_WHITE, TXT_ConvoyFull);
	}
}



// This function is used to let a player leave a convoy (when he disconnects, finishes the convoy, when he dies, ...)
Convoy_Leave(playerid)
{
	// Setup local variables
	new Convoy, NumMembers, MemberID;

	// First theck if the player is part of a convoy
	if (APlayerData[playerid][InConvoy] == false)
	    return 1; // Exit the function if the player isn't part of a convoy

	// Get the convoy-id from the player
	Convoy = APlayerData[playerid][ConvoyID];

	// Get the number of members in the convoy
	NumMembers = Convoy_CountMembers(Convoy);

	// If there is only 1 member in the convoy (convoy will have no members if this one leaves), cancel the convoy
	if (NumMembers == 1)
	{
		// Cancel the convoy
		Convoy_Cancel(Convoy);
		// Exit the function
		return 1;
	}

	// Remove the player from the convoy
	APlayerData[playerid][InConvoy] = false;
	APlayerData[playerid][ConvoyID] = 0;
	// Hide both convoy-textdraws (for leader and members) as the member leaves the convoy
	TextDrawHideForPlayer(playerid, AConvoys[Convoy][ConvoyTextLeader]);
	TextDrawHideForPlayer(playerid, AConvoys[Convoy][ConvoyTextMember]);
	// Also update the player's missiontext to inform the player that he can start a job now (if there isn't a job started)
	if (APlayerData[playerid][JobStarted] == false)
		TextDrawSetString(APlayerData[playerid][MissionText], Trucker_NoJobText);

    // If the player is the leader
	if (AConvoys[Convoy][Members][0] == playerid)
	{
		// Set another player as leader
		for (new j = 1; j < CONVOY_MAX_MEMBERS; j++)
		{
			// Get the playerid of the member
			MemberID = AConvoys[Convoy][Members][j];
			if (MemberID != -1) // If a valid playerid is found
			{
				// Hide the member-textdraw as the member just became the leader
				TextDrawHideForPlayer(MemberID, AConvoys[Convoy][ConvoyTextMember]);
                // Set this member as leader
				AConvoys[Convoy][Members][0] = MemberID;
                // Clear this index, or the player would be twice in the same convoy
				AConvoys[Convoy][Members][j] = -1;
                // Exit the function
				return 1;
			}
		}
	}
	else // The leaving player isn't the leader
	{
		// Find the player inside the convoy
		for (new i = 1; i < CONVOY_MAX_MEMBERS; i++)
		{
			// If the current player is this player
			if (AConvoys[Convoy][Members][i] == playerid)
			{
			    // Reset the stored playerid in the convoy
			    AConvoys[Convoy][Members][i] = -1;
				// Stop the job for this player (all data gets cleared, including the missiontext)
				Trucker_EndJob(playerid);

				return 1; // Exit the function
			}
		}
	}

	return 1;
}

// This function cancels the convoy, kicking every member in it
Convoy_Cancel(Convoy)
{
	// Setup local variables
	new MemberID;

	// Loop through all members
	for (new i; i < CONVOY_MAX_MEMBERS; i++)
	{
		// Get the member's playerid
	    MemberID = AConvoys[Convoy][Members][i];
		// If a valid playerid is found
		if (MemberID != -1)
		{
			// Remove the player from the convoy
			APlayerData[MemberID][InConvoy] = false;
			APlayerData[MemberID][ConvoyID] = 0;
			// Hide both convoy-textdraws (for leader and members)
   	        TextDrawHideForPlayer(MemberID, AConvoys[Convoy][ConvoyTextLeader]);
   	        TextDrawHideForPlayer(MemberID, AConvoys[Convoy][ConvoyTextMember]);
			// Cancel the trucker-job
			Trucker_EndJob(MemberID);
		    // Reset the stored playerid in the convoy
		    AConvoys[Convoy][Members][i] = -1;
			// Send the member a message that the convoy was cancelled by the leader
			SendClientMessage(MemberID, COLOR_WHITE, TXT_LeaderCancelledConvoy);
		}
	}

	// Clear all the data of the convoy
	AConvoys[Convoy][LoadID] = 0;
	AConvoys[Convoy][Location1] = 0;
	AConvoys[Convoy][Location2] = 0;
	AConvoys[Convoy][Status] = CONVOY_EMPTY;
	AConvoys[Convoy][ConvoyStep] = 0;
	AConvoys[Convoy][TrailerModel] = 0;
	AConvoys[Convoy][LeaderInformedTrailers] = false;
	// Kill the convoy-timer
	KillTimer(AConvoys[Convoy][ConvoyTimer]);
}



// This function is called for every member when the leader of the convoy started a job (missiontext is updated, loading-checkpoint is created, ...
Convoy_StartMemberJob(playerid, Convoy)
{
    // Setup local variables
	new StartLoc[50], EndLoc[50], Load[50], RouteText[255], Float:x, Float:y, Float:z, LoadMsg[128];

	// Job has started
	APlayerData[playerid][JobStarted] = true;
	// Copy the convoy-data to this player
	APlayerData[playerid][LoadID] = AConvoys[Convoy][LoadID];
	APlayerData[playerid][JobLoc1] = AConvoys[Convoy][Location1];
	APlayerData[playerid][JobLoc2] = AConvoys[Convoy][Location2];

	// Store the vehicleID (required to be able to check if the player left his vehicle)
	APlayerData[playerid][VehicleID] = GetPlayerVehicleID(playerid);
	// Store the trailerID (required to be able to check if the player lost his trailer)
	APlayerData[playerid][TrailerID] = GetVehicleTrailer(GetPlayerVehicleID(playerid));

	// Set jobstep to 1 (going to load the goods)
	APlayerData[playerid][JobStep] = 1;
	// Get the startlocation, endlocation and the load texts
	format(StartLoc, sizeof(StartLoc), ALocations[APlayerData[playerid][JobLoc1]][LocationName]);
	format(EndLoc, sizeof(EndLoc), ALocations[APlayerData[playerid][JobLoc2]][LocationName]);
	format(Load, sizeof(Load), ALoads[APlayerData[playerid][LoadID]][LoadName]);
	// Combine it all into a string for the TextDraw (the player can see this all the time) to describe the mission
	format(RouteText, sizeof(RouteText), TXT_HaulingCargoFromToPickup, Load, StartLoc, EndLoc);
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
	format(LoadMsg, sizeof(LoadMsg), TXT_PickupCargoAt, Load, StartLoc);
	SendClientMessage(playerid, COLOR_WHITE, LoadMsg);
	SendClientMessage(playerid, COLOR_WHITE, TXT_MeetOtherConvoyMembers);
}



// This function is called when all convoy-members have loaded their cargo (it updates the missiontext and creates the unload-checkpoint)
Convoy_UpdateMemberJob(playerid)
{
    // Setup local variables
	new StartLoc[50], EndLoc[50], Load[50], RouteText[255], Float:x, Float:y, Float:z, UnloadMsg[128];

	// Set the jobstep to 3 (going to unload the cargo at the destination)
	APlayerData[playerid][JobStep] = 3;
	// Get the startlocation, endlocation and the load texts
	format(StartLoc, sizeof(StartLoc), ALocations[APlayerData[playerid][JobLoc1]][LocationName]);
	format(EndLoc, sizeof(EndLoc), ALocations[APlayerData[playerid][JobLoc2]][LocationName]);
	format(Load, sizeof(Load), ALoads[APlayerData[playerid][LoadID]][LoadName]);
	// Update the missiontext
	format(RouteText, sizeof(RouteText), TXT_HaulingCargoFromToDeliver, Load, StartLoc, EndLoc);
	// Set the TextDraw so the player can see it
	TextDrawSetString(APlayerData[playerid][MissionText], RouteText);
	// Grab the x, y, z positions for the second location (to unload the goods)
	x = ALocations[APlayerData[playerid][JobLoc2]][LocX];
	y = ALocations[APlayerData[playerid][JobLoc2]][LocY];
	z = ALocations[APlayerData[playerid][JobLoc2]][LocZ];
	// Create a checkpoint where the player should unload the goods
	SetPlayerCheckpoint(playerid, x, y, z, 7);
	// Inform the player that he must unload his goods
	format(UnloadMsg, sizeof(UnloadMsg), TXT_DeliverCargoTo, Load, EndLoc);
	SendClientMessage(playerid, COLOR_WHITE, UnloadMsg);
}



// This is the timer used by every convoy (it updates and checks everything), is executed every 2.5 seconds
public Convoy_Timer(Convoy)
{
	// Setup local variables
	new LeaderID, MemberID;

	// Update the textdraws for all convoy members
    Convoy_UpdateTextDraws(Convoy);
	// Get the leader-id
	LeaderID = AConvoys[Convoy][Members][0];

    // Check the jobstep for the entire convoy
	switch (AConvoys[Convoy][ConvoyStep])
	{
		case 0: // Convoy has just been created, but a job hasn't started yet by the leader
		{
			new bool:AllSameTrailer = true;

			// Keep checking if the leader has started a job already
			if (APlayerData[LeaderID][JobStarted] == true)
			{
				// Copy the job-data from the leader to the convoy
				AConvoys[Convoy][LoadID] = APlayerData[LeaderID][LoadID];
				AConvoys[Convoy][Location1] = APlayerData[LeaderID][JobLoc1];
				AConvoys[Convoy][Location2] = APlayerData[LeaderID][JobLoc2];
				// Set the trailer-model required by all members to the convoy
				AConvoys[Convoy][TrailerModel] = GetVehicleModel(GetVehicleTrailer(GetPlayerVehicleID(LeaderID)));

				// First check if the leader has a trailer attached or not
				if (AConvoys[Convoy][TrailerModel] != 0)
				{
					// First check if all players have the correct trailer (except for the leader)
					for (new i = 1; i < CONVOY_MAX_MEMBERS; i++)
					{
						MemberID = AConvoys[Convoy][Members][i]; // Get the member-id from this member-spot
						if (MemberID != -1) // Check if the member-id is a valid playerid
						{
						    // Check if the player has the same trailer-model attached to his vehicle as the convoy requires
							if (GetVehicleModel(GetVehicleTrailer(GetPlayerVehicleID(MemberID))) != AConvoys[Convoy][TrailerModel])
							{
							    // Inform the player that he hasn't got the correct trailer
							    switch (AConvoys[Convoy][TrailerModel])
							    {
									case VehicleTrailerCargo, VehicleTrailerCargo2: TextDrawSetString(APlayerData[MemberID][MissionText], TXT_MemberNeedsCargoTrailer);
									case VehicleTrailerOre: TextDrawSetString(APlayerData[MemberID][MissionText], TXT_MemberNeedsOreTrailer);
									case VehicleTrailerFluids: TextDrawSetString(APlayerData[MemberID][MissionText], TXT_MemberNeedsFluidsTrailer);
								}
								// Not everyone has the same trailer
								AllSameTrailer = false;
							}
						}
					}
				}
				else // Leader has no trailer attached, so check for the vehiclemodel
				{
					// First check if all players have the correct trailer (except for the leader)
					for (new i = 1; i < CONVOY_MAX_MEMBERS; i++)
					{
						MemberID = AConvoys[Convoy][Members][i]; // Get the member-id from this member-spot
						if (MemberID != -1) // Check if the member-id is a valid playerid
						{
						    // Get the vehiclemodel of the member
						    new vModel = GetVehicleModel(GetPlayerVehicleID(MemberID));
						    // Check if the member has a valid trucking vehicle (flatbed or DFT30)
							switch (vModel)
							{
								case VehicleFlatbed, VehicleDFT30: AllSameTrailer = true;
								default:
								{
									TextDrawSetString(APlayerData[MemberID][MissionText], "You need a Flatbed or DFT-30");
									AllSameTrailer = false;
								}
							}
						}
					}
				}

				// If all members have the same trailer
				if (AllSameTrailer == true)
				{
					// Inform the leader that everyone has the same trailer
					SendClientMessage(LeaderID, COLOR_WHITE, TXT_AllMembersSameTrailer);

					// Start the same job for every member if they all have the same trailer
					for (new i = 1; i < CONVOY_MAX_MEMBERS; i++)
					{
						MemberID = AConvoys[Convoy][Members][i]; // Get the member-id from this member-spot
						if (MemberID != -1) // Check if the member-id is a valid playerid
							Convoy_StartMemberJob(MemberID, Convoy); // Start the job for the member
					}

					// Select the next step for the convoy (all members are now en-route to the loading-point)
	                AConvoys[Convoy][ConvoyStep] = 1;
					// Also close the convoy so no more members can join
				    AConvoys[Convoy][Status] = CONVOY_CLOSED;
				}
				else
				{
				    // Check if the leader has been informed already that not all members have the same trailer
					if (AConvoys[Convoy][LeaderInformedTrailers] == false)
					{
						// Inform the leader that not every member has the same trailer, convoy cannot start yet
						SendClientMessage(LeaderID, COLOR_WHITE, TXT_AllMembersNotSameTrailer);
						AConvoys[Convoy][LeaderInformedTrailers] = true; // Leader is informed now
					}
				}
			}
		}
		case 1: // Everyone has received their job-data (but haven't loaded their cargo yet)
		{
			new bool:AllMembersLoaded = true;

			// Check if everyone has loaded their cargo before moving on to convoystep 2
			for (new i; i < CONVOY_MAX_MEMBERS; i++)
			{
				MemberID = AConvoys[Convoy][Members][i]; // Get the playerid of the member
				if (MemberID != -1) // Check if the memberid is a valid id
					if (APlayerData[MemberID][JobStep] != 2) // Check if the player hasn't loaded his cargo yet
					    AllMembersLoaded = false; // Not all members have loaded their cargo yet
			}

			// Check if everyone has loaded their cargo
			if (AllMembersLoaded == true)
			{
				// Inform the leader that everyone has the same trailer
				SendClientMessage(LeaderID, COLOR_WHITE, TXT_AllMembersLoadedCargo);

				// Update the job for every member if they all have loaded their cargo
				for (new i; i < CONVOY_MAX_MEMBERS; i++)
				{
					MemberID = AConvoys[Convoy][Members][i]; // Get the member-id from this member-spot
					if (MemberID != -1) // Check if the member-id is a valid playerid
						Convoy_UpdateMemberJob(MemberID); // Start the job for the member
				}

				// Select the next step for the convoy (all members are now en-route to the unloading-point)
                AConvoys[Convoy][ConvoyStep] = 2;
			}
		}
		case 2: // Everybody has loaded their cargo and all members have their job updated, all members are en-route to the destination
		{
			// Check if everyone is staying close to the leader and check if all members have unloaded their cargo
			new bool:AllMembersUnloaded = true;

			// Also check if all players have delivered their load
			for (new i; i < CONVOY_MAX_MEMBERS; i++)
			{
				MemberID = AConvoys[Convoy][Members][i]; // Get the playerid of the member
				if (MemberID != -1) // Check if the memberid is a valid id
					if (APlayerData[MemberID][JobStep] != 4) // Check if the player hasn't unloaded his cargo yet
					    AllMembersUnloaded = false; // Not all members have unloaded their cargo yet
			}

			if (AllMembersUnloaded == true) // Check if all members have unloaded their cargo (nobody cleared this variable)
				AConvoys[Convoy][ConvoyStep] = 3; // Set the jobstep for the entire convoy to 3 (everybody unloaded their cargo, but jobs must still be payed out)
		}
		case 3: // Everybody has unloaded their cargo (now it's time to pay all members and finish the job)
		{
		    // Setup local variables
			new Float:x1, Float:y1, Float:x2, Float:y2, Float:Distance, Message[128], Payment, Bonus, NumMembers, Name[24], BonusMsg[128];

			// Count the number of members in the convoy
			NumMembers = Convoy_CountMembers(Convoy);

			// Get the name of the convoy-leader
			GetPlayerName(LeaderID, Name, sizeof(Name));

			// Grab the x, y, z positions for the first location (to load the goods)
			x1 = ALocations[APlayerData[LeaderID][JobLoc1]][LocX];
			y1 = ALocations[APlayerData[LeaderID][JobLoc1]][LocY];
			// Grab the x, y, z positions for the second location (to unload the goods)
			x2 = ALocations[APlayerData[LeaderID][JobLoc2]][LocX];
			y2 = ALocations[APlayerData[LeaderID][JobLoc2]][LocY];
			// Calculate the distance between both points
			Distance = floatsqroot(((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1)));

			// Calculate the payment for the player
			Payment = floatround((Distance * ALoads[APlayerData[LeaderID][LoadID]][PayPerUnit]), floatround_floor);

			// Check if the convoy has done the bonus mission
			if (RandomBonusMission[MissionFinished] == false)
			{
				// Check all paramters (load, startlocation and end-location)
				if (RandomBonusMission[RandomLoad] == APlayerData[LeaderID][LoadID])
					if (RandomBonusMission[RandomStartLoc] == APlayerData[LeaderID][JobLoc1])
						if (RandomBonusMission[RandomEndLoc] == APlayerData[LeaderID][JobLoc2])
						{
						    Payment = Payment * 2; // Double the payment is the player was the first to do the bonus mission
                            RandomBonusMission[MissionFinished] = true; // Only one player/convoy can do the bonus mission, a new one is chosen next
							format(BonusMsg, sizeof(BonusMsg), "{00BBFF}Convoy with leader {FFBB00}%s{00BBFF} has finished the bonus mission", Name);
							SendClientMessageToAll(COLOR_WHITE, BonusMsg);
						}
			}

		    // Calculate convoy-bonus (standard payment of 100% and 25% extra for each convoy-member)
		    Bonus = (NumMembers * 25) + 100; // For every member, 25% bonus is added to the payment, on top of the standard payment
			// Calculate total payment for each member
			Payment = (Payment * Bonus) / 100;

			// Pay every member and finish their mission
			for (new i; i < CONVOY_MAX_MEMBERS; i++)
			{
				MemberID = AConvoys[Convoy][Members][i]; // Get the playerid of the member
				if (MemberID != -1) // Check if the memberid is a valid id
				{
					// Reward the player (give cash and points)
					RewardPlayer(MemberID, Payment, 5);

					// Increase the stats for completing a trucking job while in a convoy
					APlayerData[MemberID][StatsConvoyJobs]++;
					// Also save the data (in case the server crashes, progress would be lost)
					PlayerFile_Save(MemberID);

					// End the member's job
					Trucker_EndJob(MemberID);
					// Send a message to let the player know he finished his mission and got paid
					format(Message, sizeof(Message), TXT_FinishedConvoy, Payment);
					SendClientMessage(MemberID, COLOR_WHITE, Message);
					// Also update the player's missiontext to inform the player that he must wait for the leader to start a job
					if (i != 0) // Skip this if the current index is the leader (the leader doesn't have to wait for a new job)
						TextDrawSetString(APlayerData[MemberID][MissionText], TXT_WaitingLeaderJob);
				}
			}

			// Clear the data in the convoy
			AConvoys[Convoy][LoadID] = 0; // Clear the load-id
			AConvoys[Convoy][Location1] = 0; // Clear the loadingpoint id
			AConvoys[Convoy][Location2] = 0; // Clear the unloading point id
			AConvoys[Convoy][Status] = CONVOY_OPEN; // Set status to "open" again, so new members can join
			AConvoys[Convoy][ConvoyStep] = 0; // Set convoystep to 0 (wait for a new job to be started by the leader)
			AConvoys[Convoy][TrailerModel] = 0; // Clear trailer model (the next job can be for another trailer)
			AConvoys[Convoy][LeaderInformedTrailers] = false; // Allow the leader to be informed again if not all members have the correct trailer
		}
	}

	return 1;
}



// This function is used to update the textdraws for the leader and all members (used by the convoy-timer)
Convoy_UpdateTextDraws(Convoy)
{
	// Setup local variables
	new LeaderID, MemberID, LeaderName[24], NumMembers, TextLeader[128], TextMember[128], LastMember[24], LastMemberID, Float:Distance;

	// Get the leader-id
	LeaderID = AConvoys[Convoy][Members][0];
	// Get the name of the convoy-leader
	GetPlayerName(LeaderID, LeaderName, sizeof(LeaderName));
	// Get the number of members of the convoy
	NumMembers = Convoy_CountMembers(Convoy);

	// Check if there members besides the leader
	if (NumMembers > 1)
	{
	    LastMemberID = Convoy_GetFurthestMember(Convoy); // Get the playerid of the member who is furthest away from the leader
		GetPlayerName(LastMemberID, LastMember, sizeof(LastMember)); // Get the name of the furthest member
		Distance = PlayerToPlayer(LeaderID, LastMemberID); // Get the distance to the last member
	}
	else // No other members are in the convoy yet
	{
		format(LastMember, sizeof(LastMember), " - ");
		Distance = 0.0;
	}

	// Update the convoy-textdraw for the leader
	format(TextLeader, sizeof(TextLeader), TXT_LeaderInfoBar, NumMembers, LastMember, Distance);
	TextDrawSetString(AConvoys[Convoy][ConvoyTextLeader], TextLeader);
	// Enable the convoy-textDraw for the leader
	TextDrawShowForPlayer(LeaderID, AConvoys[Convoy][ConvoyTextLeader]);

	// Update the convoy-textdraw for every member
	for (new i = 1; i < CONVOY_MAX_MEMBERS; i++)
	{
		MemberID = AConvoys[Convoy][Members][i]; // Get the playerid of the member
		if (MemberID != -1) // Check if the memberid is a valid id
		{
			// Calculate the distance to the leader
			Distance = PlayerToPlayer(LeaderID, MemberID);
			// Update the textdraw for the members
			format(TextMember, sizeof(TextMember), TXT_MemberInfoBar, LeaderName, Distance, NumMembers);
			TextDrawSetString(AConvoys[Convoy][ConvoyTextMember], TextMember);
			TextDrawShowForPlayer(MemberID, AConvoys[Convoy][ConvoyTextMember]);
		}
	}
}



// This function counts the members in the convoy
Convoy_CountMembers(Convoy)
{
	// Setup local variables
	new NumMembers;

	// Loop through all members
	for (new i; i < CONVOY_MAX_MEMBERS; i++)
	{
		// Check if there is a valid member-id stored (playerid)
		if (AConvoys[Convoy][Members][i] != -1)
		    NumMembers++; // Increase the number of members
	}

	// Return the number of members to the calling routine
	return NumMembers;
}



// This function checks the player and determines if he's a valid trucker who's able to create or join a convoy
Convoy_PlayerAllowed(playerid)
{
	// Make sure that the leader is a trucker
	if (APlayerData[playerid][PlayerClass] == ClassTruckDriver)
	{
		// Check if the player isn't a member of a convoy already
		if (APlayerData[playerid][InConvoy] == false)
		{
			// Make sure that the player hasn't started a job
			if (APlayerData[playerid][JobStarted] == false)
				return true; // The player is allowed to create or join a convoy
			else
			    SendClientMessage(playerid, COLOR_WHITE, TXT_CannotJoinJobStarted);
		}
		else
		    SendClientMessage(playerid, COLOR_WHITE, TXT_ConvoyAllreadyJoined);
	}
	else
	    SendClientMessage(playerid, COLOR_WHITE, TXT_ConvoyNeedsTruckerClass);

	// If any condition wasn't true, the player isn't allowed to create or join a convoy
	return false;
}

// This function sends the given message to all members of the convoy
Convoy_SendMessage(Convoy, Message[])
{
	// Setup local variables
	new MemberID;

	// Loop through all members
	for (new i; i < CONVOY_MAX_MEMBERS; i++)
	{
	    MemberID = AConvoys[Convoy][Members][i]; // Get the member-id on this index
	    if (MemberID != -1) // Check if this member has a valid playerid
		{
			SendClientMessage(MemberID, COLOR_WHITE, Message); // Send the given message to the member
		}
	}
}

// This function returns "true" is the given player is the leader of the convoy
stock Convoy_IsLeader(playerid, Convoy)
{
	// Check if the player is part of a convoy
	if ((APlayerData[playerid][InConvoy] == true) && (AConvoys[Convoy][Members][0] = playerid))
	    return true; // Player is in a convoy AND he's the leader of it
	else
	    return false; // Player is a member of the convoy (or not in the same convoy)
}

// This function returns true if the player is a member of the given convoy
stock Convoy_IsMember(playerid, Convoy)
{
	// Loop through all members (excluding the leader)
	for (new i = 1; i < CONVOY_MAX_MEMBERS; i++)
	    if (AConvoys[Convoy][Members][i] == playerid) // Check if this member is the given player
			return true; // Return true (the player is a member of the convoy)

	// If the given playerid wasn't found among the members, return false
	return false;
}



// A function that returns the member of a convoy that's the furthest away from the leader
Convoy_GetFurthestMember(Convoy)
{
	// Setup local variables
	new Float:distance = 0.0, Float:distance2 = 0.0, LeaderID, MemberID, result = -1;

	// Get the leader-id
	LeaderID = AConvoys[Convoy][Members][0];

	// Loop through all members (excluding the leader)
	for(new i = 1; i < CONVOY_MAX_MEMBERS; i++)
	{
		MemberID = AConvoys[Convoy][Members][i]; // Get the playerid of the member
		if (MemberID != -1) // Check if the memberid is a valid id
		{
			// Get the distance between leader and member
			distance2 = PlayerToPlayer(LeaderID, MemberID);
			// Check if the distance is bigger than the previous distance
			if(distance2 > distance)
			{
				// Store the distance
				distance = distance2;
				// Store the member-id
				result = MemberID;
			}
		}
	}

	// Return the vehicle-id of the closest vehicle
	return result;
}

// Get the distance between the two players
PlayerToPlayer(player1, player2)
{
	// Setup local variables
	new Float:pX, Float:pY, Float:pZ, Float:cX, Float:cY, Float:cZ, Float:distance;
	// Get the player1 position
	GetPlayerPos(player1, pX, pY, pZ);
	// Get the player2 position
	GetPlayerPos(player2, cX, cY, cZ);
	// Calculate the distance
	distance = floatsqroot(floatpower(floatabs(floatsub(cX, pX)), 2) + floatpower(floatabs(floatsub(cY, pY)), 2) + floatpower(floatabs(floatsub(cZ, pZ)), 2));
	// Return the distance to the calling routine
	return floatround(distance);
}

