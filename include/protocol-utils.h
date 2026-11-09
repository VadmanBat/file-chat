#pragma once
// Created by Vadim on 04.04.2026.

#include "constants/config.hxx"

#include <net/tcp-socket.h>

bool send_message_type(const net::TcpSocket& socket, MessageType type);
bool receive_message_type(const net::TcpSocket& socket, MessageType& type);