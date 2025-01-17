#pragma once

#include <cstdint>

enum class GameState : int
{
    DOTA_GAMERULES_STATE_INIT = 0,
    DOTA_GAMERULES_WAIT_FOR_PLAYERS_TO_LOAD,
    DOTA_GAMERULES_HERO_SELECTION,
    DOTA_GAMERULES_STRATEGY_TIME,
    DOTA_GAMERULES_PREGAME,
    DOTA_GAMERULES_GAME_IN_PROGRESS,
    DOTA_GAMERULES_POSTGAME,
    DOTA_GAMERULES_DISCONNECT,
    DOTA_GAMERULES_TEAM_SHOWCASE,
    DOTA_GAMERULES_CUSTOM_GAME_SETUP,
    DOTA_GAMERULES_WAIT_FOR_MAP_TO_LOAD
};

class CGameEvent
{
public:
    virtual ~CGameEvent() {};

    virtual const char* GetName(void) = 0;
    virtual int GetID(void) = 0;

    virtual bool IsReliable(void) = 0;
    virtual bool IsLocal(void) = 0;
    virtual bool IsEmpty(char const* keyname = nullptr) = 0;

    virtual bool GetBool(char const* keyname = nullptr, bool defaultValue = false) = 0;
    virtual int GetInt(char const* keyname = nullptr, int defaultValue = 0) = 0;
    virtual uint64_t GetUint64(char const* keyname = nullptr, uint64_t DefaultValue = 0) = 0;
    virtual float GetFloat(char const* keyname = nullptr, float defaultValue = 0.0f) = 0;
    virtual const char* GetString(char const* keyname = nullptr, char const* defaultValue = nullptr) = 0;
    virtual const void* GetPtr(char const* keyname = nullptr) = 0;

    virtual void SetBool(char const* keyname, bool value) = 0;
    virtual void SetInt(char const* keyname, int value) = 0;
    virtual void SetUint64(char const* keyname, uint64_t value) = 0;
    virtual void SetFloat(char const* keyname, float value) = 0;
    virtual void SetString(char const* keyname, char const* value) = 0;
    virtual void SetPtr(char const* keyname, void const* value) = 0;

    virtual void* GetDataKeys(void) = 0;
};

class CMsgSource1LegacyGameEvent;

// xref "CGameEventManager::AddListener" to AddListener()
class CGameEventManager
{
public:
    static CGameEventManager* GetInstance( ) {
        if ( auto source2client = CSource2Client::get( ); ( source2client ) ) {
            return *(CGameEventManager**)util::get_absolute_address( util::vmt( (uintptr_t)source2client, 4 ) + 0x70, 3, 7 );
        }

        return nullptr;
    }
    virtual ~CGameEventManager() {};
    virtual int LoadEventsFromFile(const char* filename) = 0;
    virtual void Reset(void) = 0;
    virtual bool AddListener(void* listener, const char* eventName, bool serverSide) = 0;
    virtual bool FindListener(void* listener, const char* eventName) = 0;
    virtual void RemoveListener(void* listener) = 0;
    virtual CGameEvent* CreateEvente(const char* name, bool force = false, int* cookie = nullptr) = 0;
    virtual bool FireEvent(CGameEvent* event, bool dontBroadcast) = 0;
    virtual bool FireEventClientSide(CGameEventManager* thisptr, CGameEvent* ev) = 0;
    virtual CGameEvent* DuplicateEvent(CGameEvent* event) = 0;
    virtual void FreeEvent(CGameEvent* event) = 0;
    virtual bool SerializeEvent(CGameEvent* event, void* Source1LegacyGameEvent) = 0;
    virtual CGameEvent* UnserializeEvent(CMsgSource1LegacyGameEvent* ev) = 0;
    virtual bool LookupEventId(const char* id) = 0;
};