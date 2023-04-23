#include "node.h"

class Job
{
   public:
    Job(/* args */);
    ~Job();

    void tx();
    void rx();

    static void run_tx();
    static void run_rx();

   private:
};
