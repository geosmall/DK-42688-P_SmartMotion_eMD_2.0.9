#ifdef __cplusplus
extern "C" {
#endif

#include "gpio.h"
#include "util/hal_map.h"

struct gpio_mapping {
	// Pio * p_pio;
	// uint32_t pio_id;
	// uint32_t pio_index;
	// uint32_t pio_mask;
	// pio_type_t pio_type;
	// uint32_t pio_attributes;
	// IRQn_Type pio_irqn;
	GPIO_TypeDef* GPIOx;
	GPIO_InitTypeDef GPIO_Init;
	void (*callback) (void *context, unsigned pin_num);
	void* context;
};

static struct gpio_mapping gm[INV_GPIO_MAX] = {
    {
        // GPIO_INV_SENSOR_INT1
        .GPIOx = nullptr,
        .GPIO_Init = {
            .Pin = 0,
            .Mode = GPIO_MODE_IT_RISING,
            .Pull = GPIO_NOPULL,
            .Speed = GPIO_SPEED_FREQ_MEDIUM
        },
        .callback = 0,
        .context = 0
    },
    {
        // GPIO_INV_SENSOR_INT2
        .GPIOx = nullptr,
        .GPIO_Init = {
            .Pin = 0,
            .Mode = GPIO_MODE_IT_RISING,
            .Pull = GPIO_NOPULL,
            .Speed = GPIO_SPEED_FREQ_MEDIUM
        },
        .callback = 0,
        .context = 0
    }
};

void EXTI2_IRQHandler(void);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
static void gpio_common_callback(const uint32_t id, const uint32_t index);
void inv_gpio_sensor_irq_init(unsigned pin_num,
        void (*interrupt_cb)(void * context, unsigned pin_num), void * context);

extern "C" void EXTI2_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
}

/**
  * @brief EXTI line detection callbacks
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == gm[INV_GPIO_INT1].GPIO_Init.Pin) {
        if (gm[INV_GPIO_INT1].callback) {
            // static void ext_interrupt_cb(void *context, unsigned int int_num)
            gm[INV_GPIO_INT1].callback(gm[INV_GPIO_INT1].context, INV_GPIO_INT1);
        }
    }
}

// static void gpio_common_callback(const uint32_t id, const uint32_t index)
// {
    // for(unsigned i=0; i<sizeof(gm)/sizeof(gm[invn_gpio_board_rev][0]); i++) {
    //     if((gm[invn_gpio_board_rev][i].pio_id == id) && (gm[invn_gpio_board_rev][i].pio_mask == index)) {
    //         if(gm[invn_gpio_board_rev][i].callback)
    //             gm[invn_gpio_board_rev][i].callback(gm[invn_gpio_board_rev][i].context, i);
    //         break;
    //     }
    // }
// }

void inv_gpio_sensor_irq_init(unsigned pin_num,
        void (*interrupt_cb)(void * context, unsigned pin_num), void * context)
{
    if(pin_num >= INV_GPIO_MAX)
        return;

    const uvs_gpio_pin int1_pin = INT1_PIN;
    // GPIO_TypeDef* GPIOx = uvs_hal_map_get_port(&int1_pin);

    gm[pin_num].GPIOx = uvs_hal_map_get_port(&int1_pin);
    gm[pin_num].GPIO_Init.Pin = uvs_hal_map_get_pin(&int1_pin);

    gm[pin_num].callback = interrupt_cb;
    gm[pin_num].context = context;

    // void HAL_GPIO_Init(GPIO_TypeDef  *GPIOx, GPIO_InitTypeDef *GPIO_Init);
    HAL_GPIO_Init(gm[pin_num].GPIOx, &gm[pin_num].GPIO_Init);
}

#ifdef __cplusplus
}
#endif
