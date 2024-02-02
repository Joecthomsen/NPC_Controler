#include "DALI_transmit.h"
#include "driver/gpio.h"
#include "driver/gptimer.h"
#include "esp_log.h"

int incrementer = 0;

bool manchesterClock = 1;
enum State state = START_BIT;
int8_t counter = 0;
uint32_t dataToTransmit = 0;
esp_err_t err;
static const char * TAG = "DALI TRANSMIT";
bool timerOn = false;
uint32_t rx_data_buffer; 

gptimer_handle_t gptimer;   //Init the timer
gptimer_config_t gptimer_config;    //Init timer config struct

//Prototypes
void initGPIO();
void initTimer();
uint32_t manchesterEncode(uint16_t data);
uint8_t percentToDalimapping(uint8_t input);

/**
 * @brief Initialize the DALI transmitter.
 *
 * This function initializes the peripherals required for the DALI transmitter to operate, 
 * including GPIO and the general-purpose timer. It also enables the timer to make it ready for use.
 *  
 * @param void
 * @return void
 * @attention Must be called before any other function in this file.
 */
void init_DALI_transmit(){
    initGPIO();     // Initialize the GPIO configuration
    initTimer();    // Initialize the timer configuration
};

/**
 * @brief Initiate the transmission of a DALI data frame.
 *
 * This function initiates the transmission of a DALI data frame by starting a general-purpose timer,
 * which in turn triggers the transmit_bit_on_timer_alarm() function. Additionally, this function
 * handles the Manchester encoding process and stores the encoded data.
 *
 * @param cmd uint16 data frame to transmit.
 * @return void
 */
void sendDALI_TX(uint16_t cmd){
    
    dataToTransmit = manchesterEncode(cmd);
    if(!timerOn){
        err = gptimer_start(gptimer);
        timerOn = true;
    }

    uint64_t timerVal;
    gptimer_get_raw_count(gptimer, &timerVal);
    for(int i=31; i>=0; i--) {
        int bit = (dataToTransmit >> i) & 1;
        printf("%d", bit); 
    }
    printf("\n");
}

void receiveDALI_RX(){

}

static bool ISR_receive_message(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx){
    
    uint32_t rx_bit = gpio_get_level(GPIO_RX_PIN);
    rx_data_buffer = (rx_data_buffer << 1) | rx_bit; 
    return true;
}

/**
 * @brief Transmit a single bit on timer alarm.
 *
 * This function is specifically designed to transmit a single data bit over GPIO by utilizing
 * timer-based interrupt handling. It adheres to a specific prototype, which mandates the
 * presence of all function arguments, even if they are not utilized within the function.
 * 
 * @param timer The timer handle.
 * @param edata Pointer to the timer alarm event data.
 * @param user_ctx User context (not used in this function).
 * @return true if the operation was successful, false otherwise.
 */
static bool transmit_bit_on_timer_alarm(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    bool returnState = true;
    switch (state)
    {
        case START_BIT:
            if(counter < 1){
                err = gpio_set_level(GPIO_PIN_TX, 1);
                if (err != ESP_OK) {
                    ESP_LOGE(TAG, "Failed to send start bit");
                    returnState = false;
                }
                counter++;
            }
            else{
                err = gpio_set_level(GPIO_PIN_TX, 0);
                manchesterClock = 0;
                if (err != ESP_OK) {
                    ESP_LOGE(TAG, "Failed to send start bit");
                    returnState = false;
                }
                state = DATA;
                counter = 31;   //Set counter to 31, in order to make the counter ready to transmit the 32 bit frame. 
            }
            break;
        case DATA:
            //Here we want to send the data, one bit at each iteratation
            if(counter >= 0){
                uint32_t bitToSend = (dataToTransmit >> counter) & 0x01; // Extract the next bit
                err = gpio_set_level(GPIO_PIN_TX, bitToSend);
                if (err != ESP_OK) {
                    ESP_LOGE(TAG, "Failed to send data bit");
                    returnState = false;
                }
                counter--;
            }
            else{
                err = gpio_set_level(GPIO_PIN_TX, DALI_IDLE_VALUE);
                if (err != ESP_OK) {
                    ESP_LOGE(TAG, "Failed to send first half-stop bit");
                    returnState = false;
                }
                state = STOP_BIT;
                counter = 0;
            }
            break;
        case STOP_BIT:
            if(counter == 0){
                err = gpio_set_level(GPIO_PIN_TX, DALI_IDLE_VALUE);
                counter++;
            }
            else if(counter < 3){
                counter++;
            }
            else{
                err = gptimer_stop(gptimer);
                if (err != ESP_OK) {
                    ESP_LOGE(TAG, "Failed to send stop timer after transmitting frame");
                    returnState = false;
                }
                state = START_BIT;
                timerOn = false;
                incrementer++;                
            }
            break;
    };
    return returnState;    
}


/**
 * @brief Translate a data frame into Manchester-encoded format.
 *
 * This function translates a 16-bit data frame into a 32-bit Manchester-encoded data frame by encoding
 * each bit as either '0b01' or '0b10'.
 *
 * @param data The 16-bit data frame to be translated.
 * @return The resulting Manchester-encoded 32-bit data frame.
 */
uint32_t manchesterEncode(uint16_t data) {

    uint32_t manchesterEncodedData = 0; 

    for (int i = 0; i < 16; i++) {      // Convert DALI value to Manchester-encoded data
        uint8_t bit = (data >> i) & 1;
        if (bit == 1) {
            manchesterEncodedData |= (MANCHESTER_ENCODED_0 << (2 * i));  // '1' is encoded as 01
        } else {
            manchesterEncodedData |= (MANCHESTER_ENCODED_1 << (2 * i));  // '0' is encoded as 10
        }
    }
    return manchesterEncodedData;
}

/**
 * @brief Map a percentage value (1-100) to the DALI brightness range (1-254).
 *
 * This function maps a percentage input to the DALI brightness range.
 *
 * @param input The 8-bit data frame representing the percentage (1-100).
 * @return The resulting 8-bit DALI brightness mapping (1-254).
 * @deprecated this function is deprecated and will be removed/edit in a future version.
 * @attention this function is intended to use with Matter project. 
 */
uint8_t percentToDalimapping(uint8_t input){

    /* 
        Calculate the slope (a) of the linear equation that maps the input range to the output range:
            a = (delta y) / (delta x) = 253 / 100 = 2,53

        Calculate the intercept (b) of the linear equation:
            y has to be 1, when x is 0, therefor: 1 = 2.53x + b  =  b = 1
        Now, for any input value (x) within the range [1, 100], we can calculate the corresponding output value (y) using the linear equation:
            y = a * x + b   =   y = 2.54 * x + 1 
    */
    //TODO Replace this with a lookup table
    if(input > 100){
        input = 100;
    }
    return 2.54 * input + 1;
}

void initGPIO(){

    //Configure the GPIO pin TX
    gpio_config_t io_config_tx = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << GPIO_PIN_TX),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };

    //Configure the GPIO pin RX
    gpio_config_t io_config_rx = {
        .intr_type = GPIO_INTR_ANYEDGE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << GPIO_PIN_TX),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };

    err = gpio_config(&io_config_tx); //Init the GPIO TX
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure GPIO pin");
    }
    else
        ESP_LOGI(TAG, "GPIO pin configured successfully");

    err = gpio_set_level(GPIO_PIN_TX, DALI_IDLE_VALUE);     // Set the GPIO TX pin to 0 (LOW) as the DALI standby requires
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set TX pin to LOW");
    }
    else
        ESP_LOGI(TAG, "TX pin set to LOW successfully");

    err = gpio_config(&io_config_rx); //Init the GPIO
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure GPIO pin");
    }
    else
        ESP_LOGI(TAG, "GPIO pin configured successfully");
    err = gpio_set_level(GPIO_PIN_RX, DALI_IDLE_VALUE);     // Set the GPIO RX pin to 0 (LOW) as the DALI standby requires
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set RX pin to LOW");
    }
    else
        ESP_LOGI(TAG, "RX pin set to LOW successfully");
    

}

void initTimer(){

    //Configure the timer
    gptimer_alarm_config_t gptimer_alarm_config;    //Init alarm config struct
    gptimer_config.clk_src = GPTIMER_CLK_SRC_DEFAULT;   //Set clock source to default   
    gptimer_config.direction = GPTIMER_COUNT_UP;        //Set counting direction to UP   
    gptimer_config.resolution_hz = TIMER_FREQUENZ;      //Set timer frequenz (to 1MHz)                                                                
    gptimer_config.intr_priority = 1;

    err = gptimer_new_timer(&gptimer_config, &gptimer);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure the timer");
    }
    else
        ESP_LOGI(TAG, "Timer configured successfully");

    //Configure the timer alarm
    gptimer_alarm_config.alarm_count = TIMER_FREQUENZ/BAUD_RATE;    //Set the alarm trigger point
    gptimer_alarm_config.flags.auto_reload_on_alarm = true;         //Reload value upon alarm trigger
    gptimer_alarm_config.reload_count = 0;      

    err = gptimer_set_alarm_action(gptimer, &gptimer_alarm_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set alarm action");
    }
    else
        ESP_LOGI(TAG, "Alarm action set successfully");

    //Register event callback for timer
    gptimer_event_callbacks_t cbs = 
    {
        .on_alarm = transmit_bit_on_timer_alarm, // register callback
    };

    err = gptimer_register_event_callbacks(gptimer, &cbs, NULL);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register event callback for timer");
    }
    else
        ESP_LOGI(TAG, "Timer event callback registered successfully");

    //Enable the timer
    err = gptimer_enable(gptimer);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable the timer in DALI init function");
    }
    else
        ESP_LOGI(TAG, "Timer enabled in DALI_transmit_init() function");
}