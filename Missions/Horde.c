class HordeMission extends SurvivorMissions		//by Xellose and Funkdoc
{
	//Mission related entities 
	ItemBase MissionObject;
	Object MissionLight;
	Object MissionWreck;
	
	//Mission parameters
	int MsgDlyFinish = 300;					//seconds, message delay time after player has finished mission
	int HordeDensity = 60;					//infected, spawned in circle 
	float WreckMinSpawnDistance = 300.0;	//meters from mission center
	float WreckCollisionCheckRadius = 5.0;	//meters around wreck spawn point
	float WreckGroundClearance = 0.08;	//meters, tiny lift to keep the wreck from clipping after centered placement
	int WreckSpawnMaxAttempts = 500;		//max attempts to find a valid spawn position for the wreck
	float WreckLifetimeSeconds = 3000.0;	//seconds before CE cleanup can remove the wreck

	//Mission containers
	ref array<string> InfectedTypes = new array<string>;
	ref array<PlayerBase> AssistingPlayers = new array<PlayerBase>;
		
	override bool IsExtended() { return false; };
	
	void HordeMission()
	{
		//Mission mission timeout
		m_MissionTimeout = 2700;			//seconds, mission duration time
		
		//Mission zones
		m_MissionZoneOuterRadius = 200.0;	//meters (!Do not set lower than 200m), mission activation distance
		m_MissionZoneInnerRadius = 3.0;		//meters (!Do not set outside of 1-5m), mission finishing distance to target object
				
		//Mission informant
		m_MissionInformant = "Dr. Legasov";
		
		//Translate mission location name and direction
		if ( MissionSettings.Opt_TranslateEWData )
		{
			m_MissionLocation = EventsWorldData.TransLoc( m_MissionLocation );
			m_MissionLocationDir = EventsWorldData.TransDir( m_MissionLocationDir );
		}
				
		//Set mission messages
		m_MissionMessage1 = "I got an emergency call from a heli pilot who crashed somewhere in the forests. He is injured but still alive. He was afraid that the infected will find him soon.";
		m_MissionMessage2 = "He made it to a house in the village near the crash site. He is well equiped and he think he will stay in the house over the night where he treats his open wounds.";
		m_MissionMessage3 = "The house is at the ** "+ m_MissionLocationDir +" side of "+ m_MissionLocation +" **\nBe careful there may be infected and other survivors who want his equipment!";
		//Add GPS coordinates of mission in message
		if ( MissionSettings.Opt_ShowGpsInfo ) m_MissionMessage3 += "\n\nGPS coordinates:"+ m_MissionInstance.GetGPS();		

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

		
		g_Game.GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( this.SpawnMissionWreck, 5000, false );

	}
	
	void ~HordeMission()
	{
		
		//Despawn all remaining mission objects
		if ( m_MissionObjects )
		{	
			Print("[SMM] Horde Mission : Despawning "+ m_MissionObjects.Count() +" mission objects from old mission...");				
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
				Print("[SMM] Horde Mission : Despawning "+ m_MissionAIs.Count() +" mission AI's from old mission...");
				for ( int k = 0; k < m_MissionAIs.Count(); k++ )
				{
					g_Game.ObjectDelete( m_MissionAIs.Get(k) );
				}
				m_MissionAIs.Clear();			
			}
			else Print("[SMM] Horde Mission : No mission AI's to despawn.");
		}
		
		//Delete PlayersInZone list
		if ( m_PlayersInZone )	m_PlayersInZone.Clear();
	}

	bool TryFindWreckSpawnPosition(out vector outPos)
	{
		float outerRadius = Math.Max(m_MissionZoneOuterRadius - 6.0, 5.0);
		float effectiveMinDist = Math.Max(Math.Min(WreckMinSpawnDistance, outerRadius - 1.0), 0.0);

		float minDistSq = effectiveMinDist * effectiveMinDist;
		float maxDistSq = outerRadius * outerRadius;
		array<Object> nearbyObjects = new array<Object>;
		array<CargoBase> nearbyCargo = new array<CargoBase>;

		for ( int attempt = 0; attempt < WreckSpawnMaxAttempts; attempt++ )
		{
			float x = Math.RandomFloatInclusive(-outerRadius, outerRadius);
			float z = Math.RandomFloatInclusive(-outerRadius, outerRadius);
			float distSq = (x * x) + (z * z);

			if ( distSq < minDistSq || distSq > maxDistSq )
				continue;

			vector candidatePos = m_MissionPosition + Vector(x, 0, z);
			candidatePos[1] = g_Game.SurfaceY(candidatePos[0], candidatePos[2]);

			if ( g_Game.SurfaceIsSea(candidatePos[0], candidatePos[2]) || g_Game.SurfaceIsPond(candidatePos[0], candidatePos[2]) )
				continue;

			nearbyObjects.Clear();
			nearbyCargo.Clear();
			g_Game.GetObjectsAtPosition3D(candidatePos, WreckCollisionCheckRadius, nearbyObjects, nearbyCargo);

			bool isBlocked = false;
			for ( int i = 0; i < nearbyObjects.Count(); i++ )
			{
				Object obj = nearbyObjects.Get(i);
				if ( obj && obj != MissionObject && obj != MissionLight && obj != MissionWreck )
				{
					if ( obj.IsBuilding() || obj.IsTree() || obj.IsBush() || obj.IsPlainObject() )
					{
						isBlocked = true;
						break;
					}
				}
			}

			if ( !isBlocked )
			{
				outPos = candidatePos;
				return true;
			}
		}

		return false;
	}

	void SpawnMissionWreck()
	{
		if ( MissionWreck || vector.DistanceSq(m_MissionPosition, "0 0 0") <= 0.01 )
			return;

		vector wreckPos;
		if ( !TryFindWreckSpawnPosition(wreckPos) )
		{
			Print("[SMM] Horde Mission : ERROR :: Could not find a valid wreck spawn position around mission " + m_MissionName + ".");
			return;
		}

		int wreckSpawnFlags = ECE_FULL | ECE_CREATEPHYSICS | ECE_CENTER;
		MissionWreck = g_Game.CreateObjectEx("Wreck_UH1Y", wreckPos, wreckSpawnFlags, RF_DEFAULT);
		if ( !MissionWreck )
			return;

		MissionWreck.PlaceOnSurface();
		wreckPos = MissionWreck.GetPosition();
		wreckPos[1] = wreckPos[1] + WreckGroundClearance;
		MissionWreck.SetPosition(wreckPos);

		EntityAI wreckEntity = EntityAI.Cast(MissionWreck);
		if ( wreckEntity )
			wreckEntity.SetLifetime(WreckLifetimeSeconds);

		m_MissionObjects.Insert( MissionWreck );
		Print("[SMM] Horde Mission : Spawned wreck type " + MissionWreck.GetType() + " at " + MissionWreck.GetPosition() + ".");
	}

	void SpawnObjects()
	{				

		//Mission object is pilot bag
		MissionObject = ItemBase.Cast( g_Game.CreateObjectEx("MissionBagBig", m_MissionPosition, ECE_PLACE_ON_SURFACE ) );
		MissionObject.SetPosition( m_MissionPosition + "0 0.2 0" );
		MissionObject.SetTakeable(false);
		//MissionObject.PlaceOnSurface(); 
		
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

		Object rewardBag = MissionObject;
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(g_Game.ObjectDelete, 300000, false, rewardBag);

		Print("[SMM] Horde Mission : Mission rewards spawned in reward container. Randomly selected loadout was "+ selectedLoadout +"." );
		
		Print("[SMM] Horde Mission : Survivor Mission "+ m_selectedMission +" :: "+ m_MissionName +" ...mission deployed!");
	}
	
	void SpawnAIs()
	{	
		//Spawn pilot
		DayZInfected InfectedPilot = DayZInfected.Cast( g_Game.CreateObjectEx( "ZmbM_CommercialPilotOld_Olive" , m_MissionPosition , ECE_PLACE_ON_SURFACE | ECE_INITAI ));
		InfectedPilot.SetPosition( m_MissionPosition );
		InfectedPilot.GetInventory().CreateAttachment("ZSh3PilotHelmet_Green");
		InfectedPilot.GetInventory().CreateInInventory("M67Grenade");
		InfectedPilot.GetInventory().CreateInInventory("CanOpener");
		InfectedPilot.GetInventory().CreateInInventory("BandageDressing");
		InfectedPilot.GetInventory().CreateInInventory("PeachesCan");
		InfectedPilot.GetInventory().CreateInInventory("Canteen");
		InfectedPilot.GetInventory().CreateAttachment("UKAssVest_Olive");
		InfectedPilot.GetInventory().CreateInInventory("PersonalRadio");
		InfectedPilot.GetInventory().CreateInInventory("Battery9V");
		InfectedPilot.GetInventory().CreateInInventory("Battery9V");
		InfectedPilot.GetInventory().CreateInInventory("Battery9V");
			
		//Pilot is dead
		InfectedPilot.SetHealth("","",0);
		
		m_MissionAIs.Insert( InfectedPilot );
		
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
				Print("[SMM] Horde Mission : Test Hour is "+ hour +", SunRiseHour: "+ SunRiseHour +", SonSetHour: "+ SunSetHour );
				
				if ( hour >= SunSetHour || hour <= SunRiseHour ) 
				{

					//Choose the Light position Here. 
					MissionLight = g_Game.CreateObjectEx("PortableGasLamp", m_MissionPosition + "1 0 1", ECE_PLACE_ON_SURFACE);

					ItemBase lamp = ItemBase.Cast(MissionLight);
					if ( !lamp )
					{
						Print("[SMM] Horde Mission : ERROR :: Could not spawn mission light at " + m_MissionPosition);
						return;
					}

					EntityAI canister = lamp.GetInventory().CreateAttachment("SmallGasCanister");
					if ( !canister )
					{
						Print("[SMM] Horde Mission : ERROR :: Could not attach SmallGasCanister to mission light.");
					}

					if ( lamp.HasEnergyManager() && lamp.GetCompEM().CanSwitchOn() )
					{
						lamp.GetCompEM().SwitchOn();
					}
					else
					{
						Print("[SMM] Horde Mission : ERROR :: Mission light can not be switched on.");
					}
					m_MissionObjects.Insert( lamp );
				}
			}
		}
		
		//Spawn horde
		for ( int j = 0; j < HordeDensity; j++ )
		{
			//calc new spawn position
			float x = Math.RandomFloatInclusive( 3 , 30 );
			float y = Math.RandomFloatInclusive( 3 , 30 );	
			int Dice = Math.RandomIntInclusive( 0, 9);
			if ( Dice > 4 ) x *= -1.0;
			Dice = Math.RandomIntInclusive( 0, 9);
			if ( Dice < 5 ) y *= -1.0;
			vector NewPosVector = { x, 0, y };
			
			//Spawn infected
			m_MissionAIs.Insert( g_Game.CreateObjectEx( InfectedTypes.GetRandomElement(), m_MissionPosition + NewPosVector, ECE_PLACE_ON_SURFACE | ECE_INITAI ));	
		}
	}
	
	override void ObjDespawn() 
	{	
		if ( MissionWreck )
			g_Game.ObjectDelete( MissionWreck );
		MissionWreck = null;
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
			Print("[SMM] Horde Mission : Player with SteamID64: "+ IdentPlainID +" deserves respect for his "+ ExtMsg +".");
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
			g_Game.GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( this.SpawnAIs, 1000, false );
			return true;		
		}
		else 
		{
			Print("[SMM] Horde Mission : ERROR :: Mission position was rejected or doesn't exist. MissionPosition is NULL!");
			return false;
		}
	}
}

