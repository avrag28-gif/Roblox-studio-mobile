#pragma once
#include <functional>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <cstddef>

namespace rsm {

template <typename... Args>
class Signal {
public:
    using Callback = std::function<void(Args...)>;
    using ConnectionId = std::size_t;

    ConnectionId Connect(Callback callback) {
        std::lock_guard lock(mutex_);
        const auto id = nextId_++;
        callbacks_.emplace(id, std::move(callback));
        return id;
    }

    void Disconnect(ConnectionId id) {
        std::lock_guard lock(mutex_);
        callbacks_.erase(id);
    }

    void Fire(Args... args) {
        std::vector<Callback> snapshot;
        {
            std::lock_guard lock(mutex_);
            for (const auto& [_, callback] : callbacks_) snapshot.push_back(callback);
        }
        for (auto& callback : snapshot) callback(args...);
    }

private:
    mutable std::mutex mutex_;
    std::unordered_map<ConnectionId, Callback> callbacks_;
    ConnectionId nextId_ = 1;
};

}
