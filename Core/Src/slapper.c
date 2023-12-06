/**
 * @brief   Runs the state machine for the slapping game
 *
 * @author Tom Schmitz
 */

#include "timers.h"
#include <sensors.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __DEBUG__
#define FILE_STATIC
#else
#include <assert.h>
#define FILE_STATIC static

typedef enum {
    SLAPPER_IDLE,
    SLAPPER_RANDOMIZE,
    SLAPPER_CHECK_HAND,
    SLAPPER_RUN_TIMER,
    SLAPPER_PAUSE,
    SLAPPER_REACTION,
    SLAPPER_CHECK_REACTION,
    SLAPPER_ACTIVATE,
    SLAPPER_RESET,
    SLAPPER_UPDATE_CPU_SCORE,
    SLAPPER_PLAY_AGAIN,
    SLAPPER_UPDATE_USER_SCORE
} _slap_game_state_t;
#endif

// FILE_STATIC _slap_game_state_t currentState = SLAPPER_IDLE;
// FILE_STATIC uint32_t difficulty_buffer = 0;

/**
 * @brief Crappy algorithm to generate not so random numbers, but works for
 * this purpose. Ideally would want to take in values from the counter, ADC,
 * and some other sensor and use an appropriate algorithm to generate an actual
 * random number. But this should do for the prototype
 */
/*FILE_STATIC uint32_t genrand(void)
{
        const uint32_t scaler = 17;
        const uint32_t offset = 100;

        uint32_t random = read_heartbeat();

        return (random + offset) * scaler;
}

FILE_STATIC void run_state_machine(bool start, bool pause, bool ir_sensors[],
uint32_t fsr)
{
    static uint32_t buffer = 0;
    static uint32_t Random = 0;
    static uint32_t count = 0;
    bool hand_placed = false, actuator_done = false;


    switch(currentState)
    {
        case SLAPPER_IDLE:
                kick_the_watchdog();
            currentState = (start) ? SLAPPER_RANDOMIZE : SLAPPER_IDLE;
            break;
        case SLAPPER_RANDOMIZE:
            buffer = 0;
            count = 0;
            Random = genrand();
            currentState = SLAPPER_CHECK_HAND;
            break;
        case SLAPPER_CHECK_HAND:
            // Check sensors here
            currentState = (hand_placed) ? SLAPPER_RUN_TIMER :
SLAPPER_CHECK_HAND; break; case SLAPPER_RUN_TIMER:
            // Check sensors
            currentState = (!hand_placed || pause) ? SLAPPER_PAUSE : (count++ >=
Random) ? SLAPPER_REACTION : SLAPPER_RUN_TIMER; kick_the_watchdog(); break; case
SLAPPER_REACTION: currentState = (buffer++ >= difficulty_buffer) ?
SLAPPER_CHECK_REACTION : SLAPPER_REACTION; break; case SLAPPER_CHECK_REACTION:
            // Check sensors
            currentState = (hand_placed) ? SLAPPER_ACTIVATE :
SLAPPER_UPDATE_USER_SCORE; break; case SLAPPER_ACTIVATE:
            // Activate actuator
            // Check actuator position
            currentState = (actuator_done) ? SLAPPER_RESET : SLAPPER_ACTIVATE;
            break;
        case SLAPPER_RESET:
            // Reset actuator - make this blocking
            currentState = SLAPPER_UPDATE_CPU_SCORE;
            break;
        case SLAPPER_UPDATE_CPU_SCORE:
            currentState = SLAPPER_PLAY_AGAIN;
            break;
        case SLAPPER_PLAY_AGAIN:
            currentState = (start) ? SLAPPER_RANDOMIZE : SLAPPER_PLAY_AGAIN;
            break;
        case SLAPPER_UPDATE_USER_SCORE:
            currentState = SLAPPER_PLAY_AGAIN;
            break;
        case SLAPPER_PAUSE:
            currentState = (start) ? SLAPPER_CHECK_HAND : SLAPPER_PAUSE;
            break;
        default:
            assert(false);
    }
}
*/
