class CampMission extends SurvivorMissions		//by Xellose and Funkdoc
{
	//Mission related entities 
	TentBase MissionObject;
	FireplaceBase MissionCampfire;
	Object cc_fireplace;
	Object cc_mtent;
	Grenade_Base BoobyTrap; 
	
	//Mission containers
	ref array<vector> InfectedSpawns = new array<vector>;
	ref array<string> InfectedTypes = new array<string>;
	ref array<PlayerBase> AssistingPlayers = new array<PlayerBase>;
	
	//Mission variables 
	string SurvivorName;		
	
	override bool IsExtended() { return false; };
	
	void CampMission()
	{
		//Mission mission timeout
		m_MissionTimeout = 2700;			//seconds, mission duration time

		//Mission zones
		m_MissionZoneOuterRadius = 170.0;	//meters (!Do not set lower than 200m), mission activation distance
		m_MissionZoneInnerRadius = 3.0;		//meters (!Do not set outside of 1-5m), mission finishing distance to target object
				
		//Mission informant
		m_MissionInformant = "Dr. Legasov";
	
		//Mission person names
		TStringArray SurvivorNames = {"Yuri", "Michail", "Boris", "Valeri", "Anatoli", "Ivan", "Alexej", "Dimitrij", "Sergej", "Nikolai"};
		SurvivorName = SurvivorNames.GetRandomElement();
		
		//Translate mission location name and direction
		if ( MissionSettings.Opt_TranslateEWData )
		{
			m_MissionLocation = EventsWorldData.TransLoc( m_MissionLocation );
			m_MissionLocationDir = EventsWorldData.TransDir( m_MissionLocationDir );
		}
				
		//Set mission messages
        m_MissionMessage1 = "My friend "+ SurvivorName +" hasn't responded on the radio for last 2 hours. He is an excellent outdoor survivor and im wondering if he's ok.";
        m_MissionMessage2 = "His daughter is infected and he looks after her sometimes. About 5 hours ago, he told me that he has found stuff in some houses of "+ m_MissionLocation +" and was attacked on the way back to his camp but wasn't injured.";
        m_MissionMessage3 = "I think he said that he recently pitched up his tent\n** "+ m_MissionLocationDir +" of "+ m_MissionLocation +" **\nI am very worried, I would really appreciate it if you could go look after him. Stay safe!";		
		//Add GPS coordinates of mission in message
		if ( MissionSettings.Opt_ShowGpsInfo ) m_MissionMessage3 += "\n\nGPS coordinates:"+ m_MissionInstance.GetGPS();
						
		//Infected spawnpoints
		InfectedSpawns.Insert("-10.5186 0 25.0269");
		InfectedSpawns.Insert("24.9775 0 -10.4146");
		InfectedSpawns.Insert("-30.1726 0 -6.2729");
		InfectedSpawns.Insert("-20.9209 0 4.6636");
		InfectedSpawns.Insert("15.0283 0 -10.3276");
		InfectedSpawns.Insert("7.2461 0 30.5884");
		InfectedSpawns.Insert("-20.6855 0 5.9956");
		
		//Infected types
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
	}
	
	void ~CampMission()
	{
		//Despawn all remaining mission objects
		if ( m_MissionObjects )
		{	
			Print("[SMM] Camp Mission : Despawning "+ m_MissionObjects.Count() +" mission objects from old mission...");				
			for ( int i = 0; i < m_MissionObjects.Count(); i++ )
			{
				if ( !m_MissionObjects.Get(i) ) continue;
				else
				{
					//Delete Object clientside first
					if ( m_MissionObjects.Get(i).GetType() == "ClutterCutterFireplace" )
					g_Game.RemoteObjectDelete( m_MissionObjects.Get(i) );				
					
					//Delete Object serverside
					g_Game.ObjectDelete( m_MissionObjects.Get(i) );
				}		
			}
			m_MissionObjects.Clear();
		}
		
		//Despawn mission AI's
		if ( m_MissionAIs )
		{
			if ( m_MissionAIs.Count() > 0 )
			{
				Print("[SMM] Camp Mission : Despawning "+ m_MissionAIs.Count() +" mission AI's from old mission...");
				for ( int k = 0; k < m_MissionAIs.Count(); k++ )
				{
					g_Game.ObjectDelete( m_MissionAIs.Get(k) );
				}
				m_MissionAIs.Clear();			
			}
			else Print("[SMM] Camp Mission : No mission AI's to despawn.");
		}
		
		//Delete PlayersInZone list
		if ( m_PlayersInZone )	m_PlayersInZone.Clear();
	}
	
	void SpawnObjects()
	{				
		//Clean up place from droped items before new tent spawns 
		bool ItemsAtGround = false; 
		
		g_Game.GetObjectsAtPosition( m_MissionPosition , 4.0 , m_ObjectList , m_ObjectCargoList );
		for ( int i = 0 ; i < m_ObjectList.Count(); i++ )
		{ 
			Object FoundObject = m_ObjectList.Get(i);
			if ( FoundObject.IsItemBase() )
			{
				ItemsAtGround = true;
				Print("[SMM] Camp Mission : BeforeSpawnCleanUp :: Object  " + FoundObject.ToString() + " at new mission position was deleted.");				
				g_Game.ObjectDelete( FoundObject );
			}
		}
		//...and in case move tent pos a bit
		if ( ItemsAtGround )
		{
			float x = Math.RandomFloatInclusive( 1.5 , 3.0 );
			float y = Math.RandomFloatInclusive( 1.5 , 3.0 );
			int Dice = Math.RandomIntInclusive( 0, 9);
			if ( Dice > 4 ) x *= -1.0;
			Dice = Math.RandomIntInclusive( 0, 9);
			if ( Dice < 5 ) y *= -1.0;
			vector NewPosVector = { x, 0, y };
			m_MissionPosition += NewPosVector;
			Print("[SMM] Camp Mission : BeforeSpawnCleanUp :: Items on the ground detected. Tent position predictively was moved a bit.");
		}
		
		//Spawn & pitch tent		
		MissionObject = TentBase.Cast( g_Game.CreateObjectEx( "MissionTent", m_MissionPosition, ECE_PLACE_ON_SURFACE ) );
		MissionObject.Pitch( true , true );
		MissionObject.SetTakeable(false);
		
		//Get random loadout 
		int selectedLoadout = Math.RandomIntInclusive(0,9);	//!change randomization limit after adding new loadouts!	
		
		//Spawn selected loadout items in mission object
		EntityAI weapon;
				
		if (selectedLoadout == 0)
		{
			weapon = MissionObject.GetInventory().CreateInInventory("M4A1");
				weapon.GetInventory().CreateAttachment("M4_RISHndgrd");
				weapon.GetInventory().CreateAttachment("M4_MPBttstck");
				weapon.GetInventory().CreateAttachment("ACOGOptic");
				weapon.GetInventory().CreateAttachment("M4_Suppressor");
			MissionObject.GetInventory().CreateInInventory("Mag_STANAG_30Rnd");
			MissionObject.GetInventory().CreateInInventory("Mag_STANAG_30Rnd");
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
				weapon.GetInventory().CreateAttachment("AK_RailHndgrd");
				weapon.GetInventory().CreateAttachment("AK_PlasticBttstck");
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
				
		Print("[SMM] Camp Mission : Mission rewards spawned in reward container. Randomly selected loadout was "+selectedLoadout+"." );
		
		//Insert mission tent into mission objects list
		m_MissionObjects.Insert( MissionObject );
	
		//Clutter Cutter Tent 	
		cc_mtent = g_Game.CreateObjectEx( "MediumTentClutterCutter" , MissionObject.GetPosition() , ECE_PLACE_ON_SURFACE );
		cc_mtent.SetOrientation( MissionObject.GetOrientation() );
		g_Game.RemoteObjectCreate( cc_mtent );
		m_MissionObjects.Insert( cc_mtent );
		
		//Fireplace
		MissionCampfire = FireplaceBase.Cast( g_Game.CreateObjectEx( "Fireplace", MissionObject.ModelToWorld("0 0 -4"), ECE_PLACE_ON_SURFACE ) );
		MissionCampfire.GetInventory().CreateAttachment("Firewood");
		MissionCampfire.GetInventory().CreateAttachment("WoodenStick");
		MissionCampfire.GetInventory().CreateAttachment("Rag");
		MissionCampfire.StartFire( true );
		m_MissionObjects.Insert( MissionCampfire );
		
		//Cluttercutter fireplace
		cc_fireplace = g_Game.CreateObjectEx( "ClutterCutterFireplace" , MissionCampfire.GetPosition(), ECE_PLACE_ON_SURFACE );
		cc_fireplace.SetOrientation( MissionCampfire.GetOrientation() );
		g_Game.RemoteObjectCreate( cc_fireplace );
		m_MissionObjects.Insert( cc_fireplace );
							
		Print("[SMM] Camp Mission : Survivor Mission "+ m_selectedMission +" :: "+ m_MissionName +" ...mission deployed!");
	}
	
	void SpawnAIs()
	{	
		//Spawn dead infected
		vector InfectedPos = MissionObject.ModelToWorld( "0 0 -6" );
		DayZInfected InfectedSurvivor = DayZInfected.Cast( g_Game.CreateObjectEx( "ZmbM_HikerSkinny_Blue" , InfectedPos , ECE_PLACE_ON_SURFACE | ECE_INITAI ) );
		InfectedSurvivor.GetInventory().CreateAttachment("FlatCap_Blue");
		InfectedSurvivor.GetInventory().CreateAttachment("UKAssVest_Black");
		InfectedSurvivor.SetHealth("","",0);	
		m_MissionAIs.Insert( InfectedSurvivor ); 
		
		//spawn survivors bag, additional reward with basic items to survive		
		EntityAI bag = EntityAI.Cast( g_Game.CreateObjectEx("MountainBag_Blue", MissionObject.ModelToWorld("0 0 -5.5"), ECE_PLACE_ON_SURFACE ) );
		bag.GetInventory().CreateInInventory("WeaponCleaningKit");
		bag.GetInventory().CreateInInventory("PeachesCan");
		bag.GetInventory().CreateInInventory("SpaghettiCan");
		bag.GetInventory().CreateInInventory("Canteen");
		bag.GetInventory().CreateInInventory("CanOpener");
		bag.GetInventory().CreateInInventory("Battery9V");
		bag.GetInventory().CreateInInventory("Battery9V");
		bag.GetInventory().CreateInInventory("Battery9V");
		bag.GetInventory().CreateInInventory("Matchbox");

		//Spawn boobytraped infected
		string RandomInfected = InfectedTypes.GetRandomElement();
		vector InfectedPos1 = MissionObject.ModelToWorld( "0 0 5" );
		
		m_MissionAIs.Insert( g_Game.CreateObjectEx( RandomInfected, InfectedPos1, ECE_PLACE_ON_SURFACE | ECE_INITAI ) );
		BoobyTrap = Grenade_Base.Cast( g_Game.CreateObjectEx( "M67Grenade" , InfectedPos1, ECE_PLACE_ON_SURFACE ) );
		BoobyTrap.ActivateImmediate();
		
		
		//Spawn infected
		for ( int j = 0 ; j < InfectedSpawns.Count() ; j++ )
		{
    	   	RandomInfected = InfectedTypes.GetRandomElement();
			InfectedPos1 = MissionObject.ModelToWorld( InfectedSpawns.Get(j) );
			m_MissionAIs.Insert( g_Game.CreateObjectEx( RandomInfected, InfectedPos1, ECE_PLACE_ON_SURFACE | ECE_INITAI ) );
		}	
	}
	
	override void ObjDespawn() 
	{
		//Despawn all mission objects at mission timeout except those retains until next mission
		for ( int i = 0; i < m_MissionObjects.Count(); i++ )
		{
			//Exception: Fireplace & ClutterCutterFireplace will remain
			if ( m_MissionObjects.Get(i).GetType() == "Fireplace" || m_MissionObjects.Get(i).GetType() == "ClutterCutterFireplace" )	
			{
				Print("[SMM] Camp Mission : "+ m_MissionObjects.Get(i).GetType() +" was excluded from pre-deletion.");
				continue;
			}
			
			//Delete Object clientside first
			if ( m_MissionObjects.Get(i).GetType() == "MediumTentClutterCutter" ) g_Game.RemoteObjectDelete( m_MissionObjects.Get(i) );
			
			//Delete Object serverside
			g_Game.ObjectDelete( m_MissionObjects.Get(i) );
		}
	}
	
	override void MissionFinal()
	{	//When player enters last mission target zone
		
		//Log assisting players
		PlayerBase MissionPlayer;
		
		g_Game.GetObjectsAtPosition3D( m_MissionPosition, 7, m_ObjectList, m_ObjectCargoList );
		
		for ( int k=0; k < m_ObjectList.Count(); k++)
		{
			Object FoundObject = m_ObjectList.Get(k);
			
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
			Print("[SMM] Camp Mission : Player with SteamID64: "+ IdentPlainID +" deserves respect for his "+ ExtMsg +".");
			//send Respect message to player
			NotificationSystem.SendNotificationToPlayerIdentityExtended( Ident , 15.0, RespectMsg, "", MsgIcon );
		}

				
		//Respawn some infected 
		for ( int j = 0 ; j < 2 ; j++ )
		{
    	   	string RandomInfected = InfectedTypes.GetRandomElement();
			vector InfectedPos = MissionObject.ModelToWorld( InfectedSpawns.GetRandomElement() );
			m_MissionAIs.Insert( g_Game.CreateObjectEx( RandomInfected, InfectedPos, ECE_PLACE_ON_SURFACE | ECE_INITAI ) );			
		}

		//Finish mission
		m_RewardsSpawned = true;
		m_MsgNum = -1;
		m_MsgChkTime = m_MissionTime;
		
		Object rewardTent = m_MissionObjects[0];
		m_MissionObjects.Remove(0);
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(g_Game.ObjectDelete, 300000, false, rewardTent);
	}
	
	override void PlayerChecks( PlayerBase player )
	{	//Executed continuously while mission is active and triggered. Avoid complex calculations or big data processing! 
		//do nothing		
	}

	override void EntityChecks()
	{	//Executed continuously when mission was actived. Avoid complex calculations or big data processing!
		//do nothing		
	}	
		
	override bool DeployMission()
	{	//When first player enters the mission zone (primary/secondary)
		if ( m_MissionPosition )
		{
			//Call spawners	
			g_Game.GetCallQueue( CALL_CATEGORY_GAMEPLAY ).Call( this.SpawnObjects );
			g_Game.GetCallQueue( CALL_CATEGORY_GAMEPLAY ).Call( this.SpawnAIs );
			return true;		
		}
		else 
		{
			Print("[SMM] Camp Mission : ERROR :: Mission position was rejected or doesn't exist. MissionPosition is NULL!");
			return false;
		}
	}
}

