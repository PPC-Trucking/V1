// This file holds an array of defined Trailers, used in the /Trailer dialog



enum TTrailer
{
	TrailerName[50], // Holds the name of the Trailer
	TrailerModel // Holds the model-ID of the Trailer
}

new ATrailers[][TTrailer] =
{
	{"Article Trailer", 435},	{"Article Trailer 2", 450}, 	{"Article Trailer 3", 591},		{"Baggage Trailer A", 606},	// ID 0, 1, 2, 3
	{"Baggage Trailer B", 607},	{"Farm Trailer", 610},			{"Petrol Trailer", 584},		{"Stairs Trailer", 608},    // ID 4, 5, 6, 7
	{"Utility Trailer", 611}                                                                                                // ID 8
};



// This function creates a list of Trailers, starting from the FirstTrailer and automatically shows the dialog
TrailerList_Create(playerid)
{
	// Setup local variables
	new Counter, TrailerList[500], DialogTitle[128];

	// Only add 10 Trailers to the list, starting from the FirstTrailer
	for (new i = APlayerData[playerid][DialogTrailerFirstTrailer]; i < sizeof(ATrailers); i++)
	{
		// Increase a counter (which holds the number of Trailers that have been added to the list
		Counter++;

		// Check if the maximum hasn't been reached yet
		if (Counter <= 10)
		{
		    // Add the Trailername to the list
		    if (strlen(TrailerList) == 0) // If this is the start of the list (no Trailers have been added yet)
				format(TrailerList, sizeof(TrailerList), "%s", ATrailers[i][TrailerName]); // Add the name of the Trailer at the start of the Trailerlist
		    else
				format(TrailerList, sizeof(TrailerList), "%s%s%s", TrailerList, "\n", ATrailers[i][TrailerName]); // Add the name of the next Trailer to the list on the next line
		}
		else // 10 Trailers have been added to the list (now Counter = 11)
		{
		    // Add an empty line and "Next..." to the list to let the player know there are more Trailers to choose from
			format(TrailerList, sizeof(TrailerList), "%s%s%s", TrailerList, "\n \n", TXT_DialogEntryNext);
			// Also stop the For-loop
			break;
		}
	}

	// Construct the title for the dialog (to include a page number)
	format(DialogTitle, sizeof(DialogTitle), TXT_DialogTrailerTitle, (APlayerData[playerid][DialogTrailerFirstTrailer] / 10) + 1);
	// Ask which Trailer the player wants to have by showing the dialog
	ShowPlayerDialog(playerid, DialogTrailer, DIALOG_STYLE_LIST, DialogTitle, TrailerList, TXT_DialogButtonSpawn, TXT_DialogButtonCancel);

	return 1;
}

