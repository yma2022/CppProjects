#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

 private:
  long prev_idle_{0};
  long prev_total_{0};
  long prev_user_{0};
  long prev_nice_{0};
  long prev_system_{0};
  long prev_irq_{0};
  long prev_softirq_{0};
  long prev_steal_{0};
  long prev_guest_{0};
  long prev_guest_nice_{0};
};

#endif