**Unfortunately work on SA:MP has officially dropped in late September of 2020. There won't be any updates for anymore. Also Wiki and Forum have been closed. As I was also rarely working on this gamemode, I'm archiving this repository for now. If you want to take it over and work on it, please let me know: b495472b(@)anon(.)leemail(.)me**

**Special thanks to all [contributors](https://github.com/PPC-Trucking/V1/graphs/contributors) and those who helped with [issue tickets](https://github.com/PPC-Trucking/V1/issues?q=-author%3Acodealdente)!**

---

# PPC Trucking V1

This is a modified version of the popular "Trucking Gamemode" by [PowerPC603](http://forum.sa-mp.com/member.php?u=109984) for Grand Theft Auto [San Andreas Multiplayer](http://www.sa-mp.com) (SA:MP). You can find the original post on SA:MP forums here: http://forum.sa-mp.com/showthread.php?t=196493.

---

## TEST THIS GAMEMODE!
Feel free to join our test server at 92.42.45.80:7790 to see this gamemode in action. Please note, that we WILL reset the server from time to time. **This gameserver will be online until 29th June, 2022.**

---

## Included
*	Complete gamemode as editable .pwn file
*	A ready-to-go server.cfg file (You only have to change the rcon_password!)
*	All required files for PAWNO so you can modify the gamemode as you like and re-compile it

## Installation
*	First go to the SA:MP website and [download the newest version](http://sa-mp.com/download.php) (both: server and client)
*	Follow [these instructions](http://forum.sa-mp.com/showthread.php?t=106958) to set up your server
*	Then [get the latest stable release](https://github.com/PPC-Trucking/V1/releases/latest) of this repository (choose .zip or .tar.gz)
*	Unpack this archive into the directory where you have set up your server. Follow the structure of the folders. For example: Place files from the "gamemodes" folder **only** into the "gamemodes" folder. **Do not mix it up!**
*	([__*__](#licensing)) Download the following plugins and place them into the "plugins" folder. If the folder does not exist in the root directory (where you can find your "gamemodes" folder), create it. **Only there!**
	*	[sscanf](https://github.com/maddinat0r/sscanf/releases)
	*   [streamer](https://github.com/samp-incognito/samp-streamer-plugin/releases)
*   ([__*__](#licensing)) Get these files as well and put them into the _~/pawno/include_ folder.
	*   [sscanf2.inc](https://github.com/maddinat0r/sscanf/releases)
	*   [streamer.inc](https://github.com/samp-incognito/samp-streamer-plugin/releases)
	*   [zcmd.inc](https://github.com/Southclaws/zcmd/blob/master/zcmd.inc)
	*   [dutils.inc](http://dracoblue.net/downloads/dutils/)
*	Now open _~/pawno/pawno.exe_ and load the gamemode to re-compile it
	* If everything worked, you should get a line saying ``Pawn compiler 3.2.3664 		Copyright (c) 1997-2006, ITB CompuPhase``
		* In this case, you can continue with the next point
		* If it did not work for you, please use Google first to solve the problem by yourself
			* If you could solve the problem, continue with the next point
			* If you could **not find** a solution for your problem, post the output in the [forum post](http://forum.sa-mp.com/showthread.php?t=196493). Please do not send a private message to someone to ask for his or her help. If you post the problem with your output in forums **everyone** can benefit from the answers.
*	If everything worked for you, the last thing you have to do is this:
	*	As a Windows user, you only have to open the server.cfg file and to set your desired rcon_password
	*	If you are running your server on a Linux machine, open the server.cfg file and change ``plugins sscanf streamer`` to ``plugins sscanf.so streamer.so`` and make sure that you have placed these plugins into the folder. You also have to set your desired rcon_password.

## Credits
*	[PowerPC603](http://forum.sa-mp.com/member.php?u=109984) - awesome work (gamemode)
*	[Y_Less](http://forum.sa-mp.com/member.php?u=29176) - very handy to handle parameters and more (sscanf)
*	[maddinat0r](https://github.com/maddinat0r) - mirror for sscanf
*	[Incognito](http://forum.sa-mp.com/member.php?u=925) - abbility to use more objects than usually possible (streamer)
*	[samp-incognito](https://github.com/samp-incognito) - mirror for streamer
*	Zeex - very efficient to create commands in seconds (zcmd)
*	[Southclaws](https://github.com/Southclaws) - mirror for zcmd
*	[DracoBlue](http://forum.sa-mp.com/member.php?u=389) - makes it easier to save information in files and to load them (dutils)

### Thanks for reading!
*	By the way, if you already have experience with [Notepad++](http://notepad-plus-plus.org), take a look at Slice's tutorial [how to use Notepad++ with PAWN](http://forum.sa-mp.com/showthread.php?t=174046). Notepad++ makes it more comfortable to maintain your SA:MP gamemodes and filterscripts than the interal default editor.

### Licensing
__*__ Due to copyright terms it is not allowed to include those files. You have to download and put them into the right folder by yourself.

If you have any questions or problems with this version of this gamemode, feel free to create a [new issue](https://github.com/PPC-Trucking/V1/issues) and to ask your question here. If you have general problems with this gamemode you can [post](http://forum.sa-mp.com/showthread.php?t=196493) in the SA:MP forum.