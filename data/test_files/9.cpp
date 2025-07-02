// --- SettingsManager_Fixed.cpp ---
// A singleton manager for global application settings.
// This version uses std::optional to safely handle cases where a setting
// might not exist, preventing dangling references.

#include <string>
#include <vector>
#include <algorithm>
#include <optional> // Required for std::optional

struct Setting {
    std::string key;
    int value;
};

class SettingsManager {
public:
    static SettingsManager& getInstance() {
        static SettingsManager instance;
        return instance;
    }

    void load() {
        m_settings.push_back({"difficulty", 5});
        m_settings.push_back({"volume", 75});
    }

    // Returns an optional containing the value if found, or an empty optional otherwise.
    std::optional<int> getSettingValue(const std::string& key) const {
        auto it = std::find_if(m_settings.begin(), m_settings.end(), 
                               [&](const Setting& s) { return s.key == key; });

        if (it != m_settings.end()) {
            return it->value;
        }

        // If not found, return an empty optional.
        return std::nullopt;
    }

private:
    SettingsManager() = default;
    ~SettingsManager() = default;
    SettingsManager(const SettingsManager&) = delete;
    void operator=(const SettingsManager&) = delete;

    std::vector<Setting> m_settings;
};

void applySettingsSafely() {
    SettingsManager::getInstance().load();

    // The caller must check if the optional has a value.
    if (auto volumeOpt = SettingsManager::getInstance().getSettingValue("volume")) {
        // .value() accesses the contained value.
        int volume = volumeOpt.value();
        // Use volume...
    }

    // Handling the non-existent key case safely.
    auto vsyncOpt = SettingsManager::getInstance().getSettingValue("vsync");
    // .value_or() provides a default value if the optional is empty.
    int vsync = vsyncOpt.value_or(0); // Safely gets 0 if "vsync" is not found.
    // This is safe. `vsync` is a local variable with a well-defined value.
}