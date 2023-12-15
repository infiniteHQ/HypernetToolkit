#include "../../../../vortex.h"

namespace Vortex {
namespace hstd {
namespace Notifications {

class SampleNotification : public VortexNotificationHandler {
public:
  SampleNotification() {
    this->notificationLabel =
        "::HSTD::SampleNotification"; // ::SampleNotification is the unique Tag
                                      // of this notification
  };

  void
  OnNotificationReceived(VortexNotification* notification) override {
    std::cout << "=================================================="
              << std::endl;
    std::cout << "::HSTD::SampleNotification handler activated" << std::endl;
    std::cout << "Notification:" << std::endl;
    std::cout << "With type of notif = " << notification->type.c_str() << std::endl;


    std::cout << "Body = " << notification->body.c_str() << std::endl;
    std::cout << "=================================================="
              << std::endl;
  };

  static VortexNotificationHandler *CreateHandler() {
    SampleNotification *s = new SampleNotification;
    return s;
  }
};
MOD_NOTIFICATION_HANDLER(HSTD, SampleNotification)



} // namespace Notifications
} // namespace hstd
} // namespace Vortex