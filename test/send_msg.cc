#include <boost/interprocess/ipc/message_queue.hpp>
#include <iostream>
#include <vector>
#include "structure.h"

using namespace boost::interprocess;

int main() {
  try {
    // Erase previous message queue
    message_queue::remove("message_queue");

    // Create a message_queue.
    message_queue mq(create_only  // only create
                     ,"message_queue"  // name
                     ,100  // max message number
                     ,sizeof(Data)  // max message size
                     );
    // Send 100 numbers
    Data test = {"Hello!", 'm', 0, 1.5};
    for (int i = 0; i < 100; ++i) {
      test.foobar = i;
      mq.send(&test, sizeof(test), 0);
    }
  } catch (interprocess_exception &ex) {
    std::cout << ex.what() << std::endl;
    return 1;
  }

  return 0;
}
