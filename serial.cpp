
#include <termios.h> #include <fcntl.h> #include <unistd.h> #include <string.h>

class SerialConfig
{
public:
    SerialConfig(const char *portName, int baudRate);
    ~SerialConfig();
    int openPort();
    void closePort();
    int setBaudRate(int baudRate);
    int setDataBits(int dataBits);
    int setStopBits(int stopBits);
    int setParity(int parity);
    int setFlowControl(int flowControl);
    int setTimeout(int timeout);

private:
    int fd;
    struct termios options;
    const char *portName;
    int baudRate;
};

SerialConfig::SerialConfig(const char *portName, int baudRate)
{
    this->portName = portName;
    this->baudRate = baudRate;
    fd = -1;
}

SerialConfig::~SerialConfig() { closePort(); }

int SerialConfig::openPort()
{
    fd = open(portName, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1)
    {
        return -1;
    }
    tcgetattr(fd, &options);
    cfsetispeed(&options, baudRate);
    cfsetospeed(&options, baudRate);
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag &= ~OPOST;
    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = 0;
    tcsetattr(fd, TCSANOW, &options);
    return 0;
}

void SerialConfig::closePort()
{
    if (fd != -1)
    {
        close(fd);
        fd = -1;
    }
}

int SerialConfig::setBaudRate(int baudRate)
{
    if (fd == -1)
    {
        return -1;
    }
    this->baudRate = baudRate;
    cfsetispeed(&options, baudRate);
    cfsetospeed(&options, baudRate);
    tcsetattr(fd, TCSANOW, &options);
    return 0;
}

int SerialConfig::setDataBits(int dataBits)
{
    if (fd == -1)
    {
        return -1;
    }
    options.c_cflag &= ~CSIZE;
    switch (dataBits)
    {
    case 5:
        options.c_cflag |= CS5;
        break;
    case 6:
        options.c_cflag |= CS6;
        break;
    case 7:
        options.c_cflag |= CS7;
        break;
    case 8:
        options.c_cflag |= CS8;
        break;
    default:
        return -1;
    }
    tcsetattr(fd, TCSANOW, &options);
    return 0;
}

int SerialConfig::setStopBits(int stopBits)
{
    if (fd == -1)
    {
        return -1;
    }
    switch (stopBits)
    {
    case 1:
        options.c_cflag &= ~CSTOPB;
        break;
    case 2:
        options.c_cflag |= CSTOPB;
        break;
    default:
        return -1;
    }
    tcsetattr(fd, TCSANOW, &options);
    return 0;
}

int SerialConfig::setParity(int parity)
{
    if (fd == -1)
    {
        return -1;
    }
    switch (parity)
    {
    case 0:
        options.c_cflag &= ~PARENB;
        break;
    case 1:
        options.c_cflag |= PARENB;
        options.c_cflag &= ~PARODD;
        break;
    case 2:
        options.c_cflag |= PARENB;
        options.c_cflag |= PARODD;
        break;
    default:
        return -1;
    }
    tcsetattr(fd, TCSANOW, &options);
    return 0;
}

int SerialConfig::setFlowControl(int flowControl)
{
    if (fd == -1)
    {
        return -1;
    }
    switch (flowControl)
    {
    case 0:
        options.c_cflag &= ~CRTSCTS;
        break;
    case 1:
        options.c_cflag |= CRTSCTS;
        break;
    default:
        return -1;
    }
    tcsetattr(fd, TCSANOW, &options);
    return 0;
}

int SerialConfig::setTimeout(int timeout)
{
    if (fd == -1)
    {
        return -1;
    }
    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = timeout;
    tcsetattr(fd, TCSANOW, &options);
    return 0;
}