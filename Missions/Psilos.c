class PsilosMission extends SurvivorMissions		//by Xellose and Funkdoc
{	//Psilos mission is a copy of Shrooms mission. It has a different secondary MissionBuilding which is the small PoliceStation to make it working on maps like enoch
	//Mission related entities
	ItemBase MissionObject;
	Object MissionBuilding;

	//Mission parameters
	int ReqShroomsCount = 10;				//pieces, requested mushroom amount
	int MsgDlyFinish = 60;					//seconds, message delay time after player has finished mission
	
	//Mission containers
	ref array<vector> ExtendedPosList = new array<vector>;
	ref array<vector> Spawnpoints = new array<vector>;
	ref array<vector> InfectedSpawns = new array<vector>;
	ref array<string> InfectedTypes = new array<string>;
	ref array<PlayerBase> AssistingPlayers = new array<PlayerBase>;
		
	//Mission variables 
	string SurvivorName, SecondaryLoc;		
	string SurvivorExtName;
	int CargoCount;
	int LastCount = 0;
	int FoundObjects = 0;
	
	override bool IsExtended() { return true; };
	
	void PsilosMission()
	{	
		//Select primary mission
		m_MissionExtended = true;
				
		//Mission timeout
		m_MissionTimeout = 2700;			//seconds, mission max duration time
		m_ExtendedTimout = 1800;				//seconds, mission duration time for extended mission
		
		//Mission zones
		m_MissionZoneOuterRadius = 70.0;	//meters (!Do not set lower than 200m), mission activation distance
		m_MissionZoneInnerRadius = 30.0;	//meters (!Do not set outside of 1-5m), mission finishing distance to target object
				
		//Mission informant
		m_MissionInformant = "Dr. Legasov";
	
		//Mission person names
		TStringArray SurvivorNames = {"Yuri", "Michail", "Boris", "Valeri", "Anatoli", "Ivan", "Alexej", "Dimitrij", "Sergej", "Nikolai"};
		SurvivorName = SurvivorNames.GetRandomElement();
		TStringArray SurvivorExtNames = {"Dr. Tamarova", "Dr. Lorzinski", "Dr. Blosmanova", "Dr. Haidenau", "Dr. Gabarin", "Dr. Sloskova", "Dr. Belzin", "Dr. Homyuk", "Dr. Trademzyuk", "Dr. Jankarova"};
		SurvivorExtName = SurvivorExtNames.GetRandomElement();
		
		//Translate mission location name and direction
		if ( MissionSettings.Opt_TranslateEWData )
		{
			m_MissionLocation = EventsWorldData.TransLoc( m_MissionLocation );
			m_MissionLocationDir = EventsWorldData.TransDir( m_MissionLocationDir );
		}
				
		//Set mission messages for primary mission
		m_MissionMessage1 = SurvivorName +", a survivor I met a few days ago, told me that there are some places around here with numerous occurrencies of a special mushroom. It's classification name is 'Psilocybe semilanceata'.";
		m_MissionMessage2 = "At the moment I am experimenting with the psychoactive ingredient Psilocybin on infected wild animals. The animals show interesting behaviors after 30 minutes of the oral intake. I can't say much about it yet, but for my experiments I need more mushrooms.";
		m_MissionMessage3 = SurvivorName +" said that he found some of those anywhere\n** "+ m_MissionLocationDir +" of "+ m_MissionLocation +" **\nI need "+ ReqShroomsCount +" more pieces of it. The mushrooms are very small. I recommend using a mushroom encyclopedia for possible occurrencies!";
		//Add GPS coordinates of mission in message
		if ( MissionSettings.Opt_ShowGpsInfo ) m_MissionMessage3 += "\n\nGPS coordinates:"+ m_MissionInstance.GetGPS();
				
		//Spawnpoints for MissionObject for secondary mission
		Spawnpoints.Insert("-8.2598 -5.3138 -3.3914");
		Spawnpoints.Insert("-9.3223 -5.3234 1.1267");
		Spawnpoints.Insert("2.4033 -1.7478 -3.8455");
		Spawnpoints.Insert("-5.2329 -1.7252 -3.8408");
		Spawnpoints.Insert("-9.3237 -1.7233 0.9534");
		Spawnpoints.Insert("2.8477 -1.7252 2.5952");
		Spawnpoints.Insert("0.7725 -1.7252 2.4839");
				
		//Infected spawnpoints for secondary mission
		InfectedSpawns.Insert("-6.9097 -5.9196 -2.2151"); //policeman
		InfectedSpawns.Insert("-7.7876 -5.9899 1.9773"); //policeman
		InfectedSpawns.Insert("-7.8467 -2.4139 1.9091"); //policeman
		InfectedSpawns.Insert("0.7358 -2.4106 -2.3098"); //policeman
		InfectedSpawns.Insert("1.3223 -5.9844 -3.1926"); //prisoner
		InfectedSpawns.Insert("1.0986 -5.9846 -1.3384"); //prisoner
		//...outside MissionBuilding 
		InfectedSpawns.Insert("-8.12 0 -5.97");
		InfectedSpawns.Insert("-2.81 0 -5.8");
		InfectedSpawns.Insert("0.41 0 -5.8");
		InfectedSpawns.Insert("6.7 0 6.41");
		InfectedSpawns.Insert("6 0 -1.85");
		
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
		
		//check if height is correct related to surface (on ground), this is for quick correction if there were terrain changes on this map
		float Mheight = m_MissionPosition[1];
		float SfcY = g_Game.SurfaceY( m_MissionPosition[0], m_MissionPosition[2] );
		
		if ( SfcY - Mheight > 1.0 || SfcY - Mheight < -1.0 )
		{
			m_MissionPosition[1] = SfcY;
			Print("[SMM] Psilos Mission : WARNING :: MissionPosition height in EWD doesn't match the surface height ("+ SfcY +"m)! MissionPosition was set on surface.");
		}
	}
	
	void ~PsilosMission()
	{	
		//Despawn all remaining objects
		if ( m_MissionObjects )
		{	
			Print("[SMM] Psilos Mission : Despawning "+ m_MissionObjects.Count() +" mission objects from old mission...");		
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
				Print("[SMM] Psilos Mission : Despawning "+ m_MissionAIs.Count() +" mission AI's from old mission...");
				for ( int k = 0; k < m_MissionAIs.Count(); k++ )
				{
					g_Game.ObjectDelete( m_MissionAIs.Get(k) );
				}
				m_MissionAIs.Clear();			
			}
			else Print("[SMM] Psilos Mission : No mission AI's to despawn.");
		}
		
		//Delete PlayersInZone list
		if ( m_PlayersInZone )	m_PlayersInZone.Clear();
	}
	
	void SpawnContainer()
	{
		MissionObject =  ItemBase.Cast( g_Game.CreateObjectEx( "MissionCaseSmall", m_MissionPosition, ECE_PLACE_ON_SURFACE ));
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
					
		Print("[SMM] Psilos Mission : Mission rewards spawned in reward container. Randomly selected loadout was "+ selectedLoadout +"." );	
	}
	
	void SpawnObjects()
	{			
		if ( m_MissionExtended )
		{	
			//Spawn mushroom occurence		
			float OccurenceDist = 30 * 0.707 ;
			int MushroomsCount = Math.RandomIntInclusive( 50, 150);
			vector SpawnPos;
			bool ItemsAtGround = false; 
		 
			MissionObject = ItemBase.Cast( g_Game.CreateObjectEx( "PsilocybeMushroom", m_MissionPosition, ECE_PLACE_ON_SURFACE ));
			if ( MissionObject ) Print("[SMM] Psilos Mission : Mushrooms occurence spawned around @ "+ MissionObject.GetPosition());
			
			m_MissionObjects.Insert( MissionObject );
			for ( int j = 1; j < MushroomsCount; j++ )
			{
				//calc new spawn position
				float x = Math.RandomFloatInclusive( 0.5 , OccurenceDist );
				float y = Math.RandomFloatInclusive( 0.5 , OccurenceDist );	
				int Dice = Math.RandomIntInclusive( 0, 9);
				if ( Dice > 4 ) x *= -1.0;
				Dice = Math.RandomIntInclusive( 0, 9);
				if ( Dice < 5 ) y *= -1.0;
				vector NewPosVector = { x, 0, y };
				SpawnPos = m_MissionPosition + NewPosVector;
				
				//check for items at ground
				g_Game.GetObjectsAtPosition( SpawnPos , 1.0 , m_ObjectList , m_ObjectCargoList );
				for ( int i = 0; i < m_ObjectList.Count(); i++ )
				{ 
					Object FoundObject = m_ObjectList.Get(i);
					if ( FoundObject.IsItemBase() || FoundObject.IsTree() || FoundObject.IsRock() || FoundObject.IsBush() ) 
					{
						ItemsAtGround = true;
						break;
					}
				}
				if ( ItemsAtGround )
				{
					ItemsAtGround = false;
					j--;
					continue;
				} 
			
				//spawn mushroom
				m_MissionObjects.Insert( g_Game.CreateObjectEx( "PsilocybeMushroom", SpawnPos, ECE_PLACE_ON_SURFACE ));	
			}
				
			Print("[SMM] Psilos Mission : Survivor Mission "+ m_selectedMission +" :: "+ m_MissionName +" ...mission deployed!");
		}
		else
		{				
			if ( MissionBuilding )
			{
				//Cleanup position before spawning protector case
				g_Game.GetObjectsAtPosition( m_MissionPosition, 0.5, m_ObjectList, m_ObjectCargoList );
				for ( int k = 0; k < m_ObjectList.Count(); k++ )
				{
					FoundObject = m_ObjectList.Get(k);
					if ( FoundObject.IsItemBase() )
					g_Game.ObjectDelete( FoundObject );
				}
				
				//Spawn container
				g_Game.GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( this.SpawnContainer, 1000 );
								
				Print("[SMM] Psilos Mission : Survivor Mission Extension "+ m_selectedMission +" :: "+ m_MissionName +" ...mission extended!");
			}
			else Print("[SMM] Psilos Mission : Extended MissionBuilding couldn't be found in "+ SecondaryLoc +" ... Mission rejected!");
		}
	}
	
	void SpawnAIs()
	{	
		if ( m_MissionExtended )
		{
			//Spawn no AIs at primary mission
		}
		else
		{
			//Spawn infected
			string RandomInfected;
			vector InfectedPos;
			
			//Spawn some infected at Police Station
			for ( int k = 0 ; k < InfectedSpawns.Count() ; k++ )
			{
				if ( k == 0 )	RandomInfected = "ZmbM_PolicemanFat";
				else if ( k == 1 )	RandomInfected = "ZmbF_PoliceWomanNormal";
				else if ( k == 2 )	RandomInfected = "ZmbM_PolicemanSpecForce";
				else if ( k == 3 )	RandomInfected = "ZmbM_PolicemanFat";
	    	   	else RandomInfected = InfectedTypes.GetRandomElement();
				InfectedPos = MissionBuilding.ModelToWorld( InfectedSpawns.Get(k) );
				DayZInfected Zed = DayZInfected.Cast( g_Game.CreateObjectEx( RandomInfected, InfectedPos, ECE_PLACE_ON_SURFACE | ECE_INITAI ));
				m_MissionAIs.Insert( Zed );
			}
		}
	}
	
	override void ObjDespawn() 
	{	
		//Despawn all mission objects at mission timeout except those retains until next mission
		for ( int i = 0; i < m_MissionObjects.Count(); i++ )
		{
			//Exception: Psilo's will remain
			if ( m_MissionObjects.Get(i) && m_MissionObjects.Get(i).GetType() == "PsilocybeMushroom" )	continue;

			//Delete Object
			g_Game.ObjectDelete( m_MissionObjects.Get(i) );
			m_MissionObjects.Remove(i);
		}
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
		m_MissionZoneOuterRadius = 90.0;
		m_MissionZoneInnerRadius = 2.0;
		
		//Get building position from building type and location
		if ( EventsWorldData.GetBuildingsAtLoc("Land_Village_PoliceStation_Enoch", SecondaryLoc, ExtendedPosList ))
		{			
			//Get MissionPosition for MissionBuilding from secondary mission
			m_MissionPosition = ExtendedPosList.GetRandomElement();
			Print("[SMM] Psilos Mission : New Mission Position is Land_Village_PoliceStation_Enoch at "+ SecondaryLoc +" @ "+ m_MissionPosition );
			SecondaryLoc.Replace("_"," ");			
		}
		else Print("[SMM] Psilos Mission : Can't find MissionBuilding Land_Village_PoliceStation_Enoch in "+ SecondaryLoc +"!");
		
		//translate secondary location
		if ( MissionSettings.Opt_TranslateEWData )
		{
			m_MissionSecondaryLoc = EventsWorldData.TransLoc( m_MissionSecondaryLoc );
		}
		
		//Set new mission messages
		m_MissionMessage1 = "Allright survivor, if you have found "+ ReqShroomsCount +" pieces of these mushrooms bring them to the following place. Hold on a second...";
		m_MissionMessage2 = "Bring them to the ** "+ m_MissionSecondaryLoc +" Police Station **\n"+ SurvivorExtName +" said she could extract the active substance for me. She has left a small protector case on the desk in her former office.";
		m_MissionMessage3 = "Put all you have found in there. Be careful, there might be bandits around who could have intercepted our little radio talk here. Good luck!";
		//Add GPS coordinates of mission in message
		if ( MissionSettings.Opt_ShowGpsInfo ) m_MissionMessage3 += "\n\nGPS coordinates:"+ m_MissionInstance.GetGPS();			
	}
	
	override void MissionFinal()
	{	//When player enters last mission target zone
		//do nothing			
	}
	
	override void PlayerChecks( PlayerBase player )
	{	//Executed continuously on target player while he is inside mission zone. Avoid complex calculations or big data processing!
				
		//Check if container has desired amount of mushrooms collected at primary mission position
		if ( !m_MissionExtended && MissionObject && MissionObject.ClassName() == "SmallProtectorCase" && player.GetInventory().HasEntityInInventory( MissionObject ) )
		{		
			CargoCount = MissionObject.GetInventory().CountInventory();
			
			if ( CargoCount != LastCount )
			{
				if ( CargoCount >= ReqShroomsCount )
				{	
					CargoBase CargoItems1 = MissionObject.GetInventory().GetCargo();		
					
					for ( int i = 0; i < CargoCount; i++ )
					{
						EntityAI CargoItem = CargoItems1.GetItem(i);
						if ( m_MissionObjects.Find( CargoItem ) > -1 ) FoundObjects++;
						else continue;					
						
						//When requested amount of mushrooms is present, despawn MissionObject & spawn rewards
						if ( FoundObjects >= ReqShroomsCount )
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
									Print("[SMM] Psilos Mission : Player with SteamID64: "+ player.GetIdentity().GetPlainId() +" has successfully stored the requested amount of "+ ReqShroomsCount +" MissionObjects in the container.");
									break;
								}
							}
							
							//Log assisting players
							PlayerBase MissionPlayer;
							
							g_Game.GetObjectsAtPosition3D( MissionObject.GetPosition(), 7, m_ObjectList, m_ObjectCargoList );
							
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
								Print("[SMM] Psilos Mission : Player with SteamID64: "+ IdentPlainID +" deserves respect for his "+ ExtMsg +".");
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
	{	//Executed continuously when mission was actived. Avoid complex calculations or big data processing!
		//do nothing 
	}
	
	override bool DeployMission()
	{	//When first player enters the mission zone (primary/secondary)
		//Get MissionBuilding at mission position
		if ( !m_MissionExtended && !MissionBuilding )
		{
			g_Game.GetObjectsAtPosition( m_MissionPosition , 1.0 , m_ObjectList , m_ObjectCargoList );
			for ( int m=0; m < m_ObjectList.Count(); m++ )
			{ 
				Object FoundObject = m_ObjectList.Get(m);
				if ( FoundObject.GetType() == "Land_Village_PoliceStation_Enoch")
				{	
					MissionBuilding = FoundObject;			 
					Print("[SMM] Psilos Mission : Secondary mission zone triggered. MissionBuilding extended is Land_Village_PoliceStation_Enoch at "+ SecondaryLoc +" @ "+ MissionBuilding.GetPosition() );
					
					//new MissionPosition is rewards spawnpoint
					m_MissionPosition = MissionBuilding.ModelToWorld( Spawnpoints.GetRandomElement() );
					break;
				}	
			}
		}	
			
		if ( (m_MissionPosition && m_MissionPosition != "0 0 0" && m_MissionExtended) || (MissionBuilding && !m_MissionExtended) )
		{
			//Call spawners	
			g_Game.GetCallQueue( CALL_CATEGORY_GAMEPLAY ).Call( this.SpawnObjects );
			g_Game.GetCallQueue( CALL_CATEGORY_GAMEPLAY ).Call( this.SpawnAIs );
			return true;		
		}
		else 
		{
			Print("[SMM] Psilos Mission : Mission position was rejected or MissionBuilding doesn't exist!");
			if ( MissionSettings.DebugMode && !MissionBuilding && !m_MissionExtended ) Print("[SMM] Psilos Mission : MissionBuilding is NULL!");
			return false;
		}
	}
}

