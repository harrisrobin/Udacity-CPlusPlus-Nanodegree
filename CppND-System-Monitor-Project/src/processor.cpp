#include "processor.h"
#include "linux_parser.h"

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() {
    long total = LinuxParser::Jiffies();
    long idle = LinuxParser::IdleJiffies();

    long totalDiff = total - this->preTotal;
    long idleDiff = total - this->preIdle;

    this->preTotal = total;
    this->preIdle = idle;

    return float(totalDiff - idleDiff) / idleDiff;
}