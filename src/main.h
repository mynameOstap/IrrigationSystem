#include <stdint.h>

#define ZONE_COUNT 2
#define CHECK_INTERVAL_MS  (1000)
#define PULSE_MS           (60LL * 500)
#define SOAK_MS            (15LL * 60 * 1000)
#define COOLDOWN_MS        (6LL * 60 * 60 * 1000) 

typedef enum {
    ST_IDLE,       // чекаємо, періодично міряємо
    ST_WATERING,   // насос працює
    ST_SOAKING,    // насос стоїть, вода вбирається
    ST_BLOCKED,    // сервіс помилок заборонив полив
} zone_state_t;

typedef struct {
    uint8_t      id;
    zone_state_t state;
    int64_t      state_since_ms;   
    int64_t      last_check_ms;
    int64_t      cooldown_until_ms;
    uint8_t      pulses;
    uint64_t      moisture_before;
} zone_ctx_t;