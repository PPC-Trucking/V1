// Make sure you don't get warnings about tabsize
#pragma tabsize 0

#pragma unused ret_memcpy

#if __Pawn >= 0x0400
	#error This script does not support PAWN V4.0 or higher
#elseif __Pawn < 0x0300
	#error This script does not support PAWN V2.0 or lower
#endif

// ********************************************************************************************************************
// Limit the amount of cops with a value greater than 0
// Setting this to "3" would mean:
// - having 3 normal players (non-cop players) before the first cop can join the server
// - having 6 normal players before 2 cops can be active
// - having 9 normal players before the third cop can join and so on
// Leaving this at 0 disables the police-limitation, so anyone can choose the police class anytime
// ********************************************************************************************************************

new PlayersBeforePolice	= 0;

// ********************************************************************************************************************
// ********************************************************************************************************************



// Include default files
#include <a_samp>
#include <zcmd>
#include <dutils>
#include <sscanf2>
#include <streamer>

// Include all define-statements and custom-type declarations and the arrays which use them
// These files need to be included before the functions get included, because the functions use the defines, custom types and the arrays
// Also include the defined loads (for truckers, military, mafia, ...) and locations arrays
#include <PPC_DefTexts>
#include <PPC_ServerSettings>
#include <PPC_Defines>
#include <PPC_DefLocations>
#include <PPC_DefLoads>
#include <PPC_DefCars>
#include <PPC_DefPlanes>
#include <PPC_DefTrailers>
#include <PPC_DefBuyableVehicles>
// Include functions for this gamemode
#include <PPC_AutoEvict>
#include <PPC_GlobalTimer>
#include <PPC_Common>
#include <PPC_Housing>
#include <PPC_Business>
#include <PPC_GameModeInit>
#include <PPC_FileOperations>
#include <PPC_Speedometer>
#include <PPC_MissionsTrucking>
#include <PPC_MissionsBus>
#include <PPC_MissionsPilot>
#include <PPC_MissionsPolice>
#include <PPC_MissionsMafia>
#include <PPC_MissionsAssistance>
#include <PPC_MissionsCourier>
#include <PPC_MissionsRoadworker>
#include <PPC_Convoys>
#include <PPC_Dialogs>
#include <PPC_PlayerCommands>
#include <PPC_PlayerCommandsAliases>
#include <PPC_Toll>
// #include <PPC_Extras> /* !! uncomment this line if you want to use additional features !! */



// The main function (used only once when the server loads)
main()
{
	// Print some standard lines to the server's console
	print("\n----------------------------------");
	print("Gamemode loading...");
	print("----------------------------------\n");
}



// This callback gets called when the server initializes the gamemode
public OnGameModeInit()
{
	GameModeInit_VehiclesPickups(); // Add all static vehicles and pickups when the server starts that are required (also load the houses)
	GameModeInit_Classes(); // Add character models to the class-selection (without weapons)

	Convoys_Init(); // Setup textdraws and default data for convoys

	ShowPlayerMarkers(PLAYER_MARKERS_MODE_GLOBAL); // Show players on the entire map (and on the radar)
	ShowNameTags(1); // Show player names (and health) above their head
	ManualVehicleEngineAndLights(); // Let the server control the vehicle's engine and lights
	EnableStuntBonusForAll(0); // Disable stunt bonus for all players
	DisableInteriorEnterExits(); // Removes all building-entrances in the game
	UsePlayerPedAnims(); // Use CJ's walking animation

	// Start the timer that will show timed messages every 2 minutes
	SetTimer("Timer_TimedMessages", 1000 * 60 * 2, true);
	// Start the timer that will show a random bonus mission for truckers every 5 minutes
	SetTimer("ShowRandomBonusMission", 1000 * 60 * 5, true);
	// Start the timer that checks the toll-gates
	SetTimer("Toll", 1000, true);

	// Fix the bugged houses (after fixing the houses, you can remove this line, as it's not needed anymore)
	FixHouses();

	// While the gamemode starts, start the global timer, and run it every second
	SetTimer("GlobalTimer", 1000, true);

	// Load the auto-evict-time and start the auto-evict timer (it runs every minute)
	AutoEvict_Load();
	SetTimer("AutoEvictTimer", 60 * 1000, true);

	return 1;
}



// This callback gets called when a player connects to the server
public OnPlayerConnect(playerid)
{
	// Always allow NPC's to login without password or account
	if (IsPlayerNPC(playerid))
		return 1;

	// start timer to check for login
	SetTimerEx("CheckPlayerLoggedIn", (TIMESPAN_LOGIN * 1000), false, "i", playerid);

	// Setup local variables
	new Name[MAX_PLAYER_NAME], NewPlayerMsg[128], HouseID;

	// Setup a PVar to allow cross-script money-transfers (only from filterscript to this mainscript) and scorepoints
	SetPVarInt(playerid, "PVarMoney", 0);
	SetPVarInt(playerid, "PVarScore", 0);

	// Get the playername
	GetPlayerName(playerid, Name, sizeof(Name));
	// Also store this name for the player
	GetPlayerName(playerid, APlayerData[playerid][PlayerName], 24);

	// Send a message to all players to let them know somebody else joined the server
	format(NewPlayerMsg, sizeof(NewPlayerMsg), TXT_PlayerJoinedServer, Name, playerid);
	SendClientMessageToAll(COLOR_WHITE, NewPlayerMsg);

	// Try to load the player's datafile ("PlayerFile_Load" returns "1" is the file has been read, "0" when the file cannot be read)
	if (PlayerFile_Load(playerid) == 1)
	{
		// check if the player is permanently banned
		if (APlayerData[playerid][BanTime] == -1) {
			SendClientMessage(playerid, COLOR_WHITE, TXT_BannedPermanently);
			SetTimerEx("TimedKick", 1000, false, "i", playerid); // Kick the player
		}
		// Check if the player is still banned
		else if (APlayerData[playerid][BanTime] > gettime()) // Player is still banned
		{
			ShowRemainingBanTime(playerid); // Show the remaining ban-time to the player is days, hours, minutes, seconds
			SetTimerEx("TimedKick", 1000, false, "i", playerid); // Kick the player			
		}
		else // Player ban-time is passed
		{
			ShowPlayerDialog(playerid, DialogLogin, DIALOG_STYLE_PASSWORD, TXT_DialogLoginTitle, TXT_DialogLoginMsg, TXT_DialogLoginButton1, TXT_DialogButtonCancel);
		}
	}
	else
		ShowPlayerDialog(playerid, DialogRegister, DIALOG_STYLE_PASSWORD, TXT_DialogRegisterTitle, TXT_DialogRegisterMsg, TXT_DialogRegisterButton1, TXT_DialogButtonCancel);

	// The houses have been loaded but not the cars, so load all vehicles assigned to the player's houses
	for (new HouseSlot; HouseSlot < MAX_HOUSESPERPLAYER; HouseSlot++)
	{
	    // Get the HouseID from this slot
	    HouseID = APlayerData[playerid][Houses][HouseSlot];
	    // Check if there is a house in this slot
		if (HouseID != 0)
		    HouseFile_Load(HouseID, true); // Load the cars of the house
	}

	// Speedometer setup
	Speedometer_Setup(playerid);

	// MissionText TextDraw setup
	APlayerData[playerid][MissionText] = TextDrawCreate(320.0, 430.0, " "); // Setup the missiontext at the bottom of the screen
	TextDrawAlignment(APlayerData[playerid][MissionText], 2); // Align the missiontext to the center
	TextDrawUseBox(APlayerData[playerid][MissionText], 1); // Set the missiontext to display inside a box
	TextDrawBoxColor(APlayerData[playerid][MissionText], 0x00000066); // Set the box color of the missiontext
	
	// Setup local variables
	new BusID;
	// Update the AutoEvict-time for this player's houses and businesses
	for (new HouseSlot; HouseSlot < MAX_HOUSESPERPLAYER; HouseSlot++)
	{
	// Get the HouseID from this slot
		HouseID = APlayerData[playerid][Houses][HouseSlot];
	// Check if there is a house in this slot
		if (HouseID != 0)
			AHouseData[HouseID][AutoEvictDays] = AutoEvict[AEDays];
	}
	for (new BusSlot; BusSlot < MAX_BUSINESSPERPLAYER; BusSlot++)
	{
	// Get the BusID from this slot
		BusID = APlayerData[playerid][Business][BusSlot];
	// Check if there is a business in this slot
		if (BusID != 0)
			ABusinessData[BusID][AutoEvictDays] = AutoEvict[AEDays];
	}

	return 1;
}



// This function shows the player how long his ban still is when he tries to login (in days, hours, minutes, seconds)
ShowRemainingBanTime(playerid)
{
	// Setup local variables
	new TotalBanTime, Days, Hours, Minutes, Seconds, Msg[128];

	// Get the total ban-time
	TotalBanTime = APlayerData[playerid][BanTime] - gettime();

	// Calculate days
	if (TotalBanTime >= 86400)
	{
		Days = TotalBanTime / 86400;
		TotalBanTime = TotalBanTime - (Days * 86400);
	}
	// Calculate hours
	if (TotalBanTime >= 3600)
	{
		Hours = TotalBanTime / 3600;
		TotalBanTime = TotalBanTime - (Hours * 3600);
	}
	// Calculate minutes
	if (TotalBanTime >= 60)
	{
		Minutes = TotalBanTime / 60;
		TotalBanTime = TotalBanTime - (Minutes * 60);
	}
	// Calculate seconds
	Seconds = TotalBanTime;

	// Display the remaining ban-time for this player
	SendClientMessage(playerid, COLOR_WHITE, TXT_StillBanned);
	format(Msg, sizeof(Msg), TXT_BannedDuration, Days, Hours, Minutes, Seconds);
	SendClientMessage(playerid, COLOR_WHITE, Msg);
}



// This function shows the player how long his muted time is
ShowRemainingMutedTime(playerid)
{
	// Setup local variables
	new TotalMutedTime, Minutes, Seconds, Msg[128];

	// Get the total muted time
	TotalMutedTime = APlayerData[playerid][Muted] - gettime();

	// Calculate minutes
	if (TotalMutedTime >= 60)
	{
		Minutes = TotalMutedTime / 60;
		TotalMutedTime = TotalMutedTime - (Minutes * 60);
	}
	// Calculate seconds
	Seconds = TotalMutedTime;

	// Display the remaining muted time for this player
	format(Msg, sizeof(Msg), TXT_MutedDuration, Minutes, Seconds);
	SendClientMessage(playerid, COLOR_SILVERCHALICE, Msg);
}



// This callback gets called when a player disconnects from the server
public OnPlayerDisconnect(playerid, reason)
{
	// Always allow NPC's to logout without password or account
	if (IsPlayerNPC(playerid))
		return 1;

	// Setup local variables
	new Name[MAX_PLAYER_NAME], Msg[128], HouseID;

	// Get the playername
	GetPlayerName(playerid, Name, sizeof(Name));

	// Stop spectate mode for all players who are spectating this player
	for (new i; i < MAX_PLAYERS; i++)
	    if (IsPlayerConnected(i)) // Check if the player is connected
	        if (GetPlayerState(i) == PLAYER_STATE_SPECTATING) // Check if this player is spectating somebody
	            if (APlayerData[i][SpectateID] == playerid) // Check if this player is spectating me
		   		{
					TogglePlayerSpectating(i, 0); // Turn off spectate-mode
					APlayerData[i][SpectateID] = INVALID_PLAYER_ID;
					APlayerData[i][SpectateType] = ADMIN_SPEC_TYPE_NONE;
					SendClientMessage(i, COLOR_RED, "Target player has logged off, ending spectate mode");
				}

	// Send a message to all players to let them know somebody left the server
	format(Msg, sizeof(Msg), TXT_PlayerLeftServer, Name, playerid);
	SendClientMessageToAll(COLOR_WHITE, Msg);

	// Reset muted time so the player gets unmuted automatically
	APlayerData[playerid][Muted] = 0;

	// If the player entered a proper password (the player has an account)
	if (strlen(APlayerData[playerid][PlayerPassword]) != 0)
	{
	    // Save the player data and his houses
		PlayerFile_Save(playerid);
	}

	// Stop any job that may have started (this also clears all mission data)
	switch (APlayerData[playerid][PlayerClass])
	{
		case ClassTruckDriver: Trucker_EndJob(playerid); // Stop any trucker job
		case ClassBusDriver: BusDriver_EndJob(playerid); // Stop any busdriver job
		case ClassPilot: Pilot_EndJob(playerid); // Stop any pilot job
		case ClassPolice: Police_EndJob(playerid); // Stop any police job
		case ClassMafia: Mafia_EndJob(playerid); // Stop any mafia job
		case ClassAssistance: Assistance_EndJob(playerid);
		case ClassRoadWorker: Roadworker_EndJob(playerid);
	}

	// If the player is part of a convoy, kick him from it
	Convoy_Leave(playerid);

	// Unload all the player's house-vehicles to make room for other player's vehicles
	for (new HouseSlot; HouseSlot < MAX_HOUSESPERPLAYER; HouseSlot++)
	{
	    // Get the HouseID from this slot
	    HouseID = APlayerData[playerid][Houses][HouseSlot];
	    // Check if there is a house in this slot
		if (HouseID != 0)
		{
		    // Unload the cars of the house
		    House_RemoveVehicles(HouseID);
			// Set the house so it cannot be entered by anyone (close the house)
			AHouseData[HouseID][HouseOpened] = false;
		}
	}

	// Clear the data in the APlayerData array to make sure the next player with the same id doesn't hold wrong data
	APlayerData[playerid][SpectateID] = -1;
	APlayerData[playerid][SpectateVehicle] = -1;
	APlayerData[playerid][SpectateType] = ADMIN_SPEC_TYPE_NONE;
	APlayerData[playerid][LoggedIn] = false;
	APlayerData[playerid][AssistanceNeeded] = false;
	APlayerData[playerid][PlayerPassword] = 0;
	APlayerData[playerid][PlayerLevel] = 0;
	APlayerData[playerid][PlayerJailed] = 0;
	APlayerData[playerid][PlayerFrozen] = 0; // Clearing this variable automatically kills the frozentimer
	APlayerData[playerid][Bans] = 0;
	APlayerData[playerid][BanTime] = 0;
	APlayerData[playerid][RulesRead] = false;
	APlayerData[playerid][AutoReportTime] = 0;
	APlayerData[playerid][TruckerLicense] = 0;
	APlayerData[playerid][BusLicense] = 0;
	APlayerData[playerid][PlayerClass] = 0;
	APlayerData[playerid][Warnings] = 0;
	APlayerData[playerid][PlayerMoney] = 0;
	APlayerData[playerid][PlayerScore] = 0;
	for (new HouseSlot; HouseSlot < MAX_HOUSESPERPLAYER; HouseSlot++)
		APlayerData[playerid][Houses][HouseSlot] = 0;
	for (new BusSlot; BusSlot < MAX_BUSINESSPERPLAYER; BusSlot++)
		APlayerData[playerid][Business][BusSlot] = 0;
	APlayerData[playerid][CurrentHouse] = 0;
	APlayerData[playerid][CurrentBusiness] = 0;

	// Clear the spectacting information
	APlayerData[playerid][Spectating] = false;
	APlayerData[playerid][SpectateX] = -1;
	APlayerData[playerid][SpectateY] = -1;
	APlayerData[playerid][SpectateZ] = -1;
	APlayerData[playerid][SpectateA] = -1;

	// Clear bank account info
	APlayerData[playerid][BankPassword] = 0;
	APlayerData[playerid][BankLoggedIn] = false;
	APlayerData[playerid][BankMoney] = 0;
	APlayerData[playerid][UseMoney] = 0;
	APlayerData[playerid][LastIntrestTime] = 0;

	// Clear stats
	APlayerData[playerid][StatsTruckerJobs] = 0;
	APlayerData[playerid][StatsConvoyJobs] = 0;
	APlayerData[playerid][StatsBusDriverJobs] = 0;
	APlayerData[playerid][StatsPilotJobs] = 0;
	APlayerData[playerid][StatsMafiaJobs] = 0;
	APlayerData[playerid][StatsMafiaStolen] = 0;
	APlayerData[playerid][StatsPoliceFined] = 0;
	APlayerData[playerid][StatsPoliceJailed] = 0;
	APlayerData[playerid][StatsCourierJobs] = 0;
	APlayerData[playerid][StatsRoadworkerJobs] = 0;
	APlayerData[playerid][StatsAssistance] = 0;
	APlayerData[playerid][StatsMetersDriven] = 0.0;

	// Clear police warnings
	APlayerData[playerid][PoliceCanJailMe] = false;
	APlayerData[playerid][PoliceWarnedMe] = false;
	APlayerData[playerid][Value_PoliceCanJailMe] = 0;

	// Make sure the jailtimer has been destroyed
	KillTimer(APlayerData[playerid][PlayerJailedTimer]);
	KillTimer(APlayerData[playerid][Timer_PoliceCanJailMe]);

	// Destroy the speedometer TextDraw for this player and the timer, also set the speed to 0
	Speedometer_Cleanup(playerid);

	// Also destroy the missiontext TextDraw for this player
	TextDrawDestroy(APlayerData[playerid][MissionText]);

	// Destroy a rented vehicle is the player had any
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
		RemoveAllPlayersFromVehicle(APlayerData[playerid][RentedVehicleID]);
		// Destroy the vehicle
		SetTimerEx("TimedDestroyVehicle", 1000, false, "i", APlayerData[playerid][RentedVehicleID]);
		// Clear the RentedVehicleID
		APlayerData[playerid][RentedVehicleID] = 0;
	}

	return 1;
}



// This callback gets called whenever a player uses the chat-box
public OnPlayerText(playerid, text[])
{
	// Check if the player is not logged in
	if (APlayerData[playerid][LoggedIn] != true) {
		// Let the player know that he must login first
		SendClientMessage(playerid, COLOR_RED, TXT_NeedToLogin);
		return 0;
	}

	// Block the player's text if he has been muted
    if ((APlayerData[playerid][Muted] > gettime()))
	{
		// Let the player know that he is still muted
		SendClientMessage(playerid, COLOR_ORANGE, TXT_StillMuted);

		// Show the remaining muted time to the player
		ShowRemainingMutedTime(playerid);

		// Do not send the text to the chatbox
		return 0;
	}

    return 1;
}



public OnPlayerCommandReceived(playerid, cmdtext[]) {
	// Check if the player is not logged in
	if (APlayerData[playerid][LoggedIn] != true) {
		// Let the player know that he must login first
		SendClientMessage(playerid, COLOR_RED, TXT_NeedToLogin);
		return 0;
	}

	// Check if the player is using the commands /me or /pm
	if (strfind(cmdtext, "/me ", true) != -1 || strfind(cmdtext, "/pm ", true) != -1)
	{
		// Check if the player is muted
		if (APlayerData[playerid][Muted] > gettime())
		{
			// Let the player know that he is still muted
			SendClientMessage(playerid, COLOR_ORANGE, TXT_StillMuted);
			
			// Show the remaining muted time to the player
			ShowRemainingMutedTime(playerid);

			// Do not send the text to the chatbox
			return 0;
		}
	} else
		if (GetPlayerState(playerid) == PLAYER_STATE_WASTED)
			return SendClientMessage(playerid, COLOR_RED, TXT_MustBeSpawned);

	return 1;
}



public OnPlayerCommandPerformed( playerid, cmdtext[ ], success )
 {
	// Check if the command is not valid
	if (!success)
		SendClientMessage(playerid, COLOR_GRAY, TXT_CmdNotExists);

	return 1;
}



// This callback gets called when a player interacts with a dialog
public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
	// Select the proper dialog to process
	switch (dialogid)
	{
		case DialogRegister: Dialog_Register(playerid, response, inputtext); // The "Register"-dialog
		case DialogLogin: Dialog_Login(playerid, response, inputtext); // The "Login"-dialog

		case DialogStatsOtherPlayer: Dialog_StatsOtherPlayer(playerid, response, listitem);
		case DialogStatsHouse: Dialog_StatsHouse(playerid, response, listitem);
		case DialogStatsGoHouse: Dialog_StatsGoHouse(playerid, response, listitem);
		case DialogStatsGoBusiness: Dialog_StatsGoBusiness(playerid, response, listitem);

		case DialogRescue: Dialog_Rescue(playerid, response, listitem); // The rescue-dialog

		case DialogBuyLicenses: Dialog_BuyLicenses(playerid, response, listitem); // The license-dialog (allows the player to buy trucker/busdriver licenses)

		case DialogRules: Dialog_Rules(playerid, response);

		case DialogTruckerJobMethod: Dialog_TruckerSelectJobMethod(playerid, response, listitem); // The work-dialog for truckers (shows the loads he can carry and lets the player choose the load)
		case DialogTruckerSelectLoad: Dialog_TruckerSelectLoad(playerid, response, listitem); // The load-selection dialog for truckers (shows the startlocations for the selected load and let the player choose his startlocation)
		case DialogTruckerStartLoc: Dialog_TruckerSelectStartLoc(playerid, response, listitem); // The start-location dialog for truckers (shows the endlocations for the selected load and let the player choose his endlocation)
		case DialogTruckerEndLoc: Dialog_TruckerSelectEndLoc(playerid, response, listitem); // The end-location dialog for truckers (processes the selected endlocation and starts the job)

		case DialogBusJobMethod: Dialog_BusSelectJobMethod(playerid, response, listitem); // The work-dialog for busdrivers (process the options to choose own busroute or auto-assigned busroute)
		case DialogBusSelectRoute: Dialog_BusSelectRoute(playerid, response, listitem); // Choose the busroute and start the job

		case DialogCourierSelectQuant: Dialog_CourierSelectQuant(playerid, response, listitem);

		case DialogBike: Dialog_Bike(playerid, response, listitem); // The bike-dialog
		case DialogCar: Dialog_Car(playerid, response, listitem); // The car-dialog (which uses a split dialog structure)
		case DialogPlane: Dialog_Plane(playerid, response, listitem); // The plane-dialog (which uses a split dialog structure)
		case DialogTrailer: Dialog_Trailer(playerid, response, listitem); // The trailer-dialog (which uses a split dialog structure)
		case DialogBoat: Dialog_Boat(playerid, response, listitem); // The boat-dialog
		case DialogNeon: Dialog_Neon(playerid, response, listitem); // The neon-dialog

		case DialogRentVehicleClass: Dialog_RentProcessClass(playerid, response, listitem); // The player chose a vehicleclass from where he can rent a vehicle
		case DialogRentVehicle: Dialog_RentVehicle(playerid, response, listitem); // The player chose a vehicle from the list of vehicles from the vehicleclass he chose before

		case DialogPlayerCommands: Dialog_PlayerCommands(playerid, response, listitem); // Displays all commands in a split-dialog structure
		case DialogPrimaryCarColor: Dialog_PrimaryCarColor(playerid, response, listitem);
		case DialogSecondaryCarColor: Dialog_SecondaryCarColor(playerid, response, listitem);

		case DialogWeather: Dialog_Weather(playerid, response, listitem); // The weather dialog
		case DialogCarOption: Dialog_CarOption(playerid, response, listitem); // The caroption dialog

		case DialogSelectConvoy: Dialog_SelectConvoy(playerid, response, listitem);

		case DialogHouseMenu: Dialog_HouseMenu(playerid, response, listitem); // Process the main housemenu
		case DialogUpgradeHouse: Dialog_UpgradeHouse(playerid, response, listitem); // Process the house-upgrade menu
		case DialogGoHome: Dialog_GoHome(playerid, response, listitem); // Port to one of your houses
		case DialogHouseNameChange: Dialog_ChangeHouseName(playerid, response, inputtext); // Change the name of your house
		case DialogSellHouse: Dialog_SellHouse(playerid, response); // Sell the house
		case DialogBuyCarClass: Dialog_BuyCarClass(playerid, response, listitem); // The player chose a vehicleclass from where he can buy a vehicle
		case DialogBuyCar: Dialog_BuyCar(playerid, response, listitem); // The player chose a vehicle from the list of vehicles from the vehicleclass he chose before
		case DialogSellCar: Dialog_SellCar(playerid, response, listitem);
		case DialogBuyInsurance: Dialog_BuyInsurance(playerid, response);
		case DialogGetVehiclesSelectHouse: Dialog_GetVehiclesSelectHouse(playerid, response, listitem);
		case DialogGetVehiclesSelectVehicle: Dialog_GetVehiclesSelectVehicle(playerid, response, listitem);
		case DialogUnclampVehicles: Dialog_UnclampVehicles(playerid, response);

		case DialogCreateBusSelType: Dialog_CreateBusSelType(playerid, response, listitem);
		case DialogBusinessMenu: Dialog_BusinessMenu(playerid, response, listitem);
		case DialogGoBusiness: Dialog_GoBusiness(playerid, response, listitem);
		case DialogBusinessNameChange: Dialog_ChangeBusinessName(playerid, response, inputtext); // Change the name of your business
		case DialogSellBusiness: Dialog_SellBusiness(playerid, response); // Sell the business

		case DialogBankPasswordRegister: Dialog_BankPasswordRegister(playerid, response, inputtext);
		case DialogBankPasswordLogin: Dialog_BankPasswordLogin(playerid, response, inputtext);
		case DialogBankOptions: Dialog_BankOptions(playerid, response, listitem);
		case DialogBankDeposit: Dialog_BankDeposit(playerid, response, inputtext);
		case DialogBankWithdraw: Dialog_BankWithdraw(playerid, response, inputtext);
		case DialogBankTransferMoney: Dialog_BankTransferMoney(playerid, response, inputtext);
		case DialogBankTransferName: Dialog_BankTransferName(playerid, response, inputtext);
		case DialogBankCancel: Dialog_BankCancel(playerid, response);

		case DialogHelpItemChosen: Dialog_HelpItemChosen(playerid, response, listitem);
		case DialogHelpItem: Dialog_HelpItem(playerid, response);

		case DialogOldPassword: Dialog_OldPassword(playerid, response, inputtext);
		case DialogNewPassword: Dialog_NewPassword(playerid, response, inputtext);
		case DialogConfirmPassword: Dialog_ConfirmPassword(playerid, response);
	}

    return 1;
}

// this callback gets called when a player clicks on another player on the scoreboard
public OnPlayerClickPlayer(playerid, clickedplayerid, source)
{
	// Check if the player is an admin of at least level 1
	if (APlayerData[playerid][PlayerLevel] >= 1)
	{
		// Setup local variables
		new Name[MAX_PLAYER_NAME], DialogTitle[128], PlayerStatList[3000], PlayerIP[16], NumHouses, NumBusinesses;

		// Construct the dialog-title
		GetPlayerName(clickedplayerid, Name, sizeof(Name));
		format(DialogTitle, sizeof(DialogTitle), "Statistics of player: %s", Name);

		// Add the IP of the player to the list
	    GetPlayerIp(clickedplayerid, PlayerIP, sizeof(PlayerIP));
		format(PlayerStatList, sizeof(PlayerStatList), "%s{FFFFFF}Player-IP: {00FF00}%s\n", PlayerStatList, PlayerIP);
		// Add the level of the player to the list
		format(PlayerStatList, sizeof(PlayerStatList), "%s{FFFFFF}Admin-level: {00FF00}%i\n", PlayerStatList, APlayerData[clickedplayerid][PlayerLevel]);
		// Add the class of the player to the list
		switch(APlayerData[clickedplayerid][PlayerClass])
		{
			case ClassTruckDriver: format(PlayerStatList, sizeof(PlayerStatList), "%s{FFFFFF}Class: {00FF00}Trucker\n", PlayerStatList);
			case ClassBusDriver: format(PlayerStatList, sizeof(PlayerStatList), "%s{FFFFFF}Class: {00FF00}Bus driver\n", PlayerStatList);
			case ClassPilot: format(PlayerStatList, sizeof(PlayerStatList), "%s{FFFFFF}Class: {00FF00}Pilot\n", PlayerStatList);
			case ClassPolice: format(PlayerStatList, sizeof(PlayerStatList), "%s{FFFFFF}Class: {00FF00}Police\n", PlayerStatList);
			case ClassMafia: format(PlayerStatList, sizeof(PlayerStatList), "%s{FFFFFF}Class: {00FF00}Mafia\n", PlayerStatList);
			case ClassCourier: format(PlayerStatList, sizeof(PlayerStatList), "%s{FFFFFF}Class: {00FF00}Courier\n", PlayerStatList);
			case ClassAssistance: format(PlayerStatList, sizeof(PlayerStatList), "%s{FFFFFF}Class: {00FF00}Assistance\n", PlayerStatList);
		}
		// Add money and score of the player to the list
		format(PlayerStatList, sizeof(PlayerStatList), "%s{FFFFFF}Money: {00FF00}%i\n", PlayerStatList, APlayerData[clickedplayerid][PlayerMoney]);
		format(PlayerStatList, sizeof(PlayerStatList), "%s{FFFFFF}Score: {00FF00}%i\n", PlayerStatList, APlayerData[clickedplayerid][PlayerScore]);
		// Add wanted-level of the player to the list
		format(PlayerStatList, sizeof(PlayerStatList), "%s{FFFFFF}Wanted-level: {00FF00}%i\n", PlayerStatList, GetPlayerWantedLevel(clickedplayerid));
		// Add truckerlicense and busdriver license of the player to the list
		if (APlayerData[clickedplayerid][TruckerLicense] == 1)
			format(PlayerStatList, sizeof(PlayerStatList), "%s{FFFFFF}Trucker License: {00FF00}Yes\n", PlayerStatList);
		else
			format(PlayerStatList, sizeof(PlayerStatList), "%s{FFFFFF}Trucker License: {00FF00}No\n", PlayerStatList);

		if (APlayerData[clickedplayerid][BusLicense] == 1)
			format(PlayerStatList, sizeof(PlayerStatList), "%s{FFFFFF}Bus License: {00FF00}Yes\n", PlayerStatList);
		else
			format(PlayerStatList, sizeof(PlayerStatList), "%s{FFFFFF}Bus License: {00FF00}No\n", PlayerStatList);

		format(PlayerStatList, sizeof(PlayerStatList), "%s{FFFFFF}Completed trucker jobs: {00FF00}%i\n", PlayerStatList, APlayerData[clickedplayerid][StatsTruckerJobs]);
		format(PlayerStatList, sizeof(PlayerStatList), "%s{FFFFFF}Completed convoy jobs: {00FF00}%i\n", PlayerStatList, APlayerData[clickedplayerid][StatsConvoyJobs]);
		format(PlayerStatList, sizeof(PlayerStatList), "%s{FFFFFF}Completed busdriver jobs: {00FF00}%i\n", PlayerStatList, APlayerData[clickedplayerid][StatsBusDriverJobs]);
		format(PlayerStatList, sizeof(PlayerStatList), "%s{FFFFFF}Completed pilot jobs: {00FF00}%i\n", PlayerStatList, APlayerData[clickedplayerid][StatsPilotJobs]);
		format(PlayerStatList, sizeof(PlayerStatList), "%s{FFFFFF}Completed mafia jobs: {00FF00}%i\n", PlayerStatList, APlayerData[clickedplayerid][StatsMafiaJobs]);
		format(PlayerStatList, sizeof(PlayerStatList), "%s{FFFFFF}Stolen mafia-loads: {00FF00}%i\n", PlayerStatList, APlayerData[clickedplayerid][StatsMafiaStolen]);
		format(PlayerStatList, sizeof(PlayerStatList), "%s{FFFFFF}Fined players: {00FF00}%i\n", PlayerStatList, APlayerData[clickedplayerid][StatsPoliceFined]);
		format(PlayerStatList, sizeof(PlayerStatList), "%s{FFFFFF}Jailed players: {00FF00}%i\n", PlayerStatList, APlayerData[clickedplayerid][StatsPoliceJailed]);
		format(PlayerStatList, sizeof(PlayerStatList), "%s{FFFFFF}Completed courier jobs: {00FF00}%i\n", PlayerStatList, APlayerData[clickedplayerid][StatsCourierJobs]);
		format(PlayerStatList, sizeof(PlayerStatList), "%s{FFFFFF}Completed roadworker jobs: {00FF00}%i\n", PlayerStatList, APlayerData[clickedplayerid][StatsRoadworkerJobs]);
		format(PlayerStatList, sizeof(PlayerStatList), "%s{FFFFFF}Assisted players: {00FF00}%i\n", PlayerStatList, APlayerData[clickedplayerid][StatsAssistance]);
		format(PlayerStatList, sizeof(PlayerStatList), "%s{FFFFFF}Distance driven: {00FF00}%.0f meters (%.2f km)\n", PlayerStatList, APlayerData[clickedplayerid][StatsMetersDriven], (APlayerData[clickedplayerid][StatsMetersDriven] / 1000));

		// Count the number of houses/businesses that the player has and add them to the list
		for (new i; i < MAX_HOUSESPERPLAYER; i++)
			if (APlayerData[clickedplayerid][Houses][i] != 0)
			    NumHouses++;

		for (new i; i < MAX_BUSINESSPERPLAYER; i++)
			if (APlayerData[clickedplayerid][Business][i] != 0)
			    NumBusinesses++;

		format(PlayerStatList, sizeof(PlayerStatList), "%s{FFFFFF}Houses: {00FF00}%i (double-click for stats)\n", PlayerStatList, NumHouses);
		format(PlayerStatList, sizeof(PlayerStatList), "%s{FFFFFF}Businesses: {00FF00}%i (double-click for stats)\n", PlayerStatList, NumBusinesses);

		// Store the player-id of the other player so the other dialogs can display his statistics further (houses, businesses, cars)
		APlayerData[playerid][DialogOtherPlayer] = clickedplayerid;

		// Show the statistics of the other player
		ShowPlayerDialog(playerid, DialogStatsOtherPlayer, DIALOG_STYLE_LIST, DialogTitle, PlayerStatList, TXT_DialogButtonSelect, TXT_DialogButtonCancel); // Let the player buy a license
	}

	return 1;
}



// This callback gets called when a player picks up any pickup
public OnPlayerPickUpPickup(playerid, pickupid)
{
	// If the player picks up the Buy_License pickup at the driving school in Doherty
	if (pickupid == Pickup_License)
	    // Ask the player which license he wants to buy
		ShowPlayerDialog(playerid, DialogBuyLicenses, DIALOG_STYLE_LIST, TXT_DialogLicenseTitle, TXT_DialogLicenseList, TXT_DialogButtonBuy, TXT_DialogButtonCancel); // Let the player buy a license

	return 1;
}



// This callback gets called when a player spawns somewhere
public OnPlayerSpawn(playerid)
{
	// Always allow NPC's to spawn without logging in
	if (IsPlayerNPC(playerid))
		return 1;

	// Check if the player properly logged in by typing his password
	if (APlayerData[playerid][LoggedIn] == false)
	{
		SendClientMessage(playerid, COLOR_WHITE, TXT_FailedLoginProperly);
	    SetTimerEx("TimedKick", 1000, false, "i", playerid); // Kick the player if he didn't log in properly
	}

	// Setup local variables
	new missiontext[200];

	// Spawn the player in the global world (where everybody plays the game)
    SetPlayerVirtualWorld(playerid, 0);
	SetPlayerInterior(playerid, 0);
	// Also set a variable that tracks in which house the player currently is
	APlayerData[playerid][CurrentHouse] = 0;
	APlayerData[playerid][CurrentBusiness] = 0;
	
	// Disable the clock
	TogglePlayerClock(playerid, 0);

	// Delete all weapons from the player
	ResetPlayerWeapons(playerid);

	// Set the missiontext based on the chosen class
	switch (APlayerData[playerid][PlayerClass])
	{
		case ClassTruckDriver: // Truck-driver class
		{
			format(missiontext, sizeof(missiontext), Trucker_NoJobText); // Preset the missiontext
			SetPlayerColor(playerid, ColorClassTruckDriver); // Set the playercolor (chatcolor for the player and color on the map)
		}
		case ClassBusDriver: // Bus-driver class
		{
			format(missiontext, sizeof(missiontext), BusDriver_NoJobText); // Preset the missiontext
			SetPlayerColor(playerid, ColorClassBusDriver); // Set the playercolor (chatcolor for the player and color on the map)
		}
		case ClassPilot: // Pilot class
		{
			format(missiontext, sizeof(missiontext), Pilot_NoJobText); // Preset the missiontext
			SetPlayerColor(playerid, ColorClassPilot); // Set the playercolor (chatcolor for the player and color on the map)
		}
		case ClassPolice: // Police class
		{
			format(missiontext, sizeof(missiontext), Police_NoJobText); // Preset the missiontext
			SetPlayerColor(playerid, ColorClassPolice); // Set the playercolor (chatcolor for the player and color on the map)
			// Start the PlayerCheckTimer to scan for wanted players (be sure the timer has been destroyed first)
			KillTimer(APlayerData[playerid][PlayerCheckTimer]);
			APlayerData[playerid][PlayerCheckTimer] = SetTimerEx("Police_CheckWantedPlayers", 1000, true, "i", playerid);
			// Check if the police player can get weapons
			if (PoliceGetsWeapons == true)
			{
			    // Give weapons to the police player
				for (new i; i < sizeof(APoliceWeapons); i++)
				    GivePlayerWeapon(playerid, APoliceWeapons[i], PoliceWeaponsAmmo);
			}
		}
		case ClassMafia: // Mafia class
		{
			format(missiontext, sizeof(missiontext), Mafia_NoJobText); // Preset the missiontext
			SetPlayerColor(playerid, ColorClassMafia); // Set the playercolor (chatcolor for the player and color on the map)
			// Start the PlayerCheckTimer to scan for players that carry mafia-loads (be sure the timer has been destroyed first)
			KillTimer(APlayerData[playerid][PlayerCheckTimer]);
			APlayerData[playerid][PlayerCheckTimer] = SetTimerEx("Mafia_CheckMafiaLoads", 1000, true, "i", playerid);
		}
		case ClassCourier: // Courier class
		{
			format(missiontext, sizeof(missiontext), Courier_NoJobText); // Preset the missiontext
			SetPlayerColor(playerid, ColorClassCourier); // Set the playercolor (chatcolor for the player and color on the map)
		}
		case ClassAssistance: // Assistance class
		{
			format(missiontext, sizeof(missiontext), Assistance_NoJobText); // Preset the missiontext
			SetPlayerColor(playerid, ColorClassAssistance); // Set the playercolor (chatcolor for the player and color on the map)
			// Start the PlayerCheckTimer to scan for players who need assistance (be sure the timer has been destroyed first)
			KillTimer(APlayerData[playerid][PlayerCheckTimer]);
			APlayerData[playerid][PlayerCheckTimer] = SetTimerEx("Assistance_CheckPlayers", 1000, true, "i", playerid);
		}
		case ClassRoadWorker: // Roadworker class
		{
			format(missiontext, sizeof(missiontext), RoadWorker_NoJobText); // Preset the missiontext
			SetPlayerColor(playerid, ColorClassRoadWorker); // Set the playercolor (chatcolor for the player and color on the map)
		}
	}

	// Display a message if the player hasn't accepted the rules yet
	if (APlayerData[playerid][RulesRead] == false)
	    SendClientMessage(playerid, COLOR_RED, "You haven't accepted the {FFFF00}/rules{FF0000} yet");

	// Set the missiontext
	TextDrawSetString(APlayerData[playerid][MissionText], missiontext);
	// Show the missiontext for this player
	TextDrawShowForPlayer(playerid, APlayerData[playerid][MissionText]);

	// If the player spawns and his jailtime hasn't passed yet, put him back in jail
	if (APlayerData[playerid][PlayerJailed] != 0)
	    Police_JailPlayer(playerid, APlayerData[playerid][PlayerJailed]);

	// Teleport the player to the latest position if he was spectating
	if (APlayerData[playerid][Spectating] == true) {
		SetPlayerPos(playerid, APlayerData[playerid][SpectateX], APlayerData[playerid][SpectateY], APlayerData[playerid][SpectateZ]);
		SetPlayerFacingAngle(playerid, APlayerData[playerid][SpectateA]);

		// Reset the coordinates
		APlayerData[playerid][Spectating] = false;
		APlayerData[playerid][SpectateX] = -1;
		APlayerData[playerid][SpectateY] = -1;
		APlayerData[playerid][SpectateZ] = -1;
		APlayerData[playerid][SpectateA] = -1;
	}

	return 1;
}



// This callback gets called whenever a player enters a checkpoint
public OnPlayerEnterCheckpoint(playerid)
{
	// Check the player's class
	switch (APlayerData[playerid][PlayerClass])
	{
		case ClassTruckDriver: // Truckdriver class
			Trucker_OnPlayerEnterCheckpoint(playerid); // Process the checkpoint (load or unload goods)
		case ClassBusDriver: // BusDriver class
		{
			GameTextForPlayer(playerid, TXT_BusDriverMissionPassed, 3000, 4); // Show a message to let the player know he finished his job
			BusDriver_EndJob(playerid); // End the current mission
		}
		case ClassPilot: // Pilot class
			Pilot_OnPlayerEnterCheckpoint(playerid); // Process the checkpoint (load or unload)
		case ClassMafia: // Mafia class
			Mafia_OnPlayerEnterCheckpoint(playerid);
		case ClassCourier: // Courier class
			Courier_OnPlayerEnterCheckpoint(playerid);
		case ClassRoadWorker: // Roadworker class
		{
			// Only end the mission when doing "repair-speedcamera" jobtype (checkpoint is the base of the roadworker)
			if (APlayerData[playerid][JobID] == 1) // Repairing speedcamera's
			{
				GameTextForPlayer(playerid, TXT_RoadworkerMissionPassed, 3000, 4); // Show a message to let the player know he finished his job
				Roadworker_EndJob(playerid); // End the current mission
			}
			if (APlayerData[playerid][JobID] == 2) // Towing broken vehicle to shredder
                Roadworker_EnterCheckpoint(playerid);
		}
	}

	return 1;
}



// This callback gets called when a player enters a race-checkpoint
public OnPlayerEnterRaceCheckpoint(playerid)
{
	// Check the player's class
	switch (APlayerData[playerid][PlayerClass])
	{
		case ClassBusDriver: // BusDriver class
		    Bus_EnterRaceCheckpoint(playerid); // Process the checkpoint
		case ClassRoadWorker: // Roadworker class
			Roadworker_EnterRaceCheckpoint(playerid);
	}

	return 1;
}



// This callback gets called whenever a player dies
public OnPlayerDeath(playerid, killerid, reason)
{
	// Setup local variables
	new VictimName[MAX_PLAYER_NAME], KillerName[MAX_PLAYER_NAME], Msg[128];

	// Clear the missiontext
	TextDrawSetString(APlayerData[playerid][MissionText], " ");
	// Hide the missiontext for this player (when the player is choosing a class, it's not required to show any mission-text)
	TextDrawHideForPlayer(playerid, APlayerData[playerid][MissionText]);

	// Stop any job that may have started
	switch (APlayerData[playerid][PlayerClass])
	{
		case ClassTruckDriver: Trucker_EndJob(playerid);
		case ClassBusDriver: BusDriver_EndJob(playerid);
		case ClassPilot: Pilot_EndJob(playerid);
		case ClassPolice: Police_EndJob(playerid);
		case ClassMafia: Mafia_EndJob(playerid);
		case ClassCourier: Courier_EndJob(playerid);
		case ClassAssistance: Assistance_EndJob(playerid);
		case ClassRoadWorker: Roadworker_EndJob(playerid);
	}

	// If the player is part of a convoy, kick him from it
	Convoy_Leave(playerid);

	// If another player kills you, he'll get an extra star of his wanted level if he's isn't a police officer
	if (killerid != INVALID_PLAYER_ID && APlayerData[killerid][PlayerClass] != ClassPolice)
	{
		// Increase the wanted level of the killer by one star
	    SetPlayerWantedLevel(killerid, GetPlayerWantedLevel(killerid) + 1);
	    // Get the name of the killed player and the killer
	    GetPlayerName(playerid, VictimName, sizeof(VictimName));
	    GetPlayerName(killerid, KillerName, sizeof(KillerName));
	    // Let the killed know the police are informed about the kill
		format(Msg, sizeof(Msg), "{FF0000}You've killed {FFFF00}%s{FF0000}, you're wanted by the police now", VictimName);
		SendClientMessage(killerid, COLOR_WHITE, Msg);
		// Inform all police players about the kill
		format(Msg, sizeof(Msg), "{00FF00}Player {FFFF00}%s{00FF00} killed {FFFF00}%s{00FF00}, pursue and fine him", KillerName, VictimName);
		Police_SendMessage(Msg);
	}

	return 1;
}



// This callback gets called when the player is selecting a class (but hasn't clicked "Spawn" yet)
public OnPlayerRequestClass(playerid, classid)
{
	if (APlayerData[playerid][BanTime] != 0) {
		TogglePlayerSpectating(playerid, 1);
		return 1;
	}
 
 	SetPlayerInterior(playerid,14);
	SetPlayerPos(playerid,258.4893,-41.4008,1002.0234);
	SetPlayerFacingAngle(playerid, 270.0);
	SetPlayerCameraPos(playerid,256.0815,-43.0475,1004.0234);
	SetPlayerCameraLookAt(playerid,258.4893,-41.4008,1002.0234);

	// Display a short message to inform the player about the class he's about to choose
	switch (classid)
	{
		case 0, 1, 2, 3, 4, 5, 6, 7: // Classes that will be truckdrivers
		{
			// Display the name of the class
            GameTextForPlayer(playerid, TXT_ClassTrucker, 3000, 4);
			// Store the class for the player (truckdriver)
			APlayerData[playerid][PlayerClass] = ClassTruckDriver;
		}
		case 8, 9: // Classes that will be bus-drivers
		{
			// Display the name of the class
            GameTextForPlayer(playerid, TXT_ClassBusDriver, 3000, 4);
			// Store the class for the player (busdriver)
			APlayerData[playerid][PlayerClass] = ClassBusDriver;
		}
		case 10: // Classes that will be Pilot
		{
			// Display the name of the class
            GameTextForPlayer(playerid, TXT_ClassPilot, 3000, 4);
			// Store the class for the player (pilot)
			APlayerData[playerid][PlayerClass] = ClassPilot;
		}
		case 11, 12, 13: // Classes that will be police
		{
			// Display the name of the class
            GameTextForPlayer(playerid, TXT_ClassPolice, 3000, 4);
			// Store the class for the player (police)
			APlayerData[playerid][PlayerClass] = ClassPolice;
		}
		case 14, 15, 16: // Classes that will be mafia
		{
			// Display the name of the class
			GameTextForPlayer(playerid, TXT_ClassMafia, 3000, 4);
			// Store the class for the player (mafia)
			APlayerData[playerid][PlayerClass] = ClassMafia;
		}
		case 17, 18: // Classes that will be courier
		{
			// Display the name of the class
			GameTextForPlayer(playerid, TXT_ClassCourier, 3000, 4);
			// Store the class for the player (courier)
			APlayerData[playerid][PlayerClass] = ClassCourier;
		}
		case 19: // Classes that will be assistance
		{
			// Display the name of the class
			GameTextForPlayer(playerid, TXT_ClassAssistance, 3000, 4);
			// Store the class for the player (assistance)
			APlayerData[playerid][PlayerClass] = ClassAssistance;
		}
		case 20, 21, 22: // Classes that will be roadworker
		{
			// Display the name of the class
			GameTextForPlayer(playerid, TXT_ClassRoadWorker, 3000, 4);
			// Store the class for the player (roadworker)
			APlayerData[playerid][PlayerClass] = ClassRoadWorker;
		}
	}

	return 1;
}



// This callback is called when the player attempts to spawn via class-selection
public OnPlayerRequestSpawn(playerid)
{
	if (APlayerData[playerid][LoggedIn] != true) {
		return 0;
	}

	new Index, Float:x, Float:y, Float:z, Float:Angle, Name[MAX_PLAYER_NAME], Msg[128];

	// Get the player's name
	GetPlayerName(playerid, Name, sizeof(Name));

	// Choose a random spawnlocation based on the player's class
	switch (APlayerData[playerid][PlayerClass])
	{
		case ClassTruckDriver:
		{
			Index = random(sizeof(ASpawnLocationsTrucker)); // Get a random array-index to chose a random spawnlocation
			x = ASpawnLocationsTrucker[Index][SpawnX]; // Get the X-position for the spawnlocation
			y = ASpawnLocationsTrucker[Index][SpawnY]; // Get the Y-position for the spawnlocation
			z = ASpawnLocationsTrucker[Index][SpawnZ]; // Get the Z-position for the spawnlocation
			Angle = ASpawnLocationsTrucker[Index][SpawnAngle]; // Get the rotation-angle for the spawnlocation
			format(Msg, sizeof(Msg), "{00FF00}Player {FFFF00}%s{00FF00} joined {FFFF00}Trucker class", Name);
		}
		case ClassBusDriver:
		{
			Index = random(sizeof(ASpawnLocationsBusDriver));
			x = ASpawnLocationsBusDriver[Index][SpawnX]; // Get the X-position for the spawnlocation
			y = ASpawnLocationsBusDriver[Index][SpawnY]; // Get the Y-position for the spawnlocation
			z = ASpawnLocationsBusDriver[Index][SpawnZ]; // Get the Z-position for the spawnlocation
			Angle = ASpawnLocationsBusDriver[Index][SpawnAngle]; // Get the rotation-angle for the spawnlocation
			format(Msg, sizeof(Msg), "{00FF00}Player {FFFF00}%s{00FF00} joined {FFFF00}Busdriver class", Name);
		}
		case ClassPilot:
		{
			Index = random(sizeof(ASpawnLocationsPilot));
			x = ASpawnLocationsPilot[Index][SpawnX]; // Get the X-position for the spawnlocation
			y = ASpawnLocationsPilot[Index][SpawnY]; // Get the Y-position for the spawnlocation
			z = ASpawnLocationsPilot[Index][SpawnZ]; // Get the Z-position for the spawnlocation
			Angle = ASpawnLocationsPilot[Index][SpawnAngle]; // Get the rotation-angle for the spawnlocation
			format(Msg, sizeof(Msg), "{00FF00}Player {FFFF00}%s{00FF00} joined {FFFF00}Pilot class", Name);
		}
		case ClassPolice:
		{
		    // Count the number of normal players (all classes except police) and count the amount of police players
		    new NormalPlayers, PolicePlayers, bool:CanSpawnAsCop = false;

			// Block this check if PlayersBeforePolice is set to 0 (this allows anyone to join as police)
			if (PlayersBeforePolice > 0)
			{
				// Loop through all players
				for (new pid; pid < MAX_PLAYERS; pid++)
				{
					// Exclude this player, as he doesn't have a class yet, he's still choosing here
					if (pid != playerid)
					{
					    // Also exclude all players who are still in the class-selection screen, as they don't have a class selected yet
					    if (GetPlayerInterior(pid) != 14)
					    {
							// Check if this player is logged in
							if (APlayerData[pid][LoggedIn] == true)
							{
								// Count the amount of normal players and police players
								switch (APlayerData[pid][PlayerClass])
								{
									case ClassPolice:
									    PolicePlayers++;
									case ClassTruckDriver, ClassBusDriver, ClassPilot, ClassMafia, ClassCourier, ClassAssistance, ClassRoadWorker:
								    	NormalPlayers++;
								}
							}
						}
					}
				}
				// Check if there are less police players than allowed
				if (PolicePlayers < (NormalPlayers / PlayersBeforePolice))
				    CanSpawnAsCop = true; // There are less police players than allowed, so the player can choose this class
				else
				    CanSpawnAsCop = false; // The maximum amount of police players has been reached, the player can't choose to be a cop

				// Check if the player isn't allowed to spawn as police
				if (CanSpawnAsCop == false)
				{
					// Let the player know the maximum amount of cops has been reached
					GameTextForPlayer(playerid, "Maximum amount of cops already reached", 5000, 4);
					SendClientMessage(playerid, COLOR_RED, "The maximum amount of cops has been reached already, please select another class");
					return 0; // Don't allow the player to spawn as police player
				}
			}

			// If the player has less than 100 scorepoints
		    if (APlayerData[playerid][PlayerScore] < 100)
		    {
				// Let the player know he needs 100 scorepoints
				GameTextForPlayer(playerid, "You need 100 scorepoints for police class", 5000, 4);
				SendClientMessage(playerid, COLOR_RED, "You need 100 scorepoints for police class");
				return 0; // Don't allow the player to spawn as police player
		    }
			// If the player has a wanted level
		    if (GetPlayerWantedLevel(playerid) > 0)
		    {
				// Let the player know he cannot have a wanted level to join police
				GameTextForPlayer(playerid, "You are not allowed to choose police class when you're wanted", 5000, 4);
				SendClientMessage(playerid, COLOR_RED, "You are not allowed to choose police class when you're wanted");
				return 0; // Don't allow the player to spawn as police player
		    }

			Index = random(sizeof(ASpawnLocationsPolice));
			x = ASpawnLocationsPolice[Index][SpawnX]; // Get the X-position for the spawnlocation
			y = ASpawnLocationsPolice[Index][SpawnY]; // Get the Y-position for the spawnlocation
			z = ASpawnLocationsPolice[Index][SpawnZ]; // Get the Z-position for the spawnlocation
			Angle = ASpawnLocationsPolice[Index][SpawnAngle]; // Get the rotation-angle for the spawnlocation
			format(Msg, sizeof(Msg), "{00FF00}Player {FFFF00}%s{00FF00} joined {FFFF00}Police class", Name);
		}
		case ClassMafia:
		{
			Index = random(sizeof(ASpawnLocationsMafia));
			x = ASpawnLocationsMafia[Index][SpawnX]; // Get the X-position for the spawnlocation
			y = ASpawnLocationsMafia[Index][SpawnY]; // Get the Y-position for the spawnlocation
			z = ASpawnLocationsMafia[Index][SpawnZ]; // Get the Z-position for the spawnlocation
			Angle = ASpawnLocationsMafia[Index][SpawnAngle]; // Get the rotation-angle for the spawnlocation
			format(Msg, sizeof(Msg), "{00FF00}Player {FFFF00}%s{00FF00} joined {FFFF00}Mafia class", Name);
		}
		case ClassCourier:
		{
			Index = random(sizeof(ASpawnLocationsCourier));
			x = ASpawnLocationsCourier[Index][SpawnX]; // Get the X-position for the spawnlocation
			y = ASpawnLocationsCourier[Index][SpawnY]; // Get the Y-position for the spawnlocation
			z = ASpawnLocationsCourier[Index][SpawnZ]; // Get the Z-position for the spawnlocation
			Angle = ASpawnLocationsCourier[Index][SpawnAngle]; // Get the rotation-angle for the spawnlocation
			format(Msg, sizeof(Msg), "{00FF00}Player {FFFF00}%s{00FF00} joined {FFFF00}Courier class", Name);
		}
		case ClassAssistance:
		{
			Index = random(sizeof(ASpawnLocationsAssistance));
			x = ASpawnLocationsAssistance[Index][SpawnX]; // Get the X-position for the spawnlocation
			y = ASpawnLocationsAssistance[Index][SpawnY]; // Get the Y-position for the spawnlocation
			z = ASpawnLocationsAssistance[Index][SpawnZ]; // Get the Z-position for the spawnlocation
			Angle = ASpawnLocationsAssistance[Index][SpawnAngle]; // Get the rotation-angle for the spawnlocation
			format(Msg, sizeof(Msg), "{00FF00}Player {FFFF00}%s{00FF00} joined {FFFF00}Assistance class", Name);
		}
		case ClassRoadWorker:
		{
			Index = random(sizeof(ASpawnLocationsRoadWorker));
			x = ASpawnLocationsRoadWorker[Index][SpawnX]; // Get the X-position for the spawnlocation
			y = ASpawnLocationsRoadWorker[Index][SpawnY]; // Get the Y-position for the spawnlocation
			z = ASpawnLocationsRoadWorker[Index][SpawnZ]; // Get the Z-position for the spawnlocation
			Angle = ASpawnLocationsRoadWorker[Index][SpawnAngle]; // Get the rotation-angle for the spawnlocation
			format(Msg, sizeof(Msg), "{00FF00}Player {FFFF00}%s{00FF00} joined {FFFF00}Roadworker class", Name);
		}
	}

	// Spawn the player with his chosen skin at a random location based on his class
	SetSpawnInfo(playerid, 0, GetPlayerSkin(playerid), x, y, z, Angle, 0, 0, 0, 0, 0, 0);
	// Send the message to all players (who joined which class)
	SendClientMessageToAll(COLOR_WHITE, Msg);

    return 1;
}



// This callback gets called when a vehicle respawns at it's spawn-location (where it was created)
public OnVehicleSpawn(vehicleid)
{
	// Set the vehicle as not-wanted by the mafia
	AVehicleData[vehicleid][MafiaLoad] = false;
	// Also reset the fuel to maximum (only for non-owned vehicles)
	if (AVehicleData[vehicleid][Owned] == false)
		AVehicleData[vehicleid][Fuel] = MaxFuel;

	// Re-apply the paintjob (if any was applied)
	if (AVehicleData[vehicleid][PaintJob] != 0)
	{
	    // Re-apply the paintjob
		ChangeVehiclePaintjob(vehicleid, AVehicleData[vehicleid][PaintJob] - 1);
	}

	// Also update the car-color
	ChangeVehicleColor(vehicleid, AVehicleData[vehicleid][Color1], AVehicleData[vehicleid][Color2]);

	// Re-add all components that were installed (if they were there)
	for (new i; i < 14; i++)
	{
		// Remove all mods from the vehicle (all added mods applied by hackers will hopefully be removed this way when the vehicle respawns)
        RemoveVehicleComponent(vehicleid, GetVehicleComponentInSlot(vehicleid, i));

	    // Check if the componentslot has a valid component-id
		if (AVehicleData[vehicleid][Components][i] != 0)
	        AddVehicleComponent(vehicleid, AVehicleData[vehicleid][Components][i]); // Add the component to the vehicle
	}

    return 1;
}



// This callback is called when the vehicle leaves a mod shop
public OnVehicleRespray(playerid, vehicleid, color1, color2)
{
	// Setup local variables
	new Message[128];

	// Let the player pay for changing the color (if they have been changed)
	if ((AVehicleData[vehicleid][Color1] != color1) || (AVehicleData[vehicleid][Color2] != color2))
	{
		RewardPlayer(playerid, -PRICE_RESPRAY, 0);
		format(Message, sizeof(Message), TXT_VehColorChangePaid, PRICE_RESPRAY);
		return SendClientMessage(playerid, COLOR_GREEN, Message);
	}

	// Save the colors
	AVehicleData[vehicleid][Color1] = color1;
	AVehicleData[vehicleid][Color2] = color2;

	// If the primary color is black, remove the paintjob
	if (color1 == 0)
		AVehicleData[vehicleid][PaintJob] = 0;

	return 1;
}



// This callback gets called when a player enters or exits a mod-shop
public OnEnterExitModShop(playerid, enterexit, interiorid)
{
	return 1;
}



// This callback gets called whenever a player mods his vehicle
public OnVehicleMod(playerid, vehicleid, componentid)
{
	// When the player changes a component of his vehicle, reduce the price of the component from the player's money
	APlayerData[playerid][PlayerMoney] = APlayerData[playerid][PlayerMoney] - AVehicleModPrices[componentid - 1000];

	// Store the component in the AVehicleData array
	AVehicleData[vehicleid][Components][GetVehicleComponentType(componentid)] = componentid;

	return 1;
}



// This callback gets called whenever a player VIEWS at a paintjob in a mod garage (viewing automatically applies it)
public OnVehiclePaintjob(playerid, vehicleid, paintjobid)
{
	// Store the paintjobid for the vehicle (add 1 to the value, otherwise checking for an applied paintjob is difficult)
    AVehicleData[vehicleid][PaintJob] = paintjobid + 1;

	return 1;
}



// This callback gets called whenever a player enters a vehicle
public OnPlayerEnterVehicle(playerid, vehicleid, ispassenger)
{
	// Setup local variables
	new engine, lights, alarm, doors, bonnet, boot, objective;

	// Check if the vehicle has fuel
	if (AVehicleData[vehicleid][Fuel] > 0)
	{
		// Start the engine and turn on the lights
		GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
		SetVehicleParamsEx(vehicleid, 1, 1, alarm, doors, bonnet, boot, objective);
	}

	// Store the player's current location and interior-id, otherwise anti-airbreak hack code could kick you
	GetPlayerPos(playerid, APlayerData[playerid][PreviousX], APlayerData[playerid][PreviousY], APlayerData[playerid][PreviousZ]);
	APlayerData[playerid][PreviousInt] = GetPlayerInterior(playerid);

	return 1;
}



// This callback gets called when a player exits his vehicle
public OnPlayerExitVehicle(playerid, vehicleid)
{
	// Setup local variables
	new engine, lights, alarm, doors, bonnet, boot, objective, Message[128];

	// Check if the player is the driver of the vehicle
	if (GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
	{
		// Turn off the lights and engine
		GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
		SetVehicleParamsEx(vehicleid, 0, 0, alarm, doors, bonnet, boot, objective);
	}

	// Chech if the player is a pilot
	if (APlayerData[playerid][PlayerClass] == ClassPilot)
	{
	    // If the pilot started a job --> as soon as a pilot leaves his plane while doing a job, he fails his mission
		if (APlayerData[playerid][JobStarted] == true)
		{
		    // End the job (clear data)
			Pilot_EndJob(playerid);
			format(Message, sizeof(Message), TXT_FailedMission, PRICE_FAILED_JOB);
			// Inform the player that he failed the mission
			GameTextForPlayer(playerid, Message, 5000, 4);
			// Reduce the player's cash
			RewardPlayer(playerid, -PRICE_FAILED_JOB, 0);
		}
	}

	return 1;
}



// This callback gets called whenever a vehicle enters the water or is destroyed (explodes)
public OnVehicleDeath(vehicleid)
{
	// Get the houseid to which this vehicle belongs
	new HouseID = AVehicleData[vehicleid][BelongsToHouse];

	// Check if this vehicle belongs to a house
	if (HouseID != 0)
	{
		// If the house doesn't have insurance for it's vehicles
		if (AHouseData[HouseID][Insurance] == 0)
		{
		    // Delete the vehicle, clear the data and remove it from the house it belongs to
			Vehicle_Delete(vehicleid);

		    // Save the house (and linked vehicles)
		    HouseFile_Save(HouseID);
		}
	}

	return 1;
}



// This callback gets called when the player changes state
public OnPlayerStateChange(playerid,newstate,oldstate)
{
	// Setup local variables
	new vid, Name[MAX_PLAYER_NAME], Msg[128], engine, lights, alarm, doors, bonnet, boot, objective;

	switch (newstate)
	{
		case PLAYER_STATE_DRIVER: // Player became the driver of a vehicle
		{
			// Get the ID of the player's vehicle
			vid = GetPlayerVehicleID(playerid);
			// Get the player's name (the one who is trying to enter the vehicle)
			GetPlayerName(playerid, Name, sizeof(Name));

			// Check if the vehicle is owned
			if (AVehicleData[vid][Owned] == true)
			{
				// Check if the vehicle is owned by somebody else (strcmp will not be 0)
				if (strcmp(AVehicleData[vid][Owner], Name, false) != 0)
				{
					// Force the player out of the vehicle
					RemovePlayerFromVehicle(playerid);
					// Turn off the lights and engine
					GetVehicleParamsEx(vid, engine, lights, alarm, doors, bonnet, boot, objective);
					SetVehicleParamsEx(vid, 0, 0, alarm, doors, bonnet, boot, objective);
					// Let the player know he cannot use somebody else's vehicle
					format(Msg, sizeof(Msg), TXT_SpeedometerCannotUseVehicle, AVehicleData[vid][Owner]);
					SendClientMessage(playerid, COLOR_WHITE, Msg);
				}

				// Check if the vehicle is clamped
				if (AVehicleData[vid][Clamped] == true)
				{
					// Force the player out of the vehicle
					RemovePlayerFromVehicle(playerid);
					// Turn off the lights and engine
					GetVehicleParamsEx(vid, engine, lights, alarm, doors, bonnet, boot, objective);
					SetVehicleParamsEx(vid, 0, 0, alarm, doors, bonnet, boot, objective);
					// Let the player know he cannot use a clamped vehicle
					format(Msg, sizeof(Msg), TXT_SpeedometerClampedVehicle);
					SendClientMessage(playerid, COLOR_WHITE, Msg);
					format(Msg, sizeof(Msg), TXT_SpeedometerClampedVehicle2);
					SendClientMessage(playerid, COLOR_WHITE, Msg);
				}
			}

			// Check if the player is not a cop
			if (APlayerData[playerid][PlayerClass] != ClassPolice)
			{
				// First check if the vehicle is a static vehicle (player can still use a bought cop-car that he bought in his house,
				// as a bought vehicle isn't static)
				if (AVehicleData[vid][StaticVehicle] == true)
				{
					// Check if the entered vehicle is a cop vehicle
					switch (GetVehicleModel(vid))
					{
						case VehiclePoliceLSPD, VehiclePoliceSFPD, VehiclePoliceLVPD, VehicleHPV1000, VehiclePoliceRanger:
						{
							// Force the player out of the vehicle
							RemovePlayerFromVehicle(playerid);
							// Turn off the lights and engine
							GetVehicleParamsEx(vid, engine, lights, alarm, doors, bonnet, boot, objective);
							SetVehicleParamsEx(vid, 0, 0, alarm, doors, bonnet, boot, objective);
							// Let the player know he cannot use a cop car
							SendClientMessage(playerid, COLOR_RED, "You cannot use a police vehicle");
						}
					}
				}
			}

			// Check if the player is not a pilot
			if (APlayerData[playerid][PlayerClass] != ClassPilot)
			{
				// First check if the vehicle is a static vehicle (player can still use a bought plane that he bought in his house,
				// as a bought vehicle isn't static)
				if (AVehicleData[vid][StaticVehicle] == true)
				{
					// Check if the entered vehicle is a plane or helicopter vehicle
					switch (GetVehicleModel(vid))
					{
						case VehicleShamal, VehicleNevada, VehicleStuntPlane, VehicleDodo, VehicleMaverick, VehicleCargobob:
						{
							// Force the player out of the vehicle
							RemovePlayerFromVehicle(playerid);
							// Turn off the lights and engine
							GetVehicleParamsEx(vid, engine, lights, alarm, doors, bonnet, boot, objective);
							SetVehicleParamsEx(vid, 0, 0, alarm, doors, bonnet, boot, objective);
							// Let the player know he cannot use a cop car
							SendClientMessage(playerid, COLOR_RED, "You cannot use a pilot vehicle");
						}
					}
				}
			}
		}
	}

	return 1;
}



// This callback gets called whenever a player presses a key
public OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	// Debug the keypresses
//	DebugKeys(playerid, newkeys, oldkeys);

	// ****************************************************************************************
	// NOTE: the keys are messed up, so the code may look strange when testing for certain keys
	// ****************************************************************************************

	// Fining and jailing players when you're police and press the correct keys
	// Check the class of the player
	switch (APlayerData[playerid][PlayerClass])
	{
		case ClassPolice:
		{
		    // If the police-player pressed the RMB key (AIM key) when OUTSIDE his vehicle
			if (((newkeys & KEY_HANDBRAKE) && !(oldkeys & KEY_HANDBRAKE)) && (GetPlayerVehicleID(playerid) == 0))
				Police_FineNearbyPlayers(playerid);

		    // If the police-player pressed the LCTRL (SECUNDAIRY key) key when INSIDE his vehicle
			if (((newkeys & KEY_ACTION) && !(oldkeys & KEY_ACTION)) && (GetPlayerVehicleID(playerid) != 0))
				Police_WarnNearbyPlayers(playerid);
		}
		case ClassAssistance:
		{
		    // If the assistance-player pressed the RMB key (AIM key) when OUTSIDE his vehicle
			if (((newkeys & KEY_HANDBRAKE) && !(oldkeys & KEY_HANDBRAKE)) && (GetPlayerVehicleID(playerid) == 0))
				Assistance_FixVehicle(playerid);

		    // If the police-player pressed the LCTRL (SECUNDAIRY key) key when INSIDE his vehicle
			if (((newkeys & KEY_ACTION) && !(oldkeys & KEY_ACTION)) && (GetPlayerVehicleID(playerid) != 0))
				Assistance_FixOwnVehicle(playerid);
		}
	}

	// Trying to attach the closest vehicle to the towtruck when the player pressed FIRE when inside a towtruck
	// Check if the player is inside a towtruck
	if(GetVehicleModel(GetPlayerVehicleID(playerid)) == VehicleTowTruck)
	{
		// Check if the player pushed the fire-key
		if(newkeys & KEY_FIRE)
		{
			// Get the vehicle-id of the closest vehicle
			new closest = GetClosestVehicle(playerid);
			if(VehicleToPlayer(playerid, closest) < 10) // Check if the closest vehicle is within 10m from the player
				AttachTrailerToVehicle(closest, GetPlayerVehicleID(playerid)); // Attach the vehicle to the towtruck
		}
	}

	// Refuel a vehicle when driving a vehicle and pressing the HORN key
	// Check if the player presses the HORN key
	if ((newkeys & KEY_CROUCH) && !(oldkeys & KEY_CROUCH))
	{
		// Check if the player is driving a vehicle
		if (GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
		{
			// Loop through all ARefuelPickups
			for (new i; i < sizeof(ARefuelPickups); i++)
			{
				// Check if the player is in range of a refuelpickup
				if(IsPlayerInRangeOfPoint(playerid, 2.5, ARefuelPickups[i][pux], ARefuelPickups[i][puy], ARefuelPickups[i][puz]))
				{
					// Show a message that the player's vehicle is refuelling
					GameTextForPlayer(playerid, TXT_Refuelling, 3000, 4);
					// Don't allow the player to move again (the timer will allow it after refuelling)
					TogglePlayerControllable(playerid, 0);
				       // Start a timer (let the player wait until the vehicle is refuelled)
				    SetTimerEx("RefuelVehicle", 5000, false, "i", playerid);
				    // Stop the search
					break;
				}
			}
		}
	}

	return 1;
}



forward VehicleToPlayer(playerid,vehicleid);
// Get the distance between the vehicle and the player
public VehicleToPlayer(playerid, vehicleid)
{
	// Setup local variables
	new Float:pX, Float:pY, Float:pZ, Float:cX, Float:cY, Float:cZ, Float:distance;
	// Get the player position
	GetPlayerPos(playerid, pX, pY, pZ);
	// Get the vehicle position
	GetVehiclePos(vehicleid, cX, cY, cZ);
	// Calculate the distance
	distance = floatsqroot(floatpower(floatabs(floatsub(cX, pX)), 2) + floatpower(floatabs(floatsub(cY, pY)), 2) + floatpower(floatabs(floatsub(cZ, pZ)), 2));
	// Return the distance to the calling routine
	return floatround(distance);
}



forward GetClosestVehicle(playerid);
// Find the vehicle closest to the player
public GetClosestVehicle(playerid)
{
	// Setup local variables
	new Float:distance = 99999.000+1, Float:distance2, result = -1;
	// Loop through all vehicles
	for(new i = 0; i < MAX_VEHICLES; i++)
	{
		// First check if the player isn't driving the current vehicle that needs to be checked for it's distance to the player
		if (GetPlayerVehicleID(playerid) != i)
		{
			// Get the distance between player and vehicle
			distance2 = VehicleToPlayer(playerid, i);
			// Check if the distance is smaller than the previous distance
			if(distance2 < distance)
			{
				// Store the distance
				distance = distance2;
				// Store the vehicle-id
				result = i;
			}
		}
	}

	// Return the vehicle-id of the closest vehicle
	return result;
}



// This function is used to debug the key-presses
stock DebugKeys(playerid, newkeys, oldkeys)
{
	// Debug keys
	if ((newkeys & KEY_FIRE) && !(oldkeys & KEY_FIRE))
		SendClientMessage(playerid, COLOR_BLUE, "You pressed the KEY_FIRE key");
	if ((newkeys & KEY_ACTION) && !(oldkeys & KEY_ACTION))
		SendClientMessage(playerid, COLOR_BLUE, "You pressed the KEY_ACTION key");
	if ((newkeys & KEY_CROUCH) && !(oldkeys & KEY_CROUCH))
		SendClientMessage(playerid, COLOR_BLUE, "You pressed the KEY_CROUCH key");
	if ((newkeys & KEY_SPRINT) && !(oldkeys & KEY_SPRINT))
		SendClientMessage(playerid, COLOR_BLUE, "You pressed the KEY_SPRINT key");
	if ((newkeys & KEY_SECONDARY_ATTACK) && !(oldkeys & KEY_SECONDARY_ATTACK))
		SendClientMessage(playerid, COLOR_BLUE, "You pressed the KEY_SECONDARY_ATTACK key");
	if ((newkeys & KEY_JUMP) && !(oldkeys & KEY_JUMP))
		SendClientMessage(playerid, COLOR_BLUE, "You pressed the KEY_JUMP key");
	if ((newkeys & KEY_LOOK_RIGHT) && !(oldkeys & KEY_LOOK_RIGHT))
		SendClientMessage(playerid, COLOR_BLUE, "You pressed the KEY_LOOK_RIGHT key");
	if ((newkeys & KEY_HANDBRAKE) && !(oldkeys & KEY_HANDBRAKE))
		SendClientMessage(playerid, COLOR_BLUE, "You pressed the KEY_HANDBRAKE key");
	if ((newkeys & KEY_LOOK_LEFT) && !(oldkeys & KEY_LOOK_LEFT))
		SendClientMessage(playerid, COLOR_BLUE, "You pressed the KEY_LOOK_LEFT key");
	if ((newkeys & KEY_SUBMISSION) && !(oldkeys & KEY_SUBMISSION))
		SendClientMessage(playerid, COLOR_BLUE, "You pressed the KEY_SUBMISSION key");
	if ((newkeys & KEY_LOOK_BEHIND) && !(oldkeys & KEY_LOOK_BEHIND))
		SendClientMessage(playerid, COLOR_BLUE, "You pressed the KEY_LOOK_BEHIND key");
	if ((newkeys & KEY_WALK) && !(oldkeys & KEY_WALK))
		SendClientMessage(playerid, COLOR_BLUE, "You pressed the KEY_WALK key");
	if ((newkeys & KEY_ANALOG_UP) && !(oldkeys & KEY_ANALOG_UP))
		SendClientMessage(playerid, COLOR_BLUE, "You pressed the KEY_ANALOG_UP key");
	if ((newkeys & KEY_ANALOG_DOWN) && !(oldkeys & KEY_ANALOG_DOWN))
		SendClientMessage(playerid, COLOR_BLUE, "You pressed the KEY_ANALOG_DOWN key");
	if ((newkeys & KEY_ANALOG_LEFT) && !(oldkeys & KEY_ANALOG_LEFT))
		SendClientMessage(playerid, COLOR_BLUE, "You pressed the KEY_ANALOG_LEFT key");
	if ((newkeys & KEY_ANALOG_RIGHT) && !(oldkeys & KEY_ANALOG_RIGHT))
		SendClientMessage(playerid, COLOR_BLUE, "You pressed the KEY_ANALOG_RIGHT key");
	if ((newkeys & KEY_UP) && !(oldkeys & KEY_UP))
		SendClientMessage(playerid, COLOR_BLUE, "You pressed the KEY_UP key");
	if ((newkeys & KEY_DOWN) && !(oldkeys & KEY_DOWN))
		SendClientMessage(playerid, COLOR_BLUE, "You pressed the KEY_DOWN key");
	if ((newkeys & KEY_LEFT) && !(oldkeys & KEY_LEFT))
		SendClientMessage(playerid, COLOR_BLUE, "You pressed the KEY_LEFT key");
	if ((newkeys & KEY_RIGHT) && !(oldkeys & KEY_RIGHT))
		SendClientMessage(playerid, COLOR_BLUE, "You pressed the KEY_RIGHT key");

	return 1;
}
