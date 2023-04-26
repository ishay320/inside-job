#include "broker.h"

class node
{
public:
    node();
    ~node();
    void publish(void* data, size_t len);
    void subscribe(const std::string& topic);
    void subscribe(const std::string& topic);

private:
    /* data */
};
