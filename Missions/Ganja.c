class GanjaMission extends SideMissions		//by Xellose and Funkdoc
{	//Ganja mission is a copy of Transport mission. It uses another type of the gas station building in some cases.
	
	//Mission related entities 
	Car MissionCar;
	Object MissionBuilding;
	ItemBase MissionObject;
	Object MissionLight;
	
	//Mission parameters
	int ReqWeedAmount = 25;					//pieces, mission ganja (equivalent to x * 4 gramms)
	int MsgDlyFinish = 60;					//seconds, message delay time after player has finished mission
	
	//Mission containers
	ref array<vector> ExtendedPosList = new array<vector>;
	ref array<vector> GarageCarSpawns = new array<vector>;
	ref array<vector> Spawnpoints = new array<vector>;
	ref array<vector> PriInfectSpawns = new array<vector>;
	ref array<vector> SecInfectSpawns = new array<vector>;
	ref array<string> InfectedTypes = new array<string>;
	ref array<PlayerBase> AssistingPlayers = new array<PlayerBase>;
		
	//Mission variables 
	string SurvivorName, SecondaryLoc;	
	vector CarPosition;
	bool CarIsHijacked = false;	
	int CargoCount;
	int LastCount = 0;
	int FoundObjects = 0;	
		
	override bool IsExtended() { return true; };
	
	void GanjaMission()
	{		
		//Select primary mission
		m_MissionExtended = true;
		
		//Mission timeout
		m_MissionTimeout = 2700;			//seconds, primary mission duration time
		m_ExtendedTimout = 1500;				//seconds, mission duration time for extended mission
		
		//Mission zones
		m_MissionZoneOuterRadius = 150.0;	//meters (!Do not set lower than 200m), mission activation distance
		m_MissionZoneInnerRadius = 2.0;		//meters (!Do not set outside of 1-5m), mission finishing distance to target object
				
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
		m_MissionMessage1 = SurvivorName +", a guy who was working for me, told me that he has cross-breeded a special Cannabis strain of various 'Cannabis Ruderalis' plants. I just need it for my experiments on the infected. He said that he fertilized the mother plant with shredded infected.";
		m_MissionMessage2 = "I think he meant some kind of fermented brew with water, infected ash and pieces of flesh done in a kitchen mixer. The impact on plants grow and bloom was massive. It also seems that it contains a substance other than the native Cannabinoids. I want to extract this substance to see how it acts on the infected.";
		m_MissionMessage3 = SurvivorName +" has hidden his herbs in a garage at\n** "+ m_MissionLocation +" **\nHe certainly has locked the garage doors, so try to use a lockpick or shoot them up if you have none.";
		//Add GPS coordinates of mission in message
		if ( MissionSettings.Opt_ShowGpsInfo ) m_MissionMessage3 += "\n\nGPS coordinates:"+ m_MissionInstance.GetGPS();
				
		//Spawnpoints for MissionCar in Garage 
		GarageCarSpawns.Insert("5.055 -1.504 -2.064"); 	//left garage
		GarageCarSpawns.Insert("-4.95 -1.504 -1.843");	//right garage
		
		//Spawnpoints for MissionObjects in middle garage (Do not change order!)
		Spawnpoints.Insert("1.575 -0.605 -5.431");  //on table right (3x motor oil), doesn't spawn anymore - motoroil has been removed from the gamefiles
		Spawnpoints.Insert("1.750 -0.605 -4.731");  //on table center (bag)
		Spawnpoints.Insert("1.800 -0.575 -3.8");	//on table left (weed pile)
		Spawnpoints.Insert("1.724 -1.504 -1.560");	//left beside table (gasoline canister)
		Spawnpoints.Insert("-1.85 -0.96 -2.196");	//shelf (spark plug)
		
		//Spawnpoint for reward container in Gas Station Building
		Spawnpoints.Insert("0.330 -1.538 0.6");		//beside desk
				
		//Infected spawnpoints for primary mission
		PriInfectSpawns.Insert("-10 0 20");
		PriInfectSpawns.Insert("-12 0 -30");
		PriInfectSpawns.Insert("20 0 -15");
		PriInfectSpawns.Insert("-30 0 -15");
		PriInfectSpawns.Insert("-16 0 -25");
		PriInfectSpawns.Insert("17 0 10");
		PriInfectSpawns.Insert("28 0 -30");
		
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
			if ( FoundObject.GetType() == "Land_Garage_Row_Small" )
			{	
				MissionBuilding = FoundObject;			 
				Print("[SMM] Ganja Mission : MissionBuilding is "+ MissionBuilding.GetType() +" at "+ m_MissionDescription[2] +" of "+ m_MissionDescription[1] +" @ "+ MissionBuilding.GetPosition() );
				break;
			}	
		}
		if ( !MissionBuilding ) Print("[SMM] Ganja Mission : Garage couldn't be found. Mission rejected!");
		else
		{
			//Close all garage doors
			Building Garage = Building.Cast( MissionBuilding );
			for ( int k=0; k < 3; k++ ) 
			{
				if ( Garage.IsDoorOpen(k) ) Garage.CloseDoor(k);
				if ( !Garage.IsDoorLocked(k) ) Garage.LockDoor(k); 
			}
			
			//Remove car wrecks in front of garage
			g_Game.GetObjectsAtPosition( m_MissionPosition , 15.0 , m_ObjectList , m_ObjectCargoList );
			for ( int w = 0 ; w < m_ObjectList.Count(); w++ )
			{ 
				Object FoundWreck = m_ObjectList.Get(w);
				if ( FoundWreck && (FoundWreck.GetType().Contains("Land_Wreck") || FoundWreck.GetType().Contains("StaticObj_Wreck")) )
				{				 
					Print("[SMM] Ganja Mission : BeforeSpawnCleanup :: "+ FoundWreck.GetType() +" was found near the garage, but can't be deleted. It could block the garage door.");
					g_Game.ObjectDeleteOnClient( FoundWreck );
					//g_Game.RemoteObjectDelete( FoundWreck );
					g_Game.ObjectDelete( FoundWreck );
					//!! nothing works when trying to delete the wreck infront of garage. Currently no Fix available.
				}	
			}
			
			//Update AI navmesh
			g_Game.GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( g_Game.UpdatePathgraphRegionByObject, 1000, false, Garage );		
		}			
	}
	
	void ~GanjaMission()
	{	
		//Despawn all remaining mission objects
		if ( m_MissionObjects )
		{
			Print("[SMM] Ganja Mission : Despawning "+ m_MissionObjects.Count() +" mission objects from old mission...");		
			for ( int i = 0; i < m_MissionObjects.Count(); i++ )
			{				
				if ( !m_MissionObjects.Get(i))	continue;
				else
				{	
					if ( m_MissionObjects.Get(i).IsTransport() ) 
					{
						//Don't delete just add full damage to MissionCar
						m_MissionObjects.Get(i).SetHealth("","",0);
						continue;
					}
							
					g_Game.ObjectDelete( m_MissionObjects.Get(i) );
				}			
			}
			m_MissionObjects.Clear();
		}	
		
		//Delete mission AI's
		if ( m_MissionAIs )
		{
			if ( m_MissionAIs.Count() > 0 )
			{
				Print("[SMM] Ganja Mission : Despawning "+ m_MissionAIs.Count() +" mission AI's from old mission...");
				for ( int k = 0; k < m_MissionAIs.Count(); k++ )
				{
					g_Game.ObjectDelete( m_MissionAIs.Get(k) );
				}
				m_MissionAIs.Clear();			
			}
			else Print("[SMM] Ganja Mission : No mission AI's to despawn.");
		}
		
		//Delete PlayersInZone list
		if ( m_PlayersInZone )	m_PlayersInZone.Clear();
	}
	
	void SpawnCar()
	{
		//reset position
        if ( MissionCar )
        {
            MissionCar.SetPosition( m_MissionPosition );
            return;
        }
					
		//Create MissionCar
		Print("[SMM] Ganja Mission : Spawning mission vehicle");
		
		//CAUTION: If car is not class OffroadHatchback then it can't be deleted at OnStartCleanup
		MissionCar = Car.Cast(g_Game.CreateObjectEx("OffroadHatchback", m_MissionPosition, ECE_PLACE_ON_SURFACE ));
		MissionCar.GetInventory().CreateAttachment("HatchbackWheel");
		MissionCar.GetInventory().CreateAttachment("HatchbackWheel");
		MissionCar.GetInventory().CreateAttachment("HatchbackWheel");
		MissionCar.GetInventory().CreateAttachment("HatchbackWheel");
		MissionCar.GetInventory().CreateAttachment("HatchbackWheel");
		MissionCar.GetInventory().CreateAttachment("HatchbackTrunk");
		MissionCar.GetInventory().CreateAttachment("HatchbackHood");
		MissionCar.GetInventory().CreateAttachment("HatchbackDoors_Driver");
		MissionCar.GetInventory().CreateAttachment("HatchbackDoors_CoDriver");
		MissionCar.GetInventory().CreateAttachment("CarRadiator");
		MissionCar.GetInventory().CreateAttachment("CarBattery");
		MissionCar.GetInventory().CreateAttachment("HeadlightH7");
		MissionCar.GetInventory().CreateAttachment("HeadlightH7");
		
		//Create car inventory
		MissionCar.GetInventory().CreateInInventory("Canteen");
		MissionCar.GetInventory().CreateInInventory("PersonalRadio");
		MissionCar.GetInventory().CreateInInventory("TireRepairKit");
		EntityAI kit = MissionCar.GetInventory().CreateInInventory("FirstAidKit");
			kit.GetInventory().CreateInInventory("TetracyclineAntibiotics");
			kit.GetInventory().CreateInInventory("BandageDressing");
			kit.GetInventory().CreateInInventory("Morphine");
		MissionCar.GetInventory().CreateInInventory("Battery9V");
		MissionCar.GetInventory().CreateInInventory("Battery9V");
		MissionCar.GetInventory().CreateInInventory("Battery9V");
		MissionCar.GetInventory().CreateInInventory("Battery9V");
		MissionCar.GetInventory().CreateInInventory("HeadlightH7");
		MissionCar.GetInventory().CreateInInventory("HeadlightH7");
		
		//Fill up needed car fluids
		MissionCar.Fill(CarFluid.COOLANT, 10.0 ); 
		MissionCar.Fill(CarFluid.OIL, 5.0 );
		
		//Set car orientation with rear to door 
		MissionCar.SetOrientation( MissionBuilding.GetOrientation() + "180 0 0" );
		
		//Set car CE lifetime
		MissionCar.SetLifetime( 3600 );	
		
		m_MissionObjects.InsertAt( MissionCar, 0 );
		
		//recall method for position reset
		g_Game.GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( this.SpawnCar, 3000, false );	
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
					
		Print("[SMM] Ganja Mission : Mission rewards spawned in reward container. Randomly selected loadout was "+selectedLoadout+"." );			
	}
	
	void SpawnObjects()
	{	
		if ( m_MissionExtended )
		{	
			//Get random spawnpoint for car
			m_MissionPosition = MissionBuilding.ModelToWorld( GarageCarSpawns.GetRandomElement());
	 		
			//Clean up MissionCar spawnpoint
			g_Game.GetObjectsAtPosition( m_MissionPosition, 1.5, m_ObjectList, m_ObjectCargoList );
			
			if ( m_ObjectList.Count() > 0)
			{
				for ( int i=0; i < m_ObjectList.Count(); i++)
				{
					Object FoundObject = m_ObjectList.Get(i);
					if ( FoundObject.IsItemBase() )
					{
						Print("[SMM] Ganja Mission : BeforeSpawnCleanUp :: Object  " + FoundObject.ToString() + " at new mission position was deleted.");
						g_Game.ObjectDelete( FoundObject );
					}
				}
			}

			//Spawn car 1 second later			
			g_Game.GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( this.SpawnCar, 1000 );
						
			//Spawn Ganja bag
			MissionObject = ItemBase.Cast( g_Game.CreateObjectEx("CourierBag", MissionBuilding.ModelToWorld( Spawnpoints.Get(1) ), ECE_PLACE_ON_SURFACE ));
			MissionObject.SetTakeable(false);
			m_MissionObjects.InsertAt( MissionObject, 1 );			
			
			//Spawn mission vehicle related stuff			
			m_MissionObjects.Insert( g_Game.CreateObjectEx("CanisterGasoline", MissionBuilding.ModelToWorld( Spawnpoints.Get(3) ), ECE_PLACE_ON_SURFACE ));
			m_MissionObjects.Insert( g_Game.CreateObjectEx("SparkPlug", MissionBuilding.ModelToWorld( Spawnpoints.Get(4) ), ECE_PLACE_ON_SURFACE ));		
			m_MissionObjects.Insert( g_Game.CreateObjectEx("Wrench", MissionBuilding.ModelToWorld( Spawnpoints.Get(4) - "0 0 0.2" ), ECE_PLACE_ON_SURFACE ));
			
			//Cluster spawn mission ganja 			
			for ( int j=0; j < ReqWeedAmount; j++ )
			{
				Object Ganja = g_Game.CreateObjectEx("Cannabis", MissionBuilding.ModelToWorld( Spawnpoints.Get(2) ), ECE_PLACE_ON_SURFACE );
				vector gpos = { Math.RandomFloatInclusive(-0.3, 0.3), 0, Math.RandomFloatInclusive(-0.3, 0.3) };
				Ganja.SetPosition( MissionBuilding.ModelToWorld( Spawnpoints.Get(2) + gpos ) );
				m_MissionObjects.Insert( Ganja );
			}

			//check darkness for location light support and spawn lamp if needed
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
					
					if ( MissionSettings.DebugMode )
					Print("[SMM] Ganja Mission : Test Hour is "+ hour +", SunRiseHour: "+ SunRiseHour +", SonSetHour: "+ SunSetHour );
					
					if ( hour >= SunSetHour || hour <= SunRiseHour ) 
					{

						//Choose the Light position Here. 
						MissionLight = g_Game.CreateObjectEx("PortableGasLamp", MissionBuilding.ModelToWorld( "1.575 -0.605 -5.431" ), ECE_PLACE_ON_SURFACE);

						ItemBase lamp = ItemBase.Cast(MissionLight);
						if ( !lamp )
						{
							Print("[SMM] Ganja Mission : ERROR :: Could not spawn mission light at " + m_MissionPosition);
							return;
						}

						EntityAI canister = lamp.GetInventory().CreateAttachment("SmallGasCanister");
						if ( !canister )
						{
							Print("[SMM] Ganja Mission : ERROR :: Could not attach SmallGasCanister to mission light.");
						}

						if ( lamp.HasEnergyManager() && lamp.GetCompEM().CanSwitchOn() )
						{
							lamp.GetCompEM().SwitchOn();
						}
						else
						{
							Print("[SMM] Ganja Mission : ERROR :: Mission light can not be switched on.");
						}
						m_MissionObjects.Insert( lamp );
					}
				}
			}
			
			Print("[SMM] Ganja Mission : Survivor Mission "+ m_selectedMission +" :: "+ m_MissionName +" ...primary mission deployed!");
		}
		else
		{
			//Spawn nothing when player is entering secondary mission zone
				
			Print("[SMM] Ganja Mission : Survivor Mission Extension "+ m_selectedMission +" :: "+ m_MissionName +" ...secondary mission deployed!");
		}
	}
	
	void SpawnAIs()
	{	
		string RandomInfected;
		vector InfectedPos;
		DayZInfected Zed;
				
		if ( !MissionBuilding ) Print("[SMM] Ganja Mission : Extended Mission : Missionbuilding couldn't be found in "+ SecondaryLoc +"." );		
		else
		{
			if ( m_MissionExtended )
			{
				//Spawn some infected at garage
				for ( int j = 0 ; j < PriInfectSpawns.Count() ; j++ )
				{
		    	   	RandomInfected = InfectedTypes.GetRandomElement();
					InfectedPos = MissionBuilding.ModelToWorld( PriInfectSpawns.Get(j) );
					Zed = DayZInfected.Cast( g_Game.CreateObjectEx( RandomInfected, InfectedPos, ECE_PLACE_ON_SURFACE | ECE_INITAI ));
					m_MissionAIs.Insert( Zed );
				}
			}
			else
			{
				//Spawn some infected at Gas station
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
		//Despawn all mission objects at mission timeout except those retains until next mission
		for ( int i = 0; i < m_MissionObjects.Count(); i++ )
		{
			//Exception: MissionCar will remain but gets damaged
			if ( m_MissionObjects.Get(i) && m_MissionObjects.Get(i).IsTransport() )	
			{
				m_MissionObjects.Get(i).SetHealth("","",0);
				continue;
			}
			
			//Delete Object
			g_Game.ObjectDelete( m_MissionObjects.Get(i) );
			m_MissionObjects.Remove(i);
		}
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
		m_MissionZoneOuterRadius = 80.0;
		m_MissionZoneInnerRadius = 2.0;
		
		//Get secondary mission position
		if ( SecondaryLoc == "Widok" )
		{
			if ( EventsWorldData.GetBuildingsAtLoc("Land_FuelStation_Build", SecondaryLoc, ExtendedPosList ))
			{
				m_MissionPosition = ExtendedPosList.GetRandomElement();
				Print("[SMM] Ganja Mission : New Mission Position is Land_FuelStation_Build at "+ SecondaryLoc +" @ "+ m_MissionPosition );
				SecondaryLoc.Replace("_"," ");
			}
		}
		else if ( EventsWorldData.GetBuildingsAtLoc("Land_FuelStation_Build_Enoch", SecondaryLoc, ExtendedPosList ))
		{
			m_MissionPosition = ExtendedPosList.GetRandomElement();
			Print("[SMM] Ganja Mission : New Mission Position is Land_FuelStation_Build_Enoch at "+ SecondaryLoc +" @ "+ m_MissionPosition );
			SecondaryLoc.Replace("_"," ");
		}
		else Print("[SMM] Ganja Mission : Can't get secondary MissionPosition in "+ SecondaryLoc +" from EventsWorldData!");
		
		//translate secondary location
		if ( MissionSettings.Opt_TranslateEWData )
		{
			m_MissionSecondaryLoc = EventsWorldData.TransLoc( m_MissionSecondaryLoc );
		}
				
		//Set messages for secondary mission
		m_MissionMessage1 = "Allright, you have found the car. "+ SurvivorName +" has probably drained all fuel, so prepare the car before that long trip. Check if the dried Cannabis is anywhere in the garages there and take the bag and put all 25 pieces in it.";
		m_MissionMessage2 = "Drive to the ** "+ m_MissionSecondaryLoc +" Gas Station **\nand bring the Ganja bag to the cash desk. "+ SurvivorName +"'s Ganja has the highest priority. Please don't steal or smoke any of it by your own because it could be very dangerous.";
		m_MissionMessage3 = "I have some stuff for you at the gas station. Watch out! There might be bandits on the way to "+ m_MissionSecondaryLoc +", who could have intercepted our little radio talk here. Good luck and drive carefully!";
		//Add GPS coordinates of mission in message
		if ( MissionSettings.Opt_ShowGpsInfo ) m_MissionMessage3 += "\n\nGPS coordinates:"+ m_MissionInstance.GetGPS();		
	}
	
	override void MissionFinal()
	{	//When player enters last mission target zone
		//do nothing		
	}
	
	override void PlayerChecks( PlayerBase player )
	{	//executed only on players in mission zone. If nobody is in zone this method is not executed!
		
		//Check if MissionObject is brought to MissionPosition
		if ( !m_MissionExtended && MissionObject )
		{		
			float BagDistance = vector.Distance( MissionObject.GetPosition(), m_MissionPosition );
			
			if ( BagDistance < 3 )
			{				
				CargoCount = MissionObject.GetInventory().CountInventory();
				
				if ( CargoCount != LastCount )
				{
					if ( CargoCount >= ReqWeedAmount )
					{	
						CargoBase CargoItems1 = MissionObject.GetInventory().GetCargo();		
						
						for ( int i = 0; i < CargoCount; i++ )
						{
							EntityAI CargoItem = CargoItems1.GetItem(i);
							if ( m_MissionObjects.Find( CargoItem ) > -1 ) FoundObjects++;
							else continue;					
							
							//When requested amount of Ganja is present, despawn MissionObject & spawn rewards
							if ( FoundObjects >= ReqWeedAmount )
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
										Print("[SMM] Ganja Mission : Player with SteamID64: "+ player.GetIdentity().GetPlainId() +" has successfully stored the requested amount of "+ ReqWeedAmount +" MissionObjects in the container.");
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
									Print("[SMM] Ganja Mission : Player with SteamID64: "+ IdentPlainID +" deserves respect for his "+ ExtMsg +".");
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
	}

	override void EntityChecks()
	{	//Executed continuously when mission was actived. Avoid complex calculations or big data processing!
		
		//Check if MissionCar was hijacked & covered. Delete car cover ( from 'CarCover' Mod ) to prevent abuse and car farming.
		if ( !CarIsHijacked )
		{
			if ( MissionCar ) CarPosition = MissionCar.GetPosition();
			else if ( CarPosition )
			{
				g_Game.GetObjectsAtPosition( CarPosition, 3.0 , m_ObjectList , m_ObjectCargoList );
				
				for ( int k=0; k < m_ObjectList.Count(); k++)
				{
					Object FoundObject = m_ObjectList.Get(k);
					if ( g_Game.ObjectIsKindOf( FoundObject, "CarCoverBase" ) )
					{
						Print("[SMM] Ganja Mission : MissionCar was hijacked. Deleting car cover...");
						CarIsHijacked = true;
						g_Game.ObjectDelete( FoundObject );
						
						//abbort mission
						m_RewardsSpawned = true;
						m_MsgNum = -2;
						m_MsgChkTime = m_MissionTime + MsgDlyFinish;
						break;										
					}
				}
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
				if ( FoundObject.GetType() == "Land_FuelStation_Build_Enoch" || FoundObject.GetType() == "Land_FuelStation_Build")
				{			 
					MissionBuilding = FoundObject;
					Print("[SMM] Ganja Mission : Secondary mission zone triggered. MissionBuilding extended is Land_FuelStation_Build_Enoch at "+ SecondaryLoc +" @ "+ MissionBuilding.GetPosition() );
					
					//new MissionPosition is rewards spawnpoint
					m_MissionPosition = MissionBuilding.ModelToWorld( Spawnpoints.Get(5) );
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
			Print("[SMM] Ganja Mission : ERROR :: MissionBuilding can't be found!");
			if ( MissionSettings.DebugMode ) Print("[SMM] Ganja Mission : MissionBuilding is NULL!");
			return false;
		}
	}
}
