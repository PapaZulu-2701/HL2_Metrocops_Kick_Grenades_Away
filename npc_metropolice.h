/*
 Reminder:
 Use CTRL+F to search for "MODIFICATION" and easily spot what's custom 
 versus what's original Valve code.
*/

//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef NPC_METROPOLICE_H
#define NPC_METROPOLICE_H
#ifdef _WIN32
#pragma once
#endif

#include "rope.h"
#include "rope_shared.h"
#include "ai_baseactor.h"
#include "ai_basenpc.h"
#include "ai_goal_police.h"
#include "ai_behavior.h"
#include "ai_behavior_standoff.h"
#include "ai_behavior_assault.h"
#include "ai_behavior_functank.h"
#include "ai_behavior_actbusy.h"
#include "ai_behavior_rappel.h"
#include "ai_behavior_police.h"
#include "ai_behavior_follow.h"
#include "ai_sentence.h"
#ifdef MAPBASE
#include "mapbase/ai_grenade.h"
#endif
#include "props.h"
#ifdef EXPANDED_RESPONSE_SYSTEM_USAGE
#include "mapbase/expandedrs_combine.h"
#define METROPOLICE_USES_RESPONSE_SYSTEM 1
#endif

class CNPC_MetroPolice;

#ifdef MAPBASE
class CNPC_MetroPolice : public CAI_GrenadeUser<CAI_BaseActor>
{
	DECLARE_CLASS( CNPC_MetroPolice, CAI_GrenadeUser<CAI_BaseActor> );
#else
class CNPC_MetroPolice : public CAI_BaseActor
{
	DECLARE_CLASS( CNPC_MetroPolice, CAI_BaseActor );
#endif
	DECLARE_DATADESC();

public:
	CNPC_MetroPolice();

	virtual bool CreateComponents();
	bool CreateBehaviors();
	void Spawn( void );
	void Precache( void );

	Class_T		Classify( void );
	Disposition_t IRelationType(CBaseEntity *pTarget);
	float		MaxYawSpeed( void );
	void		HandleAnimEvent( animevent_t *pEvent );
	Activity NPC_TranslateActivity( Activity newActivity );
#ifdef MAPBASE
	Activity Weapon_TranslateActivity( Activity baseAct, bool *pRequired );

	virtual int			UnholsterWeapon( void );
	virtual void		OnChangeRunningBehavior( CAI_BehaviorBase *pOldBehavior,  CAI_BehaviorBase *pNewBehavior );

	const char*		GetGrenadeAttachment() { return "LHand"; }

	virtual bool IsAltFireCapable() { return (m_iGrenadeCapabilities & GRENCAP_ALTFIRE) != 0 && BaseClass::IsAltFireCapable(); }
	virtual bool IsGrenadeCapable() { return (m_iGrenadeCapabilities & GRENCAP_GRENADE) != 0; }

	virtual bool	ShouldDropGrenades() { return (m_iGrenadeDropCapabilities & GRENDROPCAP_GRENADE) != 0 && BaseClass::ShouldDropGrenades(); }
	virtual bool	ShouldDropInterruptedGrenades() { return (m_iGrenadeDropCapabilities & GRENDROPCAP_INTERRUPTED) != 0 && BaseClass::ShouldDropInterruptedGrenades(); }
	virtual bool	ShouldDropAltFire() { return (m_iGrenadeDropCapabilities & GRENDROPCAP_ALTFIRE) != 0 && BaseClass::ShouldDropAltFire(); }
#endif

	Vector		EyeDirection3D( void )	{ return CAI_BaseHumanoid::EyeDirection3D(); } // cops don't have eyes

	virtual void Event_Killed( const CTakeDamageInfo &info );

	virtual void Event_KilledOther( CBaseEntity *pVictim, const CTakeDamageInfo &info );

	virtual void OnScheduleChange();

	float		GetIdealAccel( void ) const;
	int			ObjectCaps( void ) { return UsableNPCObjectCaps(BaseClass::ObjectCaps()); }
	void		PrecriminalUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	// These are overridden so that the cop can shove and move a non-criminal player safely
	CBaseEntity *CheckTraceHullAttack( float flDist, const Vector &mins, const Vector &maxs, int iDamage, int iDmgType, float forceScale, bool bDamageAnyNPC );
	CBaseEntity *CheckTraceHullAttack( const Vector &vStart, const Vector &vEnd, const Vector &mins, const Vector &maxs, int iDamage, int iDmgType, float flForceScale, bool bDamageAnyNPC );

	virtual int	SelectSchedule( void );
	virtual int SelectFailSchedule( int failedSchedule, int failedTask, AI_TaskFailureCode_t taskFailCode );
	virtual int TranslateSchedule( int scheduleType );
	void		StartTask( const Task_t *pTask );
	void		RunTask( const Task_t *pTask );
	virtual Vector GetActualShootTrajectory( const Vector &shootOrigin );
	virtual void FireBullets( const FireBulletsInfo_t &info );
	virtual bool HandleInteraction(int interactionType, void *data, CBaseCombatCharacter* sourceEnt);
	virtual void Weapon_Equip( CBaseCombatWeapon *pWeapon );

	//virtual bool OverrideMoveFacing( const AILocalMoveGoal_t &move, float flInterval );
	bool		OnObstructionPreSteer( AILocalMoveGoal_t *pMoveGoal, float distClear, AIMoveResult_t *pResult );
	bool		ShouldBruteForceFailedNav()	{ return false; }

	virtual void GatherConditions( void );

	virtual bool OverrideMoveFacing( const AILocalMoveGoal_t &move, float flInterval );

	// Can't move and shoot when the enemy is an airboat
	virtual bool ShouldMoveAndShoot();

	// TraceAttack
	virtual void TraceAttack( const CTakeDamageInfo &info, const Vector &vecDir, trace_t *ptr, CDmgAccumulator *pAccumulator );

	// Speaking
	virtual void SpeakSentence( int nSentenceType );
#ifdef METROPOLICE_USES_RESPONSE_SYSTEM
	bool			SpeakIfAllowed( const char *concept, SentencePriority_t sentencepriority = SENTENCE_PRIORITY_NORMAL, SentenceCriteria_t sentencecriteria = SENTENCE_CRITERIA_IN_SQUAD )
	{
		return SpeakIfAllowed( concept, NULL, sentencepriority, sentencecriteria );
	}
	bool			SpeakIfAllowed( const char *concept, const char *modifiers, SentencePriority_t sentencepriority = SENTENCE_PRIORITY_NORMAL, SentenceCriteria_t sentencecriteria = SENTENCE_CRITERIA_IN_SQUAD );
	bool			SpeakIfAllowed( const char *concept, AI_CriteriaSet& modifiers, SentencePriority_t sentencepriority = SENTENCE_PRIORITY_NORMAL, SentenceCriteria_t sentencecriteria = SENTENCE_CRITERIA_IN_SQUAD );
	void			ModifyOrAppendCriteria( AI_CriteriaSet& set );
#endif

	// Set up the shot regulator based on the equipped weapon
	virtual void OnUpdateShotRegulator( );

	bool	ShouldKnockOutTarget( CBaseEntity *pTarget );
	void	KnockOutTarget( CBaseEntity *pTarget );
	void	StunnedTarget( CBaseEntity *pTarget );
	void	AdministerJustice( void );

	bool	QueryHearSound( CSound *pSound );

	void	SetBatonState( bool state );
	bool	BatonActive( void );

#ifndef METROPOLICE_USES_RESPONSE_SYSTEM
	CAI_Sentence< CNPC_MetroPolice > *GetSentences() { return &m_Sentences; }
#endif

	virtual	bool		AllowedToIgnite( void ) { return true; }

	void	PlayFlinchGesture( void );

protected:
	// Determines the best type of flinch anim to play.
	virtual Activity GetFlinchActivity( bool bHeavyDamage, bool bGesture );

	// Only move and shoot when attacking
	virtual bool OnBeginMoveAndShoot();
	virtual void OnEndMoveAndShoot();

private:
	bool		PlayerIsCriminal( void );
	void		ReleaseManhack( void );

	// Speech-related methods
	void		AnnounceTakeCoverFromDanger( CSound *pSound );
	void		AnnounceEnemyType( CBaseEntity *pEnemy );
	void		AnnounceEnemyKill( CBaseEntity *pEnemy );
	void		AnnounceHarrassment( );
	void		AnnounceOutOfAmmo( );

	// Behavior-related sentences
	void		SpeakFuncTankSentence( int nSentenceType );
	void		SpeakAssaultSentence( int nSentenceType );
	void		SpeakStandoffSentence( int nSentenceType );

#ifdef MAPBASE
	virtual void	LostEnemySound( CBaseEntity *pEnemy );
	virtual void	FoundEnemySound( CBaseEntity *pEnemy );
#else
	virtual void	LostEnemySound( void );
	virtual void	FoundEnemySound( void );
#endif
	virtual void	AlertSound( void );
	virtual void	PainSound( const CTakeDamageInfo &info );
	virtual void	DeathSound( const CTakeDamageInfo &info );
	virtual void	IdleSound( void );
	virtual bool	ShouldPlayIdleSound( void );

	// Burst mode!
	void		SetBurstMode( bool bEnable );

	int			OnTakeDamage_Alive( const CTakeDamageInfo &info );

	int			GetSoundInterests( void );

	void		BuildScheduleTestBits( void );

	bool		CanDeployManhack( void );

	bool		ShouldHitPlayer( const Vector &targetDir, float targetDist );

	void		PrescheduleThink( void );
	
	void		SetPlayerCriminalDuration( float time );

	void		IncrementPlayerCriminalStatus( void );

	virtual bool		UseAttackSquadSlots()	{ return true; }

	WeaponProficiency_t CalcWeaponProficiency( CBaseCombatWeapon *pWeapon );

	// Inputs
	void InputEnableManhackToss( inputdata_t &inputdata );
#ifdef MAPBASE
	void InputDisableManhackToss( inputdata_t &inputdata );
	void InputDeployManhack( inputdata_t &inputdata );
	void InputAddManhacks( inputdata_t &inputdata );
	void InputSetManhacks( inputdata_t &inputdata );
#endif
	void InputSetPoliceGoal( inputdata_t &inputdata );
	void InputActivateBaton( inputdata_t &inputdata );
#ifdef MAPBASE
	void InputAdministerJustice( inputdata_t &inputdata );
	void InputAddWarnings( inputdata_t &inputdata );
	void InputSetWarnings( inputdata_t &inputdata );
#endif

	void NotifyDeadFriend ( CBaseEntity* pFriend );

	// Stitch aiming!
	void AimBurstRandomly( int nMinCount, int nMaxCount, float flMinDelay, float flMaxDelay );
	void AimBurstAtEnemy( float flReactionTime );
	void AimBurstInFrontOfEnemy( float flReactionTime );
	void AimBurstAlongSideOfEnemy( float flFollowTime );
	void AimBurstBehindEnemy( float flFollowTime );
	void AimBurstTightGrouping( float flShotTime );

	// Anim event handlers
	void OnAnimEventDeployManhack( animevent_t *pEvent );
	void OnAnimEventShove( void );
	void OnAnimEventBatonOn( void );
	void OnAnimEventBatonOff( void );
	void OnAnimEventStartDeployManhack( void );
	void OnAnimEventPreDeployManhack( void );

	bool HasBaton( void );

	// Normal schedule selection 
	int SelectCombatSchedule();
	int SelectScheduleNewEnemy();
	int SelectScheduleArrestEnemy();
	int SelectRangeAttackSchedule();
	int SelectScheduleNoDirectEnemy();
	int SelectScheduleInvestigateSound();
	int SelectShoveSchedule( void );

	bool TryToEnterPistolSlot( int nSquadSlot );

	// Airboat schedule selection
	int SelectAirboatCombatSchedule();
	int SelectAirboatRangeAttackSchedule();

#ifdef MAPBASE
	int SelectBehaviorOverrideSchedule();

	bool IsCrouchedActivity( Activity activity );

	// This is something Valve did with Combine soldiers so they would throw grenades during standoffs.
	// We're using a similar thing here so metrocops deploy manhacks.
	class CMetroPoliceStandoffBehavior : public CAI_ComponentWithOuter<CNPC_MetroPolice, CAI_StandoffBehavior>
	{
		typedef CAI_ComponentWithOuter<CNPC_MetroPolice, CAI_StandoffBehavior> BaseClass;

		virtual int SelectScheduleAttack();
	};
#endif

	// Handle flinching
	bool IsHeavyDamage( const CTakeDamageInfo &info );

	// Is my enemy currently in an airboat?
	bool IsEnemyInAnAirboat() const;

	// Returns the airboat
	CBaseEntity *GetEnemyAirboat() const;

	// Compute a predicted enemy position n seconds into the future
	void PredictShootTargetPosition( float flDeltaTime, float flMinLeadDist, float flAddVelocity, Vector *pVecTarget, Vector *pVecTargetVel );

	// Compute a predicted velocity n seconds into the future (given a known acceleration rate)
	void PredictShootTargetVelocity( float flDeltaTime, Vector *pVecTargetVel );

	// How many shots will I fire in a particular amount of time?
	int CountShotsInTime( float flDeltaTime ) const;
	float GetTimeForShots( int nShotCount ) const;

	// Visualize stitch
	void VisualizeStitch( const Vector &vecStart, const Vector &vecEnd );

	// Visualize line of death
	void VisualizeLineOfDeath( );

	// Modify the stitch length
	float ComputeDistanceStitchModifier( float flDistanceToTarget ) const;

	// Adjusts the burst toward the target as it's being fired.
	void SteerBurstTowardTarget( );

	// Methods to compute shot trajectory based on burst mode
	Vector ComputeBurstLockOnTrajectory( const Vector &shootOrigin );
	Vector ComputeBurstDeliberatelyMissTrajectory( const Vector &shootOrigin );
	Vector ComputeBurstTrajectory( const Vector &shootOrigin );
	Vector ComputeTightBurstTrajectory( const Vector &shootOrigin );

	// Are we currently firing a burst?
	bool IsCurrentlyFiringBurst() const;

	// Which entity are we actually trying to shoot at?
	CBaseEntity *GetShootTarget();

	// Different burst steering modes
	void SteerBurstTowardTargetUseSpeedOnly( const Vector &vecShootAt, const Vector &vecShootAtVelocity, float flPredictTime, int nShotsTillPredict );
	void SteerBurstTowardTargetUseVelocity( const Vector &vecShootAt, const Vector &vecShootAtVelocity, int nShotsTillPredict );
	void SteerBurstTowardTargetUsePosition( const Vector &vecShootAt, const Vector &vecShootAtVelocity, int nShotsTillPredict );
	void SteerBurstTowardPredictedPoint( const Vector &vecShootAt, const Vector &vecShootAtVelocity, int nShotsTillPredict );
	void SteerBurstWithinLineOfDeath( );

	// Set up the shot regulator
	int SetupBurstShotRegulator( float flReactionTime );

	// Choose a random vector somewhere between the two specified vectors
	void RandomDirectionBetweenVectors( const Vector &vecStart, const Vector &vecEnd, Vector *pResult );

	// Stitch selector
	float StitchAtWeight( float flDist, float flSpeed, float flDot, float flReactionTime, const Vector &vecTargetToGun );
	float StitchAcrossWeight( float flDist, float flSpeed, float flDot, float flReactionTime );
	float StitchAlongSideWeight( float flDist, float flSpeed, float flDot );
	float StitchBehindWeight( float flDist, float flSpeed, float flDot );
	float StitchTightWeight( float flDist, float flSpeed, const Vector &vecTargetToGun, const Vector &vecVelocity );
	int SelectStitchSchedule();

	// Can me enemy see me? 
	bool CanEnemySeeMe( );

	// Combat schedule selection 
	int SelectMoveToLedgeSchedule();

	// position to shoot at
	Vector StitchAimTarget( const Vector &posSrc, bool bNoisy );

	// Should we attempt to stitch?
	bool ShouldAttemptToStitch();

	// Deliberately aims as close as possible w/o hitting
	Vector AimCloseToTargetButMiss( CBaseEntity *pTarget, const Vector &shootOrigin );

	// Compute the actual reaction time based on distance + speed modifiers
	float AimBurstAtReactionTime( float flReactonTime, float flDistToTargetSqr, float flCurrentSpeed );
	int AimBurstAtSetupHitCount( float flDistToTargetSqr, float flCurrentSpeed );

	// How many squad members are trying to arrest the player?
	int SquadArrestCount();

	// He's resisting arrest!
	void EnemyResistingArrest();
	void VPhysicsCollision( int index, gamevcollisionevent_t *pEvent );

	// Rappel
	virtual bool IsWaitingToRappel( void ) { return m_RappelBehavior.IsWaitingToRappel(); }
	void BeginRappel() { m_RappelBehavior.BeginRappel(); }

private:
	enum
	{
		BURST_NOT_ACTIVE = 0,
		BURST_ACTIVE,
		BURST_LOCK_ON_AFTER_HIT,
		BURST_LOCKED_ON,
		BURST_DELIBERATELY_MISS,
		BURST_TIGHT_GROUPING,
	};

	enum
	{
		BURST_STEER_NONE = 0,
		BURST_STEER_TOWARD_PREDICTED_POINT,
		BURST_STEER_WITHIN_LINE_OF_DEATH,
		BURST_STEER_ADJUST_FOR_SPEED_CHANGES,
		BURST_STEER_EXACTLY_TOWARD_TARGET,
	};

	enum
	{
		COND_METROPOLICE_ON_FIRE = BaseClass::NEXT_CONDITION,
		COND_METROPOLICE_ENEMY_RESISTING_ARREST,
		COND_METROPOLICE_PLAYER_TOO_CLOSE,
		COND_METROPOLICE_CHANGE_BATON_STATE,
		COND_METROPOLICE_PHYSOBJECT_ASSAULT,

	};

	enum
	{
		SCHED_METROPOLICE_WALK = BaseClass::NEXT_SCHEDULE,
		SCHED_METROPOLICE_WAKE_ANGRY,
		SCHED_METROPOLICE_HARASS,
		SCHED_METROPOLICE_CHASE_ENEMY,
		SCHED_METROPOLICE_ESTABLISH_LINE_OF_FIRE,
		SCHED_METROPOLICE_DRAW_PISTOL,
		SCHED_METROPOLICE_DEPLOY_MANHACK,
		SCHED_METROPOLICE_ADVANCE,
		SCHED_METROPOLICE_CHARGE,
		SCHED_METROPOLICE_BURNING_RUN,
		SCHED_METROPOLICE_BURNING_STAND,
		SCHED_METROPOLICE_SMG_NORMAL_ATTACK,
		SCHED_METROPOLICE_SMG_BURST_ATTACK,
		SCHED_METROPOLICE_AIM_STITCH_AT_AIRBOAT,
		SCHED_METROPOLICE_AIM_STITCH_IN_FRONT_OF_AIRBOAT,
		SCHED_METROPOLICE_AIM_STITCH_TIGHTLY,
		SCHED_METROPOLICE_AIM_STITCH_ALONG_SIDE_OF_AIRBOAT,
		SCHED_METROPOLICE_AIM_STITCH_BEHIND_AIRBOAT,
		SCHED_METROPOLICE_ESTABLISH_STITCH_LINE_OF_FIRE,
		SCHED_METROPOLICE_INVESTIGATE_SOUND,
		SCHED_METROPOLICE_WARN_AND_ARREST_ENEMY,
		SCHED_METROPOLICE_ARREST_ENEMY,
		SCHED_METROPOLICE_ENEMY_RESISTING_ARREST,
		SCHED_METROPOLICE_WARN_TARGET,
		SCHED_METROPOLICE_HARASS_TARGET,
		SCHED_METROPOLICE_SUPPRESS_TARGET,
		SCHED_METROPOLICE_RETURN_FROM_HARASS,
		SCHED_METROPOLICE_SHOVE,
		SCHED_METROPOLICE_ACTIVATE_BATON,
		SCHED_METROPOLICE_DEACTIVATE_BATON,
		SCHED_METROPOLICE_ALERT_FACE_BESTSOUND,
		SCHED_METROPOLICE_RETURN_TO_PRECHASE,
		SCHED_METROPOLICE_SMASH_PROP,
		SCHED_METROPOLICE_KICK_GRENADE,		  //   [MODIFICATION]
#ifdef MAPBASE
		SCHED_METROPOLICE_FORCED_GRENADE_THROW,
		SCHED_METROPOLICE_MOVE_TO_FORCED_GREN_LOS,
		SCHED_METROPOLICE_RANGE_ATTACK2,
		SCHED_METROPOLICE_AR2_ALTFIRE,
#endif
	};

	enum 
	{
		TASK_METROPOLICE_HARASS = BaseClass::NEXT_TASK,
		TASK_METROPOLICE_DIE_INSTANTLY,
		TASK_METROPOLICE_BURST_ATTACK,
		TASK_METROPOLICE_STOP_FIRE_BURST,
		TASK_METROPOLICE_AIM_STITCH_AT_PLAYER,
		TASK_METROPOLICE_AIM_STITCH_AT_AIRBOAT,
		TASK_METROPOLICE_AIM_STITCH_TIGHTLY,
		TASK_METROPOLICE_AIM_STITCH_IN_FRONT_OF_AIRBOAT,
		TASK_METROPOLICE_AIM_STITCH_ALONG_SIDE_OF_AIRBOAT,
		TASK_METROPOLICE_AIM_STITCH_BEHIND_AIRBOAT,
		TASK_METROPOLICE_RELOAD_FOR_BURST,
		TASK_METROPOLICE_GET_PATH_TO_STITCH,
		TASK_METROPOLICE_RESET_LEDGE_CHECK_TIME,
		TASK_METROPOLICE_GET_PATH_TO_BESTSOUND_LOS,
		TASK_METROPOLICE_AIM_WEAPON_AT_ENEMY,
		TASK_METROPOLICE_ARREST_ENEMY,
		TASK_METROPOLICE_LEAD_ARREST_ENEMY,
		TASK_METROPOLICE_SIGNAL_FIRING_TIME,
		TASK_METROPOLICE_ACTIVATE_BATON,
		TASK_METROPOLICE_WAIT_FOR_SENTENCE,
		TASK_METROPOLICE_GET_PATH_TO_PRECHASE,
		TASK_METROPOLICE_CLEAR_PRECHASE,
		TASK_METROPOLICE_FACE_GRENADE,		// [MODIFICATION] // [MODIFICATION] An instantaneous task that enables a 'turbo turn' to quickly face the memorized grenade.
		TASK_METROPOLICE_KICK_LAUNCH,		// [MODIFICATION] An instantaneous task that applies kick physics to a nearby grenade.
		TASK_METROPOLICE_MEMORIZE_GRENADE,	// [MODIFICATION] An instantaneous task that "memorizes" the grenade as a target for the kick schedule.
		TASK_METROPOLICE_PLAY_KICK_ANIMATION, // [MODIFICATION] A blocking task that plays a specific kick animation by name and completes when the sequence is finished. 

#ifdef MAPBASE
		TASK_METROPOLICE_GET_PATH_TO_FORCED_GREN_LOS,
		TASK_METROPOLICE_DEFER_SQUAD_GRENADES,
		TASK_METROPOLICE_FACE_TOSS_DIR,
		TASK_METROPOLICE_PLAY_SEQUENCE_FACE_ALTFIRE_TARGET,
#endif
	};

private:

	EHANDLE			m_hPulledObject;			// [MODIFICATION] Safely stores a handle to the grenade being manipulated.
	float			m_flNextGrenadeCatchTime;	// [MODIFICATION] Shared cooldown timer for all special grenade abilities to prevent loops.
	bool			m_bForceFastTurn;			// [MODIFICATION] A temporary flag to enable a high-speed "turbo" turn, used by the MaxYawSpeed() override.

	int				m_iPistolClips;		// How many clips the cop has in reserve
	int				m_iManhacks;		// How many manhacks the cop has
	bool			m_fWeaponDrawn;		// Is my weapon drawn? (ready to use)
	bool			m_bSimpleCops;		// The easy version of the cops
	int				m_LastShootSlot;
	CRandSimTimer	m_TimeYieldShootSlot;
	CSimpleSimTimer m_BatonSwingTimer;
	CSimpleSimTimer m_NextChargeTimer;

	// All related to burst firing
	Vector			m_vecBurstTargetPos;
	Vector			m_vecBurstDelta;
	int				m_nBurstHits;
	int				m_nMaxBurstHits;
	int				m_nBurstReloadCount;
	Vector			m_vecBurstLineOfDeathDelta;
	Vector			m_vecBurstLineOfDeathOrigin;
	int				m_nBurstMode;
	int				m_nBurstSteerMode;
	float			m_flBurstSteerDistance;
	float			m_flBurstPredictTime;
	Vector			m_vecBurstPredictedVelocityDir;
	float			m_vecBurstPredictedSpeed;
	float			m_flValidStitchTime;
	float			m_flNextLedgeCheckTime;
	float			m_flTaskCompletionTime;
	
	bool			m_bShouldActivateBaton;
	float			m_flBatonDebounceTime;	// Minimum amount of time before turning the baton off
	float			m_flLastPhysicsFlinchTime;
	float			m_flLastDamageFlinchTime;
	
	// Sentences
	float			m_flNextPainSoundTime;
	float			m_flNextLostSoundTime;
	int				m_nIdleChatterType;
	bool			m_bPlayerIsNear;

	// Policing state
	bool			m_bPlayerTooClose;
	bool			m_bKeepFacingPlayer;
	float			m_flChasePlayerTime;
	Vector			m_vecPreChaseOrigin;
	float			m_flPreChaseYaw;
	int				m_nNumWarnings;
	int				m_iNumPlayerHits;

	// Outputs
	COutputEvent	m_OnStunnedPlayer;
	COutputEvent	m_OnCupCopped;
#ifdef MAPBASE
	COutputEHANDLE	m_OnHitByPhysicsObject;
	COutputEHANDLE	m_OutManhack;

	// Determines whether this NPC is allowed to use grenades or alt-fire stuff.
	eGrenadeCapabilities m_iGrenadeCapabilities;
	eGrenadeDropCapabilities m_iGrenadeDropCapabilities;
#endif

	AIHANDLE		m_hManhack;
	CHandle<CPhysicsProp>	m_hBlockingProp;

	CAI_ActBusyBehavior		m_ActBusyBehavior;
#ifdef MAPBASE
	CMetroPoliceStandoffBehavior	m_StandoffBehavior;
#else
	CAI_StandoffBehavior	m_StandoffBehavior;
#endif
	CAI_AssaultBehavior		m_AssaultBehavior;
	CAI_FuncTankBehavior	m_FuncTankBehavior;
	CAI_RappelBehavior		m_RappelBehavior;
	CAI_PolicingBehavior	m_PolicingBehavior;
	CAI_FollowBehavior		m_FollowBehavior;

#ifndef METROPOLICE_USES_RESPONSE_SYSTEM
	CAI_Sentence< CNPC_MetroPolice > m_Sentences;
#endif

	int				m_nRecentDamage;
	float			m_flRecentDamageTime;

	// The last hit direction, measured as a yaw relative to our orientation
	float			m_flLastHitYaw;

	static float	gm_flTimeLastSpokePeek;

public:
	DEFINE_CUSTOM_AI;
};

#endif // NPC_METROPOLICE_H
