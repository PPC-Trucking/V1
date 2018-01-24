// This file holds an array of defined Planes, used in the /Plane dialog



enum TPlane
{
	PlaneName[50], // Holds the name of the Plane
	PlaneModel // Holds the model-ID of the Plane
}

new APlanes[][TPlane] =
{
	{"Andromada", 592},		{"AT400", 577}, 	{"Beagle", 511},				{"Cargobob", 548},          // ID 0, 1, 2, 3
	{"Cropduster", 512},	{"Dodo", 593},		{"Hunter", 425},				{"Hydra", 520},             // ID 4, 5, 6, 7
	{"Leviathan", 417},		{"Maverick", 487},	{"Nevada", 553},				{"Police Maverick", 497},   // ID 8, 9, 10, 11
	{"Raindance", 563},		{"Rustler", 476},	{"SAN News Maverick", 488},		{"Seasparrow", 447},        // ID 12, 13, 14, 15
	{"Shamal", 519},		{"Skimmer", 460},	{"Sparrow", 469},				{"Stuntplane", 513}         // ID 16, 17, 18, 19
};



// This function creates a list of Planes, starting from the FirstPlane and automatically shows the dialog
PlaneList_Create(playerid)
{
	// Setup local variables
	new Counter, PlaneList[500], DialogTitle[128];

	// Only add 10 Planes to the list, starting from the FirstPlane
	for (new i = APlayerData[playerid][DialogPlaneFirstPlane]; i < sizeof(APlanes); i++)
	{
		// Increase a counter (which holds the number of Planes that have been added to the list
		Counter++;

		// Check if the maximum hasn't been reached yet
		if (Counter <= 10)
		{
		    // Add the Planename to the list
		    if (strlen(PlaneList) == 0) // If this is the start of the list (no Planes have been added yet)
				format(PlaneList, sizeof(PlaneList), "%s", APlanes[i][PlaneName]); // Add the name of the Plane at the start of the Planelist
		    else
				format(PlaneList, sizeof(PlaneList), "%s%s%s", PlaneList, "\n", APlanes[i][PlaneName]); // Add the name of the next Plane to the list on the next line
		}
		else // 10 Planes have been added to the list (now Counter = 11)
		{
		    // Add an empty line and "Next..." to the list to let the player know there are more Planes to choose from
			format(PlaneList, sizeof(PlaneList), "%s%s%s", PlaneList, "\n \n", TXT_DialogEntryNext);
			// Also stop the For-loop
			break;
		}
	}

	// Construct the title for the dialog (to include a page number)
	format(DialogTitle, sizeof(DialogTitle), TXT_DialogPlaneTitle, (APlayerData[playerid][DialogPlaneFirstPlane] / 10) + 1);
	// Ask which Plane the player wants to have by showing the dialog
	ShowPlayerDialog(playerid, DialogPlane, DIALOG_STYLE_LIST, DialogTitle, PlaneList, TXT_DialogButtonSpawn, TXT_DialogButtonCancel);

	return 1;
}

