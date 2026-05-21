**Survivor Missions Anti-Hoarding Patch README**

============================================



**Purpose**

\-------

These changes were made to prevent specific mission reward/storage items from being picked up, hoarded, or left behind long-term by players.







**Main Goals**

\----------

\- Prevent players from taking certain mission containers

\- Allow players to still loot containers normally

\- Automatically despawn reward containers after a reasonable amount of time

\- Reduce long-term server clutter and persistence/performance issues





**Code Lines**

\----------

**MissionObject.SetTakeable(false);**

\--------------------------------

The following line was added to specific mission reward/storage containers to prevent players from picking them up or moving them while still allowing players to loot the items inside normally.



"MissionObject.SetTakeable(false);"



This was primarily used on:

\- MissionChest

\- MissionCrate

\- MissionBagBig

\- MissionBagSmall

\- Mission tents

\- Other large mission reward/storage containers





**Cleanup Timer Used**

\------------------

"Object rewardObject = MissionObject;"

"g\_Game.GetCallQueue(CALL\_CATEGORY\_GAMEPLAY).CallLater(g\_Game.ObjectDelete, 300000, false, rewardObject);"



300000 = 5 minute despawn timer \*(timer can be adjusted to any amount of time. Time is in milliseconds)







========================================================

VALIDATED / WORKING MISSIONS

========================================================



\- Apartment

\- BearHunt

\- Camp

\- CaptureFlag

\- CityMall

\- FreePigs

\- Graveyard

\- HeadHunt

\- Horde

\- PlaneCrash

\- Shrooms

\- Transport

\- UrbanMall







========================================================

PATCHED BUT NOT TESTED / NOT ACTIVE

========================================================



**The following mission files were patched using the same methods/patterns as the fully validated missions above, however they could not be fully tested in-game for the reasons listed below.**



\- Assassin

&#x20; Patched successfully, however this mission requires another active player/live target behavior to fully validate mission progression and completion behavior in a solo/local test environment.



\- CityStore

&#x20; Patched successfully, however this mission never appeared active in the current MissionList.txt configuration during testing. This may be because the mission is intended for another map/layout configuration such as Enoch/Livonia or is simply disabled in the current server setup.



\- EscortDefend

&#x20; Patched successfully, however this mission never appeared active in the current MissionList.txt configuration during testing. Could not be force-tested locally.



\- Ganja

&#x20; Patched successfully, however this mission never appeared active in the current MissionList.txt configuration during testing. Mission notes indicate it functions similarly to Transport missions but with different locations/objectives.



\- Psilos

&#x20; Patched successfully, however this mission never appeared active in the current MissionList.txt configuration during testing. Mission notes indicate it is essentially a variant/copy of Shrooms mission intended for maps such as Enoch/Livonia using different building layouts.







========================================================

IMPORTANT NOTES

========================================================



These files are edited versions of the original Survivor Missions mod files from the Steam > Dayz > Workshop > https://steamcommunity.com/sharedfiles/filedetails/?id=1988925918\&searchtext=survivor+mission .



The primary changes made are:

\- Certain reward/storage containers can no longer be picked up by players

\- Reward containers now despawn after 5 minutes once mission rewards are earned/completed



These changes were intentionally kept minimal so server owners can continue using:

\- custom mission locations

\- custom loot tables

\- custom loadouts

\- existing server balancing/configuration



Only the reward container interaction/cleanup behavior was modified.



Server owners should compare these edited files against their own current mission files before replacing anything.



Always verify files received from other people before using them on a live server.



Recommended comparison tools:

\- WinMerge

\- Notepad++ Compare Plugin

\- Visual Studio Code Compare

\- AI/code comparison tools of your choice



These tools can help quickly identify and transfer only the necessary changes into your own existing mission files.

