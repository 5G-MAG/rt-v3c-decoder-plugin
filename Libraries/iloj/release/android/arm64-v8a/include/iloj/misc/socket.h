/*
* Copyright (c) 2025 InterDigital CE Patent Holdings SASU
* Licensed under the License terms of 5GMAG software (the "License").
* You may not use this file except in compliance with the License.
* You may obtain a copy of the License at https://www.5g-mag.com/license .
* Unless required by applicable law or agreed to in writing, software distributed under the License is
* distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and limitations under the License.
*/

#pragma once

#if defined _WIN64
constexpr int MSG_WAITALL = 0x8;
constexpr int SD_BOTH = 0x02;
#include <windows.h>
constexpr int MSG_NOSIGNAL = 0;
#ifdef DELETE
#undef DELETE
#endif
#elif defined __linux__ || defined __ANDROID__
#include <netinet/in.h>
#elif defined __APPLE__
#include <netinet/in.h>
#ifdef MSG_NOSIGNAL
#undef MSG_NOSIGNAL
#endif
constexpr int MSG_NOSIGNAL = 0;
#endif

#include "string.h" //NOLINT
#include "thread.h"
#include <functional>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <utility>

namespace iloj::misc
{
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Socket
{
public:
#if defined _WIN64
    using address_type = SOCKADDR_IN;
#elif defined __linux__ || defined __APPLE__ || defined __ANDROID__
    using address_type = sockaddr_in;
#endif
protected:
    int m_socketId = -1;
    address_type m_local{}, m_peer{};

public:
    Socket();
    virtual ~Socket() = default;
    Socket(const Socket &) = delete;
    Socket(Socket &&other) noexcept;
    auto operator=(const Socket &) -> Socket & = delete;
    auto operator=(Socket &&other) noexcept -> Socket &;
    [[nodiscard]] auto isValid() const -> bool { return (-1 < m_socketId); }
    [[nodiscard]] auto getPeerName() const -> std::string;
    static auto error() -> int;

protected:
    void setLocalAddress(const std::string &ip, int port);
    void setPeerAddress(const std::string &ip, int port);
};

namespace TCP
{
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Client: public Socket
{
public:
    enum class Status
    {
        None,
        Error,
        Connected
    };

private:
    Status m_status{Status::None};

public:
    Client(const std::string &ip_peer, int port_peer);
    ~Client() override;
    Client(const Client &) = delete;
    Client(Client &&) = default;
    auto operator=(const Client &) -> Client & = delete;
    auto operator=(Client &&) -> Client & = default;
    auto getStatus() -> Status { return m_status; }
    auto receive(void *buffer, std::size_t sz, int timeout = -1, int flags = MSG_WAITALL) -> int;
    auto send(const void *buffer, std::size_t sz, int flags = MSG_NOSIGNAL) -> int;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Server: public Socket, public Service
{
public:
    enum class Status
    {
        None,
        Error,
        Connected
    };

    class Agent: public Socket
    {
        friend class Server;

    public:
        explicit Agent(int id);
        ~Agent() override;
        Agent(const Agent &) = delete;
        Agent(Agent &&) = default;
        auto operator=(const Agent &) -> Agent & = delete;
        auto operator=(Agent &&) -> Agent & = default;
        auto receive(void *buffer, std::size_t sz, int timeout = -1, int flags = MSG_WAITALL) -> int;
        auto send(const void *buffer, std::size_t sz, int flags = MSG_NOSIGNAL) -> int;

    protected:
        [[nodiscard]] virtual auto isAlive() const -> bool = 0;
        virtual void enable() = 0;
        virtual void disable() = 0;
    };

    using AgentAllocator = std::function<std::unique_ptr<Agent>(int)>;
    using ConnectionRegulator = std::function<bool(const address_type &)>;

protected:
    std::list<std::unique_ptr<Agent>> m_agentList;
    AgentAllocator m_agentAllocator = [](int /*unused*/) { return nullptr; };
    ConnectionRegulator m_connectionRegulator = [](const address_type & /*unused*/) { return true; };
    Status m_status{Status::None};

public:
    Server(const std::string &ip_local, int port_local, int max_connections);
    ~Server() override = default;
    Server(const Server &) = delete;
    Server(Server &&) = delete;
    auto operator=(const Server &) -> Server & = delete;
    auto operator=(Server &&) -> Server & = delete;
    auto getStatus() -> Status { return m_status; }
    void setAgentAllocator(AgentAllocator agentAllocator) { m_agentAllocator = std::move(agentAllocator); }
    void setConnectionRegulator(ConnectionRegulator connnectionRegulator)
    {
        m_connectionRegulator = std::move(connnectionRegulator);
    }
    template<typename AGENT, typename... Args>
    static auto create(const std::string &ip_local, int port_local, int max_connections, Args &&... args)
        -> std::unique_ptr<Server>
    {
        auto server = std::make_unique<Server>(ip_local, port_local, max_connections);
        server->setAgentAllocator([&](int id) { return std::make_unique<AGENT>(id, std::forward<Args>(args)...); });
        return server;
    }

protected:
    void onStop() override;
    void idle() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Agent
{
class HTTP: public Server::Agent, public Service
{
public:
    class Request
    {
    public:
        enum class Command
        {
            NONE,
            GET,
            HEAD,
            POST,
            PUT,
            DELETE
        };
        static std::map<Command, std::string> CommandMap;
        static std::map<unsigned, std::string> CodeMap;

    private:
        Command m_command = Command::NONE;
        std::string m_url;
        std::string m_protocol;
        mutable std::map<std::string, std::string> m_header;
        std::string m_body;

    public:
        void clear();
        auto parse(std::stringstream &iss) -> bool;
        auto getCommand() const -> const Command & { return m_command; }
        auto getURL() const -> const std::string & { return m_url; }
        auto getFullURL() const -> std::string { return (m_url.back() == '/') ? (m_url + "index.html") : m_url; }
        auto getProtocol() const -> const std::string & { return m_protocol; }
        auto hasHeader(const std::string &h) const -> bool { return (m_header.find(h) != m_header.end()); }
        auto getHeaderValue(const std::string &h) const -> const std::string & { return m_header[h]; }
        auto getBody() const -> const std::string & { return m_body; }
        auto toString() const -> std::string;
        auto hasContent() const -> bool { return hasHeader("content-type"); }
        auto getContentSize() const -> std::size_t { return str2any<std::size_t>(getHeaderValue("content-length")); }
        auto isJSON() const -> bool
        {
            return (hasHeader("content-type") &&
                    (getHeaderValue("content-type").find("application/json") != std::string::npos));
        }

    private:
        static auto buildCommandMap() -> std::map<TCP::Agent::HTTP::Request::Command, std::string>;
        static auto buildCodeMap() -> std::map<unsigned, std::string>;
    };

protected:
    std::string m_rootDirectory;
    std::string m_buffer;
    std::stringstream m_stream;
    Request m_request;

public:
    HTTP(int id, std::string rootDirectory, std::size_t bufferSize = 4096)
        : Agent{id}, m_rootDirectory{std::move(rootDirectory)}, m_buffer(bufferSize, '0')
    {
    }
    ~HTTP() override = default;
    HTTP(const HTTP &) = delete;
    HTTP(HTTP &&) = delete;
    auto operator=(const HTTP &) -> HTTP & = delete;
    auto operator=(HTTP &&) -> HTTP & = delete;

protected:
    auto isAlive() const -> bool override { return (state() != StateId::Finished); }
    void enable() override { Service::start(); }
    void disable() override { Service::stop(); }

protected:
    void idle() override;

protected:
    virtual void onGetRequest();
    virtual void onPostRequest() {}
    void sendAcknowledgment();
    void sendError();
    auto sendFile() -> bool;
    void sendNotFound();
    void sendContent(const std::string &type, const std::string &content);

private:
    auto receiveRequest() -> bool;
    void processRequest();
};

} // namespace Agent

} // namespace TCP

namespace UDP
{
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Agent: public Socket
{
public:
    Agent(const std::string &ip_local, int port_local, const std::string &ip_peer, int port_peer);
    ~Agent() override;
    Agent(const Agent &) = delete;
    Agent(Agent &&) = default;
    auto operator=(const Agent &) -> Agent & = delete;
    auto operator=(Agent &&) -> Agent & = default;
    auto receiveFrom(void *buffer, std::size_t sz, int timeout = -1, int flags = MSG_WAITALL) -> int;
    auto sendTo(const void *buffer, std::size_t sz, int flags = MSG_NOSIGNAL) -> int;
};

} // namespace UDP

} // namespace iloj::misc
