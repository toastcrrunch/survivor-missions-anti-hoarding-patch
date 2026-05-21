class BearHuntMission extends SurvivorMissions		//by Xellose and Funkdoc
{
	//Mission related entities
	ItemBase MissionObject;
	Object MissionBuilding;
	Object Bear;

	//Mission parameters
	int ReqMeatAmount = 4;					//pieces, requested mushroom amount
	int MsgDlyFinish = 60;					//seconds, message delay time after player has finished mission
	
	//Mission containers
	ref array<vector> ExtendedPosList = new array<vector>;
	ref array<vector> Spawnpoints = new array<vector>;
	ref array<vector> InfectedSpawns = new array<vector>;
	ref array<string> InfectedTypes = new array<string>;
	ref array<PlayerBase> AssistingPlayers = new array<PlayerBase>;
		
	//Mission variables 
	string SurvivorName, SecondaryLoc;
	bool BearSkinned = false;
	vector OldBearPos;
	int CargoCount;
	int LastCount = 0;
	int FoundMeatObjects = 0;
	bool FoundPeltObject = false;
	bool MapGridYnorthAscending;
	float XX, YY;
		
	override bool IsExtended() { return true; };
	
	void BearHuntMission()
	{	
		//Select primary mission
		m_MissionExtended = true;
				
		//Mission timeout
		m_MissionTimeout = 2700;			//seconds, mission max duration time
		m_ExtendedTimout = 1800;				//seconds, mission duration time for extended mission
		
		//Mission zones
		m_MissionZoneOuterRadius = 80.0;	//meters (!Do not set lower than 200m), mission activation distance
		m_MissionZoneInnerRadius = 3.0;		//meters (!Do not set outside of 1-5m), mission finishing distance to target object
				
		//Mission informant
		m_MissionInformant = "Dr. Legasov";
	
		//Mission person names
		TStringArray SurvivorNames = {"Yuri", "Michail", "Boris", "Valeri", "Anatoli", "Ivan", "Alexej", "Dimitrij", "Sergej", "Nikolai"};
		SurvivorName = SurvivorNames.GetRandomElement();
		
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
				
		//Set mission messages for primary mission
		m_MissionMessage1 = "A survivor recently reported to me that a bear attacked my friend "+ SurvivorName +" and killed 2 other survivors. They were at the wrong place trying to hunt some deer in the forest of "+ m_MissionLocation +".";
		m_MissionMessage2 = "I still can't believe it. "+ SurvivorName +" was one of the best hunters around here. Once a week he brought fresh meat to the hidden camp to feed the survivor's. He was a selfless person who tried to do everything to help out in the camp.";
		m_MissionMessage3 = "The bear was last seen at the forest near\n** "+ m_MissionLocationDir +" of "+ m_MissionLocation +" **\nI want you to kill this animal and take its pelt. It's not for revenge, we just need the pelt to make winter clothing for the kids. Be careful!";
		//Add GPS coordinates of mission in message
		if ( MissionSettings.Opt_ShowGpsInfo ) m_MissionMessage3 += "\n\nGPS coordinates:"+ m_MissionInstance.GetGPS();
				
		//Spawnpoint for MissionObject for secondary mission
		Spawnpoints.Insert("-18.40 2.19 18.57");
				
		//Infected spawnpoints for secondary mission
		InfectedSpawns.Insert("-7.46 -4.94 -12.99");
		InfectedSpawns.Insert("-15.65 -4.93 -22.08");
		InfectedSpawns.Insert("-22.5 -4.93 1.95");
		InfectedSpawns.Insert("-21.93 -4.93 13.0");
		InfectedSpawns.Insert("-17.95 -4.93 21.84");
		InfectedSpawns.Insert("-6.77 -4.89 21.28");
		InfectedSpawns.Insert("-21.14 -1.83 -5.83");
		InfectedSpawns.Insert("-10.5 1.27 -22.22");
		//...outside MissionBuilding 
		InfectedSpawns.Insert("0.23 -5.52 3.55");
		InfectedSpawns.Insert("19.78 -5.52 -5.66");
		
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
	
	void ~BearHuntMission()
	{	
		//Despawn all remaining objects
		if ( m_MissionObjects )
		{	
			Print("[SMM] BearHunt Mission : Despawning "+ m_MissionObjects.Count() +" mission objects from old mission...");		
			for ( int i = 0; i < m_MissionObjects.Count(); i++ )
			{
				if ( !m_MissionObjects.Get(i) ) continue;
				else g_Game.ObjectDelete( m_MissionObjects.Get(i) );		
			}
			m_MissionObjects.Clear();
		}
		
		//Delete mission AI's
		if ( m_MissionAIs )
		{
			if ( m_MissionAIs.Count() > 0 )
			{
				Print("[SMM] BearHunt Mission : Despawning "+ m_MissionAIs.Count() +" mission AI's from old mission...");
				for ( int k = 0; k < m_MissionAIs.Count(); k++ )
				{
					g_Game.ObjectDelete( m_MissionAIs.Get(k) );
				}
				m_MissionAIs.Clear();			
			}
			else Print("[SMM] BearHunt Mission : No mission AI's to despawn.");
		}
		
		//Delete PlayersInZone list
		if ( m_PlayersInZone )	m_PlayersInZone.Clear();
	}
	
	void SpawnContainer()
	{
		MissionObject =  ItemBase.Cast( g_Game.CreateObjectEx( "MissionBagBig", m_MissionPosition, ECE_PLACE_ON_SURFACE ));
		MissionObject.SetTakeable(false);
		m_MissionObjects.InsertAt( MissionObject, 0); 		
	}
	
	void SpawnRewards()
	{
		//new MissionObject after deleting protector case
		MissionObject = ItemBase.Cast( g_Game.CreateObjectEx( "MissionChest", m_MissionPosition, ECE_PLACE_ON_SURFACE ));
		MissionObject.SetTakeable(false);
		
		//Get random loadout 
		int selectedLoadout = Math.RandomIntInclusive( 0, 9);	//!change randomization limit after adding new loadouts!	

		//Spawn selected loadout items in mission object
		EntityAI weapon;
				
		if ( selectedLoadout == 0 )
		{
			weapon = MissionObject.GetInventory().CreateInInventory("M4A1");
				weapon.GetInventory().CreateAttachment("M4_RISHndgrd");
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
			MissionObject.GetInventory().CreateInInventory("AK_Suppressor");
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
						
		Print("[SMM] BearHunt Mission : Mission rewards spawned in reward container. Randomly selected loadout was "+ selectedLoadout +"." );
	}
	
	void SpawnObjects()
	{			
		if ( m_MissionExtended )
		{	
			//Spawn no objects at primary mission
		}
		else
		{				
			if ( MissionBuilding )
			{
				//Cleanup position before spawning protector case
				g_Game.GetObjectsAtPosition( m_MissionPosition, 0.5, m_ObjectList, m_ObjectCargoList );
				for ( int k = 0; k < m_ObjectList.Count(); k++ )
				{
					Object FoundObject = m_ObjectList.Get(k);
					if ( FoundObject.IsItemBase() )
					g_Game.ObjectDelete( FoundObject );
				}
				
				//Spawn container
				g_Game.GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( this.SpawnContainer, 1000 );
			}
			else Print("[SMM] BearHunt Mission : Extended MissionBuilding couldn't be found in "+ SecondaryLoc +" ... Mission rejected!");
		}
	}
	
	void SpawnAIs()
	{	
		if ( m_MissionExtended )
		{
			//Spawn Bear at primary mission
			m_MissionAIs.Insert( g_Game.CreateObjectEx("Animal_UrsusArctos", m_MissionPosition, ECE_PLACE_ON_SURFACE | ECE_INITAI )); 
			
			Print("[SMM] BearHunt Mission : Survivor Mission "+ m_selectedMission +" :: "+ m_MissionName +" ...mission deployed!");
		}
		else
		{
			if ( MissionBuilding )
			{			
				//Spawn some infected at school 
				for ( int j = 0 ; j < InfectedSpawns.Count() ; j++ )
				{
		    	   	string RandomInfected = InfectedTypes.GetRandomElement();
					vector InfectedPos = MissionBuilding.ModelToWorld( InfectedSpawns.Get(j) );
					DayZInfected Zed = DayZInfected.Cast( g_Game.CreateObjectEx( RandomInfected, InfectedPos, ECE_PLACE_ON_SURFACE | ECE_INITAI ));
					m_MissionAIs.Insert( Zed );
				}			
			}
		}
	}
	
	override void ObjDespawn() 
	{	
		//Despawn nothing
	}
	
	override void ExtendMission()
	{
		//init Messenger for new messages
		m_MsgNum = 1;					//skip msg 0, begin with msg 1
		m_MsgChkTime = m_MissionTime;
		MsgCutoff = false;
		
		//increase mission MissionTimeout
		m_MissionTimeout = m_MissionTime + m_ExtendedTimout;  
		
		//deployment settings & init for extended mission
		m_MissionZoneOuterRadius = 120.0;
		m_MissionZoneInnerRadius = 2.0;
		
		//Get building position from building type and location
		if ( EventsWorldData.GetBuildingsAtLoc("Land_City_School", SecondaryLoc, ExtendedPosList ))
		{
			//Get MissionPosition for MissionBuilding from secondary mission
			m_MissionPosition = ExtendedPosList.GetRandomElement();						
			g_Game.GetObjectsAtPosition( m_MissionPosition, 1.0 , m_ObjectList , m_ObjectCargoList );
			for ( int m=0; m < m_ObjectList.Count(); m++ )
			{ 
				Object FoundObject = m_ObjectList.Get(m);
				if ( FoundObject.GetType() == "Land_City_School" )
				{	
					MissionBuilding = FoundObject;
					Print("[SMM] BearHunt Mission : New Mission Position is Land_City_School at "+ SecondaryLoc +" @"+ FoundObject.GetPosition() ); 
					SecondaryLoc.Replace("_"," ");
					
					//new MissionPosition is rewards spawnpoint
					m_MissionPosition = MissionBuilding.ModelToWorld( Spawnpoints.Get(0) );
					break;
				}	
			}
		}
		else Print("[SMM] BearHunt Mission : Can't find MissionBuilding City School in "+ SecondaryLoc +" from EventsWorldData!");
		
		//translate secondary location
		if ( MissionSettings.Opt_TranslateEWData )
		{
			m_MissionSecondaryLoc = EventsWorldData.TransLoc( m_MissionSecondaryLoc );
		}
			
		//Set new mission messages
		if ( !Bear.IsAlive() )
		m_MissionMessage1 = "Allright survivor, I already heard shots from your direction. It seems you got the bear. First ensure that you are safe, be aware of ohter bears or infected attracted by your shots.";
		else
		{
			//translate SecondarySpawn coords		
		    XX = m_MissionPosition[0] / 10000;
		    if ( !MapGridYnorthAscending )
		    YY = ( EventsWorldData.EWD_MAP_SIZE - m_MissionPosition[2] ) / 10000;
			else
			YY = m_MissionPosition[2] / 10000;
				
			string Coords = " X "+ ( XX.ToString().Substring(0,4) ) +" / Y "+ ( YY.ToString().Substring(0,4) ) +" ";
			
			m_MissionMessage1 = "The bear is still running around at\nGPS coordinates:** "+ Coords + " **\nKill the bear before he runs away!";
		}
		m_MissionMessage2 = "Take out your knife and eviscerate him. Bring the pelt and the meat to the\n** "+ m_MissionSecondaryLoc +" School laboratory class room **\n(left wing, top floor) because I want to examine the bear meat. I left a black bag on the labor desk.";
		m_MissionMessage3 = "Put at least "+ ReqMeatAmount +" bear steaks and the pelt in there.\nBe careful, bandits could have intercepted our little radio talk here. Good luck!";
		//Add GPS coordinates of mission in message
		if ( MissionSettings.Opt_ShowGpsInfo ) m_MissionMessage3 += "\n\nGPS coordinates:"+ m_MissionInstance.GetGPS();
		
		Print("[SMM] BearHunt Mission : "+ m_selectedMission +" :: "+ m_MissionName +" ...mission extended!");
	}
	
	override void MissionFinal()
	{	//When player enters last mission target zone
		
		//Spawn infected
		string RandomInfected;
		vector InfectedPos;
		
		for ( int k = 0 ; k < InfectedSpawns.Count() ; k++ )
		{
    	   	RandomInfected = InfectedTypes.GetRandomElement();
			InfectedPos = MissionBuilding.ModelToWorld( InfectedSpawns.Get(k) );
			m_MissionAIs.Insert( g_Game.CreateObjectEx( RandomInfected, InfectedPos, ECE_PLACE_ON_SURFACE | ECE_INITAI ));
		}				
	}
	
	override void PlayerChecks( PlayerBase player )
	{	//Executed continuously on target player while he is inside mission zone. Avoid complex calculations or big data processing!
			
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
						if ( CargoItem && m_MissionObjects.Find( CargoItem ) > -1 && CargoItem.GetType() == "BearSteakMeat" ) FoundMeatObjects++;
						else if ( CargoItem && m_MissionObjects.Find( CargoItem ) > -1 && CargoItem.GetType() == "BearPelt" ) FoundPeltObject = true;
						else continue;					
						
						//When requested amount of mushrooms is present, despawn MissionObject & spawn rewards
						if ( FoundMeatObjects >= ReqMeatAmount && FoundPeltObject )
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
									Print("[SMM] BearHunt Mission : Player with SteamID64: "+ player.GetIdentity().GetPlainId() +" has successfully stored the requested amount of "+ ReqMeatAmount +" MissionObjects in the container.");
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
								Print("[SMM] BearHunt Mission : Player with SteamID64: "+ IdentPlainID +" deserves respect for his "+ ExtMsg +".");
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
					
					FoundMeatObjects = 0;
					FoundPeltObject = false;
				}
				
				LastCount = CargoCount;
			} 
		}		
	}

	override void EntityChecks()
	{	//Executed continuously when mission was actived. Avoid complex calculations or big data processing!
		
		if ( !m_MissionExtended )
		{
			//Check if bear was skinned and add bear parts to MissionObjects for secondary's requested items check
			if ( Bear )
			OldBearPos = Bear.GetPosition();
			else
			{
				if ( !BearSkinned )
				{
					g_Game.GetObjectsAtPosition( OldBearPos, 5.0, m_ObjectList, m_ObjectCargoList );
					
					for (int i=0; i < m_ObjectList.Count(); i++)
					{
						Object FoundObject = m_ObjectList.Get(i);
						if ( FoundObject.GetType() == "BearPelt" || FoundObject.GetType() == "BearSteakMeat" )
						m_MissionObjects.Insert( FoundObject );
					}
					
					BearSkinned = true;
				}
			}
		} 
		else
		{
			//Update Bear position at primary
			Bear = Object.Cast( m_MissionAIs[0] );
			if ( Bear && Bear.GetType() == "Animal_UrsusArctos" )
			m_MissionPosition = Bear.GetPosition();
		}		 
	}	
		
	override bool DeployMission()
	{	//When first player enters the mission zone (primary/secondary)
		
		//Get MissionBuilding for secondary mission
		if ( !m_MissionExtended && !MissionBuilding )
		{
			if ( EventsWorldData.GetBuildingsAtLoc("Land_City_School", SecondaryLoc, ExtendedPosList ))
			{		
				//Get MissionPosition from MissionBuilding
				m_MissionPosition = ExtendedPosList.GetRandomElement();						
				g_Game.GetObjectsAtPosition( m_MissionPosition, 1.0 , m_ObjectList , m_ObjectCargoList );
				for ( int m=0; m < m_ObjectList.Count(); m++ )
				{ 
					Object FoundObject = m_ObjectList.Get(m);
					if ( FoundObject.GetType() == "Land_City_School" )
					{	
						MissionBuilding = FoundObject;			 
						Print("[SMM] BearHunt Mission : Secondary mission zone triggered. MissionBuilding extended is Land_City_School at "+ SecondaryLoc +" @ "+ MissionBuilding.GetPosition() );
											
						//new MissionPosition is rewards spawnpoint
						m_MissionPosition = MissionBuilding.ModelToWorld( Spawnpoints.Get(0) );
						break;
					}	
				}
			}
			else Print("[SMM] BearHunt Mission : Can't find MissionBuilding Land_City_School in "+ SecondaryLoc +"!");
		}
							
		if ( (m_MissionPosition && m_MissionExtended) || (MissionBuilding && !m_MissionExtended) )
		{
			//Call spawners	
			g_Game.GetCallQueue( CALL_CATEGORY_GAMEPLAY ).Call( this.SpawnObjects );
			g_Game.GetCallQueue( CALL_CATEGORY_GAMEPLAY ).Call( this.SpawnAIs );
			return true;		
		}
		else 
		{
			Print("[SMM] BearHunt Mission : Mission position was rejected or MissionBuilding doesn't exist!");
			if ( MissionSettings.DebugMode && !MissionBuilding && !m_MissionExtended ) Print("[SMM] BearHunt Mission : MissionBuilding is NULL!");
			return false;
		}
	}
}

