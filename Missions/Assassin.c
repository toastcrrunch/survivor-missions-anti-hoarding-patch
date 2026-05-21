class AssassinMission extends SideMissions		//by Xellose and Funkdoc
{
	//Mission related entities
	PlayerBase TargetPlayer;
	ItemBase MissionObject;
	
	//Mission params
	int FreshieUpTime = 30;					//minutes, max. LifeSpan / playtime of a char to count as freshie for targetplayer selection
	 	
	//Mission containers	
	ref array<PlayerBase> AssistingPlayers = new array<PlayerBase>;
	ref array<Man> PlayerList = new array<Man>;
	
	//Settings buffer vars
	int MsgWaitTimeRst;
		
	//Mission variables 
	string SurvivorName, MapName;
	bool TargetPlayerSelected = false;
	float PLifeSpan;
	int GPSupdTime, TPChkTime, crntECTime;
		
	override bool IsExtended() { return false; };
	
	void AssassinMission()
	{
		//exclude mission from saving mission position to file
		m_DynamicObjective = true;
		
		//Mission timeout
		m_MissionTimeout = 2700;			//seconds, mission duration time
		
		//Mission zone params
		m_MissionZoneOuterRadius = 90.0;	//meters (!Do not set lower than 50m), mission activation distance
		m_MissionZoneInnerRadius = 0.0001;	//meters (!Do not set outside of 1-5m), mission finishing distance to target object
		
		//set map name for messages
		string WldName = g_Game.GetWorldName();
		
		if ( WldName == "chernarusplus")		MapName = "Chernarus";
		else if ( WldName == "enoch")			MapName = "Livonia";
		else if ( WldName == "deerisle")		MapName = "Deer Isle";
		else if ( WldName == "namalsk")			MapName = "Namalsk";
		else MapName = WldName;
		
		//Mission informant
		m_MissionInformant = "Dr. Legasov";
										
		//Mission messages 
        m_MissionMessage1 = "I heard that a spy has recently crossed the border of "+ MapName +". It seems its mission is to gather informations about the Anti-Gen of the virus.";
        m_MissionMessage2 = "He is working for the Umbrella Corp. and they want to develop a new biological weapon with this informations. I can't believe it, so many people died here and they want to make it even worse.";
        m_MissionMessage3 = "There is just one chance to stop it, that means we have to eliminate him.\nIf you are up to the task, stay tuned on this channel. I will give you the needed information about this person as soon as possible.";
	}
	
	void ~AssassinMission()
	{
		//Despawn remaining mission objects
		if ( m_MissionObjects )
		{
			Print("[SMM] Assassin Mission : Despawning "+ m_MissionObjects.Count() +" mission objects from old mission...");
			for (int i=0; i < m_MissionObjects.Count(); i++ )
			{
				if ( !m_MissionObjects.Get(i))	continue;
				else g_Game.ObjectDelete( m_MissionObjects.Get(i));
			}
			m_MissionObjects.Clear();
		}
		
		//Despawn mission AI's
		if ( m_MissionAIs )
		{
			if ( m_MissionAIs.Count() > 0 )
			{
				Print("[SMM] Assassin Mission : Despawning "+ m_MissionAIs.Count() +" mission AI's from old mission...");
				for ( int k = 0; k < m_MissionAIs.Count(); k++ )
				{
					g_Game.ObjectDelete( m_MissionAIs.Get(k) );
				}
				m_MissionAIs.Clear();			
			}
			else Print("[SMM] Assassin Mission : No mission AI's to despawn.");
		}
				
		//Delete PlayersInZone list
		if ( m_PlayersInZone )	m_PlayersInZone.Clear();
		
		//Reset changed settings by mission 
		if ( MsgWaitTimeRst )	MissionSettings.MsgWaitTime = MsgWaitTimeRst;
		
		//Reset TargetPlayer in case the mission timed out and the selected player was not killed
		if ( m_TargetPlayer )	m_TargetPlayer = null;
		
		//reset dynamic objective mission status
		m_DynamicObjective = false;
	}
	
	void SpawnRewards()
	{		
		//Spawn seachest as mission object
		MissionObject = ItemBase.Cast( g_Game.CreateObjectEx( "MissionChest", m_MissionPosition, ECE_PLACE_ON_SURFACE) );
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
			MissionObject.GetInventory().CreateInInventory("Battery9V");
			MissionObject.GetInventory().CreateInInventory("Battery9V");
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
			MissionObject.GetInventory().CreateInInventory("Battery9V");
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
			MissionObject.GetInventory().CreateInInventory("Battery9V");
			MissionObject.GetInventory().CreateInInventory("Battery9V");				
		}
		if (selectedLoadout == 6)
		{			
			weapon = MissionObject.GetInventory().CreateInInventory("UMP45");
				weapon.GetInventory().CreateAttachment("M68Optic");
				weapon.GetInventory().CreateAttachment("PistolSuppressor");			
			MissionObject.GetInventory().CreateInInventory("Mag_UMP_25Rnd");
			MissionObject.GetInventory().CreateInInventory("Mag_UMP_25Rnd");
			MissionObject.GetInventory().CreateInInventory("Ammo_45ACP");
			MissionObject.GetInventory().CreateInInventory("Cannabis");
			MissionObject.GetInventory().CreateInInventory("CannabisSeedsPack");
			MissionObject.GetInventory().CreateInInventory("WaterBottle");	
			MissionObject.GetInventory().CreateInInventory("TunaCan");
			MissionObject.GetInventory().CreateInInventory("RDG5Grenade");
			MissionObject.GetInventory().CreateInInventory("Battery9V");
			MissionObject.GetInventory().CreateInInventory("Battery9V");				
		}	
		if (selectedLoadout == 7)
		{			
			weapon = MissionObject.GetInventory().CreateInInventory("VSS");
				weapon.GetInventory().CreateAttachment("PSO11Optic");		
			MissionObject.GetInventory().CreateInInventory("Mag_VSS_10Rnd");
			MissionObject.GetInventory().CreateInInventory("Mag_VSS_10Rnd");
			MissionObject.GetInventory().CreateInInventory("AmmoBox_9x39_20Rnd");
			MissionObject.GetInventory().CreateInInventory("GardenLime");
			MissionObject.GetInventory().CreateInInventory("CannabisSeedsPack");
			MissionObject.GetInventory().CreateInInventory("WaterBottle");	
			MissionObject.GetInventory().CreateInInventory("SardinesCan");
			MissionObject.GetInventory().CreateInInventory("M18SmokeGrenade_Green");
			MissionObject.GetInventory().CreateInInventory("Battery9V");
			MissionObject.GetInventory().CreateInInventory("Battery9V");				
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
		
		Print("[SMM] Assassin Mission : Mission rewards spawned in reward container. Randomly selected loadout was "+ selectedLoadout +"." );
					
		//Insert mission crate into mission objects list
		m_MissionObjects.Insert( MissionObject );	
	}
	
	override void ObjDespawn()
	{	//immediately despawns mission objects at mission timeout or after finish message, except those which remains until next mission
		//despawn nothing
	}
	
	override void MissionFinal()
	{	//When player enters last mission target zone
		//Log assisting players
		g_Game.GetWorld().GetPlayerList( PlayerList );
		int PlayerCount = PlayerList.Count();
		
		for ( int i=0; i < PlayerCount; i++)
		{
			Man Player = PlayerList.Get(i);
			PlayerBase ZonePlayer;
			Class.CastTo( ZonePlayer ,Player );
			
			if ( ZonePlayer )
			{
				string ZonePlayerID = ZonePlayer.GetIdentity().GetPlainId();
				
				for ( int j=0; j < m_PlayersInZone.Count(); j++)
				{
					if ( m_PlayersInZone.Get(j) == ZonePlayerID )
					AssistingPlayers.Insert( ZonePlayer );
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
			
			//log player
			Print("[SMM] Assassin Mission : Player with SteamID64: "+ AssistPlayer.GetIdentity().GetPlainId() +" deserves respect for his "+ ExtMsg +".");
			//send Respect message to player
			NotificationSystem.SendNotificationToPlayerIdentityExtended( AssistPlayer.GetIdentity() , 15.0, RespectMsg, "", MsgIcon );	
		}
	}
	
	override void PlayerChecks( PlayerBase player )
	{	//Executed continuously while mission is active and triggered. Avoid complex calculations or big data processing!
		//do nothing
	}

	override void EntityChecks()
	{	//Executed continuously when mission was actived. Avoid complex calculations or big data processing!
		
		if ( !m_RewardsSpawned && !m_MissionTimedOut )
		{
			//Get server runtime for timer functions
			crntECTime = g_Game.GetTime();
			
			//if target player was not selected yet, check player list every 10 sec and select a random freshie if possible
			if ( crntECTime >= TPChkTime + 10000 && !TargetPlayerSelected )
			{
				g_Game.GetWorld().GetPlayerList( PlayerList );
				int PlayerCount = PlayerList.Count();
				
				for ( int i=0; i < PlayerCount; i++)
				{
					Man Player = PlayerList.Get(i);
					if ( Player )
					{
						//get life span in seconds
						PLifeSpan = Player.StatGet( AnalyticsManagerServer.STAT_PLAYTIME );
						
						//check if players life span time is lower than the max. freshie's uptime 
						if ( PLifeSpan < FreshieUpTime * 60 )
						{
							//continue with next if player is alrready dead
							if ( !PlayerBase.Cast( Player ).IsAlive() )
							continue;
						
							m_TargetPlayer = PlayerBase.Cast( Player );
							if ( m_TargetPlayer )
							{
								SurvivorName = m_TargetPlayer.GetIdentity().GetName();
								m_MissionPosition = m_TargetPlayer.GetPosition();
							}
							else
							{
								Print("[SMM] Assassin Mission : WARNING :: Couldn't get the target spy player from the player list, maybe it's AI. Trying to get another fresh spawned player.");
								continue;
							}
							
							TargetPlayerSelected = true;
							
							Print("[SMM] Assassin Mission : A new Freshie has been selected with LifeSpan = "+ (PLifeSpan/60).ToString() +" min, Freshie name = "+ SurvivorName);
							
							//set radio messages for hunting mode
							m_MissionMessage1 = "I have new infos about the spy. We don't know how it's dressed or if it's male or female spy but we know it's currently looting some houses to gather intel about the Anti-Gen.";
							m_MissionMessage2 = "Someone told me that Umbrella implanted a nano GPS tracker inside its body for evacuation. Now we have access to the GPS data of this guy, probably only for a short period of time.";
							//message 3 is continuously updated in the check below
							
							//increase message loop speed, settings override
							MsgWaitTimeRst = MissionSettings.MsgWaitTime; //buffer settings
							MissionSettings.MsgWaitTime = 200; 			//override message intervall (restore default value when destructing class)
																		
							m_MsgNum = 1;		//skip msg 0, begin with msg 1
							m_MsgChkTime = m_MissionTime + 3;
							break;
							
						}
					}
				}
				
				if ( TargetPlayerSelected ) return;
				
				TPChkTime = crntECTime;
			}
			
			//check target player
			if ( m_TargetPlayer )
			{
				//update position
				m_MissionPosition = m_TargetPlayer.GetPosition();
				
				//refresh radio message 3
				if ( crntECTime > GPSupdTime + 2000 )	//every 2 sec
				{
					m_MissionMessage3 = "The undercover name of the spy is "+ SurvivorName +" and the last known position is as follows.\n\nGPS coordinates:"+ m_MissionInstance.GetGPS();
					GPSupdTime = crntECTime;
				}
				
				//check if spy died, print death reason, determine killer data and call reward spawn and finish mission in case
				if ( !m_TargetPlayer.IsAlive() )
				{
					EntityAI DeadSpyEntity = m_TargetPlayer;
					 
					if ( DeadSpyEntity )
					{
						KillerData DeadSpyKillData = DeadSpyEntity.m_KillerData;
						
						if ( DeadSpyKillData )
						{
							EntityAI DeadSpyKillerEntity = DeadSpyKillData.m_Killer;
							EntityAI KillerWeapon = DeadSpyKillData.m_MurderWeapon;
							bool DeadByHeadshot = DeadSpyKillData.m_KillerHiTheBrain;
							PlayerBase Killer;
							string HdShot = "";
							bool KillerWasAI = false;
							
							Class.CastTo( Killer, DeadSpyKillerEntity );
							
							if ( Killer ) //dead by player or eAI
							{
								if ( DeadByHeadshot )
								HdShot = " by headshot.";
								
#ifdef ENFUSION_AI_PROJECT
#ifdef EXPANSIONMODAI
								if ( Killer.IsAI() )
								{
									Print("[SMM] Assassin Mission : Spy "+ SurvivorName +" was killed with "+ KillerWeapon.GetType() +" by ExpAI "+ Killer.GetType() +" @ "+ m_MissionPosition );
									Print("[SMM] Assassin Mission : No rewards spawned because killer is not human.");
									KillerWasAI = true;
									m_MissionPosition = "0 0 0";
									
									//abort mission
									m_MissionTimedOut = true;
									m_MsgNum = -3;
								}
#endif
#endif
								if ( !KillerWasAI )
								{
									if ( Killer == m_TargetPlayer )
									{
										Print("[SMM] Assassin Mission : Spy "+ SurvivorName +" committed suicide or accidently fell down @ "+ m_MissionPosition );
										m_MissionPosition = "0 0 0";
										
										//abort mission
										m_MissionTimedOut = true;
										m_MsgNum = -3;
									}
									else
									{
										Print("[SMM] Assassin Mission : Spy "+ SurvivorName +" was killed with "+ KillerWeapon.GetType() +" by "+ Killer.GetIdentity().GetName() + HdShot +" @ "+ m_MissionPosition );
										
										//new mission position after spy died
										m_MissionPosition += "1.5 0 1.5";
										m_MissionZoneInnerRadius = 2.0;
																
										//spawn rewards
										SpawnRewards();
										
										//log players in mission zone and send respect msg 
										MissionFinal();
										
										//finish mission
										m_RewardsSpawned = true;
										m_MsgNum = -1;

										//delayed cleanup for reward chest
										Object rewardChest = MissionObject;
										g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(g_Game.ObjectDelete, 300000, false, rewardChest);							

										//accomplished message delay
										//m_MsgChkTime = m_MissionTime + 5;
										//return;
									}
								}
							}
							else	//or another death reason
							{
								Print("[SMM] Assassin Mission : Spy "+ SurvivorName +" was killed by infected @ "+ m_MissionPosition );
								
								m_MissionPosition = "0 0 0";
								
								//abort mission
								m_MissionTimedOut = true;
								m_MsgNum = -3;
							}
						}
						else 
						{
							Print("[SMM] Assassin Mission : WARNING :: Spy "+ SurvivorName +" died but can't get dead spy's killer data, no killer data provided.! Probably spy killed himself with an admin tool or gun @ "+ m_MissionPosition );
							m_TargetPlayer = null;
							m_MissionPosition = "0 0 0";
							return;
						}
					}
					else 
					{
						Print("[SMM] Assassin Mission : WARNING :: Spy "+ SurvivorName +" died but can't get dead body entity @ "+ m_MissionPosition );
						m_TargetPlayer = null;
						m_MissionPosition = "0 0 0";
						return;
					}
/*					
					//remove player from mission zone list
					for ( int j=0; j < m_PlayersInZone.Count(); j++ )
					{
						string ZonePlayerID = m_PlayersInZone.Get( j );
						if ( m_TargetPlayer.GetIdentity().GetPlainId() == ZonePlayerID )
						m_PlayersInZone.RemoveOrdered( j );
					}
*/
					//...and reset TargetPlayer 
					m_TargetPlayer = null;
					
					//mission message delay
					m_MsgChkTime = m_MissionTime + 1;
				}
			}
			else
			{
				//target player logged out or suicided while in hunting mode
				if ( TargetPlayerSelected )
				{
					Print("[SMM] Assassin Mission : WARNING :: Spy "+ SurvivorName +" disconnected or comitted sucide in hunting mode. Reactivating freshie selection!");

					//reset TargetPlayer & selection and init mission position
					m_TargetPlayer = null;
					TargetPlayerSelected = false;
					m_MissionPosition = "0 0 0";
										
					//reset mission messages from hunting mode 
			        m_MissionMessage1 = "I heard that a spy has recently entered the border of "+ MapName +". It seems his mission is to gather informations about the Anti-Gen of the virus.";
			        m_MissionMessage2 = "He is working for the Umbrella Corp. and they want to develop a new biological weapon with this informations. I can't believe it, so many people died here and they want to make it even worse.";
			        m_MissionMessage3 = "There is just one chance to stop it, that means we have to eliminate him.\nIf you are up to the task, stay tuned on this channel. I will give you the needed information about this person as soon as possible.";
				}
			}
		}
	}	
			
	override bool DeployMission()
	{	//When first player enters the mission zone (primary/secondary)
		//nothing to deploy
		return true;
	}		
}