#include <boost/interprocess/ipc/message_queue.hpp>
#include <iostream>
#include <vector>
#include "structure.h"

using namespace boost::interprocess;

int main() {
  try {
    // Create a message_queue.
    message_queue mq(open_only  // only create
                     ,"message_queue"  // name
                     );

    // Send 100 numbers
    for (int i = 0; i < 100; ++i) {
      Data test;
      unsigned priority;
      message_queue::size_type size;
      mq.receive(&test, sizeof(test), size, priority);
      std::cout << test.buf << ", " << test.byte << ", " << test.foobar << ", "
                << test.asasd << "\n";
    }
  } catch (interprocess_exception &ex) {
    std::cout << ex.what() << std::endl;
    return 1;
  }

  // Erase previous message queue
  message_queue::remove("message_queue");

  return 0;
}
