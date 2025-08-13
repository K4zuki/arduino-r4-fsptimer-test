#include "Arduino.h"
#include "FspTimer.h"

#define _USE_GPT4 (4)
#define _PWM_FREQ (4.0e3)
#define _PWM_DUTY (50.5f)

FspTimer gpt4;
gpt_extended_cfg_t* gpt4_ext_cfg;
gpt_extended_pwm_cfg_t* gpt4_ext_pwm_cfg;

char buffer_string[50] = { 0 };
uint8_t i = 0;


void setup() {
  Serial.begin(9600);

  gpt4.begin(TIMER_MODE_PWM, GPT_TIMER, _USE_GPT4, _PWM_FREQ, _PWM_DUTY);  // Initialise GPT4 timer with saw-tooth PWM
  // gpt4.begin(TIMER_MODE_TRIANGLE_WAVE_SYMMETRIC_PWM, GPT_TIMER, _USE_GPT4, _PWM_FREQ, _PWM_DUTY); // Initialise GPT4 timer with triangle PWM mode 1

  gpt4.add_pwm_extended_cfg();  // extend PWM specific config

  gpt4.enable_pwm_channel(CHANNEL_A);  // enable both phase
  gpt4.enable_pwm_channel(CHANNEL_B);

  /** IO pin config: D0=B phase; D1=A phase */
  // assign to GPT4 Channel B (GTIOC4B)
  R_IOPORT_PinCfg(&g_ioport_ctrl, g_pin_cfg[D0].pin, (uint32_t)(IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_GPT1));
  // assign to GPT4 Channel A (GTIOC4A)
  R_IOPORT_PinCfg(&g_ioport_ctrl, g_pin_cfg[D1].pin, (uint32_t)(IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_GPT1));

  /** prepare variables to update config */
  timer_cfg_t* gpt4_cfg = gpt4.get_cfg();
  gpt4_ext_cfg = (gpt_extended_cfg_t*)gpt4_cfg->p_extend;
  gpt4_ext_pwm_cfg = (gpt_extended_pwm_cfg_t*)gpt4_ext_cfg->p_pwm_cfg;

  gpt4_ext_cfg->gtioca.output_enabled = true;  // not referenced when gtior_setting.gtior is not 0
  gpt4_ext_cfg->gtiocb.output_enabled = true;  // not referenced when gtior_setting.gtior is not 0

  gpt4_ext_cfg->gtior_setting.gtior_b.gtioa = 0x06;  // 0_01_10: initial low; low at cycle end; high at compare match
  gpt4_ext_cfg->gtior_setting.gtior_b.gtiob = 0x19;  // 1_10_01: initial high; high at cycle end; low at compare match
  gpt4_ext_cfg->gtior_setting.gtior_b.oae = 1;       // A phase output enable
  gpt4_ext_cfg->gtior_setting.gtior_b.obe = 1;       // B phase output enable

  gpt4_ext_pwm_cfg->dead_time_count_up = 120;  // dead time 120 clock cycles; invalid with TIMER_MODE_PWM

  gpt4.open();   // internal function will not initialize GTIOR register
  gpt4.start();  // timer count start
}

void loop() {
  // Serial.println("Hello, World!");
  // Serial.println(gpt4_ext_cfg->gtioca.stop_level);
  // Serial.println(gpt4_ext_cfg->gtiocb.stop_level);

  // sprintf(buffer_string, "0x%02X", gpt4_ext_cfg->gtior_setting.gtior_b.gtioa);
  // Serial.println(buffer_string);
  // sprintf(buffer_string, "0x%02X", gpt4_ext_cfg->gtior_setting.gtior_b.gtiob);
  // Serial.println(buffer_string);

  // sprintf(buffer_string, "0x%02X", R_GPT4->GTIOR_b.GTIOA);
  // Serial.println(buffer_string);
  // sprintf(buffer_string, "0x%02X", R_GPT4->GTIOR_b.GTIOB);
  // Serial.println(buffer_string);

  gpt4.set_duty_cycle(100 + i * 45, CHANNEL_A);
  gpt4.set_duty_cycle(100 + i * 45, CHANNEL_B);
  i += 30;

  delay(10);
}
