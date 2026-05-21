class HeadHuntMission extends SideMissions		//by Xellose and Funkdoc
{	
	//Mission related entities 
	Object MissionBuilding;
	Object MissionTarget;
	Object MissionNotice;
	Object MissionLight;
	ItemBase MissionObject;
	ItemBase TargetSmoke;
	EffecterBase TargetSmokeEffect;
		
	//Mission parameters
	int MsgDlyFinish = 60;								//seconds, message delay time after player has finished mission
	string MissionTargetType = "ScientificBriefcase";	//class name of desired mission target object (documents), default: ScientificBriefcase
	string TargetDepositType = "SmallProtectorCase"; 	//class name of desired mission target deposit, default: SmallProtectorCase
	
	//Mission containers
	ref array<vector> ExtendedPosList = new array<vector>;	
	ref array<ref Param4<string,string,string,vector>> Secondaries = new array<ref Param4<string,string,string,vector>>;	
	ref array<vector> Spawnpoints = new array<vector>;
	ref array<vector> InfectedSpawns = new array<vector>;
	ref array<string> InfectedTypes = new array<string>;
	Param4<string,string,string,vector> SecBuildInfo;
	ref array<PlayerBase> AssistingPlayers = new array<PlayerBase>;
		
	//Mission variables
	bool TargetWasTaken, MapGridYnorthAscending;
	vector TargetPos;
	float XX, YY;
	int SmokeCHKtime=3000, PosUpdTime=1000, crntTime;
	int SmokeEffecterID;
				
	string SurvivorName, StoryName, SurvivorID, SurvivorCompDir, TargetCoords;
	float SurvivorDir, SurvivorDist;
	
	string SecondaryLoc, SecondaryBuild, SecondaryBldClass, SecondaryBldMsgName;

	vector SecondaryBldPos, SecondarySpawn, SecondaryBldSpawn;
	
	bool DocsFoundAtPrimary = false;
	bool PlayerNearSecondary = false;
	
	//Settings buffer vars
	int MsgWaitTimeRst;

	override bool IsExtended() { return true; };
	
	void HeadHuntMission()
	{		
		//Select primary mission
		m_MissionExtended = true;
		m_TargetPlayer = null;
		
		//Mission timeout
		m_MissionTimeout = 2700;			//seconds, primary mission max. duration
		m_ExtendedTimout = 3600;			//seconds, mission max. duration for extended mission
		
		//Mission zones
		m_MissionZoneOuterRadius = 80.0;	//meters (!Do not set lower than 200m), mission activation distance
		m_MissionZoneInnerRadius = 0.001;		//meters (!Do not set outside of 1-5m), mission finishing distance to target object
				
		//Mission informant
		m_MissionInformant = "Dr. Legasov";
	
		//Mission person names
		TStringArray SurvivorNames = {"Yuri","Michail","Boris","Valeri","Anatoli","Ivan","Alexej","Dimitrij","Sergej","Nikolai","Vladimir"};
		StoryName = SurvivorNames.GetRandomElement();
		
		//Translate mission location name and direction
		if ( MissionSettings.Opt_TranslateEWData )
		{
			m_MissionLocation = EventsWorldData.TransLoc( m_MissionLocation );
			m_MissionLocationDir = EventsWorldData.TransDir( m_MissionLocationDir );
		}
		
		//Set mission messages for primary mission
		m_MissionMessage1 = StoryName +", a chemist who was working at the "+ m_MissionLocation +"'s Hospital, told me that they were developing Anti-Gen's from the virus a few months ago. They made several samples but the hospital was looted by the Soviet Decontamination Forces.";
		m_MissionMessage2 = "They probably have stolen all the Anti-Gen samples but they didnt know about "+ StoryName +"'s documents case he left behind anywhere on his desk in the hospital office. Hopefully we could develope new Anti-Gen's again once we have these important informations in our hands.";
		m_MissionMessage3 = "Search for a silver metal case in the \n**"+ m_MissionLocation +"'s Hospital**\nHe has left a notice on the case where to deliver it. I can't tell you about the destination on the radio... it's too unsafe on this channel. Take care, I believe the Soviet Forces already know about the documents.";
		//Add GPS coordinates of mission in message
		if ( MissionSettings.Opt_ShowGpsInfo ) m_MissionMessage3 += "\n\nGPS coordinates:"+ m_MissionInstance.GetGPS();
		
		//set map grid Y coords dir (Y is ascending in north dir?)
		TStringArray YNorthAscMaps = {"enoch"};
		
		if ( YNorthAscMaps.Find( g_Game.GetWorldName() ) > -1 )
		MapGridYnorthAscending = true;
		else MapGridYnorthAscending = false;
								
		//Spawnpoints for MissionTarget in hospital
		Spawnpoints.Insert("-21.12 -6.64 1.14");
		Spawnpoints.Insert("-20.95 -6.64 5.63");
		Spawnpoints.Insert("-15.88 -6.64 5.71");
		Spawnpoints.Insert("-10.82 -6.64 8.60");
		Spawnpoints.Insert("-9.14 -6.4 8.33");
		Spawnpoints.Insert("-4.83 -6.64 8.54");
						
		//Infected spawnpoints for primary mission at hospital
		InfectedSpawns.Insert("-6.3779 -7.3111 1.6016");
		InfectedSpawns.Insert("-12.0635 -7.3111 -2.3115");
		InfectedSpawns.Insert("0.5469 -7.3111 -1.01953");
		InfectedSpawns.Insert("-2.1709 -7.3111 -0.9775");
		InfectedSpawns.Insert("-16.0303 -7.3111 -2.0146");
		InfectedSpawns.Insert("17.8955 -7.3067 4.6143");
		InfectedSpawns.Insert("17.6504 -3.5781 -0.2275");
		//...outside MissionBuilding 
		InfectedSpawns.Insert("-17.0156 -7.6364 -9.7158");
		InfectedSpawns.Insert("-7.4268 -7.6066 -11.7314");
		InfectedSpawns.Insert("0.8662 -7.6027 -9.7480");
		InfectedSpawns.Insert("10.6084 -7.6134 -8.6611");
		InfectedSpawns.Insert("18.0771 -7.5431 -9.1602");
				
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

		//Secondary building table for classname, message callname of building and modelvector of MissionObject spawn point
		//only translate parameter 3 (eg. "hospital") into your language
		Secondaries.Insert( new Param4<string,string,string,vector>( "Hospital","Land_City_Hospital", 		"hospital", 				"-15.291 -6.639 5.189"));
		Secondaries.Insert( new Param4<string,string,string,vector>( "Police","Land_Village_PoliceStation", "small police station", 	"-9.226 -5.323 0.939"));
		Secondaries.Insert( new Param4<string,string,string,vector>( "CityPolice","Land_City_PoliceStation", "police department", 		"8.562 -1.657 -2.336"));
		Secondaries.Insert( new Param4<string,string,string,vector>( "FireStation","Land_City_FireStation", "fire department", 			"-9.547 6.477 10.248"));
		Secondaries.Insert( new Param4<string,string,string,vector>( "Church22","Land_Church2_2", 	"church", 							"7.762 -9.920 1.218"));
		Secondaries.Insert( new Param4<string,string,string,vector>( "Church3","Land_Church3", 	"church", 								"9.372 -12.749 -0.033"));
		Secondaries.Insert( new Param4<string,string,string,vector>( "Office1","Land_Office1", "small office building",					"-4.295 -1.433 5.451"));
		Secondaries.Insert( new Param4<string,string,string,vector>( "Office2","Land_Office2", "big office building", 					"4.882 -2.150 6.541"));
		Secondaries.Insert( new Param4<string,string,string,vector>( "MedCenter","Land_Village_HealthCare", "medical center", 			"3.126 -2.262 3.482"));
		Secondaries.Insert( new Param4<string,string,string,vector>( "TrainStop","Land_Rail_Station_Small", "train stop", 				"1.426 -0.425 -3.505"));
		Secondaries.Insert( new Param4<string,string,string,vector>( "RailStation","Land_Rail_Station_Big", "rail station", 			"1.514 -4.052 1.041"));
						
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
			SecondaryBuild = SecondaryDescription[1];
		}
		else
		{
			for ( int i = 0; i < DescriptionCount; i++)
			{
				if ( i < DescriptionCount - 2 ) SecondaryLoc += SecondaryDescription.Get(i) + "_";
				else if ( i < DescriptionCount - 1 ) SecondaryLoc += SecondaryDescription.Get(i);
				else if ( i < DescriptionCount ) break;
			}
			
			SecondaryBuild = SecondaryDescription[DescriptionCount - 1];
		}
						
		//get secondary building data
		bool BldFound = false;
		
		for ( int k=0; k < Secondaries.Count(); k++)
		{
			SecBuildInfo = Secondaries.Get( k );
			
			if ( SecBuildInfo.param1 == SecondaryBuild )
			{
				SecondaryBldClass = SecBuildInfo.param2;
				SecondaryBldMsgName = SecBuildInfo.param3;
				SecondaryBldSpawn = SecBuildInfo.param4;
				BldFound = true;
				break;			
			}	
		}
		if ( !BldFound ) Print("[SMM] HeadHunt Mission : MissionBuilding couldn't be found. Check EWD parameter. Mission isn't accomplishable and times out regularely!");
		
		//Search for mission building at primary mission position	
		g_Game.GetObjectsAtPosition( m_MissionPosition , 1.0 , m_ObjectList , m_ObjectCargoList );
		for ( int j = 0 ; j < m_ObjectList.Count(); j++ )
		{ 
			Object FoundObject = m_ObjectList.Get(j);
			if ( FoundObject.GetType() == "Land_City_Hospital" )
			{	
				MissionBuilding = FoundObject;			 
				Print("[SMM] HeadHunt Mission : MissionBuilding is "+ MissionBuilding.GetType() +" at "+ m_MissionDescription[2] +" of "+ m_MissionDescription[1] +" @ "+ MissionBuilding.GetPosition() );
				break;
			}	
		}
		if ( !MissionBuilding ) Print("[SMM] HeadHunt Mission : Hospital couldn't be found. Mission isn't accomplishable and times out regularely!");
		
		//predictively search for mission building at secondary mission position and get random spawn point for MissionObject yellow protector case in building 
		if ( EventsWorldData.GetBuildingsAtLoc( SecondaryBldClass, SecondaryLoc, ExtendedPosList ))
		{
			SecondaryBldPos = ExtendedPosList.GetRandomElement();
			g_Game.GetObjectsAtPosition( SecondaryBldPos, 1.0 , m_ObjectList , m_ObjectCargoList );
			for ( int m=0; m < m_ObjectList.Count(); m++ )
			{ 
				FoundObject = m_ObjectList.Get(m);
				if ( FoundObject.GetType() == SecondaryBldClass )
				{	
					SecondarySpawn = FoundObject.ModelToWorld( SecondaryBldSpawn );
					Print("[SMM] HeadHunt Mission : Predictive Building Check: Found secondary side mission building "+ SecondaryBldClass +" in "+ SecondaryLoc +".");
					SecondaryLoc.Replace("_"," ");	 
					break;
				}	
			}
		}
		else Print("[SMM] HeadHunt Mission : Can't get secondary MissionPosition in "+ SecondaryLoc +" from EventsWorldData. Check EventsWorldData.GetBuildingsAtLoc() and the passthrough vars in the HeadHuntMission constructor!");
	
	}
	
	void ~HeadHuntMission()
	{		
		//Despawn all remaining mission objects
		if ( m_MissionObjects )
		{
			Print("[SMM] HeadHunt Mission : Despawning "+ m_MissionObjects.Count() +" mission objects from old side mission...");		
			for ( int i = 0; i < m_MissionObjects.Count(); i++ )
			{				
				if ( !m_MissionObjects.Get(i))	continue;
				else g_Game.ObjectDelete( m_MissionObjects.Get(i) );			
			}
			m_MissionObjects.Clear();
		}
		
		if ( MissionTarget )
			g_Game.ObjectDelete( MissionTarget );
		
		if ( MissionLight )
			g_Game.ObjectDelete( MissionLight );
				
		//Delete mission AI's
		if ( m_MissionAIs )
		{
			if ( m_MissionAIs.Count() > 0 )
			{
				Print("[SMM] HeadHunt Mission : Despawning "+ m_MissionAIs.Count() +" mission AI's from old side mission...");
				for ( int k = 0; k < m_MissionAIs.Count(); k++ )
				{
					g_Game.ObjectDelete( m_MissionAIs.Get(k) );
				}
				m_MissionAIs.Clear();			
			}
			else Print("[SMM] HeadHunt Mission : No mission AI's to despawn.");
		}
		
		//Delete PlayersInZone list
		if ( m_PlayersInZone )	m_PlayersInZone.Clear();
		
		//Reset changed settings by mission 
		if ( MsgWaitTimeRst )	MissionSettings.MsgWaitTime = MsgWaitTimeRst;
		
		//Reset target player and release var for garbage collector
		if ( m_TargetPlayer )	m_TargetPlayer = null;		
	}
		
	void SpawnRewards()
	{		
		//new MissionObject after deleting protector case
		MissionObject = ItemBase.Cast( g_Game.CreateObjectEx( "MissionChest", m_MissionPosition, ECE_PLACE_ON_SURFACE ));
		MissionObject.SetTakeable(false);		
		//Get random loadout 			
		int selectedLoadout = Math.RandomIntInclusive(0,11);	//!Change randomization limit after adding new loadouts!
		
		//Spawn selected loadout items in mission object
		EntityAI weapon;
								
		if (selectedLoadout == 0)
		{
			weapon = MissionObject.GetInventory().CreateInInventory("M4A1_Green");
				weapon.GetInventory().CreateAttachment("M4_RISHndgrd_Green");
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
				weapon.GetInventory().CreateAttachment("AK_RailHndgrd_Green");
				weapon.GetInventory().CreateAttachment("AK_Suppressor");
				weapon.GetInventory().CreateAttachment("AK_PlasticBttstck_Green");
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
		
		m_MissionObjects.Insert( MissionObject );

		Object rewardChest = MissionObject;
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(g_Game.ObjectDelete, 300000, false, rewardChest);

		Print("[SMM] HeadHunt Mission : Rewards spawned in reward container. Randomly selected loadout was "+selectedLoadout+"." );			
	}
	
	void SpawnObjects()
	{	
		Object FoundObject;
		
		if ( m_MissionExtended )	//primary mission is set, spawn regarding objects on primary mission position
		{	
			//Get spawnpoint for MissionTarget
			m_MissionPosition = MissionBuilding.ModelToWorld( Spawnpoints.GetRandomElement());
	 		
			//Clean up MissionTarget spawnpoint
			g_Game.GetObjectsAtPosition( m_MissionPosition, 1.5, m_ObjectList, m_ObjectCargoList );
			
			if ( m_ObjectList.Count() > 0)
			{
				for ( int i=0; i < m_ObjectList.Count(); i++)
				{
					FoundObject = m_ObjectList.Get(i);
					if ( FoundObject.IsItemBase() )
					{
						Print("[SMM] HeadHunt Mission : BeforeSpawnCleanUp :: Object  " + FoundObject.ToString() + " at new HeadHunt mission position was deleted.");
						g_Game.ObjectDelete( FoundObject );
					}
				}
			}
			
			//spawn mission target at hospital and place the notice on it 
			MissionTarget = g_Game.CreateObjectEx( MissionTargetType, m_MissionPosition, ECE_PLACE_ON_SURFACE|ECE_ROTATIONFLAGS, RF_FRONT );
			EntityAI.Cast( MissionTarget ).SetLifetime(3600);
			MissionNotice = g_Game.CreateObjectEx( "Paper", MissionTarget.ModelToWorld("0 0.2 0.07"), ECE_KEEPHEIGHT, RF_DEFAULT );
			ItemBase.Cast( MissionNotice ).SetTakeable( false );
			
			Print("[SMM] HeadHunt Mission : "+ m_selectedMission +" :: "+ m_MissionName +" ...primary mission deployed!");
		}
		else					//secondary mission is set, spawn regarding objects in or around secondary mission building or referenced object 
		{
			//Clean up MissionTarget spawnpoint
			g_Game.GetObjectsAtPosition( SecondarySpawn, 1.5, m_ObjectList, m_ObjectCargoList );
			
			if ( m_ObjectList.Count() > 0)
			{
				for ( i=0; i < m_ObjectList.Count(); i++)
				{
					FoundObject = m_ObjectList.Get(i);
					if ( FoundObject.IsItemBase() )
					{
						Print("[SMM] HeadHunt Mission : BeforeSpawnCleanUp :: Object  " + FoundObject.ToString() + " at new side mission position was deleted.");
						g_Game.ObjectDelete( FoundObject );
					}
				}
			}
			
			//spawn mission object (protector case) in secondary building
			MissionObject = ItemBase.Cast( g_Game.CreateObjectEx( TargetDepositType, SecondarySpawn , ECE_PLACE_ON_SURFACE ));
			MissionObject.SetLifetime(1800);
			MissionObject.SetTakeable(false);
			m_MissionObjects.Insert( MissionObject );
			
			Print("[SMM] HeadHunt Mission : Side Mission Extension "+ m_selectedMission +" :: "+ m_MissionName +" ...secondary mission deployed!");
		}
	}
	
	void SpawnAIs()
	{	
		if ( m_MissionExtended )
		{
			//Spawn infected at hospital
			string RandomInfected = InfectedTypes.GetRandomElement();
			vector InfectedPos;
			
			for ( int k = 0 ; k < InfectedSpawns.Count() ; k++ )
			{
	    	   	RandomInfected = InfectedTypes.GetRandomElement();
				InfectedPos = MissionBuilding.ModelToWorld( InfectedSpawns.Get(k) );
				m_MissionAIs.Insert( g_Game.CreateObject( RandomInfected, InfectedPos, false, true ) );
			}
		}
		else
		{
			//Spawn no AIs at secondary mission
		}
	}
	
	override void ObjDespawn() 
	{	
		//do nothing
	}
	
	override void ExtendMission()
	{	//When player enters mission target zone at primary mission

		//holding mission primary 
		m_MsgChkTime = m_MissionTime + 19;
		m_MsgNum = 0;
		
		//set MissionPosition to spawn point of secondary MissionBuilding 
		m_MissionPosition = SecondarySpawn;
		
		//set mission zones for secondary building
		m_MissionZoneOuterRadius = 400;
		m_MissionZoneInnerRadius = 2.5;
		
		//translate secondary location
		if ( MissionSettings.Opt_TranslateEWData )
		{
			SecondaryLoc = EventsWorldData.TransLoc( SecondaryLoc );
		}		
	}
	
	override void MissionFinal()
	{	//When player enters last mission target
		//do nothing	
	}
	
	void MessageTargetPlayer( PlayerBase player )
	{
		//translate secondary location
		if ( MissionSettings.Opt_TranslateEWData )
		{
			m_MissionSecondaryLoc = EventsWorldData.TransLoc( m_MissionSecondaryLoc );
		}
		
		//translate SecondarySpawn coords		
	    XX = SecondarySpawn[0] / 10000;
	    if ( !MapGridYnorthAscending )
	    YY = ( EventsWorldData.EWD_MAP_SIZE - SecondarySpawn[2] ) / 10000;
		else
		YY = SecondarySpawn[2] / 10000;
		
		TargetCoords = " X "+ ( XX.ToString().Substring(0,4) ) +" / Y "+ ( YY.ToString().Substring(0,4) ) +" ";
						
		string icon = "set:dayz_gui image:gear";
		string message1 = "Dear "+ SurvivorName +",\nthese documents in the case are really important. It has information about new methods to gain Anti-Gen's from the virus. It seems we can produce vaccine now. It has the highest priority.";
		string message2 = "\nMeet Legasov in the **"+ SecondaryBldMsgName +" of "+ m_MissionSecondaryLoc +"** as fast as you can. There is a yellow protector case in the building. Just go there and place the case on this desk! Good luck, stay safe and trust nobody!";
		//Add GPS coordinates of mission in message
		if ( MissionSettings.Opt_ShowGpsInfo ) message2 += "\n\nGPS coordinates: "+ TargetCoords;
						
		NotificationSystem.SendNotificationToPlayerIdentityExtended( player.GetIdentity() , 30.0, "Secret Documents:", message1 + message2, icon );
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
					//if ( !TargetSmokeEffect )
					//{
						ParticleEffecterParameters param = new ParticleEffecterParameters( "ParticleEffecter", EffecterBase.NOT_DEFINED_LIFESPAN, ParticleList.GRENADE_M18_YELLOW_LOOP );
						SmokeEffecterID = SEffectManager.CreateParticleServer( TargetPos, param );
					//}
				}
			}
		}
						
		SmokeCHKtime = crntTime;
	}
	
	void DeleteMarker()
	{
		g_Game.ObjectDelete( MissionLight );
		g_Game.ObjectDelete( TargetSmokeEffect );
		MissionLight = null;
		TargetSmokeEffect = null;
		SEffectManager.DestroyEffecterParticleServer( SmokeEffecterID );
	}
		
	override void PlayerChecks( PlayerBase player )
	{	//Executed continuously only on players in the mission zone. Avoid complex calculations or big data processing!		
		//check if mission target exists
		if ( MissionTarget )
		{
			//check if current player has mission target
			if ( player.GetInventory().HasEntityInInventory( EntityAI.Cast( MissionTarget )))
			{
				if ( !PlayerNearSecondary ) m_TargetPlayer = player;
				else TargetPos = player.GetPosition();
				
				//update mission target position = player pos
				//TargetPos = player.GetPosition();
				//if ( m_MissionExtended ) m_MissionPosition = TargetPos;
							
				//log player who is carrying mission target
				SurvivorID = player.GetIdentity().GetPlainId();
				SurvivorName = player.GetIdentity().GetName(); // GetFullName();
			
				//if player picked up target recently
				if ( !TargetWasTaken )
				{
					TargetWasTaken = true;
					
					if ( DocsFoundAtPrimary ) 
					Print("[SMM] HeadHunt Mission : Player "+ SurvivorName +", SteamID64: "+ SurvivorID +" picked up the documents from ground.");						
					else 
					{
						Print("[SMM] HeadHunt Mission : Documents were taken at hospital by player "+ SurvivorName +", SteamID64: "+ SurvivorID );
						g_Game.ObjectDelete( MissionNotice );
						
						DocsFoundAtPrimary = true;
						//m_MissionTimeout = m_MissionTime + m_ExtendedTimout;
															
						//increase message loop speed, settings override
						MsgWaitTimeRst = MissionSettings.MsgWaitTime; //buffer settings
						MissionSettings.MsgWaitTime = 200; 			//override message intervall (restore default value when destructing class)
						
						//disable message cutoff
						MsgCutoff = false;
					}
		
					//init Messenger for hunting mode
					m_MsgChkTime = m_MissionTime + 19;
					m_MsgNum = 0;
											
					//show intel to player, information on the paper 
					MessageTargetPlayer( player );
					DeleteMarker();
				}
			}
		}		
									
		//Check if MissionTarget was brought to secondary MissionPosition 
		if ( MissionObject && MissionTarget && !m_MissionExtended ) //MissionObject.ClassName() == TargetDepositType &&
		{				
			if ( vector.Distance( player.GetPosition(), MissionObject.GetPosition() )  < 2 )
			{
				if ( player.GetInventory().HasEntityInInventory( EntityAI.Cast( MissionTarget )))
				{
					Print("[SMM] HeadHunt Mission : Player with SteamID64: "+ SurvivorID +" has successfully saved the documents from getting into wrong hands.");
	
					//delete MissionObjects first (SmallProtectorCase and ScientificBriefcase) 
					g_Game.ObjectDelete( MissionObject );
					MissionObject = null;
					g_Game.ObjectDelete( MissionTarget );
					MissionTarget = null;
					
					//spawn rewards 
					g_Game.GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( this.SpawnRewards, 1000 );
						
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
						Print("[SMM] HeadHunt Mission : Player with SteamID64: "+ IdentPlainID +" deserves respect for his "+ ExtMsg +".");
						//send Respect message to player
						NotificationSystem.SendNotificationToPlayerIdentityExtended( Ident , 15.0, RespectMsg, "", MsgIcon );
					}
										
					//finish mission
					m_RewardsSpawned = true;
					m_MissionInformant = "Dr. Legasov";				
					m_MsgNum = -1;
					m_MsgChkTime = m_MissionTime + MsgDlyFinish;
				}		
			}
		}		
	}
	
	override void EntityChecks()
	{	//Executed continuously when mission was actived. Avoid complex calculations or big data processing! 
		//if player left the server with mission target in inventory
		if ( DocsFoundAtPrimary && !m_RewardsSpawned )
		{
			if ( !MissionTarget )
			{
				//MissionTarget does not exist anymore, create new one on last known player position
				//... player left the server with mission target in inventory  => PLAYER LOGGED OUT WITH DOCUMENTS 
				if ( TargetWasTaken )
				{
					TargetWasTaken = false;					
					//m_MissionTargetFound = true;
					//m_ZoneIsTriggered = true;
					m_TargetPlayer = null;
					TargetPos += "3 0 3";
					m_MissionPosition = TargetPos;
					m_PlayersInZone.Clear();
					
					//respawn mission target
					MissionTarget = g_Game.CreateObjectEx( MissionTargetType, TargetPos, ECE_PLACE_ON_SURFACE|ECE_ROTATIONFLAGS, RF_TOP );
					m_MissionZoneOuterRadius = 150;
					m_MissionZoneInnerRadius = 0.001;
					
					if ( m_MissionExtended )
					{
						//toggle mission messages to find mode
						m_MissionInformant = "Dr. Legasov";
						
						//translate SecondarySpawn coords		
					    XX = TargetPos[0] / 10000;
					    if ( !MapGridYnorthAscending )
					    YY = ( EventsWorldData.EWD_MAP_SIZE - TargetPos[2] ) / 10000;
						else
						YY = TargetPos[2] / 10000;
						
						TargetCoords = " X "+ ( XX.ToString().Substring(0,4) ) +" / Y "+ ( YY.ToString().Substring(0,4) ) +" ";
									
						m_MissionMessage1 = "It seems a survivor has droped the documents case or died. Somebody reported the last known position of the survivor.";
						m_MissionMessage2 = "Try to get the documents before others will do. The metal case is anywhere around there and it's marked with yellow smoke. Read the notice, it contains the information where you have to bring it.";
						m_MissionMessage3 = "GPS coordinates: " + TargetCoords +"\n\nNo worries, I have the keys and if you could bring it to the mentioned location it can save many of lives.";
					}
					
					//mark documents with yellow smoke 
					MarkTargetSmoke();
					
					Print("[SMM] HeadHunt Mission : TargetPlayer has left the server with the documents in inventory. Respawning intel on ground and marking it with yellow smoke.");
				}
				else	//in case player died with documents in inventory (TargetWasTaken=false)
				{
					//hold mission primary					
					//m_MissionTargetFound = true;
					//m_ZoneIsTriggered = true;
					m_TargetPlayer = null;
					TargetPos += "3 0 3";
					m_MissionPosition = TargetPos;
					//m_PlayersInZone.Clear();
					
					//respawn mission target
					MissionTarget = g_Game.CreateObjectEx( MissionTargetType, TargetPos, ECE_PLACE_ON_SURFACE, RF_TOP );
					m_MissionZoneOuterRadius = 4.0;
					m_MissionZoneInnerRadius = 0.001;
						
					if ( m_MissionExtended )
					{
						//toggle mission messages to find mode	
						m_MissionInformant = "Major Stolcic";
						
						//translate SecondarySpawn coords		
					    XX = TargetPos[0] / 10000;
					    if ( !MapGridYnorthAscending )
					    YY = ( EventsWorldData.EWD_MAP_SIZE - TargetPos[2] ) / 10000;
						else
						YY = TargetPos[2] / 10000;
						
						TargetCoords = " X "+ ( XX.ToString().Substring(0,4) ) +" / Y "+ ( YY.ToString().Substring(0,4) ) +" ";
						
						m_MissionMessage1 = "I was told that the document's thief was shot. I have information about his last known position. Grab the documents and bring it to me immediately!";
						m_MissionMessage2 = "Watch out! There might be more of them collaborating with Legasov. The documents case is anywhere around his body and it's marked with yellow smoke.";
						m_MissionMessage3 = "If you bring it to me I guarantee you get alot of money, a warm meal and the respect from me and my troops.\n\nGPS coordinates: " + TargetCoords;
					}

					//mark documents with yellow smoke 				
					MarkTargetSmoke();
					
					Print("[SMM] HeadHunt Mission : TargetPlayer died with documents in inventory. Intel was respawned on ground and marked with yellow smoke.");
				}
			}
			else	//MissionTarget exists
			{
				//get server run time in ms for delayed check
				crntTime = g_Game.GetTime();
				
				//MissionTarget exists and Target was already taken, then the PlayerChecks() returns early on TargetPlayer
				if ( TargetWasTaken )
				{		//...means position update and other player related things must be done here with m_TargetPlayer!   => UPDATE DOCUMENTS/PLAYER POSITION
					if ( m_TargetPlayer )
					{
						if ( crntTime > PosUpdTime + 1000 )
						{
							PosUpdTime = crntTime;
							
							//check if TargetPlayer still has the MissionTarget in inventory
							if ( m_TargetPlayer.GetInventory().HasEntityInInventory( EntityAI.Cast( MissionTarget )))
							{
								//update MissionTarget position
								TargetPos = m_TargetPlayer.GetPosition();
								
								//check if player died with mission target in inventory
								if ( !m_TargetPlayer.IsAlive() )
								{
									Print("[SMM] HeadHunt Mission : Player "+ SurvivorName +" with SteamID64: "+ SurvivorID +" died with the documents in inventory. Deleting mission target for respawn.");
									
									g_Game.ObjectDelete( MissionTarget );
									//TargetPos += "1 0 1"; 
									TargetWasTaken = false;
									return;
								}
								
								//get target player distance to secondary
								SurvivorDist = vector.Distance( TargetPos, SecondarySpawn );
								
								//Secondary zone check ( ?? only precheck, the actual mission position is still target player pos)
								if ( SurvivorDist < m_MissionZoneOuterRadius + 20 && !PlayerNearSecondary ) 
								{
									if ( MissionSettings.DebugMode )
									Print("[SMM] HeadHunt Mission : Player gets close to secondary side mission zone. Player distance < 420m.");
									Print("[SMM] HeadHunt Mission : Extending mission...");
																		
									PlayerNearSecondary = true;
									
									//change member vars
									//new mission position is yellow protector case spawnpoint in secondary MissionBuilding
									//m_MissionPosition = SecondarySpawn;
									//switch from primary to secondary mission
									m_MissionExtended = false;
									m_ZoneIsTriggered = false;
									//m_MissionTargetFound = true;
									//reset TargetPlayer and remove all players from zone list
									m_TargetPlayer = null;
									m_PlayersInZone.Clear();
									MsgCutoff = false;
									ExtendMission();
									
									//Set messages for hunting mode near secondary mission building
									m_MissionInformant = "Major Stolcic";
									
									//translate secondary location
									if ( MissionSettings.Opt_TranslateEWData )
									m_MissionSecondaryLoc = EventsWorldData.TransLoc( m_MissionSecondaryLoc );
														
									m_MissionMessage1 = "The survivor with the stolen documents was recently spotted very close to\n** "+ m_MissionSecondaryLoc +" **\nStop him from reaching the meeting point!";
									m_MissionMessage2 = "These documents are the property of the Soviet Army. Do not read them, it's top secret. Just bring them back to me. I guarantee for your safety, the money and a warm meal.";
									m_MissionMessage3 = "The survivor is probably meeting Legasov in the\n** "+ SecondaryBldMsgName +" **\nHurry up and get the survivor before he hands out the documents.";
									
									//init Messenger for new messages
									m_MsgNum = 1;					//skip msg 0, begin with msg 1
									MsgCutoff = false;
														
									//deployment settings & init for secondary mission
									//m_MissionZoneOuterRadius = 400.0;		
									//m_MissionZoneInnerRadius = 0.001;			
								}
								
								//if primary in hunting/finding mode refresh player position description
								if ( m_MissionExtended )
								{
									//calc relative direction of player to secondary mission position 
									vector PosDiff = TargetPos - SecondarySpawn;				//offset player position to secondary
									float x = PosDiff[0];
									float y = PosDiff[2];
									float angle = Math.Atan2(x,y) * 180 / Math.PI ;	//arctan of pos. diff. and transformation from radians to degrees 
									
									if ( x > 0 && y > 0 )	SurvivorDir = angle;			//quadrant I ...0° + angle
									if ( x < 0 && y > 0 )	SurvivorDir = 360 + angle;		//quadrant II ...360° + (-angle)
									if ( x < 0 && y < 0 )	SurvivorDir = 360 + angle;		//quadrant III ...360° + (-angle)
									if ( x > 0 && y < 0 )	SurvivorDir = angle;			//quadrant IV ...0° + angle
									
									//get target player relative compass direction from secondary mission position
									//360° / 8 = 45° ( angular range of relative compass direction for messages) north = angular section from (360° - 22.5°) to ( 0° + 22.5°)= 337.5° to 22.5° 
									//...more exact directions can be calculated like 'northnortheast' by increasing the denominator
									if ( SurvivorDir > 337.5 && SurvivorDir <= 22.5 ) 	SurvivorCompDir = "north";
									if ( SurvivorDir > 22.5 && SurvivorDir <= 67.5 ) 	SurvivorCompDir = "northeast";
									if ( SurvivorDir > 67.5 && SurvivorDir <= 112.5 ) 	SurvivorCompDir = "east";
									if ( SurvivorDir > 112.5 && SurvivorDir <= 157.5 ) 	SurvivorCompDir = "southeast";
									if ( SurvivorDir > 157.5 && SurvivorDir <= 202.5 ) 	SurvivorCompDir = "south";
									if ( SurvivorDir > 202.5 && SurvivorDir <= 247.5 ) 	SurvivorCompDir = "southwest";
									if ( SurvivorDir > 247.5 && SurvivorDir <= 292.5 ) 	SurvivorCompDir = "west";
									if ( SurvivorDir > 292.5 && SurvivorDir <= 337.5 ) 	SurvivorCompDir = "northwest";
									
									//toggle mission messages to hunting mode
									m_MissionInformant = "Major Stolcic";
									
									//translate secondary location
									if ( MissionSettings.Opt_TranslateEWData )
									{
										m_MissionSecondaryLoc = EventsWorldData.TransLoc( m_MissionSecondaryLoc );
										SurvivorCompDir = EventsWorldData.TransDir( SurvivorCompDir );
									}					
									m_MissionMessage1 = "Someone has stolen the documents. I command all to shoot him on sight and to bring the documents back, immediatly! I pay 100.000 Rubles for it.";
									m_MissionMessage2 = "This person is probably collaborating with Legasov and he wants to give the information away to suspicious people. We don't tolerate that.";
									m_MissionMessage3 = "A few minutes ago he was spotted with the document case\n** "+ Math.Round(SurvivorDist).ToString() +"m "+ SurvivorCompDir +" of "+ m_MissionSecondaryLoc +" **";
								}
							}
							else	//MissionTarget still exists but is not in inventory of Targetplayer anymore => DOCUMENTS DROPPED
							{
								//update MissionTarget position
								TargetPos = MissionTarget.GetPosition();
										
								if ( vector.Distance( TargetPos, m_TargetPlayer.GetPosition() ) > 5 )
								{
									TargetWasTaken = false;
									//m_MissionTargetFound = true;
									//m_ZoneIsTriggered = 
									m_TargetPlayer = null;
									m_MissionPosition = TargetPos;
									
									m_MissionZoneOuterRadius = 4;
									m_MissionZoneInnerRadius = 0.001;
									
									if ( m_MissionExtended )
									{
										//toggle mission messages to find mode	
										m_MissionInformant = "Dr. Legasov";
										
										//translate SecondarySpawn coords		
									    XX = TargetPos[0] / 10000;
									    if ( !MapGridYnorthAscending )
									    YY = ( EventsWorldData.EWD_MAP_SIZE - TargetPos[2] ) / 10000;
										else
										YY = TargetPos[2] / 10000;
										
										TargetCoords = " X "+ ( XX.ToString().Substring(0,4) ) +" / Y "+ ( YY.ToString().Substring(0,4) ) +" ";
										
										m_MissionMessage1 = "It seems the survivor has lost the documents. Somebody reported me the last known position of the survivor. Try to grab the documents before others will do.";
										m_MissionMessage2 = "They are anywhere around there and it's marked with yellow smoke. Read the notice where you have to bring it. I can't tell via the radio.";
										m_MissionMessage3 = "GPS coordinates: " + TargetCoords +"\nThe documents must be anywhere there. Stay safe, military troops are alerted and searching for it.";
									}
				
									//mark documents with yellow smoke 				
									MarkTargetSmoke();
									
									Print("[SMM] HeadHunt Mission : MissionTarget was droped and TargetPlayer left the spot. Intel was marked with yellow smoke. ");	
								}	
							}
						}
					}
				}
			}
			
			//check MissionTarget in secondary mission zone
			if ( PlayerNearSecondary )
			{
				//check if player with case left the server
				if ( !MissionTarget )
				{
					if ( TargetWasTaken )
					{
						TargetWasTaken = false;
						
						//respawn mission target
						MissionTarget = g_Game.CreateObjectEx( MissionTargetType, TargetPos, ECE_PLACE_ON_SURFACE|ECE_ROTATIONFLAGS, RF_TOP );
						//m_MissionZoneOuterRadius = 150;
						//m_MissionZoneInnerRadius = 0.001;
						
						if ( m_MissionExtended )
						{
							//toggle mission messages to find mode
							m_MissionInformant = "Dr. Legasov";
							
							//translate secondary location
							if ( MissionSettings.Opt_TranslateEWData )
							m_MissionSecondaryLoc = EventsWorldData.TransLoc( m_MissionSecondaryLoc );
											
							m_MissionMessage1 = "It seems the survivor has droped the documents case or died. Somebody reported the last known position of the survivor.";
							m_MissionMessage2 = "Try to get the documents before others will do. The documents case is marked with yellow smoke.";
							m_MissionMessage3 = "Look for it anywhere in or around **"+ m_MissionSecondaryLoc +"**\nNo worries, I have the keys and if you could bring it to the mentioned location it can save many of lives.";
						}
						
						//mark documents with yellow smoke 
						DeleteMarker();
						MarkTargetSmoke();
						
						Print("[SMM] HeadHunt Mission : Player has left the server with the documents in inventory near secondary location. Respawning intel on ground and marking it with yellow smoke.");
					}
				}
			
			}
		}
	}	
		
	override bool DeployMission()
	{	//When first player enters the mission zone (primary/secondary)
		
		if ( MissionBuilding )
		{
			//Call spawners	
			g_Game.GetCallQueue( CALL_CATEGORY_GAMEPLAY ).Call( this.SpawnObjects );
			g_Game.GetCallQueue( CALL_CATEGORY_GAMEPLAY ).Call( this.SpawnAIs );
			return true;		
		}
		else 
		{
			Print("[SMM] HeadHunt Mission : ERROR :: MissionBuilding can't be found!");
			if ( MissionSettings.DebugMode ) Print("[SMM] HeadHunt Mission : MissionBuilding is NULL!");
			return false;
		}
	}
}
