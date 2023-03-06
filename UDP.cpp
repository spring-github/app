#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class UDP
{
public:
    UDP();
    ~UDP();
    bool init(int port);
    bool send(const std::string &message, const std::string &ip, int port);
    bool recv(std::string &message, std::string &ip, int &port);

private:
    int m_socket;
    struct sockaddr_in m_addr;
};

UDP::UDP() : m_socket(-1)
{
}

UDP::~UDP()
{
    if (m_socket != -1)
    {
        close(m_socket);
    }
}

bool UDP::init(int port)
{
    m_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_socket == -1)
    {
        return false;
    }

    m_addr.sin_family = AF_INET;
    m_addr.sin_port = htons(port);
    m_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(m_socket, (struct sockaddr *)&m_addr, sizeof(m_addr)) == -1)
    {
        return false;
    }

    return true;
}

bool UDP::send(const std::string &message, const std::string &ip, int port)
{
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());

    int ret = sendto(m_socket, message.c_str(), message.length(), 0, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1)
    {
        return false;
    }

    return true;
}

bool UDP::recv(std::string &message, std::string &ip, int &port)
{
    char buf[1024];
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);

    int ret = recvfrom(m_socket, buf, sizeof(buf), 0, (struct sockaddr *)&addr, &len);
    if (ret == -1)
    {
        return false;
    }

    message = std::string(buf, ret);
    ip = inet_ntoa(addr.sin_addr);
    port = ntohs(addr.sin_port);

    return true;
}