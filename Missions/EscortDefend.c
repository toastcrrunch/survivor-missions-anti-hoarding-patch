class EscortDefendMission extends SideMissions		//by Xellose and Funkdoc
{
#ifdef ENFUSION_AI_PROJECT
#ifdef EXPANSIONMODAI

	//IMPORTANT !!!
	//This mission requires the "Expansion AI" MOD to be installed.
	//The AI setting for vaulting has to be enabled in JSON file in profile folder: .\ExpansionMod\Settings\AISettings.json -> "Vaulting": 1,
	
	//Mission status flags
    bool secondwavespawned = false;
	bool firstwavespawned = false;
	bool Secondtrigger = false;
	bool Finaltrigger = false;
	bool FinalDest = false;
	bool turnOffCheck = false;
	bool LeaderWasLost = false;
		
	//Mission related entities
	ItemBase MissionObject;
	Object MissionBuilding;
	Object MissionLight;
	Grenade_Base TargetSmoke;
	PlayerBase TempLeader;
	UndergroundStash stash;
	eAIGroup MissionGroup;
	eAIBase EscortBot;
	EffecterBase TargetSmokeEffect;
	ref HumanMovementState LeaderStance = new HumanMovementState();
	PlayerBase LastKnownPlayerLeader = null;
	
	//Mission parameters
	int ExtendedTimout = 3600;				//seconds, mission duration time for extended mission	
	int MsgDlyFinish = 30;					//seconds, message delay time after player has finished mission
	int HordeDensity = 10;					//infected, spawned in circle area
	int Killrequiredwave1 = 2;				// # of Kills to push to Wave 2
	int Killrequiredwave2 = 5;				// # of Kills to Push to Final
	int Killrequiredfinal = 10;				// # of Kills to Trigger FinalChase
	int RejoinRaidus = 3;					//meters, distance for player to rejoin EscortBot if bot is separated from player and not engaged in combat, also used for mission failure if player flees too far from bot
	
	//mission vars
	string SurvivorName, SecondaryLoc;
	int EscortWalkChkTime;
	int StuckChkTime;
    vector currentPosition;
    vector lastPosition;
	vector FinalDestination;
	vector TempLeaderPos;
	vector RandomNearbyDest;
	vector ChestPos;
	vector ChestOri;
	vector TargetPos;
	int SmokeEffecterID;
	int crntECTime;
	int LastLeaderStanceID;
	float XX, YY;
	bool MapGridYnorthAscending;
	//AI related status flags
	bool EscortBotFollowsLeader = false;
	bool EscortLeftHouse = false;
	bool BotFound = false;

	// The model vector of the Police Station to Enable Defense Part of Mission.
	vector EscortFinalPos = "1.29 -2.42 -2.32";

	//Mission containers
	ref array<vector> ExtendedPosList = new array<vector>;
	ref array<vector> Spawnpoints = new array<vector>;
	ref array<string> InfectedTypes = new array<string>;
    ref array<vector> InfectedSpawns = new array<vector>;
    ref array<Object> DeadInfected = new array<Object>;
	ref TVectorArray IdleWaypoints = new TVectorArray;
	ref array<PlayerBase> AssistingPlayers = new array<PlayerBase>;
	
	override bool IsExtended() { return true; };
	
	void EscortDefendMission()
	{
		//exclude mission from saving mission position to file
		m_DynamicObjective = true;
		
		//Select primary mission
		m_MissionExtended = true;
						
		//Mission mission timeout
		m_MissionTimeout = 2700;			//seconds, max. duration for primary mission

		//Mission zones
		m_MissionZoneOuterRadius = 70.0;	//meters (!Do not set lower than 200m), mission activation distance
		m_MissionZoneInnerRadius = 3.0;		//meters (!Do not set outside of 1-5m), mission finishing distance to target object
				
		//Mission informant
		m_MissionInformant = "Dr. Legasov";
	
		//Mission person names
		SurvivorName = "Lt. Lehm";
		
		//Mission group init
		EscortBotFollowsLeader = false;
		
		//set map grid Y coords dir (Y is ascending in north dir?)
		TStringArray YNorthAscMaps = {"enoch"};
		
		if ( YNorthAscMaps.Find( g_Game.GetWorldName() ) > -1 )
		MapGridYnorthAscending = true;
		else MapGridYnorthAscending = false;
				
		//Translate mission location name and direction
		if ( MissionSettings.Opt_TranslateEWData )
		{
			m_MissionLocation = EventsWorldData.TransLoc( m_MissionLocation );
			m_MissionLocationDir = EventsWorldData.TransDir( m_MissionLocationDir );
		}
		
		//Set mission messages
		m_MissionMessage1 = SurvivorName + ", once a high-ranking officer in the Chernarus Police Department, has managed to survive for quite some time since the accident.";
		m_MissionMessage2 = "He knows about important documents of the investigations from hospital of those first infected people. However, he's looking for an escort.";
		m_MissionMessage3 = "If you're up to the task, he's holding out in a house\n** " + m_MissionLocationDir + " of " + m_MissionLocation + " **";
		//Add GPS coordinates of mission in message
		if ( MissionSettings.Opt_ShowGpsInfo ) m_MissionMessage3 += "\n\nGPS coordinates:"+ m_MissionInstance.GetGPS();

		//Infected types for secondary mission position
		//Male												//Female
		InfectedTypes.Insert("ZmbM_CitizenASkinny_Brown");	InfectedTypes.Insert("ZmbF_JournalistNormal_White");
		InfectedTypes.Insert("ZmbM_priestPopSkinny");		InfectedTypes.Insert("ZmbF_Clerk_Normal_White");
		InfectedTypes.Insert("ZmbM_HermitSkinny_Beige");	InfectedTypes.Insert("ZmbF_CitizenANormal_Blue");
		InfectedTypes.Insert("ZmbM_CitizenBFat_Red");		InfectedTypes.Insert("ZmbF_CitizenBSkinny");
		InfectedTypes.Insert("ZmbM_FishermanOld_Green");	InfectedTypes.Insert("ZmbF_HikerSkinny_Grey");
		InfectedTypes.Insert("ZmbM_HunterOld_Autumn");		InfectedTypes.Insert("ZmbF_SurvivorNormal_Orange");
		InfectedTypes.Insert("ZmbM_CitizenBFat_Blue");		InfectedTypes.Insert("ZmbF_HikerSkinny_Green");
		InfectedTypes.Insert("ZmbM_MotobikerFat_Black");	InfectedTypes.Insert("ZmbF_JoggerSkinny_Green");
		InfectedTypes.Insert("ZmbM_JoggerSkinny_Red");		InfectedTypes.Insert("ZmbF_SkaterYoung_Striped");
		InfectedTypes.Insert("ZmbM_MechanicSkinny_Grey");	InfectedTypes.Insert("ZmbF_BlueCollarFat_Red");
		InfectedTypes.Insert("ZmbM_HandymanNormal_Green");	InfectedTypes.Insert("ZmbF_MechanicNormal_Beige");
		InfectedTypes.Insert("ZmbM_HeavyIndustryWorker");	InfectedTypes.Insert("ZmbF_PatientOld");
		InfectedTypes.Insert("ZmbM_Jacket_black");			InfectedTypes.Insert("ZmbF_ShortSkirt_beige");
		InfectedTypes.Insert("ZmbM_Jacket_stripes");		InfectedTypes.Insert("ZmbF_VillagerOld_Red");
		InfectedTypes.Insert("ZmbM_HikerSkinny_Blue");		InfectedTypes.Insert("ZmbF_JoggerSkinny_Red");
		InfectedTypes.Insert("ZmbM_HikerSkinny_Yellow");	InfectedTypes.Insert("ZmbF_MilkMaidOld_Beige");
		InfectedTypes.Insert("ZmbM_PolicemanFat");			InfectedTypes.Insert("ZmbF_VillagerOld_Green");
		InfectedTypes.Insert("ZmbM_PatrolNormal_Summer");	InfectedTypes.Insert("ZmbF_ShortSkirt_yellow");
		InfectedTypes.Insert("ZmbM_JoggerSkinny_Blue");		InfectedTypes.Insert("ZmbF_NurseFat");
		InfectedTypes.Insert("ZmbM_VillagerOld_White");		InfectedTypes.Insert("ZmbF_PoliceWomanNormal");
		InfectedTypes.Insert("ZmbM_SkaterYoung_Brown");		InfectedTypes.Insert("ZmbF_HikerSkinny_Blue");
		InfectedTypes.Insert("ZmbM_MechanicSkinny_Green");	InfectedTypes.Insert("ZmbF_ParamedicNormal_Green");
		InfectedTypes.Insert("ZmbM_DoctorFat");				InfectedTypes.Insert("ZmbF_JournalistNormal_Red");
		InfectedTypes.Insert("ZmbM_PatientSkinny");			InfectedTypes.Insert("ZmbF_SurvivorNormal_White");
		InfectedTypes.Insert("ZmbM_ClerkFat_Brown");		InfectedTypes.Insert("ZmbF_JoggerSkinny_Brown");
		InfectedTypes.Insert("ZmbM_ClerkFat_White");		InfectedTypes.Insert("ZmbF_MechanicNormal_Grey");
		InfectedTypes.Insert("ZmbM_Jacket_magenta");		InfectedTypes.Insert("ZmbF_BlueCollarFat_Green");
		InfectedTypes.Insert("ZmbM_PolicemanSpecForce");	InfectedTypes.Insert("ZmbF_DoctorSkinny");
		
		//parse Extended Mission parameter to get secondary location
		array<string> SecondaryDescription = new array<string>;
		int DescriptionCount;
		
		SecondaryLoc = m_MissionDescription[3];
		SecondaryLoc.Split("_", SecondaryDescription );
		SecondaryLoc = "";
		DescriptionCount = SecondaryDescription.Count();
		
		if  ( DescriptionCount < 3 )
		{
			SecondaryLoc = SecondaryDescription[0];
		}
		else
		{
			for ( int i = 0; i < DescriptionCount; i++)
			{
				if ( i < DescriptionCount - 2 ) SecondaryLoc += SecondaryDescription.Get(i) + "_";
				else if ( i < DescriptionCount - 1 ) SecondaryLoc += SecondaryDescription.Get(i);
				else if ( i < DescriptionCount ) break;
			}
		}
	}

	void ~EscortDefendMission()
	{
		//Despawn all remaining mission objects
		if ( m_MissionObjects )
		{	
			Print("[SMM] Despawning "+ m_MissionObjects.Count() +" mission objects from old mission...");				
			for ( int i = 0; i < m_MissionObjects.Count(); i++ )
			{
				if ( !m_MissionObjects.Get(i) ) continue;
				else g_Game.ObjectDelete( m_MissionObjects.Get(i) );		
			}
			m_MissionObjects.Clear();
		}
		
		//Despawn mission AI's
		if ( m_MissionAIs )
		{
			if ( m_MissionAIs.Count() > 0 )
			{
				Print("[SMM] Despawning "+ m_MissionAIs.Count() +" mission AI's from old mission...");
				for ( int k = 0; k < m_MissionAIs.Count(); k++ )
				{
					
					g_Game.ObjectDelete( m_MissionAIs.Get(k) );
				}
				m_MissionAIs.Clear();			
			}
			else Print("[SMM] No mission AI's to despawn.");
		}
		
		//Delete PlayersInZone list
		if ( m_PlayersInZone )	m_PlayersInZone.Clear();
		
		//reset dynamic objective mission status
		m_DynamicObjective = false;
	}

	void SpawnRewards()
	{
		//spawn new MissionObject after deleting standard SeaChest
		MissionObject = ItemBase.Cast( g_Game.CreateObject("MissionChest", ChestPos ));
		MissionObject.SetOrientation( ChestOri );
		MissionObject.SetTakeable(false);

		//Get random loadout 
		int selectedLoadout = Math.RandomIntInclusive( 0, 9);	//!change randomization limit after adding new loadouts!	
		
		//Spawn selected loadout items in mission object
		EntityAI weapon;
				
		if ( selectedLoadout == 0 )
		{
			weapon = MissionObject.GetInventory().CreateInInventory("M4A1_Green");
				weapon.GetInventory().CreateAttachment("M4_RISHndgrd_Green");
				weapon.GetInventory().CreateAttachment("M4_MPBttstck");
				weapon.GetInventory().CreateAttachment("ACOGOptic");
				weapon.GetInventory().CreateAttachment("M4_Suppressor");
			MissionObject.GetInventory().CreateInInventory("Mag_STANAG_30Rnd");
			MissionObject.GetInventory().CreateInInventory("Mag_STANAG_30Rnd");
			MissionObject.GetInventory().CreateInInventory("M4_T3NRDSOptic");
			MissionObject.GetInventory().CreateInInventory("Ammo_556x45");
			MissionObject.GetInventory().CreateInInventory("Ammo_556x45");
			MissionObject.GetInventory().CreateInInventory("CanOpener");
			MissionObject.GetInventory().CreateInInventory("PeachesCan");
			MissionObject.GetInventory().CreateInInventory("Canteen");
			MissionObject.GetInventory().CreateInInventory("Battery9V");
		}
		if (selectedLoadout == 1)
		{
			weapon = MissionObject.GetInventory().CreateInInventory("SVD");
				weapon.GetInventory().CreateAttachment("PSO1Optic");
			MissionObject.GetInventory().CreateInInventory("Mag_SVD_10Rnd");
			MissionObject.GetInventory().CreateInInventory("Mag_SVD_10Rnd");
			MissionObject.GetInventory().CreateInInventory("PSO1Optic");
			MissionObject.GetInventory().CreateInInventory("KazuarOptic");
			MissionObject.GetInventory().CreateInInventory("Ammo_762x54");
			MissionObject.GetInventory().CreateInInventory("Ammo_762x54");
			MissionObject.GetInventory().CreateInInventory("Ammo_762x54");
			MissionObject.GetInventory().CreateInInventory("Ammo_762x54");
			MissionObject.GetInventory().CreateInInventory("Ammo_762x54");
			MissionObject.GetInventory().CreateInInventory("CanOpener");
			MissionObject.GetInventory().CreateInInventory("PeachesCan");
			MissionObject.GetInventory().CreateInInventory("Canteen");
			MissionObject.GetInventory().CreateInInventory("Battery9V");
		}
		if (selectedLoadout == 2)
		{
			weapon = MissionObject.GetInventory().CreateInInventory("AKM");
				weapon.GetInventory().CreateAttachment("AK_RailHndgrd_Green");
				weapon.GetInventory().CreateAttachment("AK_PlasticBttstck_Green");
				weapon.GetInventory().CreateAttachment("PSO1Optic");
				weapon.GetInventory().CreateAttachment("AK_Suppressor");
			MissionObject.GetInventory().CreateInInventory("Mag_AKM_30Rnd");
			MissionObject.GetInventory().CreateInInventory("Mag_AKM_30Rnd");
			MissionObject.GetInventory().CreateInInventory("Ammo_762x39");
			MissionObject.GetInventory().CreateInInventory("Ammo_762x39");
			MissionObject.GetInventory().CreateInInventory("Ammo_762x39");
			MissionObject.GetInventory().CreateInInventory("CanOpener");
			MissionObject.GetInventory().CreateInInventory("PeachesCan");
			MissionObject.GetInventory().CreateInInventory("Canteen");
			MissionObject.GetInventory().CreateInInventory("Battery9V");
		}
		if (selectedLoadout == 3)
		{
			weapon = MissionObject.GetInventory().CreateInInventory("FAL");
				weapon.GetInventory().CreateAttachment("Fal_OeBttstck");
			MissionObject.GetInventory().CreateInInventory("Mag_FAL_20Rnd");
			MissionObject.GetInventory().CreateInInventory("Mag_FAL_20Rnd");
			MissionObject.GetInventory().CreateInInventory("Mag_FAL_20Rnd");
			MissionObject.GetInventory().CreateInInventory("M4_Suppressor");
			MissionObject.GetInventory().CreateInInventory("ACOGOptic");
			MissionObject.GetInventory().CreateInInventory("FNX45");
			MissionObject.GetInventory().CreateInInventory("Mag_FNX45_15Rnd");
			MissionObject.GetInventory().CreateInInventory("Mag_FNX45_15Rnd");
			MissionObject.GetInventory().CreateInInventory("Ammo_45ACP");
			MissionObject.GetInventory().CreateInInventory("FNP45_MRDSOptic");
			MissionObject.GetInventory().CreateInInventory("PistolSuppressor");
			MissionObject.GetInventory().CreateInInventory("PsilocybeMushroom");
			MissionObject.GetInventory().CreateInInventory("AmmoBox");
			MissionObject.GetInventory().CreateInInventory("Battery9V");
		}	
		if (selectedLoadout == 4)
		{
			weapon = MissionObject.GetInventory().CreateInInventory("SKS");
				weapon.GetInventory().CreateAttachment("PUScopeOptic");
			MissionObject.GetInventory().CreateInInventory("Ammo_762x39");
			MissionObject.GetInventory().CreateInInventory("Ammo_762x39");
			weapon = MissionObject.GetInventory().CreateInInventory("FNX45");
				weapon.GetInventory().CreateAttachment("PistolSuppressor");
				EntityAI weaponlight = weapon.GetInventory().CreateAttachment("TLRLight");
					weaponlight.GetInventory().CreateAttachment("Battery9V");
			MissionObject.GetInventory().CreateInInventory("Mag_FNX45_15Rnd");
			MissionObject.GetInventory().CreateInInventory("Ammo_45ACP");
			MissionObject.GetInventory().CreateInInventory("AmmoBox");
		}	
		if (selectedLoadout == 5)
		{
			weapon = MissionObject.GetInventory().CreateInInventory("Winchester70");
				weapon.GetInventory().CreateAttachment("HuntingOptic");
			MissionObject.GetInventory().CreateInInventory("Ammo_308Win");
			MissionObject.GetInventory().CreateInInventory("Ammo_308Win");
			MissionObject.GetInventory().CreateInInventory("FNX45");
			MissionObject.GetInventory().CreateInInventory("Mag_FNX45_15Rnd");
			MissionObject.GetInventory().CreateInInventory("Ammo_45ACP");
			MissionObject.GetInventory().CreateInInventory("AmmoBox");
			MissionObject.GetInventory().CreateInInventory("PistolSuppressor");
			MissionObject.GetInventory().CreateInInventory("TLRLight");
			MissionObject.GetInventory().CreateInInventory("Battery9V");
		}
		if (selectedLoadout == 6)
		{			
			weapon = MissionObject.GetInventory().CreateInInventory("MP5K");
				weapon.GetInventory().CreateAttachment("MP5_RailHndgrd");
				weapon.GetInventory().CreateAttachment("MP5k_StockBttstck");
				weapon.GetInventory().CreateAttachment("M68Optic");
				weapon.GetInventory().CreateAttachment("PistolSuppressor");			
			MissionObject.GetInventory().CreateInInventory("Mag_MP5_30Rnd");
			MissionObject.GetInventory().CreateInInventory("Mag_MP5_30Rnd");
			MissionObject.GetInventory().CreateInInventory("AmmoBox_9x19_25rnd");
			MissionObject.GetInventory().CreateInInventory("GP5GasMask");
			MissionObject.GetInventory().CreateInInventory("NBCGlovesGray");
			MissionObject.GetInventory().CreateInInventory("WaterBottle");	
			MissionObject.GetInventory().CreateInInventory("SpaghettiCan");
			MissionObject.GetInventory().CreateInInventory("M18SmokeGrenade_Red");
			MissionObject.GetInventory().CreateInInventory("Battery9V");
			MissionObject.GetInventory().CreateInInventory("Battery9V");
			MissionObject.GetInventory().CreateInInventory("Battery9V");				
		}
		if (selectedLoadout == 7)
		{			
			weapon = MissionObject.GetInventory().CreateInInventory("AK74");
				weapon.GetInventory().CreateAttachment("AK_RailHndgrd");
				weapon.GetInventory().CreateAttachment("AK74_WoodBttstck");	
				weapon.GetInventory().CreateAttachment("KashtanOptic");
				weapon.GetInventory().CreateAttachment("AK_Suppressor");			
			MissionObject.GetInventory().CreateInInventory("Mag_AK74_30Rnd");
			MissionObject.GetInventory().CreateInInventory("Mag_AK74_30Rnd");
			MissionObject.GetInventory().CreateInInventory("Headtorch_Grey");
			MissionObject.GetInventory().CreateInInventory("NBCBootsGray");
			MissionObject.GetInventory().CreateInInventory("Canteen");	
			MissionObject.GetInventory().CreateInInventory("TacticalBaconCan");
			MissionObject.GetInventory().CreateInInventory("Tomato");
			MissionObject.GetInventory().CreateInInventory("Battery9V");
			MissionObject.GetInventory().CreateInInventory("Battery9V");				
		}
		if (selectedLoadout == 8)
		{			
			weapon = MissionObject.GetInventory().CreateInInventory("AKS74U");
				weapon.GetInventory().CreateAttachment("AKS74U_Bttstck");			
			MissionObject.GetInventory().CreateInInventory("Mag_AK74_30Rnd");
			MissionObject.GetInventory().CreateInInventory("Mag_AK74_30Rnd");
			MissionObject.GetInventory().CreateInInventory("M67Grenade");
			MissionObject.GetInventory().CreateInInventory("M67Grenade");
			MissionObject.GetInventory().CreateInInventory("Matchbox");
			MissionObject.GetInventory().CreateInInventory("Canteen");	
			MissionObject.GetInventory().CreateInInventory("PortableGasStove");
			MissionObject.GetInventory().CreateInInventory("SmallGasCanister");
			MissionObject.GetInventory().CreateInInventory("Battery9V");
			MissionObject.GetInventory().CreateInInventory("Battery9V");
			MissionObject.GetInventory().CreateInInventory("Battery9V");			
		}
		if (selectedLoadout == 9)
		{			
			weapon = MissionObject.GetInventory().CreateInInventory("Glock19");
				weapon.GetInventory().CreateAttachment("PistolSuppressor");			
			MissionObject.GetInventory().CreateInInventory("Mag_Glock_15Rnd");
			MissionObject.GetInventory().CreateInInventory("Mag_Glock_15Rnd");
			MissionObject.GetInventory().CreateInInventory("FishingRod");
			MissionObject.GetInventory().CreateInInventory("Carp");
			MissionObject.GetInventory().CreateInInventory("Hook");
			MissionObject.GetInventory().CreateInInventory("Worm");
			MissionObject.GetInventory().CreateInInventory("CombatKnife");
			MissionObject.GetInventory().CreateInInventory("FieldShovel");
			MissionObject.GetInventory().CreateInInventory("Canteen");	
			MissionObject.GetInventory().CreateInInventory("MackerelFilletMeat");
			MissionObject.GetInventory().CreateInInventory("Battery9V");
			MissionObject.GetInventory().CreateInInventory("Battery9V");
			MissionObject.GetInventory().CreateInInventory("Battery9V");			
		}
		
		m_MissionObjects.Insert( MissionObject );

		Object rewardChest = MissionObject;
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(g_Game.ObjectDelete, 300000, false, rewardChest);

		Print("[SMM] EscortDefend Mission : Mission rewards spawned in reward container. Randomly selected loadout was "+selectedLoadout+"." );			
		
		Print("[SMM] EscortDefend Mission : Rewards container is SeaChest @ "+ RandomNearbyDest.ToString() );
		
		//finish mission
		//g_Game.GetCallQueue( CALL_CATEGORY_SYSTEM ).Remove( EntityChecks );
		m_RewardsSpawned = true;
		FinalDest = true;
		m_MsgNum = -1;
		m_MsgChkTime = m_MissionTime + MsgDlyFinish;
	}
		
	void SpawnAIs()
	{	//Spawning the Escort Bot
		if ( m_MissionExtended )
		{
			//spawn EscortBot and set AI props
			EscortBot = eAIBase.Cast(g_Game.CreateObject( "eAI_SurvivorM_Peter", m_MissionPosition));
			EscortBot.SetPosition(m_MissionPosition);
			EscortBot.PlaceOnSurface();
			EscortBot.SetMovementSpeedLimits(0.0, 0.0);
						
			//EscortBot clothes
			EscortBot.GetInventory().CreateAttachment("PoliceCap");		
			EscortBot.GetInventory().CreateAttachment("PoliceJacket");
			EscortBot.GetInventory().CreateAttachment("PoliceVest");
			EscortBot.GetInventory().CreateAttachment("PolicePants");
			EscortBot.GetInventory().CreateAttachment("DressShoes_Black");
			
			//EscortBot loadout
			EntityAI weapon;
			weapon = EscortBot.GetInventory().CreateInInventory("Glock19");
			weapon.GetInventory().CreateAttachment("PistolSuppressor");
			EscortBot.GetInventory().CreateAttachment("PoliceBaton");
			EscortBot.GetInventory().CreateInInventory("Mag_Glock_15Rnd");
			EscortBot.GetInventory().CreateInInventory("Mag_Glock_15Rnd");
			EscortBot.GetInventory().CreateInInventory("Mag_Glock_15Rnd");
			EscortBot.GetInventory().CreateInInventory("Mag_Glock_15Rnd");
			EscortBot.GetInventory().CreateInInventory("Mag_Glock_15Rnd");
						 
			//set AI behavior and props
			EscortBot.eAI_SetThreatDistanceLimit(25);
			EscortBot.eAI_SetLootingBehavior(0);
			EscortBot.eAI_SetUnlimitedReload(true);

			//Get MissionGroup and ensure EscortBot is a member of the group
			MissionGroup = EscortBot.GetGroup();
			if (!MissionGroup.IsMember(EscortBot))
				MissionGroup.AddMember(EscortBot);
						
			//Set faction for house defend
			eAIFaction Passive = new eAIFactionGuards();
			MissionGroup.SetFaction(Passive);
			
			//Add escort bot to MissionAI's
			m_MissionAIs.Insert(EscortBot);
			
			//Set Bot as Leader of Group
			MissionGroup.SetLeader(EscortBot);
			//Set Bot as Escort
			EscortBotFollowsLeader = false;
			EscortLeftHouse = false;

			if ( MissionSettings.DebugMode )
			{
				string aigroupfaction = EscortBot.GetGroup().GetFaction().GetName();
				
				Print("[SMM] EscortDefend Mission :  AI faction is: "+ aigroupfaction);
				Print("[SMM] EscortDefend Mission :  Members in group: " + MissionGroup.Count());
			}
		
		}
		else
		{
			if ( EscortBotFollowsLeader )
			{
				//spanwn no AIs at secondary
			}
		}
	}

	void DelayedAISpawn()
	{
		//calc new spawn position
		float x = Math.RandomFloatInclusive( 10 , 20 );
		float y = Math.RandomFloatInclusive( 10 , 20 );	
		int Dice = Math.RandomIntInclusive( 0, 9);
		if ( Dice > 4 ) x *= -1.0;
		Dice = Math.RandomIntInclusive( 0, 9);
		if ( Dice < 5 ) y *= -1.0;
		vector NewPosVector = { x, 0, y };
		
		//spawn infected
		Object horde = g_Game.CreateObjectEx( InfectedTypes.GetRandomElement(), m_MissionPosition + NewPosVector, ECE_PLACE_ON_SURFACE|ECE_INITAI );
		m_MissionAIs.Insert( horde );
	}	
	
	void SpawnObjects()
	{
		int year, month, day, hour, minute, SunRiseHour, SunSetHour;
		Mission currentMission;
		WorldData worldData;
		float ApproxSunrise;
		float ApproxSunset;
		vector lightPos;
		EntityAI canister;
		ItemBase lamp;
		
		if ( m_MissionExtended )
		{
			//check darkness for location light support and spawn lamp if needed			
			g_Game.GetWorld().GetDate( year, month, day, hour, minute );		
			
			currentMission = g_Game.GetMission();

			if ( currentMission )
			{			
				worldData = currentMission.GetWorldData();
				if ( worldData )
				{
					ApproxSunrise = worldData.GetApproxSunriseTime( month + ( day / 32.0 ) );
					ApproxSunset = worldData.GetApproxSunsetTime( month + ( day / 32.0 ) );
					
					SunRiseHour = Math.Round( ApproxSunrise + 0.25 );
					SunSetHour = Math.Round( ApproxSunset - 0.25 );
					
					if ( MissionSettings.DebugMode )
					Print("[SMM] EscortDefend Mission : Test Hour is "+ hour +", SunRiseHour: "+ SunRiseHour +", SonSetHour: "+ SunSetHour );
					
					if ( hour >= SunSetHour || hour <= SunRiseHour ) 
					{

						//Choose the Light position Here. 
						lightPos = m_MissionPosition + "1 0 1";
						MissionLight = g_Game.CreateObjectEx("PortableGasLamp", lightPos, ECE_PLACE_ON_SURFACE);

						lamp = ItemBase.Cast(MissionLight);
						if ( !lamp )
						{
							Print("[SMM] EscortDefend Mission : ERROR :: Could not spawn mission light at " + lightPos);
							return;
						}

						canister = lamp.GetInventory().CreateAttachment("SmallGasCanister");
						if ( !canister )
						{
							Print("[SMM] EscortDefend Mission : ERROR :: Could not attach SmallGasCanister to mission light.");
						}

						if ( lamp.HasEnergyManager() && lamp.GetCompEM().CanSwitchOn() )
						{
							lamp.GetCompEM().SwitchOn();
						}
						else
						{
							Print("[SMM] EscortDefend Mission : ERROR :: Mission light can not be switched on.");
						}
						m_MissionObjects.Insert( lamp );
					}
				}
			}
		}
		else
		{
			if ( EscortBotFollowsLeader )
			{
				//check darkness for location light support and spawn lamp if needed			
				g_Game.GetWorld().GetDate( year, month, day, hour, minute );		
				
				currentMission = g_Game.GetMission();
	
				if ( currentMission )
				{			
					worldData = currentMission.GetWorldData();
					if ( worldData )
					{
						ApproxSunrise = worldData.GetApproxSunriseTime( month + ( day / 32.0 ) );
						ApproxSunset = worldData.GetApproxSunsetTime( month + ( day / 32.0 ) );
						
						SunRiseHour = Math.Round( ApproxSunrise + 0.25 );
						SunSetHour = Math.Round( ApproxSunset - 0.25 );
						
						if ( MissionSettings.DebugMode )
						Print("[SMM] EscortDefend Mission : Test Hour is "+ hour +", SunRiseHour: "+ SunRiseHour +", SunSetHour: "+ SunSetHour );
						
						if ( hour >= SunSetHour || hour <= SunRiseHour ) 
						{
	
							//Choose the Light position Here. 
							lightPos = m_MissionPosition + "1 0 1";
							MissionLight = g_Game.CreateObjectEx("PortableGasLamp", lightPos, ECE_PLACE_ON_SURFACE);
	
							lamp = ItemBase.Cast(MissionLight);
							if ( !lamp )
							{
								Print("[SMM] EscortDefend Mission : ERROR :: Could not spawn mission light at " + lightPos);
								return;
							}
	
							canister = lamp.GetInventory().CreateAttachment("SmallGasCanister");
							if ( !canister )
							{
								Print("[SMM] EscortDefend Mission : ERROR :: Could not attach SmallGasCanister to mission light.");
							}
	
							if ( lamp.HasEnergyManager() && lamp.GetCompEM().CanSwitchOn() )
							{
								lamp.GetCompEM().SwitchOn();
							}
							else
							{
								Print("[SMM] EscortDefend Mission : ERROR :: Mission light can not be switched on.");
							}
							m_MissionObjects.Insert( lamp );
						}
					}
				}
			}
		}
	}

	override void ObjDespawn()
	{	//immediately despawns mission objects at mission timeout or after finish message, except those which remains until next mission
		//nothing to despawn
	}
	
	override void ExtendMission()
	{
		//init Messenger for new messages
		m_MsgNum = 1;
		m_MsgChkTime = m_MissionTime + 25;
		
		//increase mission MissionTimeout
		m_MissionTimeout = m_MissionTime + ExtendedTimout;  
		
		//deployment settings & init for extended mission
		m_MissionZoneOuterRadius = 50.0;
		m_MissionZoneInnerRadius = 0.5;
		
		//Get building position from building type and location for secondary mission		
		if ( EventsWorldData.GetBuildingsAtLoc("Land_Village_PoliceStation", SecondaryLoc, ExtendedPosList ))
		{
			bool SecBuildFound = false;
			
			//Get MissionPosition for MissionBuilding from secondary mission
			m_MissionPosition = ExtendedPosList.GetRandomElement();
									
			g_Game.GetObjectsAtPosition( m_MissionPosition, 1.0 , m_ObjectList , m_ObjectCargoList );
			for ( int m=0; m < m_ObjectList.Count(); m++ )
			{ 
				Object FoundObject = m_ObjectList.Get(m);
				if ( FoundObject.GetType() == "Land_Village_PoliceStation" || FoundObject.GetType() == "Land_Village_PoliceStation_Enoch")
				{	
					MissionBuilding = FoundObject;
					SecBuildFound = true;
					
					Print("[SMM] EscortDefend Mission : Secondary mission location is "+ m_MissionSecondaryLoc );
													
					break;
				}	
			}
			if ( !SecBuildFound )
			Print("[SMM] EscortDefend Mission : ERROR :: Can't find object Land_Village_PoliceStation in "+ m_MissionSecondaryLoc +" on the map!");
		}
		else Print("[SMM] EscortDefend Mission : ERROR :: MissionBuilding PoliceStation is not defined in EventsWorldData GetBuildingsAtLoc() for "+ SecondaryLoc +" !");
	}
	
	void MarkTargetSmoke()
	{
		//check darkness for documents case marker
		int year, month, day, hour, minute;
		g_Game.GetWorld().GetDate( year, month, day, hour, minute );		
		
		Mission currentMission = g_Game.GetMission();
		if ( currentMission )
		{			
			WorldData worldData = currentMission.GetWorldData();
			if ( worldData )
			{
				float ApproxSunrise = worldData.GetApproxSunriseTime( month + ( day / 32.0 ) );
				float ApproxSunset = worldData.GetApproxSunsetTime( month + ( day / 32.0 ) );
				
				int SunRiseHour = Math.Round( ApproxSunrise + 0.25 );
				int SunSetHour = Math.Round( ApproxSunset - 0.25 );
								
				if ( hour >= SunSetHour || hour <= SunRiseHour ) 
				{
					if ( !MissionLight )
					{
						MissionLight = g_Game.CreateObjectEx("AnniversarySpotLight", TargetPos + "0 0.15 0", ECE_KEEPHEIGHT );					
						AnniversarySpotLight.Cast( MissionLight ).Activate();
					}
				}
				else
				{
					ParticleEffecterParameters param = new ParticleEffecterParameters( "ParticleEffecter", EffecterBase.NOT_DEFINED_LIFESPAN, ParticleList.GRENADE_M18_YELLOW_LOOP );
					SmokeEffecterID = SEffectManager.CreateParticleServer( TargetPos, param );
				}
			}
		}
	}

	void DeleteMarker()
	{
		g_Game.ObjectDelete( MissionLight );
		g_Game.ObjectDelete( TargetSmokeEffect );
		MissionLight = null;
		TargetSmokeEffect = null;
		SEffectManager.DestroyEffecterParticleServer( SmokeEffecterID );
	}
	
	// Here is where we spawn our first wave - Reference the DelayedAI Spawn from Above.
	void Firstwave()
	{
		//Spawn horde
		int SpawnDlyTime = 500;
		
		for ( int j = 0; j < HordeDensity; j++ )
		{
			//Spawn infected time delayed 500ms
			g_Game.GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( this.DelayedAISpawn, SpawnDlyTime, false );
			SpawnDlyTime += 500;	
		}
	}

	// Once the First Wave as been Cleared We will start this.
    void SecondWave()
	{
		int Dice1 = Math.RandomIntInclusive( 0, 9);  
		int Dice2 = Math.RandomIntInclusive( 0, 9);
	    for ( int j2 = 0; j2 < HordeDensity; j2++ )
		{
			//calc new spawn position
			float x = Math.RandomFloatInclusive( 20 , 65 );
			float y = Math.RandomFloatInclusive( 20 , 65 );	

			if ( Dice1 > 4 ) x *= -1.0;  
			if ( Dice2 < 5 ) y *= -1.0;
	
			vector NewPosVector = { x, 0, y };
			
			//Spawn infected
			m_MissionAIs.Insert( g_Game.CreateObjectEx( InfectedTypes.GetRandomElement(), m_MissionPosition + NewPosVector, ECE_PLACE_ON_SURFACE|ECE_INITAI ));	
		}
	}
		
	override void PlayerChecks( PlayerBase player )
	{	//Executed continuously on target player while he is inside mission zone. Avoid complex calculations or big data processing!
		if ( m_MissionExtended )
		{
			if ( player && !EscortBotFollowsLeader && !m_RewardsSpawned && !firstwavespawned && player.IsAlive() )
			{
				// If player is still a member, force remove them (not leader)
				if (MissionGroup.IsMember(player) && MissionGroup.GetLeader() != player) {
					MissionGroup.RemoveMember(player);
				}
				if ( !MissionGroup.IsMember( player ) ) 
				{        
					if ( vector.Distance( m_MissionPosition , player.GetPosition()) < m_MissionZoneInnerRadius )
					{
						if ( EscortLeftHouse )
						{
							//Reset secondary mission zone
							m_MissionPosition = MissionBuilding.ModelToWorld( Spawnpoints.Get(0) );
							m_MissionZoneOuterRadius = 120.0;
							m_MissionZoneInnerRadius = 2.0;
						}
						
						Print("[SMM] EscortDefend Mission : Player with SteamID64: "+ player.GetIdentity().GetPlainId() +" was set to group leader. The escort bot follows him now.");

						EscortBotFollowsLeader = true;
						
						//Add player to MissionGroup
						MissionGroup.SetLeader( player );
						player.SetGroup( MissionGroup );
						
						// Track the last known player leader
						LastKnownPlayerLeader = player;

						//Set formation to file for following mode
						MissionGroup.SetFormation( new eAIFormationFile );
                        
						if ( MissionSettings.DebugMode )
						{
							if ( MissionGroup.IsMember( player ) )
							Print("[SMM] EscortDefend Mission : Player is member.");
							else Print("[SMM] EscortDefend Mission : Player is not a member.");
                            
							if ( MissionGroup.IsMember( EscortBot ) )
							Print("[SMM] EscortDefend Mission : MissionAI is member.");
							else Print("[SMM] EscortDefend Mission : MissionAI is not a member.");
                        
							Print("[SMM] EscortDefend Mission : Player "+ MissionGroup.GetLeader().GetIdentity().GetPlainId() +" is leader. MemberCount = "+ MissionGroup.Count() );
							Print("[SMM] EscortDefend Mission : Old formation was "+ MissionGroup.GetFormation().ClassName() );
						}
					}

				}
			}
		}
		if ( player && !EscortBotFollowsLeader && !m_RewardsSpawned && !firstwavespawned && player.IsAlive() && EscortLeftHouse && vector.Distance( m_MissionPosition , player.GetPosition()) < RejoinRaidus )
		{                        
			if ( EscortLeftHouse )
			{
				//Reset secondary mission zone
				m_MissionPosition = MissionBuilding.ModelToWorld( Spawnpoints.Get(0) );
				m_MissionZoneOuterRadius = 120.0;
				m_MissionZoneInnerRadius = 2.0;
			}

			Print("[SMM] EscortDefend Mission : Player with SteamID64: "+ player.GetIdentity().GetPlainId() +" was set to group leader. The escort bot follows him now.");
			
			EscortBotFollowsLeader = true;
			
			//Add player to MissionGroup
			MissionGroup.SetLeader( player );
			player.SetGroup( MissionGroup );
			
			// Track the last known player leader
			LastKnownPlayerLeader = player;

			//Set formation to file for following mode
			MissionGroup.SetFormation( new eAIFormationFile );
			DeleteMarker();
		}
	}
	
	void FinalChase()
	{

		//Bot talks about last destination with player
		string icon = "set:dayz_gui image:notification_friend";		
		string msgs[4] = { "Ok thank you!\nI think that gave us enough space to get out of here.\n\nFollow me and stay close.",
		"I am sorry, this mission was a failure in the terms of finding the documents we were looking for.",
		"I want you to have at least something for assisting me in checking out this Police Station.",
		"I have a stash of supplies nearby I will take you to. Please keep up and keep me safe!" };
			
		int timedelay = 0;
		
		for ( int i=0; i < 4; i++)
		{
			g_Game.GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( NotificationSystem.SendNotificationToPlayerIdentityExtended, timedelay, false, MissionGroup.GetMember(1).GetIdentity(), 10.0, SurvivorName + ":", msgs[i], icon );
			timedelay += 25000; 
		}
		
		//add headtorch to Escort Bot if its night time
		int year, month, day, hour, minute, SunRiseHour, SunSetHour;
		Mission currentMission;
		WorldData worldData;
		float ApproxSunrise;
		float ApproxSunset;
		vector lightPos;
		
        //check darkness for location light support and spawn lamp if needed            
        g_Game.GetWorld().GetDate( year, month, day, hour, minute );        
        
        currentMission = g_Game.GetMission();

        if ( currentMission )
        {            
            worldData = currentMission.GetWorldData();
            if ( worldData )
            {
                ApproxSunrise = worldData.GetApproxSunriseTime( month + ( day / 32.0 ) );
                ApproxSunset = worldData.GetApproxSunsetTime( month + ( day / 32.0 ) );
                
                SunRiseHour = Math.Round( ApproxSunrise + 0.25 );
                SunSetHour = Math.Round( ApproxSunset - 0.25 );
                
                if ( MissionSettings.DebugMode )
                Print("[SMM] EscortDefend Mission : Test Hour is "+ hour +", SunRiseHour: "+ SunRiseHour +", SonSetHour: "+ SunSetHour );
                
                if ( hour >= SunSetHour || hour <= SunRiseHour ) 
                {
					//remove cap
					EntityAI cap = EscortBot.GetInventory().GetAttachmentFromIndex( 0 );
					if ( cap.GetType() == "PoliceCap")
					{
						Print("[SMM] Bot drops the cap and gets headtorch");
						g_Game.ObjectDelete( cap );
					}
					else Print("[SMM] wrong slot ID for cap.");
					
					EntityAI headTorch;
					headTorch = EscortBot.GetInventory().CreateAttachment("HeadTorch_Grey");
					headTorch.GetInventory().CreateAttachment("Battery9v");
					headTorch.GetCompEM().SwitchOn();
                }
            }
        }
		
		//update group waypoints
		MissionGroup.ClearWaypoints();
		Print("[SMM] EscortDefend Mission : Waypoints cleared.");
		
		//set bot speed to JOG
		EscortBot.SetMovementSpeedLimits(2.0);
		
		//try to get a nearby random position on land, retry if under water
		for ( int k=0; k < 50 ; k++)
		{
			//Get random nearby location
			float x = Math.RandomFloatInclusive( 200 , 500 );
			float y = Math.RandomFloatInclusive( 200 , 500 );
			int Dice = Math.RandomIntInclusive(0,9);
			if ( Dice > 4 ) x *= -1;
			Dice = Math.RandomIntInclusive(0,9);
			if ( Dice < 5 ) y *= -1;
			RandomNearbyDest = MissionBuilding.ModelToWorld( Vector( x, 0, y ) );
			RandomNearbyDest[1] = g_Game.SurfaceY( RandomNearbyDest[0], RandomNearbyDest[2]);
			bool ItemsAtGround = false;
			
			if ( RandomNearbyDest[1] > 1.0 )
			{
				Object FoundObject;
				g_Game.GetObjectsAtPosition( RandomNearbyDest,10.0, m_ObjectList, m_ObjectCargoList );
				for ( int o=0; o < m_ObjectList.Count(); o++)
				{
					FoundObject = m_ObjectList.Get(o);
					if ( FoundObject.IsRock() || FoundObject.IsBuilding() || FoundObject.IsTree() || FoundObject.IsInherited(BaseBuildingBase) || FoundObject.IsPlainObject() )
					{
						Print("[SMM] EscortDefend Mission : WARNING :: Attempt "+ (k + 1) +" - The random stash position is near the ObjectType "+ FoundObject.GetType() +"! "+ RandomNearbyDest.ToString() );
						ItemsAtGround = true;
						break;
					}
				}
				if ( ItemsAtGround ) continue;
				else break;
			}
			else
			Print("[SMM] EscortDefend Mission : WARNING :: Attempt "+ (k + 1) +" - The random stash position is under sea level! "+ RandomNearbyDest.ToString() );
			if ( k == 49 )
			Print("[SMM] EscortDefend Mission : ERROR :: Couldn't find a proper position for the mission stash after 50 attempts! Stash with reward was not spawned.");
		}

		FinalDestination = RandomNearbyDest;
		
		//update group waypoints
		MissionGroup.AddWaypoint( RandomNearbyDest + "0.8 0 0.8");
		Print("[SMM] EscortDefend Mission : Waypoint for Loot and Mission End Set.");
		
		//check darkness for location light support and spawn lamp if needed
		EntityAI canister;
		ItemBase lamp;
		
		g_Game.GetWorld().GetDate( year, month, day, hour, minute );		
		
		currentMission = g_Game.GetMission();

		if ( currentMission )
		{			
			worldData = currentMission.GetWorldData();
			if ( worldData )
			{
				ApproxSunrise = worldData.GetApproxSunriseTime( month + ( day / 32.0 ) );
				ApproxSunset = worldData.GetApproxSunsetTime( month + ( day / 32.0 ) );
				
				SunRiseHour = Math.Round( ApproxSunrise + 0.25 );
				SunSetHour = Math.Round( ApproxSunset - 0.25 );
				
				if ( MissionSettings.DebugMode )
				Print("[SMM] EscortDefend Mission : Test Hour is "+ hour +", SunRiseHour: "+ SunRiseHour +", SonSetHour: "+ SunSetHour );
				
				if ( hour >= SunSetHour || hour <= SunRiseHour ) 
				{

					//Choose the Light position Here. 
					lightPos = RandomNearbyDest + "-0.5 0 -0.5";
					MissionLight = g_Game.CreateObjectEx("PortableGasLamp", lightPos, ECE_PLACE_ON_SURFACE);

					lamp = ItemBase.Cast(MissionLight);
					if ( !lamp )
					{
						Print("[SMM] EscortDefend Mission : ERROR :: Could not spawn mission light at " + lightPos);
						return;
					}

					canister = lamp.GetInventory().CreateAttachment("SmallGasCanister");
					if ( !canister )
					{
						Print("[SMM] EscortDefend Mission : ERROR :: Could not attach SmallGasCanister to mission light.");
					}

					if ( lamp.HasEnergyManager() && lamp.GetCompEM().CanSwitchOn() )
					{
						lamp.GetCompEM().SwitchOn();
					}
					else
					{
						Print("[SMM] EscortDefend Mission : ERROR :: Mission light can not be switched on.");
					}
					m_MissionObjects.Insert( lamp );
				}
			}
		}
		
		//Spawn shovel at stash pos
		Object shovel = g_Game.CreateObject( "Shovel", RandomNearbyDest );
		shovel.SetPosition( RandomNearbyDest + "0.3 0.5 0.3");
		shovel.SetOrientation("0 170 0");

		//Create underground stash  
		stash = UndergroundStash.Cast( g_Game.CreateObject( "UndergroundStash", RandomNearbyDest ));
		stash.PlaceOnGround(); 
		Print(" Orientation UndergroundStash : "+ stash.GetOrientation().ToString() );
		vector pos = stash.GetPosition();
		vector ori = stash.GetOrientation();
		
		if ( ori[2] == -180 || ori[2] == 180 )
		{	
			ori[2] = 0;
			stash.SetOrientation( ori );
		}
		
		//Spawn seachest in stash
		MissionObject = ItemBase.Cast( stash.GetInventory().CreateInInventory("SeaChest"));
		MissionObject.SetTakeable(false);		
		
		//Clutter cutter stash 	
		Object cc_stash = g_Game.CreateObject( "ClutterCutter2x2" , MissionObject.GetPosition() );
		g_Game.RemoteObjectCreate( cc_stash );		
		
		//Insert mission objects into mission objects list
		m_MissionObjects.Insert( MissionObject );
		m_MissionObjects.InsertAt( stash, 0 );		 
		m_MissionObjects.Insert( cc_stash );		

	}

	override void MissionFinal()
	{
		//not used in this template
	}

	bool HasValidEscortLeader()
	{
		if (!MissionGroup)
			return false;

		DayZPlayerImplement leader = MissionGroup.GetLeader();
		if (!leader)
			return false;

		if (leader == EscortBot || leader.IsAI())
			return true;

		PlayerBase playerLeader = PlayerBase.Cast(leader);
		if (!playerLeader)
			return false;

		if (!playerLeader.IsAlive())
			return false;

		if (!playerLeader.GetIdentity())
			return false;

		if (!MissionGroup.IsMember(playerLeader))
			return false;

		return true;
	}

	void ResetEscortLeaderToBot(string reason)
	{
		if (!MissionGroup || !EscortBot)
			return;

		Print("[SMM] EscortDefend Mission : Resetting escort leadership to bot. Reason: " + reason);

		EscortBotFollowsLeader = false;
		TempLeader = null;
		LastKnownPlayerLeader = null;

		for (int removeIdx = MissionGroup.Count() - 1; removeIdx >= 0; removeIdx--)
		{
			DayZPlayerImplement member = MissionGroup.GetMember(removeIdx);
			if (member && member != EscortBot)
				MissionGroup.RemoveMember(member);
		}

		MissionGroup.SetLeader(EscortBot);
		MissionGroup.ClearWaypoints();
		MissionGroup.SetFormation(new eAIFormationFile);

		if (m_PlayersInZone)
			m_PlayersInZone.Clear();

		LeaderWasLost = true;
		m_MissionZoneOuterRadius = 25.0;
		m_MissionZoneInnerRadius = 0.001;
		m_MissionPosition = EscortBot.GetPosition();
	}
	
	override void EntityChecks()
	{	//Executed continuously when mission was actived. Avoid complex calculations or big data processing!
		//Escort bot checks 
		if ( EscortBot && !m_RewardsSpawned )
		{
			//if bot is dead, abbort mission
			if ( !EscortBot.IsAlive() && !m_MissionTimedOut )
			{
				m_MissionTimedOut = true;
				EscortBotFollowsLeader = false;
				m_MsgNum = -3;
				m_MsgChkTime = m_MissionTime + 3;
				
				EscortBot = null;
				
				Print("[SMM] EscortDefend Mission : The mission target died, the mission is set to 'Timed Out'. Sending Mission Failed message via radio.");
				return;
			}
			
			if ( !m_MissionExtended && !Finaltrigger )
			EscortBot.SetMovementSpeedLimits(3.0);

			if (EscortBotFollowsLeader && !HasValidEscortLeader())
			{
				ResetEscortLeaderToBot("player leader logged out, disconnected or became invalid");
				return;
			}

			//Get server runtime for timer functions
			crntECTime = g_Game.GetTime();
			
			//Bot Anti-stuck
			if ( !lastPosition ) lastPosition =  EscortBot.GetPosition(); //init
			
			if ( crntECTime >= StuckChkTime + 1000 )
			{
				currentPosition = EscortBot.GetPosition();

				if ( vector.Distance( currentPosition, lastPosition ) < 0.2 )
				{
					//Anti-Stuck in following mode
					if ( !TempLeader && vector.Distance( currentPosition, MissionGroup.GetLeader().GetPosition() ) > 4 )
					{						
						TempLeader = PlayerBase.Cast( MissionGroup.GetLeader() );
						TempLeaderPos = TempLeader.GetPosition();
						
						if ( TempLeader )
						Print("[SMM] EscortDefend Mission : Bot stuck detected. Temporary set the bot as leader. Leader was "+ TempLeader.GetIdentity().GetPlainId() );
						else Print("[SMM] EscortDefend Mission : Leader is NULL before temporary adding Bot as leader.");
						
						MissionGroup.SetLeader( EscortBot );
						
						MissionGroup.ClearWaypoints();
						MissionGroup.AddWaypoint( TempLeaderPos );

					}
					else if ( TempLeader && vector.Distance( currentPosition, TempLeaderPos ) < 3.0 )
					{
						Print("[SMM] EscortDefend Mission : Bot reached the leader's last position. Player "+ TempLeader.GetIdentity().GetPlainId() +" is now leader again.");
						MissionGroup.SetLeader( TempLeader );
						TempLeader = null;
						MissionGroup.ClearWaypoints();
						MissionGroup.SetFormation( new eAIFormationFile );
					}
				}
				
				//rem
				lastPosition = currentPosition;
				StuckChkTime = crntECTime;
			}
						
			//Escort bot joins the group leader and starts a conversation
			if ( EscortBotFollowsLeader ) 
			{
				auto leader = MissionGroup.GetLeader();
				
				//bot is always changing stance to match player stance in following mode
				//get leaders stance				
				Human LeaderHuman;
				Class.CastTo( LeaderHuman, leader );
				if ( LeaderHuman )
				{
					LeaderHuman.GetMovementState( LeaderStance );
					if ( LeaderStance.m_iStanceIdx != LastLeaderStanceID )
					{
						if ( LeaderStance.m_iStanceIdx == 0 )
						EscortBot.OverrideStance( DayZPlayerConstants.STANCEIDX_ERECT, false );
						else EscortBot.OverrideStance( LeaderStance.m_iStanceIdx, false );
						
						LastLeaderStanceID = LeaderStance.m_iStanceIdx;
					}
				}
				 
				// Only proceed if leader is valid and alive
				if (leader && vector.Distance( EscortBot.GetPosition(), leader.GetPosition() ) < 6 && leader.IsAlive() && !BotFound )
				{
					BotFound = true;
					EscortLeftHouse = true;
					//reset secondary message to escort mode
/*					if ( EscortLeftHouse )
					{
						m_MissionInformant = "Cpt. Frank";
						m_MissionMessage1 = SurvivorName +" do you read me?\n\nI know where you are going and I can tell you that's not a good idea. The informations about the hospital's investigation are secret and you are not allowed to take it.";
						m_MissionMessage2 = "All Bohemian Rebels, this is Captain Frank speaking. We have a spy going for the documents which we were searching for weeks now. They are very valuable for the Umbrella Corp.";
						m_MissionMessage3 = "They promised to bring better weapons and equipment to our base. I have intel that "+ SurvivorName +" is on the way to "+ m_MissionSecondaryLoc +".\n\nHe may have a companion. Stop him immediately!";
						m_MsgChkTime = m_MissionTime + 15;
					}
*/
					// Set GrpLeaderName to the player who triggered the inner mission zone
					string GrpLeaderName = "Survivor";
					PlayerBase triggeringPlayer;
					if (m_PlayersInZone)
					{
						for (int i = 0; i < m_PlayersInZone.Count(); i++)
						{
							PlayerBase playerPt;
							string playerId = m_PlayersInZone.Get(i);
							foreach (Man man : ClientData.m_PlayerBaseList)
							{
								PlayerBase testPlayer = PlayerBase.Cast(man);
								if (testPlayer && testPlayer.GetIdentity() && testPlayer.GetIdentity().GetId() == playerId)
								{
									playerPt = testPlayer;
									break;
								}
							}
							if (playerPt && vector.Distance(m_MissionPosition, playerPt.GetPosition()) < m_MissionZoneInnerRadius + 3.0)
							{
								triggeringPlayer = playerPt;
								break;
							}
						}
					}
					if (triggeringPlayer && triggeringPlayer.GetIdentity())
					{
						GrpLeaderName = triggeringPlayer.GetIdentity().GetName();
					}
					else if (MissionGroup && MissionGroup.GetLeader() && MissionGroup.GetLeader().GetIdentity())
					{
						GrpLeaderName = MissionGroup.GetLeader().GetIdentity().GetName();
					}
					string icon = "set:dayz_gui image:notification_friend";
					string msg = "Hello "+ GrpLeaderName +"!\nI am glad that you found me. "+ m_MissionInformant +" told me about you. Please bring me to the "+ m_MissionSecondaryLoc +"'s Police Station, where the important docs are. I will follow you, please stay close!";
					NotificationSystem.SendNotificationToPlayerIdentityExtended( MissionGroup.GetLeader().GetIdentity() , 8.0, SurvivorName + ":", msg, icon );                
				}
			}
			
			// Detect if the last known player leader is dead or removed
			if (LastKnownPlayerLeader && !m_RewardsSpawned && EscortBotFollowsLeader && EscortLeftHouse )
			{
				bool playerLeaderGone = false;
				if (!LastKnownPlayerLeader.IsAlive())
				{
					Print("[SMM] EscortDefend Mission : LastKnownPlayerLeader is dead.");
					playerLeaderGone = true;
				}
				else if (!MissionGroup.IsMember(LastKnownPlayerLeader))
				{
					Print("[SMM] EscortDefend Mission : LastKnownPlayerLeader is no longer in the group.");
					playerLeaderGone = true;
				}
				if (playerLeaderGone && !m_RewardsSpawned && !turnOffCheck && EscortLeftHouse)
				{
					if (!LeaderWasLost) // Only clear once per event
					{
						m_PlayersInZone.Clear();
						LeaderWasLost = true;
					}
					Print("[SMM] EscortDefend Mission : The player group leader died or left. Removing leader from group and setting Escort to idle.");
					EscortBotFollowsLeader = false;
					// Remove all non-bot members from the group
					// Remove all non-bot members by iterating backwards
					for (int removeIdx = MissionGroup.Count() - 1; removeIdx >= 0; removeIdx--) {
						DayZPlayerImplement member = MissionGroup.GetMember(removeIdx);
						if (member && member != EscortBot) {
							Print("[SMM][DEBUG] EscortDefend Mission : Removing member: " + member.ToString());
							MissionGroup.RemoveMember(member);
						}
					}
					MissionGroup.SetLeader(EscortBot);
                
					// Debug: Print current group members
					Print("[SMM] EscortDefend Mission : Current MissionGroup members after cleanup:");
					for (int debugIdx = 0; debugIdx < MissionGroup.Count(); debugIdx++) {
						Print("[SMM] EscortDefend Mission : Member " + debugIdx.ToString() + ": " + MissionGroup.GetMember(debugIdx).ToString());
					}

					// Restore mission zone radii so new players can become leader
					m_MissionZoneOuterRadius = 25.0; // original value from constructor
					m_MissionZoneInnerRadius = 0.001;  // original value from constructor
					
					// Center the mission zone on the bot's current position
					if (EscortBot) {
						m_MissionPosition = EscortBot.GetPosition();
						Print("[SMM] EscortDefend Mission : Mission zone recentered on EscortBot at " + m_MissionPosition.ToString());
					}

					// Reset last known player leader
					LastKnownPlayerLeader = null;
				}
				else
				{
					LeaderWasLost = false; // Reset flag if leader is not lost
				}
			}
			
			//Escort bot walks around in finding mode
			if ( !EscortBotFollowsLeader && EscortLeftHouse )
			{
				//get new destination every 10 sec
				if ( crntECTime >= EscortWalkChkTime + 10000 )
				{
					//Get random nearby location
					float x = Math.RandomFloatInclusive( 2.0, 5.0);
					float y = Math.RandomFloatInclusive( 2.0, 5.0);
					int Dice = Math.RandomIntInclusive(0,9);
					if ( Dice > 4 ) x *= -1;
					Dice = Math.RandomIntInclusive(0,9);
					if ( Dice < 5 ) y *= -1;
					vector NearbyDest = EscortBot.ModelToWorld( Vector( x, 0, y ) );
					NearbyDest[1] = g_Game.SurfaceY( NearbyDest[0], NearbyDest[2]);

					//update group waypoints
					MissionGroup.ClearWaypoints();
					MissionGroup.AddWaypoint( NearbyDest );

					EscortWalkChkTime = crntECTime;

					//translate SecondarySpawn coords		
				    XX = m_MissionPosition[0] / 10000;
				    if ( !MapGridYnorthAscending )
				    YY = ( EventsWorldData.EWD_MAP_SIZE - m_MissionPosition[2] ) / 10000;
					else
					YY = m_MissionPosition[2] / 10000;
						
					string Coords = " X "+ ( XX.ToString().Substring(0,4) ) +" / Y "+ ( YY.ToString().Substring(0,4) ) +" ";

					// Delete old marker and place new one at EscortBot's current position
					DeleteMarker();
					TargetPos = m_MissionPosition;
					MarkTargetSmoke();

					//set secondary messages to finding mode
					m_MissionInformant = "Dr. Legasov";
					
					m_MissionMessage1 = "It seems "+ SurvivorName +" has lost his escort. Thats very bad but he needs someone to escort him to the assumed location.";
					m_MissionMessage2 = "I can't tell you on radio, too many people could hear this and we don't want to expose him to any danger at this destination.";
					m_MissionMessage3 = "He has marked his current position with yellow smoke anywhere around...\n\nGPS coordinates: ** "+ Coords +" **\n\nTry to find him there and hurry up!";
				}
				
				//update mission position
				m_MissionPosition = EscortBot.GetPosition();
			}
			else m_MissionPosition = EscortBot.GetPosition();
		}

		//if the mission is extended do this
		if ( !m_MissionExtended && EscortBot && !m_RewardsSpawned )
		{			
			//Escort Bot to 2nd POS - DEFEND HIM for 2 Waves! - !!!!
            if ( MissionBuilding && m_PlayersInZone.Count() > 0 && !m_RewardsSpawned )
            {
              if ( !EscortBot ) Print("[SMM] EscortDefend Mission : ERROR :: EscortBot is Null after Esort part was accomplished!");
              if ( !MissionBuilding ) Print("[SMM] EscortDefend Mission : ERROR :: MissionBuilding is Null after Esort part was accomplished!");

              if ( vector.Distance( EscortBot.GetPosition(), MissionBuilding.ModelToWorld(EscortFinalPos) ) < 20.0 && MissionGroup.GetLeader() != EscortBot && !FinalDest )
				{   
					Print("[SMM] EscortDefend Mission : Player has successfully escorted the Bot to the requested position.");

					turnOffCheck = true;
										
					// Set the Escort Bot as the Leader.
					MissionGroup.SetLeader( EscortBot );
			
					//update goup waypoints
					MissionGroup.ClearWaypoints();
					Print("[SMM] EscortDefend Mission : Waypoints cleared. Adding indoor waypoint. Bot is now on his way to 2nd floor office.");
					MissionGroup.AddWaypoint( MissionBuilding.ModelToWorld( EscortFinalPos ));
				}
									
				// Once Bot is at office desk - Print Message - Start Firstwave Spawning
				if ( vector.Distance( EscortBot.GetPosition(), MissionBuilding.ModelToWorld(EscortFinalPos) ) < 3.0 )   
				{
					//First Wave Spawning only if First Wave isnt True and RewardSpawned Isnt True.
					if(!firstwavespawned && !m_RewardsSpawned)
					{

						//Set new bot messages in police station
						icon = "set:dayz_gui image:notification_friend";	
						string message1 = "We made it!, Thank you. Tell "+ m_MissionInformant +" that if we meet in person, I owe him a drink. ";
						string message2 = "There should be the important documents anywhere up here. Unless it was taken by another survivor.\nWait... What's that Sound. I think I hear a Horde Outside!!!!\n";
						string message3 = "Get down stairs, close the doors to the building and prepare to defend us!";
						
						NotificationSystem.SendNotificationToPlayerIdentityExtended( MissionGroup.GetLeader().GetIdentity() , 10.0, SurvivorName + ":", message1 + message2 + message3, icon );

						
						firstwavespawned = true;
						Firstwave();
						Print("[SMM] EscortDefend Mission : Spawning Wave 1");

					}					
				}
			}

			//Here we setup our counting of deadinfected.
			for ( int ix=0; ix < m_MissionAIs.Count(); ix++)
			{
				Object MissionAI = m_MissionAIs.Get(ix);
				if ( MissionAI && !MissionAI.IsAlive() )
				{
					if ( DeadInfected.Find( MissionAI ) > -1 )
					continue;
					else
					{
						DeadInfected.Insert( MissionAI );
					}
				}
			}

			//now we check if the array has the wanted amount of dead infected( I use 5 less than spawn in case a few get stuck in a rock)
			icon = "set:dayz_gui image:notification_friend"; 
			string message;
			
			if ( DeadInfected.Count() >= Killrequiredwave1 && !secondwavespawned && !m_RewardsSpawned && turnOffCheck )
			{
				Print("[SMM] EscortDefend Mission : The players in the mission zone successfully killed the first requested amount of "+ Killrequiredwave1 +" infected.");
				SecondWave(); 
				secondwavespawned = true;
				message = "That's it "+ GrpLeaderName +"!\nI am glad that you are here!! "+ m_MissionInformant +" was right to send you!!!\nJust a little more now!";
				           
				NotificationSystem.SendNotificationToPlayerIdentityExtended( MissionGroup.GetLeader().GetIdentity() , 10.0, SurvivorName + ":", message, icon );
			}
			
			//now we check if the array has the wanted amount of dead infected ( I use 5 less than spawn in case a few get stuck in a rock) and once met proceed to Final Chase.
			if ( DeadInfected.Count() >= Killrequiredwave2 && !m_RewardsSpawned && !Secondtrigger && turnOffCheck )
			{
				Secondtrigger = true;
				Print("[SMM] EscortDefend Mission : The players in the mission zone successfully killed the second requested amount of "+ Killrequiredwave2 +" infected.");
				message = "That's it"+ GrpLeaderName +"!\nI think that gave us the opening we needed!! Let's Head outside and finish the rest off!!!";
				           
				NotificationSystem.SendNotificationToPlayerIdentityExtended( MissionGroup.GetLeader().GetIdentity() , 10.0, SurvivorName + ":", message, icon );

			}
			//now we check if the array has the wanted amount of dead infected ( I use 5 less than spawn in case a few get stuck in a rock) and once met proceed to Final Chase.
			if ( DeadInfected.Count() >= Killrequiredfinal && !m_RewardsSpawned && !Finaltrigger && turnOffCheck )
			{
				Finaltrigger = true;
				Print("[SMM] EscortDefend Mission : The players in the mission zone successfully killed the third requested amount of "+ Killrequiredfinal +" infected.");
/*
				message = "Ok "+ GrpLeaderName +"!\n Let's get out of here!!!!";
				           
				NotificationSystem.SendNotificationToPlayerIdentityExtended( MissionGroup.GetLeader().GetIdentity() , 10.0, SurvivorName + ":", message, icon );
*/				
				//send the bot to the final destination / stash position
				FinalChase();
				
				//disable radio messages after police station was cleared and EscortBot is jogging to rewards 
				MsgCutoff = true;
			}		
			
			if ( FinalDestination && vector.Distance( EscortBot.GetPosition(), FinalDestination ) <= 3.0 && !FinalDest  )
			{
				//check if stash was dug out = null
				if ( !stash )
				{
					ChestPos = MissionObject.GetPosition();
					ChestOri = MissionObject.GetOrientation();
					
					g_Game.ObjectDelete( MissionObject );
					g_Game.GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( this.SpawnRewards ,1000 );
					
					FinalDest = true;
					
					//send message to player for bot's kill request
					message = "Take all you need. Just do me a last favor, please.\nUnfortunately, I was infected and I want you to kill me before I will turn into such things.\nThank you and live long!";
					
					PlayerBase ZonePlayer = PlayerBase.Cast( MissionGroup.GetMember(1) );
					if ( ZonePlayer )
					{
						NotificationSystem.SendNotificationToPlayerIdentityExtended( ZonePlayer.GetIdentity() , 10.0, SurvivorName + ":", message, icon );
					}
					
					Print("[SMM] EscortDefend Mission : Player reached FinalDestination and dug out the MissionObject.");
					
					//Log assisting players
					PlayerBase MissionPlayer;
					
					g_Game.GetObjectsAtPosition3D( FinalDestination, 7, m_ObjectList, m_ObjectCargoList );
					
					for ( int k=0; k < m_ObjectList.Count(); k++)
					{
						Object FoundObject = m_ObjectList.Get(k);
						
						if ( FoundObject.IsMan() )
						{
							Class.CastTo( MissionPlayer, FoundObject );
							
							if ( MissionPlayer )
							{
								if ( !MissionPlayer.Expansion_IsAI() )
								AssistingPlayers.Insert( MissionPlayer );
							}
						}
					}
			
					string RespectMsg, ExtMsg, MsgIcon;
						
					if ( AssistingPlayers.Count() > 1 )
					ExtMsg = "teamwork";
					else ExtMsg = "braveness";
					
					RespectMsg = "You earned respect for your "+ ExtMsg +"!";
					MsgIcon = "set:ccgui_enforce image:MapShieldBooster";
					
					for ( int m=0; m < AssistingPlayers.Count(); m++)
					{
						PlayerBase AssistPlayer = AssistingPlayers.Get(m);
						PlayerIdentity Ident = AssistPlayer.GetIdentity();
						string IdentPlainID = AssistPlayer.GetIdentity().GetPlainId();
						
						//log player
						Print("[SMM] EscortDefend Mission : Player with SteamID64: "+ IdentPlainID +" deserves respect for his "+ ExtMsg +".");
						//send Respect message to player
						NotificationSystem.SendNotificationToPlayerIdentityExtended( Ident , 15.0, RespectMsg, "", MsgIcon );
					}
				}
			}
		}
	}

	override bool DeployMission()	
	{	//When first player enters the mission zone (primary/secondary)
		//set messages from Cpt. Frank whenever this mission deploys (player found bot in house or rejoins AI group on the way to secondary)
		m_MissionInformant = "Cpt. Frank";
		m_MissionMessage1 = SurvivorName +" do you read me?\n\nI know where you are going and I can tell you that's not a good idea. The informations about the hospital's investigation are secret and you are not allowed to take it.";
		m_MissionMessage2 = "All Bohemian Rebels, this is Captain Frank speaking. We have a spy going for the documents which we were searching for weeks now. They are very valuable for the Umbrella Corp.";
		m_MissionMessage3 = "They promised to bring better weapons and equipment to our base. I have intel that "+ SurvivorName +" is on the way to "+ m_MissionSecondaryLoc +".\n\nHe may have a companion. Stop him immediately!";
		
		if ( m_MissionPosition != "0 0 0" )
		{
			//Call spawners
			g_Game.GetCallQueue( CALL_CATEGORY_GAMEPLAY ).Call( this.SpawnObjects );
			g_Game.GetCallQueue( CALL_CATEGORY_GAMEPLAY ).Call( this.SpawnAIs );
			return true;
		}
		else 
		{
			Print("[SMM] ERROR : Side Mission position was rejected or doesn't exist. MissionPosition is NULL!");
			return false;
		}
	}
#endif
#endif
}