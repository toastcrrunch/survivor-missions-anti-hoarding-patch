class CaptureFlagMission extends SideMissions		//by Xellose and Funkdoc
{
	//Mission related entities 
	ItemBase MissionObject;
	Object FlagBase, Table1, Table2;
	BarrelHoles_ColorBase BarrelFire;
	Flag_Base Flag;
	
	//Mission parameters
	bool EnableBots = true;					//enable ExpansionAI for this mission
	int BotCount = 3;						//amount of bots spawning to counter attack the flag position
	int MsgDlyFinish = 15;					//seconds, message delay time after player has finished mission
	int HordeDensity = 60;					//infected, spawned in circle area
	int CaptureTime = 30;					//seconds, time to capture the flag ..full minutes (60,120,...)
	int CAPercentage = 60;					//percent, capture progress for counter attack start
	string DownFlag = "Flag_Bohemia";		//class name of flag to lower
	string UpFlag = "Flag_Chernarus";		//class name of flag to raise
	
	//mission vars
	vector PoleHighPos, PoleLowPos;		
	float flag_speed, delta_height;
	int FlagElevationChkTime;
	int FireplaceSpendChkTime, BotStuckChkTime;
	int FlagCaptureProgress, BotStuckCounter;
	int TeamPlayerCount;
	bool CaptureFinished = false;
	bool RebelsGroupSpawned = false;
	
#ifdef ENFUSION_AI_PROJECT
#ifdef EXPANSIONMODAI	
	eAIGroup BohemianRebels;
#endif
#endif
	
	//Mission containers
	ref array<ref Param3<string,vector,vector>> BuildingSpawns = new array<ref Param3<string,vector,vector>>;
	ref array<vector> Spawnpoints = new array<vector>;
	ref array<vector> FireSpawns = new array<vector>;
	ref array<string> InfectedTypes = new array<string>;
	ref array<PlayerBase> AssistingPlayers = new array<PlayerBase>;
	ref array<vector> GroupPosBuffer = new array<vector>;
	override bool IsExtended() { return true; };
	
	void CaptureFlagMission()
	{
		//Select primary mission
		m_MissionExtended = true;
		
		//Mission timeout
		m_MissionTimeout = 2700;			//seconds, max. mission duration (default value: 2700s = 45min)
		
		//Mission zones
		m_MissionZoneOuterRadius = 500.0;	//meters (!Do not set lower than 200m), mission activation distance
		m_MissionZoneInnerRadius = 100.0;	//meters (!Do not set outside of base radius), mission finishing distance to target object
		
		//Mission informant
		m_MissionInformant = "Dr. Legasov";
		
		//Translate mission location name and direction
		if ( MissionSettings.Opt_TranslateEWData )
		{
			m_MissionLocation = EventsWorldData.TransLoc( m_MissionLocation );
			m_MissionLocationDir = EventsWorldData.TransDir( m_MissionLocationDir );
		}
				
		//Set mission messages
		m_MissionMessage1 = "One of our defense positions was occupied by the Bohemian Rebels yesterday. Because of the exchange of fire between the rebels and a few survivor's alot of infected were attracted.";
		m_MissionMessage2 = "Find the defense position and capture the base flag for us. It needs atleast "+ ( CaptureTime / 60 ).ToString() +" minutes to lower the rebel's flag and raise our own.";
		m_MissionMessage3 = "The defense position is at the\n** "+ m_MissionLocationDir +" side of "+ m_MissionLocation +" **\nBe careful, there are alot of infected and the rebels might be still around there!";
		//Add GPS coordinates of mission in message
		if ( MissionSettings.Opt_ShowGpsInfo ) m_MissionMessage3 += "\n\nGPS coordinates:"+ m_MissionInstance.GetGPS();
		
		//Rewards spawns
		Spawnpoints.Insert("-0.422 0.39 0");	//table 1
		Spawnpoints.Insert("0.486 0.39 0");	//table 1
		Spawnpoints.Insert("0 0.39 0");		//table 2
		
		//Fireplace spawns
		FireSpawns.Insert("0 0 10");
		FireSpawns.Insert("0 0 -10");
		
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

		//Define surrounding flag base buildings/objects
		BuildingSpawns.Insert( new Param3<string,vector,vector>("Land_Roadblock_WoodenCrate", "-1.5 0 4", "65 0 90"));
		BuildingSpawns.Insert( new Param3<string,vector,vector>("Land_Roadblock_WoodenCrate", "1.5 0 4", "135 0 0"));
		BuildingSpawns.Insert( new Param3<string,vector,vector>("Land_Roadblock_Table", "-1.5 0 -3", "15 0 0"));
		BuildingSpawns.Insert( new Param3<string,vector,vector>("Land_Roadblock_Table", "1.5 0 -3", "0 0 0"));
		BuildingSpawns.Insert( new Param3<string,vector,vector>("BarrelHoles_Blue", "1.8 0 1.8", "0 0 0"));
		BuildingSpawns.Insert( new Param3<string,vector,vector>("Land_Mil_Fortified_Nest_Watchtower", "9 0 9", "135 0 0"));
		BuildingSpawns.Insert( new Param3<string,vector,vector>("Land_Mil_Fortified_Nest_Watchtower", "-9 0 -9", "315 0 0"));		
		BuildingSpawns.Insert( new Param3<string,vector,vector>("Land_Wreck_Ikarus", "13 0 -2.8", "-160 0 0"));
		BuildingSpawns.Insert( new Param3<string,vector,vector>("Land_Wreck_Uaz", "-12 0 2.8", "15 0 0"));
		BuildingSpawns.Insert( new Param3<string,vector,vector>("Land_wreck_truck01_aban1_orange", "0 0 13.5", "90 0 0"));
		
		//Clean up flag base position		
		g_Game.GetObjectsAtPosition( m_MissionPosition, 18.0 , m_ObjectList, m_ObjectCargoList );
		for ( int b=0; b < m_ObjectList.Count(); b++) 
		{
			Object FoundObject = m_ObjectList.Get(b);
			if ( FoundObject.IsItemBase() )
			{
				Print("[SMM] CaptureFlag Mission : BeforeSpawnCleanUp :: Object deleted: "+ FoundObject );  
				g_Game.ObjectDelete( FoundObject );
			}
		}
	}
	
	void ~CaptureFlagMission()
	{
		//Despawn all remaining mission objects
		if ( m_MissionObjects )
		{	
			Print("[SMM] CaptureFlag Mission : Despawning "+ m_MissionObjects.Count() +" mission objects from old mission...");				
			for ( int i = 0; i < m_MissionObjects.Count(); i++ )
			{ 
				if ( !m_MissionObjects.Get(i) ) continue;
				else
				{
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
				Print("[SMM] CaptureFlag Mission : Despawning "+ m_MissionAIs.Count() +" mission AI's from old mission...");
				for ( int k = 0; k < m_MissionAIs.Count(); k++ )
				{
					g_Game.ObjectDelete( m_MissionAIs.Get(k) );
				}
				m_MissionAIs.Clear();			
			}
			else Print("[SMM] CaptureFlag Mission : No mission AI's to despawn.");
		}
		
		//Delete PlayersInZone list
		if ( m_PlayersInZone )	m_PlayersInZone.Clear();
	}
	
	void SpawnObjects()
	{	
		//spawn flagpole (flagpole is the reference for all mission building & object spawnpoints!) 
		FlagBase = g_Game.CreateObject( "Land_Power_Pole_Conc4_Lamp_Amp", m_MissionPosition );
		FlagBase.SetOrientation( Vector( Math.RandomFloatInclusive(0,359.9), 0 , 0));		
		m_MissionObjects.Insert( FlagBase );											//Object 0

		//Spawn additional flag base buildings and objects
		for ( int i=0; i < BuildingSpawns.Count(); i++ )
		{
			Param3<string,vector,vector> BuildingDef = BuildingSpawns.Get(i);
			string SM_BuildingType = BuildingDef.param1;
			vector BuildingPos = FlagBase.ModelToWorld( BuildingDef.param2 );
			vector BuildingOri = BuildingDef.param3;
			vector BuildingDir = FlagBase.GetDirection();
			
			Object HQextra = g_Game.CreateObject( SM_BuildingType, BuildingPos );
			HQextra.SetDirection( BuildingDir );
			HQextra.SetOrientation( HQextra.GetOrientation() + BuildingOri );
			HQextra.PlaceOnSurface();
			m_MissionObjects.Insert( HQextra );											//Object 1 to 7
		}		
		
		//raise flagbase a little bit after other base objects spawned
		FlagBase.SetPosition( FlagBase.GetPosition() + "0 3.65 0" );
		
		//get flag height limits and offset height positions min max
		vector temp = FlagBase.ModelToWorld("0.82 0 -0.18");
		float tempheight = g_Game.SurfaceY(temp[0], temp[2]);
		temp[1] = tempheight;
		
		PoleHighPos = temp + "0 9.47 0";
		PoleLowPos = temp + "0 3 0";
		
		//flag hight and speed calculations
		delta_height = PoleHighPos[1] - PoleLowPos[1];						//height diff min max ...[m]
		flag_speed = -( delta_height / (( CaptureTime / 2 ) * 10 ) );		//vertical speed of flag movement, v = s / t ...[m/100ms]

		//spawn flag
		Flag = Flag_Base.Cast( g_Game.CreateObjectEx( DownFlag, PoleHighPos, ECE_AIRBORNE ));
		Flag.SetPosition( PoleHighPos );
		Flag.SetOrientation( FlagBase.GetOrientation() + "0 0 0");
		Flag.ShowSelection("unfolded");
		Flag.HideSelection("folded");
		Flag.SetTakeable(false);
		Flag.SetLifetime(7200);
		m_MissionObjects.Insert( Flag );												//Object 9

		//check darkness for location light support and spawn fireplaces
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
				Print("[SMM] CaptureFlag Mission : Test Hour is "+ hour +", SunRiseHour: "+ SunRiseHour +", SonSetHour: "+ SunSetHour );
				
				if ( hour >= SunSetHour || hour <= SunRiseHour ) 
				{
					EntityAI wood;
					
					for ( int k=0; k < FireSpawns.Count(); k++)
					{
						vector CampfirePosition = FlagBase.ModelToWorld( FireSpawns.Get(k) );
						
						//fireplace
						FireplaceBase Campfire = FireplaceBase.Cast( g_Game.CreateObjectEx( "Fireplace", CampfirePosition, ECE_PLACE_ON_SURFACE ) );
						//Campfire.Synchronize();
						wood = Campfire.GetInventory().CreateAttachment("Firewood");
						ItemBase.Cast( wood ).SetQuantityMax();
						Campfire.GetInventory().CreateAttachment("WoodenStick");
						Campfire.GetInventory().CreateAttachment("Rag");
						Campfire.StartFire( true );
						Campfire.Synchronize();
						m_MissionObjects.Insert( Campfire );
						
						//Cluttercutter fireplace
						Object cc_fireplace = g_Game.CreateObjectEx( "ClutterCutterFireplace" , Campfire.GetPosition(), ECE_PLACE_ON_SURFACE );
						cc_fireplace.SetOrientation( Campfire.GetOrientation() );
						g_Game.RemoteObjectCreate( cc_fireplace );
						m_MissionObjects.Insert( cc_fireplace );
					}
				}
			}
		}
		
		//flip flagpole after spawning all objects
		g_Game.GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( this.FlipPole, 3000);
		
		//update navmesh for AI after everything spawned
		vector min = Vector( m_MissionPosition[0] - 40, m_MissionPosition[1], m_MissionPosition[2] - 40 );
		vector max = Vector( m_MissionPosition[0] + 40, m_MissionPosition[1], m_MissionPosition[2] + 40 );
		g_Game.UpdatePathgraphRegion( min, max );
		
		Print("[SMM] CaptureFlag Mission : Survivor Mission "+ m_selectedMission +" :: "+ m_MissionName +" ...mission deployed!");
	}
	
	void FlipPole()
	{
		FlagBase.SetOrientation( FlagBase.GetOrientation() + "0 180 0");
	}
	
	void SpawnRewards()
	{
		Table1 = m_MissionObjects[3];
		Table2 = m_MissionObjects[4];
		
		for (int i=0; i < TeamPlayerCount; i++)
		{
			vector spawnpos;
			
			if ( i < 2 )
			spawnpos = Table1.ModelToWorld( Spawnpoints.Get(i) );
			else spawnpos = Table2.ModelToWorld( Spawnpoints.Get(i) );
			
			//Mission object is seachest
			MissionObject = ItemBase.Cast( g_Game.CreateObjectEx("MissionChest", spawnpos, ECE_PLACE_ON_SURFACE) );
			MissionObject.SetTakeable(false); 
			
			//Get random loadout 			
			int selectedLoadout = Math.RandomIntInclusive(0,11);	//!Change randomization limit after adding new loadouts!
			
			//Spawn selected loadout items in mission object
			EntityAI weapon;
									
			if (selectedLoadout == 0)
			{
				weapon = MissionObject.GetInventory().CreateInInventory("M4A1");
					weapon.GetInventory().CreateAttachment("M4_RISHndgrd");
					weapon.GetInventory().CreateAttachment("M4_MPBttstck");
					weapon.GetInventory().CreateAttachment("M4_Suppressor");
					weapon.GetInventory().CreateAttachment("ACOGOptic");
					//weapon.GetInventory().CreateAttachment("M4_T3NRDSOptic");
				MissionObject.GetInventory().CreateInInventory("Mag_STANAG_30Rnd");
				MissionObject.GetInventory().CreateInInventory("Ammo_556x45");
				MissionObject.GetInventory().CreateInInventory("Ammo_556x45");
				MissionObject.GetInventory().CreateInInventory("HuntingKnife");
				MissionObject.GetInventory().CreateInInventory("BakedBeansCan");
				MissionObject.GetInventory().CreateInInventory("Canteen");
				MissionObject.GetInventory().CreateInInventory("Battery9V");
			}
			if (selectedLoadout == 1)
			{
				weapon = MissionObject.GetInventory().CreateInInventory("SVD");
					weapon.GetInventory().CreateAttachment("PSO11Optic");
					//weapon.GetInventory().CreateAttachment("PSO1Optic");
					//weapon.GetInventory().CreateAttachment("KazuarOptic");
				MissionObject.GetInventory().CreateInInventory("Mag_SVD_10Rnd");
				MissionObject.GetInventory().CreateInInventory("Mag_SVD_10Rnd");
				MissionObject.GetInventory().CreateInInventory("Ammo_762x54");
				MissionObject.GetInventory().CreateInInventory("Ammo_762x54");
				MissionObject.GetInventory().CreateInInventory("CanOpener");
				MissionObject.GetInventory().CreateInInventory("PeachesCan");
				MissionObject.GetInventory().CreateInInventory("WaterBottle"); 
				MissionObject.GetInventory().CreateInInventory("Battery9V");
			}
			if (selectedLoadout == 2)
			{
				weapon = MissionObject.GetInventory().CreateInInventory("AKM");
					weapon.GetInventory().CreateAttachment("AK_RailHndgrd");
					weapon.GetInventory().CreateAttachment("AK_Suppressor");
					weapon.GetInventory().CreateAttachment("AK_PlasticBttstck");
					weapon.GetInventory().CreateAttachment("PSO1Optic");
					//weapon.GetInventory().CreateAttachment("KobraOptic");
				MissionObject.GetInventory().CreateInInventory("Mag_AKM_30Rnd");
				MissionObject.GetInventory().CreateInInventory("Ammo_762x39");
				MissionObject.GetInventory().CreateInInventory("Ammo_762x39");
				MissionObject.GetInventory().CreateInInventory("KitchenKnife");
				MissionObject.GetInventory().CreateInInventory("SardinesCan");
				MissionObject.GetInventory().CreateInInventory("Canteen");
				MissionObject.GetInventory().CreateInInventory("Battery9V");
			}
			if (selectedLoadout == 3)
			{
				weapon = MissionObject.GetInventory().CreateInInventory("FAL");
					weapon.GetInventory().CreateAttachment("Fal_OeBttstck");
					weapon.GetInventory().CreateAttachment("M4_Suppressor");
					weapon.GetInventory().CreateAttachment("M68Optic");
					//weapon.GetInventory().CreateAttachment("ReflexOptic");
				MissionObject.GetInventory().CreateInInventory("Mag_FAL_20Rnd");
				MissionObject.GetInventory().CreateInInventory("Mag_FAL_20Rnd");
				MissionObject.GetInventory().CreateInInventory("WaterBottle");
				MissionObject.GetInventory().CreateInInventory("MilitaryBelt");	
				MissionObject.GetInventory().CreateInInventory("M18SmokeGrenade_Yellow");	
				MissionObject.GetInventory().CreateInInventory("TacticalBaconCan");
				MissionObject.GetInventory().CreateInInventory("SteakKnife");
			}	
			if (selectedLoadout == 4)
			{
				weapon = MissionObject.GetInventory().CreateInInventory("SKS");
					weapon.GetInventory().CreateAttachment("PUScopeOptic");
					weapon.GetInventory().CreateAttachment("SKS_Bayonet");
				MissionObject.GetInventory().CreateInInventory("Ammo_762x39");
				MissionObject.GetInventory().CreateInInventory("Ammo_762x39");
				weapon = MissionObject.GetInventory().CreateInInventory("FNX45");
					weapon.GetInventory().CreateAttachment("PistolSuppressor");
					weapon.GetInventory().CreateAttachment("FNP45_MRDSOptic");
					weapon.GetInventory().CreateAttachment("TLRLight");
				MissionObject.GetInventory().CreateInInventory("Mag_FNX45_15Rnd");
				MissionObject.GetInventory().CreateInInventory("Ammo_45ACP");
				MissionObject.GetInventory().CreateInInventory("WaterBottle");			
				MissionObject.GetInventory().CreateInInventory("Battery9V");
			}
			if (selectedLoadout == 5)
			{			
				weapon = MissionObject.GetInventory().CreateInInventory("Winchester70");
					weapon.GetInventory().CreateAttachment("HuntingOptic");		
				MissionObject.GetInventory().CreateInInventory("Ammo_308Win");
				MissionObject.GetInventory().CreateInInventory("Ammo_308Win");
			
				weapon = MissionObject.GetInventory().CreateInInventory("CZ75");
					weapon.GetInventory().CreateAttachment("PistolSuppressor");
					weapon.GetInventory().CreateAttachment("FNP45_MRDSOptic");
				MissionObject.GetInventory().CreateInInventory("Mag_CZ75_15Rnd");
				MissionObject.GetInventory().CreateInInventory("Ammo_45ACP");
						
				weapon = MissionObject.GetInventory().CreateInInventory("NVGHeadstrap");
					weapon.GetInventory().CreateInInventory("NVGoggles");
				MissionObject.GetInventory().CreateInInventory("Battery9V");
				MissionObject.GetInventory().CreateInInventory("PsilocybeMushroom");
				MissionObject.GetInventory().CreateInInventory("WaterBottle");	
				MissionObject.GetInventory().CreateInInventory("SpaghettiCan");
				MissionObject.GetInventory().CreateInInventory("StoneKnife");
				MissionObject.GetInventory().CreateInInventory("M67Grenade");					
			}
			if (selectedLoadout == 6)
			{			
				weapon = MissionObject.GetInventory().CreateInInventory("UMP45");
					weapon.GetInventory().CreateAttachment("M68Optic");
					weapon.GetInventory().CreateAttachment("PistolSuppressor");			
				MissionObject.GetInventory().CreateInInventory("Mag_UMP_25Rnd");
				MissionObject.GetInventory().CreateInInventory("Mag_UMP_25Rnd");
				MissionObject.GetInventory().CreateInInventory("Ammo_45ACP");
				MissionObject.GetInventory().CreateInInventory("Battery9V");
				MissionObject.GetInventory().CreateInInventory("Cannabis");
				MissionObject.GetInventory().CreateInInventory("CannabisSeedsPack");
				MissionObject.GetInventory().CreateInInventory("WaterBottle");	
				MissionObject.GetInventory().CreateInInventory("TunaCan");
				MissionObject.GetInventory().CreateInInventory("RDG5Grenade");					
			}	
			if (selectedLoadout == 7)
			{			
				weapon = MissionObject.GetInventory().CreateInInventory("VSS");
					weapon.GetInventory().CreateAttachment("PSO11Optic");
					//weapon.GetInventory().CreateAttachment("PistolSuppressor");			
				MissionObject.GetInventory().CreateInInventory("Mag_VSS_10Rnd");
				MissionObject.GetInventory().CreateInInventory("Mag_VSS_10Rnd");
				MissionObject.GetInventory().CreateInInventory("AmmoBox_9x39_20Rnd");
				MissionObject.GetInventory().CreateInInventory("Battery9V");
				MissionObject.GetInventory().CreateInInventory("GardenLime");
				MissionObject.GetInventory().CreateInInventory("CannabisSeedsPack");
				MissionObject.GetInventory().CreateInInventory("WaterBottle");	
				MissionObject.GetInventory().CreateInInventory("SardinesCan");
				MissionObject.GetInventory().CreateInInventory("M18SmokeGrenade_Green");					
			}
			if (selectedLoadout == 8)
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
			if (selectedLoadout == 9)
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
			if (selectedLoadout == 10)
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
			if (selectedLoadout == 11)
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

			Print("[SMM] CaptureFlag Mission : Mission rewards spawned in reward container. Randomly selected loadout was "+ selectedLoadout +"." );
			Object rewardChest = MissionObject;
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(g_Game.ObjectDelete, 300000, false, rewardChest);
		}
		
		//barrel add firewood and start fire
		BarrelFire = BarrelHoles_ColorBase.Cast( m_MissionObjects[5] );
		BarrelFire.Open();
		BarrelFire.Synchronize();
		
		EntityAI barrel_wood = BarrelFire.GetInventory().CreateAttachment("Firewood"); 		
		BarrelFire.GetInventory().CreateAttachment("WoodenStick");
		BarrelFire.GetInventory().CreateAttachment("Rag");
		BarrelFire.StartFire( true );
		ItemBase.Cast( barrel_wood ).SetQuantityMax();
		
		//spawn 5 fish filets for smoking in barrel
		if ( BarrelFire.IsOpen() )	BarrelFire.Close();
		for ( i=0; i < 7; i++)
		BarrelFire.GetInventory().CreateAttachment("MackerelFilletMeat");

		BarrelFire.SetTakeable(false);		
	}
	
	void SpawnAIs()
	{
		int SpawnDlyTime = 500;
		
		//Spawn horde
		for ( int j = 0; j < HordeDensity; j++ )
		{
			//Spawn infected time delayed 500ms
			g_Game.GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( DelayedAISpawn, SpawnDlyTime, false );
			SpawnDlyTime += 500;
		}
	}
	
	void DelayedAISpawn()
	{
		//calc new spawn position
		float x = Math.RandomFloatInclusive( 20 , 50 );
		float y = Math.RandomFloatInclusive( 20 , 50 );	
		int Dice = Math.RandomIntInclusive( 0, 9);
		if ( Dice > 4 ) x *= -1.0;
		Dice = Math.RandomIntInclusive( 0, 9);
		if ( Dice < 5 ) y *= -1.0;
		vector NewPosVector = { x, 0, y };
		
		//spawn infected
		m_MissionAIs.Insert( g_Game.CreateObjectEx( InfectedTypes.GetRandomElement(), m_MissionPosition + NewPosVector, ECE_PLACE_ON_SURFACE | ECE_INITAI ));
	}
	
	void AICounterAttack()
	{
#ifdef ENFUSION_AI_PROJECT
#ifdef EXPANSIONMODAI
		//ExpAI declarations
		int Attempts = 100;
		float x,y,z;
		int Dice;
		
		eAIBase RebelBot;
		vector GroupSpawnPos;
		Object FoundObject;
		bool ItemsAtGround;
		
		//Get random nearby rebels spawn position in free terrain
		Print("[SMM] CaptureFlag Mission : Trying to get a proper random spawn position for the rebels for max. "+ Attempts +" attempts.");
		for ( int p=0; p < Attempts; p++ )
		{
			x = Math.RandomFloatInclusive( 80 , 150 );
			y = Math.RandomFloatInclusive( 80 , 150 );	
			Dice = Math.RandomIntInclusive( 0, 9);
			if ( Dice > 4 ) x *= -1.0;
			Dice = Math.RandomIntInclusive( 0, 9);
			if ( Dice < 5 ) y *= -1.0;
			
			GroupSpawnPos = m_MissionPosition + Vector(x,0,y);
			GroupSpawnPos[1] = g_Game.SurfaceY( GroupSpawnPos[0], GroupSpawnPos[2]);
			
			//check for sea level
			if ( GroupSpawnPos[1] > 1.0 )
			{
				//check for objects
				g_Game.GetObjectsAtPosition( GroupSpawnPos, 20, m_ObjectList, m_ObjectCargoList );
				
				for ( int q=0; q < m_ObjectList.Count(); q++ )
				{
					ItemsAtGround = false;
					
					FoundObject = m_ObjectList.Get(q);
					if ( FoundObject.IsRock() || FoundObject.IsBuilding() || FoundObject.IsInherited(BaseBuildingBase) ) //FoundObject.IsTree() || FoundObject.IsPlainObject()
					{
						Print("[SMM] CaptureFlag Mission : WARNING :: Attempt "+ (p + 1) +" - The random rebel group spawn position is near the ObjectType "+ FoundObject.GetType() +"! "+ GroupSpawnPos.ToString() );
						ItemsAtGround = true;
						break;
					}
				}
				if ( ItemsAtGround ) continue;
				else break;
			}
			else
			Print("[SMM] CaptureFlag Mission : WARNING :: Attempt "+ (p + 1) +" - The random stash position is under sea level! "+ GroupSpawnPos.ToString() );
			
			if ( p == Attempts - 1 )
			{
				Print("[SMM] CaptureFlag Mission : ERROR :: Couldn't find a proper spawn position for the rebels after "+ Attempts +" attempts! Rebel group was not spawned.");
				return;
			}
		}
		
		//add headtorch to rebel bots if its night time
		int year, month, day, hour, minute, SunRiseHour, SunSetHour;
		Mission currentMission;
		WorldData worldData;
		float ApproxSunrise;
		float ApproxSunset;
		vector lightPos;
		bool NightTime;
		
        //check darkness for light support and attach headtorch to rebel bot if needed            
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
                Print("[SMM] CaptureFlag Mission : Test Hour is "+ hour +", SunRiseHour: "+ SunRiseHour +", SunSetHour: "+ SunSetHour );
                
                if ( hour >= SunSetHour || hour <= SunRiseHour ) 
				NightTime = true;
				else
				NightTime = false;
            }
        }
		
		//Spawn Bohemian Rebels
		for (int i=0; i < BotCount; i++ )
		{
			//spawn RebelBot and set AI props
			RebelBot = eAIBase.Cast(g_Game.CreateObject( "eAI_SurvivorM_Peter", GroupSpawnPos ));
			RebelBot.SetPosition( GroupSpawnPos + Vector(i * 1.5, 0, 0) );
			RebelBot.PlaceOnSurface();
			RebelBot.SetMovementSpeedLimits(2.0, 3.0);
			
			if ( MissionSettings.DebugMode && !RebelBot )
			Print("[SMM] CaptureFlag Mission : ERROR :: RebelBot "+ i +" is null!");
			
			//RebelBot headwear
			if ( NightTime )
			{
				//attach headtorch for night time and switch on
				EntityAI headTorch;
				headTorch = RebelBot.GetInventory().CreateAttachment("HeadTorch_Grey");
				headTorch.GetInventory().CreateAttachment("Battery9v");
				headTorch.GetCompEM().SwitchOn();
            }
			else
			{
				//attach headwear for daytime
				TStringArray headwear = {"OKZKCap_Green","RadarCap_Green","BalaclavaMask_Green","BalaclavaMask_BDU","Balaclava3Holes_Green"};
				
				if ( i == 0 )
				{
					RebelBot.GetInventory().CreateAttachment("MilitaryBeret_Red");				//red beret for group leader
					RebelBot.GetInventory().CreateAttachment("AviatorGlasses");
				}
				else
				RebelBot.GetInventory().CreateAttachment( headwear.GetRandomElement() );		//military headwear for group members
			}
			
			//RebelBot body clothes
			//jacket
			if ( i == 0 )
			RebelBot.GetInventory().CreateAttachment("MilitarySweater_Chernarus");				//mil sweater for group leader
			else
			RebelBot.GetInventory().CreateAttachment("TTsKOJacket_Camo");
			//pants
			RebelBot.GetInventory().CreateAttachment("TTSKOPants");
			//boots
			RebelBot.GetInventory().CreateAttachment("TTSKOBoots");
			//gloves
			if ( i == 0 )
			RebelBot.GetInventory().CreateAttachment("WoolGlovesFingerless_Green");				//fingerless wool gloves for group leader
			else
			RebelBot.GetInventory().CreateAttachment("TacticalGloves_Black");
			
			//bags
			if ( i > 0 )
			RebelBot.GetInventory().CreateAttachment("DuffelBagSmall_Camo");
			
			//RebelBot main & melee weapon and mags
			EntityAI weapon;
			if ( i == 0 )
			{
				weapon = RebelBot.GetInventory().CreateInInventory("CZ61");
				//weapon.GetInventory().CreateAttachment("PistolSuppressor");
				RebelBot.GetInventory().CreateAttachment("Machete");
				RebelBot.GetInventory().CreateInInventory("Mag_CZ61_20Rnd");
				RebelBot.GetInventory().CreateInInventory("Mag_CZ61_20Rnd");
				RebelBot.GetInventory().CreateInInventory("Mag_CZ61_20Rnd");
			}
			else
			{
				weapon = RebelBot.GetInventory().CreateInInventory("Mp133Shotgun");
				//weapon.GetInventory().CreateAttachment("");
				RebelBot.GetInventory().CreateAttachment("Machete");
				RebelBot.GetInventory().CreateInInventory("Ammo_12gaPellets");
				RebelBot.GetInventory().CreateInInventory("Ammo_12gaPellets");
				RebelBot.GetInventory().CreateInInventory("Ammo_12gaPellets");			
			}
						 
			//set AI behavior and props
			RebelBot.eAI_SetThreatDistanceLimit(40);
			RebelBot.eAI_SetLootingBehavior(0);
			RebelBot.eAI_SetUnlimitedReload(true);
			
			//Add this bot to MissionAI's
			m_MissionAIs.Insert( RebelBot );
			GroupPosBuffer.Insert( RebelBot.GetPosition() );
									
			//Set first bot as leader of group
			if ( i == 0 )
			{
				//Get MissionGroup and ensure RebelBot is a member of the group
				BohemianRebels = RebelBot.GetGroup();
								
				if (!BohemianRebels.IsMember(RebelBot))
				BohemianRebels.AddMember(RebelBot);
				
				RebelBot.SetGroup(BohemianRebels);
				
				//Set group faction for counter attack
				eAIFaction West = new eAIFactionWest();
				BohemianRebels.SetFaction( West );
				
				BohemianRebels.SetLeader(RebelBot);
				Print("[SMM] CaptureFlag Mission : AI RebelBot_"+ i +" was added to the Bohemian Rebels and set as group leader.");
			}
			else
			{
				if (!BohemianRebels.IsMember(RebelBot))
				{
					BohemianRebels.AddMember(RebelBot);
					RebelBot.SetGroup( BohemianRebels );
					Print("[SMM] CaptureFlag Mission : AI RebelBot_"+ i +" was added to the Bohemian Rebels.");
				} 
			}
			

			if ( MissionSettings.DebugMode )
			{
				string aigroupfaction = RebelBot.GetGroup().GetFaction().GetName();
				
				Print("[SMM] CaptureFlag Mission : AI faction is: "+ aigroupfaction );
				Print("[SMM] CaptureFlag Mission : Actual group count is " + BohemianRebels.Count() );
			}
		}
				
		//Set mission position as target counter attack position
		BohemianRebels.ClearWaypoints();
		
		
		vector AttackPos = m_MissionPosition + "1 0 1";
		AttackPos[1] = g_Game.SurfaceY( AttackPos[0], AttackPos[2] );
		BohemianRebels.AddWaypoint( AttackPos );
		BohemianRebels.SetWaypointBehaviour( eAIWaypointBehavior.LOOP );
		//BohemianRebels.GetLeader().SetPosition( BohemianRebels.GetLeader().GetPosition() + "0 0.5 0" );
		g_Game.CreateObjectEx("ZmbF_JournalistNormal_White", GroupSpawnPos + "-2 0 -2", ECE_PLACE_ON_SURFACE | ECE_INITAI );
		g_Game.CreateObjectEx("ZmbF_JournalistNormal_White", GroupSpawnPos + "-4 0 -4", ECE_PLACE_ON_SURFACE | ECE_INITAI );
		
		Print("[SMM] CaptureFlag Mission : Bohemian Rebels spawned at : "+ GroupSpawnPos.ToString() +" and are on their way to the flag." );
#endif
#endif		
	}
	
	override void ObjDespawn() 
	{	
		//nothing to despawn
	}
	
	override void ExtendMission()
	{
		//select secondary mission
		m_MissionExtended = false;

		//Translate mission location name and direction
		if ( MissionSettings.Opt_TranslateEWData )
		{
			m_MissionLocation = EventsWorldData.TransLoc( m_MissionLocation );
			m_MissionLocationDir = EventsWorldData.TransDir( m_MissionLocationDir );
		}
				
		//Set new mission messages
		m_MissionMessage1 = "Allright survivor, it seems you have found the defence position "+ m_MissionLocationDir +" of "+ m_MissionLocation +". There are probably alot of infected, try to get in there safely and hold the position.";
		m_MissionMessage2 = "Be aware, the Bohemian Rebels could still counter attack the defence position. If you have captured it completely, I am sure there is some interesting equipment for you on the tables at the flag.";
		m_MissionMessage3 = "The flag was already captured by "+ FlagCaptureProgress +"%. Hold the position until our flag was hoisted!";
		//Add GPS coordinates of mission in message
		if ( MissionSettings.Opt_ShowGpsInfo ) m_MissionMessage1 += "\n\nGPS coordinates:"+ m_MissionInstance.GetGPS();
				
		//init Messenger for new messages
		m_MsgNum = 1;					//skip msg 0, begin with msg 1
		m_MsgChkTime = m_MissionTime;
		MsgCutoff = false;
		
		//increase mission timeout
		m_MissionTimeout = m_MissionTime + CaptureTime + 900;  
		
		//deployment settings & init for extended mission
		m_MissionZoneOuterRadius = 20.0;
		m_MissionZoneInnerRadius = 0.0001;		//not reachable... to hold the mission unfinished until flag was hoisted completely
	}
			
	override void MissionFinal()
	{	//When player enters last mission target zone
						
		//Finish mission
		m_RewardsSpawned = true;
		m_MsgNum = -1;
		m_MsgChkTime = m_MissionTime + MsgDlyFinish;			
	}
	
	override void PlayerChecks( PlayerBase player )
	{	//Executed continuously only on players in the mission zone. Avoid complex calculations or big data processing!
		//do nothing
	}
	
	override void EntityChecks()
	{	//Executed continuously when mission was actived. Avoid complex calculations or big data processing!
		
		int crntTime = g_Game.GetTime();
		
		//flag movement & capture progress calculation
		if ( !m_MissionExtended && !CaptureFinished && FlagBase && Flag && m_PlayersInZone.Count() > 0 )
		{			
			if ( !FlagElevationChkTime ) FlagElevationChkTime = crntTime;
			
			if ( crntTime >= FlagElevationChkTime + 100 )	//checking every 100ms
			{
				vector FlagPos = Flag.GetPosition();
				vector OldFlagPos;
				vector OldFlagOri;
				float flag_height = FlagPos[1];
			
				//check if flag is at lowest position
				if ( flag_height < PoleLowPos[1] && flag_speed < 0 ) 
				{
					flag_speed *= -1;
					
					//replace flag
					OldFlagPos = FlagPos;
					OldFlagOri = Flag.GetOrientation();
					
					g_Game.ObjectDelete( Flag );
					
					Flag = Flag_Base.Cast( g_Game.CreateObject( UpFlag, OldFlagPos ));
					Flag.SetPosition( OldFlagPos );
					Flag.SetOrientation( OldFlagOri );
					Flag.ShowSelection("unfolded");
					Flag.HideSelection("folded");
					Flag.SetTakeable(false);					
				}
				
				//check if flag is at highest position
				if ( flag_height > PoleHighPos[1] && flag_speed > 0 ) 
				{
					flag_speed *= -1;
					
					//check if flag has desired type
					if ( Flag.GetType() == UpFlag )
					{
						CaptureFinished = true;
						
						TeamPlayerCount = m_PlayersInZone.Count();
						if ( TeamPlayerCount > 3 ) TeamPlayerCount = 3;
						
						//Log assisting players
						PlayerBase MissionPlayer;
						
						g_Game.GetObjectsAtPosition3D( m_MissionPosition, 20, m_ObjectList, m_ObjectCargoList );
						
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
							Print("[SMM] CaptureFlag Mission : Player with SteamID64: "+ IdentPlainID +" deserves respect for his "+ ExtMsg +".");
							//send Respect message to player
							NotificationSystem.SendNotificationToPlayerIdentityExtended( Ident , 15.0, RespectMsg, "", MsgIcon );
						}
								
						//spawn rewards & finish mission
						SpawnRewards();
						MissionFinal();
					}
				}
				
				//update flag position
				Flag.SetPosition( FlagPos + Vector(0,flag_speed,0) );
				
				FlagElevationChkTime = crntTime;
			}

			//flag capture progress 0-100% (only for the progress in the message)
			if ( FlagPos[1] - PoleLowPos[1] > 0 && FlagPos[1] - PoleHighPos[1] < 0 )
			{
				if ( flag_speed < 0 ) 	//when lowering flag
				FlagCaptureProgress = 50 + ( FlagPos[1] - PoleLowPos[1] ) / ( -delta_height * 2 / 100 ); 
				else					//when raising flag
				FlagCaptureProgress = 50 + ( FlagPos[1] - PoleLowPos[1] ) / ( delta_height * 2 / 100 );
			}
			
			//refresh message 3 (capture progress)
			if ( !m_MissionExtended )
			m_MissionMessage3 = "The flag was already captured by "+ FlagCaptureProgress +"%. Hold the position until our flag was hoisted!";
			
			//Spawn RebelGroup if ExpansionAI mod is existing and enabled in settings and mission
			if ( FlagCaptureProgress >= CAPercentage && !RebelsGroupSpawned && m_EAIenabled && MissionSettings.Opt_AddMissionBots && EnableBots )
			{
				AICounterAttack();
				RebelsGroupSpawned = true;
			}
		}
		
		
		if ( !CaptureFinished )
		{
			//refuel fireplaces
			if ( !FireplaceSpendChkTime ) FireplaceSpendChkTime = crntTime;
			
			if ( crntTime >= FireplaceSpendChkTime + 300000 )	//checking every 5min
			{
				for ( int i=0; i < m_MissionObjects.Count(); i++)
				{
					Object AnyObject = m_MissionObjects.Get(i);
					
					if ( AnyObject && AnyObject.IsFireplace() )
					{
						FireplaceBase Campfire = FireplaceBase.Cast( AnyObject );
						if ( Campfire )
						{
							for (int j=0; j < Campfire.GetInventory().AttachmentCount(); ++j )
							{
								ItemBase attachment = ItemBase.Cast( Campfire.GetInventory().GetAttachmentFromIndex(j) );
								if ( attachment && attachment.GetType() == "Firewood" )
								{
									if ( attachment.GetQuantity() < 4 )		attachment.SetQuantityMax();
								}
							}												
						}
					}
				}
				
				FireplaceSpendChkTime = crntTime;
			}
			
#ifdef ENFUSION_AI_PROJECT
#ifdef EXPANSIONMODAI			
			//bot's stuck check (a workaround for the actual Expansion AI bug with initialization and update if AI is self created, on TODO list and possible fix after new ExpAI 1.29 upd)
			if ( !m_MissionExtended && BotStuckCounter < 40 && RebelsGroupSpawned && m_EAIenabled && MissionSettings.Opt_AddMissionBots && EnableBots )
			{
				if ( crntTime > BotStuckChkTime + 10000 )	//check every 5 sec
				{					
					for ( int n=0; n < GroupPosBuffer.Count(); n++ )
					{
						eAIBase GroupMember = eAIBase.Cast( BohemianRebels.GetMember(n) );
						if ( GroupMember && GroupMember.IsAlive() && !GroupMember.IsUnconsciousStateOnly() )
						{
							vector ActGrpMemPos = GroupMember.GetPosition();
							
							if ( vector.Distance( ActGrpMemPos, GroupPosBuffer.Get(n) ) < 0.1 && vector.Distance( m_MissionPosition , ActGrpMemPos ) > 30 )
							{
								Print("[SMM] CaptureFlag Mission : BohemianRebels GroupMember_"+ n +" , position : "+ ActGrpMemPos.ToString() +", Bot stuck detected. Spawning a threat nearby.");
								g_Game.CreateObjectEx("ZmbF_CitizenBSkinny", ActGrpMemPos + "2 0 2", ECE_PLACE_ON_SURFACE | ECE_INITAI );
								BotStuckCounter++;
/*								
								//get a random player in mission zone  ...does not fix the issue!
								g_Game.GetObjectsAtPosition3D( m_MissionPosition, m_MissionZoneOuterRadius , m_ObjectList, m_ObjectCargoList );
						
								for ( int p=0; p < m_ObjectList.Count(); p++)
								{
									Object FoundPlayer = m_ObjectList.Get(p);
									if ( FoundPlayer.IsMan() )
									{
										eAITargetInformation GM_TargetInfo = DayZPlayerImplement.Cast( FoundPlayer ).GetTargetInformation();
										GroupMember.eAI_GetTargetInformationState( GM_TargetInfo );
										break;
									}
								}
*/								
							}
							
							GroupPosBuffer[n] = ActGrpMemPos;
						}	
					}
					
					BotStuckChkTime = crntTime;
				}
			}
#endif
#endif					
		}
	}
				
	override bool DeployMission()
	{	//When first player enters the mission zone (primary/secondary)
		if ( m_MissionPosition )
		{
			if ( m_MissionExtended )
			{	//only if primary is selected
				//Call spawners	
				g_Game.GetCallQueue( CALL_CATEGORY_GAMEPLAY ).Call( this.SpawnObjects );
				g_Game.GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( this.SpawnAIs, 5000, false );
			}
			return true;
		}
		else 
		{
			Print("[SMM] CaptureFlag Mission : ERROR :: Mission position was rejected or doesn't exist. MissionPosition is NULL!");
			return false;
		}
	}
}

