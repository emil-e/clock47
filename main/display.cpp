#include "display.h"

#include <esp_log.h>

#include <driver/i2c.h>

namespace display {
namespace {

const char *TAG = "display";

// Page 0
constexpr std::uint8_t REG_LED_ONOFF_BASE = 0x00;
constexpr std::uint8_t REG_LED_OPEN_BASE = 0x18;
constexpr std::uint8_t REG_LED_SHORT_BASE = 0x30;

constexpr std::uint8_t REG_COMMAND = 0xFD;
constexpr std::uint8_t REG_COMMAND_WRITE_LOCK = 0xFE;
constexpr std::uint8_t COMMAND_WRITE_ENABLE = 0xC5;

// Page 1
constexpr std::uint8_t REG_PWM_BASE = 0x00;

// Page 3
constexpr std::uint8_t REG_CONFIGURATION = 0x00;
constexpr std::uint8_t CONFIG_NORMAL_OPERATION = 0x01;
constexpr std::uint8_t CONFIG_OSD = 0x04;
constexpr std::uint8_t REG_CURRENT_CONTROL = 0x01;
constexpr std::uint8_t REG_RESET = 0x11;

class Commands {
public:
  Commands(i2c_port_t i2cPort, std::uint8_t address)
      : _i2cPort(i2cPort), _address(address), _cmd(i2c_cmd_link_create()) {}

  void read(std::uint8_t reg, std::uint8_t *dataOut) {
    selectReg(reg);
    i2c_master_start(_cmd);
    i2c_master_write_byte(_cmd, (_address << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(_cmd, dataOut, I2C_MASTER_NACK);
  }

  void write(std::uint8_t reg, std::uint8_t data) {
    selectReg(reg);
    i2c_master_write_byte(_cmd, data, true);
  }

  void write(std::uint8_t reg, std::uint8_t *data, std::size_t size) {
    selectReg(reg);
    i2c_master_write(_cmd, data, size, true);
  }

  void selectPage(std::uint8_t page) {
    write(REG_COMMAND_WRITE_LOCK, COMMAND_WRITE_ENABLE);
    write(REG_COMMAND, page);
  }

  esp_err_t exec() {
    i2c_master_stop(_cmd);
    return i2c_master_cmd_begin(_i2cPort, _cmd, 1000 / portTICK_RATE_MS);
  }

  ~Commands() { i2c_cmd_link_delete(_cmd); }

 private:
  void selectReg(std::uint8_t reg) {
    i2c_master_start(_cmd);
    i2c_master_write_byte(_cmd, (_address << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(_cmd, reg, true);
  }

  i2c_port_t _i2cPort;
  std::uint8_t _address;
  i2c_cmd_handle_t _cmd;
};

class Digit {
 public:
  Digit(i2c_port_t i2cPort, std::uint8_t address, gpio_num_t sdbPin)
      : _i2cPort(i2cPort), _address(address), _sdbPin(sdbPin) {}

  void init() {
    gpio_config_t ioConf;
    ioConf.pin_bit_mask = (1ULL << _sdbPin);
    ioConf.intr_type = GPIO_INTR_DISABLE;
    ioConf.mode = GPIO_MODE_OUTPUT;
    ioConf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    ioConf.pull_up_en = GPIO_PULLUP_DISABLE;
    ESP_ERROR_CHECK(gpio_config(&ioConf));

    gpio_set_level(_sdbPin, 1);

    {
      Commands cmds(_i2cPort, _address);
      cmds.selectPage(3);
      std::uint8_t ignore;
      cmds.read(REG_RESET, &ignore);
      ESP_ERROR_CHECK(cmds.exec());
    }

    {
      Commands cmds(_i2cPort, _address);
      cmds.selectPage(3);
      cmds.write(REG_CONFIGURATION, CONFIG_NORMAL_OPERATION);
      cmds.write(REG_CURRENT_CONTROL, 255);

      cmds.selectPage(0);
      std::uint8_t en[24];
      std::fill(en, en + sizeof(en), 0xFF);
      cmds.write(REG_LED_ONOFF_BASE, en, sizeof(en));

      cmds.selectPage(1);
      std::uint8_t frame[190] = {};
      cmds.write(REG_PWM_BASE, frame, PANE_BUFFER_SIZE);

      ESP_ERROR_CHECK(cmds.exec());
      ESP_LOGI(TAG, "Initialized digit port=%d addr=%x", _i2cPort, _address);
    }
  }

  void renderPane(const Pane &pane) {
    Commands cmds(_i2cPort, _address);
    cmds.write(REG_PWM_BASE, const_cast<std::uint8_t *>(pane.data()), PANE_BUFFER_SIZE);
    ESP_ERROR_CHECK(cmds.exec());
  }

 private:
  i2c_port_t _i2cPort;
  std::uint8_t _address;
  gpio_num_t _sdbPin;
};

Digit g_digits[NUM_PANES] = {
    Digit(I2C_NUM_0, 0x50, GPIO_NUM_5), Digit(I2C_NUM_0, 0x5F, GPIO_NUM_16),
    Digit(I2C_NUM_0, 0x5A, GPIO_NUM_4), Digit(I2C_NUM_1, 0x50, GPIO_NUM_26),
    /*Digit(I2C_NUM_1, 0x5F, GPIO_NUM_32),*/
};

void initI2CPort(int port, int sda, int scl) {
  i2c_config_t config = {.mode = I2C_MODE_MASTER,
                         .sda_io_num = sda,
                         .scl_io_num = scl,
                         .sda_pullup_en = false,
                         .scl_pullup_en = false,
                         .master = {.clk_speed = 400000}};
  ESP_ERROR_CHECK(i2c_param_config(port, &config));
  ESP_ERROR_CHECK(i2c_driver_install(port, I2C_MODE_MASTER, 0, 0, 0));
}

void initI2C(void) {
  initI2CPort(I2C_NUM_0, 21, 19);
  initI2CPort(I2C_NUM_1, 25, 33);
}

}  // namespace

void init() {
  initI2C();

  for (std::size_t i = 0; i < NUM_PANES; i++) {
    g_digits[i].init();
  }
}

void displayPanes(Pane *panes) {
  for (int i = 0; i < NUM_PANES; i++) {
    g_digits[i].renderPane(panes[i]);
  }
}

}  // namespace display
