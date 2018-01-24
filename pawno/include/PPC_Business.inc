// This timer increases the variable "BusinessTransactionTime" every hour
forward Business_TransactionTimer();
public Business_TransactionTimer()
{
	// Increase the variable by one
    BusinessTransactionTime++;

	// And save it to the file
	BusinessTime_Save();
}



// This function returns the first free business-slot for the given player
Player_GetFreeBusinessSlot(playerid)
{
	// Check if the player has room for another business (he hasn't bought the maximum amount of businesses per player yet)
	// and get the slot-id
	for (new BusIndex; BusIndex < MAX_BUSINESSPERPLAYER; BusIndex++) // Loop through all business-slots of the player
		if (APlayerData[playerid][Business][BusIndex] == 0) // Check if this business slot is free
		    return BusIndex; // Return the free BusIndex for this player

	// If there were no free business-slots, return "-1"
	return -1;
}



// This function sets ownership to the given player
Business_SetOwner(playerid, BusID)
{
	// Setup local variables
	new BusSlotFree, Name[24], Msg[128], BusType;

	// Get the first free business-slot from this player
	BusSlotFree = Player_GetFreeBusinessSlot(playerid);

	// Check if the player has a free business-slot
	if (BusSlotFree != -1)
	{
		// Get the player's name
		GetPlayerName(playerid, Name, sizeof(Name));

		// Store the business-id for the player
		APlayerData[playerid][Business][BusSlotFree] = BusID;
		// Get the business-type
		BusType = ABusinessData[BusID][BusinessType];

		// Let the player pay for the business
		RewardPlayer(playerid, -ABusinessInteriors[BusType][BusPrice], 0);

		// Set the business as owned
		ABusinessData[BusID][Owned] = true;
		// Store the owner-name for the business
		format(ABusinessData[BusID][Owner], 24, Name);
		// Set the level to 1
		ABusinessData[BusID][BusinessLevel] = 1;
		// Set the default business-name
		format(ABusinessData[BusID][BusinessName], 100, ABusinessInteriors[BusType][InteriorName]);
		// Store the current transaction-time (this is used so the player can only retrieve cash from the business from the moment he bought it)
		ABusinessData[BusID][LastTransaction] = BusinessTransactionTime;

		// Also, update 3DText of this business
		Business_UpdateEntrance(BusID);

		// Save the player-file (and also his houses/businesses)
		PlayerFile_Save(playerid);

		// Let the player know he bought the business
		format(Msg, sizeof(Msg), TXT_PlayerBoughtBusiness, ABusinessInteriors[BusType][BusPrice]);
		SendClientMessage(playerid, COLOR_WHITE, Msg);
	}
	else
	    SendClientMessage(playerid, COLOR_WHITE, TXT_PlayerOwnsMaxBusinesses);

	return 1;
}



// This function is used to spawn back at the entrance of your business
Business_Exit(playerid, BusID)
{
	// Set the player in the normal world again
	SetPlayerVirtualWorld(playerid, 0);
	SetPlayerInterior(playerid, 0);
	// Set the position of the player at the entrance of his business
	SetPlayerPos(playerid, ABusinessData[BusID][BusinessX], ABusinessData[BusID][BusinessY], ABusinessData[BusID][BusinessZ]);
	// Also clear the tracking-variable to track in which business the player is
	APlayerData[playerid][CurrentBusiness] = 0;

	// Check if there is a timer-value set for exiting the business (this timer freezes the player while the environment is being loaded)
	if (ExitBusinessTimer > 0)
	{
		// Don't allow the player to fall
	    TogglePlayerControllable(playerid, 0);
		// Let the player know he's frozen for 5 seconds
		GameTextForPlayer(playerid, TXT_ExitHouseReloadEnv, ExitBusinessTimer, 4);
		// Start a timer that will allow the player to fall again when the environment has loaded
		SetTimerEx("Business_ExitTimer", ExitBusinessTimer, false, "ii", playerid, BusID);
	}

	return 1;
}

forward Business_ExitTimer(playerid, BusID);
public Business_ExitTimer(playerid, BusID)
{
	// Allow the player to move again (environment should have been loaded now)
    TogglePlayerControllable(playerid, 1);

	return 1;
}



// This function adds a pickup for the given business
Business_CreateEntrance(BusID)
{
	// Setup local variables
	new Msg[128], Float:x, Float:y, Float:z, BusType, Icon;

	// Get the coordinates of the house's pickup (usually near the door)
	x = ABusinessData[BusID][BusinessX];
	y = ABusinessData[BusID][BusinessY];
	z = ABusinessData[BusID][BusinessZ];
	// Get the business-type and icon
	BusType = ABusinessData[BusID][BusinessType];
	Icon = ABusinessInteriors[BusType][IconID];

	// Add a dollar-sign to indicate this business
	ABusinessData[BusID][PickupID] = CreateDynamicPickup(1274, 1, x, y, z, 0);
	// Add a map-icon depending on which type the business is
	ABusinessData[BusID][MapIconID] = CreateDynamicMapIcon(x, y, z, Icon, 0, 0, 0, -1, 150.0);

	// Add a new 3DText at the business's location (usually near the door)
	if (ABusinessData[BusID][Owned] == true)
	{
		// Create the 3DText that appears above the business-pickup (displays the businessname, the name of the owner and the current level)
		format(Msg, sizeof(Msg), TXT_PickupBusinessOwned, ABusinessData[BusID][BusinessName], ABusinessData[BusID][Owner], ABusinessData[BusID][BusinessLevel]);
		ABusinessData[BusID][DoorText] = CreateDynamic3DTextLabel(Msg, COLOR_TEAL, x, y, z + 1.0, 50.0);
	}
	else
	{
		// Create the 3DText that appears above the business-pickup (displays the price of the business and the earnings)
		format(Msg, sizeof(Msg), TXT_PickupBusinessForSale, ABusinessInteriors[BusType][InteriorName], ABusinessInteriors[BusType][BusPrice], ABusinessInteriors[BusType][BusEarnings]);
		ABusinessData[BusID][DoorText] = CreateDynamic3DTextLabel(Msg, COLOR_TEAL, x, y, z + 1.0, 50.0);
	}
}

// This function changes the 3DText for the given business (used when buying or selling a business)
Business_UpdateEntrance(BusID)
{
	// Setup local variables
	new Msg[128], BusType;

	// Get the business-type
	BusType = ABusinessData[BusID][BusinessType];

	// Update the 3DText at the business's location (usually near the door)
	if (ABusinessData[BusID][Owned] == true)
	{
		// Create the 3DText that appears above the business-pickup (displays the businessname, the name of the owner and the current level)
		format(Msg, sizeof(Msg), TXT_PickupBusinessOwned, ABusinessData[BusID][BusinessName], ABusinessData[BusID][Owner], ABusinessData[BusID][BusinessLevel]);
		UpdateDynamic3DTextLabelText(ABusinessData[BusID][DoorText], COLOR_TEAL, Msg);
	}
	else
	{
		// Create the 3DText that appears above the business-pickup (displays the price of the business and the earnings)
		format(Msg, sizeof(Msg), TXT_PickupBusinessForSale, ABusinessInteriors[BusType][InteriorName], ABusinessInteriors[BusType][BusPrice], ABusinessInteriors[BusType][BusEarnings]);
		UpdateDynamic3DTextLabelText(ABusinessData[BusID][DoorText], COLOR_TEAL, Msg);
	}
}

// This function pays the current earnings of the given business to the player
Business_PayEarnings(playerid, BusID)
{
	// Setup local variables
	new Msg[128];

	// Get the business-type
	new BusType = ABusinessData[BusID][BusinessType];

	// Calculate the earnings of the business since the last transaction
	// This is calculated by the number of minutes between the current business-time and last business-time, multiplied by the earnings-per-minute and business-level
	new Earnings = (BusinessTransactionTime - ABusinessData[BusID][LastTransaction]) * ABusinessInteriors[BusType][BusEarnings] * ABusinessData[BusID][BusinessLevel];
	// Reset the last transaction time to the current time
	ABusinessData[BusID][LastTransaction] = BusinessTransactionTime;
	// Reward the player with his earnings
	RewardPlayer(playerid, Earnings, 0);
	// Inform the player that he has earned money from his business
	format(Msg, sizeof(Msg), "{00FF00}Your business has earned {FFFF00}$%i{00FF00} since your last withdrawl", Earnings);
	SendClientMessage(playerid, COLOR_WHITE, Msg);
}

