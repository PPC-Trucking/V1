// Setup a custom type that holds all data about a location on the map (used for missions)
enum TLocation
{
	LocationName[50], // The name of the location
	Float:LocX, // The X-position of the location
	Float:LocY, // The Y-position of the location
	Float:LocZ // The Z-position of the location
}

// Setup an array that holds all location-data (except taxi and busdriver classes, these have their own locations)
new ALocations[][TLocation] =
{
	// LocationName, LocationX, LocationY, LocationZ
	{"Dummy location", 0.0, 0.0, 0.0}, // Location 0 = dummy location (never used)

	{"LS Construction Site", 1859.0, -1314.0, 14.0}, // Location 1
	{"SF Construction Site", -2083.0, 209.0, 35.5}, // Location 2
	{"LV Construction Site", 2708.0, 878.0, 10.0}, // Location 3
	{"Avery Construction", 312.25, -233.75, 1.6}, // Location 4
	{"Clown's Pocket Constr.", 2467.0, 1923.0, 9.8}, // Location 5
	{"Solarin Industries", -1822.5, 41.25, 15.25}, // Location 6
	{"Blueberry Truck Terminal", 52.75, -278.0, 1.7}, // Location 7
	{"KACC Military Fuels", 2708.5, 2641.25, 11.0}, // Location 8
	{"SA Federal Mint", -2441.0, 521.5, 30.0}, // Location 9
	{"Angel Pine Sawmill", -1965.75, -2436.0, 30.7}, // Location 10
	{"Quarry", 587.25, 844.75, -42.5}, // Location 11
	{"Quarry Top", 370.0, 874.0, 20.5}, // Location 12
	{"Easter Bay Lumberyard", -763.5, -136.0, 65.7}, // Location 13
	{"LS Papermill", 2331.5, -2071.5, 13.6}, // Location 14
	{"Sprunk Factory", 1334.75, 287.0, 19.6}, // Location 15
	{"SF Medical Center", -2607.0, 684.0, 27.9}, // Location 16
	{"Angel Pine Junkyard", -1873.0, -1685.0, 21.8}, // Location 17
	{"LV Freight Depot", 599.5, 1224.75, 11.8}, // Location 18
	{"LS Garbage Compound", 2181.0, -1980.5, 13.6}, // Location 19
	{"LV Industrial Estate", 1635.75, 750.5, 11.0}, // Location 20
	{"EasterBoard Farm", -69.5, -35.75, 3.2}, // Location 21
	{"SF Hill Farm", -1423.5, -1469.0, 101.7}, // Location 22
	{"Pecker's Feed & Seed", -310.5, 2669.5, 62.7}, // Location 23
	{"San Fierro Port", -1691.0, 31.5, 3.6}, // Location 24
	{"LV Refinery", 262.5, 1453.75, 10.6}, // Location 25
	{"LS Refinery", 2484.0, -2085.0, 13.6}, // Location 26
	{"Countryside Gas Station", -86.0, -1192.75, 2.0}, // Location 27
	{"SF Xoomer Gas Station", -1659.0, 444.0, 7.2}, // Location 28
	{"SF Gas Station", -2421.5, 953.25, 45.3}, // Location 29
	{"LS Going Station", 1926.0, -1791.5, 13.5}, // Location 30
	{"Xoomer Gas Station", -1488.75, 1875.25, 32.7}, // Location 31
	{"Whetstone Gas Station", -1629.0, -2700.0, 48.6}, // Location 32
	{"Desert Gas Station", -1309.5, 2703.75, 50.1}, // Location 33
	{"Dilimore Gas Station", 664.5, -583.75, 16.4}, // Location 34
	{"Abandoned Airport Fuel", 387.9, 2590.25, 16.5}, // Location 35
	{"San Fierro International", -1152.25, -146.5, 14.2}, // Location 36
	{"Easter Bay Chemical", -1033.75, -625.25, 32.1}, // Location 37
	{"LS Ocean Docks", 2466.0, -2219.25, 13.6}, // Location 38
	{"Bioengineering Corp.", 1347.5, 349.5, 20.0}, // Location 39
	{"Greenglass College", 999.0, 1086.75, 11.0}, // Location 40
	{"Fleischberg Brewery", -149.75, -220.25, 1.5}, // Location 41
	{"Bayside Boatyard", -2430.75, 2310.5, 5.0}, // Location 42
	{"LV Butcher", 977.5, 2096.75, 11.0}, // Location 43
	{"SF Catering LTD", -1945.75, -1073.5, 31.0}, // Location 44
	{"LS Donut shop", 1025.75, -1355.0, 13.8}, // Location 45
	{"Porter's House", 2439.75, 114.5, 26.5}, // Location 46
	{"Chuckup LTD", 1470.5, 1017.5, 10.9}, // Location 47
	{"LS Warehouse", 2178.5, -2279.75, 13.6}, // Location 48
	{"SF Supa Save", -2463.5, 782.0, 35.2}, // Location 49
	{"Verona Mall", 1044.75, -1548.75, 13.6}, // Location 50
	{"Las Venturas Mall", 2867.25, 2586.0, 10.7}, // Location 51
	{"Vank Hoff Hotel", -2490.75, 337.5, 33.4}, // Location 52
	{"Emerald Mall", 2056.75, 2251.25, 10.7}, // Location 53
	{"LS Ocean Docks", 2574.25, -2223.75, 13.4}, // Location 54
	{"Bayside Warehouse", -2285.0, 2281.5, 5.0}, // Location 55
	{"Fleischberg Brewery", -18.5, -282.0, 5.5}, // Location 56
	{"Las Venturas Stadium", 1377.5, 2245.75, 10.9}, // Location 57
	{"P. Creek General Store", 2318.0, -92.5, 26.5}, // Location 58
	{"LV Dirtring Stadium", 1112.0, 1451.75, 5.9}, // Location 59
	{"The Pleasure Domes", -2697.0, 1402.5, 7.2}, // Location 60
	{"4 Dragons Casino", 1903.75, 960.5, 10.9}, // Location 61
	{"SF Financial Center", -2039.5, 467.5, 35.2}, // Location 62
	{"Area 69", 134.54, 1945.79, 19.00}, // Location 63

	{"Busstop", 1067.00, 1357.75, 10.7}, // Location 64
	{"Busstop", 1324.75, 1190.75, 10.7}, // Location 65
	{"Busstop", 1544.00, 1135.50, 10.7}, // Location 66
	{"Busstop", 1603.50, 1130.50, 10.7}, // Location 67
	{"Busstop", 1722.50, 1373.25, 10.5}, // Location 68
	{"Busstop", 1714.25, 1421.75, 10.5}, // Location 69
	{"Busstop", 1010.00, 1411.25, 10.7}, // Location 70
	{"Busstop", 1726.50, 1481.75, 10.7}, // Location 71
	{"Busstop", 1729.50, 1529.50, 10.7}, // Location 72
	{"Busstop", 1705.00, 1965.00, 10.7}, // Location 73
	{"Busstop", 1385.00, 1920.00, 10.7}, // Location 74
	{"Busstop", 1335.25, 2055.50, 10.7}, // Location 75
	{"Busstop", 1073.00, 2055.50, 10.7}, // Location 76
	{"Busstop", 1502.25, 2055.50, 10.7}, // Location 77
	{"Busstop", 1565.00, 2200.25, 10.7}, // Location 78
	{"Busstop", 1636.25, 2175.50, 10.7}, // Location 79
	{"Busstop", 1503.50, 2311.00, 10.7}, // Location 80
	{"Busstop", 1564.75, 2345.75, 10.7}, // Location 81
	{"Busstop", 1725.00, 2247.75, 10.7}, // Location 82
	{"Busstop", 1894.50, 2175.50, 10.7}, // Location 83
	{"Busstop", 1925.25, 2314.75, 10.7}, // Location 84
	{"Busstop", 1925.25, 2115.75, 10.8}, // Location 85
	{"Busstop", 2083.50, 2455.50, 10.7}, // Location 86
	{"Busstop", 2285.00, 2315.00, 10.7}, // Location 87
	{"Busstop", 2305.00, 2515.50, 10.7}, // Location 88
	{"Busstop", 2392.00, 2415.50, 10.7}, // Location 89
	{"Busstop", 2425.00, 2285.00, 10.7}, // Location 90
	{"Busstop", 2449.75, 2235.50, 10.7}, // Location 91
	{"Busstop", 2580.25, 2115.50, 10.7}, // Location 92
	{"Busstop", 2447.50, 2055.50, 10.7}, // Location 93
	{"Busstop", 2115.00, 755.50, 10.7}, // Location 94
	{"Busstop", 2019.75, 750.50, 10.7}, // Location 95
	{"Busstop", 1975.00, 775.50, 10.7}, // Location 96
	{"Busstop", 1885.00, 770.75, 10.7}, // Location 97
	{"Busstop", 1894.00, 635.75, 10.7}, // Location 98
	{"Busstop", 1974.25, 631.00, 10.7}, // Location 99
	{"Busstop", 2020.50, 670.50, 10.7}, // Location 100
	{"Busstop", 2114.75, 675.75, 10.7}, // Location 101
	{"Busstop", 1084.75, 1196.00, 10.7}, // Location 102

	{"Busstop 1 Bayside", -2565.75, 2315.25,    4.83}, // Location 103
	{"Busstop 2 Bayside", -2337.00, 2375.00,    5.59}, // Location 104
	{"Busstop 1 El Quebrados", -1467.00, 2674.00,   55.68}, // Location 105
	{"Busstop 2 El Quebrados", -1528.50, 2548.00,   55.68}, // Location 106
	{"Busstop 1 Las Barrancas", -805.64, 1562.30,   26.96}, // Location 107
	{"Busstop 2 Las Barrancas", -847.00, 1529.50,   21.66}, // Location 108
	{"Busstop Las Payasdas", -269.00, 2625.75,   62.64}, // Location 109
	{"Busstop 1 Fort Carson",  -45.37, 1200.68,   19.21}, // Location 110
	{"Busstop 2 Fort Carson", -100.50, 1195.75,   19.58}, // Location 111
	{"Busstop 3 Fort Carson", -249.50, 1016.00,   19.59}, // Location 112
	{"Busstop 1 Angel Pine", -2169.75, -2305.75,   30.46}, // Location 113
	{"Busstop 2 Angel Pine", -2098.25, -2499.50,   30.46}, // Location 114
	{"Busstop 1 Palomino Creek", 2263.75,  -30.75,   26.33}, // Location 115
	{"Busstop 2 Palomino Creek", 2461.75,    1.00,   26.33}, // Location 116
	{"Busstop 1 Montgomery", 1288.50,  329.00,   19.40}, // Location 117
	{"Busstop 2 Montgomery", 1286.75,  323.75,   19.40}, // Location 118
	{"Busstop 1 Dillimore",  736.00, -527.50,   16.18}, // Location 119
	{"Busstop 2 Dillimore",  736.00, -532.50,   16.18}, // Location 120

	{"SF Airport Gate 1", -1337.94, -222.79,   14.14}, // ID = 121
	{"SF Airport Gate 2", -1395.26, -104.20,   14.14}, // ID = 122
	{"SF Airport Cargo Bay 1", -1337.16, -542.29,   14.14}, // ID = 123
	{"SF Airport Cargo Bay 2", -1412.96, -579.85,   14.14}, // ID = 124

	{"LS Airport Gate 1", 1747.04, -2441.04,   13.55}, // ID = 125
	{"LS Airport Gate 2", 1902.16, -2350.11,   13.54}, // ID = 126
	{"LS Airport Cargo Bay 1", 1889.10, -2625.49,   13.54}, // ID = 127
	{"LS Airport Cargo Bay 2", 1754.74, -2622.69,   13.54}, // ID = 128
	{"LS Airport Cargo Bay 3", 1617.61, -2622.30,   13.54}, // ID = 129

	{"LV Airport Gate 1", 1564.77, 1536.77,   10.82}, // ID = 130
	{"LV Airport Gate 2", 1569.85, 1358.58,   10.86}, // ID = 131
	{"LV Airport Cargo Bay 1", 1307.41, 1324.01,   10.82}, // ID = 132
	{"LV Airport Cargo Bay 2", 1308.47, 1361.32,   10.82}, // ID = 133

	{"Verdant Meadows Gate", 392.13, 2531.27, 16.54}, // ID = 134
	{"Verdant Meadows Cargo Bay", 291.08, 2525.75, 16.79}, // ID = 135

	{"TestRock", 351.5, 1008.75, 28.4}, // ID = 136
	{"TestCP", 344.75, 1020.5, 28.3} // ID = 137
};

