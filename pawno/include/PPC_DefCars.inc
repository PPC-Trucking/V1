// This file holds an array of defined cars, used in the /car dialog



enum TCar
{
	CarName[50], // Holds the name of the car
	CarModel // Holds the model-ID of the car
}

new ACars[][TCar] =
{
	{"Admiral", 445},			{"Alpha", 602},             {"Ambulance", 416},			{"Baggage", 485},           // ID 0, 1, 2, 3
	{"Bandito", 568},			{"Banshee", 429},			{"Barracks", 433},			{"Benson", 499},            // ID 4, 5, 6, 7
	{"Berkley's RC Van", 459},	{"BF Injection", 424},  	{"Blade", 536},				{"Blista Compact", 496},    // ID 8, 9, 10, 11
	{"Bloodring Banger", 504},	{"Bobcat", 422},			{"Boxville 1", 498},		{"Boxville 2", 609},        // ID 12, 13, 14, 15
	{"Bravura", 401},			{"Broadway", 575},			{"Buccaneer", 518},			{"Buffalo", 402},           // ID 16, 17, 18, 19
	{"Bullet", 541},            {"Burrito", 482},			{"Bus", 431},				{"Cabbie", 438},            // ID 20, 21, 22, 23
	{"Caddy", 457},				{"Cadrona", 527},   		{"Camper", 483},			{"Cement Truck", 524},      // ID 24, 25, 26, 27
	{"Cheetah", 415},			{"Clover", 542},			{"Club", 589},				{"Coach", 437},             // ID 28, 29, 30, 31
	{"Combine Harvester", 532},	{"Comet", 480},				{"DFT-30", 578},			{"Dozer", 486},             // ID 32, 33, 34, 35
	{"Dumper", 406},            {"Dune", 573},				{"Elegant", 507},			{"Elegy", 562},             // ID 36, 37, 38, 39
	{"Emperor", 585},			{"Enforcer", 427},			{"Esperanto", 419},			{"Euros", 587},             // ID 40, 41, 42, 43
	{"FBI Rancher", 490},		{"FBI Truck", 528},			{"Feltzer", 533},			{"Firetruck", 407},         // ID 44, 45, 46, 47
	{"Firetruck LA", 544},		{"Flash", 565},				{"Flatbed", 455},			{"Forklift", 530},          // ID 48, 49, 50, 51
	{"Fortune", 526},			{"Glendale", 466},			{"Greenwood", 492},			{"Hermes", 474},            // ID 52, 53, 54, 55
	{"Hotdog", 588},			{"Hotknife", 434},			{"Hotring Racer 1", 494},	{"Hotring Racer 2", 502},   // ID 56, 57, 58, 59
	{"Hotring Racer 3", 503},	{"Huntley", 579},			{"Hustler", 545},			{"Infernus", 411},          // ID 60, 61, 62, 63
	{"Intruder", 546},			{"Jester", 559},            {"Journey", 508},			{"Kart", 571},              // ID 64, 65, 66, 67
	{"Landstalker", 400},       {"Linerunner", 403},		{"Majestic", 517},			{"Manana", 410},            // ID 68, 69, 70, 71
	{"Merit", 551},				{"Mesa", 500},				{"Monster", 444},			{"Monster A", 556},         // ID 72, 73, 74, 75
	{"Monster B", 557},         {"Moonbeam", 418},			{"Mower", 572},				{"Mr Whoopee", 423},        // ID 76, 77, 78, 79
	{"Mule", 414},  			{"Nebula", 516},            {"Newsvan", 582},			{"Oceanic", 467},           // ID 80, 81, 82, 83
	{"Packer", 443},			{"Patriot", 470},			{"Perenniel", 404},			{"Phoenix", 603},           // ID 84, 85, 86, 87
	{"Picador", 600},			{"Police Car (LSPD)", 596},	{"Police Car (LVPD)", 598},	{"Police Car (SFPD)", 597}, // ID 88, 89, 90, 91
	{"Police Ranger", 599},		{"Pony", 413},				{"Premier", 426},			{"Previon", 436},           // ID 92, 93, 94, 95
	{"Primo", 547},				{"Rancher 1", 489},			{"Rancher 2", 505},			{"Regina", 479},            // ID 96, 97, 98, 99
	{"Remington", 534},			{"Rhino", 432},             {"Roadtrain", 515},			{"Romero", 442},            // ID 100, 101, 102, 103
	{"Rumpo", 440},				{"Sabre", 475},				{"Sadler", 543},			{"Sandking", 495},          // ID 104, 105, 106, 107
	{"Savanna", 567},			{"Securicar", 428},			{"Sentinel", 405},			{"Slamvan", 535},           // ID 108, 109, 110, 111
	{"Solair", 458},			{"Stafford", 580},			{"Stallion", 439},			{"Stratum", 561},           // ID 112, 113, 114, 115
	{"Stretch", 409},			{"Sultan", 560},			{"Sunrise", 550},			{"Super GT", 506},          // ID 116, 117, 118, 119
	{"S.W.A.T.", 601},			{"Sweeper", 574},			{"Tahoma", 566},			{"Tampa", 549},             // ID 120, 121, 122, 123
	{"Tanker", 514},			{"Taxi", 420},				{"Tornado", 576},			{"Towtruck", 525},          // ID 124, 125, 126, 127
	{"Tractor", 531},			{"Trashmaster", 408},		{"Tug", 583},				{"Turismo", 451},           // ID 128, 129, 130, 131
	{"Uranus", 558},            {"Utility Van", 552},		{"Vincent", 540},			{"Virgo", 491},             // ID 132, 133, 134, 135
	{"Voodoo", 412},			{"Walton", 478},			{"Washington", 421},		{"Willard", 529},           // ID 136, 137, 138, 139
	{"Windsor", 555},		    {"Yankee", 456},            {"Yosemite", 554},			{"ZR-350", 477}             // ID 140, 141, 142, 143
};



// This function creates a list of cars, starting from the FirstCar and automatically shows the dialog
CarList_Create(playerid)
{
	// Setup local variables
	new Counter, CarList[500], DialogTitle[128];

	// Only add 10 cars to the list, starting from the FirstCar
	for (new i = APlayerData[playerid][DialogCarFirstCar]; i < sizeof(ACars); i++)
	{
		// Increase a counter (which holds the number of cars that have been added to the list
		Counter++;

		// Check if the maximum hasn't been reached yet
		if (Counter <= 10)
		{
		    // Add the carname to the list
		    if (strlen(CarList) == 0) // If this is the start of the list (no cars have been added yet)
				format(CarList, sizeof(CarList), "%s", ACars[i][CarName]); // Add the name of the car at the start of the carlist
		    else
				format(CarList, sizeof(CarList), "%s%s%s", CarList, "\n", ACars[i][CarName]); // Add the name of the next car to the list on the next line
		}
		else // 10 cars have been added to the list (now Counter = 11)
		{
		    // Add an empty line and "Next..." to the list to let the player know there are more cars to choose from
			format(CarList, sizeof(CarList), "%s%s%s", CarList, "\n \n", TXT_DialogEntryNext);
			// Also stop the For-loop
			break;
		}
	}

	// Construct the title for the dialog (to include a page number)
	format(DialogTitle, sizeof(DialogTitle), TXT_DialogCarTitle, (APlayerData[playerid][DialogCarFirstCar] / 10) + 1);
	// Ask which car the player wants to have by showing the dialog
	ShowPlayerDialog(playerid, DialogCar, DIALOG_STYLE_LIST, DialogTitle, CarList, TXT_DialogButtonSpawn, TXT_DialogButtonCancel);

	return 1;
}

