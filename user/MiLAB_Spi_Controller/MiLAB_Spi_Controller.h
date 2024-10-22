#ifndef CHEETAH_SOFTWARE_MINICHEETAHSPI_CONTROLLER_H
#define CHEETAH_SOFTWARE_MINICHEETAHSPI_CONTROLLER_H

#include <RobotController.h>
#include <lcm/lcm-cpp.hpp>
#include "SpiUserParameters.h"
#include <thread>
#include <mutex>

class MiLAB_Spi_Controller : public RobotController {
public:
  MiLAB_Spi_Controller(): RobotController(), _lcm(getLcmUrl(255)) {

    _lcm.subscribe("spi_debug_cmd", &MiLAB_Spi_Controller::handleLcm, this);
    _lcmThread = std::thread([&](){
      for(;;) _lcm.handle();
    });
  }
  virtual ~MiLAB_Spi_Controller(){}

  virtual void initializeController(){}
  virtual void runController();
  virtual void updateVisualization(){}
  virtual ControlParameters* getUserControlParameters() {
    return nullptr;
  }

  void handleLcm(const lcm::ReceiveBuffer* rbuf, const std::string& chan,
    const leg_control_command_lcmt* msg) {
    (void)rbuf;
    (void)chan;
    _mutex.lock();
    command = *msg;
    _mutex.unlock();
  }

private:
  lcm::LCM _lcm;
  leg_control_command_lcmt command;
  std::thread _lcmThread;
  std::mutex _mutex;
};

#endif //CHEETAH_SOFTWARE_MINICHEETAHSPI_CONTROLLER_H
