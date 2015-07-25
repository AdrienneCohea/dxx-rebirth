/*
 * Portions of this file are copyright Rebirth contributors and licensed as
 * described in COPYING.txt.
 * Portions of this file are copyright Parallax Software and licensed
 * according to the Parallax license below.
 * See COPYING.txt for license details.

THE COMPUTER CODE CONTAINED HEREIN IS THE SOLE PROPERTY OF PARALLAX
SOFTWARE CORPORATION ("PARALLAX").  PARALLAX, IN DISTRIBUTING THE CODE TO
END-USERS, AND SUBJECT TO ALL OF THE TERMS AND CONDITIONS HEREIN, GRANTS A
ROYALTY-FREE, PERPETUAL LICENSE TO SUCH END-USERS FOR USE BY SUCH END-USERS
IN USING, DISPLAYING,  AND CREATING DERIVATIVE WORKS THEREOF, SO LONG AS
SUCH USE, DISPLAY OR CREATION IS FOR NON-COMMERCIAL, ROYALTY OR REVENUE
FREE PURPOSES.  IN NO EVENT SHALL THE END-USER USE THE COMPUTER CODE
CONTAINED HEREIN FOR REVENUE-BEARING PURPOSES.  THE END-USER UNDERSTANDS
AND AGREES TO THE TERMS HEREIN AND ACCEPTS THE SAME BY USE OF THIS FILE.
COPYRIGHT 1993-1999 PARALLAX SOFTWARE CORPORATION.  ALL RIGHTS RESERVED.
*/

#pragma once

#include <physfs.h>
#include "fwdobject.h"

#define MAX_PLAYERS 8u
#define MAX_MULTI_PLAYERS MAX_PLAYERS+3
#define MULTI_PNUM_UNDEF 0xcc

// Initial player stat values
#define INITIAL_ENERGY  i2f(100)    // 100% energy to start
#define INITIAL_SHIELDS i2f(100)    // 100% shields to start

#define MAX_ENERGY      i2f(200)    // go up to 200
#define MAX_SHIELDS     i2f(200)

#define INITIAL_LIVES               3   // start off with 3 lives

// Values for special flags
#define PLAYER_FLAGS_INVULNERABLE   1       // Player is invincible
#define PLAYER_FLAGS_BLUE_KEY       2       // Player has blue key
#define PLAYER_FLAGS_RED_KEY        4       // Player has red key
#define PLAYER_FLAGS_GOLD_KEY       8       // Player has gold key
#define PLAYER_FLAGS_MAP_ALL        64      // Player can see unvisited areas on map
#define PLAYER_FLAGS_QUAD_LASERS    1024    // Player shoots 4 at once
#define PLAYER_FLAGS_CLOAKED        2048    // Player is cloaked for awhile
#if defined(DXX_BUILD_DESCENT_II)
#define PLAYER_FLAGS_FLAG           16      // Player has his team's flag
#define PLAYER_FLAGS_AMMO_RACK      128     // Player has ammo rack
#define PLAYER_FLAGS_CONVERTER      256     // Player has energy->shield converter
#define PLAYER_MAX_AMMO(PLR,BASE)	((PLR.flags & PLAYER_FLAGS_AMMO_RACK) ? BASE * 2 : BASE)
#define PLAYER_FLAGS_AFTERBURNER    4096    // Player's afterburner is engaged
#define PLAYER_FLAGS_HEADLIGHT      8192    // Player has headlight boost
#define PLAYER_FLAGS_HEADLIGHT_ON   16384   // is headlight on or off?

#define AFTERBURNER_MAX_TIME    (F1_0*5)    // Max time afterburner can be on.
#endif
#define CALLSIGN_LEN                8       // so can use as filename (was: 12)

// Amount of time player is cloaked.
#define CLOAK_TIME_MAX          (F1_0*30)
#define INVULNERABLE_TIME_MAX   (F1_0*30)

#if defined(DXX_BUILD_DESCENT_I)
#define PLAYER_STRUCT_VERSION 	16		//increment this every time player struct changes
#define PLAYER_MAX_AMMO(PLR,BASE)	(static_cast<void>(PLR), BASE)
#elif defined(DXX_BUILD_DESCENT_II)
#define PLAYER_STRUCT_VERSION   17  // increment this every time player struct changes

// defines for teams
#define TEAM_BLUE   0
#define TEAM_RED    1
#endif

#ifdef __cplusplus
#include "dxxsconf.h"
#include "compiler-array.h"
struct callsign_t;

#if defined(DXX_BUILD_DESCENT_I) || defined(DXX_BUILD_DESCENT_II)
struct player;
struct player_rw;
#endif

#define N_PLAYER_GUNS 8
#define N_PLAYER_SHIP_TEXTURES 32

struct player_ship;

typedef unsigned playernum_t;
typedef array<playernum_t, MAX_PLAYERS> playernum_array_t;

extern unsigned N_players;   // Number of players ( >1 means a net game, eh?)
extern playernum_t Player_num;  // The player number who is on the console.

#if defined(DXX_BUILD_DESCENT_I) || defined(DXX_BUILD_DESCENT_II)
#if defined(DXX_BUILD_DESCENT_I)
#define DXX_PLAYER_HEADER_ADD_EXTRA_PLAYERS	0
#elif defined(DXX_BUILD_DESCENT_II)
#define DXX_PLAYER_HEADER_ADD_EXTRA_PLAYERS	4
#endif
extern array<player, MAX_PLAYERS + DXX_PLAYER_HEADER_ADD_EXTRA_PLAYERS> Players;   // Misc player info
void player_rw_swap(player_rw *p, int swap);
#endif

extern array<object *, MAX_PLAYERS> Guided_missile;
extern array<object_signature_t, MAX_PLAYERS> Guided_missile_sig;

/*
 * reads a player_ship structure from a PHYSFS_file
 */
void player_ship_read(player_ship *ps, PHYSFS_File *fp);

#endif
