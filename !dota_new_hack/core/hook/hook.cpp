﻿#include "hook.hpp"
#include "../../gui/gui.hpp"
#include "../../gui/panorama_gui.h"
#include "../features/inventory_changer.h"
#include "../features/hero_bar.h"
#include <Psapi.h>

extern CGui* pGui;
extern CPanoramaGUI panorama_gui;
extern CGameEntitySystem* g_pGameEntitySystem;
C_DOTAGamerules* g_pGameRules;


void EntityEventListener::OnEntityCreated( C_BaseEntity* rcx ) {
	if ( const auto client_class = rcx->client_class( ); ( client_class && client_class->m_pNetworkName ) ) {
		const auto class_name = std::string_view( client_class->m_pNetworkName );
		if ( class_name == "C_DOTAGamerulesProxy" ) { // class id 1438
			g_pGameRules = rcx->schema_member<C_DOTAGamerules*>( "client.dll/C_DOTAGamerulesProxy/m_pGameRules" );
		}

		if ( class_name.starts_with( "C_DOTA_Unit_Hero" ) || class_name.starts_with( "CDOTA_Unit_Hero" ) )
			cheat_data.heroes.insert( static_cast<C_DOTA_BaseNPC_Hero*>( rcx ) );

	}
}

void EntityEventListener::OnEntityDeleted( C_BaseEntity* rcx ) {
	if ( const auto client_class = rcx->client_class( ); ( client_class && client_class->m_pNetworkName ) ) {
		const auto class_name = std::string_view( client_class->m_pNetworkName );

		if ( class_name.starts_with( "C_DOTA_Unit_Hero" ) || class_name.starts_with( "CDOTA_Unit_Hero" ) && cheat_data.heroes.contains( static_cast<C_DOTA_BaseNPC_Hero*>( rcx ) ) )
			cheat_data.heroes.erase( static_cast<C_DOTA_BaseNPC_Hero*>( rcx ) );
	}
}

bool hook::functions::BAsyncSendProto( CProtoBufMsgBase* protobufMsg, IProtoBufSendHandler* handler, google::protobuf::Message* responseMsg, unsigned int respMsgID ) {
	//spdlog::debug( "sent message to GC: {}\n", protobufMsg->msgID );

	if ( protobufMsg->msgID == k_EMsgClientToGCEquipItems ) {
		features::inventory_changer.process_equips( (CMsgClientToGCEquipItems*)protobufMsg->msg );
	}

	return reinterpret_cast<decltype( &BAsyncSendProto )>( hook::original::fpBAsyncSendProto )( protobufMsg, handler, responseMsg, respMsgID );
}

EGCResults hook::functions::SGCRetrieveMessage( ISteamGameCoordinator* thisptr, uint32* punMsgType, ProtoBufMsgHeader_t* pubDest, uint32 cubDest, uint32* pcubMsgSize ) {
	const auto ret = reinterpret_cast<decltype( &SGCRetrieveMessage )>( hook::original::fpSGCRetrieveMessage )( thisptr, punMsgType, pubDest, cubDest, pcubMsgSize );
	const auto msg_id = *punMsgType & 0x7FFFFFFF;
	//spdlog::debug( "received message from GC: {}\n", msg_id );

	auto body_data = (void*)( ( (std::uintptr_t)pubDest ) + 8 );
	auto body_size = *pcubMsgSize - 8;
	if ( panorama_gui.log_searching_players && msg_id == 7198 ) { // k_EMsgGCMatchmakingStatsResponse
		CMsgDOTAMatchmakingStatsResponse body;

		if ( body.ParsePartialFromArray( body_data, body_size ) ) {
			constexpr const auto matchgroups = mapbox::eternal::map<std::uint8_t, mapbox::eternal::string>( {
				{ 0, "#dota_matchgroup_uswest" },
				{ 1, "#dota_matchgroup_useast"},
				{ 3, "#dota_matchgroup_sea"},
				{ 2, "#dota_matchgroup_eu" },
				{ 8, "#dota_matchgroup_vie" },
				{ 4, "#dota_matchgroup_cn" },
				{ 7, "#dota_matchgroup_ru" },
				{ 9, "#dota_matchgroup_au" },
				{ 10, "#dota_matchgroup_jnb" },
				{ 13, "#dota_matchgroup_dubai" },
				{ 5, "#dota_matchgroup_brazil" },
				{ 14, "#dota_matchgroup_chile" },
				{ 15, "#dota_matchgroup_peru" },
				{ 6, "#dota_matchgroup_argentina" },
				{ 16, "#dota_matchgroup_india" },
				{ 19, "#dota_matchgroup_japan" },
				{ 11, "#dota_matchgroup_pw_telecom_shanghai" },
				{ 12, "#dota_matchgroup_pw_unicom" },
				{ 17, "#dota_matchgroup_pw_telecom_guangdong" },
				{ 18, "#dota_matchgroup_pw_telecom_zhejiang" },
				{ 20, "#dota_matchgroup_pw_telecom_wuhan" },
				{ 25, "#dota_matchgroup_pw_unicom_tianjin" },
				{ 21, "#dota_matchgroup_taiwan" }
				} );

			for ( auto iterator = 0; iterator < body.match_groups_size( ); ++iterator ) {
				const auto& group = body.match_groups( iterator );

				if ( const auto searching = group.players_searching( ); searching > 0 ) {
					const auto res = CLocalize::GetInstance( ).FindStringSafely( matchgroups.at( iterator ).data( ) );
					std::wcout << util::utf8_decode( res ) << L": " << searching << "\n";
				}
			}
		}
	}
	if ( msg_id == 26 && panorama_gui.auto_accept ) { // k_ESOMsg_UpdateMultiple
		CMsgSOMultipleObjects body;
		if ( body.ParsePartialFromArray( body_data, body_size ) ) {

			if ( body.objects_modified_size( ) == 5 ) {
				const auto inversed_lobby_id = ~( CGCClient::get( ).lobby_manager( )->find_dota_lobby( )->lobby_id( ) );
				const uint32_t account_id = reinterpret_cast<CDOTAPlayerInventory*>( CGCClient::get( ).so_listeners( )[ 1 ] )->m_soid.m_steamid;

				CMsgReadyUp msg;
				msg.set_ready_up_key( inversed_lobby_id ^ ( account_id | ( (uint64_t)account_id << 32 ) ) );
				msg.set_state( DOTALobbyReadyState_ACCEPTED );
				ISteamClient::get( ).GetISteamGenericInterface( ISteamClient::GetHSteamPipe( ), ISteamClient::GetHSteamUser( ), "SteamGameCoordinator001" )
					->send_msg( msg, 7070 ); // k_EMsgGCReadyUp
			}
		}
	}

	return ret;
}

bool hook::functions::SendNetMessage( INetChannel* thisptr, NetMessageHandle_t* rdx, google::protobuf::Message* msg, NetChannelBufType_t type ) {
	bool ret = reinterpret_cast<decltype( &SendNetMessage )>( hook::original::fpSendNetMessage )( thisptr, rdx, msg, type );

	auto msg_handle = (CNetworkSerializerPB*)rdx;
	std::string messageName{ msg_handle->unscopedName };
	std::transform( messageName.begin( ), messageName.end( ), messageName.begin( ),
		[]( unsigned char c ) { return std::tolower( c ); } );

	if ( messageName.find( "cvar" ) != -1 || messageName.find( "convar" ) != -1 ) {
		char b[ 256 ]{ '\0' };
		spdlog::info( "sent {}:\n{}\n\n", msg_handle->unscopedName, msg_handle->protobufBinding->ToString( msg, &b ) );
	}

	return ret;
}

void* hook::functions::PostReceivedNetMessage( INetChannel* rcx, CNetworkSerializerPB* rdx, google::protobuf::Message* r8, NetChannelBufType_t* r9 ) {

	// if ( rdx->messageID != clc_Move && rdx->messageID != net_Tick && rdx->messageID != svc_PacketEntities && rdx->messageID != svc_UpdateStringTable
	// && rdx->messageID != 207 && rdx->messageID != 488 && rdx->messageID != 350 && rdx->messageID != 154 && rdx->messageID != 145 && rdx->messageID != 521 && rdx->messageID != 522 ) {
		// char b[ 256 ]{ '\0' };
		// spdlog::info( "received {}:\n{}\n\n", rdx->unscopedName, rdx->protobufBinding->ToString( r8, &b ) );
	// }

	std::string messageName{ rdx->unscopedName };
	std::transform( messageName.begin( ), messageName.end( ), messageName.begin( ),
		[]( unsigned char c ) { return std::tolower( c ); } );

	if ( messageName.find( "cvar" ) != -1 || messageName.find( "convar" ) != -1 ) {
		char b[ 256 ]{ '\0' };
		spdlog::info( "received {}:\n{}\n\n", rdx->unscopedName, rdx->protobufBinding->ToString( r8, &b ) );
	}

	if ( rdx->messageID == DOTA_UM_TE_DotaBloodImpact ) {
		return 0;
	}
	else if ( rdx->messageID == GE_SosStartSoundEvent ) {
		auto msg_ = (CMsgSosStartSoundEvent*)r8;
		C_BaseEntity* ent;

		if ( ent = g_pGameEntitySystem->find_entity( msg_->source_entity_index( ) ); !ent || !ent->client_class( ) )
			goto end;

		if ( util::fast_strstr( ent->client_class( )->m_pNetworkName, "Creep" ) ) {
			return 0;
		}
	}
	else if ( rdx->messageID == UM_ParticleManager ) {
		CUserMsg_ParticleManager* particle_manager = static_cast<CUserMsg_ParticleManager*>( r8 );

		if ( panorama_gui.draw_particles && particle_manager->type( ) == GAME_PARTICLE_MANAGER_EVENT_SET_FOW_PROPERTIES ) {
			particle_manager->mutable_set_particle_fow_properties( )->set_fow_radius( 999999.f );

		}
	}
	else if ( rdx->messageID == GE_Source1LegacyGameEvent ) {
		CGameEvent* deserialized = CGameEventManager::GetInstance( )->UnserializeEvent( (CMsgSource1LegacyGameEvent*)r8 );
		spdlog::info( "{}\n", deserialized->GetName( ) );
	}

	if ( rdx->messageID != net_Tick )
		goto end;

	if ( global::in_game = IEngineClient::GetInstance( ).in_game( ); global::in_game ) {

		if ( !global::g_LocalEntity ) {
			static C_DOTAPlayerController** players = (decltype( players ))util::get_absolute_address( util::find_pattern( global::client, "\x48\x8B\x05\xCC\xCC\xCC\xCC\x89\xBE", "", false ), 3, 7 );

			if ( const auto local_controller = players[ 0 ]; local_controller ) {
				global::g_LocalEntity = g_pGameEntitySystem->find_entity_by_handle<std::uintptr_t>( local_controller->assigned_hero( ) );

				ICVar::get( )[ "r_farz" ]->m_value.flt = 10000.f;
#ifdef _DEBUG
				ICVar::get( )[ "stats_display" ]->m_value.i32 = 5;
#endif
			}
		}

		static bool camera_hooked = false;
		if ( !camera_hooked && calls::GetCurrentCamera ) {
			if ( auto camera = reinterpret_cast<VClass*>( calls::GetCurrentCamera( ) ); camera ) {
				hook::install_hook( camera->GetVF( 9 ), &hook::functions::OnMouseWheeled, &hook::original::fpOnMouseWheeled, "C_DOTACamera::OnMouseWheeled" );
				camera_hooked = true;
			}
		}

		if ( g_pGameRules->game_state( ) == DOTA_GAMERULES_STATE_GAME_IN_PROGRESS || g_pGameRules->game_state( ) == DOTA_GAMERULES_STATE_PRE_GAME ) {

			if ( panorama_gui.draw_networthdelta ) {
				util::set_timer( []( ) {
					CUIPanel* DotaHud = CPanoramaUIEngine::GetInstance( )->engine_source2( )->find_panel( "DotaHud" );
					CPanel2D* topbar = DotaHud->find_child_traverse( "topbar" )->panel2d_as( );

					int goodguys_top = 0;
					int badguys_top = 0;
					for ( int i = 0; i != 64; ++i )
					{
						const int TeamNum = C_DOTA_PlayerResource::get( )->GetTeam( i );
						if ( TeamNum == 2 ) // radiant
							goodguys_top += C_DOTA_PlayerResource::get( )->GetNetWorthOfPlayer( i );
						else if ( TeamNum == 3 ) // dire
							badguys_top += C_DOTA_PlayerResource::get( )->GetNetWorthOfPlayer( i );
					}
					calls::CDOTA_Hud_Top_Bar__UpdateNetWorthDifference( topbar, goodguys_top, badguys_top );
				}, 4000 );
			}
		}
	}
	else {
		if ( !global::g_LocalEntity )
			goto end;

		global::camera_distance = 1200;
		global::g_LocalEntity = 0;
		g_pGameRules = nullptr;
		cheat_data.heroes.clear( );
	}

end:
	void* ret = reinterpret_cast<decltype( &PostReceivedNetMessage )>( hook::original::fpPostReceivedNetMessage )( rcx, rdx, r8, r9 );
	return ret;
}

long hook::functions::Present( IDXGISwapChain* pSwapchain, UINT SyncInterval, UINT Flags ) {
	static bool imgui_inited = false;
	if ( !imgui_inited ) {
		if ( pGui->init( pSwapchain ) ) imgui_inited = true;
	}

	ImGui_ImplDX11_NewFrame( );
	ImGui_ImplWin32_NewFrame( );
	ImGui::NewFrame( );

	// pGui->Render( );

	if ( panorama_gui.draw_health && g_pGameRules && ( g_pGameRules->game_state( ) == DOTA_GAMERULES_STATE_GAME_IN_PROGRESS ||
		 g_pGameRules->game_state( ) == DOTA_GAMERULES_STATE_PRE_GAME ) )
		features::hero_bar.draw_health( panorama_gui.draw_mana_bar );

	ImGui::Render( );

	pGui->pContext->OMSetRenderTargets( 1, &pGui->pRenderTargetView, nullptr );
	ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData( ) );
	return reinterpret_cast<decltype( &Present )>( hook::original::fpPresent )( pSwapchain, SyncInterval, Flags );
}

void hook::functions::OnMouseWheeled( CDOTA_Camera* rcx, int delta ) {
	reinterpret_cast<decltype( &OnMouseWheeled )>( hook::original::fpOnMouseWheeled )( rcx, delta );

	if ( pGui->mouse_distance && !pGui->show && global::in_game ) {
		if ( delta == -1 ) global::camera_distance += pGui->camera_step;
		else if ( delta == 1 ) global::camera_distance -= pGui->camera_step;
		if ( global::camera_distance > 3500 ) global::camera_distance = 3500;
		if ( global::camera_distance < 1100 ) global::camera_distance = 1100;

		if ( panorama_gui.camera_dist_slider ) {
			const auto casted2volvotype = static_cast<float>( global::camera_distance - 1100 ) / ( 3500 - 1100 );
			panorama_gui.camera_dist_slider->children( )[ 1 ]->panel2d_as<CSlider>( )->set_fl( casted2volvotype );
		}

		rcx->set_distance( static_cast<float>( global::camera_distance ) );
	}
}

LRESULT __stdcall hook::functions::WndProc( const HWND hWnd, const unsigned int uMsg, const uintptr_t wParam, const uintptr_t lParam ) {
	if ( uMsg == WM_KEYUP ) {
		if ( wParam == VK_F1 ) {
			// url("s2r://panorama/images/front_page/ti6_video_bg_psd.vtex")
			auto bg = CPanoramaUIEngine::GetInstance( )->engine_source2( )->find_panel( "DotaDashboard" )->find_child_traverse( "DashboardBackgroundManager" )->children( )[ 0 ];
			bg->set_style( "background-image: url(\"file://{resources}/ambg.vtex\");" );
		}
		if ( wParam == VK_F2 ) {

		}
		if ( wParam == VK_F3 ) {
			panorama_gui.show( );
		}
		if ( wParam == VK_INSERT ) {
			CPanoramaUIEngine::GetInstance( )->engine_source2( )->play_sound_effect( "ui_menu_activate_open" );
			pGui->show ^= true;
		}
		if ( wParam == VK_HOME ) {

			CDOTA_ParticleManager::GetInstance( )->destroy_own_particles( );

			constexpr auto unhook = []( void* ) -> unsigned long {
				global::camera_distance = 1200;
				global::g_LocalEntity = 0;
				global::g_mapItemIcons.clear( );
				global::g_mapSpellIcons.clear( );

				hook::uninstall_all_hooks( );
				ImGui_ImplDX11_Shutdown( );
				ImGui_ImplWin32_Shutdown( );
				ImGui::DestroyContext( );

				SetWindowLongPtrW( pGui->retWND( ), GWLP_WNDPROC, reinterpret_cast<LONG_PTR>( hook::original::fpWndProc ) );
				FreeConsole( );
				FreeLibraryAndExitThread( global::hModule, 0 );
			};
			CreateThread( nullptr, 0, unhook, nullptr, 0, 0 );
			return 1;
		}
	}

	if ( true && ImGui_ImplWin32_WndProcHandler( hWnd, uMsg, wParam, lParam ) ) return 1;

	return CallWindowProcA( (WNDPROC)hook::original::fpWndProc, hWnd, uMsg, wParam, lParam );
}



bool hook::install_hook( std::uintptr_t address, LPVOID function, LPVOID* original, std::string_view name ) {
	static bool init = false;
	if ( !init ) {
		MH_Initialize( );
		init = true;
	}

	if ( MH_CreateHook( reinterpret_cast<LPVOID>( address ), function, original ) == MH_ERROR_ALREADY_CREATED ) {
		spdlog::warn( "[hook::install_hook]: Hook {} is already created\n", name );
	}
	const auto status = MH_EnableHook( reinterpret_cast<LPVOID>( address ) );
	if ( status == MH_STATUS::MH_OK ) {
		hook::hooks[ address ] = name;
	}
	else {
		spdlog::error( "[hook::install_hook]: Hook {} is not installed\n", name );
	}
	return status == MH_STATUS::MH_OK;
}

bool hook::uninstall_hook( std::uintptr_t address ) {
	const auto status = MH_DisableHook( reinterpret_cast<LPVOID>( address ) ) == MH_STATUS::MH_OK;

	if ( status && hook::hooks.contains( address ) ) {
		const auto& name = hook::hooks.at( address );
		hook::hooks.erase( address );
	}

	return status;
}

void hook::uninstall_all_hooks( ) {
	for ( const auto& addr : hooks ) {
		hook::uninstall_hook( addr.first );
		MH_RemoveHook( reinterpret_cast<LPVOID>( addr.first ) );
	}
}