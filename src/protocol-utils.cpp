#include "protocol-utils.h"

#include <net/file-transfer.h>

bool send_message_type(const net::TcpSocket& socket, MessageType type) {
    const auto byte = static_cast<uint8_t>(type);
    return socket.sendBytes({byte});
}

bool receive_message_type(const net::TcpSocket& socket, MessageType& type) {
    std::vector<uint8_t> byte(1);
    if (!receive_exact(socket, byte.data(), 1))
        return false;
    type = static_cast<MessageType>(byte[0]);
    return true;
}
