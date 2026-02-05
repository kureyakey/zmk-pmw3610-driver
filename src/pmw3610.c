#define TRANSFORMED_BINDINGS(n) \
    { LISTIFY(DT_PROP_LEN(n, bindings), ZMK_KEYMAP_EXTRACT_BINDING, (, ), n) }

/* ===== BALL ACTION DEFINITIONS (per instance) ===== */

#define BALL_ACTIONS_INST(child)                                                    \
    static struct zmk_behavior_binding                                               \
        ball_action_config_##child##_bindings[DT_PROP_LEN(child, bindings)] =       \
            TRANSFORMED_BINDINGS(child);                                             \
                                                                                    \
    static struct ball_action_cfg ball_action_cfg_##child = {                        \
        .bindings_len = DT_PROP_LEN(child, bindings),                                \
        .bindings = ball_action_config_##child##_bindings,                           \
        .layers = DT_PROP(child, layers),                                            \
        .layers_len = DT_PROP_LEN(child, layers),                                    \
        .tick = DT_PROP_OR(child, tick, CONFIG_PMW3610_BALL_ACTION_TICK),            \
        .wait_ms = DT_PROP_OR(child, wait_ms, 0),                                    \
        .tap_ms = DT_PROP_OR(child, tap_ms, 0),                                      \
    };

#define BALL_ACTIONS_ITEM(child) &ball_action_cfg_##child,
#define BALL_ACTIONS_LEN(inst) (DT_INST_FOREACH_CHILD(inst, BALL_ACTIONS_UTIL_ONE) 0)
#define BALL_ACTIONS_UTIL_ONE(child) 1 +

/* ===== PMW3610 DEVICE DEFINE ===== */

#define PMW3610_DEFINE(n)                                                            \
    DT_INST_FOREACH_CHILD(n, BALL_ACTIONS_INST)                                      \
                                                                                    \
    static struct pixart_data data##n;                                                \
                                                                                    \
    static int32_t scroll_layers##n[] =                                               \
        DT_PROP(DT_DRV_INST(n), scroll_layers);                                      \
                                                                                    \
    static int32_t snipe_layers##n[] =                                                \
        DT_PROP(DT_DRV_INST(n), snipe_layers);                                       \
                                                                                    \
    static struct ball_action_cfg *ball_actions##n[] = {                             \
        DT_INST_FOREACH_CHILD(n, BALL_ACTIONS_ITEM)                                  \
    };                                                                               \
                                                                                    \
    static const struct pixart_config config##n = {                                  \
        .irq_gpio = GPIO_DT_SPEC_INST_GET(n, irq_gpios),                              \
        .bus = {                                                                     \
            .bus = DEVICE_DT_GET(DT_INST_BUS(n)),                                    \
            .config = {                                                              \
                .frequency = DT_INST_PROP(n, spi_max_frequency),                     \
                .operation = SPI_WORD_SET(8) | SPI_TRANSFER_MSB |                    \
                             SPI_MODE_CPOL | SPI_MODE_CPHA,                          \
                .slave = DT_INST_REG_ADDR(n),                                        \
            },                                                                       \
        },                                                                           \
        .cs_gpio = SPI_CS_GPIOS_DT_SPEC_GET(DT_DRV_INST(n)),                          \
        .scroll_layers = scroll_layers##n,                                           \
        .scroll_layers_len = DT_PROP_LEN(DT_DRV_INST(n), scroll_layers),             \
        .snipe_layers = snipe_layers##n,                                             \
        .snipe_layers_len = DT_PROP_LEN(DT_DRV_INST(n), snipe_layers),               \
        .ball_actions = ball_actions##n,                                             \
        .ball_actions_len = ARRAY_SIZE(ball_actions##n),                             \
    };                                                                               \
                                                                                    \
    DEVICE_DT_INST_DEFINE(                                                           \
        n,                                                                           \
        pmw3610_init,                                                                \
        NULL,                                                                        \
        &data##n,                                                                    \
        &config##n,                                                                  \
        POST_KERNEL,                                                                 \
        CONFIG_SENSOR_INIT_PRIORITY,                                                 \
        NULL                                                                         \
    );

DT_INST_FOREACH_STATUS_OKAY(PMW3610_DEFINE)
