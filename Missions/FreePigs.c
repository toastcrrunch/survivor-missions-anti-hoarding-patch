class FreePigsMission extends SurvivorMissions		//by Xellose and Funkdoc
{
	//Mission related entities 
	Object MissionBuilding;
	Object MissionLight;
	ItemBase MissionObject;
	
	//Mission parameters
	int ReqMeatAmount = 14;					//pieces, mission ganja (equivalent to x * 4 gramms)
	int MsgDlyFinish = 60;					//seconds, message delay time after player has finished mission
	
	//Mission containers
	ref array<vector> ExtendedPosList = new array<vector>;
	ref array<vector> Spawnpoints = new array<vector>;
	ref array<vector> PriInfectSpawns = new array<vector>;
	ref array<vector> SecInfectSpawns = new array<vector>;
	ref array<string> InfectedTypes = new array<string>;
	ref array<vector> CagedPigSpawns = new array<vector>;
	ref array<Object> DeadPigs = new array<Object>;
	ref array<vector> DeadPigsPos = new array<vector>;
	ref array<PlayerBase> AssistingPlayers = new array<PlayerBase>;
	
	//Mission variables 
	string SurvivorName, SecondaryLoc;
	vector RewardsPosition = "0.84 -0.75 0.16";	
	vector TargetPosition = "-1.74 -3.01 0.49";
	int EntChkTime = 0;
	int LastCount = 0;
	int CargoCount;
	int FoundObjects;	
	
	override bool IsExtended() { return true; };
	
	void FreePigsMission()
	{		
		//Select primary mission
		m_MissionExtended = true;
		
		//Mission timeout
		m_MissionTimeout = 2700;			//seconds, primary mission duration time
		m_ExtendedTimout = 1500;			//seconds, mission duration time for extended mission
		
		//Mission zones
		m_MissionZoneOuterRadius = 150;	//meters (!Do not set lower than 200m), mission activation distance
		m_MissionZoneInnerRadius = 10;		//meters (!Do not set outside of 1-5m), mission finishing distance to target object
				
		//Mission informant
		m_MissionInformant = "Dr. Legasov";
	
		//Mission person names
		TStringArray SurvivorNames = {"Yuri","Michail","Boris","Valeri","Anatoli","Ivan","Alexej","Dimitrij","Sergej","Nikolai","Vladimir"};
		SurvivorName = SurvivorNames.GetRandomElement();
		
		//Translate mission location name and direction
		if ( MissionSettings.Opt_TranslateEWData )
		{
			m_MissionLocation = EventsWorldData.TransLoc( m_MissionLocation );
			m_MissionLocationDir = EventsWorldData.TransDir( m_MissionLocationDir );
		}
				
		//Set mission messages for primary mission
		m_MissionMessage1 = SurvivorName +", a farmer from "+ m_MissionLocation +", died 2 days ago after a attack by bandits. He was protecting his animals from infected and scavengers and was farming pigs to deliver meat to the survivors hideout, somewhere in the forests.";
		m_MissionMessage2 = "He was well known under survivors for his perfection in making good bacon and spiced meat for barbecue. His pigs are still in the shed and I am afraid they could starve to death, if they are not already dead by infected or robbed by bandits.";
		m_MissionMessage3 = "Go to "+ SurvivorName +"'s farm located \n** "+ m_MissionLocationDir +" of "+ m_MissionLocation +" **\nand free all the pigs from their cage to give them a chance to survive. You need at least a knife for this task.";
		//Add GPS coordinates of mission in message
		if ( MissionSettings.Opt_ShowGpsInfo ) m_MissionMessage3 += "\n\nGPS coordinates:"+ m_MissionInstance.GetGPS();
				
		//Spawnpoints for caged pigs in farm (view from inside building at door position)
		//right cage row
		CagedPigSpawns.Insert("0.92 -3.01 3.10"); 
		CagedPigSpawns.Insert("-0.49 -3.01 3.10");
		CagedPigSpawns.Insert("-4.72 -3.01 3.10");
		CagedPigSpawns.Insert("-6.13 -3.01 3.10");
		//left cage row 
		CagedPigSpawns.Insert("-7.42 -3.01 -2.21");
		//next building part of shed left 
		CagedPigSpawns.Insert("7.78 -3.04 2.85");
		CagedPigSpawns.Insert("6.28 -3.04 2.85");
		
		//Spawnpoints for MissionObjects in middle garage (Do not change order!)
		Spawnpoints.Insert("0.249 -3.01 -3.24");  	//at left door to chicken room (Wellies)

		//Infected spawnpoints for primary mission
		PriInfectSpawns.Insert("-30 0 -10");
		PriInfectSpawns.Insert("-35 0 -13");
		PriInfectSpawns.Insert("-40 0 -9");
		PriInfectSpawns.Insert("-45 0 -15");
		PriInfectSpawns.Insert("-30 0 10");
		PriInfectSpawns.Insert("-35 0 13");
		PriInfectSpawns.Insert("-40 0 9");
		PriInfectSpawns.Insert("-45 0 15");
		
		//Infected spawnpoints for secondary mission 
		SecInfectSpawns.Insert("-11 0 -7");
		SecInfectSpawns.Insert("-8 0 8");
		SecInfectSpawns.Insert("8 0 -9");
		SecInfectSpawns.Insert("6 0 -5");
		SecInfectSpawns.Insert("18 0 15");
		
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
						
		//Search for mission building at primary mission position	
		g_Game.GetObjectsAtPosition( m_MissionPosition , 1.0 , m_ObjectList , m_ObjectCargoList );
		for ( int j = 0 ; j < m_ObjectList.Count(); j++ )
		{ 
			Object FoundObject = m_ObjectList.Get(j);
			if ( FoundObject.GetType() == "Land_Farm_CowshedA")
			{	
				MissionBuilding = FoundObject;			 
				Print("[SMM] FreePigs Mission : MissionBuilding is "+ MissionBuilding.GetType() +" at "+ m_MissionDescription[2] +" of "+ m_MissionDescription[1] +" @ "+ MissionBuilding.GetPosition() );
				break;
			}	
		}
		if ( !MissionBuilding ) Print("[SMM] FreePigs Mission : Farm couldn't be found. Mission rejected!");
		else
		{
			//Close all farm doors
			Building farm = Building.Cast( MissionBuilding );
			for ( int m=0; m < 5; m++ ) if ( farm.IsDoorOpen(m) ) farm.CloseDoor(m); 
	
			g_Game.GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( g_Game.UpdatePathgraphRegionByObject, 1000, false, farm );		
		}				
	}
	
	void ~FreePigsMission()
	{	
		//Despawn all remaining mission objects
		if ( m_MissionObjects )
		{
			Print("[SMM] FreePigs Mission : Despawning "+ m_MissionObjects.Count() +" mission objects from old mission...");		
			for ( int i = 0; i < m_MissionObjects.Count(); i++ )
			{				
				if ( !m_MissionObjects.Get(i))	continue;
				else g_Game.ObjectDelete( m_MissionObjects.Get(i) );			
			}
			m_MissionObjects.Clear();
		}
			
		//Delete mission AI's
		if ( m_MissionAIs )
		{
			if ( m_MissionAIs.Count() > 0 )
			{
				Print("[SMM] FreePigs Mission : Despawning "+ m_MissionAIs.Count() +" mission AI's from old mission...");
				for ( int k = 0; k < m_MissionAIs.Count(); k++ )
				{
					g_Game.ObjectDelete( m_MissionAIs.Get(k) );
				}
				m_MissionAIs.Clear();			
			}
			else Print("[SMM] FreePigs Mission : No mission AI's to despawn.");
		}
		
		//Delete PlayersInZone list
		if ( m_PlayersInZone )	m_PlayersInZone.Clear();
	}

	void SpawnRewards()
	{						
		//new MissionObject after deleting hunting bag
		MissionObject = ItemBase.Cast( g_Game.CreateObjectEx("MissionChest", m_MissionPosition, ECE_PLACE_ON_SURFACE ));
		MissionObject.SetPosition( m_MissionPosition );
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
				weapon.GetInventory().CreateAttachment("'AK_Suppressor");			
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
					
		Print("[SMM] FreePigs Mission : Mission rewards spawned in reward container. Randomly selected loadout was "+selectedLoadout+"." );		
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
			//New MissionPosition is at first cages in farm building
			m_MissionPosition = MissionBuilding.ModelToWorld( TargetPosition );
			
			//Spawn farmer loot			
			m_MissionObjects.Insert( g_Game.CreateObjectEx("Wellies_Green", MissionBuilding.ModelToWorld( Spawnpoints.Get(0) ), ECE_PLACE_ON_SURFACE ));
			
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
					Print("[SMM] FreePigs Mission : Test Hour is "+ hour +", SunRiseHour: "+ SunRiseHour +", SonSetHour: "+ SunSetHour );
					
					if ( hour >= SunSetHour || hour <= SunRiseHour ) 
					{
	
						//Choose the Light position Here. 
						lightPos = MissionBuilding.ModelToWorld(TargetPosition);
						MissionLight = g_Game.CreateObjectEx("PortableGasLamp", lightPos, ECE_PLACE_ON_SURFACE);
	
						lamp = ItemBase.Cast(MissionLight);
						if ( !lamp )
						{
							Print("[SMM] FreePigs Mission : ERROR :: Could not spawn mission light at " + lightPos);
							return;
						}
	
						canister = lamp.GetInventory().CreateAttachment("SmallGasCanister");
						if ( !canister )
						{
							Print("[SMM] FreePigs Mission : ERROR :: Could not attach SmallGasCanister to mission light.");
						}
	
						if ( lamp.HasEnergyManager() && lamp.GetCompEM().CanSwitchOn() )
						{
							lamp.GetCompEM().SwitchOn();
						}
						else
						{
							Print("[SMM] FreePigs Mission : ERROR :: Mission light can not be switched on.");
						}
						m_MissionObjects.Insert( lamp );
					}
				}
			}
			
			Print("[SMM] FreePigs Mission : Survivor Mission "+ m_selectedMission +" :: "+ m_MissionName +" ...primary mission deployed!");
		}
		else
		{
			//New MissionPosition is hiking rest table
			m_MissionPosition = MissionBuilding.ModelToWorld( RewardsPosition );
			
			//Spawn mission target huntig bag
			MissionObject = ItemBase.Cast( g_Game.CreateObjectEx("MissionBagMedium", m_MissionPosition, ECE_PLACE_ON_SURFACE ));
			MissionObject.SetPosition( m_MissionPosition );
			MissionObject.SetOrientation( MissionBuilding.GetOrientation() + "-90 -25 0" );
			MissionObject.SetTakeable(false);
			m_MissionObjects.InsertAt( MissionObject, 0 );
			
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
					Print("[SMM] FreePigs Mission : Test Hour is "+ hour +", SunRiseHour: "+ SunRiseHour +", SonSetHour: "+ SunSetHour );
					
					if ( hour >= SunSetHour || hour <= SunRiseHour ) 
					{
	
						//Choose the Light position Here. 
						lightPos = MissionBuilding.ModelToWorld("-0.186 -0.2895 -0.064");
						MissionLight = g_Game.CreateObjectEx("PortableGasLamp", lightPos, ECE_PLACE_ON_SURFACE);
	
						lamp = ItemBase.Cast(MissionLight);
						if ( !lamp )
						{
							Print("[SMM] FreePigs Mission : ERROR :: Could not spawn mission light at " + lightPos);
							return;
						}
	
						canister = lamp.GetInventory().CreateAttachment("SmallGasCanister");
						if ( !canister )
						{
							Print("[SMM] FreePigs Mission : ERROR :: Could not attach SmallGasCanister to mission light.");
						}
	
						if ( lamp.HasEnergyManager() && lamp.GetCompEM().CanSwitchOn() )
						{
							lamp.GetCompEM().SwitchOn();
						}
						else
						{
							Print("[SMM] FreePigs Mission : ERROR :: Mission light can not be switched on.");
						}
						m_MissionObjects.Insert( lamp );
					}
				}
			}
							
			Print("[SMM] FreePigs Mission : Survivor Mission Extension "+ m_selectedMission +" :: "+ m_MissionName +" ...secondary mission deployed!");
		}
	}
	
	void SpawnAIs()
	{	
		int PigPosCount;
		string RandomInfected;
		vector InfectedPos;
		vector PigPos;
		DayZInfected Zed;
		Object Pig;
		Object MissionBuilding2;
				
		if ( !MissionBuilding ) Print("[SMM] FreePigs Mission : Extended Mission : Missionbuilding couldn't be found in "+ SecondaryLoc +".");		
		else
		{
			if ( m_MissionExtended )
			{
				//Search for second farm building part 
				g_Game.GetObjectsAtPosition( MissionBuilding.ModelToWorld("-19.54 0 0"), 1.5, m_ObjectList, m_ObjectCargoList );				
				for ( int i=0; i < m_ObjectList.Count(); i++)
				{
					Object FoundObject = m_ObjectList.Get(i);
					if ( FoundObject.GetType() == "Land_Farm_CowshedB" )
					{
						Print("[SMM] FreePigs Mission : Second farm building found.");
						MissionBuilding2 = FoundObject;
						PigPosCount = CagedPigSpawns.Count();
						break;
					}
				}
				
				if ( !MissionBuilding2 )
				{
					Print("[SMM] FreePigs Mission : No second farm building found");
					PigPosCount = 5;
				}
				
				//Spawn caged pigs in farm
				for ( int j = 0 ; j < PigPosCount ; j++ )
				{
					if ( j < 5 )
					PigPos = MissionBuilding.ModelToWorld( CagedPigSpawns.Get(j) );		
					else PigPos = MissionBuilding2.ModelToWorld( CagedPigSpawns.Get(j) );
					
					Pig = g_Game.CreateObjectEx( "Animal_SusDomesticus", PigPos, ECE_PLACE_ON_SURFACE | ECE_INITAI );
					Pig.SetPosition( PigPos );
					m_MissionAIs.Insert( Pig );
				}	
				
				//Spawn a hen (stays on server til restart)
				g_Game.CreateObjectEx( "Animal_GallusGallusDomesticus", MissionBuilding.ModelToWorld("6.8 -3.01 -5.76"), ECE_PLACE_ON_SURFACE | ECE_INITAI );
			}
			else
			{
				//Spawn some infected at hiking rest
				for ( int k = 0 ; k < SecInfectSpawns.Count() ; k++ )
				{
		    	   	RandomInfected = InfectedTypes.GetRandomElement();
					InfectedPos = MissionBuilding.ModelToWorld( SecInfectSpawns.Get(k) );
					Zed = DayZInfected.Cast( g_Game.CreateObjectEx( RandomInfected, InfectedPos, ECE_PLACE_ON_SURFACE | ECE_INITAI ));
					m_MissionAIs.Insert( Zed );
				}		
			}
		}
	}
	
	override void ObjDespawn() 
	{	

	}
	
	override void ExtendMission()
	{	//When player enters mission target zone at primary mission
		
		//init Messenger for new messages
		m_MsgNum = 1;					//skip msg 0, begin with msg 1
		m_MsgChkTime = m_MissionTime;
		MsgCutoff = false;
		
		//increase mission MissionTimeout for secondary mission
		m_MissionTimeout = m_MissionTime + m_ExtendedTimout;  
		
		//deployment settings & init for secondary mission		
		m_MissionZoneOuterRadius = 120.0;
		m_MissionZoneInnerRadius = 2.0;
		
		//Get secondary mission position
		if ( EventsWorldData.GetBuildingsAtLoc("Land_Misc_TrailRoof_Small", SecondaryLoc , ExtendedPosList ))
		{
			m_MissionPosition = ExtendedPosList.GetRandomElement();
			Print("[SMM] FreePigs Mission : New Mission Position is Land_Misc_TrailRoof_Small at "+ SecondaryLoc +" @"+ m_MissionPosition );
			SecondaryLoc.Replace("_"," ");
		}
		else Print("[SMM] FreePigs Mission : Can't find secondary MissionPosition in "+ SecondaryLoc +" from EventsWorldData!");  
		
		//translate secondary location
		if ( MissionSettings.Opt_TranslateEWData )
		{
			m_MissionSecondaryLoc = EventsWorldData.TransLoc( m_MissionSecondaryLoc );
		}
				
		//Set messages for secondary mission,
		m_MissionMessage1 = "Allright, you have found the pigs. Herd them all out of the shed. Protect them from infected attacks and kill at least 2 pigs to get some meat.";
		m_MissionMessage2 = "A man from the survivor camp promised me to help. Today he will go the picnic table on the hiking trail where I left a hunting bag. Bring the meat there.";
		m_MissionMessage3 = "The picnic table is near\n** "+ m_MissionSecondaryLoc +" **\nPut at least "+ ReqMeatAmount +" pig steaks in there and then take what you want. Be careful!";
		//Add GPS coordinates of mission in message
		if ( MissionSettings.Opt_ShowGpsInfo ) m_MissionMessage3 += "\n\nGPS coordinates:"+ m_MissionInstance.GetGPS();
						
		//Search for mission building at old mission position	
		g_Game.GetObjectsAtPosition( m_MissionPositions.Get( m_selectedMission ) , 1.0 , m_ObjectList , m_ObjectCargoList );
		for ( int i = 0 ; i < m_ObjectList.Count(); i++ )
		{ 
			Object FoundObject = m_ObjectList.Get(i);
			if ( FoundObject.GetType() == "Land_Farm_CowshedA")
			{	
				Object Farm = FoundObject;			 
				//Spawn some infected at farm
				for ( int j = 0 ; j < PriInfectSpawns.Count() ; j++ )
				{
		    	   	string RandomInfected = InfectedTypes.GetRandomElement();
					vector InfectedPos = Farm.ModelToWorld( PriInfectSpawns.Get(j) );
					DayZInfected Zed = DayZInfected.Cast( g_Game.CreateObjectEx( RandomInfected, InfectedPos, ECE_PLACE_ON_SURFACE | ECE_INITAI ));
					m_MissionAIs.Insert( Zed );
				}
				
				break;
			}	
		}
		
		Print("[SMM] FreePigs Mission : "+ m_selectedMission +" :: "+ m_MissionName +" ...mission extended!");								
	}
	
	override void MissionFinal()
	{	//When player enters last mission target zone
		//do nothing		
	}
	
	override void PlayerChecks( PlayerBase player )
	{	//Executed continuously when mission was actived. Avoid complex calculations or big data processing!
				
		//Check if container has desired amount of steaks collected at primary mission position
		if ( !m_MissionExtended && MissionObject ) 
		{
			CargoCount = MissionObject.GetInventory().CountInventory();
			
			if ( CargoCount != LastCount )
			{				
				if ( CargoCount >= ReqMeatAmount )
				{	
					CargoBase CargoItems1 = MissionObject.GetInventory().GetCargo();		
					
					for ( int i = 0; i < CargoCount; i++ )
					{
						EntityAI CargoItem = CargoItems1.GetItem(i);
						if ( CargoItem && m_MissionObjects.Find( CargoItem ) > -1 && CargoItem.GetType() == "PigSteakMeat" ) FoundObjects++;
						else continue;
												
						//When requested amount of mushrooms is present, despawn MissionObject & spawn rewards
						if ( FoundObjects >= ReqMeatAmount )
						{
							Object FoundObject;
							
							//Log mission accomplisher 
							g_Game.GetObjectsAtPosition3D( MissionObject.GetPosition(), 2, m_ObjectList, m_ObjectCargoList );
							for ( int n = 0; n < m_ObjectList.Count(); n++)
							{
								FoundObject = m_ObjectList.Get(n);
								if ( FoundObject.IsMan() )
								{
									player = PlayerBase.Cast( FoundObject );
									Print("[SMM] FreePigs Mission : Player with SteamID64: "+ player.GetIdentity().GetPlainId() +" has successfully stored the requested amount of "+ ReqMeatAmount +" MissionObjects in the container.");
									break;
								}
							}
							
							//Log assisting players
							PlayerBase MissionPlayer;
							
							g_Game.GetObjectsAtPosition3D( m_MissionPosition, 7, m_ObjectList, m_ObjectCargoList );
							
							for ( int k=0; k < m_ObjectList.Count(); k++)
							{
								FoundObject = m_ObjectList.Get(k);
								
								if ( FoundObject.IsMan() )
								{
									Class.CastTo( MissionPlayer, FoundObject );
									
									if ( MissionPlayer )
									{
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
								Print("[SMM] FreePigs Mission : Player with SteamID64: "+ IdentPlainID +" deserves respect for his "+ ExtMsg +".");
								//send Respect message to player
								NotificationSystem.SendNotificationToPlayerIdentityExtended( Ident , 15.0, RespectMsg, "", MsgIcon );
							}
							
							//delete container first
							g_Game.ObjectDelete( MissionObject );
							//call rewards spawn one second later 
							g_Game.GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( this.SpawnRewards, 1000 );
																				
							//finish mission
							m_RewardsSpawned = true;
							m_MsgNum = -1;
							m_MsgChkTime = m_MissionTime + MsgDlyFinish;
							break;
						} 
					}
					
					FoundObjects = 0;
				}
				
				LastCount = CargoCount;
			} 
		}		
	}
		
	override void EntityChecks()
	{	//Executed continuously while mission is active and triggered. Avoid complex calculations or big data processing!
		
		//Check if pigs were skinned after mission was extended and add pig steaks to MissionObjects
		if ( !m_MissionExtended )
		{
			if ( m_MissionTime >= EntChkTime )
			{
				for ( int j=0; j < m_MissionAIs.Count(); j++)
				{
					Object AI = m_MissionAIs.Get(j);
					
					if ( AI && DeadPigs.Find( AI ) == -1 && AI.GetType() == "Animal_SusDomesticus" && !AI.IsAlive() )
					{
						DeadPigs.Insert( AI );
						DeadPigsPos.Insert( AI.GetPosition() );
					}
				}
				
				for ( int m=0; m < DeadPigs.Count(); m++)
				{
					Object DeadPig = DeadPigs.Get(m);
					
					if ( !DeadPig )
					{
						g_Game.GetObjectsAtPosition( DeadPigsPos.Get(m), 3.0, m_ObjectList, m_ObjectCargoList);
						
						for ( int n=0; n < m_ObjectList.Count(); n++)
						{
							Object FoundObject = m_ObjectList.Get(n);
							
							if ( m_MissionObjects.Find( FoundObject ) == -1 && FoundObject.GetType() == "PigSteakMeat")
							m_MissionObjects.Insert( FoundObject );
						}
					}
				}
				
				EntChkTime = m_MissionTime + 1;
			}
		} 
	}
	
	override bool DeployMission()
	{	//When first player enters the mission zone (primary/secondary)
		//Get MissionBuilding at secondary mission position
		if ( !m_MissionExtended )
		{			
			g_Game.GetObjectsAtPosition( m_MissionPosition , 1.0 , m_ObjectList , m_ObjectCargoList );
			for ( int i=0; i < m_ObjectList.Count(); i++ )
			{ 
				Object FoundObject = m_ObjectList.Get(i);
				if ( FoundObject.GetType() == "Land_Misc_TrailRoof_Small" )
				{			 
					MissionBuilding = FoundObject;
					Print("[SMM] FreePigs Mission : Secondary mission zone triggered. MissionBuilding extended is "+ SecondaryLoc +" picnic table @ "+ MissionBuilding.GetPosition() );
					
					//new MissionPosition is rewards spawnpoint
					m_MissionPosition = MissionBuilding.ModelToWorld( RewardsPosition );
					break;
				}	
			}
		}
		
		if ( MissionBuilding )
		{
			//Call spawners	
			g_Game.GetCallQueue( CALL_CATEGORY_GAMEPLAY ).Call( this.SpawnObjects );
			g_Game.GetCallQueue( CALL_CATEGORY_GAMEPLAY ).Call( this.SpawnAIs );
			return true;		
		}
		else 
		{
			Print("[SMM] FreePigs Mission : ERROR :: MissionBuilding can't be found!");
			if ( MissionSettings.DebugMode ) Print("[SMM] FreePigs Mission : MissionBuilding is NULL!");
			return false;
		}
	}
}
