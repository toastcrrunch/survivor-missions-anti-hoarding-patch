class PlaneCrashMission extends SurvivorMissions		//by Xellose and Funkdoc
{
	//Mission related entities 
	Object CrashedC130;
	ItemBase MissionObject;
	EffecterBase TargetSmokeEffect;
	EffecterBase TargetFireEffect;
	Object MissionLight;

#ifdef ENFUSION_AI_PROJECT
#ifdef EXPANSIONMODAI	
	//Mission AI related entities
	eAIGroup MissionGroup;
	eAIBase Pilot;
#endif
#endif
		
	//Mission parameters
	int MsgDlyFinish = 60;					//seconds, message delay time after player has finished mission
	bool EnableBots = true;					//1= Enable bots in this mission, 0= Disable bots even if EAI mod is installed

	vector PilotSpawn = "0.74 -4.46 16";
	
	//Mission containers
	ref array<vector> ContainerSpawns = new array<vector>;
	ref array<vector> InfectedSpawns = new array<vector>;
	ref array<vector> FireSpawns = new array<vector>;
	ref array<string> InfectedTypes = new array<string>;
	ref array<vector> AIWaypoints = new array<vector>;
	ref array<PlayerBase> AssistingPlayers = new array<PlayerBase>;
		
	//Mission variables 
	string SurvivorName;
	int SmokeEffecterID;
	int FireEffecterID;		
	
	override bool IsExtended() { return false; };
	
	void PlaneCrashMission()
	{		
		//Mission mission timeout
		m_MissionTimeout = 2700;			//seconds, mission duration time
		
		//Mission zones
		m_MissionZoneOuterRadius = 700.0;	//meters (!Do not set lower than 200m), mission activation distance
		m_MissionZoneInnerRadius = 3.0;		//meters (!Do not set outside of 1-10m), mission finishing distance to target object
				
		//Mission informant
		m_MissionInformant = "Dr. Legasov";
	
		//Mission person names
		SurvivorName = "Gerry Lane";
		
		//Translate mission location name and direction
		if ( MissionSettings.Opt_TranslateEWData )
		{
			m_MissionLocation = EventsWorldData.TransLoc( m_MissionLocation );
			m_MissionLocationDir = EventsWorldData.TransDir( m_MissionLocationDir );
		}
		
		//Set mission messages
        m_MissionMessage1 = "I recently received an emergency call from a military pilot. One crew member was infected and suddenly attacked others. He said this flight has the highest international priority.";
        m_MissionMessage2 = "Also onboard is Mr. "+ SurvivorName +", he is an UN Agent and it seems he has some information about the cure for this pandemic disease. They were at flight level 300 on their way to Wales.";
        m_MissionMessage3 = "The pilot was bitten by an infected crew member and tried to emergency land somewhere in the fields\n** "+ m_MissionLocationDir +" of "+ m_MissionLocation +" **\nFind out if they made it. They have some equipment on board!";		

		if ( EnableBots )
		{
			m_MissionMessage1 = "I recently got a satellite phone call from UN Agent "+ SurvivorName +". He has important information about the cure for this pandemic disease and tried to get to Wales with a C130 military plane but the pilot took off and left him behind in Jerusalem.";
			m_MissionMessage2 = "It seems the pilot of that C130 is collaborating with the Soviets and stole the important documents. The plane was on its way to Moscow when suddenly an engine caught on fire and alot of fuel was lost.";
			m_MissionMessage3 = "The pilot tried to emergency land somewhere in the fields\n** "+ m_MissionLocationDir +" of "+ m_MissionLocation +" **\nHe is armed and will defend himself if needed but there is some equipment on board you should pick up. Be careful!";
		}
		//Add GPS coordinates of mission in message
		if ( MissionSettings.Opt_ShowGpsInfo ) m_MissionMessage1 += "\n\nGPS coordinates:"+ m_MissionInstance.GetGPS();
						
		//Engine fire spawns 
		FireSpawns.Insert("5.15 -2.18 -4.63");		//engine
		FireSpawns.Insert("5.15 -2.18 -5");			//engine
		FireSpawns.Insert("5.15 0 0");			//fire trace
		FireSpawns.Insert("4.8 0 0.5");			//fire trace
		FireSpawns.Insert("5.3 0 1");			//fire trace
		FireSpawns.Insert("5.15 0 15");			//fire trace
		FireSpawns.Insert("5.15 0 25");			//fire trace
		//FireSpawns.Insert("5.15 0 30");			//fire trace
		//FireSpawns.Insert("5.15 0 45");			//fire trace
		//FireSpawns.Insert("5.15 0 55");			//fire trace
		
		//Container spawnpoints
		ContainerSpawns.Insert("0.87 -4.5 -2.5");
		ContainerSpawns.Insert("0.87 -4.47 -0.5");
		
		//eAI waypoints
		AIWaypoints.Insert("0 0 22");
		AIWaypoints.Insert("22 0 -2");
		AIWaypoints.Insert("0 0 -22");
		AIWaypoints.Insert("-22 0 -2");
	
		//Infected spawnpoints
		InfectedSpawns.Insert("-10 -3 25");
		InfectedSpawns.Insert("25 -3 -10");
		InfectedSpawns.Insert("-30 -3 -10");
		InfectedSpawns.Insert("-20 -3 15");
		InfectedSpawns.Insert("15 -3 -10");
		InfectedSpawns.Insert("10 -3 30");
		InfectedSpawns.Insert("-20 -3 10");
		InfectedSpawns.Insert("40 -3 20");
		InfectedSpawns.Insert("-20 -3 -40");
		InfectedSpawns.Insert("-50 -3 50");
		InfectedSpawns.Insert("35 -3 40");
		InfectedSpawns.Insert("-40 -3 -20");
		InfectedSpawns.Insert("-30 -3 -40");
		
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
	
	void ~PlaneCrashMission()
	{
		//Despawn all remaining mission objects
		if ( m_MissionObjects )
		{	
			Print("[SMM] PlaneCrash Mission : Despawning "+ m_MissionObjects.Count() +" mission objects from old mission...");				
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
				Print("[SMM] PlaneCrash Mission : Despawning "+ m_MissionAIs.Count() +" mission AI's from old mission...");
				for ( int k = 0; k < m_MissionAIs.Count(); k++ )
				{
					g_Game.ObjectDelete( m_MissionAIs.Get(k) );
				}
				m_MissionAIs.Clear();			
			}
			else Print("[SMM] PlaneCrash Mission : No mission AI's to despawn.");
		}

		DeleteMarker();
		
		//Delete PlayersInZone list
		if ( m_PlayersInZone )	m_PlayersInZone.Clear();
	}
	
	void SpawnObjects()
	{
		//Spawn crashed plane crashed C130 
		int PlaneOriConversion;
		string PlaneOrientation;
		
		PlaneOriConversion = m_MissionDescription[3].ToInt() - 180;
		if ( PlaneOriConversion < 0 ) PlaneOriConversion += 360;
		PlaneOrientation = PlaneOriConversion.ToString() +" 0 0";
		
		CrashedC130 = g_Game.CreateObjectEx( "Land_Wreck_C130J", m_MissionPosition, ECE_PLACE_ON_SURFACE );
		CrashedC130.SetOrientation( PlaneOrientation.ToVector() );
		CrashedC130.PlaceOnSurface();
		m_MissionObjects.Insert( CrashedC130 );

		//Engine fire particle effects
		ParticleEffecterParameters fireParam = new ParticleEffecterParameters( "ParticleEffecter", EffecterBase.NOT_DEFINED_LIFESPAN, ParticleList.CAMP_NORMAL_FIRE );
		for (int s=0; s < FireSpawns.Count(); s++)
		{
			vector firePos;

			if ( s > 1 )
			{
				vector posXZ = CrashedC130.ModelToWorld( FireSpawns.Get(s) );
				float x = posXZ[0];
				float z = posXZ[2];
				float y = g_Game.SurfaceY(x,z);
				firePos = {x,y,z};
			}
			else
			{
				firePos = CrashedC130.ModelToWorld( FireSpawns.Get(s) );
			}

			FireEffecterID = SEffectManager.CreateParticleServer( firePos, fireParam );
		}
		
		//Create smoke on destroyed engine  
		ParticleEffecterParameters param = new ParticleEffecterParameters( "ParticleEffecter", EffecterBase.NOT_DEFINED_LIFESPAN, ParticleList.SMOKING_HELI_WRECK );
		SmokeEffecterID = SEffectManager.CreateParticleServer( CrashedC130.ModelToWorld("5.15 -2.18 -4.63"), param );

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
				Print("[SMM] PlaneCrash Mission : Test Hour is "+ hour +", SunRiseHour: "+ SunRiseHour +", SonSetHour: "+ SunSetHour );
				
				if ( hour >= SunSetHour || hour <= SunRiseHour ) 
				{

					//Choose the Light position Here. 
					vector lightPos = CrashedC130.ModelToWorld("-0.5 -4.1 -5.2");
					MissionLight = g_Game.CreateObjectEx("PortableGasLamp", lightPos, ECE_PLACE_ON_SURFACE);

					ItemBase lamp = ItemBase.Cast(MissionLight);
					if ( !lamp )
					{
						Print("[SMM] PlaneCrash Mission : ERROR :: Could not spawn mission light at " + lightPos);
						return;
					}

					EntityAI canister = lamp.GetInventory().CreateAttachment("SmallGasCanister");
					if ( !canister )
					{
						Print("[SMM] PlaneCrash Mission : ERROR :: Could not attach SmallGasCanister to mission light.");
					}

					if ( lamp.HasEnergyManager() && lamp.GetCompEM().CanSwitchOn() )
					{
						lamp.GetCompEM().SwitchOn();
					}
					else
					{
						Print("[SMM] PlaneCrash Mission : ERROR :: Mission light can not be switched on.");
					}
					m_MissionObjects.Insert( lamp );
				}
			}
		}

		
		//Spawn containers 
		int selectedLoadout;
		
		for (int i=0; i < ContainerSpawns.Count(); i++)
		{	
			MissionObject = ItemBase.Cast( g_Game.CreateObjectEx( "MissionCrate", CrashedC130.ModelToWorld( ContainerSpawns.Get(i)), ECE_PLACE_ON_SURFACE ));
			MissionObject.SetTakeable(false);
			
			//Get random loadout 
			selectedLoadout = Math.RandomIntInclusive(0,9);	//!change randomization limit after adding new loadouts!	
		
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
					
			Print("[SMM] PlaneCrash Mission : Mission rewards spawned in reward container"+i+". Randomly selected loadout was "+selectedLoadout+"." );
				
			//Insert mission container into mission objects list
			m_MissionObjects.Insert( MissionObject );

			Object rewardCrate = MissionObject;
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(g_Game.ObjectDelete, 300000, false, rewardCrate);
		}
		
		//move mission position to first crate
		m_MissionPosition = CrashedC130.ModelToWorld( ContainerSpawns.Get(0) );
		
		Print("[SMM] PlaneCrash Mission : Survivor Mission "+ m_selectedMission +" :: "+ m_MissionName +" ...mission deployed!");
	}

	void DeleteMarker()
	{
		g_Game.ObjectDelete( TargetSmokeEffect );
		g_Game.ObjectDelete( TargetFireEffect );
		TargetSmokeEffect = null;
		TargetFireEffect = null;
		SEffectManager.DestroyEffecterParticleServer( SmokeEffecterID );
		SEffectManager.DestroyEffecterParticleServer( FireEffecterID );
	}
	
	void SpawnAIs()
	{
		//Spawn the Pilot Bot
		if ( m_EAIenabled && MissionSettings.Opt_AddMissionBots && EnableBots )
		{	
#ifdef ENFUSION_AI_PROJECT
#ifdef EXPANSIONMODAI	
			vector temp = CrashedC130.ModelToWorld( PilotSpawn );
			PilotSpawn = {temp[0],g_Game.SurfaceY(temp[0],temp[2]),temp[2]};

			//spawn Pilot and set AI props
			Pilot = eAIBase.Cast(g_Game.CreateObject( "eAI_SurvivorM_Peter", PilotSpawn));
			Pilot.SetPosition(m_MissionPosition);
			Pilot.PlaceOnSurface();
			Pilot.SetMovementSpeedLimits(1.0, 3.0);
						
			//Pilot clothes
			Pilot.GetInventory().CreateAttachment("ZSh3PilotHelmet_Green");
			Pilot.GetInventory().CreateAttachment("BomberJacket_Olive");
			Pilot.GetInventory().CreateAttachment("UKAssVest_Olive");
			Pilot.GetInventory().CreateAttachment("Jeans_Blue");
			Pilot.GetInventory().CreateAttachment("HikingBoots_Brown");
			
			//Pilot loadout
			EntityAI weapon;
			weapon = Pilot.GetInventory().CreateInInventory("Glock19");
			weapon.GetInventory().CreateAttachment("PistolSuppressor");
			Pilot.GetInventory().CreateInInventory("Mag_Glock_15Rnd");
			Pilot.GetInventory().CreateInInventory("Mag_Glock_15Rnd");
			Pilot.GetInventory().CreateInInventory("Mag_Glock_15Rnd");
			Pilot.GetInventory().CreateInInventory("Mag_Glock_15Rnd");
			Pilot.GetInventory().CreateInInventory("Mag_Glock_15Rnd");
						 
			//set AI behavior and props
			Pilot.eAI_SetThreatDistanceLimit(25);
			Pilot.eAI_SetLootingBehavior(0);
			Pilot.eAI_SetUnlimitedReload(true);

			//Get MissionGroup and ensure Pilot is a member of the group
			MissionGroup = Pilot.GetGroup();
			if (!MissionGroup.IsMember(Pilot))
				MissionGroup.AddMember(Pilot);
						
			//Set faction for house defend
			eAIFaction Passive = new eAIFactionGuards();
			MissionGroup.SetFaction(Passive);
			
			//Add pilot to MissionAI's
			m_MissionAIs.Insert(Pilot);
			
			//Set Pilot as Leader of Group
			MissionGroup.SetLeader(Pilot);
			
			Print("[SMM] PlaneCrash Mission : eAI Mission Bot spawned at crashed plane and is armed with "+ weapon.GetType() +"." );
			
			//Add waypoints around the plane
			for ( int i=0; i < AIWaypoints.Count(); i++ )
			{
				vector wp = CrashedC130.ModelToWorld( AIWaypoints.Get(i) );
				vector setwp = {wp[0],g_Game.SurfaceY(wp[0],wp[2]),wp[2]};
				MissionGroup.AddWaypoint( setwp );
			}
			
			//Bot is looping through the waypoints
			MissionGroup.SetWaypointBehaviour(eAIWaypointBehavior.LOOP);
#endif
#endif
		}
		//spawn only infected if ExpAI is not installed, was disabled globally or only in this mission
		else	
		{
			//Spawn infected pilot
			vector InfectedPos = CrashedC130.ModelToWorld( "3 -3 -9" );
			m_MissionAIs.Insert( g_Game.CreateObjectEx( "ZmbM_CommercialPilotOld_Olive" , InfectedPos , ECE_PLACE_ON_SURFACE | ECE_INITAI ) );
			DayZInfected InfectedPilot = DayZInfected.Cast( m_MissionAIs[0] );
				InfectedPilot.GetInventory().CreateAttachment("ZSh3PilotHelmet_Green");
				InfectedPilot.GetInventory().CreateAttachment("UKAssVest_Olive");
				InfectedPilot.GetInventory().CreateInInventory("Glock19");
				InfectedPilot.GetInventory().CreateInInventory("Mag_Glock_15Rnd");
				InfectedPilot.GetInventory().CreateInInventory("Battery9V");
				InfectedPilot.GetInventory().CreateInInventory("Battery9V");
				InfectedPilot.GetInventory().CreateInInventory("Battery9V");
						
				InfectedPilot.SetHealth("","",15);
			
			
			//Spawn infected
			for ( int j = 0 ; j < InfectedSpawns.Count() ; j++ )
			{
	    	   	string RandomInfected = InfectedTypes.GetRandomElement();
				vector InfectedPos1 = CrashedC130.ModelToWorld( InfectedSpawns.Get(j) );
				m_MissionAIs.Insert( g_Game.CreateObjectEx( RandomInfected, InfectedPos1, ECE_PLACE_ON_SURFACE | ECE_INITAI ) );
			}
		}
	}
	
	override void ObjDespawn() 
	{	
		//Despawn nothing
	}
	
	override void MissionFinal()
	{	//When player enters last mission target zone	
		
		//Respawn some infected 
		if ( EnableBots )
		{
			for ( int i=0 ; i < InfectedSpawns.Count() ; i++)
			{
	    	   	string RandomInfected = InfectedTypes.GetRandomElement();
				vector InfectedPos = CrashedC130.ModelToWorld( InfectedSpawns.Get(i) );
				m_MissionAIs.Insert( g_Game.CreateObjectEx( RandomInfected, InfectedPos, ECE_PLACE_ON_SURFACE | ECE_INITAI ));			
			}
		}
		
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
			Print("[SMM] PlaneCrash Mission : Player with SteamID64: "+ IdentPlainID +" deserves respect for his "+ ExtMsg +".");
			//send Respect message to player
			NotificationSystem.SendNotificationToPlayerIdentityExtended( Ident , 15.0, RespectMsg, "", MsgIcon );
		}
		
		//Finish mission
		m_RewardsSpawned = true;
		m_MsgNum = -1;
		m_MsgChkTime = m_MissionTime + MsgDlyFinish;			
	}
	
	override void PlayerChecks( PlayerBase player )
	{	//Executed continuously on target player while he is inside mission zone. Avoid complex calculations or big data processing!
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
			Print("[SMM] PlaneCrash Mission : ERROR :: Mission position was rejected or doesn't exist. MissionPosition is NULL!");
			return false;
		}
	}
}

