#include "bn_core.h"
#include "bn_log.h"
#include "bn_math.h"
#include "bn_keypad.h"
#include "bn_display.h"
#include "bn_bg_palettes.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_string.h"
#include "bn_vector.h"
#include "bn_fixed_point.h"
#include "bn_sound.h"
#include "bn_music.h"

// We'll add sprite includes once we have graphics
// #include "bn_sprite_items_player_car.h"
// #include "bn_sprite_items_opponent_car.h"

// Font for debug text
#include "common_info.h"
#include "common_variable_8x16_sprite_font.h"

namespace {
    // Audio modes
    enum class AudioMode {
        STANDARD,  // Basic front/back with panning
        ILD        // Interaural Level Difference mode
    };
    
    // Car structure for opponents
    struct OpponentCar {
        bn::fixed x;
        bn::fixed y;
        bn::fixed z;  // Distance along track (positive = in front, negative = behind)
        bn::fixed speed;
        bool active;
        
        OpponentCar() : x(0), y(0), z(0), speed(1), active(false) {}
    };
    
    // Player state
    struct Player {
        bn::fixed x;
        bn::fixed y;
        bn::fixed speed;
        
        Player() : x(0), y(0), speed(0) {}
    };
    
    // Audio parameters for debugging
    struct AudioDebugInfo {
        bn::fixed pan;           // -1 to 1
        bn::fixed volume;        // 0 to 1
        bn::fixed left_level;    // For ILD mode
        bn::fixed right_level;   // For ILD mode
        bool is_front;          // Which sample is playing
        bn::fixed distance;      // Distance to nearest car
        
        AudioDebugInfo() : pan(0), volume(1), left_level(1), right_level(1), 
                           is_front(true), distance(0) {}
    };
}

class PsychoacousticDemo {
private:
    Player player;
    bn::vector<OpponentCar, 4> opponents;
    AudioMode current_mode;
    AudioDebugInfo audio_debug;
    bool debug_display_enabled;
    
    // Text generators for debug display
    bn::sprite_text_generator text_generator;
    bn::vector<bn::sprite_ptr, 32> text_sprites;
    
public:
    PsychoacousticDemo() : 
        current_mode(AudioMode::STANDARD),
        debug_display_enabled(true),
        text_generator(common::variable_8x16_sprite_font) {
        
        // Initialize display
        bn::bg_palettes::set_transparent_color(bn::color(0, 0, 0));
        
        // Initialize player position (center bottom)
        player.x = 0;
        player.y = 40;
        player.speed = 0;
        
        // Initialize opponent cars
        spawn_opponent(0, -60, 100, 2);  // Car in front
        spawn_opponent(40, -20, -50, 1.5);  // Car behind right
        
        BN_LOG("Psychoacoustic Demo initialized");
        BN_LOG("Press A to toggle audio mode (Standard/ILD)");
        BN_LOG("Press B to toggle debug display");
        BN_LOG("D-Pad to move player car");
    }
    
    void spawn_opponent(bn::fixed x, bn::fixed y, bn::fixed z, bn::fixed speed) {
        for(auto& car : opponents) {
            if(!car.active) {
                car.x = x;
                car.y = y;
                car.z = z;
                car.speed = speed;
                car.active = true;
                break;
            }
        }
    }
    
    void update_player() {
        // Handle input
        if(bn::keypad::left_held()) {
            player.x = bn::max(player.x - 2, bn::fixed(-100));
        }
        if(bn::keypad::right_held()) {
            player.x = bn::min(player.x + 2, bn::fixed(100));
        }
        if(bn::keypad::up_held()) {
            player.speed = bn::min(player.speed + bn::fixed(0.1), bn::fixed(3));
        }
        if(bn::keypad::down_held()) {
            player.speed = bn::max(player.speed - bn::fixed(0.1), bn::fixed(-1));
        }
        
        // Toggle modes
        if(bn::keypad::a_pressed()) {
            current_mode = (current_mode == AudioMode::STANDARD) ? 
                          AudioMode::ILD : AudioMode::STANDARD;
            BN_LOG("Audio mode: ", (current_mode == AudioMode::STANDARD) ? 
                   "STANDARD" : "ILD");
        }
        
        if(bn::keypad::b_pressed()) {
            debug_display_enabled = !debug_display_enabled;
        }
    }
    
    void update_opponents() {
        for(auto& car : opponents) {
            if(car.active) {
                // Move car relative to player speed
                car.z -= player.speed;
                
                // Simple oscillation for lateral movement
                car.x += bn::sin(car.z * 16) * bn::fixed(0.5);
                
                // Wrap around (respawn)
                if(car.z < -200) {
                    car.z = 200;
                } else if(car.z > 200) {
                    car.z = -200;
                }
            }
        }
    }
    
    void calculate_audio_parameters() {
        // Find nearest car
        bn::fixed min_distance = 1000;
        OpponentCar* nearest = nullptr;
        
        for(auto& car : opponents) {
            if(car.active) {
                bn::fixed dist = bn::abs(car.z);
                if(dist < min_distance) {
                    min_distance = dist;
                    nearest = &car;
                }
            }
        }
        
        if(nearest) {
            audio_debug.distance = min_distance;
            audio_debug.is_front = nearest->z > 0;
            
            // Calculate pan based on x position
            bn::fixed x_diff = nearest->x - player.x;
            audio_debug.pan = bn::max(bn::fixed(-1), 
                             bn::min(bn::fixed(1), x_diff / 100));
            
            // Calculate volume based on distance
            audio_debug.volume = bn::max(bn::fixed(0), 
                                bn::fixed(1) - (min_distance / 200));
            
            if(current_mode == AudioMode::STANDARD) {
                // Standard panning: equal power panning
                bn::fixed pan_angle = (audio_debug.pan + 1) * 45;  // 0-90 degrees
                audio_debug.left_level = bn::cos(pan_angle * 16) * audio_debug.volume;
                audio_debug.right_level = bn::sin(pan_angle * 16) * audio_debug.volume;
            } else {
                // ILD mode: simulate head shadow effect
                if(audio_debug.pan < 0) {
                    // Sound from left: right ear gets attenuated
                    audio_debug.left_level = audio_debug.volume;
                    bn::fixed shadow_factor = bn::fixed(1) + audio_debug.pan;  // 0 to 1
                    audio_debug.right_level = audio_debug.volume * shadow_factor * bn::fixed(0.6);
                } else {
                    // Sound from right: left ear gets attenuated
                    audio_debug.right_level = audio_debug.volume;
                    bn::fixed shadow_factor = bn::fixed(1) - audio_debug.pan;  // 0 to 1
                    audio_debug.left_level = audio_debug.volume * shadow_factor * bn::fixed(0.6);
                }
                
                // Additional attenuation for sounds behind (head shadow)
                if(!audio_debug.is_front) {
                    audio_debug.left_level *= bn::fixed(0.7);
                    audio_debug.right_level *= bn::fixed(0.7);
                }
            }
        }
        
        // Apply audio (placeholder - would use actual sound API here)
        apply_audio();
    }
    
    void apply_audio() {
        // This is where we would apply the actual audio
        // For now, we'll just log the parameters periodically
        static int frame_counter = 0;
        frame_counter++;
        
        if(frame_counter % 60 == 0) {  // Log once per second
            BN_LOG("Audio - Mode:", (current_mode == AudioMode::STANDARD) ? "STD" : "ILD",
                   " Front:", audio_debug.is_front,
                   " Pan:", audio_debug.pan,
                   " Vol:", audio_debug.volume);
        }
    }
    
    void render_debug() {
        if(!debug_display_enabled) {
            text_sprites.clear();
            return;
        }
        
        text_sprites.clear();
        
        // Display debug info
        bn::string<32> mode_text("Mode: ");
        mode_text.append(current_mode == AudioMode::STANDARD ? "STANDARD" : "ILD");
        text_generator.generate(-100, -70, mode_text, text_sprites);
        
        bn::string<32> sample_text("Sample: ");
        sample_text.append(audio_debug.is_front ? "FRONT" : "BACK");
        text_generator.generate(-100, -55, sample_text, text_sprites);
        
        bn::string<32> pan_text("Pan: ");
        pan_text.append(bn::to_string<8>(audio_debug.pan));
        text_generator.generate(-100, -40, pan_text, text_sprites);
        
        bn::string<32> vol_text("Volume: ");
        vol_text.append(bn::to_string<8>(audio_debug.volume));
        text_generator.generate(-100, -25, vol_text, text_sprites);
        
        bn::string<32> left_text("L: ");
        left_text.append(bn::to_string<8>(audio_debug.left_level));
        text_generator.generate(-100, -10, left_text, text_sprites);
        
        bn::string<32> right_text("R: ");
        right_text.append(bn::to_string<8>(audio_debug.right_level));
        text_generator.generate(-100, 5, right_text, text_sprites);
        
        bn::string<32> dist_text("Dist: ");
        dist_text.append(bn::to_string<8>(audio_debug.distance));
        text_generator.generate(-100, 20, dist_text, text_sprites);
        
        // Controls hint
        text_generator.generate(-100, 60, "A:Mode B:Debug", text_sprites);
    }
    
    void render_cars() {
        // Visual representation using simple shapes
        // We'll use text characters as placeholders for now
        
        // Clear any existing car sprites
        static bn::vector<bn::sprite_ptr, 8> car_sprites;
        car_sprites.clear();
        
        // Render player car at bottom
        text_generator.generate(player.x.integer(), player.y.integer(), "P", car_sprites);
        
        // Render opponent cars
        for(auto& car : opponents) {
            if(car.active) {
                // Scale position based on Z distance for perspective
                bn::fixed scale = bn::fixed(1) - (car.z / 400);
                bn::fixed screen_x = car.x * scale;
                bn::fixed screen_y = car.y - (car.z / 10);
                
                if(screen_y > -80 && screen_y < 80) {
                    bn::string<2> car_char = car.z > 0 ? "^" : "v";
                    text_generator.generate(screen_x.integer(), screen_y.integer(), 
                                          car_char, car_sprites);
                }
            }
        }
    }
    
    void update() {
        update_player();
        update_opponents();
        calculate_audio_parameters();
        render_debug();
        render_cars();
    }
};

int main() {
    bn::core::init();
    
    BN_LOG("=================================");
    BN_LOG("F-Zero Psychoacoustic Demo");
    BN_LOG("Testing GBA spatial audio");
    BN_LOG("=================================");
    
    PsychoacousticDemo demo;
    
    while(true) {
        demo.update();
        bn::core::update();
    }
}
