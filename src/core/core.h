/*
 * core.h - everything the game core modules share.
 *
 * The core is pure C with no OS calls: a whole frame is drawn into a 480x270 pixel buffer, sounds are
 * synthesised into sample arrays, and the platform layer (see game.h) only shows the buffer, plays the
 * sounds and forwards input. Each module has its own header; this one includes them all, in order.
 */
#ifndef NCS_CORE_H
#define NCS_CORE_H

#include "game.h"
#include <math.h>
#include <stdarg.h>
#include <string.h>

#include "base_common.h"
#include "audio_sound.h"
#include "audio_music.h"
#include "base_sprites.h"
#include "base_draw.h"
#include "world_districts.h"
#include "world_scenery.h"
#include "world_layers.h"
#include "world_physics.h"
#include "gameplay_state.h"
#include "gameplay_progress.h"
#include "gameplay_pets.h"
#include "gameplay_save.h"
#include "gameplay_daily.h"
#include "gameplay_achievements.h"
#include "gameplay_extra.h"
#include "gameplay_spawn.h"
#include "gameplay_combat.h"
#include "gameplay_enemies.h"
#include "gameplay_player.h"
#include "gameplay_update.h"
#include "render_weather.h"
#include "render_giants.h"
#include "render_entities.h"
#include "render_hud.h"
#include "render_cursors.h"
#include "render_pets.h"
#include "ui_style.h"
#include "ui_title.h"
#include "ui_layout.h"
#include "ui_actions.h"
#include "ui_widgets.h"
#include "ui_screens_shop.h"
#include "ui_screens_options.h"
#include "ui_screens_misc.h"
#include "ui_screens_extras.h"
#include "ui_frame.h"
#include "api.h"
#include "api_pad.h"

#endif
