// This file holds all functions for the toll-system

forward Toll();
public Toll()
{
	// Loop through all players
    for(new playerid; playerid < MAX_PLAYERS; playerid++)
	{
		// If the player isn't connected, skip to the next player
        if(APlayerData[playerid][LoggedIn] == false) continue;

		// Check if the player is the driver of a vehicle
		if (GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
		{
			// Loop through all toll-gates
			for (new TollGate; TollGate < MAX_TOLLGATES; TollGate++)
			{
				// Check if this toll-gate exists
				if (ATollGates[TollGate][GateID] != 0)
				{
				    // Check if the player is in range of the tollgate
			        if(IsPlayerInRangeOfPoint(playerid, 7.5, ATollGates[TollGate][CloseX], ATollGates[TollGate][CloseY], ATollGates[TollGate][CloseZ]))
			        {
						// Check if the toll-gate is closed
			            if(ATollGates[TollGate][GateStatus] == 0)
						{
						    // Open the gate
			                MoveObject(ATollGates[TollGate][GateID], ATollGates[TollGate][OpenX], ATollGates[TollGate][OpenY], ATollGates[TollGate][OpenZ], 3.0);
							// Set status to OPEN
			                ATollGates[TollGate][GateStatus] = 1;
			                // Let the player pay the toll
			                RewardPlayer(playerid, -ATollGates[TollGate][TollPrice], 0);
			                new string[50];
			                format(string, sizeof(string), TXT_PlayerPaysToll, ATollGates[TollGate][TollPrice]);
			                GameTextForPlayer(playerid, string, 3000, 4);
			                // Start a timer that closes the gate after 5 seconds
			                SetTimerEx("CloseGate", 5000, false, "i", TollGate);
						}
			        }
				}
			}
		}
    }
}

forward CloseGate(TollGate);
public CloseGate(TollGate)
{
    // Close the gate
	MoveObject(ATollGates[TollGate][GateID], ATollGates[TollGate][CloseX], ATollGates[TollGate][CloseY], ATollGates[TollGate][CloseZ], 3.0);
	// Set status to CLOSED
	ATollGates[TollGate][GateStatus] = 0;
}

